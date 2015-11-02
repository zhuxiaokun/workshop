
// Modify by ZhangGuobiao at 2009.10.07.
//////////////////////////////////////////////////////////////////////////

#ifndef _K_LUA_WRAP_H_
#define _K_LUA_WRAP_H_

#pragma once
#include <System/KPlatform.h>
#include <System/Misc/StrUtil.h>
#include "CppTable.h"
#include <System/KType.h>

#pragma   warning(disable:4251)
#if defined(WIN32) || defined(WINDOWS)
#include <typeinfo.h>
#else
#include <typeinfo>
#endif

#include <string.h>
#include <System/KType.h>
#include <System/Collections/KSDList.h>
#include <System/Collections/KMapByVector.h>
#include <System/Collections/KHashTable.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Patterns/KSingleton.h>
#include <System/Collections/KString.h>
#include "KParamPackage.h"

//#if defined(LUA_BUILD_AS_DLL)
//	#if defined(LUA_CORE) || defined(LUA_LIB)
//		#define LUAWARP_API __declspec(dllexport)
//	#else
//		#define LUAWARP_API __declspec(dllimport)
//	#endif
//#else
//	#define LUAWARP_API
//#endif

#if defined(__cplusplus) && (defined(WIN32) || defined(__clang__))
extern "C"
{
#endif // WIN32

#include "Lua/lauxlib.h"
#include "Lua/lua.h"
#include "Lua/lualib.h"
#include "Lua/luaconf.h"
#include "Lua/lstate.h"
#include "Lua/lundump.h"
#include "addon/ThirdpartyLib.h"
#include "luacompiler.h"

#if defined(__cplusplus) && (defined(WIN32) || defined(__clang__))
}
#endif // WIN32

///*
//KLuaWrap类,本类实现了对lua基本操作的封装,包括
//调用脚本中的函数,向Lua中输出对象,以及断点调试
//等功能。
//
//使用方法：
//定义唯一的 KLuaWrap 对象
//1、通过 KLuaWrap::InitScript() 方法完成初始化即可使用
//2、通过 KLuaWrap::DoScriptFunc()方法执行lua函数
//3、通过 KLuaWrap::LoadScripts() 方法载入lua脚本文件
//4、通过 KLuaWrap::doScripts() 方法执行脚本
//5、通过 KLuaWrap::releaseScript() 方法释放不用的脚本
//
//6、可通过 [] 访问或设置脚本中的任何值
//如：
//
//KLuaWrap lw;
//lw.InitScript();
//
//lw["t1"] = TABLE_;
//建立了一个名为t1的空表
//
//lw["t1"]["x"] = 100;
//lw["t1"]["y"] = 200;
//lw["t1"]["name"] = "test";
//给表中的x,y,name赋值
//如同lua中的 t1={x=100, y=200, name="test"}
//
//7、通过[]访问脚本中的数据:
//int x = lw["t1"]["x"];
//int y = lw["t1"]["y"];
//char* name = lw["t1"]["name"];
//
//8、同过[]调用函数,只能调用全局函数,参数最多5个,返回值一个可任意类型
//int r = lw[ts](1,2);
//如需多返回值,使用DoScriptFunc方法
//
//9、输出函数到脚本
//有函数 int add(int a, int b){ return a+b; }
//可如此输出 lw["add"] = add;
//然后在脚本中 n = add(100, 200)即可
//
//10、输出对象到脚本的方法：
//class CTestLua
//{
//public:
//CTestLua(){ m=0; }
//int add(int a, int b)
//{
//m = a + b;
//return m;
//}
//
//int m;
//
//BeginDefLuaClass(CTestLua) // 定义类
//DefMemberFunc(c)		// 成员函数
//DefMemberVar(m)			// 成员变量,在lua中只读
//EndDef
//};
//如上面的类定义
//
//在任何地方执行
//lw.RegisterClass<CTestLua>(); 即可在lua中使用CTestLua对象
//
//lua中脚本如下：
//t=CTestLua()
//t:add(1,2)
//
//可以给全局变量赋予对象值,但是不能给表中的成员赋对象值
//给表中的成员赋对象值仅可用于读取,不能在lua中调用对象的成员
//如：
//CTestLua t;
//lw["t1"] = t;
//
//这样在lua中可以使用t1:add()方法
//
//但是
//CTestLua t;
//lw["tb"] = TABLE_;
//lw["tb"]["t1"] = t;
//此时,在lua中调用 tb.t1:add()是没有效果的。
//但是可以同过 lw["tb"]["t1"] 取出 t 的指针
//
//11、成员函数多返回值方法
//注册和声明和普通成员函数一样
//只是在函数返回时,先把多个返回值用LuaPush压栈
//在将返回值个数压栈,函数返回MULT_RET
//如某成员函数
//int k()
//{
//
//LuaPush(100);
//LuaPush(200);
//LuaPush(300);
//LuaPush("400");
//// 以上四行将返回值压栈
//LuaPush(4); // 本行将返回值个数压栈
//return MULT_RET; // 返回MULT_RET告诉luawrap是多返回值函数
//}
//
//关于 register 生成的 metatable:
//由于一些扩展针对 class_type 进行，所以要对它们进行一些必要的数据扩展。考虑到对外接口都是适用 string 类型作为Key
//扩展使用 使用 key 为 number 进行，并且要进行  定义.放在 cpptable 中
//*/

#define lwp_error -1
#define lwp_success 0
#define MULT_RET 0x1fff9876
#define TABLE_ 0x1fff9875
#define NILVALUE 0x1fff9874

typedef System::Collections::KString<512> LuaString;
typedef System::Collections::KString<1024> LongLuaString;

class KLuaWrap;
extern "C" KLuaWrap* g_pLuaWrapper;
#define LuaWraper (*g_pLuaWrapper)

#define export_enum_to_lua(x) LuaWraper[#x] = (int)x;
#define export_const_to_lua(x) LuaWraper[#x] = x;

LUAWARP_API	KLuaWrap &	GetLuaSystem();
LUAWARP_API	void luaSetGlobalLogger(ILogger* logger);
LUAWARP_API	void luaSetLuaLogger(ILogger* logger);

namespace LMREG
{
	void	enum_stack(lua_State *L);
	//LUAWARP_API int on_error(lua_State *L);
	void LUAWARP_API print_error(lua_State *L, const char* fmt, ...);

	// class helper
	LUAWARP_API int meta_get(lua_State *L);
	LUAWARP_API int meta_set(lua_State *L);
	LUAWARP_API void push_meta(lua_State *L, const char* name);

	// user data in global lua state
	// 有关lua的全局信息可以存在这个结构里面
	struct LUAWARP_API KLuaState_UD
	{
		int m_errfunc;	// key of error funciton in lua registry

		///
		KLuaState_UD():m_errfunc(0)
		{

		}
		// 在 LuaWrap 创建 L 的时候赋值 this，使得通过 L 也可找到luawarp，便于共享 luawrap 实例
		KLuaWrap* m_this;
	};

	// 在不知道object类型的情况下，往脚本中传入对象用的结构
	struct KPtrObjcet
	{
		void*		pObject;
		LuaString	strObjectName;
		KPtrObjcet():pObject(NULL) { }
		KPtrObjcet(void* p, const char* n):pObject(p),strObjectName(n) { }
	};

#define lua_loadregistryref(L,ref) \
	{ \
		lua_rawgeti(L, LUA_REGISTRYINDEX, ref); \
	}

inline int lua_geterrfunc(lua_State* L)
{
	LMREG::KLuaState_UD& ud = *(LMREG::KLuaState_UD*)G(L)->ud;
	lua_rawgeti(L, LUA_REGISTRYINDEX, ud.m_errfunc);
	return lua_gettop(L);
}

#define lua_getobjudata(L,T,obj) \
	{ \
		lua_rawgeti(L, LUA_REGISTRYINDEX, KBaseLuaClass<T>::m_udataRef); \
		lua_pushlightuserdata(L, (void*)(T*)obj); \
		lua_gettable(L, -2); \
		lua_remove(L, -2); \
	}

	// dynamic type extention
	struct lua_value
	{
		virtual void to_lua(lua_State *L) = 0;
	};

	// 特殊返回值标志码
	const int cn_MtlRetFlag = 0x1fff9876;	// 多返回值
	const int cn_TabRetFlag = 0x1fff9877;	// 返回表
	LUAWARP_API int RetValue_(lua_State *L);

	// type trait
	template<typename T> struct class_name;
	struct table;
	struct l_func;
	struct l_stackval;

	template<bool C, typename A, typename B> struct if_ {};
	template<typename A, typename B>		struct if_<true, A, B> { typedef A type; };
	template<typename A, typename B>		struct if_<false, A, B> { typedef B type; };

	template<typename A>
	struct is_ptr { static const bool value = false; };
	template<typename A>
	struct is_ptr<A*> { static const bool value = true; };

	template<typename A>
	struct is_ref { static const bool value = false; };
	template<typename A>
	struct is_ref<A&> { static const bool value = true; };

	template<typename A> struct remove_const { typedef A type; };
	template<typename A> struct remove_const<const A> { typedef A type; };

	template<typename A> struct remove_ref { typedef A type; };
	template<typename A> struct remove_ref<A&> { typedef A type; };

	template<typename A>
	struct base_type { typedef A type; };
	template<typename A>
	struct base_type<A*> { typedef A type; };
	template<typename A>
	struct base_type<A&> { typedef A type; };

	template<typename A>
	struct class_type { typedef typename remove_const<typename base_type<A>::type>::type type; };

	/////////////////////////////////
	enum { no = 1, yes = 2 }; 
	typedef char (& no_type )[no]; 
	typedef char (& yes_type)[yes]; 

	struct int_conv_type { int_conv_type(int); }; 

	no_type int_conv_tester (...); 
	yes_type int_conv_tester (int_conv_type); 

	no_type vfnd_ptr_tester (const volatile char *); 
	no_type vfnd_ptr_tester (const volatile short *); 
	no_type vfnd_ptr_tester (const volatile int *); 
	no_type vfnd_ptr_tester (const volatile long *); 
	no_type vfnd_ptr_tester (const volatile double *); 
	no_type vfnd_ptr_tester (const volatile float *); 
	no_type vfnd_ptr_tester (const volatile bool *); 
	yes_type vfnd_ptr_tester (const volatile void *); 

	template <typename T> T* add_ptr(T&); 

	template <bool C> struct bool_to_yesno { typedef no_type type; }; 
	template <> struct bool_to_yesno<true> { typedef yes_type type; }; 

	template <typename T> 
	struct is_enum 
	{ 
		static T arg; 
		static const bool value = ( (sizeof(int_conv_tester(arg)) == sizeof(yes_type)) && (sizeof(vfnd_ptr_tester(add_ptr(arg))) == sizeof(yes_type)) ); 
	}; 

	template <typename T> struct simple_data_type			{ static const bool value = is_enum<T>::value; };
	template <typename T> struct simple_data_type<T*>		{ static const bool value = true; };
	template <> struct simple_data_type<bool>				{ static const bool value = true; };
	template <> struct simple_data_type<char>				{ static const bool value = true; };
	template <> struct simple_data_type<short>				{ static const bool value = true; };
	template <> struct simple_data_type<int>				{ static const bool value = true; };
	template <> struct simple_data_type<long>				{ static const bool value = true; };
	template <> struct simple_data_type<long long>			{ static const bool value = true; };
	template <> struct simple_data_type<float>				{ static const bool value = true; };
	template <> struct simple_data_type<double>				{ static const bool value = true; };
	template <> struct simple_data_type<unsigned char>		{ static const bool value = true; };
	template <> struct simple_data_type<unsigned short>		{ static const bool value = true; };
	template <> struct simple_data_type<unsigned int>		{ static const bool value = true; };
	template <> struct simple_data_type<unsigned long>		{ static const bool value = true; };
	template <> struct simple_data_type<unsigned long long>	{ static const bool value = true; };
	
	/////////////////////////////////

	// from lua
	template<typename T>
	struct void2val { static T invoke(void* input){ return *(T*)input; } };
	template<typename T>
	struct void2ptr { static T* invoke(void* input){ return (T*)input; } };
	template<typename T>
	struct void2ref { static T& invoke(void* input){ return *(T*)input; } };

	template<typename T>  
	struct void2type
	{
		static T invoke(void* ptr)
		{
			return	if_<is_ptr<T>::value
				,void2ptr<typename base_type<T>::type>
				,typename if_<is_ref<T>::value
				,void2ref<typename base_type<T>::type>
				,void2val<typename base_type<T>::type>
				>::type
			>::type::invoke(ptr);
		}
	};

	struct user
	{
		user(void* p) : m_p(p) {}
		virtual ~user() {}
		void* m_p;
	};

	template<typename T>  
	struct user2type { static T invoke(lua_State *L, int index) { return void2type<T>::invoke(lua_touserdata(L, index)); } };

	template<typename T>
	struct lua2enum { static T invoke(lua_State *L, int index) { return (T)(int)lua_tonumber(L, index); } };

	template <typename T> struct lua2ptr
	{
		static T invoke(lua_State *L, int index)
		{
			if(!lua_isuserdata(L,index))
			{
				if(lua_isnoneornil(L, index)) return NULL;
				lua_pushstring(L, "no class at first argument. (forgot ':' expression ?)");
				lua_error(L);
			}
			return void2type<T>::invoke(user2type<user*>::invoke(L,index)->m_p);
		}
	};

	template<typename T>
	struct lua2object
	{ 
		static T invoke(lua_State *L, int index)
		{
			if(!lua_isuserdata(L,index))
			{
				lua_pushstring(L, "no class at first argument. (forgot ':' expression ?)");
				lua_error(L);
			}
			return void2type<T>::invoke(user2type<user*>::invoke(L,index)->m_p); 
		} 
	};

	template<typename T>
		T lua2type(lua_State *L, int index)
	{
		return	if_<is_enum<T>::value
			,lua2enum<T>
			,typename if_< is_ptr<T>::value, lua2ptr<T>, lua2object<T> >::type
		>::type::invoke(L, index);
	}

	template<typename T>
	struct val2user : user
	{
		val2user() : user(new T) {}

		template<typename T1>
		val2user(T1 t1) : user(new T(t1)) {}

		template<typename T1, typename T2>
		val2user(T1 t1, T2 t2) : user(new T(t1, t2)) {}

		template<typename T1, typename T2, typename T3>
		val2user(T1 t1, T2 t2, T3 t3) : user(new T(t1, t2, t3)) {}

		template<typename T1, typename T2, typename T3, typename T4>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4) : user(new T(t1, t2, t3,t4)) {}

		template<typename T1, typename T2, typename T3, typename T4, typename T5>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) : user(new T(t1, t2, t3, t4, t5)) {}

		template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) : user(new T(t1, t2, t3, t4, t5, t6)) {}

		template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7) : user(new T(t1, t2, t3, t4, t5, t6, t7)) {}

		template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8) : user(new T(t1, t2, t3, t4, t5, t6, t7, t8)) {}

		template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9) : user(new T(t1, t2, t3, t4, t5, t6, t7, t8, t9)) {}

		template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10) : user(new T(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10)) {}

		template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11) : user(new T(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11)) {}

		template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
		val2user(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8, T9 t9, T10 t10, T11 t11, T12 t12) : user(new T(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, t11, t12)) {}

		~val2user() { delete ((T*)m_p); }
	};

	template<typename T>
	struct ptr2user : user
	{
		ptr2user(T* t) : user((void*)t) {}
	};

	struct voidPtr2user : public user
	{
		voidPtr2user(void* p):user(p) { }
	};

	template<typename T>
	struct ref2user : user
	{
		ref2user(T& t) : user(&t) {}
	};

	// to lua

	/// 为每一个 type 生成一个类型，用来保存 ptr2userdata数据
	// 1 可以使 ptr 和 已创建过的 userdata 匹配,这样就可以配合 脚本自定义数据持久化的操作(extendTable)。
	// 2 节省创建新 userdata 带来的开销
	// 3 目前只负责 ptr 类型的。
	LUAWARP_API void pushPtrObject(lua_State *L, void* input, const char* metaname);

	template<typename T> struct val2lua
	{
		static void invoke(lua_State *L, T& input)
		{
			new (lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(input);
			push_meta(L, class_name<typename class_type<T>::type>::name());
			lua_setmetatable(L, -2);
		}
		static void invoke(lua_State *L, T& input, const char* name)
		{
			new (lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(input);
			push_meta(L, name);
			lua_setmetatable(L, -2);
		}
	};
	template<typename T> struct ptr2lua
	{
		static void invoke(lua_State *L, T* input)
		{
			if(input)
			{
				//new (lua_newuserdata(L, sizeof(ptr2user<T>))) ptr2user<T>(input);
				//push_meta(L, class_name<typename class_type<T>::type>::name());
				//lua_setmetatable(L, -2);
				//class_name<remove_const<T>::type>::pushPtrObject<T>(L, input);
				pushPtrObject(L, (void*)input, class_name<typename remove_const<T>::type>::name());
			}
			else
			{
				lua_pushnil(L);
			}
		}
		static void invoke(lua_State *L, T* input, const char* name)
		{
			if(input)
			{
				//new (lua_newuserdata(L, sizeof(ptr2user<T>))) ptr2user<T>(input);
				//push_meta(L, name);
				//lua_setmetatable(L, -2);
				//class_type_userdateTable<T>::pushPtrObject(L, input, name);
				//class_name<remove_const<T>::type>::pushPtrObject<T>(L, input, name);
				pushPtrObject(L, (void*)input, name);
			}
			else
			{
				lua_pushnil(L);
			}
		}
	};
	struct voidPtr2lua
	{
		static void invoke(lua_State* L, void* p, const char* typeName)
		{
			if(p)
			{
				new (lua_newuserdata(L, sizeof(voidPtr2user))) voidPtr2user(p);
				push_meta(L, typeName);
				lua_setmetatable(L, -2);
			}
			else
			{
				lua_pushnil(L);
			}
		}
	};
	template<typename T> struct ref2lua
	{
		static void invoke(lua_State *L, T& input)
		{ 
//			new (lua_newuserdata(L, sizeof(ref2user<T>))) ref2user<T>(input);
//			push_meta(L, class_name<typename class_type<T>::type>::name());
			//			lua_setmetatable(L, -2);
			//class_type_userdateTable<T>::pushPtrObject(L, &input, class_name<typename class_type<T>::type>::name());
			pushPtrObject(L, &input, class_name<typename remove_const<T>::type>::name());
		}
		static void invoke(lua_State *L, T& input, const char* name)
		{ 
//			new (lua_newuserdata(L, sizeof(ref2user<T>))) ref2user<T>(input);
//			push_meta(L, name);
			//			lua_setmetatable(L, -2);
			//class_type_userdateTable<T>::pushPtrObject(L, &input, name);
			pushPtrObject(L, &input, name);
		}
	};

	template<typename T>
	struct enum2lua
	{
		static void invoke(lua_State *L, T val)
		{
			lua_pushnumber(L, (int)val);
		}
	};

	template<typename T>
	struct object2lua 
	{ 
		static void invoke(lua_State *L, T val) 
		{ 
			if_<is_ptr<T>::value
				,ptr2lua<typename base_type<T>::type>
				,typename if_<is_ref<T>::value
				,ref2lua<typename base_type<T>::type>
				,val2lua<typename base_type<T>::type>
				>::type
			>::type::invoke(L, val);

			//push_meta(L, class_name<typename class_type<T>::type>::name());
			//lua_setmetatable(L, -2);
		} 
        //add by yangbin for object type
		static void invoke(lua_State *L, T val, const char* typeName)
		{ 
			if_<is_ptr<T>::value
				,ptr2lua<typename base_type<T>::type>
				,typename if_<is_ref<T>::value
				,ref2lua<typename base_type<T>::type>
				,val2lua<typename base_type<T>::type>
				>::type
			>::type::invoke(L, val, typeName);

			//push_meta(L, typeName);
			//lua_setmetatable(L, -2);
		} 
		//
	};

	template<typename T>
		void type2lua(lua_State *L, T val)
	{
		if_<is_enum<T>::value
			,enum2lua<T>
			,object2lua<T>
		>::type::invoke(L, val);
	}
    // add by yangbin for obj type
	template<typename T>
	void type2lua(lua_State *L, T val, const char* typeName)
	{
		if_<is_enum<T>::value
			,enum2lua<T>
			,object2lua<T>
		>::type::invoke(L, val, typeName);
	}
	//
	// get value from cclosure
	template<typename T>  
		T upvalue_(lua_State *L)
	{
		return user2type<T>::invoke(L, lua_upvalueindex(1));
	}

	// read a value from lua stack 
	template<typename T>  
	T read(lua_State *L, int index)				{ return lua2type<T>(L, index); }

	template<>	LUAWARP_API void*				read(lua_State *L, int index);
	template<>	LUAWARP_API const void*			read(lua_State *L, int index);

	template<>	LUAWARP_API char*				read(lua_State *L, int index);
	template<>	LUAWARP_API const char*			read(lua_State *L, int index);
	template<>	LUAWARP_API LuaString			read(lua_State *L, int index);
	template<>	LUAWARP_API LongLuaString		read(lua_State *L, int index);
	template<>	LUAWARP_API char				read(lua_State *L, int index);
	template<>	LUAWARP_API unsigned char		read(lua_State *L, int index);
	template<>	LUAWARP_API short				read(lua_State *L, int index);
	template<>	LUAWARP_API unsigned short		read(lua_State *L, int index);
	template<>	LUAWARP_API long				read(lua_State *L, int index);
	template<>	LUAWARP_API unsigned long		read(lua_State *L, int index);
	template<>	LUAWARP_API int					read(lua_State *L, int index);
	template<>	LUAWARP_API unsigned int		read(lua_State *L, int index);
	template<>	LUAWARP_API float				read(lua_State *L, int index);
	template<>	LUAWARP_API double				read(lua_State *L, int index);
	template<>	LUAWARP_API bool				read(lua_State *L, int index);
	template<>	LUAWARP_API void				read(lua_State *L, int index);
	template<>	LUAWARP_API INT64				read(lua_State *L, int index);
	template<>	LUAWARP_API UINT64				read(lua_State *L, int index);
	template<>	LUAWARP_API table				read(lua_State *L, int index);
	template<>  LUAWARP_API CppTable			read(lua_State *L, int index);
	template<>  LUAWARP_API l_func				read(lua_State *L, int index);
	template<>  LUAWARP_API l_stackval			read(lua_State *L, int index);
	template<> LUAWARP_API INT64 read(lua_State *L, int index);
	template<> LUAWARP_API UINT64 read(lua_State *L, int index);
	template<> LUAWARP_API Identity32 read(lua_State *L, int index);
	template<> LUAWARP_API Identity64 read(lua_State *L, int index);

	// push a value to lua stack 
	template<typename T> void push(lua_State *L, T ret)
	{
		type2lua<T>(L, ret);
	}

	template <typename T> void push_ref(lua_State *L, T& ret)
	{
		object2lua<T&>::invoke(L, ret);
	}
    
	// add by yangbin for obj type
	template<typename T>  
	void push(lua_State *L, T ret, const char* typeName)
	{
		type2lua<T>(L, ret, typeName);
	}

	template<>	LUAWARP_API void push(lua_State *L, void* ret);
	template<>	LUAWARP_API void push(lua_State *L, const void* ret);

	template<>	LUAWARP_API void push(lua_State *L, char ret);
	template<>	LUAWARP_API void push(lua_State *L, unsigned char ret);
	template<>	LUAWARP_API  void push(lua_State *L, short ret);
	template<>	LUAWARP_API  void push(lua_State *L, unsigned short ret);
	template<>	LUAWARP_API  void push(lua_State *L, long ret);
	template<>	LUAWARP_API  void push(lua_State *L, unsigned long ret);
	template<>	LUAWARP_API  void push(lua_State *L, int ret);
	template<>	LUAWARP_API  void push(lua_State *L, unsigned int ret);
	template<>	LUAWARP_API  void push(lua_State *L, float ret);
	template<>	LUAWARP_API  void push(lua_State *L, double ret);
	template<>	LUAWARP_API  void push(lua_State *L, char* ret);
	template<>	LUAWARP_API  void push(lua_State *L, const char* ret);
	template<>	LUAWARP_API  void push(lua_State *L, LuaString lstr);
	template<>	LUAWARP_API  void push(lua_State *L, LuaString& lstr);
	template<>	LUAWARP_API  void push(lua_State *L, const LuaString& lstr);

	template<>	LUAWARP_API  void push(lua_State *L, LongLuaString lstr);
	template<>	LUAWARP_API  void push(lua_State *L, LongLuaString& lstr);
	template<>	LUAWARP_API  void push(lua_State *L, const LongLuaString& lstr);

	template<>	LUAWARP_API  void push(lua_State *L, bool ret);
	template<>	LUAWARP_API  void push(lua_State *L, lua_value* ret);
	template<>	LUAWARP_API  void push(lua_State *L, table ret);
	template<>	LUAWARP_API  void push(lua_State *L, CppTable ret);
	template<>  LUAWARP_API  void push(lua_State *L, KPtrObjcet PtrObj );

	template<> LUAWARP_API void push(lua_State *L, INT64 ret);
	template<> LUAWARP_API void push(lua_State *L, UINT64 ret);
	template<> LUAWARP_API void push(lua_State *L, Identity32 ret);
	template<>	LUAWARP_API  void push(lua_State *L, l_stackval ret);
	template<> LUAWARP_API void push(lua_State *L, Identity64 ret);
	template<> LUAWARP_API void push(lua_State *L, variant ret);

	// pop a value from lua stack
	template<typename T>  
	T pop(lua_State *L) { T t = read<T>(L, -1); lua_pop(L, 1); return t; }

	template<>	LUAWARP_API void	pop(lua_State *L);
	template<>	LUAWARP_API table	pop(lua_State *L);

	// 从堆栈取值并用指定值恢复堆栈
	template<typename T>
	T retset(lua_State *L, int nTop) { T t = read<T>(L, -1); lua_settop(L, nTop); return t; }

	template<>	void LUAWARP_API retset<void>(lua_State *L, int nTop);

	// 解析表函数名，支持表嵌套
	int getsctnum(const char* src, int ch);
	char* getsctstr(const char* src, int ch, int pos, char* dst);

	// 有返回值的函数构造器
	template<	typename RVal,
				typename T1=void,
				typename T2=void,
				typename T3=void,
				typename T4=void,
				typename T5=void,
				typename T6=void,
				typename T7=void,
				typename T8=void,
				typename T9=void,
				typename T10=void,
				typename T11=void,
				typename T12=void>
	struct functor
	{
		static int invoke(lua_State *L)
		{
			push(	L,
					upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)>(L)(
						read<T1>(L,1),
						read<T2>(L,2),
						read<T3>(L,3),
						read<T4>(L,4),
						read<T5>(L,5),
						read<T6>(L,6),
						read<T7>(L,7),
						read<T8>(L,8),
						read<T9>(L,9),
						read<T10>(L,10),
						read<T11>(L,11),
						read<T12>(L,12)));
			return RetValue_(L);
		}
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5
		, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
	struct functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)>(L)
			(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8)
			,read<T9>(L,9),read<T10>(L,10),read<T11>(L,11))); return RetValue_(L); }
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5
		, typename T6, typename T7, typename T8, typename T9, typename T10>
	struct functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>(L)
			(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8)
			,read<T9>(L,9),read<T10>(L,10))); return RetValue_(L); }
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5
		, typename T6, typename T7, typename T8, typename T9>
	struct functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>(L)
			(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8)
			,read<T9>(L,9))); return RetValue_(L); }
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5
		, typename T6, typename T7, typename T8>
	struct functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L)
			(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8)
			)); return RetValue_(L); }
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5
		, typename T6, typename T7>
	struct functor<RVal,T1,T2,T3,T4,T5,T6,T7>
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6,T7)>(L)
			(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7))); return RetValue_(L); }
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	struct functor<RVal,T1,T2,T3,T4,T5,T6>
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5,T6)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6))); return RetValue_(L); }
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
	struct functor<RVal,T1,T2,T3,T4,T5>
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5))); return RetValue_(L); }
	};

	template<typename RVal, typename T1, typename T2, typename T3, typename T4>
	struct functor<RVal,T1,T2,T3,T4> 
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2,T3,T4)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4))); return RetValue_(L); }
	};

	template<typename RVal,
			typename T1, 
			typename T2, 
			typename T3>
	struct functor<RVal,T1,T2,T3> 
	{
		static int invoke(lua_State *L)
		{
			push
			(
				L,
				upvalue_<RVal(*)(T1,T2,T3)>(L)
				(
					read<T1>(L,1),
					read<T2>(L,2),
					read<T3>(L,3)
				)
			); 
			return RetValue_(L);
		}
	};

	template<typename RVal, typename T1, typename T2>
	struct functor<RVal,T1,T2> 
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1,T2)>(L)(read<T1>(L,1),read<T2>(L,2))); return RetValue_(L); }
	};

	template<typename RVal, typename T1>
	struct functor<RVal,T1> 
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)(T1)>(L)(read<T1>(L,1))); return RetValue_(L); }
	};

	template<typename RVal>
	struct functor<RVal>
	{
		static int invoke(lua_State *L) { push(L,upvalue_<RVal(*)()>(L)()); return RetValue_(L); }
	};


	// 无返回值函数构造器
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9
		, typename T10, typename T11, typename T12>
	struct functor<void, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5, T6, T7, T8, T9, T10,T11,T12)>(L)
			(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8)
			,read<T9>(L,9),read<T10>(L,10),read<T11>(L,11),read<T12>(L,12)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9
		, typename T10, typename T11>
	struct functor<void, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5, T6, T7, T8, T9, T10,T11)>(L)
			(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8)
			,read<T9>(L,9),read<T10>(L,10),read<T11>(L,11)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9
		, typename T10>
	struct functor<void, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5, T6, T7, T8, T9, T10)>(L)
			(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8)
			,read<T9>(L,9),read<T10>(L,10)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9
	>
	struct functor<void, T1, T2, T3, T4, T5, T6, T7, T8, T9>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5, T6, T7, T8, T9)>(L)
			(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8)
			,read<T9>(L,9)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	struct functor<void, T1, T2, T3, T4, T5, T6, T7, T8>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5, T6, T7, T8)>(L)
			(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7),read<T8>(L,8)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	struct functor<void, T1, T2, T3, T4, T5, T6, T7>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5, T6, T7)>(L)
			(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6),read<T7>(L,7)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	struct functor<void, T1, T2, T3, T4, T5, T6>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5, T6)>(L)
			(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5),read<T6>(L,6)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5>
	struct functor<void, T1, T2, T3, T4, T5>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4,T5)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4),read<T5>(L,5)); return 0; }
	};

	template<typename T1, typename T2, typename T3, typename T4>
	struct functor<void, T1, T2, T3, T4>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3,T4)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3),read<T4>(L,4)); return 0; }
	};

	template<typename T1, typename T2, typename T3>
	struct functor<void, T1, T2, T3>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2,T3)>(L)(read<T1>(L,1),read<T2>(L,2),read<T3>(L,3)); return 0; }
	};

	template<typename T1, typename T2>
	struct functor<void, T1, T2>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1,T2)>(L)(read<T1>(L,1),read<T2>(L,2)); return 0; }
	};

	template<typename T1>
	struct functor<void, T1>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)(T1)>(L)(read<T1>(L,1)); return 0; }
	};

	template<>
	struct functor<void>
	{
		static int invoke(lua_State *L) { upvalue_<void(*)()>(L)(); return 0; }
	};

	// 构造器压栈函数
	template<typename RVal> 
		void push_functor(lua_State *L, RVal* s)
	{
	}

	template<typename RVal> 
		void push_functor(lua_State *L, RVal (*func)())
	{
		lua_pushcclosure(L, functor<RVal>::invoke, 1);
	}

	template<typename RVal, typename T1> 
		void push_functor(lua_State *L, RVal (*func)(T1))
	{ 
		lua_pushcclosure(L, functor<RVal,T1>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2> 
		void push_functor(lua_State *L, RVal (*func)(T1,T2))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3> 
		void push_functor(lua_State *L, RVal (*func)(T1,T2,T3))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4> 
		void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5> 
		void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> 
		void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> 
		void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6,T7>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> 
		void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7,T8))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9> 
		void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8,T9>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9
		, typename T10> 
		void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9
		, typename T10, typename T11> 
		void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::invoke, 1);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9
		, typename T10, typename T11, typename T12> 
		void push_functor(lua_State *L, RVal (*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12))
	{ 
		lua_pushcclosure(L, functor<RVal,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>::invoke, 1);
	}

	// member variable
	struct var_base
	{
		virtual void get(lua_State *L) = 0;
		virtual void set(lua_State *L) = 0;
	};

	template<typename T, typename V>
	struct mem_var : var_base
	{
		V T::*_var;
		mem_var(V T::*val) : _var(val) {}
		void get(lua_State *L)
		{
			T* pThis = read<T*>(L,1);
			if(simple_data_type<typename remove_ref<typename remove_const<V>::type>::type>::value)
				push(L, pThis->*(_var));
			else
				push_ref(L, (V&)(pThis->*(_var)));
		}
		void set(lua_State *L)
		{
			T* pThis = read<T*>(L,1);
			V& var_ref = pThis->*(_var);
			var_ref = read<V>(L, 3);
		}
	};

	template<typename RVal, typename T, typename T1=void, typename T2=void, typename T3=void, typename T4=void, typename T5=void
		,typename T6=void, typename T7=void,typename T8=void,typename T9=void,typename T10=void,typename T11=void,typename T12=void,typename T13=void>
	struct mem_functor
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)>
			(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9),read<T9>(L,10),read<T10>(L,11),read<T11>(L,12),read<T12>(L,13),read<T13>(L,14))); return RetValue_(L); }
	};

	// member function
	template<typename cls
		, typename T
		, typename RVal=void
		, typename T1=void
		, typename T2=void
		, typename T3=void
		, typename T4=void
		, typename T5=void>
	struct mem_functor_x
	{
		static int invoke(lua_State *L)
		{
			cls* p = read<cls*>(L,1);
			push(L,(p->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6)));
			return 1;
		}
	};
	template<typename cls
		, typename T
		, typename T1
		, typename T2
		, typename T3
		, typename T4
		, typename T5>
	struct mem_functor_x<cls,T,void,T1,T2,T3,T4,T5>
	{
		static int invoke(lua_State *L)
		{
			cls* p = read<cls*>(L,1);
			(p->*upvalue_<void(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6));
			return 0;
		}
	};
	template<typename cls
		, typename T
		, typename RVal
		, typename T1
		, typename T2
		, typename T3
		, typename T4>
	struct mem_functor_x<cls,T,RVal,T1,T2,T3,T4>
	{
		static int invoke(lua_State *L)
		{
			cls* p = read<cls*>(L,1);
			push(L,(p->*upvalue_<RVal(T::*)(T1,T2,T3,T4)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5)));
			return 1;
		}
	};
	
	template<typename cls
		, typename T
		, typename T1
		, typename T2
		, typename T3
		, typename T4>
	struct mem_functor_x<cls,T,void,T1,T2,T3,T4>
	{
		static int invoke(lua_State *L)
		{
			cls* p = read<cls*>(L,1);
			(p->*upvalue_<void(T::*)(T1,T2,T3,T4)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5));
			return 0;
		}
	};
	template<typename cls
		, typename T
		, typename RVal
		, typename T1
		, typename T2
		, typename T3>
	struct mem_functor_x<cls,T,RVal,T1,T2,T3>
	{
		static int invoke(lua_State *L)
		{
			cls* p = read<cls*>(L,1);
			push(L,(p->*upvalue_<RVal(T::*)(T1,T2,T3)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4)));
			return 1;
		}
	};
	template<typename cls
		, typename T
		, typename T1
		, typename T2
		, typename T3>
	struct mem_functor_x<cls,T,void,T1,T2,T3>
	{
		static int invoke(lua_State *L)
		{
			cls* p = read<cls*>(L,1);
			(p->*upvalue_<void(T::*)(T1,T2,T3)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4));
			return 0;
		}
	};
	template<typename cls
		, typename T
		, typename RVal
		, typename T1
		, typename T2>
	struct mem_functor_x<cls,T,RVal,T1,T2>
	{
		static int invoke(lua_State *L)
		{
			cls* p = read<cls*>(L,1);
			push(L,(p->*upvalue_<RVal(T::*)(T1,T2)>(L))(read<T1>(L,2),read<T2>(L,3)));
			return 1;
		}
	};
	template<typename cls
		, typename T
		, typename T1
		, typename T2>
	struct mem_functor_x<cls,T,void,T1,T2>
	{
		static int invoke(lua_State *L)
		{
			cls* p = read<cls*>(L,1);
			(p->*upvalue_<void(T::*)(T1,T2)>(L))(read<T1>(L,2),read<T2>(L,3));
			return 0;
		}
	};
	template<typename cls
		, typename T
		, typename RVal
		, typename T1>
	struct mem_functor_x<cls,T,RVal,T1>
	{
		static int invoke(lua_State *L)
		{
			cls* p = read<cls*>(L,1);
			push(L,(p->*upvalue_<RVal(T::*)(T1)>(L))(read<T1>(L,2)));
			return 1;
		}
	};
	template<typename cls
		, typename T
		, typename T1>
	struct mem_functor_x<cls,T,void,T1>
	{
		static int invoke(lua_State *L)
		{
			cls* p = read<cls*>(L,1);
			(p->*upvalue_<void(T::*)(T1)>(L))(read<T1>(L,2));
			return 0;
		}
	};
	template<typename cls
		, typename T
		, typename RVal>
	struct mem_functor_x<cls,T,RVal>
	{
		static int invoke(lua_State *L)
		{
			cls* p = read<cls*>(L,1);
			if(is_same_type<RVal,void>::value)
			{
				(p->*upvalue_<RVal(T::*)()>(L))();
				return 0;
			}
			else
			{
				push(L,(p->*upvalue_<RVal(T::*)()>(L))());
				return 1;
			}
		}
	};
	template<typename cls, typename T>
	struct mem_functor_x<cls,T,void>
	{
		static int invoke(lua_State *L)
		{
			cls* p = read<cls*>(L,1);
			(p->*upvalue_<void(T::*)()>(L))();
			return 0;
		}
	};

// 	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5
// 		,typename T6, typename T7,typename T8,typename T9,typename T10,typename T11>
// 	struct mem_functor
// 	{
// 		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)>
// 			(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9),read<T9>(L,10),read<T10>(L,11),read<T11>(L,12))); return RetValue_(L); }
// 	};

 	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5
 		,typename T6, typename T7,typename T8,typename T9,typename T10>
 	struct mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9, T10>
 	{
 		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>
 			(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9),read<T9>(L,10),read<T10>(L,11))); return RetValue_(L); }
 	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4,typename T5,typename T6,typename T7,typename T8,typename T9> 
	struct mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>(L))
			(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9),read<T9>(L,10))); return RetValue_(L); }
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4,typename T5,typename T6,typename T7,typename T8> 
	struct mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L))
			(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8),read<T8>(L,9))); return RetValue_(L); }
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4,typename T5,typename T6,typename T7> 
	struct mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6,T7)>(L))
			(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7),read<T7>(L,8))); return RetValue_(L); }
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4,typename T5,typename T6> 
	struct mem_functor<RVal,T,T1,T2,T3,T4,T5,T6>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5,T6)>(L))
			(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7))); return RetValue_(L); }
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4,typename T5> 
	struct mem_functor<RVal,T,T1,T2,T3,T4,T5>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4,T5)>(L))
			(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6))); return RetValue_(L); }
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4> 
	struct mem_functor<RVal,T,T1,T2,T3,T4>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3,T4)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5))); return RetValue_(L); }
	};

	template<typename RVal, typename T, typename T1, typename T2, typename T3> 
	struct mem_functor<RVal,T,T1,T2,T3>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2,T3)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4))); return RetValue_(L); }
	};

	template<typename RVal, typename T, typename T1, typename T2> 
	struct mem_functor<RVal,T,T1, T2>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1,T2)>(L))(read<T1>(L,2),read<T2>(L,3))); return RetValue_(L); }
	};

	template<typename RVal, typename T, typename T1> 
	struct mem_functor<RVal,T,T1>
	{
		static int invoke(lua_State *L) { push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)(T1)>(L))(read<T1>(L,2))); return RetValue_(L); }
	};

	template<typename RVal, typename T> 
	struct mem_functor<RVal,T>
	{
		static int invoke(lua_State *L) 
		{
			push(L,(read<T*>(L,1)->*upvalue_<RVal(T::*)()>(L))());
			return RetValue_(L);
		}
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9, typename T10, typename T11, typename T12, typename T13>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)
			,read<T7>(L,8),read<T8>(L,9),read<T9>(L,10),read<T10>(L,11),read<T11>(L,12),read<T12>(L,13),read<T13>(L,14)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9, typename T10, typename T11, typename T12>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)
			,read<T7>(L,8),read<T8>(L,9),read<T9>(L,10),read<T10>(L,11),read<T11>(L,12),read<T12>(L,13)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9, typename T10, typename T11>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)
			,read<T7>(L,8),read<T8>(L,9),read<T9>(L,10),read<T10>(L,11),read<T11>(L,12)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9, typename T10>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)
			,read<T7>(L,8),read<T8>(L,9),read<T9>(L,10),read<T10>(L,11)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7,T8,T9>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7,T8,T9)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)
			,read<T7>(L,8),read<T8>(L,9),read<T9>(L,10)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
	>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7,T8>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7,T8)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)
			,read<T7>(L,8),read<T8>(L,9)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6,T7>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6,T7)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)
			,read<T7>(L,8)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	struct mem_functor<void,T,T1,T2,T3,T4,T5,T6>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5,T6)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6),read<T6>(L,7)
			); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
	struct mem_functor<void,T,T1,T2,T3,T4,T5>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4,T5)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3, typename T4>
	struct mem_functor<void,T,T1,T2,T3,T4>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3,T4)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5)); return 0; }
	};

	template<typename T, typename T1, typename T2, typename T3>
	struct mem_functor<void,T,T1,T2,T3>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2,T3)>(L))(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4)); return 0; }
	};

	template<typename T, typename T1, typename T2>
	struct mem_functor<void,T,T1,T2>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1,T2)>(L))(read<T1>(L,2),read<T2>(L,3)); return 0; }
	};

	template<typename T, typename T1>
	struct mem_functor<void,T,T1>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)(T1)>(L))(read<T1>(L,2)); return 0; }
	};

	template<typename T>
	struct mem_functor<void,T>
	{
		static int invoke(lua_State *L)  { (read<T*>(L,1)->*upvalue_<void(T::*)()>(L))(); return 0; }
	};

	// push_functor
	template<typename cls
		, typename T
		, typename RVal>
		void push_functor_x(lua_State *L, cls* _x, RVal (T::*func)()) 
	{ 
		lua_pushcclosure(L, mem_functor_x<cls,T,RVal>::invoke, 1); 
	}
	template<typename cls
		, typename T
		, typename RVal
		, typename T1>
		void push_functor_x(lua_State *L, cls* _x, RVal (T::*func)(T1)) 
	{ 
		lua_pushcclosure(L, mem_functor_x<cls,T,RVal,T1>::invoke, 1); 
	}
	template<typename cls
		, typename T
		, typename RVal
		, typename T1
		, typename T2>
		void push_functor_x(lua_State *L, cls* _x, RVal (T::*func)(T1,T2)) 
	{ 
		lua_pushcclosure(L, mem_functor_x<cls,T,RVal,T1,T2>::invoke, 1); 
	}
	template<typename cls
		, typename T
		, typename RVal
		, typename T1
		, typename T2
		, typename T3>
		void push_functor_x(lua_State *L, cls* _x, RVal (T::*func)(T1,T2,T3)) 
	{ 
		lua_pushcclosure(L, mem_functor_x<cls,T,RVal,T1,T2,T3>::invoke, 1); 
	}
	template<typename cls
		, typename T
		, typename RVal
		, typename T1
		, typename T2
		, typename T3
		, typename T4>
		void push_functor_x(lua_State *L, cls* _x, RVal (T::*func)(T1,T2,T3,T4)) 
	{ 
		lua_pushcclosure(L, mem_functor_x<cls,T,RVal,T1,T2,T3,T4>::invoke, 1); 
	}
	template<typename cls
		, typename T
		, typename RVal
		, typename T1
		, typename T2
		, typename T3
		, typename T4
		, typename T5>
		void push_functor_x(lua_State *L, cls* _x, RVal (T::*func)(T1,T2,T3,T4,T5)) 
	{ 
		lua_pushcclosure(L, mem_functor_x<cls,T,RVal,T1,T2,T3,T4,T5>::invoke, 1); 
	}

	template<typename cls
		, typename T
		, typename RVal>
	void push_functor_x(lua_State *L, cls* _x, RVal (T::*func)() const) 
	{ 
		lua_pushcclosure(L, mem_functor_x<cls,T,RVal>::invoke, 1); 
	}
	template<typename cls
		, typename T
		, typename RVal
		, typename T1>
	void push_functor_x(lua_State *L, cls* _x, RVal (T::*func)(T1) const) 
	{ 
		lua_pushcclosure(L, mem_functor_x<cls,T,RVal,T1>::invoke, 1); 
	}
	template<typename cls
		, typename T
		, typename RVal
		, typename T1
		, typename T2>
		void push_functor_x(lua_State *L, cls* _x, RVal (T::*func)(T1,T2) const) 
	{ 
		lua_pushcclosure(L, mem_functor_x<cls,T,RVal,T1,T2>::invoke, 1); 
	}
	template<typename cls
		, typename T
		, typename RVal
		, typename T1
		, typename T2
		, typename T3>
	void push_functor_x(lua_State *L, cls* _x, RVal (T::*func)(T1,T2,T3) const) 
	{ 
		lua_pushcclosure(L, mem_functor_x<cls,T,RVal,T1,T2,T3>::invoke, 1); 
	}
	template<typename cls
		, typename T
		, typename RVal
		, typename T1
		, typename T2
		, typename T3
		, typename T4>
	void push_functor_x(lua_State *L, cls* _x, RVal (T::*func)(T1,T2,T3,T4) const) 
	{ 
		lua_pushcclosure(L, mem_functor_x<cls,T,RVal,T1,T2,T3,T4>::invoke, 1); 
	}
	template<typename cls
		, typename T
		, typename RVal
		, typename T1
		, typename T2
		, typename T3
		, typename T4
		, typename T5>
	void push_functor_x(lua_State *L, cls* _x, RVal (T::*func)(T1,T2,T3,T4,T5) const) 
	{ 
		lua_pushcclosure(L, mem_functor_x<cls,T,RVal,T1,T2,T3,T4,T5>::invoke, 1); 
	}

	/////////////////////////////////////////

	template<typename RVal, typename T>
		void push_functor(lua_State *L, RVal (T::*func)()) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T>::invoke, 1); 
	}

	template<typename RVal, typename T>
		void push_functor(lua_State *L, RVal (T::*func)() const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1>
		void push_functor(lua_State *L, RVal (T::*func)(T1)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1>
		void push_functor(lua_State *L, RVal (T::*func)(T1) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
	>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
	>
	void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9, typename T10>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9, typename T10>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9, typename T10, typename T11>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9, typename T10, typename T11>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9, typename T10, typename T11, typename T12>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9, typename T10, typename T11, typename T12>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9, typename T10, typename T11, typename T12, typename T13>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13)) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13>::invoke, 1); 
	}

	template<typename RVal, typename T, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8
		, typename T9, typename T10, typename T11, typename T12, typename T13>
		void push_functor(lua_State *L, RVal (T::*func)(T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13) const) 
	{ 
		lua_pushcclosure(L, mem_functor<RVal,T,T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,T12,T13>::invoke, 1); 
	}

	// 构造器
	template<typename T, typename T1, typename T2, typename T3, typename T4, typename T5>
		int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5),read<T5>(L,6));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2, typename T3, typename T4>
		int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4),read<T4>(L,5));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2, typename T3>
		int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3),read<T3>(L,4));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1, typename T2>
		int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2),read<T2>(L,3));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T, typename T1>
		int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>(read<T1>(L,2));
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	template<typename T>
		int constructor(lua_State *L) 
	{ 
		new(lua_newuserdata(L, sizeof(val2user<T>))) val2user<T>();
		push_meta(L, class_name<typename class_type<T>::type>::name());
		lua_setmetatable(L, -2);

		return 1; 
	}

	// destroyer
	template<typename T>
		int destroyer(lua_State *L) 
	{ 
		((user*)lua_touserdata(L, 1))->~user();
		return 0;
	}

	// global function
	template<typename F> 
		void def(lua_State* L, const char* name, F func)
	{ 
		lua_pushstring(L, name);
		lua_pushlightuserdata(L, (void*)func);
		push_functor(L, func);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	// global variable
	template<typename T>
		void set(lua_State* L, const char* name, T object)
	{
		lua_pushstring(L, name);
		push(L, object);
		lua_settable(L, LUA_GLOBALSINDEX);
	}

	template<typename T>
		T get(lua_State* L, const char* name)
	{
		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
		return pop<T>(L);
	}

	template<typename T>
		void decl(lua_State* L, const char* name, T object)
	{
		set(L, name, object);
	}

	// 判断脚本中有没有指定函数, 支持嵌套表中的函数
	bool hasfunction(lua_State* L, const char* tfname);

	// 调用LUA函数, 支持嵌套表中的函数
	template<typename RVal>
		RVal call(lua_State* L, const char* tfname)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);

		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 0;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	template<typename RVal, typename T1>
		RVal call(lua_State* L, const char* tfname, T1 arg1)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);
		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 1;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		push(L, arg1);
		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	template<typename RVal, typename T1, typename T2>
		RVal call(lua_State* L, const char* tfname, T1 arg1, T2 arg2)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);
		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 2;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		push(L, arg1);
		push(L, arg2);
		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	template<typename RVal, typename T1, typename T2, typename T3>
		RVal call(lua_State* L, const char* tfname, T1 arg1, T2 arg2, T3 arg3)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);
		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 3;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		push(L, arg1);
		push(L, arg2);
		push(L, arg3);
		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4>
		RVal call(lua_State* L, const char* tfname, T1 arg1, T2 arg2, T3 arg3, T4 arg4)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);
		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 4;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		push(L, arg1);
		push(L, arg2);
		push(L, arg3);
		push(L, arg4);
		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5>
		RVal call(lua_State* L, const char* tfname, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);
		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 5;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		push(L, arg1);
		push(L, arg2);
		push(L, arg3);
		push(L, arg4);
		push(L, arg5);
		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
		RVal call(lua_State* L, const char* tfname, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);
		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 6;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		push(L, arg1);
		push(L, arg2);
		push(L, arg3);
		push(L, arg4);
		push(L, arg5);
		push(L, arg6);
		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
		RVal call(lua_State* L, const char* tfname, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);
		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 7;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		push(L, arg1);
		push(L, arg2);
		push(L, arg3);
		push(L, arg4);
		push(L, arg5);
		push(L, arg6);
		push(L, arg7);
		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
		RVal call(lua_State* L, const char* tfname, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);
		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 8;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		push(L, arg1);
		push(L, arg2);
		push(L, arg3);
		push(L, arg4);
		push(L, arg5);
		push(L, arg6);
		push(L, arg7);
		push(L, arg8);
		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
		RVal call(lua_State* L, const char* tfname, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);
		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 9;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		push(L, arg1);
		push(L, arg2);
		push(L, arg3);
		push(L, arg4);
		push(L, arg5);
		push(L, arg6);
		push(L, arg7);
		push(L, arg8);
		push(L, arg9);
		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
		RVal call(lua_State* L, const char* tfname, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);
		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 10;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		push(L, arg1);
		push(L, arg2);
		push(L, arg3);
		push(L, arg4);
		push(L, arg5);
		push(L, arg6);
		push(L, arg7);
		push(L, arg8);
		push(L, arg9);
		push(L, arg10);
		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11>
		RVal call(lua_State* L, const char* tfname, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);
		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 11;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		push(L, arg1);
		push(L, arg2);
		push(L, arg3);
		push(L, arg4);
		push(L, arg5);
		push(L, arg6);
		push(L, arg7);
		push(L, arg8);
		push(L, arg9);
		push(L, arg10);
		push(L, arg11);
		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12>
		RVal call(lua_State* L, const char* tfname, T1 arg1, T2 arg2, T3 arg3, T4 arg4, T5 arg5, T6 arg6, T7 arg7, T8 arg8, T9 arg9, T10 arg10, T11 arg11, T12 arg12)
	{
		BOOL selfCall = FALSE;

		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, 16);

		char* ss2[2];
		char* lasts = ss[ns-1];
		int ns2 = split(lasts, ':', ss2, 2);
		if(ns2 == 2)
		{
			selfCall = TRUE;
			ss[ns-1] = ss2[0];
			ss[ns] = ss2[1];
			ns += 1;
		}

		int sp = lua_gettop(L);
		int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i<ns-1; i++)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LuaTinker::call() attempt a table `%s' (not a table)", ptname);
				lua_pushnil(L);
				return retset<RVal>(L, sp);
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LuaTinker::call() attempt to call table function `%s' (not a function)", pfname);
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		int argc = 12;
		if(selfCall)
		{
			argc += 1;
			lua_insert(L, -2);
		}

		push(L, arg1);
		push(L, arg2);
		push(L, arg3);
		push(L, arg4);
		push(L, arg5);
		push(L, arg6);
		push(L, arg7);
		push(L, arg8);
		push(L, arg9);
		push(L, arg10);
		push(L, arg11);
		push(L, arg12);
		if (lua_pcall(L, argc, 1, errfunc) != 0)
		{
			lua_pushnil(L);
			return retset<RVal>(L, sp);
		}

		return retset<RVal>(L, sp);
	}

	// class helper
	//LUAWARP_API int meta_get(lua_State *L);
	//LUAWARP_API int meta_set(lua_State *L);
	//LUAWARP_API void push_meta(lua_State *L, const char* name);

	// class init
	template<typename T>
		void class_add(lua_State* L, const char* nm, BOOL bwhitgc = 1) 
	{ 
		const char* name = class_name<T>::name(nm);

		lua_pushstring(L, name);
		lua_newtable(L);

		lua_pushstring(L, "__name");
		lua_pushstring(L, name);
		lua_rawset(L, -3);

		lua_pushstring(L, "__index");
		lua_pushcclosure(L, meta_get, 0);
		lua_rawset(L, -3);

		lua_pushstring(L, "__newindex");
		lua_pushcclosure(L, meta_set, 0);
		lua_rawset(L, -3);

		if (bwhitgc)
		{
			lua_pushstring(L, "__gc");
			lua_pushcclosure(L, destroyer<T>, 0);
			lua_rawset(L, -3);
		}

		lua_settable(L, LUA_GLOBALSINDEX);
	}

	// Tinker Class Inheritence
	template<typename T, typename P>
		void class_inh(lua_State* L)
	{
																			//(nil)
		push_meta(L, class_name<T>::name());								//metatable
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, "__parent");									//metatable "parent"
			push_meta(L, class_name<P>::name());							//metatable "parent" parentmetatable
			//防止注册类型时序不正确。基类类型导出在派生类后面是错误的！
			ASSERT(!lua_isnil(L, -1));
			lua_rawgeti(L, -1, CLASSMETA_INHERITLEVEL);						//metatable "parent" parentmetatable parentInheritLevel
			int level = 1;
			if(!lua_isnil(L, -1))
			{
				level = (int)lua_tonumber(L, -1) + 1;
			}
			lua_pop(L, 1);													//metatable "parent" parentmetatable
			lua_pushnumber(L, level);										//metatable "parent" parentmetatable inheritLevel
			lua_rawseti(L, -4, CLASSMETA_INHERITLEVEL);						//metatable "parent" parentmetatable 
			lua_rawset(L, -3);												//metatable
		}
		lua_pop(L, 1);														//(nil)
	}

	// Tinker Class Constructor
	template<typename T, typename F>
		void class_con(lua_State* L,F func)
	{
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_newtable(L);
			lua_pushstring(L, "__call");
			lua_pushcclosure(L, func, 0);
			lua_rawset(L, -3);
			lua_setmetatable(L, -2);
		}
		lua_pop(L, 1);
	}

	template<typename T, typename F>
	void class_def_x(lua_State* L, const char* name, F func) 
	{ 
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, name);
			new(lua_newuserdata(L,sizeof(F))) F(func);
			push_functor_x(L, (T*)0, func);
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}

	// Tinker Class Functions
	template<typename T, typename F>
		void class_def(lua_State* L, const char* name, F func) 
	{ 
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, name);
			new(lua_newuserdata(L,sizeof(F))) F(func);
			push_functor(L, func);
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}

	// Tinker Class Variables
	template<typename T, typename BASE, typename VAR>
		void class_mem(lua_State* L, const char* name, VAR BASE::*val) 
	{ 
		push_meta(L, class_name<T>::name());
		if(lua_istable(L, -1))
		{
			lua_pushstring(L, name);
			new(lua_newuserdata(L,sizeof(mem_var<BASE,VAR>))) mem_var<BASE,VAR>(val);
			lua_rawset(L, -3);
		}
		lua_pop(L, 1);
	}

	// Table Object on Stack
	struct LUAWARP_API table_obj
	{
		table_obj(lua_State* L, int index);
		~table_obj();

		void inc_ref();
		void dec_ref();

		bool validate();

		template<typename T>
			void set(const char* name, T object)
		{
			if(validate())
			{
				lua_pushstring(m_L, name);
				push(m_L, object);
				lua_settable(m_L, m_index);
			}
		}

		template<typename T>
			void set(int idx, T object)
		{
			if (validate())
			{
				push(m_L, object);
				lua_rawseti(m_L, -2, idx);
			}
		}

		//add by yangbin for obj type
		template<typename T>
		void set(const char* name, T object, const char* typeName)
		{
			if(validate())
			{
				lua_pushstring(m_L, name);
				push(m_L, object, typeName);
				lua_settable(m_L, m_index);
			}
		}

		template<typename T>
		void set(int idx, T object, const char* typeName)
		{
			if (validate())
			{
				push(m_L, object, typeName);
				lua_rawseti(m_L, -2, idx);
			}
		}

		bool setobject(const char* name, void* p, const char* typeName)
		{
			if(!this->validate()) return false;
			lua_pushstring(m_L, name);
			
			voidPtr2lua::invoke(m_L, p, typeName);
			//push_meta(m_L, typeName);
			//lua_setmetatable(m_L, -2);

			lua_settable(m_L, m_index);
			return true;
		}
        //

		template<typename T>
			T get(const char* name)
		{
			if(validate())
			{
				lua_pushstring(m_L, name);
				lua_gettable(m_L, m_index);
			}
			else
			{
				lua_pushnil(m_L);
			}

			return pop<T>(m_L);
		}

		template<typename T>
		T get(int idx)
		{
			if (validate())
			{
				lua_pushnumber(m_L, idx); 
				lua_gettable(m_L, m_index);
			}
			else
			{
				lua_pushnil(m_L);
			}

			return pop<T>(m_L);
		}

		lua_State*		m_L;
		int				m_index;
		const void*		m_pointer;
		int				m_ref;
	};
	
	struct LUAWARP_API l_stackval
	{
		int index;
		lua_State* L;
		l_stackval():index(0),L(NULL) { }
		l_stackval(const l_stackval& o):index(o.index),L(o.L) { }
		l_stackval(lua_State* l, int i):index(i),L(l) { }
		~l_stackval() { }
		l_stackval& operator = (const l_stackval& o) { index = o.index; L = o.L; return *this; }
	};

	struct LUAWARP_API l_func
	{
		int index;
		lua_State* L;
		
		l_func():index(0),L(NULL) { }
		l_func(const l_func& o):index(o.index),L(o.L) { }
		l_func(lua_State* _L, int _index):index(_index),L(_L) { }
		~l_func() { }

		template <typename T1> void invoke(T1 p1)
		{
			KLuaStackRecover sr(L);
			lua_pushvalue(L,index);
			push<T1>(L, p1);
			lua_call(L, 1, 0);
		}
		template <typename T1,typename T2> void invoke(T1 p1, T2 p2)
		{
			KLuaStackRecover sr(L);
			lua_pushvalue(L,index);
			push<T1>(L, p1);
			push<t2>(L, p2);
			lua_call(L, 2, 0);
		}
		template <typename T1,typename T2,typename T3> void invoke(T1 p1, T2 p2, T3 p3)
		{
			KLuaStackRecover sr(L);
			lua_pushvalue(L,index);
			push<T1>(L, p1);
			push<T2>(L, p2);
			push<T3>(L, p3);
			lua_call(L, 3, 0);
		}
		template <typename T1,typename T2,typename T3,typename T4> void invoke(T1 p1, T2 p2, T3 p3,T4 p4)
		{
			KLuaStackRecover sr(L);
			lua_pushvalue(L,index);
			push<T1>(L, p1);
			push<T2>(L, p2);
			push<T3>(L, p3);
			push<T4>(L, p4);
			lua_call(L, 4, 0);
		}
		template <typename R,typename T1> R r_invoke(T1 p1)
		{
			KLuaStackRecover sr(L);
			lua_pushvalue(L,index);
			push<T1>(L, p1);
			lua_call(L, 1, 1);
			R ret = read<R>(L, -1);
			return ret;
		}
		template <typename R,typename T1,typename T2> R r_invoke(T1 p1,T2 p2)
		{
			KLuaStackRecover sr(L);
			lua_pushvalue(L,index);
			push<T1>(L, p1);
			push<T2>(L, p2);
			lua_call(L, 2, 1);
			R ret = read<R>(L, -1);
			return ret;
		}
		template <typename R,typename T1,typename T2,typename T3> R r_invoke(T1 p1,T2 p2,T3 p3)
		{
			KLuaStackRecover sr(L);
			lua_pushvalue(L,index);
			push<T1>(L, p1);
			push<T2>(L, p2);
			push<T3>(L, p3);
			lua_call(L, 3, 1);
			R ret = read<R>(L, -1);
			return ret;
		}
		template <typename R,typename T1,typename T2,typename T3,typename T4> R r_invoke(T1 p1,T2 p2,T3 p3,T4 p4)
		{
			KLuaStackRecover sr(L);
			lua_pushvalue(L,index);
			push<T1>(L, p1);
			push<T2>(L, p2);
			push<T3>(L, p3);
			push<T4>(L, p4);
			lua_call(L, 4, 1);
			R ret = read<R>(L, -1);
			return ret;
		}
	};

	// Table Object Holder
	struct LUAWARP_API table
	{
		table(lua_State* L);
		table(lua_State* L, int index);
		table(lua_State* L, const char* name);
		table(lua_State* L, const CppTable& dataset);
		table(const table& input);
		~table();

		template<typename T>
			void set(const char* name, T object)
		{
			m_obj->set(name, object);
		}

		template<typename T>
			void set(int idx, T object)
		{
			m_obj->set(idx, object);
		}

        //add by yangbin for obj type
		template<typename T>
		void set(const char* name, T object, const char* typeName)
		{
			m_obj->set(name, object, typeName);
		}

		template<typename T>
		void set(int idx, T object, const char* typeName)
		{
			m_obj->set(idx, object, typeName);
		}

		bool setobject(const char* name, void* p, const char* typeName)
		{
			return m_obj->setobject(name, p, typeName);
		}
        //

		template<typename T>
			T get(const char* name)
		{
			return m_obj->get<T>(name);
		}

		template<typename T>
		T get(int idx)
		{
			return m_obj->get<T>(idx);
		}

		table_obj*		m_obj;
	};

	/**
	 * 根据tfname找到指定函数，将其放到lua栈顶。参照LMREG::call修改。无论找到与否，都会在原栈基础上压入一个变量（指定函数或者nil）
	 */
	LUAWARP_API bool get_func(lua_State* L, const char* tfname);
} // namespace LMREG

char** spilwhit(char* str, char* strsp, DWORD* buf, int buflen, int& ct);

// KLuaStackRecover /////////////////////
// 自动回复Lua栈的位置
//
class KLuaStackRecover
{
public:
	int m_top;
	lua_State* m_L;
	KLuaStackRecover(lua_State* L):m_L(L)
	{
		m_top = lua_gettop(m_L);
	}
	~KLuaStackRecover()
	{
		lua_settop(m_L, m_top);
	}
};

class LUAWARP_API LuaItem
{
private:
	int m_top;
	int m_parentIdx;
	short m_isNum; // myName是一个数字吗
	short m_level; // 嵌套级别，全局变量级别为0，全局表中得变量级别为1，依次类推
	const char* m_myName;
	lua_State* m_L;

private:
	static BOOL isfunctype(const char* typeName);
	void pushmyname();

public:
	LuaItem(lua_State* L, int parent, const char* myName);
	LuaItem(lua_State* L, int parent, int idx);
	~LuaItem();

public:
	bool isnil() const;
	bool isboolean() const;
	bool isnumber() const;
	bool istable() const;
	bool isstring() const;
	bool isuserdata() const;
	bool isfunction() const;
	bool isCfunction() const;

public:
	BOOL IsFunction() const;

public:
	LuaItem& operator [] (int idx);
	LuaItem& operator [] (const char* name);

public:
	template <typename T> LuaItem& operator = (T val)
	{
		lua_pop(m_L, 1);
		if(LuaItem::isfunctype(typeid(val).name()))
		{
			lua_pushlightuserdata(m_L, (void*)val);
			LMREG::push_functor(m_L, val);
			lua_pushstring(m_L, m_myName);
			lua_pushvalue(m_L, -2);
			lua_settable(m_L, m_parentIdx);
		}
		else
		{
			LMREG::push(m_L, val);
			lua_pushstring(m_L, m_myName);
			lua_pushvalue(m_L, -2);
			lua_settable(m_L, m_parentIdx);
		}
		return *this;
	}

public:
	LuaItem& operator = (bool val);
	LuaItem& operator = (char val);
	LuaItem& operator = (BYTE val);
	LuaItem& operator = (short val);
	LuaItem& operator = (WORD val);
	LuaItem& operator = (int val);
	LuaItem& operator = (unsigned int val);
	LuaItem& operator = (long val);
	LuaItem& operator = (unsigned long val);
	LuaItem& operator = (INT64 val);
	LuaItem& operator = (UINT64 val);
	LuaItem& operator = (float val);
	LuaItem& operator = (double val);
	LuaItem& operator = (const char* val);
	LuaItem& operator = (const LuaString& val);
	LuaItem& operator = (LMREG::table& val);
	LuaItem& operator = (LMREG::CppTable& val);
	LuaItem& operator = (LMREG::KPtrObjcet val);

public:
	template<typename T> operator T ()
	{
		return LMREG::read<T>(m_L, -1);
	}

public:
	LuaItem& invoke()
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		lua_pcall(m_L, 0, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1> LuaItem& invoke(const T1& t1)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		LMREG::push(m_L, t1);
		lua_pcall(m_L, 1, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2> LuaItem& invoke(const T1& t1, const T2& t2)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		lua_pcall(m_L, 2, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3>
	LuaItem& invoke(const T1& t1, const T2& t2, const T3& t3)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		lua_pcall(m_L, 3, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4>
	LuaItem& invoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		lua_pcall(m_L, 4, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5>
	LuaItem& invoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		LMREG::push(m_L, t5);
		lua_pcall(m_L, 5, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	LuaItem& invoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		LMREG::push(m_L, t5);
		LMREG::push(m_L, t6);
		lua_pcall(m_L, 6, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	LuaItem& invoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		LMREG::push(m_L, t5);
		LMREG::push(m_L, t6);
		LMREG::push(m_L, t7);
		lua_pcall(m_L, 7, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	LuaItem& invoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		LMREG::push(m_L, t5);
		LMREG::push(m_L, t6);
		LMREG::push(m_L, t7);
		LMREG::push(m_L, t8);
		lua_pcall(m_L, 8, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	LuaItem& invoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		LMREG::push(m_L, t5);
		LMREG::push(m_L, t6);
		LMREG::push(m_L, t7);
		LMREG::push(m_L, t8);
		LMREG::push(m_L, t9);
		lua_pcall(m_L, 9, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T0>
	LuaItem& invoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T0& t0)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		LMREG::push(m_L, t5);
		LMREG::push(m_L, t6);
		LMREG::push(m_L, t7);
		LMREG::push(m_L, t8);
		LMREG::push(m_L, t9);
		LMREG::push(m_L, t0);
		lua_pcall(m_L, 10, LUA_MULTRET, errfunc);
		return *this;
	}

public:
	LuaItem& selfinvoke()
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		if(m_level) lua_pushvalue(m_L, m_parentIdx);
		lua_pcall(m_L, m_level?1:0, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1> LuaItem& selfinvoke(const T1& t1)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		if(m_level) lua_pushvalue(m_L, m_parentIdx);
		LMREG::push(m_L, t1);
		lua_pcall(m_L, m_level?2:1, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2> LuaItem& selfinvoke(const T1& t1, const T2& t2)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		if(m_level) lua_pushvalue(m_L, m_parentIdx);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		lua_pcall(m_L, m_level?3:2, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3>
	LuaItem& selfinvoke(const T1& t1, const T2& t2, const T3& t3)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		if(m_level) lua_pushvalue(m_L, m_parentIdx);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		lua_pcall(m_L, m_level?4:3, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4>
	LuaItem& selfinvoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		if(m_level) lua_pushvalue(m_L, m_parentIdx);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		lua_pcall(m_L, m_level?5:4, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5>
	LuaItem& selfinvoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		if(m_level) lua_pushvalue(m_L, m_parentIdx);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		LMREG::push(m_L, t5);
		lua_pcall(m_L, m_level?6:5, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	LuaItem& selfinvoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		if(m_level) lua_pushvalue(m_L, m_parentIdx);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		LMREG::push(m_L, t5);
		LMREG::push(m_L, t6);
		lua_pcall(m_L, m_level?7:6, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	LuaItem& selfinvoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		if(m_level) lua_pushvalue(m_L, m_parentIdx);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		LMREG::push(m_L, t5);
		LMREG::push(m_L, t6);
		LMREG::push(m_L, t7);
		lua_pcall(m_L, m_level?8:7, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	LuaItem& selfinvoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		if(m_level) lua_pushvalue(m_L, m_parentIdx);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		LMREG::push(m_L, t5);
		LMREG::push(m_L, t6);
		LMREG::push(m_L, t7);
		LMREG::push(m_L, t8);
		lua_pcall(m_L, 9, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	LuaItem& selfinvoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		if(m_level) lua_pushvalue(m_L, m_parentIdx);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		LMREG::push(m_L, t5);
		LMREG::push(m_L, t6);
		LMREG::push(m_L, t7);
		LMREG::push(m_L, t8);
		LMREG::push(m_L, t9);
		lua_pcall(m_L, m_level?10:9, LUA_MULTRET, errfunc);
		return *this;
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T0>
	LuaItem& selfinvoke(const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T0& t0)
	{
		int errfunc = LMREG::lua_geterrfunc(m_L); lua_pushvalue(m_L,-2);
		if(m_level) lua_pushvalue(m_L, m_parentIdx);
		LMREG::push(m_L, t1);
		LMREG::push(m_L, t2);
		LMREG::push(m_L, t3);
		LMREG::push(m_L, t4);
		LMREG::push(m_L, t5);
		LMREG::push(m_L, t6);
		LMREG::push(m_L, t7);
		LMREG::push(m_L, t8);
		LMREG::push(m_L, t9);
		LMREG::push(m_L, t0);
		lua_pcall(m_L, m_level?11:10, LUA_MULTRET, errfunc);
		return *this;
	}

public:
	LuaItem& operator () ()
	{
		return this->invoke();
	}
	template <typename T1> LuaItem& operator () (const T1& t1)
	{
		return this->invoke(t1);
	}
	template <typename T1,typename T2>
	LuaItem& operator () (const T1& t1, const T2& t2)
	{
		return this->invoke(t1, t2);
	}
	template <typename T1,typename T2,typename T3>
	LuaItem& operator () (const T1& t1, const T2& t2, const T3& t3)
	{
		return this->invoke(t1, t2, t3);
	}
	template <typename T1,typename T2,typename T3,typename T4>
	LuaItem& operator () (const T1& t1, const T2& t2, const T3& t3, const T4& t4)
	{
		return this->invoke(t1, t2, t3, t4);
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5>
	LuaItem& operator () (const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5)
	{
		return this->invoke(t1, t2, t3, t4, t5);
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6>
	LuaItem& operator () (const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6)
	{
		return this->invoke(t1, t2, t3, t4, t5, t6);
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7>
	LuaItem& operator () (const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7)
	{
		return this->invoke(t1, t2, t3, t4, t5, t6, t7);
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8>
	LuaItem& operator () (const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8)
	{
		return this->invoke(t1, t2, t3, t4, t5, t6, t7, t8);
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9>
	LuaItem& operator () (const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9)
	{
		return this->invoke(t1, t2, t3, t4, t5, t6, t7, t8, t9);
	}
	template <typename T1,typename T2,typename T3,typename T4,typename T5,typename T6,typename T7,typename T8,typename T9,typename T0>
	LuaItem& operator () (const T1& t1, const T2& t2, const T3& t3, const T4& t4, const T5& t5, const T6& t6, const T7& t7, const T8& t8, const T9& t9, const T0& t0)
	{
		return this->invoke(t1, t2, t3, t4, t5, t6, t7, t8, t9, t0);
	}

public:
	///**
	// * 读取函数调用的返回值，支持最多返回4个值
	// * 
	// *		-- lua function
	// *		function func() { return 1, 2,3,4; }
	// *
	// *		// C++
	// *		int a,b,c,d;
	// *		LuaWrap["func"]().read(a,b,c,d);
	// *
	// * 如果函数只有一个返回值，可用下面的形式获得
	// *		int n = LuaWrap["func"]();
	// *
	// */
	template<typename T1> bool read(T1& t1)
	{
		t1 = LMREG::read<T1>(m_L, -1);
		return true;
	}
	template<typename T1,typename T2> bool read(T1& t1, T2&t2)
	{
		t2 = LMREG::read<T2>(m_L, -1);
		t1 = LMREG::read<T1>(m_L, -2);
		return true;
	}
	template<typename T1,typename T2,typename T3> bool read(T1& t1, T2&t2, T3& t3)
	{
		t3 = LMREG::read<T3>(m_L, -1);
		t2 = LMREG::read<T2>(m_L, -2);
		t1 = LMREG::read<T1>(m_L, -3);
		return true;
	}
	template<typename T1,typename T2,typename T3,typename T4> bool read(T1& t1, T2&t2, T3& t3,T4& t4)
	{
		t4 = LMREG::read<T4>(m_L, -1);
		t3 = LMREG::read<T3>(m_L, -2);
		t2 = LMREG::read<T2>(m_L, -3);
		t1 = LMREG::read<T1>(m_L, -4);
		return true;
	}
};

typedef int (*ErrReportFunc)(lua_State* L);

class LUAWARP_API KLuaWrap
{
public:
	//namestack m_stk;
	lua_State* m_l;
	//ScriptMap m_mpScripts;
	DWORD m_dwScripts;
	const char* _LastError;
	ErrReportFunc m_funcErrReport;

	// user data in lua state
	LMREG::KLuaState_UD m_ud;
	// userdata 2 extend variable ref
	int m_extendTableRef;
	// name 2 ptr2userdata table
	// 每个 class_name 都会对应一个，方便 push(T*) 的时候找到自己的 userdata(1.传承原有数据 2.优化效率)
	// class_name 会在不同的模块产生不同的实例，所以只能用 name 来
	int m_typename2userdataableRef;

	// ref int registry, env tables for doStringWithParam
	KScriptContextPool* m_pScriptContextPool;

public:
	KLuaWrap(void);
	~KLuaWrap(void);

	// 初始化Lua
	int InitScript(int bDebugMode = 1);
	// 补充一下共享 lua_State
	int InitScript(lua_State* L);
	// 释放Lua
	void Destory();

	// 调用脚本中的函数,file是要调用的脚本名,值可为空
	// func是函数名,不可为空
	// fmt为参数列表顺序,s表示字符串、d表示整数、f表示浮点数、>表示返回值开始
	// 使用方法如：DoScriptFunc("d:\\a.lua", "test", "sdd>d", "a", 3, 5, &x);
	// 表示,调用脚本"d:\\a.lua"中的函数test,参数为"a"、3、5,返回值存到x中
	int DoScriptFunc(char* file, char* func, char* fmt, ...);

	// 执行一个脚本文件
	int doScripts(const char* name);
	int doDirScripts(const char* dir);
	// 将一个脚本文件载入内存,但不执行
	int LoadScripts(const char* name);

	BOOL loadStream(StreamInterface& si);
	BOOL loadStream(StreamInterface& si,const char* fn);
	BOOL doStream(StreamInterface& si);
	BOOL doStream(StreamInterface& si,const char* fn);

	bool DoDirScripts_Recursive(const char* dir);

	// 执行一段文本
	int doString(const char* script);

	bool doStringWithParam(KParamPackage& params, const char* script);
	bool doStringWithParam(KParamPackage& params, const char* script, size_t len);

	// 直接doString，会把内容 script 当做name存起来，这里指定名字
	int doString(const char* script, const char* name);				//@1 加载一个字符串
	bool doStringInMainThread( const char* szString );
	int doBuffer(const void* buffer, int length, const char* name);				//@2 加载一段内存

	BOOL loadBuffer(const void* buffer, int len, const char* funcName);

	// 将文本调用内存,但不执行
	int LoadString(const char* script);
	// 获得指定资源的 ref
	int Ref(const char* resName);
	// 释放指定资源的 ref
	void UnRef(int ref);
	// 执行指定 ref 功能
	void DoRef(int ref);
	// 通过 LuaItem 获得 ref. DoRef 的参数补充版
	LuaItem GetRef(int ref);
	// dump系函数
	int GetDumpProtoBuffer(const char* funName, lua_Writer w, void* data, int strip);	/// @通过已存在函数,使用回调函数，把逻辑屏蔽在外面
	int GetDumpFileBuffer(const char* fileName, lua_Writer w);	/// @直接dump出一个文件
	
	bool complieScriptFile(const char* srcFilename,const char* destFilename);
	///*封装lua已有功能，争取最后需求能达到不直接调用lua基础api就能完成所有功能*/
	void GetGlobal(const char* objectName);
	//--------------------------------add by yangbin for luasystem-------------------------------
	//void GetField(const char* objectName);
	//-------------------------------------------------------------------------------------------
	void StackPop(int n);
	bool CheckType(int n, int type);

	// 释放掉不用的脚本,必须保证此脚本中的函数不再被调用,
	// 否则可能会出现运行时错误
	void releaseScript(char* name);
	void releaseScript(int id);

	// 脚本是否已经被载入内存
	bool isScriptLoaded(char* name);

	// 重载[]操作符,用于提取lua变量
	LuaItem operator[](const char* s);
	LuaItem operator[](int idx);

	// 从2项中移过来的代码，支持嵌套赋值
	int getsctnum(const char* src, int ch);
	char* getsctstr(const char* src, int ch, int pos, char* dst);
	int getNestValue(lua_State *L, const char* scr);

	template<typename T> void SetNestobjectVal(const char* fieldname, const char* key, T object)
	{
		if(fieldname == 0 || fieldname[0] == '\0' || key == 0)
		{
			return;
		}
		//
		int tp = lua_gettop(m_l);
		int ret = getNestValue(m_l, fieldname);
		if(ret != 0 && lua_istable(m_l, -1))
		{
			/// 压栈成功,并且最上面的类型为 table 类型
			lua_pushstring(m_l, key);
			LMREG::push(m_l, object);
			lua_settable(m_l, -3);
		}
		/// 恢复栈
		lua_settop(m_l, tp);
	}

	template<typename T> void SetNestobjectVal(const char* fieldname, const char* key, T object,const char* meta)
	{
		if(fieldname == 0 || fieldname[0] == '\0' || key == 0)
		{
			return;
		}
		//
		int tp = lua_gettop(m_l);
		int ret = getNestValue(m_l, fieldname);
		if(ret != 0 && lua_istable(m_l, -1))
		{
			/// 压栈成功,并且最上面的类型为 table 类型
			lua_pushstring(m_l, key);
			LMREG::push(m_l, object, meta);
			lua_settable(m_l, -3);
		}
		/// 恢复栈
		lua_settop(m_l, tp);
	}

	template<typename T> T GetNestobjectVal(const char* fieldname, const char* key)
	{
		if(fieldname == 0 || fieldname[0] == '\0' || key == 0)
		{
			return;
		}
		//
		int tp = lua_gettop(m_l);
		int ret = getNestValue(m_l, fieldname);
		T result;
		if(ret != 0 && lua_istable(m_l, -1))
		{
			/// 压栈成功,并且最上面的类型为 table 类型
			lua_pushstring(m_l, key);
			lua_gettable(m_l, -2);

			result = LMREG::pop<T>(m_l);
		}
		/// 恢复栈
		lua_settop(m_l, tp);

		return result;
	}

	void RegErrorReport(ErrReportFunc ErrReport)
	{
		m_funcErrReport = ErrReport;
	}

	///
	// 创建一个扩展表,用来扩展到处类型参数
	// 创建的是一张 弱表,也就是当 key(userdate) 无效的时候表自己会释放被注册的对象
	void createExtendTable();
	// 把扩展表压入栈中,注意这个接口改变栈的,调用方需要维护一下
	void pushExtendTable();
	// 创建 name 2 userdatatable table. 效率不行(由于 chassname<string> 查找造成，则在 class_name 中生成ref便于查找)
	// (不同模块的 ref 可以在初始化或者使用的时候动态赋值，达到各个模块都有数据的效果)
	void createName2UserdataTable();
	// 把 name 对应 ptr2userdata table 压栈
	void pushUserdataTable(const char* name);
public:
	void SetNumVar(const char* name, double object) { LMREG::set<double>(m_l, name, object); }
	double GetNumVar(const char* name) { return LMREG::get<double>(m_l, name); }

	void SetStrVar(const char* name, char* object) { LMREG::set<char*>(m_l, name, object); }
	const char* GetStrVar(const char* name) { return LMREG::get<char*>(m_l, name); }

	// 向Lua中注册一个对象
	template<typename T> void SetObjectVal(const char* name, T object) { LMREG::set<T>(m_l, name, object); }
	template<typename T> T GetObjectVal(const char* name) { return LMREG::get<T>(m_l, name); }

	// 向Lua中注册一个函数
	template<typename F>
	void RegFunc(const char* name, F func) { LMREG::def<F>(m_l, name, func); }

	// 判断脚本中有没有指定函数, 支持嵌套表中的函数
	bool hasFunction(const char* name) { return LMREG::hasfunction(m_l, name); }
	bool hasFunctionEx(const char* name) { return LMREG::hasfunction(m_l, name); }

	// build a function named name with body luaString
	BOOL buildFunction(const char* name, const char* luaString);
	BOOL buildFunction(const char* name, const char* luaString, const char* arg1);
	BOOL buildFunction(const char* name, const char* luaString, const char* arg1, const char* arg2);
	BOOL buildFunction(const char* name, const char* luaString, const char* arg1, const char* arg2, const char* arg3);
	BOOL buildFunction(const char* name, const char* luaString, const char* arg1, const char* arg2, const char* arg3, const char* arg4);
	BOOL buildFunction(const char* name, const char* luaString, const char* arg1, const char* arg2, const char* arg3, const char* arg4, const char* arg5);

	// call anonymous function, which body is luaString
	template <typename RVal> RVal callAnonymous(const char* luaString)
	{
		char buf[64];
		sprintf(buf, "__anonymous%llu__", getCurrentThreadId());
		if(!this->buildFunction(buf, luaString))
		{
			return _type_default_value_<RVal>::get();
		}
		return this->Call<RVal>(buf);
	}
	template <typename RVal,typename T1> RVal callAnonymous(const char* luaString, const char* name1, T1 arg1)
	{
		char buf[64];
		sprintf(buf, "__anonymous%llu__", getCurrentThreadId());
		if(!this->buildFunction(buf, luaString, name1))
		{
			return _type_default_value_<RVal>::get();
		}
		return this->Call<RVal,T1>(buf, arg1);
	}
	template <typename RVal,typename T1,typename T2> RVal callAnonymous(const char* luaString, const char* name1, T1 arg1, const char* name2, T2 arg2)
	{
		char buf[64];
		sprintf(buf, "__anonymous%llu__", getCurrentThreadId());
		if(!this->buildFunction(buf, luaString, name1, name2))
		{
			return _type_default_value_<RVal>::get();
		}
		return this->Call<RVal,T1,T2>(buf, arg1, arg2);
	}

	template <typename RVal,typename T1,typename T2, typename T3>
	RVal callAnonymous(const char* luaString
						, const char* name1, T1 arg1
						, const char* name2, T2 arg2
						, const char* name3, T3 arg3
						)
	{
		char buf[64];
		sprintf(buf, "__anonymous%llu__", getCurrentThreadId());
		if(!this->buildFunction(buf, luaString, name1, name2, name3))
		{
			return _type_default_value_<RVal>::get();
		}
		return this->Call<RVal,T1,T2,T3>(buf, arg1, arg2, arg3);
	}

	template <typename RVal,typename T1,typename T2, typename T3, typename T4>
	RVal callAnonymous(const char* luaString
		, const char* name1, T1 arg1
		, const char* name2, T2 arg2
		, const char* name3, T3 arg3
		, const char* name4, T4 arg4
		)
	{
		char buf[64];
		sprintf(buf, "__anonymous%llu__", getCurrentThreadId());
		if(!this->buildFunction(buf, luaString, name1, name2, name3, name4))
		{
			return _type_default_value_<RVal>::get();
		}
		return this->Call<RVal,T1,T2,T3,T4>(buf, arg1, arg2, arg3, arg4);
	}

	template <typename RVal,typename T1,typename T2, typename T3, typename T4, typename T5>
	RVal callAnonymous(const char* luaString
		, const char* name1, T1 arg1
		, const char* name2, T2 arg2
		, const char* name3, T3 arg3
		, const char* name4, T4 arg4
		, const char* name5, T5 arg5
		)
	{
		char buf[64];
		sprintf(buf, "__anonymous%llu__", getCurrentThreadId());
		if(!this->buildFunction(buf, luaString, name1, name2, name3, name4, name5))
		{
			return _type_default_value_<RVal>::get();
		}
		return this->Call<RVal,T1,T2,T3,T4,T5>(buf, arg1, arg2, arg3, arg4, arg5);
	}

	// 有返回的全局函数调用,支持嵌套表中的函数
	template<typename RVal> RVal Call(const char* name) const { return LMREG::call<RVal>(m_l, name); }
	template<typename RVal, typename T1> RVal Call(const char* name, const T1& a1) const { return LMREG::call<RVal>(m_l, name, a1); }
	template<typename RVal, typename T1, typename T2> RVal Call(const char* name, const T1& a1, const T2& a2) const { return LMREG::call<RVal>(m_l, name, a1, a2); }
	template<typename RVal, typename T1, typename T2, typename T3> RVal Call(const char* name, const T1& a1, const T2& a2, const T3& a3) const { return LMREG::call<RVal>(m_l, name, a1, a2, a3); }
	template<typename RVal, typename T1, typename T2, typename T3, typename T4> RVal Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4) const { return LMREG::call<RVal>(m_l, name, a1, a2, a3, a4); }
	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5> RVal Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5) const { return LMREG::call<RVal>(m_l, name, a1, a2, a3, a4, a5); }
	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> RVal Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6) const { return LMREG::call<RVal>(m_l, name, a1, a2, a3, a4, a5, a6); }
	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> RVal Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7) const { return LMREG::call<RVal>(m_l, name, a1, a2, a3, a4, a5, a6, a7); }
	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> RVal Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7, const T8& a8) const { return LMREG::call<RVal>(m_l, name, a1, a2, a3, a4, a5, a6, a7, a8); }
	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9> RVal Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7, const T8& a8, const T9& a9) const { return LMREG::call<RVal>(m_l, name, a1, a2, a3, a4, a5, a6, a7, a8, a9); }
	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10> RVal Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10) const { return LMREG::call<RVal>(m_l, name, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }
	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11> RVal Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10, const T11& a11) const { return LMREG::call<RVal>(m_l, name, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11); }
	template<typename RVal, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12> RVal Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10, const T11& a11, const T12& a12) const { return LMREG::call<RVal>(m_l, name, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12); }

	// 无返回的全局函数调用,支持嵌套表中的函数
	void Call(const char* name) const { LMREG::call<void>(m_l, name); }
	template<typename T1> void Call(const char* name, const T1& a1) const { LMREG::call<void>(m_l, name, a1); }
	template<typename T1, typename T2> void Call(const char* name, const T1& a1, const T2& a2) const { LMREG::call<void>(m_l, name, a1, a2); }
	template<typename T1, typename T2, typename T3> void Call(const char* name, const T1& a1, const T2& a2, const T3& a3) const { LMREG::call<void>(m_l, name, a1, a2, a3); }
	template<typename T1, typename T2, typename T3, typename T4> void Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4) const { LMREG::call<void>(m_l, name, a1, a2, a3, a4); }
	template<typename T1, typename T2, typename T3, typename T4, typename T5> void Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5) const { LMREG::call<void>(m_l, name, a1, a2, a3, a4, a5); }
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6> void Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6) const { LMREG::call<void>(m_l, name, a1, a2, a3, a4, a5, a6); }
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7> void Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7) const { LMREG::call<void>(m_l, name, a1, a2, a3, a4, a5, a6, a7); }
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8> void Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7, const T8& a8) const { LMREG::call<void>(m_l, name, a1, a2, a3, a4, a5, a6, a7, a8); }
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9> void Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7, const T8& a8, const T9& a9) const { LMREG::call<void>(m_l, name, a1, a2, a3, a4, a5, a6, a7, a8, a9); }
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10> void Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10) const { LMREG::call<void>(m_l, name, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11> void Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10, const T11& a11) const { LMREG::call<void>(m_l, name, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11); }
	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12> void Call(const char* name, const T1& a1, const T2& a2, const T3& a3, const T4& a4, const T5& a5, const T6& a6, const T7& a7, const T8& a8, const T9& a9, const T10& a10, const T11& a11, const T12& a12) const { LMREG::call<void>(m_l, name, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12); }

	template<typename T>
	void AddMtlRetVal(T t)	{ LMREG::push(m_l, t); }
	int RetMtlValNum(int n)	{ LMREG::push(m_l, n); return LMREG::cn_MtlRetFlag; }

	const char* GetLastError(void) const;
	inline lua_State* GetState(void) { return m_l; }
	inline void SetState(lua_State* ls) { m_l = ls; }
	inline lua_State* GetMainState() { return m_mainLS; }
	int SetPrintOutput(FILE* f);

private:
	lua_State* m_mainLS;
	///************************************************************************/
	///* 脚本调用传参优化部分                                                 */
	///************************************************************************/
public:
	///**
	// * 用以处理使用一个已经存在的表作为传参用，请GetTable/ReleaseTable成对使用
	// * ！注意：GetTable会将tab压入栈，ReleaseTable不会改变栈的状态，所以需要用户手动恢复lua栈
	// * 可能的优化：不再使用这种专用栈结构而使用table对象池
	// */
	class KStackTabList
	{
	public:
		///**
		// * 得到一个Table，将该Tab压入state的顶部，如果失败会压入一个nil
		// * ！注意，禁止保存该表。该表的使用不能过Breathe
		// */
		void GetTable(lua_State* l)
		{
			StackData* pToOp = NULL;
			if (LuaWraper.GetMainState() == l)
			{
				pToOp = &m_mainData;
			}
			else
			{
				DATA_MAP_TYPE::iterator it = m_stackDataMap.find(l);
				if (it == m_stackDataMap.end())
				{
					pToOp = StackData::Alloc();
					m_stackDataMap.insert_unique(l, pToOp);
				}
				else
				{
					pToOp = it->second;
				}
			}
			// 检查是否需要新建table
			if (pToOp->m_currentStack == pToOp->m_tableIDList.size())	// TODO: 不可能大于
			{
				lua_createtable(l, 0, 8);	// 预留8个位置（猜测参数一般不超过8），栈顶压入一个新table
				int tabID = luaL_ref(l, LUA_REGISTRYINDEX);		// 将栈顶table挪入寄存表，得到id
				pToOp->m_tableIDList.push_back(tabID);		// 保留该id
			}
			lua_rawgeti(l, LUA_REGISTRYINDEX, pToOp->m_tableIDList[pToOp->m_currentStack++]);	// 将指定位置的表压入栈顶，并且增加currentStack计数
		}

		/**
		 * 释放一层EnvTable的引用，不改变lua栈状态
		 */
		void ReleaseTable(lua_State* l)
		{
			if (LuaWraper.GetMainState() == l)
			{
				m_mainData.m_currentStack--;
			}
			else
			{
				DATA_MAP_TYPE::iterator it = m_stackDataMap.find(l);
				ASSERT_RETURN_VOID(it != m_stackDataMap.end() && it->second->m_currentStack > 0);
				it->second->m_currentStack--;
			}
		}

		///**
		// * 使用该功能的lua_State被释放的时候的通知
		// */
		void OnReleaseLuaState(lua_State* l)
		{
			if (LuaWraper.GetMainState() == l)
			{
				// 考虑到lua_State都要释放了，就没有必要清理lua_State下面的东西了。
//				StackData* pToOp = &m_mainData;
// 
// 				for (int i = 0; i < pToOp->m_tableIDList.size(); i++)
// 				{
// 					luaL_unref(l, LUA_REGISTRYINDEX, pToOp->m_tableIDList[i]);
// 				}
			}
			else
			{
				DATA_MAP_TYPE::iterator it = m_stackDataMap.find(l);
				if (it != m_stackDataMap.end())
				{
// 					StackData* pToOp = it->second;
// 
// 					for (int i = 0; i < pToOp->m_tableIDList.size(); i++)
// 					{
// 						luaL_unref(l, LUA_REGISTRYINDEX, pToOp->m_tableIDList[i]);
// 					}
					StackData::Free(it->second);
					m_stackDataMap.erase(it);
				}
			}
		}

	private:
		struct StackData
			: ::System::Memory::KPortableMemoryPool<StackData>
		{
			StackData()
				: m_currentStack(0)
			{}
			// 当前递归到使用第几层table
			int m_currentStack;
			// 已经创建的table列表
			::System::Collections::DynArray<int, 4, 4> m_tableIDList;
		};
		// 由于HashTable效率问题，这里主State用单独一个处理
		StackData m_mainData;
		// 根据lua_State索引找到指定的tab组
		typedef ::System::Collections::KHashTable<lua_State*
			, StackData*
			, KPointerCompare<lua_State*>
			, KPointerHasher<lua_State*>
			, ::System::Sync::KMTLock
			, 2048
			, 2048> DATA_MAP_TYPE;
		DATA_MAP_TYPE m_stackDataMap;
	}m_fastEnvTable;
};

// 所有导出到LUA的C++类的基类
template <typename SubClass> class KBaseLuaClass
{
public:
	static int m_udataRef;
	typedef KBaseLuaClass<SubClass> class_type;

public:
	KBaseLuaClass()
	{
		lua_State* L = LuaWraper.m_l;

		int sp = lua_gettop(L);

		SubClass* ptr = (SubClass*)this;
		void* addr = ptr;

		lua_loadregistryref(L, class_type::m_udataRef);

		lua_pushlightuserdata(L, addr); //lua_pushnumber(L, addr);
		void* p = lua_newuserdata(L, sizeof(LMREG::ptr2user<SubClass>));
		new (p) LMREG::ptr2user<SubClass>(ptr);

		LMREG::push_meta(L, LMREG::class_name<typename LMREG::class_type<SubClass>::type>::name());
		lua_setmetatable(L, -2);

		lua_settable(L, -3);
		lua_settop(L, sp);
	}

	~KBaseLuaClass()
	{
		lua_State* L = LuaWraper.m_l;
		int sp = lua_gettop(L);

		SubClass* ptr = (SubClass*)this;
		void* addr = ptr;

		lua_loadregistryref(L, class_type::m_udataRef);

		lua_pushlightuserdata(L, addr); //lua_pushnumber(L, addr);
		lua_pushnil(L);
		lua_settable(L, -3);

		lua_settop(L, sp);
	}
};

template <typename T> int KBaseLuaClass<T>::m_udataRef = 0;

//// 从 KBaseLuaClass 继承的类 ptr2lua
#define __luaClassPtr2Lua(T) \
namespace LMREG { \
	template<> struct ptr2lua<T> \
	{ \
		static void invoke(lua_State *L, T* input) \
		{ \
			if(input) \
			{ \
				lua_getobjudata(L,LMREG::class_type<T>::type, input); \
			} \
			else \
			{ \
				lua_pushnil(L); \
			} \
		} \
	}; \
};
#define luaClassPtr2Lua(T) \
	__luaClassPtr2Lua(T) \
	__luaClassPtr2Lua(const T)

/// 

//DWORD LUAWARP_API ELFHash(const char*str);
//char* GetFullPath(char* cafile, char* cafullfile);
void Utc2Local(int& _y, int& _m, int& _d, int& _h);

///////////////////定义lua类
#define BeginDefLuaClass(c) \
template <int X> static void luaRegister(lua_State* L) \
{ \
	int _x_ = X; \
	typedef c cls; \
	LMREG::class_add<cls>(L, #c); \
	LMREG::class_con<cls>(L,LMREG::constructor<cls>); \

#define BeginDefLuaClassNoCon(c)									\
template <int X> static void luaRegister(lua_State* L)			\
{																	\
	int _x_ = X; \
	typedef c cls;													\
	LMREG::class_add<cls>(L, #c);								\

#define BeginDefLuaClassNoneGc(c) \
template <int X> static void luaRegister(lua_State* L) \
{ \
	int _x_ = X; \
	typedef c cls; \
	LMREG::class_add<cls>(L, #c, 0); \
	LMREG::class_con<cls>(L,LMREG::constructor<cls>); \

#define DefMemberFunc_x(f) \
	LMREG::class_def_x<cls>(L, #f, &cls::f); \

#define DefMemberFunc(f) \
	LMREG::class_def<cls>(L, #f, &cls::f); \

#define DefMemberFunc2_x(f,name) \
	LMREG::class_def_x<cls>(L, name, &cls::f); \

#define DefMemberFunc2(f,name) \
	LMREG::class_def<cls>(L, name, &cls::f); \

#define DefMemberFunc3_x(f,name) \
	LMREG::class_def_x<cls>(L, name, &f); \

#define DefMemberFunc3(f,name) \
	LMREG::class_def<cls>(L, name, &f); \

#define DefMemberVar(v) \
	LMREG::class_mem<cls>(L, #v, &cls::v); \

#define DefMemberVar2(v,name) \
	LMREG::class_mem<cls>(L, name, &cls::v); \

#define InheritFrom(bc) \
	LMREG::class_inh<cls, bc>(L); \


///////////////////////使用别名定义lua类
#define BeginDefLuaClassA(c, nm) \
template <int X> static void luaRegister(lua_State* L) \
{ \
	int _x_ = X; \
	typedef c cls; \
	LMREG::class_add<cls>(L, #nm); \
	LMREG::class_con<cls>(L,LMREG::constructor<cls>); \

#define DefMemberFuncA(f, nm) \
	LMREG::class_def<cls>(L, #nm, &cls::f); \

#define DefMemberVarA(v, nm) \
	LMREG::class_mem<cls>(L, #nm, &cls::v); \

#define EndDef }
/////////////////////////////////////////

#define LuaPush(x) LMREG::push(LuaWraper.m_l, x)
#define LuaPushWithType(x,y) LMREG::push(LuaWraper.m_l, x, y)
#define LuaPop() lua_pop(LuaWraper.m_l, 1)
#define LuaRead() LMREG::pop(LuaWraper.m_l)

//#define LuaWraper (*(KLuaWrap::GetInstance()))

#if defined(LUA_BUILD_AS_DLL)
	#define CREATE_LUAWRAPPER() \
		KLuaWrap* g_pLuaWrapper = new KLuaWrap();
#else
	#define CREATE_LUAWRAPPER()
#endif

#define REGISTER_LUA_CLASS(T) \
	T::luaRegister<0>(LuaWraper.m_l);

#define REGISTER_LUA_CLASS_2(T) \
	{ \
		lua_State* L = LuaWraper.m_l; \
		T::luaRegister<0>(L); \
		lua_newtable(L); \
		lua_pushvalue(L, -1); \
		KBaseLuaClass<T>::m_udataRef = luaL_ref(L, LUA_REGISTRYINDEX); \
		LMREG::push_meta(L, LMREG::class_name<T>::name()); ASSERT(lua_istable(L, -1)); \
		lua_pushstring(L, "__ref"); \
		lua_pushvalue(L, -3); \
		lua_settable(L, -3); \
		lua_pop(L, 2); \
	}

// 直接导出类到lua，填(类，名字，lua的State)，创建默认构造
#define BeginExportLuaClassDirectly(classname, c, luastate)			\
{																	\
	typedef classname cls;											\
	lua_State* L = luastate;										\
	LMREG::class_add<cls>(L, #c);								\
	LMREG::class_con<cls>(L, LMREG::constructor<cls>);

///************************************************************************/
///* 直接执行脚本的扩展需求 by Allen, 20120925, 感谢程晓的大力支持 ^_^    */
///************************************************************************/
///*
//用例1：导出个数值让脚本可以直接拿来用。常用脚本，一般用来处理事件
//
//const char* script = "print(val)";
//// 取LuaWraper.GetState()做lua的运行环境，执行script脚本，环境中给入1个环境变量（1只是预计的优化值）
//ExecuteScriptBegin(LuaWraper.GetState(), script, 1);
//	ExecuteScriptPush("val", 12345);
//ExecuteScriptEnd;
//
//用例2：导出数值让脚本用，并且从脚本中拿返回值。常用脚本，一般用来处理强化计算
//
//const char* script = "print(val);return val + 10";
//// 取LuaWraper.GetState()做lua的运行环境，执行script脚本，返回值类型为int，创建个变量叫result来接返回值，环境中给入1个环境变量（1只是预计的优化值）
//ExecuteScriptBeginWithResult(LuaWraper.GetState(), script, int, result, 1);
//	ExecuteScriptPush("val", 12345);
//ExecuteScriptEnd;
//
//用例3：导出个数值让脚本可以直接拿来用。其中导出的数值是需要进行加工的（如KCharacter可能被转成KPlayer再导出的这种加工）
//	这样就需要先压一个类型的东西进栈，再赋予其名字
//
//// 假设已经有了个pChar为KCharacter类型，现要用个加工将其转为实际类型（通过使用函数PushCharToLuaStateByType来做）
//const char* script = "print(val)";
//ExecuteScriptBegin(LuaWraper.GetState(), script, 1);
//	// 在ExecuteScriptBegin和ExecuteScriptEnd宏中间会创建个叫"l"的变量，代表当前lua的state，可以直接用
//	KScriptAbout::PushCharToLuaStateByType(l, pChar);
//	ExecuteScriptSet("char");
//ExecuteScriptEnd;
//
//特别说明：
//	组合宏在执行过程中使用"_____"（5个下划线）开头的变量做为局部变量使用，加入的代码请避免重名，否则可能产生未知结果，最可能是编译不过。
//	脚本端为了速度优化，全局创建了一个名字叫做"__"（2个下划线）的全局table，请不要用做其它用途。
//
//	ExecuteScriptBeginWithEnvTable
//	ExecuteScriptBeginWithResultAndEnvTable
//	以上两个宏适合已经自定义好环境table的情况
//*/

///**
// * 脚本开始的辅助宏，不要直接使用该宏，只是为了省写代码而定义的
// */
#define ExecuteScriptBeingAssistMarco(luaState, script, RESULT_TYPE, result, pCount)	\
	typedef RESULT_TYPE RT;	\
	RT& _____result = result;	\
	lua_State* l = luaState;	\
	const char* _____scriptString = script;	\
	int _____paramCount = pCount;	\
	int _____stackToRestorePos = lua_gettop(l);	\
	bool _____execResult = false;	\
	bool _____useCustomEnvTable = false;	\
	::System::Collections::DynArray<const char*> _____envVarList;	\
	luaL_loadstring(l, _____scriptString);	\
	if (!lua_isfunction(l, -1))	\
	{	\
		const char* le = NULL;	\
		if (LuaWraper.m_funcErrReport && (le = lua_tostring(l, -1)))	\
		{	\
			LuaWraper.m_funcErrReport(l);	\
		}	\
		lua_pop(l, 1);	\
	}	\
	else	\
	{	\
		int _____funcPos = lua_gettop(l);	\
		LuaWraper.m_fastEnvTable.GetTable(l);	\
		_____useCustomEnvTable = true;	\
		/*lua_createtable(l, 0, _____paramCount);*/	\
		if (!lua_istable(l, -1))	\
		{	\
			lua_pushstring(l, "Error in creating environment table!");	\
			LuaWraper.m_funcErrReport(l);	\
			lua_pop(l, 1);	\
		}	\
		else	\
		{	\
			int _____tablePos = lua_gettop(l);

///**
// * 直接执行脚本宏开始，无返回值
// * param list:
// *		luaState	lua_State指针
// *		script		const char*，要执行的脚本字符串，以\0结尾
// *		pCount		优化参数，最好填的数量与ExecuteScriptPush出去的数量相符，当然填0也可以
// */
#define ExecuteScriptBegin(luaState, script, pCount)	\
{	\
	int _____noresult;	\
	ExecuteScriptBeingAssistMarco(luaState, script, int, _____noresult, pCount)

///**
// * 直接执行脚本宏开始，并且可以接返回值
// * param list:
// *		luaState	lua_State指针
// *		script		const char*，要执行的脚本字符串，以\0结尾
// *		RESULT_TYPE	结果类型（如返回有符号32位整数，就填int，字符串请使用LuaString，而不要用const char*）
// *		result		结果的变量名。会用RESULT_TYPE result;创建一个变量
// *		pCount		优化参数，最好填的数量与ExecuteScriptPush出去的数量相符，当然填0也可以
// */
#define ExecuteScriptBeginWithResult(luaState, script, RESULT_TYPE, result, pCount)	\
RESULT_TYPE result;	\
{	\
	ExecuteScriptBeingAssistMarco(luaState, script, RESULT_TYPE, result, pCount)

///**
// * 脚本开始的辅助宏，可以使用自定义table做为envrionment table的格式，要求该table已经被压入栈。不要直接使用该宏，只是为了省写代码而定义的
// */
#define ExecuteScriptBeingAssistMarcoWithEnvTable(luaState, script, RESULT_TYPE, result, envTableIndex)	\
	typedef RESULT_TYPE RT;	\
	RT& _____result = result;	\
	lua_State* l = luaState;	\
	const char* _____scriptString = script;	\
	int _____envTableIndex = envTableIndex;	\
	int _____stackToRestorePos = lua_gettop(l);	\
	bool _____execResult = false;	\
	bool _____useCustomEnvTable = false;	\
	::System::Collections::DynArray<const char*> _____envVarList;	\
	luaL_loadstring(l, _____scriptString);	\
	if (!lua_isfunction(l, -1))	\
	{	\
		const char* le = NULL;	\
		if (LuaWraper.m_funcErrReport && (le = lua_tostring(l, -1)))	\
		{	\
			LuaWraper.m_funcErrReport(l);	\
		}	\
		lua_pop(l, 1);	\
	}	\
	else	\
	{	\
		int _____funcPos = lua_gettop(l);	\
		lua_pushvalue(l, _____envTableIndex);	\
		if (!lua_istable(l, -1))	\
		{	\
			lua_pushstring(l, "Error in finding environment table!");	\
			LuaWraper.m_funcErrReport(l);	\
			lua_pop(l, 2);	\
		}	\
		else	\
		{	\
			int _____tablePos = lua_gettop(l);

///**
// * 直接执行脚本宏开始，无返回值，使用指定的environment table
// * param list:
// *		luaState		lua_State指针
// *		script			const char*，要执行的脚本字符串，以\0结尾
// *		envTableIndex	已经存在的environment table在栈中的索引
// */
#define ExecuteScriptBeginWithEnvTable(luaState, script, envTableIndex)	\
{	\
	int _____noresult;	\
	ExecuteScriptBeingAssistMarcoWithEnvTable(luaState, script, int, _____noresult, envTableIndex)

///**
// * 直接执行脚本宏开始，并且可以接返回值，使用指定的environment table
// * param list:
// *		luaState	lua_State指针
// *		script		const char*，要执行的脚本字符串，以\0结尾
// *		RESULT_TYPE	结果类型（如返回有符号32位整数，就填int，字符串请使用LuaString，而不要用const char*）
// *		result		结果的变量名。会用RESULT_TYPE result;创建一个变量
// *		envTableIndex	已经存在的environment table在栈中的索引
// */
#define ExecuteScriptBeginWithResultAndEnvTable(luaState, script, RESULT_TYPE, result, envTableIndex)	\
RESULT_TYPE result;	\
{	\
	ExecuteScriptBeingAssistMarcoWithEnvTable(luaState, script, RESULT_TYPE, result, envTableIndex)

/**
 * 脚本开始的辅助宏，不要直接使用该宏，只是为了省写代码而定义的
 */
#define ExecuteScriptBeingAssistMarcoByID(luaState, funcID, RESULT_TYPE, result)	\
	typedef RESULT_TYPE RT;	\
	RT& _____result = result;	\
	lua_State* l = luaState;	\
	int _____stackToRestorePos = lua_gettop(l);	\
	int _____funcID = funcID;	\
	bool _____execResult = false;	\
	bool _____useCustomEnvTable = false;	\
	::System::Collections::DynArray<const char*> _____envVarList;	\
	lua_rawgeti(l, LUA_REGISTRYINDEX, _____funcID);	\
	if (!lua_isfunction(l, -1))	\
	{	\
		const char* le = NULL;	\
		if (LuaWraper.m_funcErrReport && (le = lua_tostring(l, -1)))	\
		{	\
			LuaWraper.m_funcErrReport(l);	\
		}	\
		lua_pop(l, 1);	\
	}	\
	else	\
	{	\
		int _____funcPos = lua_gettop(l);	\
		LuaWraper.m_fastEnvTable.GetTable(l);	\
		_____useCustomEnvTable = true;	\
		if (!lua_istable(l, -1))	\
		{	\
			lua_pushstring(l, "Error in creating environment table!");	\
			LuaWraper.m_funcErrReport(l);	\
			lua_pop(l, 1);	\
		}	\
		else	\
		{	\
			int _____tablePos = lua_gettop(l);

///**
// * 直接执行脚本宏开始，无返回值
// * param list:
// *		luaState	lua_State指针
// *		funcID		int，要执行的脚本函数在寄存表中的索引
// *		pCount		优化参数，最好填的数量与ExecuteScriptPush出去的数量相符，当然填0也可以
// */
#define ExecuteScriptBeginByID(luaState, funcID)	\
{	\
	int _____noresult;	\
	ExecuteScriptBeingAssistMarcoByID(luaState, funcID, int, _____noresult)

///**
// * 直接执行脚本宏开始，并且可以接返回值
// * param list:
// *		luaState	lua_State指针
// *		funcID		int，要执行的脚本函数在寄存表中的索引
// *		RESULT_TYPE	结果类型（如返回有符号32位整数，就填int，字符串请使用LuaString，而不要用const char*）
// *		result		结果的变量名。会用RESULT_TYPE result;创建一个变量
// *		pCount		优化参数，最好填的数量与ExecuteScriptPush出去的数量相符，当然填0也可以
// */
#define ExecuteScriptBeginWithResultByID(luaState, funcID, RESULT_TYPE, result)	\
RESULT_TYPE result;	\
{	\
	ExecuteScriptBeingAssistMarcoByID(luaState, funcID, RESULT_TYPE, result)

///**
// * 脚本开始的辅助宏，可以使用自定义table做为envrionment table的格式，要求该table已经被压入栈。不要直接使用该宏，只是为了省写代码而定义的
// */
#define ExecuteScriptBeingAssistMarcoWithEnvTableByID(luaState, funcID, RESULT_TYPE, result, envTableIndex)	\
	typedef RESULT_TYPE RT;	\
	RT& _____result = result;	\
	lua_State* l = luaState;	\
	int _____funcID = funcID;	\
	int _____envTableIndex = envTableIndex;	\
	int _____stackToRestorePos = lua_gettop(l);	\
	bool _____execResult = false;	\
	bool _____useCustomEnvTable = false;	\
	::System::Collections::DynArray<const char*> _____envVarList;	\
	lua_rawgeti(l, LUA_REGISTRYINDEX, _____funcID);	\
	if (!lua_isfunction(l, -1))	\
	{	\
		const char* le = NULL;	\
		if (LuaWraper.m_funcErrReport && (le = lua_tostring(l, -1)))	\
		{	\
			LuaWraper.m_funcErrReport(l);	\
		}	\
		lua_pop(l, 1);	\
	}	\
	else	\
	{	\
		int _____funcPos = lua_gettop(l);	\
		lua_pushvalue(l, _____envTableIndex);	\
		if (!lua_istable(l, -1))	\
		{	\
			lua_pushstring(l, "Error in finding environment table!");	\
			LuaWraper.m_funcErrReport(l);	\
			lua_pop(l, 2);	\
		}	\
		else	\
		{	\
			int _____tablePos = lua_gettop(l);

///**
// * 直接执行脚本宏开始，无返回值，使用指定的environment table
// * param list:
// *		luaState		lua_State指针
// *		funcID		int，要执行的脚本函数在寄存表中的索引
// *		envTableIndex	已经存在的environment table在栈中的索引
// */
#define ExecuteScriptBeginWithEnvTableByID(luaState, funcID, envTableIndex)	\
{	\
	int _____noresult;	\
	ExecuteScriptBeingAssistMarcoWithEnvTableByID(luaState, funcID, int, _____noresult, envTableIndex)

///**
// * 直接执行脚本宏开始，并且可以接返回值，使用指定的environment table
// * param list:
// *		luaState	lua_State指针
// *		funcID		int，要执行的脚本函数在寄存表中的索引
// *		RESULT_TYPE	结果类型（如返回有符号32位整数，就填int，字符串请使用LuaString，而不要用const char*）
// *		result		结果的变量名。会用RESULT_TYPE result;创建一个变量
// *		envTableIndex	已经存在的environment table在栈中的索引
// */
#define ExecuteScriptBeginWithResultAndEnvTableByID(luaState, funcID, RESULT_TYPE, result, envTableIndex)	\
RESULT_TYPE result;	\
{	\
	ExecuteScriptBeingAssistMarcoWithEnvTableByID(luaState, funcID, RESULT_TYPE, result, envTableIndex)

///**
// * 直接执行脚本宏向环境压入数据并且设置好名字
// * param list:
// *		name		字符串，要在脚本中直接使用的名字
// *		val			LuaWraper支持的任意类型，要导出的数值
// */
#define ExecuteScriptPush(name, val)	\
			LMREG::push(l, val);	\
			lua_setfield(l, _____tablePos, name);	\
			if (_____useCustomEnvTable) _____envVarList.push_back(name);


///**
// * 直接执行脚本宏，在压入数据之后，将栈顶数据赋予一个名字转给环境
// * param list:
// *		name		将当前栈顶元素按name指定的名字导出到脚本
// */
#define ExecuteScriptSet(name)	\
			lua_setfield(l, _____tablePos, name);	\
			if (_____useCustomEnvTable) _____envVarList.push_back(name);

///**
// * 直接执行脚本宏结束部分
// */
#define ExecuteScriptEnd	\
			if (_____useCustomEnvTable)	\
			{	\
				lua_pushvalue(l, _____funcPos);	\
				lua_pushvalue(l, _____tablePos);	\
			}	\
			bool _____findMetatable = false;	\
			lua_pushstring(l, "__");	\
			lua_gettable(l, LUA_GLOBALSINDEX);	\
			if (lua_isnoneornil(l, -1))	\
			{	\
				lua_pop(l, 1);	\
				if (0 != luaL_dostring(l, "__={__index = _G}"))	\
				{	\
					const char* le = NULL;	\
					if (LuaWraper.m_funcErrReport && (le = lua_tostring(l, -1)))	\
					{	\
						LuaWraper.m_funcErrReport(l);	\
					}	\
					lua_pop(l, 1);	\
				}	\
				else	\
				{	\
					lua_pushstring(l, "__");	\
					lua_gettable(l, LUA_GLOBALSINDEX);	\
					if (lua_isnoneornil(l, -1))	\
					{	\
						lua_pop(l, 1);	\
					}	\
					else	\
					{	\
						_____findMetatable = true;	\
					}	\
				}	\
			}	\
			else	\
			{	\
				_____findMetatable = true;	\
			}	\
			if (_____findMetatable)	\
			{	\
				lua_setmetatable(l, _____tablePos);	\
				lua_setfenv(l, _____funcPos);	\
				if (0 != lua_pcall(l, 0, 1, 0))	\
				{	\
					const char* le = NULL;	\
					if (LuaWraper.m_funcErrReport && (le = lua_tostring(l, -1)))	\
					{	\
						LuaWraper.m_funcErrReport(l);	\
					}	\
					lua_pop(l, 1);	\
				}	\
				else	\
				{	\
					_____execResult = true;	\
				}	\
			}	\
			if (_____useCustomEnvTable)	\
			{	\
				LuaWraper.m_fastEnvTable.ReleaseTable(l);	\
				for (int i = 0; i < _____envVarList.size(); i++)	\
				{	\
					lua_pushnil(l);	\
					lua_setfield(l, _____tablePos, _____envVarList[i]);	\
				}	\
			}	\
		}	\
		_____result = LMREG::retset<RT>(l, -1);	\
		lua_settop(l, _____stackToRestorePos);	\
	}	\
}

///**
// * 脚本函数管理器，用以处理需要doString来运行的lua脚本，提供预编译
// */
class KBufferScriptFuncManager
{
public:
	KBufferScriptFuncManager()
		: m_l(NULL)
	{}
	virtual ~KBufferScriptFuncManager()
	{
		Clear();
	}
	///**
	// * 新增一个函数
	// */
	int Add(const char* script)
	{
		if (!script) return 0;
		int funcID = 0;
		KLuaStackRecover _____(m_l);
		int result = luaL_loadstring(m_l, script);
		if (result)
		{
			if(LuaWraper.m_funcErrReport) LuaWraper.m_funcErrReport(m_l);
			ASSERT_I(false);
			return 0;
		}
		funcID = luaL_ref(m_l, LUA_REGISTRYINDEX);
		if (funcID == LUA_REFNIL || funcID == LUA_NOREF)
		{
			ASSERT_I(false);
			return 0;
		}
		m_addList.push_back(funcID);
		return funcID;
	}
	///**
	// * Call一个函数，带返回值，如果出错就用T的构造返回
	// */
	template <typename T>
	T Call(int id)
	{
		KLuaStackRecover _____(m_l);
		int errfunc = LMREG::lua_geterrfunc(m_l);

		lua_rawgeti(m_l, LUA_REGISTRYINDEX, id);

		if (!lua_isfunction(m_l, -1))
		{
			// 不是函数
			LMREG::print_error(m_l, "KBufferScriptFuncManager::call() attempt to call table function `%d' (not a function)", id);
		}
		else if (lua_pcall(m_l, 0, 1, errfunc))
		{
			// 出错
			LMREG::print_error(m_l, "KBufferScriptFuncManager::call() call fail `%d'", id);
		}
		else
		{
			return (LMREG::read<T>(m_l, -1));
		}
		ASSERT_I(false);
		return T();
	}
	void Call(int id)
	{
		KLuaStackRecover _____(m_l);
		int errfunc = LMREG::lua_geterrfunc(m_l);

		lua_rawgeti(m_l, LUA_REGISTRYINDEX, id);

		if (!lua_isfunction(m_l, -1))
		{
			// 不是函数
			LMREG::print_error(m_l, "KBufferScriptFuncManager::call() attempt to call table function `%d' (not a function)", id);
		}
		else if (lua_pcall(m_l, 0, 0, errfunc))
		{
			// 出错
			LMREG::print_error(m_l, "KBufferScriptFuncManager::call() call fail `%d'", id);
		}
		ASSERT_I(false);
	}
	///**
	// * 设置lua栈
	// */
	void ChangeLuaState(lua_State* l)
	{
		if (!l || m_l == l) return;
		Clear();
		m_l = l;
	}
	///**
	// * 清理所有已注册函数
	// */
	void Clear()
	{
		for (int i = 0; i < m_addList.size(); i++)
		{
			luaL_unref(m_l, LUA_REGISTRYINDEX, m_addList[i]);
		}
	}
	///**
	// * 得到关联的lua_State
	// */
	lua_State* GetLuaState()
	{
		return m_l;
	}
protected:
	// 验证比较费效率，只是用以删除释放用
	::System::Collections::DynArray<int> m_addList;
	// 保留个操作state的指针
	lua_State* m_l;
};

///**
// * 默认的lua用来doString的脚本的管理器
// */
class KDefaultBufferScriptFuncManager
	: public KBufferScriptFuncManager
	, public ::System::Patterns::KSingleton<KDefaultBufferScriptFuncManager>
{
public:
	KDefaultBufferScriptFuncManager()
	{
		m_l = LuaWraper.GetMainState();
	}
};

#endif