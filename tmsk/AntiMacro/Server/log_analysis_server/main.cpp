#include <System/KPlatform.h>
#include <System/Log/AsyncLogger.h>
#include "Application.h"
#include <System/File/KFile.h>
#include <CommSession/KPropertyMap.h>
#include "l_mysql.h"
#include <KConsoleThread.h>

bool g_exit = false;
extern void register_lua_utils();
int main(int argc, char* argv[])
{
	AsyncDailyFileLogger* logger = new AsyncDailyFileLogger(".\\log\\log_analysis_server.log");
	setGlobalLogger(logger);

	Log(LOG_ERROR|LOG_CONSOLE, "======================================");
	Log(LOG_ERROR|LOG_CONSOLE, "|        log_analysis_server         |");
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
	REGISTER_LUA_CLASS(l_mysqlConnection);
	REGISTER_LUA_CLASS(l_mysqlResultSet);
	REGISTER_LUA_CLASS(Application);
	register_lua_utils();
	LuaWraper.doScripts("../settings/log_analysis_server/util.lua");
	LuaWraper.doScripts("../settings/log_analysis_server/dbutil.lua");

	const char* cfgFilePath = "../settings/log_analysis_server/configure.lua";

	g_pApp = new Application();
	if(!g_pApp->initialize(cfgFilePath))
	{
		g_pApp->finalize();
		return 0;
	}

	KConsoleThread consoleThread;
	if(argc > 1)
	{
		int yyyymmdd = 0;
		const char* luaFile = argv[1];
		if(argc > 2) yyyymmdd = str2int(argv[2]);
		if(!JG_F::KFileUtil::IsFileExists(luaFile))
		{
			Log(LOG_CONSOLE|LOG_ERROR, "error: lua file \'%s\' not found!", luaFile);
			Log(LOG_CONSOLE|LOG_ERROR, "usage: %s [lua_script_file_path yyyymmdd]", argv[0]);
			return 1;
		}
		//if(yyyymmdd)
		{
			char cmdstr[256];
			sprintf_k(cmdstr, sizeof(cmdstr), "ldofile(\'%s\', %d)", luaFile, yyyymmdd);
			consoleThread.pushString(cmdstr);
			consoleThread.pushString("exit()");
		}
	}

	DWORD lastTick = GetTickCount();
	while(!g_exit)
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
