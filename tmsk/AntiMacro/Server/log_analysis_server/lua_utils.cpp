#include <lua/KLuaWrap.h>
#include "KGameAccount.h"
#include "Application.h"
#include <CommSession/KPropertyMap.h>
#include <System/Misc/KDatetime.h>

static const char* time2str(int t)
{
	static char buf[32];
	KDatetime dt(t);
	JG_C::KString<32> s = dt.ToString();
	sprintf_k(buf, sizeof(buf), "%s", s.c_str());
	return buf;
}

static string_32 getFirstDayTime(int yyyymmdd, int dayOffset)
{
	if(yyyymmdd)
	{
		int y = yyyymmdd/10000;
		int m = (yyyymmdd/100) % 100;
		int d = yyyymmdd % 100;
		KLocalDatetime ldt = {y,m,d,0,0,0};
		KDatetime dt(ldt);
		dt.AddDay(dayOffset);
		return dt.ToString();
	}
	else
	{
		KDatetime dt;
		KLocalDatetime ldt = dt.GetLocalTime();
		ldt.hour = ldt.minute = ldt.second = 0;
		dt.SetLocalTime(ldt);
		dt.AddDay(dayOffset);
		return dt.ToString();
	}
}

// getFirstDayTime(yyyymmdd[, dayOffset])
// ·µ»Ø×Ö·û´® yyyy-mm-dd hh:mm:ss
static int getFirstDayTime_l(lua_State* L)
{
	int yyyymmdd = 0;
	int dayOffset = 0;

	int n = lua_gettop(L);
	if(n >= 1) yyyymmdd = luaL_checkinteger(L, 1);
	if(n >= 2) dayOffset = luaL_checkinteger(L, 2);
	string_32 str = getFirstDayTime(yyyymmdd, dayOffset);
	lua_pushstring(L, str.c_str());
	return 1;
}

static int to_yyyymmdd(const char* timeString)
{
	KDatetime dt((INT64)0);
	if(!dt.SetDatetimeString(timeString))
		return 0;
	KLocalDatetime ldt = dt.GetLocalTime();
	int y = ldt.year, m = ldt.month, d = ldt.day;
	return y*10000+m*100+d;
}

static int dofile_l(lua_State* L)
{
	int n = lua_gettop(L);
	const char* filePath = luaL_checkstring(L, 1);
	bool paramCount = n >= 2 ? 1 : 0;

	KInputFileStream fi(filePath);
	if(!fi.Good())
	{
		luaL_argerror(L, 1, "script file nod found!");
		return 0;
	}
	if(!LuaWraper.loadStream(fi))
	{
		luaL_argerror(L, 1, "script file nod found!");
		lua_pop(L, 1);
		return 0;
	}
	if(paramCount) lua_pushvalue(L, 2);
	lua_call(L, paramCount, LUA_MULTRET);
	return lua_gettop(L) - n;
}

extern bool g_exit;
static void exit_l()
{
	//TerminateProcess(GetCurrentProcess(), 0);
	g_exit = true;
}

void register_lua_utils()
{
	lua_State* L = LuaWraper.m_l;
	LuaWraper["time2str"] = time2str;
	LuaWraper["to_yyyymmdd"] = to_yyyymmdd;
	LuaWraper["exit"] = exit_l;
	lua_register(L, "getFirstDayTime", getFirstDayTime_l);
	lua_register(L, "ldofile", dofile_l);
}
