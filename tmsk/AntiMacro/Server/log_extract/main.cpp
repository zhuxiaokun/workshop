#include <System/KPlatform.h>
#include <System/Log/AsyncLogger.h>
#include "Application.h"
#include <System/File/KFile.h>
#include <CommSession/KPropertyMap.h>

int main(int argc, char* argv[])
{
	AsyncDailyFileLogger* logger = new AsyncDailyFileLogger("./log/log_extract.log");
	logger->SetLogLevel(LOG_ALL);
	setGlobalLogger(logger);

	Log(LOG_ERROR|LOG_CONSOLE, "======================================");
	Log(LOG_ERROR|LOG_CONSOLE, "|        log_extract server          |");
	Log(LOG_ERROR|LOG_CONSOLE, "======================================");

	LuaWraper.InitScript(0);
	REGISTER_LUA_CLASS_2(KEventHandler);
	REGISTER_LUA_CLASS_2(KPropertyMap);
	REGISTER_LUA_CLASS(KEventEntity);
	REGISTER_LUA_CLASS(Application);

	const char* cfgFilePath = "../settings/log_extract/configure.lua";
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
