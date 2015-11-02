#include "Application.h"
#include <KConsoleThread.h>
#include <System/Log/AsyncLogger.h>
#include "l_mysql.h"
#include <CommSession/KPropertyMap.h>

extern void register_lua_utils();
int main(int argc, char* argv[])
{
	AsyncDailyFileLogger* logger = new AsyncDailyFileLogger("./log/macro_analysis_server.log");
	setGlobalLogger(logger);
	luaSetLuaLogger(logger);

	Log(LOG_ERROR|LOG_CONSOLE, "======================================");
	Log(LOG_ERROR|LOG_CONSOLE, "|        macro analysis  server      |");
	Log(LOG_ERROR|LOG_CONSOLE, "======================================");

	LuaWraper.InitScript(0);
	REGISTER_LUA_CLASS(KEventSource);
	REGISTER_LUA_CLASS_2(KPropertyMap);
	REGISTER_LUA_CLASS_2(KEventHandler);
	REGISTER_LUA_CLASS(KEventEntity);
	REGISTER_LUA_CLASS(InnerPlayer);
	REGISTER_LUA_CLASS(KPlayer);
	REGISTER_LUA_CLASS(KLogin);
	REGISTER_LUA_CLASS(KTrade);
	REGISTER_LUA_CLASS(KServer);
	REGISTER_LUA_CLASS(KAccount);
	REGISTER_LUA_CLASS(KLargeNameList);
	REGISTER_LUA_CLASS(KAnalysis);
	REGISTER_LUA_CLASS(KServerOfIp);
	REGISTER_LUA_CLASS(KIp);
	REGISTER_LUA_CLASS(KIpAnalysis);
	REGISTER_LUA_CLASS(KServerOfDev);
	REGISTER_LUA_CLASS(KDev);
	REGISTER_LUA_CLASS(KDevAnalysis);
	REGISTER_LUA_CLASS(KEventEntity_luaMaster);
	REGISTER_LUA_CLASS(KRedisEraser);
	REGISTER_LUA_CLASS(l_mysqlConnection);
	REGISTER_LUA_CLASS(l_mysqlResultSet);
	REGISTER_LUA_CLASS(Application);
	register_lua_utils();
	LuaWraper.doScripts("../settings/macro_analysis_server/util.lua");

	g_pApp = new Application();

	const char* cfgFile = "../settings/macro_analysis_server/configure.lua";
	if(!g_pApp->initialize(cfgFile))
		return 1;

	KConsoleThread consoleThread;

	DWORD lastTick = GetTickCount();
	while(1)
	{
		DWORD nowTick = GetTickCount();
		consoleThread.processCommandQueue();
		g_pApp->breathe(nowTick - lastTick);
		lastTick = nowTick;
		msleep(50);
	}

	g_pApp->finalize();
	return 0;
}
