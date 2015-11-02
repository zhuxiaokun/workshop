#pragma once

#if !defined(_WIN32)
#include "../Thread/KThread.h"
#include "../SockHeader.h"

extern volatile DWORD _g_nowTick;
extern volatile BOOL _g_milliTimerStarted;

class KMilliTimer : public ::System::Thread::KThread
{
public:
	KMilliTimer();
	~KMilliTimer();

public:
	void Execute();

private:
	void stop();

private:
	volatile BOOL m_stopDesired;
};

#endif