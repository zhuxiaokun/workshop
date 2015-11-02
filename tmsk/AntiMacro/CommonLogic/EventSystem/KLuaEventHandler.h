#ifndef _K_LUA_EVENT_HANDLER_H_
#define _K_LUA_EVENT_HANDLER_H_

#include <lua/KLuaTableHolder.h>
#include <System/Collections/KTree.h>

template <typename T> class KLuaEventHandler : public KLuaTableHolder<T>
{
public:
	typedef System::Collections::KMapByTree_staticPooled<int,bool> EventMap;
	typedef System::Collections::KMapByTree_staticPooled<DWORD,bool> TimerMap;
	typedef System::Collections::KMapByTree_staticPooled<DWORD,bool> RealTimerMap;

public:
	KLuaEventHandler()
	{

	}
	~KLuaEventHandler()
	{

	}

protected:
	bool initLuaHandlers()
	{
		m_evtMap.clear();
		m_timerMap.clear();

		lua_State* L = LuaWraper.m_l;
		KLuaStackRecover sr(L);

		this->loadTableInstance(L);

		lua_pushstring(L, "onEvent");
		lua_gettable(L, -2);
		if(lua_istable(L, -1))
		{
			int t = lua_gettop(L); lua_pushnil(L);
			while (lua_next(L, t) != 0)
			{
				if(lua_isnumber(L,-2) && lua_isfunction(L,-1))
				{// 'key' at -2 and 'value' at -1
					int evtId = (int)lua_tonumber(L, -2);
					m_evtMap[evtId] = true;
				} lua_pop(L, 1);
			}
		} lua_pop(L, 1);

		lua_pushstring(L, "onTimer");
		lua_gettable(L, -2);
		if(lua_istable(L, -1))
		{
			int t = lua_gettop(L); lua_pushnil(L);
			while (lua_next(L, t) != 0)
			{
				if(lua_isnumber(L,-2) && lua_isfunction(L,-1))
				{// 'key' at -2 and 'value' at -1
					DWORD name = (DWORD)lua_tonumber(L, -2);
					m_timerMap[name] = true;
				} lua_pop(L, 1);
			}
		} lua_pop(L, 1);
		
		lua_pushstring(L, "onRealTimer");
		lua_gettable(L, -2);
		if(lua_istable(L, -1))
		{
			int t = lua_gettop(L); lua_pushnil(L);
			while (lua_next(L, t) != 0)
			{
				if(lua_isnumber(L,-2) && lua_isfunction(L,-1))
				{// 'key' at -2 and 'value' at -1
					DWORD name = (DWORD)lua_tonumber(L, -2);
					m_realTimerMap[name] = true;
				} lua_pop(L, 1);
			}
		} lua_pop(L, 1);

		return true;
	}

	void clearLuaHandlers()
	{
		m_evtMap.clear();
		m_timerMap.clear();
	}

	// return value indicate the event whether been handled
	bool luaHandleEvent(int evtId, const void* data, int len)
	{
		bool handled = this->_isLuaCareEvent(evtId);
		if(handled || this->_isLuaCareEvent(0))
		{
			lua_State* L = LuaWraper.m_l;
			KLuaStackRecover recover(L);

			int errfunc = LMREG::lua_geterrfunc(L);
			this->loadTableInstance(L);

			lua_getfield(L, -1, "onEvent");
			lua_pushnumber(L, handled ? evtId : 0);
			lua_gettable(L, -2);				// function
			lua_remove(L, -2);

			lua_pushvalue(L, -2);				// self
			LMREG::push<int>(L, evtId);			// evtId
			LMREG::push<const void*>(L, data);	// data
			LMREG::push<int>(L, len);			// len
			lua_pcall(L, 4, 1, errfunc);

			//return lua_toboolean(L, -1) != 0;
			return true; // lua的返回值被忽略
		}
		return false;
	}

	bool luaHandleTimer(DWORD name, Identity64 cookie, const void* data)
	{
		bool handled = this->_isLuaCareTimer(name);
		if(handled || this->_isLuaCareTimer(0))
		{
			lua_State* L = LuaWraper.m_l;
			KLuaStackRecover recover(L);

			int errfunc = LMREG::lua_geterrfunc(L);
			this->loadTableInstance(L);

			lua_getfield(L, -1, "onTimer");
			lua_pushnumber(L, handled ? (int)name : 0);
			lua_gettable(L, -2);				// function
			lua_remove(L, -2);

			lua_pushvalue(L, -2);				// self
			lua_pushnumber(L, (int)name);			// name
			LMREG::push<Identity64>(L, cookie);	// cookie
			LMREG::push<const void*>(L,data);	// data
			lua_pcall(L, 4, 1, errfunc);

			//return lua_toboolean(L, -1) != 0;
			return true; // lua的返回值被忽略
		}
		return false;
	}

	bool luaHandleRealTimer(DWORD name, Identity64 cookie, const void* data)
	{
		bool handled = this->_isLuaCareRealTimer(name);
		if(handled || this->_isLuaCareRealTimer(0))
		{
			lua_State* L = LuaWraper.m_l;
			KLuaStackRecover recover(L);

			int errfunc = LMREG::lua_geterrfunc(L);
			this->loadTableInstance(L);

			lua_getfield(L, -1, "onRealTimer");
			lua_pushnumber(L, handled ? (int)name : 0);
			lua_gettable(L, -2);				// function
			lua_remove(L, -2);

			lua_pushvalue(L, -2);				// self
			lua_pushnumber(L, (int)name);			// name
			LMREG::push<Identity64>(L, cookie);	// cookie
			LMREG::push<const void*>(L,data);	// data
			lua_pcall(L, 4, 1, errfunc);

			//return lua_toboolean(L, -1) != 0;
			return true; // lua的返回值被忽略
		}
		return false;
	}

private:
	bool _isLuaCareEvent(int evt)
	{
		return m_evtMap.find(evt) != m_evtMap.end();
	}
	bool _isLuaCareTimer(DWORD name)
	{
		return m_timerMap.find(name) != m_timerMap.end();
	}
	bool _isLuaCareRealTimer(DWORD name)
	{
		return m_realTimerMap.find(name) != m_realTimerMap.end();
	}

private:
	EventMap m_evtMap;
	TimerMap m_timerMap;
	RealTimerMap m_realTimerMap;
};

#endif