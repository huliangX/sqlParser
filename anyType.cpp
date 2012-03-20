#include "anyType.h"
#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

anyType::anyType( int _typeNum , string _value  )
{
 try 
 {  
	switch(_typeNum)
	{
	case ANY_BOOL:
		 {
			 m_value =  lexical_cast<bool>(_value);
			 return;
		 }
	case ANY_INT:
		{
			 m_value =  lexical_cast<int>(_value);
			 return;
		}
	case ANY_LONG_64:
		{
			 m_value =  lexical_cast<long long>(_value);
			 return;
		}
	case ANY_DOUBLE:
		{
			 m_value =  lexical_cast<double>(_value);
			 return;
		}
	case ANY_FLOAT:
		{
			 m_value =  lexical_cast<float>(_value);
			 return;
		}
	case ANY_STRING:
		{
			 m_value =  _value;
			 return;
		}

	case ANY_UINT:
		{
			 m_value =  lexical_cast<unsigned int>(_value);
			 return;
		}
	default:
		{
			//throw
		}

	} //switch
 }//try
 catch(boost::bad_lexical_cast& e )
 {
	 cout << "anyType initail error:!\n"<<e.what()<<endl; 
 }
}



void anyType::setValue( int _typeNum , string _value  )
{
 try 
 {  
	switch(_typeNum)
	{
	case ANY_BOOL:
		 {
			 m_value =  lexical_cast<bool>(_value);
			 return;
		 }
	case ANY_INT:
		{
			 m_value =  lexical_cast<int>(_value);
			 return;
		}
	case ANY_LONG_64:
		{
			 m_value =  lexical_cast<long long>(_value);
			 return;
		}
	case ANY_DOUBLE:
		{
			 m_value =  lexical_cast<double>(_value);
			 return;
		}
	case ANY_FLOAT:
		{
			 m_value =  lexical_cast<float>(_value);
			 return;
		}
	case ANY_STRING:
		{
			 m_value =  _value;
			 return;
		}

	case ANY_UINT:
		{
			 m_value =  lexical_cast<unsigned int>(_value);
			 return;
		}
	default:
		{
			//throw
		}

	} //switch
 }//try
 catch(boost::bad_lexical_cast& e )
 {
	 cout << "anyType initail error:!\n"<<e.what()<<endl; 
 }
}

anyType::~anyType(void)
{
}



bool anyType::getBoolean() const
{
	try
	{
	return boost::any_cast<bool>(m_value);
	}
	catch(boost::bad_any_cast& e) 
	{
	cout << "any_cast fail,cannot converse into bool type"<<e.what()<<endl; 
	return false;
	}
}

int anyType::getInt() const
{

	try
	{
	return boost::any_cast<int>(m_value);
	}
	catch(boost::bad_any_cast& e) 
	{
	cout << "any_cast fail,cannot converse into int type"<<e.what()<<endl; 
	return -1;
	}
}

unsigned int anyType::getUInt() const
{

	try
	{
	return boost::any_cast<unsigned int>(m_value);
	}
	catch(boost::bad_any_cast& e) 
	{
	cout << "any_cast fail,cannot converse into unsigned int type"<<e.what()<<endl; 
		return -1;
	}
}

long long anyType::getLong() const
{

	try
	{
	return boost::any_cast<long long>(m_value);
	}
	catch(boost::bad_any_cast& e) 
	{
	cout << "any_cast fail,cannot converse into  long long type"<<e.what()<<endl;
		return -1;
	}
}

double anyType::getDouble() const
{

	try
	{
	return boost::any_cast<double>(m_value);
	}
	catch(boost::bad_any_cast& e) 
	{
	cout << "any_cast fail,cannot converse into double type"<<e.what()<<endl; 
		return -1;
	}
}
float anyType::getFloat() const
{

	try
	{
	return boost::any_cast<float>(m_value);
	}
	catch(boost::bad_any_cast& e) 
	{
	cout << "any_cast fail,cannot converse into float type"<<e.what()<<endl; 
		return -1;
	}
}
string anyType::getString() const
{
	try
	{
	return boost::any_cast<string>(m_value);
	}
	catch(boost::bad_any_cast& e) 
	{
	cout << "any_cast fail,cannot converse into string type"<<e.what()<<endl; 
	return "error";
	}
}

int anyType::getType() const
{
	string typeName = m_value.type().name();
	if(typeName == "bool") return ANY_BOOL;
	else if(typeName == "int") return ANY_INT;
	else if(typeName == "unsigned int") return ANY_UINT;
	else if(typeName == "__int64") return ANY_LONG_64;
	else if(typeName == "double") return ANY_DOUBLE;
	else if(typeName == "float") return  ANY_FLOAT;
	else if(typeName == "string") return ANY_STRING;
	else return -1;
}


//OpNum used for build exception
template <template <typename Type> class Operator, int OpNum>
bool anyType::binary_comp_op(const anyType &b) const
{
    switch( this->getType() )
    {
	case ANY_BOOL:
		{
			switch( b.getType() )
			{
			case ANY_BOOL:
				{
				Operator<bool> op;
				return op(this->getBoolean(), b.getBoolean() );
				}
			default:
				{
					// throw
				}
			}
			break;
		}//CASE BOOL
	case ANY_INT:
		{
			switch( b.getType() )
			{
			case ANY_BOOL:
				{
					//throw
				}
			case ANY_INT:
			case ANY_STRING:
				{
				Operator<int> op;
				return op(this->getInt(), b.getInt() );
				}
			case ANY_UINT:
				{
				Operator<unsigned> op;
				return op(this->getUInt(), b.getUInt() );
				}
			case ANY_LONG_64:
				{
				Operator<long long> op;
				return op(this->getLong(), b.getLong() );				
				}
			case ANY_DOUBLE:
				{
				Operator<double> op;
				return op(this->getDouble(), b.getDouble() );				
				}
			case ANY_FLOAT:
				{
				Operator<float> op;
				return op(this->getFloat(), b.getFloat() );				
				}

			default:
				{
					// throw
				}
			}//switch b.type
			break;
		}//case INT
	case ANY_UINT:
		{
		switch( b.getType() )
			{
			case ANY_BOOL:
				{
					//throw
				}
			case ANY_INT:
			case ANY_STRING:
			case ANY_UINT:
				{
				Operator<unsigned> op;
				return op(this->getUInt(), b.getUInt() );
				}
			case ANY_LONG_64:
				{
				Operator<long long> op;
				return op(this->getLong(), b.getLong() );				
				}
			case ANY_DOUBLE:
				{
				Operator<double> op;
				return op(this->getDouble(), b.getDouble() );				
				}
			case ANY_FLOAT:
				{
				Operator<float> op;
				return op(this->getFloat(), b.getFloat() );				
				}

			default:
				{
					// throw
				}
			}//switch b.type
			break;
		}//CASE UNIT
	case ANY_LONG_64:
		{
		switch( b.getType() )
			{
			case ANY_BOOL:
				{
					//throw
				}
			case ANY_INT:
			case ANY_STRING:
			case ANY_UINT:
			case ANY_LONG_64:
				{
				Operator<long long> op;
				return op(this->getLong(), b.getLong() );				
				}
			case ANY_DOUBLE:
				{
				Operator<double> op;
				return op(this->getDouble(), b.getDouble() );				
				}
			case ANY_FLOAT:
				{
				Operator<float> op;
				return op(this->getFloat(), b.getFloat() );				
				}

			default:
				{
					// throw
				}
			}//switch b.type
			break;
		}//case LONG_64
	case ANY_FLOAT:
		{
		switch( b.getType() )
			{
			case ANY_BOOL:
				{
					//throw
				}
			case ANY_INT:
			case ANY_STRING:
			case ANY_UINT:
			case ANY_LONG_64:
			case ANY_FLOAT:
				{
				Operator<float> op;
				return op(this->getFloat(), b.getFloat() );				
				}
			case ANY_DOUBLE:
				{
				Operator<double> op;
				return op(this->getDouble(), b.getDouble() );				
				}
			default:
				{
					// throw
				}
			}//switch b.type
			break;
		}//case FLOAT
	case ANY_DOUBLE:
		{
		switch( b.getType() )
			{
			case ANY_BOOL:
				{
					//throw
				}
			case ANY_INT:
			case ANY_STRING:
			case ANY_UINT:
			case ANY_LONG_64:
			case ANY_FLOAT:
			case ANY_DOUBLE:
				{
				Operator<double> op;
				return op(this->getDouble(), b.getDouble() );				
				}
			default:
				{
					// throw
				}
			}//switch b.type
			break;
		}
	case ANY_STRING:
		{
	switch( b.getType() )
			{
			case ANY_BOOL:
				{
					//throw
				}
			case ANY_STRING:
				{
				Operator<string> op;
				return op(this->getString(), b.getString() );
				}
			case ANY_INT:
				{
				Operator<int> op;
				return op(this->getInt(), b.getInt() );
				}
			case ANY_UINT:
				{
				Operator<unsigned> op;
				return op(this->getUInt(), b.getUInt() );
				}
			case ANY_LONG_64:
				{
				Operator<long long> op;
				return op(this->getLong(), b.getLong() );				
				}
			case ANY_DOUBLE:
				{
				Operator<double> op;
				return op(this->getDouble(), b.getDouble() );				
				}
			case ANY_FLOAT:
				{
				Operator<float> op;
				return op(this->getFloat(), b.getFloat() );				
				}

			default:
				{
					// throw
				}
			}//switch b.type
			break;
		}//case STRING
	
	}//switch this.type
}