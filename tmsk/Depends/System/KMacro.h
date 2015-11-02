/********************************************************************
	created:	2007/11/15
	created:	15:11:2007   16:05
	filename: 	System\System\Macro.h
	file path:	System\System
	file base:	Macro
	file ext:	h
	author:		xueyan
	
	purpose:	定义一些最基本最常用的宏，主要是调试用的。
*********************************************************************/

#ifndef _K_MACRO_H_
#define _K_MACRO_H_

#pragma once
#include "KType.h"
#include "KAssertCallBack.h"

#define B00000000 (0x00)
#define B00000001 (0x01)
#define B00000010 (0x02)
#define B00000011 (0x03)
#define B00000100 (0x04)
#define B00000101 (0x05)
#define B00000110 (0x06)
#define B00000111 (0x07)
#define B00001000 (0x08)
#define B00001001 (0x09)
#define B00001010 (0x0a)
#define B00001011 (0x0b)
#define B00001100 (0x0c)
#define B00001101 (0x0d)
#define B00001110 (0x0e)
#define B00001111 (0x0f)
#define B00010000 (0x10)
#define B00010001 (0x11)
#define B00010010 (0x12)
#define B00010011 (0x13)
#define B00010100 (0x14)
#define B00010101 (0x15)
#define B00010110 (0x16)
#define B00010111 (0x17)
#define B00011000 (0x18)
#define B00011001 (0x19)
#define B00011010 (0x1a)
#define B00011011 (0x1b)
#define B00011100 (0x1c)
#define B00011101 (0x1d)
#define B00011110 (0x1e)
#define B00011111 (0x1f)
#define B00100000 (0x20)
#define B00100001 (0x21)
#define B00100010 (0x22)
#define B00100011 (0x23)
#define B00100100 (0x24)
#define B00100101 (0x25)
#define B00100110 (0x26)
#define B00100111 (0x27)
#define B00101000 (0x28)
#define B00101001 (0x29)
#define B00101010 (0x2a)
#define B00101011 (0x2b)
#define B00101100 (0x2c)
#define B00101101 (0x2d)
#define B00101110 (0x2e)
#define B00101111 (0x2f)
#define B00110000 (0x30)
#define B00110001 (0x31)
#define B00110010 (0x32)
#define B00110011 (0x33)
#define B00110100 (0x34)
#define B00110101 (0x35)
#define B00110110 (0x36)
#define B00110111 (0x37)
#define B00111000 (0x38)
#define B00111001 (0x39)
#define B00111010 (0x3a)
#define B00111011 (0x3b)
#define B00111100 (0x3c)
#define B00111101 (0x3d)
#define B00111110 (0x3e)
#define B00111111 (0x3f)
#define B01000000 (0x40)
#define B01000001 (0x41)
#define B01000010 (0x42)
#define B01000011 (0x43)
#define B01000100 (0x44)
#define B01000101 (0x45)
#define B01000110 (0x46)
#define B01000111 (0x47)
#define B01001000 (0x48)
#define B01001001 (0x49)
#define B01001010 (0x4a)
#define B01001011 (0x4b)
#define B01001100 (0x4c)
#define B01001101 (0x4d)
#define B01001110 (0x4e)
#define B01001111 (0x4f)
#define B01010000 (0x50)
#define B01010001 (0x51)
#define B01010010 (0x52)
#define B01010011 (0x53)
#define B01010100 (0x54)
#define B01010101 (0x55)
#define B01010110 (0x56)
#define B01010111 (0x57)
#define B01011000 (0x58)
#define B01011001 (0x59)
#define B01011010 (0x5a)
#define B01011011 (0x5b)
#define B01011100 (0x5c)
#define B01011101 (0x5d)
#define B01011110 (0x5e)
#define B01011111 (0x5f)
#define B01100000 (0x60)
#define B01100001 (0x61)
#define B01100010 (0x62)
#define B01100011 (0x63)
#define B01100100 (0x64)
#define B01100101 (0x65)
#define B01100110 (0x66)
#define B01100111 (0x67)
#define B01101000 (0x68)
#define B01101001 (0x69)
#define B01101010 (0x6a)
#define B01101011 (0x6b)
#define B01101100 (0x6c)
#define B01101101 (0x6d)
#define B01101110 (0x6e)
#define B01101111 (0x6f)
#define B01110000 (0x70)
#define B01110001 (0x71)
#define B01110010 (0x72)
#define B01110011 (0x73)
#define B01110100 (0x74)
#define B01110101 (0x75)
#define B01110110 (0x76)
#define B01110111 (0x77)
#define B01111000 (0x78)
#define B01111001 (0x79)
#define B01111010 (0x7a)
#define B01111011 (0x7b)
#define B01111100 (0x7c)
#define B01111101 (0x7d)
#define B01111110 (0x7e)
#define B01111111 (0x7f)
#define B10000000 (0x80)
#define B10000001 (0x81)
#define B10000010 (0x82)
#define B10000011 (0x83)
#define B10000100 (0x84)
#define B10000101 (0x85)
#define B10000110 (0x86)
#define B10000111 (0x87)
#define B10001000 (0x88)
#define B10001001 (0x89)
#define B10001010 (0x8a)
#define B10001011 (0x8b)
#define B10001100 (0x8c)
#define B10001101 (0x8d)
#define B10001110 (0x8e)
#define B10001111 (0x8f)
#define B10010000 (0x90)
#define B10010001 (0x91)
#define B10010010 (0x92)
#define B10010011 (0x93)
#define B10010100 (0x94)
#define B10010101 (0x95)
#define B10010110 (0x96)
#define B10010111 (0x97)
#define B10011000 (0x98)
#define B10011001 (0x99)
#define B10011010 (0x9a)
#define B10011011 (0x9b)
#define B10011100 (0x9c)
#define B10011101 (0x9d)
#define B10011110 (0x9e)
#define B10011111 (0x9f)
#define B10100000 (0xa0)
#define B10100001 (0xa1)
#define B10100010 (0xa2)
#define B10100011 (0xa3)
#define B10100100 (0xa4)
#define B10100101 (0xa5)
#define B10100110 (0xa6)
#define B10100111 (0xa7)
#define B10101000 (0xa8)
#define B10101001 (0xa9)
#define B10101010 (0xaa)
#define B10101011 (0xab)
#define B10101100 (0xac)
#define B10101101 (0xad)
#define B10101110 (0xae)
#define B10101111 (0xaf)
#define B10110000 (0xb0)
#define B10110001 (0xb1)
#define B10110010 (0xb2)
#define B10110011 (0xb3)
#define B10110100 (0xb4)
#define B10110101 (0xb5)
#define B10110110 (0xb6)
#define B10110111 (0xb7)
#define B10111000 (0xb8)
#define B10111001 (0xb9)
#define B10111010 (0xba)
#define B10111011 (0xbb)
#define B10111100 (0xbc)
#define B10111101 (0xbd)
#define B10111110 (0xbe)
#define B10111111 (0xbf)
#define B11000000 (0xc0)
#define B11000001 (0xc1)
#define B11000010 (0xc2)
#define B11000011 (0xc3)
#define B11000100 (0xc4)
#define B11000101 (0xc5)
#define B11000110 (0xc6)
#define B11000111 (0xc7)
#define B11001000 (0xc8)
#define B11001001 (0xc9)
#define B11001010 (0xca)
#define B11001011 (0xcb)
#define B11001100 (0xcc)
#define B11001101 (0xcd)
#define B11001110 (0xce)
#define B11001111 (0xcf)
#define B11010000 (0xd0)
#define B11010001 (0xd1)
#define B11010010 (0xd2)
#define B11010011 (0xd3)
#define B11010100 (0xd4)
#define B11010101 (0xd5)
#define B11010110 (0xd6)
#define B11010111 (0xd7)
#define B11011000 (0xd8)
#define B11011001 (0xd9)
#define B11011010 (0xda)
#define B11011011 (0xdb)
#define B11011100 (0xdc)
#define B11011101 (0xdd)
#define B11011110 (0xde)
#define B11011111 (0xdf)
#define B11100000 (0xe0)
#define B11100001 (0xe1)
#define B11100010 (0xe2)
#define B11100011 (0xe3)
#define B11100100 (0xe4)
#define B11100101 (0xe5)
#define B11100110 (0xe6)
#define B11100111 (0xe7)
#define B11101000 (0xe8)
#define B11101001 (0xe9)
#define B11101010 (0xea)
#define B11101011 (0xeb)
#define B11101100 (0xec)
#define B11101101 (0xed)
#define B11101110 (0xee)
#define B11101111 (0xef)
#define B11110000 (0xf0)
#define B11110001 (0xf1)
#define B11110010 (0xf2)
#define B11110011 (0xf3)
#define B11110100 (0xf4)
#define B11110101 (0xf5)
#define B11110110 (0xf6)
#define B11110111 (0xf7)
#define B11111000 (0xf8)
#define B11111001 (0xf9)
#define B11111010 (0xfa)
#define B11111011 (0xfb)
#define B11111100 (0xfc)
#define B11111101 (0xfd)
#define B11111110 (0xfe)
#define B11111111 (0xff)

const DWORD utf8_signature = 0xbfbbef;
const DWORD utf16_be_signature = 0xfffe;
const DWORD utf16_le_signature = 0xfeff;

extern void _log_(int level, const char* fmt, ...);
extern int _getlogstr_(char * szBuf, int nSize, const char* fmt, ...);

extern int sprintf_k(char* buf, size_t len, const char* fmt, ...);

extern KAssertCallBack* g_pAssertCallBack;

template <class T> const T& kmin(const T& t1, const T& t2) { return t1 < t2 ? t1 : t2; }
template <class T> const T& kmax(const T& t1, const T& t2) { return t1 < t2 ? t2 : t1; }

#define _align_size_(n) \
	((n+sizeof(void*)-1) & (~(sizeof(void*)-1)))

//#define NEW(TYPE) new TYPE

#define NEW1(TYPE,p1) new TYPE(p1)

#define NEW2(TYPE,p1,p2) new TYPE(p1,p2)

#define NEW3(TYPE,p1, p2, p3) new TYPE(p1,p2,p3)

#define NEW4(TYPE,p1, p2, p3, p4) new TYPE(p1,p2,p3,p4)

//#define DELETE(p) delete p;

#define NEWARRAY(TYPE, num) new TYPE[num]

#define DELETEARRAY(p) delete [] p;

#ifndef SAFE_FREE
#define SAFE_FREE(a)	if (a) {free(a); (a)=NULL;}
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(a)	if (a) {(a)->Release(); (a)=NULL;}
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(a)	if (a) {delete (a); (a)=NULL;}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(a)		if (a) {delete[] (a); (a)=NULL;}
#define SAFE_DELETE_ARRAY_2(type,a)	if (a) {delete[] (type*)(a); (a)=NULL;}
#endif
#ifndef SAFE_NEW
#define SAFE_NEW(type)				new type
#endif
#ifndef SAFE_NEW_ARRAY
#define SAFE_NEW_ARRAY(type,size)	(new type[size])
#endif

#undef ASSERT
#define NOASSERT

#if defined(WIN32) || defined(WINDOWS)
#define _USER_BREAK_	__debugbreak() //{ do { __asm { int 3 }; } while (0); }
#else
#if defined(__clang__)
#define _USER_BREAK_ //do { asm ("trap"); } while(0); }
#else
#define _USER_BREAK_ { do { asm ( "int $3;" ); } while(0); }
#endif
#endif

#if defined(NOASSERT)
	#define ASSERT_I(cond) \
		if(!(cond)) \
		{ \
			_log_(LOG_FATAL, "ASSERT: %s:%u <%s>", __FILE__,__LINE__,#cond); \
		}
	
	#define ASSERT_I_2(cond,rst) \
		if((cond)) \
		{ \
			rst = TRUE; \
		} \
		else \
		{ \
			rst = FALSE; \
			_log_(LOG_FATAL, "ASSERT: %s:%u <%s>", __FILE__,__LINE__,#cond); \
		}
#else
	#define ASSERT_I(cond) \
		{ \
			if(!(cond)) { _USER_BREAK_ } \
		}

	#define ASSERT_I_2(cond,rst) \
		{ \
			if((cond)) \
			{ \
				rst = TRUE; \
			} \
			else \
			{ \
				rst = FALSE; \
				_USER_BREAK_ \
			} \
		}
#endif

#define ASSERT_RETURN(cond, ret) \
	{ \
		BOOL _assert_b_; \
		ASSERT_I_2(cond, _assert_b_); \
		if(!_assert_b_) return ret; \
	}

#define ASSERT_RETURN_VOID(cond) \
	{ \
		BOOL _assert_b_; \
		ASSERT_I_2(cond, _assert_b_); \
		if(!_assert_b_) return; \
	}

#define DECLARE_CLASS_NAME(__classname) \
	static const char* staticClassName() \
	{ \
	return #__classname; \
	} \

//#undef NDEBUG

#ifndef MACRO
#define MACRO
	#undef NULL
	#undef TRUE
	#undef FALSE
	#undef MAX_PATH
	#undef BLank
	#undef OPERATION
	#undef LOCATION
	#undef EXECUTE
	#undef DEBUG_BREAK
	#undef RELEASE_BREAK
	#undef DEBUGGER
	#undef aSsErT
	#undef assert
	#undef Assert
	#undef ASSERT

#ifndef WINDOWS
	#undef SOCKET_ERROR
	#undef INVALID_SOCKET
	#define SOCKET_ERROR	-1
	#define INVALID_SOCKET  (SOCKET)(~0)
#endif

	#define NULL	0
	
	#define TRUE 1
	#define FALSE 0

	#define MAX_PATH 260
	#define BLank

	#define OPERATION														\
			static_cast<void>( 0 )
	#define LOCATION														\
			"File: %s, Line: %d\n", __FILE__, __LINE__
	#define EXECUTE(statement)												\
			do {															\
				DEBUGGER( OUTPUT( #statement "\n" ) );						\
				statement;													\
			} while( 0 )
	
	#define DEBUG_BREAK		_USER_BREAK_
	#define RELEASE_BREAK	_USER_BREAK_
	
//			DEBUGGER( RELEASE_BREAK )
	/*#define RELEASE_BREAK													\
			do {															\
				OUTPUT( STRING().Printf( LOCATION ) );						\
				if ( ::System::Available() == FALSE ) ABORT();			\
				else System()->Leave( "User breakpoint reached.\n" );	\
			} while( 0 )*/

	#ifdef NDEBUG
		#define DEBUGGER(statement)											\
				OPERATION
	#else
		#define DEBUGGER(statement)											\
				statement
	#endif

	//#define aSsErT(expression)												\
	//		typedef char AsSeRt[ (expression) ? 1 : -1 ];
	//#define assert(expression)												\
	//		DEBUGGER( if ( ! (expression) ) do { __asm { INT 3 } } while ( 0 ) )

	//#ifdef WINDOWS
	//	#define ASSERT(expression)												\
	//			DEBUGGER( if ( ! (expression) ) do { __asm { INT 3 } } while ( 0 ) )
	//#else
	//	#undef Assert
	//	#include   <assert.h>   
	//	#define   ASSERT(x)   assert(x)   
	//#endif 
	#define ASSERT(expression)				\
			{								\
				do							\
				{							\
					if ( ! (expression) )	\
					{						\
						if(NULL !=g_pAssertCallBack)                \
						{											\
							char Infor[256];						\
							sprintf_k(Infor,sizeof(Infor),"ASSERT: %s:%u <%s>", __FILE__,__LINE__,#expression);\
							g_pAssertCallBack->AssertInfor(Infor);									\
						}											\
						{ RELEASE_BREAK; } 	\
					}						\
				}							\
				while(0);					\
			}

	#define ASSERT_ADV(expression,iIndex,pError)				\
			{								\
				do							\
				{							\
					if ( ! (expression) )	\
					{						\
						if(NULL !=g_pAssertCallBack)                \
						{											\
							char Infor[256];						\
							sprintf_k(Infor,sizeof(Infor),"ASSERT: %s:%u <%s>", __FILE__,__LINE__,#expression);\
							g_pAssertCallBack->AssertInfor(Infor,iIndex,pError);									\
						}											\
						{ RELEASE_BREAK; } 	\
					}						\
				}							\
				while(0);					\
			}

	#define Assert ASSERT
	#define aSsErT ASSERT
	#define assert ASSERT

	#define g_StrCpyLen(dest, src, size)	\
	{								\
		strncpy(dest, src, size);		\
		dest[size - 1] = '\0';			\
	}

	#define g_StrLower(str)									\
	{												\
		char c;										\
		for (int i = 0; 0 != (c = str[i]); i++)		\
	{											\
		if (c >= 'A' && c <= 'Z')				\
		str[i] = c + ('a' - 'A');			\
	}											\
	}

	/**
	 * 取类型T的缺省值
	 * 数值类型缺省值为0
	 * 指针缺省值为NULL
	 * 其它类型使用空构造函数
	 * 用法:
	 *      int x = _type_default_value_<int>::get();
	 *
	 * 在KMapByVector中使用此类来初始化不存在的节点
	 *
	 */
	template <typename T> struct _type_default_value_
	{
		static const bool needFillDefaultValue = true;
		static const T& get() { static T v; return v; }
	};
	template <typename T> struct _type_default_value_<T*>
	{
		static const bool needFillDefaultValue = true;
		static T* get() { return NULL; }
	};
	template <> struct _type_default_value_<void*>
	{
		static const bool needFillDefaultValue = true;
		static void* get() { return NULL; }
	};
	template <> struct _type_default_value_<char>
	{
		static const bool needFillDefaultValue = true;
		static char get() { return 0; }
	};
	template <> struct _type_default_value_<unsigned char>
	{
		static const bool needFillDefaultValue = true;
		static int get() { return 0; }
	};
	template <> struct _type_default_value_<short>
	{
		static const bool needFillDefaultValue = true;
		static short get() { return 0; }
	};
	template <> struct _type_default_value_<unsigned short>
	{
		static const bool needFillDefaultValue = true;
		static unsigned short get() { return 0; }
	};
	template <> struct _type_default_value_<int>
	{
		static const bool needFillDefaultValue = true;
		static int get() { return 0; }
	};
	template <> struct _type_default_value_<unsigned int>
	{
		static const bool needFillDefaultValue = true;
		static int get() { return 0; }
	};
	template <> struct _type_default_value_<float>
	{
		static const bool needFillDefaultValue = true;
		static float get() { return 0.0; }
	};
	template <> struct _type_default_value_<double>
	{
		static const bool needFillDefaultValue = true;
		static double get() { return 0.0; }
	};
	template <> struct _type_default_value_<long long>
	{
		static const bool needFillDefaultValue = true;
		static long long get() { return 0; }
	};
	template <> struct _type_default_value_<unsigned long long>
	{
		static const bool needFillDefaultValue = true;
		static unsigned long long get() { return 0; }
	};

	// 判断类型是否为简单类型
	template <typename T> struct _is_simple_type_
	{
		static const bool value = sizeof(T)<=sizeof(void*) ? true:false;
	};
	template <typename T1,typename T2> struct _is_simple_type_<KPair<T1,T2> > 
	{ 
		static const bool value = _is_simple_type_<T1>::value && _is_simple_type_<T2>::value ? true : false;
	};
	template <typename T1,typename T2,typename T3> struct _is_simple_type_<KTriple<T1,T2,T3> > 
	{ 
		static const bool value = _is_simple_type_<T1>::value && _is_simple_type_<T2>::value && _is_simple_type_<T3>::value ? true : false;
	};
	template <typename T1,typename T2,typename T3,typename T4> struct _is_simple_type_<KQuadruple<T1,T2,T3,T4> > 
	{ 
		static const bool value = _is_simple_type_<T1>::value && _is_simple_type_<T2>::value && _is_simple_type_<T3>::value && _is_simple_type_<T4>::value ? true : false;
	};
	template <typename T1,typename T2,typename T3,typename T4,typename T5> struct _is_simple_type_<KQuintuple<T1,T2,T3,T4,T5> > 
	{ 
		static const bool value = _is_simple_type_<T1>::value && _is_simple_type_<T2>::value && _is_simple_type_<T3>::value && _is_simple_type_<T4>::value && _is_simple_type_<T5>::value ? true : false;
	};
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6> struct _is_simple_type_<KSextuple<T1,T2,T3,T4,T5,T6> > 
	{ 
		static const bool value = _is_simple_type_<T1>::value && _is_simple_type_<T2>::value && _is_simple_type_<T3>::value && _is_simple_type_<T4>::value && _is_simple_type_<T5>::value && _is_simple_type_<T6>::value ? true : false;
	};

	template <> struct _is_simple_type_<INT64>
	{
		static const bool value = true;
	};
	template <> struct _is_simple_type_<UINT64>
	{
		static const bool value = true;
	};
	template <> struct _is_simple_type_<double>
	{
		static const bool value = true;
	};
	

	#define DECLARE_SIMPLE_TYPE(T) \
		template <> struct _is_simple_type_<T> \
		{ \
			static const bool value = true; \
		};

	struct _trueType_
	{
	};
	struct _falseType_
	{
	};

	//根据条件选取类型
	template <bool C,class A,class B> struct _type_switch_
	{	
	};
	template <class A,class B> struct _type_switch_<true, A, B>
	{
		typedef A type;
	};
	template <class A,class B> struct _type_switch_<false, A, B>
	{
		typedef B type;
	};

	// 判断两个类型是否一样
	template <typename A,typename B> struct is_same_type
	{
		static const bool value = false;
	};
	template <typename A> struct is_same_type<A,A>
	{
		static const bool value = true;
	};

	template <int iNum1,int iNum2> struct _isLow_
	{
		static const bool Value = (iNum1 < iNum2)? 1 : 0; 
	};

	// Base^iNum
	template <DWORD iBase,DWORD iNum> struct _powCore_
	{
		static const DWORD Value = (!_isLow_< 0,(int)iNum>::Value)? 1 : (iBase*(_powCore_<_isLow_<0,(int)(iNum-1)>::Value*iBase,iNum-1>::Value));
	};

	template <> struct _powCore_<0,(DWORD)-1>
	{
		static const DWORD Value = 0;
	};

	// log(iBase,iNum)
	template <DWORD iBase,DWORD iNum> struct _mathLog_
	{
		static const DWORD Value = (DWORD)((int)(_mathLog_< _isLow_<0,(int)(iNum/iBase)>::Value*iBase,iNum/iBase>::Value)+1);
		static const bool  Exact = (_powCore_<iBase,Value>::Value == iNum);
	};

	template <> struct _mathLog_<0,1>
	{
		static const DWORD Value = 0;
		static const bool  Exact = 1;
	};

	template <> struct _mathLog_<0,0>
	{
		static const DWORD Value = -1;
		static const bool  Exact = 0;
	};

	template <typename TBASENUM,typename USEBITWISE>
	struct _Division_
	{
		static inline DWORD Execution(DWORD iNum)
		{
			return iNum/TBASENUM::value;
		}
	};

	template <typename TBASENUM>
	struct _Division_<TBASENUM,_trueType_>
	{
		static inline DWORD Execution(DWORD iNum)
		{
			return (iNum>>_mathLog_<2,TBASENUM::value>::Value);
		}
	};

	template <typename TBASENUM>
	struct NumAlignment
	{
		static const DWORD addValue = TBASENUM::value-1;
		typedef typename _type_switch_< _mathLog_<2,TBASENUM::value>::Exact,_trueType_,_falseType_>::type isBitwise;
		static DWORD CalBlockCount(DWORD iNum){return _Division_<TBASENUM,isBitwise>::Execution((iNum+addValue));}
	};


#define CHECK_DEFAULT_ALIGNMENT() \
	struct __check_default_alignment__ \
	{ \
		struct inner {char a; void* b; }; \
		enum { __alignment__ = (sizeof(inner) == sizeof(void*)*2 ? 1 : -1) }; \
		char c[__alignment__]; \
	};

	template <typename T> struct __is_string__
	{
		static const BOOL value = FALSE;
	};
	template <> struct __is_string__<char*>
	{
		static const BOOL value = TRUE;
	};
	template <> struct __is_string__<const char*>
	{
		static const BOOL value = TRUE;
	};

#endif
#endif