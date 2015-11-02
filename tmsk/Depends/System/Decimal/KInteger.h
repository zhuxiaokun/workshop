/********************************************************************
created:	2013/2/16
manager:	Allen Wang
copyright:	Aurora Interactive, 2013
*********************************************************************/
#pragma once

#include "../KType.h"
#include "../KMacro.h"
#include <string>
#include <limits.h>

/**
 * 整数类
 */
class KInteger
{
public:
	typedef DWORD digit;		// 单个位数无符号字的大小
	typedef int sdigit;			// 单个位数有符号字的大小
	typedef UINT64 twodigits;	// 两个位数无符号字的大小
	typedef INT64 stwodigits;	// 两个位数有符号字的大小
private:
	void _init()
	{
		m_size = 0;
		m_digitSize = 0;
		ob_digit = NULL;
	}
public:
	KInteger();
	virtual ~KInteger()
	{
		_Free();
	}
	KInteger(const KInteger& o);
	KInteger& operator=(const KInteger& o);
	void ToZero()
	{
		m_size = 0;
	}
	void ToOne()
	{
		_Alloc(1);
		m_size = 1;
		ob_digit[0] = 1;
	}
	// 绝对值
	KInteger abs() const
	{
		if (m_size >= 0) return *this;
		return -*this;
	}

private:
	// 从ByteArray转换成KInteger
	void _FromByteArray(const unsigned char* bytes, size_t n, int little_endian, int is_signed);
	// 把KInteger转成ByteArray
	int _AsByteArray(unsigned char* bytes, size_t n, int little_endian, int is_signed) const;

public:
	int m_size;			// 当前可用的大小
	int m_digitSize;	// 当前分配的大小
	digit* ob_digit;	// 记录整数内容，为了加速加减法（不用转换成twodigits计算），使用1位空闲，用其它位保存数据

	// TODO: 以后可能优化
	// 按新size分配内存，填充0，并且把m_size也拉大到指定大小
	virtual void _Alloc(int size);
	// 按新size分配内存，保持原来数据内容
	virtual void _Realloc(int size);
	// 清理，并且把m_size设置成0
	virtual void _Free();

public:
	//////////////////////////////////////////////////////////////////////////
	// 自己的运算
	// 四则运算
	KInteger operator+(const KInteger& o) const;
	KInteger operator-(const KInteger& o) const;
	KInteger operator*(const KInteger& o) const;
	KInteger operator%(const KInteger& o) const;
	KInteger operator/(const KInteger& o) const;
	void Divmod(const KInteger& o, KInteger& div, KInteger& mod) const;
	// 比较运算
	bool operator<(const KInteger& o) const;
	bool operator<=(const KInteger& o) const;
	bool operator==(const KInteger& o) const;
	bool operator>=(const KInteger& o) const;
	bool operator>(const KInteger& o) const;
	bool operator!=(const KInteger& o) const;
	// 单目运算
	KInteger operator-() const;
	KInteger operator+() const;
	// 位操作
	KInteger operator<<(int v) const;
	KInteger operator>>(int v) const;
	KInteger operator&(const KInteger& o) const;
	KInteger operator|(const KInteger& o) const;
	KInteger operator^(const KInteger& o) const;
	KInteger operator~() const;
	// 自操作运算
	KInteger& operator++();
	KInteger operator++(int);
	KInteger& operator--();
	KInteger operator--(int);
	KInteger& operator+=(const KInteger& o);
	KInteger& operator-=(const KInteger& o);
	KInteger& operator*=(const KInteger& o);
	KInteger& operator/=(const KInteger& o);
	KInteger& operator%=(const KInteger& o);
	KInteger& operator&=(const KInteger& o);
	KInteger& operator|=(const KInteger& o);
	KInteger& operator^=(const KInteger& o);
	KInteger& operator<<=(int val);
	KInteger& operator>>=(int val);
	// 强制类型转换
	operator bool() const;
	// 从ByteArray转成KInteger
	bool fromByteArray(const unsigned char* bytes, size_t n, int little_endian, int is_signed)
	{
		_FromByteArray(bytes, n, little_endian, is_signed);
        return true;
	}
	// 得到大整数的内存表示，能放得下返回true，否则false
	bool toByteArray(unsigned char* arr, size_t arrSize) const
	{
		if (_AsByteArray(arr, arrSize, 1, 1) < 0)
		{
			return false;
		}
		return true;
	}

	/**
	 * 从字符串转成KInteger
	 */
	KInteger( const char* str )
	{
		_init();
		fromString(str);
	}
	KInteger& operator=( const char* str )
	{
		fromString(str);
		return *this;
	}
	void fromString(const char* str);

	// 以10进制显示字符串
	std::string toString() const;
	// 以2/8/16进制显示字符串
	std::string toString(int base, int alternate = 0) const;

	//////////////////////////////////////////////////////////////////////////
	// 兼容简单数据类型处理
	/**
	 * 转换函数定义
	 */
#define FROM_TYPE_TRANSLATE_DECLARATION(TYPE, SIGN_FLAG, FUNC_NAME)	\
	KInteger( TYPE v )	\
	{	\
		_init();	\
		FUNC_NAME(v);	\
	}	\
	KInteger& operator=( TYPE v )	\
	{	\
		FUNC_NAME(v);	\
		return *this;	\
	}	\
	void FUNC_NAME( TYPE v )	\
	{	\
		_FromByteArray((unsigned char *)&v, sizeof(v), 1, SIGN_FLAG);	\
	}

	// 强制类型转换
#define FORCE_TYPE_TRANSLATE_DECLARATION(TYPE, SIGN_FLAG, MAX_VAL, MIN_VAL)	\
	operator TYPE() const	\
	{	\
		TYPE v = 0;	\
		if (_AsByteArray((unsigned char*)&v, sizeof(v), 1, SIGN_FLAG) < 0)	\
		{	\
			/* 溢出处理 */	\
			if (m_size > 0) return MAX_VAL;	\
			else return MIN_VAL;	\
		}	\
		return v;	\
	}

	// 四则运算
#define COMPUTE_OPERATOR_DECLARATION(TYPE)	\
	friend KInteger operator+(const KInteger& a, TYPE b)	\
	{	\
		KInteger ib(b);	\
		return a.operator+(ib);	\
	}	\
	friend KInteger operator+(TYPE b, const KInteger& a)	\
	{	\
		KInteger ib(b);	\
		return a.operator+(ib);	\
	}	\
	friend KInteger operator-( const KInteger& a, TYPE b )	\
	{	\
		KInteger ib(b);	\
		return a.operator-(ib);	\
	}	\
	friend KInteger operator-( TYPE b, const KInteger& a )	\
	{	\
		KInteger ib(b);	\
		return a.operator-(ib);	\
	}	\
	friend KInteger operator*( const KInteger& a, TYPE b )	\
	{	\
		KInteger ib(b);	\
		return a.operator*(ib);	\
	}	\
	friend KInteger operator*( TYPE b, const KInteger& a )	\
	{	\
		KInteger ib(b);	\
		return a.operator*(ib);	\
	}	\
	friend KInteger operator%( const KInteger& a, TYPE b )	\
	{	\
		KInteger ib(b);	\
		return a.operator%(ib);	\
	}	\
	friend KInteger operator%( TYPE b, const KInteger& a )	\
	{	\
		KInteger ib(b);	\
		return a.operator%(ib);	\
	}	\
	friend KInteger operator/( const KInteger& a, TYPE b )	\
	{	\
		KInteger ib(b);	\
		return a.operator/(ib);	\
	}	\
	friend KInteger operator/( TYPE b, const KInteger& a )	\
	{	\
		KInteger ib(b);	\
		return a.operator/(ib);	\
	}

	// 比较运算
#define COMPARE_OPERATOR_DECLARATION(TYPE)	\
	friend bool operator<(const KInteger& a, TYPE b)	\
	{	\
		KInteger ib(b);	\
		return a < ib;	\
	}	\
	friend bool operator<(TYPE b, const KInteger& a)	\
	{	\
		KInteger ib(b);	\
		return ib < a;	\
	}	\
	friend bool operator<=(const KInteger& a, TYPE b)	\
	{	\
		KInteger ib(b);	\
		return a <= ib;	\
	}	\
	friend bool operator<=(TYPE b, const KInteger& a)	\
	{	\
		KInteger ib(b);	\
		return ib <= a;	\
	}	\
	friend bool operator==(const KInteger& a, TYPE b)	\
	{	\
		KInteger ib(b);	\
		return a == ib;	\
	}	\
	friend bool operator==(TYPE b, const KInteger& a)	\
	{	\
		KInteger ib(b);	\
		return ib == a;	\
	}	\
	friend bool operator>=(const KInteger& a, TYPE b)	\
	{	\
		KInteger ib(b);	\
		return a >= ib;	\
	}	\
	friend bool operator>=(TYPE b, const KInteger& a)	\
	{	\
		KInteger ib(b);	\
		return ib >= a;	\
	}	\
	friend bool operator>(const KInteger& a, TYPE b)	\
	{	\
		KInteger ib(b);	\
		return a > ib;	\
	}	\
	friend bool operator>(TYPE b, const KInteger& a)	\
	{	\
		KInteger ib(b);	\
		return ib > a;	\
	}	\
	friend bool operator!=(const KInteger& a, TYPE b)	\
	{	\
		KInteger ib(b);	\
		return a != ib;	\
	}	\
	friend bool operator!=(TYPE b, const KInteger& a)	\
	{	\
		KInteger ib(b);	\
		return ib != a;	\
	}

/**
 * 整数相关的基础运算定义
 */
#define OPERATOR_DECLARATION_ALL_IN_ONE(TYPE, SIGN_FLAG, FUNC_NAME, MAX_VAL, MIN_VAL)	\
	FROM_TYPE_TRANSLATE_DECLARATION(TYPE, SIGN_FLAG, FUNC_NAME);	\
	FORCE_TYPE_TRANSLATE_DECLARATION(TYPE, SIGN_FLAG, MAX_VAL, MIN_VAL);	\
	COMPUTE_OPERATOR_DECLARATION(TYPE);	\
	COMPARE_OPERATOR_DECLARATION(TYPE);


	OPERATOR_DECLARATION_ALL_IN_ONE(char,				1,	fromChar,		127, -128				);
	OPERATOR_DECLARATION_ALL_IN_ONE(short,				1,	fromShort,		SHRT_MAX, SHRT_MIN		);
	OPERATOR_DECLARATION_ALL_IN_ONE(int,				1,	fromInt,		INT_MAX, INT_MIN		);
	OPERATOR_DECLARATION_ALL_IN_ONE(long,				1,	fromLong,		LONG_MAX, LONG_MIN		);
	OPERATOR_DECLARATION_ALL_IN_ONE(long long,			1,	fromLongLong,	LLONG_MAX, LLONG_MIN	);

	OPERATOR_DECLARATION_ALL_IN_ONE(unsigned char,		0,	fromUChar,		255, 0					);
	OPERATOR_DECLARATION_ALL_IN_ONE(unsigned short,		0,	fromUShort,		USHRT_MAX, 0			);
	OPERATOR_DECLARATION_ALL_IN_ONE(unsigned int,		0,	fromUInt,		UINT_MAX, 0				);
	OPERATOR_DECLARATION_ALL_IN_ONE(unsigned long,		0,	fromULong,		ULONG_MAX, 0			);
	OPERATOR_DECLARATION_ALL_IN_ONE(unsigned long long,	0,	fromULongLong,	ULLONG_MAX, 0			);

	// double类型的特殊处理
	KInteger( double v )
	{
		_init();
		fromDouble(v);
	}
	KInteger& operator=( double v )
	{
		fromDouble(v);
		return *this;
	}
	void fromDouble(double v);
	operator double() const;
	COMPUTE_OPERATOR_DECLARATION(double);
	COMPARE_OPERATOR_DECLARATION(double);

	// float类型的特殊处理
	KInteger( float v )
	{
		_init();
		fromFloat(v);
	}
	KInteger& operator=( float v )
	{
		fromFloat(v);
		return *this;
	}
	void fromFloat(float v)
	{
		fromDouble((double)v);
	}
	operator float() const
	{
		return (float)(double)*this;
	}
	COMPUTE_OPERATOR_DECLARATION(float);
	COMPARE_OPERATOR_DECLARATION(float);



#undef FROM_TYPE_TRANSLATE_DECLARATION
#undef FORCE_TYPE_TRANSLATE_DECLARATION
#undef COMPUTE_OPERATOR_DECLARATION
#undef COMPARE_OPERATOR_DECLARATION
#undef OPERATOR_DECLARATION_ALL_IN_ONE
};