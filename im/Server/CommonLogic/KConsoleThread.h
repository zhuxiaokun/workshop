#pragma once
#include <System/KPlatform.h>
#include <System/Thread/KThread.h>
#include <System/Misc/StrUtil.h>
#include <System/Collections/KQueue.h>
#include <lua/KLuaTableHolder.h>

class KConsoleThread : public System::Thread::KThread
{
public:
	typedef System::Collections::KString<512> Command;
	typedef System::Collections::KQueueFifo<Command,JG_S::KMTLock> CommandQueue;

public:
	char m_inputBuffer[1024];
	volatile BOOL m_stopDesired;
	volatile BOOL m_stopped;
	KLuaTableWraper m_ctx; // ´æ×´Ì¬ÓÃ
	CommandQueue m_cmdQueue;

public:
	KConsoleThread();
	~KConsoleThread();

public:
	int readconsole(char* buf, int len);
	void Execute();
	void Stop();

public:
	bool doString(const char* script);
	void processCommandQueue();
};
