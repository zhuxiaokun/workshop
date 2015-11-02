#ifndef _K_LUA_TABLE_HOLDER_H_
#define _K_LUA_TABLE_HOLDER_H_

#include "KLuaWrap.h"

enum enumLuaTypes
{
	luaNil_t			= LUA_TNIL,
	luaBoolean_t		= LUA_TBOOLEAN,
	luaLightuserdata_t	= LUA_TLIGHTUSERDATA,
	luaNumber_t			= LUA_TNUMBER,
	luaString_t			= LUA_TSTRING,
	luaTable_t			= LUA_TTABLE,
	luaFunction_t		= LUA_TFUNCTION,
	luaUserdata_t		= LUA_TUSERDATA,
	luaThread_t			= LUA_TTHREAD,
};

class KLuaTableVisitor
{
public:
	virtual ~KLuaTableVisitor() { }
	virtual bool onpair(lua_State* L, int key, int val) = 0;
};

/**
 * 这个 class 帮助 T 保持一个 lua table 的实例
 * 如: class T : public KLuaTableHolder<T> {...}
 *
 */
template <typename T> class KLuaTableHolder
{
friend class KLuaTableWraper;
public:
	int m_luaRef;
	lua_State* m_luaState;

public:
	static int m_nextInstNo;
	static int m_registryRef;

public:
	KLuaTableHolder():m_luaRef(0),m_luaState(LuaWraper.m_l)
	{

	}
	KLuaTableHolder(lua_State* l):m_luaRef(0),m_luaState(l)
	{

	}
	~KLuaTableHolder()
	{
		this->releaseTableInstance();
	}

protected:
	// 新生成的table实例留在栈顶
	bool createTableInstance(lua_State* L)
	{
		if(!m_registryRef)
		{
			lua_newtable(L);
			m_registryRef = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		m_luaRef = m_nextInstNo++;
		lua_newtable(L);
		lua_loadregistryref(L, m_registryRef);
		lua_pushinteger(L, m_luaRef);
		lua_pushvalue(L, -3);
		lua_settable(L, -3);
		lua_pop(L, 1);
		return true;
	}
	// metatable.__index = metatable
	// if metatable not exist, create one
	// push metatbel at top of stack when return
	bool prepareMetatable(lua_State* L, const char* metatable)
	{
		lua_pushstring(L, metatable);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushstring(L, metatable);
			lua_pushvalue(L, -2);
			lua_settable(L, LUA_GLOBALSINDEX);
		}
		ASSERT(lua_istable(L, -1));
		lua_pushstring(L, "__index");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);
		return true;
	}
	bool prepareMetatable(lua_State* L, const char* metatable, const char* field)
	{
		this->prepareMetatable(L, metatable);
		lua_pushstring(L, field);
		lua_gettable(L, -2);
		if(lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushstring(L, field);
			lua_pushvalue(L, -2);
			lua_settable(L, -4);
		}
		
		ASSERT(lua_istable(L, -1));
		
		lua_pushvalue(L, -2);
		lua_setmetatable(L, -2);
		lua_remove(L, -2);

		lua_pushstring(L, "__index");
		lua_pushvalue(L, -2);
		lua_settable(L, -3);
		return true;
	}

public: // 新生成的table实例留在栈顶
	bool loadTableInstance(lua_State* L) const
	{
		ASSERT(m_registryRef && m_luaRef);
		lua_loadregistryref(L, m_registryRef);
		lua_pushinteger(L, m_luaRef);
		lua_rawget(L, -2);
		lua_remove(L, -2);
		return true;
	}

public:// keep stack balance
	bool setMetatable(const char* metatable)
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		this->prepareMetatable(L, metatable);
		lua_setmetatable(L, -2);
		lua_pop(L, 1);
		return true;
	}
	bool setMetatable(const char* parent, const char* grand)
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		this->prepareMetatable(L, parent);
		this->prepareMetatable(L, grand);
		lua_setmetatable(L, -2);
		lua_setmetatable(L, -2);
		lua_pop(L, 1);
		return true;
	}
	bool setMetatable2(const char* metatable, const char* field)
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		this->prepareMetatable(L, metatable, field);
		lua_setmetatable(L, -2);
		lua_pop(L, 1);
		return true;
	}
	// set metatable as new table's metatable
	// if metable not exist, create one, and save it in global
	// 栈保持平衡
	bool newTableInstance(const char* metatable)
	{
		lua_State* L = m_luaState;
		this->createTableInstance(L);
		this->prepareMetatable(L, metatable);
		lua_setmetatable(L, -2);
		lua_pop(L, 1);
		return true;
	}
	// set parent's metatable to frand, and set new table's metatable to parent
	// if grand not exist, create one
	// if parent not exist, create one
	// stack is balance
	bool newTableInstance(const char* parent, const char* grand)
	{
		lua_State* L = m_luaState;
		this->createTableInstance(L);
		this->prepareMetatable(L, parent);
		this->prepareMetatable(L, grand);
		lua_setmetatable(L, -2);
		lua_setmetatable(L, -2);
		lua_pop(L, 1);
		return true;
	}
	// stack is balance
	// setmetatable(metatable.field,metatable)
	// setmetatable(inst, metatable.field)
	bool newTableInstance2(const char* metatable, const char* field)
	{
		lua_State* L = m_luaState;
		this->createTableInstance(L);
		this->prepareMetatable(L, metatable, field);
		lua_setmetatable(L, -2);
		lua_pop(L, 1);
		return true;
	}
	void releaseTableInstance()
	{
		if(m_luaRef && m_registryRef)
		{
			lua_State* L = m_luaState;
			KLuaStackRecover sr(L);
			lua_loadregistryref(L, m_registryRef);
			lua_pushinteger(L, m_luaRef);
			lua_pushnil(L);
			lua_settable(L, -3);
			m_luaRef = 0;
		}
	}
	bool checkLuaDataType(lua_State* L, enumLuaTypes t) const
	{
		switch(t)
		{
		case luaNil_t:			return lua_isnil(L, -1);
		case luaBoolean_t:		return lua_isboolean(L, -1);
		case luaLightuserdata_t:return lua_islightuserdata(L, -1);
		case luaNumber_t:		return lua_isnumber(L, -1) != 0;
		case luaString_t:		return lua_isstring(L, -1) != 0;
		case luaTable_t:		return lua_istable(L, -1);
		case luaFunction_t:		return lua_isfunction(L, -1);
		case luaUserdata_t:		return lua_isuserdata(L, -1) != 0;
		case luaThread_t:		return lua_isthread(L, -1);
		}
		return false;
	}

public:
	bool tableCall(const char* name)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		int errfunc = LMREG::lua_geterrfunc(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		if(!lua_isfunction(L, -1)) return false;
		lua_pushvalue(L, -2);
		lua_pcall(L, 1, 0, errfunc);
		return true;
	}
	template <typename T1> bool tableCall(const char* name, const T1& arg1)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		int errfunc = LMREG::lua_geterrfunc(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		if(!lua_isfunction(L, -1)) return false;
		lua_pushvalue(L, -2);
		LMREG::push<T1>(L, arg1);
		lua_pcall(L, 2, 0, errfunc);
		return true;
	}
	template <typename T1,typename T2> bool tableCall(const char* name, const T1& arg1, const T2& arg2)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		int errfunc = LMREG::lua_geterrfunc(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		if(!lua_isfunction(L, -1)) return false;
		lua_pushvalue(L, -2);
		LMREG::push<T1>(L, arg1);
		LMREG::push<T2>(L, arg2);
		lua_pcall(L, 3, 0, errfunc);
		return true;
	}
	template <typename T1,typename T2,typename T3> bool tableCall(const char* name,const T1& arg1,const T2& arg2,const T3& arg3)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		int errfunc = LMREG::lua_geterrfunc(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		if(!lua_isfunction(L, -1)) return false;
		lua_pushvalue(L, -2);
		LMREG::push<T1>(L, arg1);
		LMREG::push<T2>(L, arg2);
		LMREG::push<T3>(L, arg3);
		lua_pcall(L, 4, 0, errfunc);
		return true;
	}
	template <typename R> bool retTableCall(R& ret, const char* name)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		int errfunc = LMREG::lua_geterrfunc(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		if(!lua_isfunction(L, -1)) return false;
		lua_pushvalue(L, -2);
		lua_pcall(L, 1, 1, errfunc);
		ret = LMREG::read<R>(L, -1);
		return true;
	}
	template <typename R,typename T1> bool retTableCall(R& ret, const char* name, const T1& arg1)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		int errfunc = LMREG::lua_geterrfunc(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		if(!lua_isfunction(L, -1)) return false;
		lua_pushvalue(L, -2);
		LMREG::push<T1>(L, arg1);
		lua_pcall(L, 2, 1, errfunc);
		ret = LMREG::read<R>(L, -1);
		return true;
	}
	template <typename R,typename T1,typename T2> bool retTableCall(R& ret,const char* name,const T1& arg1,const T2& arg2)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		int errfunc = LMREG::lua_geterrfunc(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		if(!lua_isfunction(L, -1)) return false;
		lua_pushvalue(L, -2);
		LMREG::push<T1>(L, arg1);
		LMREG::push<T2>(L, arg2);
		lua_pcall(L, 3, 1, errfunc);
		ret = LMREG::read<R>(L, -1);
		return true;
	}
	template <typename R,typename T1,typename T2,typename T3> bool retTableCall(R& ret,const char* name,const T1& arg1,const T2& arg2,const T3& arg3)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		int errfunc = LMREG::lua_geterrfunc(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		if(!lua_isfunction(L, -1)) return false;
		lua_pushvalue(L, -2);
		LMREG::push<T1>(L, arg1);
		LMREG::push<T2>(L, arg2);
		LMREG::push<T3>(L, arg3);
		lua_pcall(L, 4, 1, errfunc);
		ret = LMREG::read<R>(L, -1);
		return true;
	}

public:
	size_t size() const
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		size_t n = lua_objlen(L, -1);
		lua_pop(L, 1);
		return n;
	}

public:
	bool setnil(int key)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		this->loadTableInstance(L);
		lua_pushinteger(L, key);
		lua_pushnil(L);
		lua_settable(L, -3);
		return true;
	}
	bool setnil(const char* name)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_pushnil(L);
		lua_settable(L, -3);
		return true;
	}
	template <typename V> bool setTableField(int key, V* val)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		this->loadTableInstance(L);
		lua_pushinteger(L, key);
		LMREG::push<V*>(L, val);
		lua_settable(L, -3);
		return true;
	}
	template <typename V> bool setTableField(int key, V& val)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		this->loadTableInstance(L);
		lua_pushinteger(L, key);
		LMREG::push<V>(L, val);
		lua_settable(L, -3);
		return true;
	}
	template <typename V> bool setTableField(const char* name, V* val)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		LMREG::push<V*>(L, val);
		lua_settable(L, -3);
		return true;
	}
	template <typename V> bool setTableField(const char* name, V& val)
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		LMREG::push<V>(L, val);
		lua_settable(L, -3);
		return true;
	}

public:
	bool hasTableField(int key) const
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		lua_pushinteger(L, key);
		lua_gettable(L, -2);
		bool bRet = !lua_isnil(L, -1);
		lua_pop(L, 2);
		return bRet;
	}
	bool hasTableField(int key, enumLuaTypes t) const
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		lua_pushinteger(L, key);
		lua_gettable(L, -2);
		bool bRet = this->checkLuaDataType(L, t);
		lua_pop(L, 2);
		return bRet;
	}
	bool getTableField_b(int key, bool defVal)
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		lua_pushinteger(L, key);
		lua_gettable(L, -2);
		bool val = lua_isboolean(L, -1) ? lua_toboolean(L, -1) : defVal;
		lua_pop(L, 2);
		return val;
	}
	int getTableField_i32(int key, int defVal)
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		lua_pushinteger(L, key);
		lua_gettable(L, -2);
		int val = lua_isnumber(L, -1) ? lua_tointeger(L, -1) : defVal;
		lua_pop(L, 2);
		return val;
	}
	float getTableField_f(int key, float defVal)
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		lua_pushinteger(L, key);
		lua_gettable(L, -2);
		float val = lua_isnumber(L, -1) ? (float)lua_tonumber(L, -1) : defVal;
		lua_pop(L, 2);
		return val;
	}
	const char* getTableField_s(int key, const char* defVal)
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		lua_pushinteger(L, key);
		lua_gettable(L, -2);
		const char* val = lua_isstring(L, -1) ? lua_tostring(L, -1) : defVal;
		lua_pop(L, 2);
		return val;
	}

public:
	bool hasTableField(const char* name) const
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		return lua_isnil(L, -1);
	}
	bool hasTableField(const char* name, enumLuaTypes t)
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		bool bRet = this->checkLuaDataType(L, t);
		lua_pop(L, 2);
		return bRet;
	}
	bool getTableField_b(const char* name, bool defVal) const
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		if(!lua_isboolean(L, -1)) return defVal;
		return lua_toboolean(L,-1) != 0;
	}
	int getTableField_i32(const char* name, int defVal) const
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		if(!lua_isnumber(L, -1)) return defVal;
		return lua_tointeger(L,-1);
	}
	float getTableField_f(const char* name, float defVal) const
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		if(!lua_isnumber(L, -1)) return defVal;
		return (float)lua_tonumber(L,-1);
	}
	const char* getTableField_s(const char* name, const char* defVal) const
	{
		lua_State* L = m_luaState;
		KLuaStackRecover sr(L);
		this->loadTableInstance(L);
		lua_pushstring(L, name);
		lua_gettable(L, -2);
		if(!lua_isstring(L, -1)) return defVal;
		return lua_tostring(L,-1);
	}
};

template <typename T> int KLuaTableHolder<T>::m_nextInstNo  = 1;
template <typename T> int KLuaTableHolder<T>::m_registryRef = 0;

class KLuaTableWraper : public KLuaTableHolder<KLuaTableWraper>
{
public:
	typedef KLuaTableHolder<KLuaTableWraper> base_type;

public:
	// 附着到栈index位置的table元素，缺省是全局空间
	KLuaTableWraper(int index=LUA_GLOBALSINDEX):base_type(LuaWraper.m_l)
	{
		lua_State* L = m_luaState;
		if(index != -1) lua_pushvalue(L, index);
		this->_refTable(L);
		if(index != -1) lua_pop(L, 1);
	}
	KLuaTableWraper(lua_State* l, int index):base_type(l)
	{
		lua_State* L = m_luaState;
		if(index != -1) lua_pushvalue(L, index);
		this->_refTable(L);
		if(index != -1) lua_pop(L, 1);
	}
	KLuaTableWraper(const char* tblname):base_type(LuaWraper.m_l)
	{
		lua_State* L = m_luaState; ASSERT(L);
		lua_pushstring(L, tblname);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushstring(L, tblname);
			lua_pushvalue(L, -2);
			lua_settable(L, LUA_GLOBALSINDEX);
		} ASSERT(lua_istable(L, -1));
		this->_refTable(L);
		lua_pop(L, 1);
	}
	KLuaTableWraper(lua_State* l, const char* tblname):base_type(l)
	{
		lua_State* L = m_luaState; ASSERT(L);
		lua_pushstring(L, tblname);
		lua_gettable(L, LUA_GLOBALSINDEX);
		if(lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushstring(L, tblname);
			lua_pushvalue(L, -2);
			lua_settable(L, LUA_GLOBALSINDEX);
		} ASSERT(lua_istable(L, -1));
		this->_refTable(L);
		lua_pop(L, 1);
	}
	template <typename T> KLuaTableWraper(KLuaTableHolder<T>& holder):base_type(holder.m_luaState)
	{
		lua_State* L = m_luaState; ASSERT(L);
		holder.loadTableInstance(L);
		this->_refTable(L);
		lua_pop(L, 1);
	}
	KLuaTableWraper(const KLuaTableWraper& o):base_type(o.m_luaState)
	{
		lua_State* L = m_luaState;
		this->releaseTableInstance();
		if(!o.m_luaRef) return;
		o.loadTableInstance(L);
		this->_refTable(L);
		lua_pop(L, 1);
	}
	KLuaTableWraper& operator = (const KLuaTableWraper& o)
	{
		ASSERT(m_luaState == o.m_luaState);
		lua_State* L = m_luaState;
		if(this == &o) return *this;
		this->releaseTableInstance();

		if(!o.m_luaRef) return *this;
		o.loadTableInstance(L);
		this->_refTable(L);
		return *this;
	}
	~KLuaTableWraper()
	{
		this->releaseTableInstance();
	}
	bool foreachpair(KLuaTableVisitor& v)
	{
		KLuaStackRecover sr(m_luaState);
		lua_State* L = m_luaState;

		int t = -1;
		if(t < 0)
		{
			if (t > LUA_REGISTRYINDEX)
			{
				api_check(L, idx != 0 && -idx <= L->top - L->base);
				t = (int)((L->top + t) - L->base) + 1;
			}
		}

		this->loadTableInstance(m_luaState);
		lua_pushnil(L);  // first key
		while (lua_next(L, t) != 0)
		{
			// 'key' is at index -2 and 'value' at index -1
			if(!v.onpair(m_luaState, -2, -1))
				return false;
			lua_pop(L, 1);  // removes 'value'; keeps 'key' for next iteration
		}
		return true;
	}

public:
	bool create()
	{
		this->releaseTableInstance();
		lua_State* L = m_luaState;
		lua_newtable(L);
		this->_refTable(L);
		lua_pop(L, 1);
		return true;
	}
	bool attach(int key)
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		lua_pushinteger(L, key);
		lua_gettable(L, -2);
		if(lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushinteger(L, key);
			lua_pushvalue(L, -2);
			lua_settable(L, -4);
		} ASSERT(lua_istable(L, -1));
		this->_refTable(L);
		lua_pop(L, 2);
		return true;
	}
	bool attach(const char* tblfield)
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		lua_pushstring(L, tblfield);
		lua_gettable(L, -2);
		if(lua_isnil(L, -1))
		{
			lua_pop(L, 1);
			lua_newtable(L);
			lua_pushstring(L, tblfield);
			lua_pushvalue(L, -2);
			lua_settable(L, -4);
		} ASSERT(lua_istable(L, -1));
		this->_refTable(L);
		lua_pop(L, 2);
		return true;
	}
	bool attachNewChileTable()
	{
		lua_State* L = m_luaState;
		this->loadTableInstance(L);
		lua_newtable(L);
		lua_pushvalue(L, -1);
		luaL_ref(L, -3);
		this->_refTable(L);
		lua_remove(L, -2);
		return true;
	}
	// 存到全局空间中
	bool save(const char* tableName)
	{
		lua_State* L = m_luaState;
		lua_pushstring(L, tableName);
		this->loadTableInstance(L);
		lua_settable(L, LUA_GLOBALSINDEX);
		return true;
	}

private:
	// 将栈顶的对象保存到registry
	// stack keep balance
	void _refTable(lua_State* L)
	{
		ASSERT(lua_istable(L,-1));
		if(!m_registryRef)
		{
			lua_newtable(L);
			m_registryRef = luaL_ref(L, LUA_REGISTRYINDEX);
		}
		if(!m_luaRef) m_luaRef = m_nextInstNo++;
		lua_loadregistryref(L, m_registryRef);
		lua_pushinteger(L, m_luaRef);
		lua_pushvalue(L, -3);
		lua_settable(L, -3);
		lua_pop(L, 1);
		return;
	}
};

#endif
