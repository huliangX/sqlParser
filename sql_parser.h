#ifdef WIN32
#pragma once
#endif

#include <string>
#include <vector>
#include <map>
#include <list>

#include <boost/spirit/core.hpp>
#include <boost/spirit/include/classic_ast.hpp>

#include "anyType.h"


using namespace std;
using namespace boost::spirit;


typedef std::string::const_iterator InputIterT;
typedef tree_match<InputIterT> ParseTreeMatchT;
typedef ParseTreeMatchT::const_tree_iterator TreeIterT;

//typedef boost::variant<bool,int,unsigned int,long,unsigned long,float,double,string> DPLVariant;

enum node_t{  BOOL=0,INT ,LONG,DOUBLE,STRING,VARNAME ,OP,FUNC,PLACEHOLDER };


class CObject
{

public:
         virtual anyType GetValueByName(string strName)=0;   
         virtual bool operator==(CObject* obj) const{ return (obj==this); }
         virtual bool operator<(CObject* obj) const{ return (obj<this);  }
};

class ParseNodeBase
{
public:
	 node_t NodeType;

	 anyType* m_valuePtr;  

	 virtual ~ParseNodeBase(){}

	 virtual node_t GetNodeType(){ return NodeType; }

	 virtual anyType* GetValue() { return m_valuePtr; }
	 virtual anyType evaluate(CObject *obj) const = 0;
};


class ParseNode:public ParseNodeBase
{
public:

     const ParseNodeBase *left;
     const ParseNodeBase *right;
	
	 TreeIterT ast_node_ptr; 
	 node_t NodeType;

	 anyType* m_valuePtr;   

	 int placeholder_index;  


    /// Disable copy construction

	ParseNode( TreeIterT const& i ,node_t Type,int index );
	ParseNode( TreeIterT const& i ,node_t Type,int index ,ParseNodeBase* _left,ParseNodeBase* _right);

    /// And disable assignment
    ParseNode& operator=(const ParseNode &pn);

    virtual ~ParseNode(){}
    
	virtual node_t GetNodeType();
	virtual anyType* GetValue() { return m_valuePtr; }
    virtual anyType evaluate(CObject *obj) const ;
};

class ParseFunc:public ParseNodeBase
{
public:


		 TreeIterT const ast_node_ptr; 
		 node_t NodeType;
       string funcname;

	   vector<anyType*> paramlist;  

	ParseFunc( TreeIterT const& i, string _funcname, vector<anyType*> _paramlist);
	virtual node_t GetNodeType();
    virtual anyType evaluate(CObject *obj) const ;
};

//============================================================================================

class sql_parser
{
public:
	sql_parser(void);
	~sql_parser(void);

	sql_parser(string& _metaString):metaString(_metaString)
	{
	}

	enum commandtype_t 
	{
		select_t = 1,
		insert_t,
		update_t,
		delete_t

	};
	enum order_direction
	{
		ASC = 0,   
		DESC = 1,
	};

	struct order
	{
		std::string varname;
		int direction;
	};


	void SetSQLString(const string &s);
	void ScanTree(TreeIterT & i);
	bool ParseSQLString();
	bool BuildConditionTree(const string &s);


	// child push into stack  first
	ParseNodeBase* BuildTreeStackByAST( TreeIterT const& i, int &index);
	void DumpTreeStack();

	typedef vector<string> fieldsVectorType;
	typedef map<string,string> fieldsMapType;

	fieldsVectorType& GetFields();

private:

	string metaString;    // used to build the tree 

	int commandtype;	
	string m_tablename;
	fieldsVectorType m_fields;
	fieldsMapType m_fields_map;
	vector<struct order> m_orders;
	
	// for evaluation associated with plugin
	tree_parse_info<InputIterT> m_ConditionTreeInfo;	
	list<ParseNodeBase*>  m_treestack;

};

