#include "KDemoChatServerApp.h"
#include <System/Log/AsyncLogger.h>
#include <System/File/KFile.h>

int main(int argc, char* argv[])
{
	setProcessEncoding(encode_utf8);

	AsyncDailyFileLogger* logger = new AsyncDailyFileLogger("Log/DemoChatServer.log");
	GetGlobalLogger().SetLogger(logger);

	Log(LOG_WARN|LOG_CONSOLE, "|------------------------------------------------|");
	Log(LOG_WARN|LOG_CONSOLE, "|                  DemoChatServer                |");
	Log(LOG_WARN|LOG_CONSOLE, "|------------------------------------------------|");

	if(!assureOneProgramInstance())
	{
		Log(LOG_ERROR, "error: assure one program instance");
		return FALSE;
	}

	g_pApp = new KDemoChatServerApp();
	if(!g_pApp->initialize())
	{
		Log(LOG_ERROR, "error: application initialize");
		return FALSE;
	}

	int breatheInterval = 50;
	DWORD lastBreathe = GetTickCount() - breatheInterval;

	while(TRUE)
	{
		DWORD now = GetTickCount();
		int interval = kmax((int)(now-lastBreathe), 0);
		g_pApp->breathe(interval);
		lastBreathe = now;
		g_pApp->m_inputEvent.Block(50);
	}
	
	return 0;
}
