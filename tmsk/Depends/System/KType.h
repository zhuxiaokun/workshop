#pragma once

#define _MONIT_PERFORMANCE_2_

#if defined(_DEBUG)
//#define _MONIT_PERFORMANCE_
//#define _CHK_MEM_LEAK_
#endif

#if defined(_WIN64) || defined(__LP64__) || defined(__x86_64__)
#ifndef __x64__
#define __x64__
#endif
#endif

#if defined(_WIN32) || defined(_WIN64)
#ifndef WINDOWS
#define WINDOWS
#endif
#endif

#if defined(WIN32) || defined(WINDOWS)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#else
#include <stdio.h>
#include <stdlib.h>
#endif
#include <string.h>


#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#define JG_F ::System::File
#define JG_S ::System::Sync
#define JG_T ::System::Thread
#define JG_C ::System::Collections
#define JG_M ::System::Memory
#define JG_P ::System::Patterns
#define INVALID_PTR ((void*)-1)

const int LOG_FATAL = (1<<0);
const int LOG_ERROR = (1<<1);
const int LOG_WARN  = (1<<2);
const int LOG_INFO  = (1<<3);
const int LOG_DEBUG = (1<<4);
const int LOG_CONSOLE = (1<<31);
const int LOG_ALL   = (~0);

// 定义与寄存器等长的数值类型
template <size_t n> struct __regtype__{typedef int int_r; typedef unsigned int uint_r; };
template <> struct __regtype__<8>     {typedef long long int_r;typedef unsigned long long uint_r;};
typedef __regtype__<sizeof(void*)>::int_r  int_r;
typedef __regtype__<sizeof(void*)>::uint_r uint_r;

typedef char* cstr;
typedef const char* ccstr;

#if defined(WIN32) || defined(WINDOWS)

#define PATH_SEPERATOR '\\'
typedef int	socklen_t;
typedef int	TIME_T;
typedef wchar_t wchar_16;
#define stricmp _stricmp

#else

#define TRUE  (1)
#define FALSE (0)
#define PATH_SEPERATOR '/'

typedef unsigned char		BYTE;
typedef BYTE*				LPBYTE;
typedef unsigned short		WORD;
typedef unsigned int		DWORD;
typedef char				INT8;
typedef short				INT16;
typedef int					INT32;
typedef unsigned char		UINT8;
typedef unsigned short		UINT16;
typedef unsigned int		UINT32;
typedef int					INT;
typedef	unsigned int		UINT;
typedef char				CHAR;
typedef unsigned char		UCHAR;
typedef int					LONG;
typedef unsigned int		ULONG;
typedef short				SHORT;
typedef	unsigned short		USHORT;
typedef int					BOOL;
typedef float				FLOAT;
typedef long long			INT64;
typedef unsigned long long	UINT64;
typedef double				DOUBLE;
typedef void *				HANDLE;
typedef void *				HMODULE;
typedef void				VOID;
typedef int					SOCKET;
typedef const char*			LPCTSTR;
typedef uint_r				ULONG_PTR;
typedef void*				PVOID;
typedef void*				LPVOID;
typedef void*				PVOID;
typedef void*				LPVOID;
typedef int					TIME_T;
typedef unsigned short		wchar_16;

#define _stricmp strcasecmp
#define stricmp strcasecmp

typedef struct tagPOINT
{
	LONG x;
	LONG y;
} POINT;

#endif

//typedef UINT64 KObjectID;

struct INT128
{
	INT64 l,h;
	INT128():h(0),l(0) {;}
	INT128(int nID):h(0),l(nID) {;}
	bool operator < (const int & o) const
	{
		return l < o && h == 0;
	}
	bool operator < (const INT128& o) const
	{
		return h < o.h || (h==o.h && l<o.l);
	}
	bool operator == (const int & o) const
	{
		return l == o && h == 0;
	}
	bool operator == (const INT128 & o) const
	{
		return l == o.l && h == o.h;
	}
	bool operator != (const INT128 & o) const
	{
		return l != o.l || h != o.h;
	}
	INT128 &operator=( const INT128& o )
	{
		l = o.l; 
		h = o.h;
		return *this;
	}
	INT128 &operator=( const int& o )
	{
		l = o;
		h = 0;
		return *this;
	}
};

// 32位标识号数据类型
struct Identity32
{
	UINT32 value;
	////////////////////////////////////////////
	Identity32():value(0) { }
	Identity32(char val):value(val) { }
	Identity32(unsigned char val):value(val) { }
	Identity32(short val):value(val) { }
	Identity32(unsigned short val):value(val) { }
	Identity32(int val):value(val) { }
	Identity32(unsigned int val):value(val) { }
	Identity32(long val):value((UINT32)val) { }
	Identity32(unsigned long val):value((UINT32)val) { }
	Identity32(const Identity32& o):value(o.value) { }

#if defined(__x64__)
	Identity32(int_r val):value((UINT32)val) { }
	Identity32(uint_r val):value((UINT32)val) { }
#endif

	operator int () const { return (INT32)value; }
	operator unsigned int () const { return value; }
	operator long() const { return (long)value; }
	operator unsigned long() const { return (unsigned long)value; }

#if defined(__x64__)
	operator int_r () const { return value; }
	operator uint_r () const { return value; }
#endif

	Identity32& operator = (char val)  { value = val; return *this; }
	Identity32& operator = (unsigned char val) { value = val; return *this; }
	Identity32& operator = (short val)  { value = val; return *this; }
	Identity32& operator = (unsigned short val) { value = val; return *this; }
	Identity32& operator = (int val)  { value = val; return *this; }
	Identity32& operator = (unsigned int val) { value = val; return *this; }
	Identity32& operator = (long val) { value = (UINT32)val; return *this; }
	Identity32& operator = (unsigned long val)  { value = (UINT32)val; return *this; }
	Identity32& operator = (const Identity32& o) { value = o.value; return *this; }

#if defined(__x64__)
	Identity32& operator = (int_r val)  { value = (UINT32)val; return *this; }
	Identity32& operator = (uint_r val) { value = (UINT32)val; return *this; }
#endif
};

// 64位标识号数据类型
struct Identity64
{
	UINT64 value;
	/////////////////////////////////////////////////
	Identity64():value(0) { }
	Identity64(char val):value(val) { }
	Identity64(unsigned char val):value(val) { }
	Identity64(short val):value(val) { }
	Identity64(unsigned short val):value(val) { }
	Identity64(int val):value(val) { }
	Identity64(unsigned int val):value(val) { }
	Identity64(long val):value(val) { }
	Identity64(unsigned long val):value(val) { }
	Identity64(INT64 val):value(val) { }
	Identity64(UINT64 val):value(val) { }
	Identity64(const Identity64& o):value(o.value) { }

	Identity64& operator = (char val) { value = val; return *this; }
	Identity64& operator = (unsigned char val) { value = val; return *this; }
	Identity64& operator = (short val) { value = val; return *this; }
	Identity64& operator = (unsigned short val) { value = val; return *this; }
	Identity64& operator = (int val) { value = val; return *this; }
	Identity64& operator = (unsigned int val) { value = val; return *this; }
	Identity64& operator = (long val) { value = val; return *this; }
	Identity64& operator = (unsigned long val) { value = val; return *this; }
	Identity64& operator = (INT64 val) { value = val; return *this; }
	Identity64& operator = (UINT64 val) { value = val; return *this; }
	Identity64& operator = (const Identity64& o) { value = o.value; return *this; }

	operator INT64 () const { return (INT64)value; }
	operator UINT64 () const { return value; }

	bool operator == (const Identity64& o) const { return value == o.value; }
	bool operator != (const Identity64& o) const { return value != o.value; }
	bool operator > (const Identity64& o) const  { return value > o.value;  }
	bool operator >= (const Identity64& o) const { return value >= o.value; }
	bool operator < (const Identity64& o) const  { return value < o.value;  }
	bool operator <= (const Identity64& o) const { return value <= o.value; }
};

template<typename A,typename B> struct KPair
{
	A first;B second;
	KPair() {}
	KPair(const A& a, const B& b):first(a),second(b) {}
	KPair(const KPair& o):first(o.first),second(o.second){}
	KPair& operator = (const KPair& o) { if(this == &o) return *this;first = o.first;second = o.second;return *this;}
	bool operator == (const KPair& o) { return this == &o ? true : first==o.first&&second==o.second;}
};

template<typename A,typename B,typename C> struct KTriple
{
	A first;B second;C third;
	KTriple() {}
	KTriple(const A& a,const B& b,const C& c):first(a),second(b),third(c) {}
	KTriple(const KTriple& o):first(o.first),second(o.second),third(o.third) {}
	KTriple& operator = (const KTriple& o) { if(this == &o) return *this;first = o.first;second = o.second;third = o.third;return *this;}
	bool operator == (const KTriple& o) { return this == &o ? true : first==o.first&&second==o.second&&third==o.third;}
};

template<typename A,typename B,typename C,typename D> struct KQuadruple
{
	A first;B second;C third;D fourth;
	KQuadruple() {}
	KQuadruple(const A& a, const B& b, const C& c, const D& d):first(a),second(b),third(c),fourth(d) {}
	KQuadruple(const KQuadruple& o):first(o.first),second(o.second),third(o.third),fourth(o.fourth) {}
	KQuadruple& operator = (const KQuadruple& o) { if(this == &o) return *this;first = o.first;second = o.second;third = o.third;fourth = o.fourth;;return *this;}
	bool operator == (const KQuadruple& o) { return this == &o ? true : first==o.first&&second==o.second&&third==o.third&&fourth==o.fourth;}
};

template<typename A,typename B,typename C,typename D,typename E> struct KQuintuple
{
	A first;B second;C third;D fourth;E fifth;
	KQuintuple() {}
	KQuintuple(const A& a, const B& b, const C& c, const D& d, const E& e):first(a),second(b),third(c),fourth(d),fifth(e) {}
	KQuintuple(const KQuintuple& o):first(o.first),second(o.second),third(o.third),fourth(o.fourth),fifth(o.fifth) {}
	KQuintuple& operator = (const KQuintuple& o) { if(this == &o) return *this;first = o.first;second = o.second;third = o.third;fourth = o.fourth;fifth = o.fifth;return *this;}
	bool operator == (const KQuintuple& o) { return this == &o ? true : first==o.first&&second==o.second&&third==o.third&&fourth==o.fourth&&fifth==o.fifth;}
};

template<typename A,typename B,typename C,typename D,typename E,typename F> struct KSextuple
{
	A first;B second;C third;D fourth;E fifth;F sixth;
	KSextuple() {}
	KSextuple(const A& a, const B& b, const C& c, const D& d, const E& e, const F& f):first(a),second(b),third(c),fourth(d),fifth(e),sixth(f) {}
	KSextuple(const KSextuple& o):first(o.first),second(o.second),third(o.third),fourth(o.fourth),fifth(o.fifth),sixth(o.sixth) {}
	KSextuple& operator = (const KSextuple& o) { if(this == &o) return *this;first = o.first;second = o.second;third = o.third;fourth = o.fourth;fifth = o.fifth;sixth = o.sixth;return *this;}
	bool operator == (const KSextuple& o) { return this == &o ? true : first==o.first&&second==o.second&&third==o.third&&fourth==o.fourth&&fifth==o.fifth&&sixth==o.sixth;}
};

template <typename T> struct KRect
{
	T left;
	T top;
	T right;
	T bottom;
};

template <typename T> class KLess
{
private:
	typedef T value_type;

public:
	BOOL operator () (const value_type& a, const value_type& b) const
	{
		return (a) < (b);
	}
};

template <typename T> class KIntCompare
{
public:
	int operator () (const T& a, const T& b) const
	{
		return (int)a - (int)b;
	}
};

template <typename T> class KUintCompare
{
public:
	int operator () (const T& a, const T& b) const
	{
		if((DWORD)a < (DWORD)b) return -1;
		else if((DWORD)b < (DWORD)a) return 1;
		else return 0;
	}
};

template <typename T> class KInt64Compare
{
public:
	int operator () (const T& a, const T& b) const
	{
		return (int)((INT64)a - (INT64)b);
	}
};

template <typename T> class KUint64Compare
{
public:
	int operator () (const T& a, const T& b) const
	{
		if((UINT64)a < (UINT64)b) return -1;
		else if((UINT64)b < (UINT64)a) return 1;
		else return 0;
	}
};

template <typename T> class KPointerCompare
{
public:
	int operator () (const T& a, const T& b) const
	{
		if((uint_r)a < (uint_r)b) return -1;
		else if((uint_r)b < (uint_r)a) return 1;
		else return 0;
	}
};

// 比较两个变量，大于返回 > 0，小于返回 <0，等于返回0
template < typename T,typename L=KLess<T> > class KLessCompare
{
public:
	L m_less;
	int operator () (const T& a, const T& b) const
	{
		if(m_less(a, b)) return -1;
		else if(m_less(b,a)) return 1;
		else return 0;
	}
};

template <typename T> class KHasher
{
public:
	int operator () (const T& val) const
	{
		const DWORD magic = 199;
		DWORD num = (DWORD)val;
		num *= magic;
		return num & 0x7fffffff;
	}
};

template <typename T> class KPointerHasher
{
public:
	int operator () (const T& val) const
	{
		const uint_r magic = 199;
		// 考虑到绝大部分情况至少是四字节对齐，右移两位比较合适
		uint_r num = (uint_r)val >> 2;
		num *= magic;
		return num & 0x7fffffff;
	}
};

template <typename T> class KStringHasher
{
public:
	int operator () (const T& val) const
	{
		int i = 0;
		const DWORD magic[4] = {113, 127, 131, 137};
		const char* str = (const char*)val;
		DWORD num = 0;
		while(str[0])
		{
			DWORD magicNum = magic[i++];
			if(i >= 4) i = 0;
			num += magicNum * str[0];
			str += 1;
		}
		return num & 0x7fffffff;
	}
};

class KcstrCmp
{
public:
	int operator() (const cstr& a, const cstr& b) const
	{
		return strcmp(a, b);
	}
};
class KcstriCmp
{
public:
	int operator() (const cstr& a, const cstr& b) const
	{
		return stricmp(a, b);
	}
};
class KcstrLess
{
public:
	BOOL operator() (const cstr& a, const cstr& b) const
	{
		return strcmp(a, b) < 0;
	}
};
class KcstriLess
{
public:
	BOOL operator() (const cstr& a, const cstr& b) const
	{
		return stricmp(a, b) < 0;
	}
};

class KccstrCmp
{
public:
	int operator() (const ccstr& a, const ccstr& b) const
	{
		return strcmp(a, b);
	}
};
class KccstriCmp
{
public:
	int operator() (const ccstr& a, const ccstr& b) const
	{
		return stricmp(a, b);
	}
};
class KccstrLess
{
public:
	BOOL operator() (const ccstr& a, const ccstr& b) const
	{
		return strcmp(a, b) < 0;
	}
};
class KccstriLess
{
public:
	BOOL operator() (const ccstr& a, const ccstr& b) const
	{
		return stricmp(a, b) < 0;
	}
};