#include <System/KPlatform.h>
#include <System/Log/AsyncLogger.h>
#include "Application.h"
#include <System/File/KFile.h>
#include <CommSession/KPropertyMap.h>
#include "l_mysql.h"

static LuaString time2str(int t)
{
	KDatetime dt(t);
	string_32 str = dt.ToString();
	return LuaString(str.c_str(), str.size());
}

int main(int argc, char* argv[])
{
	AsyncDailyFileLogger* logger = new AsyncDailyFileLogger("./log/log_processor.log");
	logger->SetLogLevel(LOG_ALL);
	setGlobalLogger(logger);

	Log(LOG_ERROR|LOG_CONSOLE, "======================================");
	Log(LOG_ERROR|LOG_CONSOLE, "|        log processor               |");
	Log(LOG_ERROR|LOG_CONSOLE, "======================================");

	const char* cfgFilePath = "../settings/log_processor/configure.lua";
	if(argc > 1)
	{
		cfgFilePath = argv[1];
		if(!JG_F::KFileUtil::IsFileExists(cfgFilePath))
		{
			Log(LOG_ERROR, "error: configure file '%s' not found.", cfgFilePath);
			return 1;
		}
	}
	
	if(!assureOneProgramInstance())
	{
		Log(LOG_ERROR|LOG_CONSOLE, "error: more than one program instance !!!");
		return 1;
	}

	LuaWraper.InitScript(0);
	REGISTER_LUA_CLASS_2(KPropertyMap);
	REGISTER_LUA_CLASS_2(KEventHandler);
	REGISTER_LUA_CLASS(KEventEntity);
	REGISTER_LUA_CLASS(KEventEntity_luaMaster);
	REGISTER_LUA_CLASS(l_mysqlConnection);
	REGISTER_LUA_CLASS(l_mysqlResultSet);
	LuaWraper["time2str"] = time2str;

	g_pApp = new Application();
	if(!g_pApp->initialize(cfgFilePath))
	{
		g_pApp->finalize();
		return 0;
	}

	DWORD lastTick = GetTickCount();
	while(1)
	{
		DWORD nowTick = GetTickCount();
		g_pApp->breathe(nowTick - lastTick);
		lastTick = nowTick;
		if(g_pApp->m_msFrameSleep > 10)
			msleep(g_pApp->m_msFrameSleep);
	}

	g_pApp->finalize();
	return 0;
}
