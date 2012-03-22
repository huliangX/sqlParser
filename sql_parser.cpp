#include "sql_parser.h"
#include "sqlGrammar.hpp"


#include <fstream>
#include <map>
#include <iostream>
#include <sstream>
#include <cwctype>
#include <boost/lexical_cast.hpp>




 string& trim(string& s) {
   if (s.empty()) {
     return s;
   }
   
   string::iterator c;
   // Erase whitespace before the string
   for (c = s.begin(); c != s.end() && iswspace(*c++););
     s.erase(s.begin(), --c);
 
   // Erase whitespace after the string
   for (c = s.end(); c != s.begin() && iswspace(*--c););
     s.erase(++c, s.end());
 
   return s;
 }

 ParseNode::ParseNode( TreeIterT const& i ,node_t _Type,int _index )
	 	 :ast_node_ptr(i),NodeType(_Type),placeholder_index(_index)
 {
	 TypeNum_t valueType;
	 switch (_Type)
	 {
	 case BOOL:
		 {
			 valueType = ANY_BOOL;
			 break;
		 }
	 case INT:
		 {
			 valueType = ANY_INT;
			 break;
		 }
	 case LONG:
		 {
			 valueType = ANY_LONG_64;
			 break;
		 }
	 case DOUBLE:
		 {
			 valueType = ANY_DOUBLE;
			 break;
		 }
	 case OP:
	 case STRING:
	 case PLACEHOLDER:
	 default:
		 {
			 valueType = ANY_STRING;
			 break;
		 }
	 }
	 m_valuePtr = new anyType( valueType ,string(i->value.begin(), i->value.end()) );
	
 }

 ParseNode::ParseNode( TreeIterT const& i ,node_t _Type,int _index ,ParseNodeBase* _left,ParseNodeBase* _right)
	 	 :ast_node_ptr(i),NodeType(_Type),placeholder_index(_index),left(_left),right(_right)
 {
	 TypeNum_t valueType;
	 switch (_Type)
	 {
	 case BOOL:
		 {
			 valueType = ANY_BOOL;
			 break;
		 }
	 case INT:
		 {
			 valueType = ANY_INT;
			 break;
		 }
	 case LONG:
		 {
			 valueType = ANY_LONG_64;
			 break;
		 }
	 case DOUBLE:
		 {
			 valueType = ANY_DOUBLE;
			 break;
		 }
	 case OP:
	 case STRING:
	 case PLACEHOLDER:
	 default:
		 {
			 valueType = ANY_STRING;
			 break;
		 }
	 }
	 m_valuePtr = new anyType( valueType ,string(i->value.begin(), i->value.end()) );
 }


 node_t ParseNode::GetNodeType()
 {
	return NodeType;
 }

 ParseFunc::ParseFunc( TreeIterT const& i ,string _funcname, vector<anyType*> _paramlist)
	 :ast_node_ptr(i),funcname(_funcname),paramlist(_paramlist)
 {

	 NodeType = FUNC;
 }


 node_t ParseFunc::GetNodeType()
 {
	return NodeType;
 }

sql_parser::sql_parser(void)
{
}

sql_parser::~sql_parser(void)
{
}

void sql_parser::ScanTree(TreeIterT & i)
{
    switch(i->value.id().to_long())
    {
		case select_clause_id:
		{
			commandtype = select_t;
			int childnum = i->children.size() ;
			for ( int n =0;n<childnum;n++)
				{
				ScanTree(i->children.begin()+n);			
				} 
			break;
		}
		case fields_list_id:
		{
			int childnum = i->children.size();
			for(int index = 0; index<childnum;index++)
			{
				m_fields.push_back(string( 	i->children[index].value.begin(), i->children[index].value.end() ));
			}
			break;

		}
		case order_pair_id:
		{
			struct order current;
			string dir = string(i->value.begin(),i->value.end());
			if(dir == "desc" ||dir =="DESC" )
				current.direction = DESC;
			else
				current.direction = ASC;		
			current.varname = string( i->children[0].value.begin(), i->children[0].value.end() );
			m_orders.push_back(current);
			break;

			}
		case delete_clause_id:
		{
			commandtype = delete_t;
			ScanTree(i->children.begin());
			break;
		}
		case table_name_id:
		{
			//没有联表查询
			m_tablename = string(i->value.begin(), i->value.end());
			int childnum = i->children.size() ;
			for(int index = 0; index<childnum;index++)
			{
				m_fields.push_back(string( 	i->children[index].value.begin(), i->children[index].value.end() ));
			}
			break;
		}
		case insert_clause_id:
		{
			commandtype = insert_t;
			ScanTree(i->children.begin());
			ScanTree(i->children.begin()+1);
			break;
		} 
		case insert_values_list_id:
		{
			int childnum = i->children.size() ;
			for(int index = 0; index<childnum;index++)
			{
				m_fields_map.insert( make_pair(m_fields[index], string( i->children[index].value.begin(), i->children[index].value.end() ) )  );
			}
			
			break;
		}
		case update_clause_id:
		{
			commandtype = update_t;
			ScanTree(i->children.begin());
			ScanTree(i->children.begin()+1);	
			break;			
		}
		case update_field_id:
		{
			int childnum = i->children.size() ;
			
			for(int index = 0; index<childnum;index++)
			{
				m_fields_map.insert( make_pair(
					string(  i->children.begin()->value.begin(),     i->children.begin()->value.end()    ),  
					string( (i->children.begin()+1)->value.begin(), (i->children.begin()+1)->value.end() )  
					)  );
			}								
			break;	
		}
    default:
		{
			int childnum = i->children.size() ;
			for ( int n =0;n<childnum;n++)
				{
				ScanTree(i->children.begin()+n);			
				} 
			break;
		}

	}
}


bool sql_parser::ParseSQLString()
{
		string::size_type nFind = metaString.find("where");
		if(nFind==string::npos)
	    nFind = metaString.find("WHERE");
		string beforeWhere;
		string afterWhere;

		if (nFind != string::npos) {
		//还需要做一些 字串规范性处理
			beforeWhere = metaString.substr(0,nFind-1) ; 
			afterWhere = metaString.substr(nFind +5,metaString.length() ) ;  //exclude "where" ";" 
			string::size_type nFind = afterWhere.rfind(';');
			if (nFind != string::npos) afterWhere = afterWhere.substr(0,nFind) ;
			
			trim(beforeWhere);
			trim(afterWhere);	
		  
		}
		else
		{
			beforeWhere = metaString;
			afterWhere = "";
		}	

	sql_grammar command_grammar;

#ifdef STX_DEBUG_PARSER
    BOOST_SPIRIT_DEBUG_GRAMMAR(command_grammar);
#endif

	const string temp = beforeWhere;
	tree_parse_info<InputIterT> info = ast_parse(temp.begin(), temp.end(), command_grammar, space_p);

    if (!info.full)
    {
		std::ostringstream oss;
		oss << "Syntax error at position "
			<< static_cast<int>(info.stop - temp.begin())
			<< " near " 
			<< string(info.stop, temp.end());

		cout << oss.str();
	//	throw(stx::BadSyntaxException(oss.str()));
		return false;
    }

	else
    {
		ScanTree(info.trees.begin());
    }


	// condition tree
	if(afterWhere!="")
	{



			ExpressionGrammar condition_grammar;

	#ifdef STX_DEBUG_PARSER
		BOOST_SPIRIT_DEBUG_GRAMMAR(command_grammar);
	#endif

		const string temp = afterWhere;
		m_ConditionTreeInfo = ast_parse(temp.begin(), temp.end(), condition_grammar, space_p);

		if (!info.full)
		{
			std::ostringstream oss;
			oss << "Syntax error at position "
				<< static_cast<int>(info.stop - temp.begin())
				<< " near " 
				<< string(info.stop, temp.end());

			cout << oss.str();
		//	throw(stx::BadSyntaxException(oss.str()));
			return false;
		}

		else
		{
			int index=0;
			BuildTreeStackByAST( m_ConditionTreeInfo.trees.begin(),index );
		}

		DumpTreeStack();
	}

	return true;

}

ParseNodeBase* sql_parser::BuildTreeStackByAST( TreeIterT const& i ,int &index)
{
	ParseNodeBase* tempNode;
  switch(i->value.id().to_long())
    {
		case boolean_const_id:
		{
			tempNode = new ParseNode( i, BOOL, -1 );
			m_treestack.push_back(tempNode);  
			return tempNode;

		 }
		case integer_const_id:
		{
			tempNode = new ParseNode( i, INT, -1 );
			m_treestack.push_back(tempNode);
			return tempNode;

		 }
		case long_const_id:
		{
			tempNode = new ParseNode( i, LONG, -1 );
			m_treestack.push_back(tempNode);
			return tempNode;

		 }
	    case double_const_id:
		{
			tempNode = new ParseNode( i, DOUBLE, -1 );
			m_treestack.push_back(tempNode);
			return tempNode;

		 }
		case string_const_id:
		{
			tempNode = new ParseNode( i, STRING, -1 );
			m_treestack.push_back(tempNode);
			return tempNode;

		 }
		case varname_id:
		{
			tempNode = new ParseNode( i, VARNAME, -1 );
			m_treestack.push_back(tempNode);
			return tempNode;

		 }
		case placeholder_const_id:
		{
			index++;
			tempNode = new ParseNode( i, PLACEHOLDER, index );
			m_treestack.push_back(tempNode);
			return tempNode;
		} 
		case comp_expr_id:
		case and_expr_id:
		case or_expr_id:
			{

				ParseNodeBase* left =  BuildTreeStackByAST(i->children.begin(),index);
				ParseNodeBase* right = BuildTreeStackByAST(i->children.begin()+1,index);
				tempNode = new ParseNode( i, OP, -1,left,right );

				m_treestack.push_back(tempNode);
				return tempNode;

			}
    case function_identifier_id:
    {
	std::string funcname(i->value.begin(), i->value.end());

	std::vector<anyType*> paramlist;


	if (i->children.size() > 0)
	{
	    TreeIterT const& paramlistchild = i->children.begin();

	    if (paramlistchild->value.id().to_long() == exprlist_id)
	    {
			try
			{
				for(TreeIterT ci = paramlistchild->children.begin(); ci != paramlistchild->children.end(); ++ci)
				{
					switch ( ci->value.id().to_long() )
					{
					case boolean_const_id:
						  {
							  // why we recognized it as a boolean? beause it looks lick  true | false ?  
							  string temp = string(ci->value.begin(), ci->value.end());
							  if(temp == "true"|| temp == "TRUE")
							  paramlist.push_back( new anyType( ANY_BOOL, true ));

							  else if(temp == "false"|| temp == "FALSE")
							  paramlist.push_back( new anyType( ANY_BOOL, false));
							  else
							  {
								  //throw 
							  }
							  continue;
						  }
					case integer_const_id:
						  {  
							  paramlist.push_back(  new
										  anyType( ANY_INT, 
													boost::lexical_cast<int>( string(ci->value.begin(), ci->value.end()) ) 
												 )
												);
							  continue;

						  }
					case long_const_id:
						  {  
							  paramlist.push_back(  new
											  anyType( ANY_LONG_64, 
													   boost::lexical_cast<long long>( string(ci->value.begin(), ci->value.end()) ) 
												 )
												);
							  continue;

						  }
					case double_const_id:
						  {  
							  paramlist.push_back(  new
										  anyType( ANY_DOUBLE, 
												   boost::lexical_cast<double>( string(ci->value.begin(), ci->value.end()) ) 
												 )
												);
							  continue;

						  }
					case string_const_id:
					case varname_id:
						  {
							  paramlist.push_back( new anyType( ANY_STRING,string(ci->value.begin(), ci->value.end()) ));
							  continue;
						  }
					default:
						{
							  paramlist.push_back( new anyType( ANY_STRING,string(ci->value.begin(), ci->value.end()) ));
							  continue;
						}
					}
				}
			}
			catch (...) // need to clean-up
			{
				throw;
			}
		}
	    else
	    {
		// just one subnode and its not a full expression list
		 paramlist.push_back( new anyType( ANY_STRING,string(paramlistchild->value.begin(), paramlistchild->value.end()) ));
	    }


		}//if (i->children.size() > 0)

		tempNode = new ParseFunc(i,funcname,paramlist);
		m_treestack.push_back(tempNode);
        return tempNode;

	}//case func

    default:
		{
		cout<<"Unknown AST parse tree node found. This should never happen."<<endl;
		//	throw(ExpressionParserException("Unknown AST parse tree node found. This should never happen."));
		return tempNode =0;
		}
	}
}


void sql_parser::DumpTreeStack()
{
	cout<<"DumpTreeStack:"<<endl;
	while( !m_treestack.empty())
	{
		ParseNodeBase* curNode =  m_treestack.front();
		if( curNode->GetNodeType() == FUNC)
		{
			cout<<"ParseFunc: "<< ((ParseFunc*)curNode)->funcname<<endl;
		}
		else
		{
			cout<<"ParseNode: "<< *(((ParseNode*)curNode)->m_valuePtr) <<"Type: "<<((ParseNode*)curNode)->NodeType<<endl;
		}
		m_treestack.pop_front();
	}

}


void sql_parser::SetSQLString(const string &s)
{
	metaString = s; 
}



 anyType ParseNode::evaluate(CObject *obj) const
{
	//{  BOOL=0,INT ,LONG,DOUBLE,STRING,VARNAME ,OP,FUNC,PLACEHOLDER }
	switch( NodeType )	
	{
	case BOOL:
	case INT:
	case LONG:
	case DOUBLE:
	case STRING:
		{
			return *m_valuePtr;
		}
	case VARNAME:
		{
			return obj->GetValueByName( m_valuePtr->getString() );
		}
	case OP:
		{
			//can deal with "Var in DMS(a,b,c)" except "Var in (a,b,c)"
			anyType vl = left->evaluate(obj); //value
			anyType vr = right->evaluate(obj);		
			string opStr = m_valuePtr->getString();

			if(opStr == "==") return anyType(ANY_BOOL, vl==vr );
			else if(opStr == "!=")	return anyType(ANY_BOOL, vl!=vr );
			else if(opStr == ">=")  return anyType(ANY_BOOL, vl>=vr );
			else if(opStr == "<=") return anyType(ANY_BOOL, vl<=vr );
			else if(opStr == ">") return anyType(ANY_BOOL, vl>vr );
			else if(opStr == "<") return anyType(ANY_BOOL, vl<vr );
			else if(opStr == "or" || opStr == "OR") 
			{
				if( vl.getType()!=ANY_BOOL || vr.getType()!=ANY_BOOL )
				{
					//throw
				}
				else
					return anyType(ANY_BOOL, vl.getBoolean() || vr.getBoolean() );
			}
			else if(opStr == "and" || opStr == "AND")
			{
				if( vl.getType()!=ANY_BOOL || vr.getType()!=ANY_BOOL )
				{
					//throw
				}
				else
					return anyType(ANY_BOOL, vl.getBoolean() && vr.getBoolean() );
			}
			else{ //throw illegal op 
				}

		}
	
	default: 
		{
			// throw illegal node type
		}
	}
}

 anyType ParseFunc::evaluate(CObject *obj) const
{
	return  anyType(ANY_BOOL,true );
}

int main()
{ 
	//有待修改  order by 在 where 之后 , 无where ; 的处理
	sql_parser parser;
	
	string test_select_where = "select f1,f2,f3  from mytable where Population > 10 and CountryCode = \"xxx\" and District in DMSOperation(\'California\',\'New York\');";

	string test_insert = "INSERT INTO mytable (c1,c2,c3) values (\'Fred\',\'Andy\',\'Joey\')";

	string test_update = "UPDATE Person SET Address = \"Zhongshan 23\", City = \"Nanjing\"";

	string test_delete = "DELETE FROM Person WHERE LastName = \"Wilson\"";

	string test_order = "select f1,f2,f3  from mytable ORDER BY Company DESC, OrderNumber ASC";

	parser.SetSQLString(test_select_where);
	
	
	
	parser.ParseSQLString();


#ifdef WIN32
	system("pause");
#endif


}