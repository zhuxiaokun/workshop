#ifndef _CPP_TABLE_H_
#define _CPP_TABLE_H_

#include <System/KType.h>
#include <System/KMacro.h>
#include <System/Collections/KString.h>
#include <System/Collections/DynArray.h>

#pragma warning(disable:4275)

#if defined(__cplusplus) && (defined(WIN32) || defined(__clang__))
extern "C"
{
#endif
#include "Lua/lauxlib.h"
#include "Lua/lua.h"
#include "Lua/lualib.h"
#include "Lua/luaconf.h"
#include "Lua/lstate.h"
#include "Lua/lundump.h"

#if defined(__cplusplus) && (defined(WIN32) || defined(__clang__))
}
#endif 

#if defined(LUA_BUILD_AS_DLL)
#if defined(LUA_CORE) || defined(LUA_LIB)
#define LUAWARP_API __declspec(dllexport)
#else
#define LUAWARP_API __declspec(dllimport)
#endif
#else
#define LUAWARP_API
#endif

namespace LMREG
{
	// metatable(结构类型数据)专用。目前这个table不使用number作为key
	// 所以这里起用一些固定ID共呢国内
	enum CLASSMETAEXTEDTAG
	{
		CLASSMETA_PTR2USERDATA = 1,						// ptr(结构类型指针)到userdata的对应表
		CLASSMETA_INHERITLEVEL = 2,						// 继承层次的ID，顶点基类为nil
	};

	template<typename T> struct class_name
	{
		static const char* name(const char* name = NULL)
		{
			static char temp[256] = "";
			if(name)
			{
				strcpy_k(temp, sizeof(temp), name);
			}
			return temp[0] ? temp : 0;
		}
	};

	class CppTable;
	struct LUAWARP_API variant
	{
	friend class _CppTable_base;
	public:
		enum
		{
			stack_string_len = 256,
		};
		enum enum_data_type
		{
			vt_nil,
			vt_bool,
			vt_int32,
			vt_uint32,
			vt_int64,
			vt_uint64,
			vt_float,
			vt_double,
			vt_pointer,
			vt_string,		// length than stack_string_len, static string
			vt_d_string,	// more than stack_string_len, dynamic string
			vt_table,
			vt_object,		// class object that export to lua
		};

		typedef char ObjectName[64];
		template <typename A,typename B> struct Pair
		{
			A first;
			B second;
		};

	public:
		enum_data_type vt;
		union
		{
			bool b;
			int n;
			unsigned int un;
			long l;
			unsigned long ul;
			long long ll;
			unsigned long long ull;
			float f;
			double d;
			Pair<int,char[stack_string_len]> str;
			Pair<int,char*> dstr;
			CppTable* tab;
			Pair<ObjectName,void*> obj;
			void*	p;
		};

	public:
		variant();
		variant(const variant& o);
		~variant();

	public:
		template<size_t ln> JG_C::KDString<ln> toString() const
		{
			typedef JG_C::KDString<ln> s_t;
			switch(vt)
			{
			case vt_nil:
				{
					return s_t("nil");
				} break;
			case vt_bool:
				{
					return s_t(this->b ? "true":"false");
				} break;
			case vt_int32:
				{
					char buf[64];
					int n = sprintf_k(buf, sizeof(buf), "%d", this->n);
					return s_t(buf, n);
				} break;
			case vt_uint32:
				{
					char buf[64];
					int n = sprintf_k(buf, sizeof(buf), "%u", this->un);
					return s_t(buf, n);
				} break;
			case vt_int64:
				{
					char buf[64];
					int n = sprintf_k(buf, sizeof(buf), "%lld", this->ll);
					return s_t(buf, n);
				} break;
			case vt_uint64:
				{
					char buf[64];
					int n = sprintf_k(buf, sizeof(buf), "%llu", this->ull);
					return s_t(buf, n);
				} break;
			case vt_float:
				{
					char buf[64];
					int n = sprintf_k(buf, sizeof(buf), "%f", this->f);
					return s_t(buf, n);
				} break;
			case vt_double:
				{
					char buf[64];
					int n = sprintf_k(buf, sizeof(buf), "%f", this->d);
					return s_t(buf, n);
				} break;
			case vt_string:
				{
					return s_t(this->str.second, this->str.first);
				} break;
			case vt_d_string:
				{
					return s_t(this->dstr.second, this->dstr.first);
				}
			case vt_table:
				{
					return s_t("table");
				} break;
			case vt_object:
				{
					char buf[256];
					int n = sprintf_k(buf, sizeof(buf), "%s 0x%08x", this->obj.first, this->obj.second);
					return s_t(buf, n);
				} break;
			case vt_pointer:
				{
					char buf[256];
					int n = sprintf_k(buf, sizeof(buf), "0x%08x", this->p);
					return s_t(buf, n);
				}
			}
			ASSERT(FALSE);
			return s_t();
		}

	public:
		bool isnil() const;
		bool istable() const;
		bool isstring() const;
		bool isobject() const;

	public:
		variant& setnil();
		variant& setstring(const char* val, int len);
		variant& setbinary(const void* val, int len);
		variant& setobject(const char* typeName, void* val);
		variant& operator = (const variant& o);

	public:
		operator int () const;
		operator unsigned int () const;
		operator long () const;
		operator unsigned long () const;
		operator double () const;
		operator long long () const;
		operator unsigned long long () const;
		operator const CppTable& () const;
		operator const char* () const;
		operator void* ();
		operator const void* () const;
		int length() const;

		// 这个函数不检查类型名，主要用于接收从LUA返回的table
		template<typename T> T* getObject()
		{
			if(vt == vt_nil) return (T*)NULL;
			ASSERT(vt == vt_object);
			return (T*)this->obj.second;
		}

		template<typename T> operator T* ()
		{
			if(vt == vt_nil) return (T*)NULL;
			if(is_same_type<T,char>::value || is_same_type<T,char const>::value)
			{
				switch(vt)
				{
				case vt_string: return &this->str.second[0];
				case vt_d_string: return this->dstr.second;
				case vt_object:
					{
						ASSERT(strcmp(this->obj.first, "lightuserdata") == 0);
						return (T*)this->obj.second;
					};
				}
				ASSERT(FALSE); return NULL;
			}
			else
			{
				ASSERT(vt == vt_object);
				if(strcmp(this->obj.first, "lightuserdata"))
				{
					const char* typeName = class_name<T>::name();
					ASSERT(typeName[0]);
					ASSERT(strcmp(this->obj.first, typeName) == 0);
				}
				return (T*)this->obj.second;
			}
		}

		template<typename T> operator const T* () const
		{
			if(vt == vt_nil) return (T*)NULL;
			if(is_same_type<T,char>::value || is_same_type<T,char const>::value)
			{
				switch(vt)
				{
				case vt_string: return &this->str.second[0];
				case vt_d_string: return this->dstr.second;
				case vt_object:
					{
						ASSERT(strcmp(this->obj.first, "lightuserdata") == 0);
						return (const T*)this->obj.second;
					};
				}
				ASSERT(FALSE); return NULL;
			}
			else
			{
				ASSERT(vt == vt_object);
				if(strcmp(this->obj.first, "lightuserdata"))
				{
					const char* typeName = class_name<T>::name();
					ASSERT(typeName[0]);
					ASSERT(strcmp(this->obj.first, typeName) == 0);
				}
				return (const T*)this->obj.second;
			}
		}

	public:
		variant& operator = (bool val);
		variant& operator = (char val);
		variant& operator = (unsigned char val);
		variant& operator = (short val);
		variant& operator = (unsigned short val);
		variant& operator = (int val);
		variant& operator = (unsigned int val);
		variant& operator = (long val);
		variant& operator = (unsigned long val);
		variant& operator = (long long val);
		variant& operator = (unsigned long long val);
		variant& operator = (float val);
		variant& operator = (double val);
		variant& operator = (char* val);
		variant& operator = (const char* val);
		variant& operator = (const CppTable& val);
		variant& operator = (void* val);
		variant& operator = (const void* val);
		template<typename T> variant& operator = (T* val)
		{
			const char* typeName = class_name<T>::name();
			ASSERT(typeName[0]);
			return this->setobject(typeName, val);
		}
		variant& operator [] (const char* name);

	private:
		void clear();
		variant& setInt32(int val);
		variant& setUint32(unsigned int val);
	};

	// base class of CppTable
	// 为了去掉编译警告
	class _CppTable_base
	{
	friend struct table;
	public:
		enum { max_base_value = 8 };
		typedef JG_C::KString<64> Name;
		typedef KPair<Name,variant> NameValue;
		typedef JG_C::DynArray<NameValue*,4,4> NameValueArray;

	public:
		_CppTable_base();
		_CppTable_base(const _CppTable_base& o);
		~_CppTable_base();

	public:
		_CppTable_base& operator = (const _CppTable_base& o);

	public:
		int size() const;
		void clear();

	public:
		const char* getKey(int idx) const;
		const variant& getVal(int idx) const;
		const NameValue& at(int idx) const;

	public:
		variant& operator [] (int idx);
		const variant& operator [] (int idx) const;
		variant& operator [] (const char* name);
		const variant& operator [] (const char* name) const;

	public:
		variant& setbinary(const char* name, const void* val, int len);
		variant& setobject(const char* name, void* obj, const char* typeName);

	private:
		variant& _add(const char* name);
		NameValue* _find(const char* name);
		const NameValue* _find(const char* name) const;

	public:
		int m_size;
		NameValue m_baseVals[max_base_value];
		NameValueArray m_extraVals;
	};

    class LUAWARP_API CppTable : private _CppTable_base
	{
	public:
		friend struct table;
		typedef _CppTable_base base_type;
		typedef base_type::NameValue NameValue;

	public:
		CppTable();
		CppTable(const CppTable& o);
		~CppTable();

	public:
		CppTable& operator = (const CppTable& o);

	private:
		base_type& _baseType() { return *(base_type*)this; }
		const base_type& _baseType() const { return *(base_type*)this; }

	public:
		int size() const;
		void clear();

	public:
		const char* getKey(int idx) const;
		const variant& getVal(int idx) const;
		const NameValue& at(int idx) const;

	public:
		variant& operator [] (int idx);
		const variant& operator [] (int idx) const;
		variant& operator [] (const char* name);
		const variant& operator [] (const char* name) const;

	public:
		variant& setbinary(const char* name, const void* val, int len);
		variant& setobject(const char* name, void* obj, const char* typeName);

	public:
		template<size_t ln> JG_C::KDString<ln> toString() const
		{
			return this->_toString<ln>(0);
		}
		template<size_t ln> JG_C::KDString<ln> _toString(int level) const
		{
			typedef JG_C::KDString<ln> s_t;

			int pos = 0;
			char buf[512];
			for(int i=0; i<level; i++)
			{
				buf[pos++] = ' ';
				buf[pos++] = ' ';
			}
			buf[pos] = '\0';

			s_t str; int nsize = this->size();
			for(int i=0; i<nsize; i++)
			{
				if(i) str.append('\n');
				str.append(buf);

				const char* k = this->getKey(i);
				const variant& v = this->getVal(i);

				if(v.vt == variant::vt_table)
				{
					const CppTable& t = v;
					str.append(k);
					str.append('=');
					str.append("table {");
					if(t.size()) str.append("\n");
					s_t s = t._toString<ln>(level+1);
					str.append(s.c_str(), s.size());
					if(t.size()) str.append("\n");
					str.append('}');
				}
				else
				{
					str.append(k);
					str.append('=');
					s_t s = v.toString<ln>();
					str.append(s.c_str(), s.size());
				}
			}
			return str;
		}
		template<typename T> variant& setobject(const char* name, T* obj)
		{
			const char* typeName = class_name<T>::name();
			ASSERT(typeName[0]);
			return this->setobject(name, obj, typeName);
		}
	};
}
#pragma warning(default:4275)

#endif