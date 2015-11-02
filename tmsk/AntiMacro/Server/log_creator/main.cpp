#include <KConsoleThread.h>
#include <lua/KLuaWrap.h>
#include "KAnalysisLogger.h"

extern void registLuaUtils();
int main(int argc, char* argv[])
{
	KAnalysisLogger* logger = new KAnalysisLogger("./log/log_creator.log");
	logger->SetLogLevel(0xffffffff);
	logger->SetFlushLines(16);
	setGlobalLogger(logger);

	LuaWraper.InitScript(0);
	registLuaUtils();

	KConsoleThread console;
	while(1)
	{
		console.processCommandQueue();
		msleep(50);
	}
	return 0;
}
