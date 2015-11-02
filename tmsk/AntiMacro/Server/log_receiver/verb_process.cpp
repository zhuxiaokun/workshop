#include "verb_process.h"
//#include <redis/hiredis.h>
#include "Application.h"

static JG_S::KThreadMutex g_mx;
static BOOL g_initialized = FALSE;
static VerbFunctionMap g_verbMap;

static INT64 g_lineCount = 0;
static void init_verbMap();
void process_verb(KPropertyMap& m, bool fromNetwork)
{
	if(!g_initialized)
		init_verbMap();

	const char* verb = m.get("verb");
	if(!verb) return;

	g_lineCount++;
	if(g_lineCount % 1000 == 0)
		Log(LOG_ERROR|LOG_CONSOLE, "info: proces line count %lld", g_lineCount);

	g_pApp->m_logWriter.on_logLine(m);
}

void process_login(KPropertyMap& m, bool fromNetwork)
{
	return;
}

void process_logout(KPropertyMap& m, bool fromNetwork)
{

}

void process_trade(KPropertyMap& m, bool fromNetwork)
{

}

static void init_verbMap()
{
	if(g_initialized)
		return;

	JG_S::KAutoThreadMutex mx(g_mx);
	if(g_initialized)
		return;

	g_verbMap["login"] = process_login;
	g_verbMap["logout"] = process_logout;
	g_verbMap["trade"] = process_trade;

	g_initialized = TRUE;
}
