#include "Application.h"
#include <System/Log/AsyncLogger.h>
#include <System/File/KFile.h>

//extern int testRedis(int, char* argv[]);

void on_exit()
{
	int x = 0;
	x++;
}

int main(int argc, char* argv[])
{
	atexit(on_exit);

	//////
	AsyncDailyFileLogger* logger = new AsyncDailyFileLogger("./log/log_dispatcher.log");
	logger->SetLogLevel(LOG_ALL);
	setGlobalLogger(logger);

	Log(LOG_ERROR|LOG_CONSOLE, "======================================");
	Log(LOG_ERROR|LOG_CONSOLE, "|        log_dispatcher server       |");
	Log(LOG_ERROR|LOG_CONSOLE, "======================================");

	LuaWraper.InitScript(0);
	REGISTER_LUA_CLASS_2(KPropertyMap);
	REGISTER_LUA_CLASS(KEventSource);
	REGISTER_LUA_CLASS_2(KEventHandler);
	REGISTER_LUA_CLASS(KEventEntity);

	const char* cfgFilePath = "../settings/log_dispatcher/configure.lua";
	if(argc > 1)
	{
		cfgFilePath = argv[1];
		if(!JG_F::KFileUtil::IsFileExists(cfgFilePath))
		{
			Log(LOG_ERROR, "error: configure file '%s' not found.", cfgFilePath);
			return 1;
		}
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
		if(!g_pApp->m_frameLineCount)
			msleep(50);
	}

	g_pApp->finalize();
	return 0;
}
