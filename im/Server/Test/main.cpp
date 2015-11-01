#include "KTestClient.h"
#include <KConsoleThread.h>
#include <System/Random/KRandom.h>
#include <System/File/KFile.h>
#include <KGlobalFunction.h>

int luaU_Int64(lua_State* L)
{
	INT64 val = (INT64)luaL_checkinteger(L, 1);
	double n; memcpy(&n, &val, sizeof(n));
	lua_pushnumber(L, n);
	return 1;
}

int luaU_MessageID(lua_State* L)
{
	int yyyymmdd = luaL_checkinteger(L, 1);
	int seq = luaL_checkinteger(L, 2);

	JgMessageID mid;
	mid.yyyy_mm_dd = yyyymmdd;
	mid.sequence = seq;
	INT64 val = MessageID2Int64(mid);

	double n; memcpy(&n, &val, sizeof(n));
	lua_pushnumber(L, n);
	return 1;
}

CREATE_LUAWRAPPER();

int main(int argc, char* argv[])
{
	setProcessEncoding(encode_utf8);

	LuaWraper.InitScript(0);
	LuaWraper.doScripts("env.lua");
	lua_register(LuaWraper.m_l, "mid", luaU_MessageID);
	lua_register(LuaWraper.m_l, "i64", luaU_Int64);
	REGISTER_LUA_CLASS_2(KTestClient);

	KConsoleThread consoleThread;

	g_client = new KTestClient();
	LuaWraper["c"] = g_client;
	LuaWraper["client"] = g_client;
	g_client->initialize();
	
	g_nowTicks = GetTickCount();
	const int breatheInterval = 50;
	DWORD lastBreathe = g_nowTicks - breatheInterval;
	while(!consoleThread.m_stopDesired)
	{	
		g_nowTicks = GetTickCount();
		
		g_client->breathe(g_nowTicks-lastBreathe);
		consoleThread.processCommandQueue();

		lastBreathe = g_nowTicks; g_nowTicks = GetTickCount();
		if(g_nowTicks < lastBreathe+breatheInterval)
		{
			int sleepTicks = (lastBreathe+breatheInterval) - g_nowTicks;
			msleep(sleepTicks);
		}
	}

	g_client->finalize();
	return 0;
}
