#include "KConsoleThread.h"
#include <System/KPlatform.h>

extern int luaU_onError(lua_State* L);
extern KConsoleThread::CommandQueue* g_cmdQueue;

KConsoleThread::KConsoleThread()
{
	m_stopDesired = FALSE;
	m_stopped = FALSE;
	m_ctx.attach("console_ctx");
	m_ctx.setMetatable("_G");
	this->Run();
}

KConsoleThread::~KConsoleThread()
{
	this->Stop();
}

void KConsoleThread::pushString(const char* str)
{
	m_cmdQueue.push(str);
}

int KConsoleThread::readconsole(char* buf, int len)
{
	int chr;
	int c = 0;

	while(!m_stopDesired)
	{
		chr = getchar();
		if(chr==EOF)	break;
		if(chr == '\n') break;

		buf[c++] = chr;
		if(c >= len-1) break;
	}

	buf[c] = '\0';
	return c;
}

void KConsoleThread::Execute()
{
	while(!m_stopDesired)
	{
		printf_k("lua> ");
		int len = this->readconsole(m_inputBuffer, 1024);
		if(!len) continue;

		if(stricmp(m_inputBuffer, "exit") == 0)
		{
			m_stopDesired = TRUE;
			break;
		}
		else if(stricmp(m_inputBuffer, "quit") == 0)
		{
			m_stopDesired = TRUE;
			break;
		}
		m_cmdQueue.push(m_inputBuffer);
	}
}

void KConsoleThread::Stop()
{
	m_stopDesired = TRUE;
	System::Thread::KThread::WaitFor();
	m_stopped = TRUE;
}

bool KConsoleThread::doString(const char* script)
{
	lua_State* L = LuaWraper.m_l;
	KLuaStackRecover sr(L);

	int errfunc = LMREG::lua_geterrfunc(L);

	if(luaL_loadstring(L, script))
	{
		luaU_onError(L);
		return false;
	}

	m_ctx.loadTableInstance(L);
	lua_setfenv(L, -2);

	lua_pcall(L, 0, LUA_MULTRET, errfunc);
	return true;
}

void KConsoleThread::processCommandQueue()
{
	KConsoleThread::Command cmd;
	while(m_cmdQueue.pop(cmd))
	{
		this->doString(cmd.c_str());
	}	
}