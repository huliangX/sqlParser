#ifdef WIN32
#pragma once
#endif
#include "boost/any.hpp"
#include <string>
#include <typeinfo>

#include <ostream>
#include <stdexcept>
#include <functional>

#include <iostream>
using namespace std;

enum TypeNum_t{  ANY_BOOL=0,ANY_INT ,ANY_UINT,ANY_LONG_64,ANY_DOUBLE,ANY_FLOAT, ANY_STRING,ANY_INVALID};


//{ "==", "!=", "<", ">", "<=", ">=" }
enum TypeOp_t { EQ,NE,LT,GT,LE,GE };

class anyType
{
public:

	//may occar exception when call function lexical 
	anyType( int _typeNum = ANY_STRING, string _value = "");
	~anyType(void);


	
	void setValue( int _typeNum = ANY_STRING,string _value = "");
	int getType() const;


//may occar exception when call function any_cast
	bool getBoolean() const;
	int	getInt() const;
	unsigned int	getUInt() const;
	long long 		getLong() const;
	double		getDouble() const;
	float		getFloat() const;
	std::string		getString() const;

	
//template <typename T>
//bool converseType(T &temp);


    template <template <typename Type> class Operator, int OpNum>
    bool		binary_comp_op(const anyType &b) const;



	inline bool operator==(const anyType &b) const
    {
	return binary_comp_op<std::equal_to, EQ>(b);
    }
	inline bool operator!=(const anyType &b) const
    {
	return binary_comp_op<std::not_equal_to, NE>(b);
    }
    inline bool operator<(const anyType &b) const
    {
	return binary_comp_op<std::less, LT>(b);
    }
    inline bool operator>(const anyType &b) const
    {
		return binary_comp_op<std::greater, GT>(b);
    }
    inline bool operator<=(const anyType &b) const
    {
		return binary_comp_op<std::less_equal, LE>(b);
    }
    inline bool operator>=(const anyType &b) const
    {
		return binary_comp_op<std::greater_equal, GE>(b);
    }


	 friend inline std::ostream& operator<<(ostream & os,const anyType &as)
	{
		//return _stream << as.getString();
	    std::cout<<as.getString()<<std::endl;
		return os;
	}


private:

	boost::any m_value;

};



/** Base class for other exceptions of the expression parser and
 * evaluators. \ingroup Exception */
class ExpressionParserException : public std::runtime_error
{
public:
    /// Construct with a description string.
    inline ExpressionParserException(const std::string &s) throw()
	: std::runtime_error(s)
    { }
};

/** ConversionException is an exception class thrown by some combinations of
 * get and set in AnyScalar. \ingroup Exception */

class ConversionException : public ExpressionParserException
{
public:
    /// Constructor of the exception takes the description string s.
    inline ConversionException(const std::string &s) throw()
	: ExpressionParserException(s)
    { }
};
