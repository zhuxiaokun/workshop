#if !defined(_WIN32)
#include "KMilliTimer.h"

volatile DWORD _g_nowTick = 0;
volatile BOOL _g_milliTimerStarted = FALSE;

KMilliTimer::KMilliTimer()
	: m_stopDesired(FALSE)
{
	this->Run();
	while(!_g_milliTimerStarted) Sleep(1);
}

KMilliTimer::~KMilliTimer()
{
	this->stop();
}

void KMilliTimer::Execute()
{
	struct timeval tv;

	DWORD nowTick = get_tick_count_2();
	_g_nowTick = nowTick;
	_g_milliTimerStarted = TRUE;
	
	//printf("KMilliTimer started !\n");

	while(!m_stopDesired)
	{
		tv.tv_sec = 0; tv.tv_usec = 100;
		
		if(!select(1, NULL, NULL, NULL, &tv))
		{
			nowTick = get_tick_count_2();
		}
		else
		{
			Sleep(100);
			nowTick = get_tick_count_2();
		}

		if(nowTick > _g_nowTick)
			_g_nowTick = nowTick;
	}
	
	//printf("KMilliTimer stopped !\n");
}

void KMilliTimer::stop()
{
	m_stopDesired = TRUE;
	this->WaitFor();
}
#endif