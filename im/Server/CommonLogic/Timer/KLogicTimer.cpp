#include "KLogicTimer.h"
#include "System/Misc/KDatetime.h"

//// KLogicRealTimer //////////////////////////

KLogicRealTimer::KLogicRealTimer()
{

}

KLogicRealTimer::~KLogicRealTimer()
{

}

bool KLogicRealTimer::startRealTimer(DWORD name, Identity64 cookie,int mode, int val, int hour, int minute, int second, void* data)
{
	m_state = eStop;
	m_oldstate = eStop;
	m_name = name;
	m_cookie = cookie;
	m_mode = mode;
	m_value = val;
	m_hour = hour;
	m_minute = minute;
	m_second = second;
	m_data = data;
	return true;
}

bool KLogicRealTimer::isTimeout() const
{
	if( m_oldstate != m_state && m_state == eRun )return true;
	return false;
}

void KLogicRealTimer::Breathe(int nInterval)
{
	m_oldstate = m_state;

	KDatetime NowTime;
	KDatetime alarmStartTime;
	KDatetime alarmStopTime;
	KLocalDatetime localTime = NowTime.GetLocalTime();
	localTime.hour = m_hour;
	localTime.minute = m_minute;
	localTime.second = m_second;
	alarmStartTime.SetLocalTime(localTime);
	alarmStopTime.SetLocalTime(localTime);
	alarmStopTime.AddSecond(1);

	if (m_state == eStop)
	{
		if (m_mode == 0)
		{
			//每日
			if (NowTime.Time() >= alarmStartTime.Time() && NowTime.Time() <= alarmStopTime.Time())
			{
				m_state = eRun;
			}
		}
		else if (m_mode == 1)
		{
			//每周
			if(NowTime.WeekDay() == m_value)
			{
				if (NowTime.Time() >= alarmStartTime.Time() && NowTime.Time() <= alarmStopTime.Time())
				{
					m_state = eRun;
				}
			}
		}
		else if (m_mode == 2)
		{
			//每月
			if(localTime.day < m_value)
			{
				if (NowTime.Time() >= alarmStartTime.Time() && NowTime.Time() <= alarmStopTime.Time())
				{
					m_state = eRun;
				}
			}
		}
		else if (m_mode == 3)
		{
			//每年
			INT nNow = localTime.month * 100 + localTime.day;
			if(nNow == m_value)
			{
				if (NowTime.Time() >= alarmStartTime.Time() && NowTime.Time() <= alarmStopTime.Time())
				{
					m_state = eRun;
				}
			}
		}
	}

	if (m_state == eRun)
	{
		if (m_mode == 0)
		{
			//每日
			if (NowTime.Time() < alarmStartTime.Time() || NowTime.Time() > alarmStopTime.Time())
			{
				m_state = eStop;
			}
		}
		else if (m_mode == 1)
		{
			//每周
			if(NowTime.WeekDay() == m_value)
			{
				if (NowTime.Time() < alarmStartTime.Time() || NowTime.Time() > alarmStopTime.Time())
				{
					m_state = eStop;
				}
			}
			else
			{
				m_state = eStop;
			}
		}
		else if (m_mode == 2)
		{
			//每月
			if(localTime.day == m_value)
			{
				if (NowTime.Time() < alarmStartTime.Time() || NowTime.Time() > alarmStopTime.Time())
				{
					m_state = eStop;
				}
			}
			else
			{
				m_state = eStop;
			}
		}
		else if (m_mode == 3)
		{
			//每年
			INT nNow = localTime.month * 100 + localTime.day;
			if(nNow == m_value)
			{
				if (NowTime.Time() < alarmStartTime.Time() || NowTime.Time() > alarmStopTime.Time())
				{
					m_state = eStop;
				}
			}
			else
			{
				m_state = eStop;
			}
		}
	}
}

//// KLogicTimer //////////////////////////

KLogicTimer::KLogicTimer():m_milliSecs(0), m_stepMilliSecs(0), m_bIsSuspend(false), m_bIsAutoStop(true)
{

}

KLogicTimer::~KLogicTimer()
{

}

bool KLogicTimer::start(DWORD name, Identity64 cookie, int secs, void* data)
{
	m_name = name; m_cookie = cookie;
	m_milliSecs = secs * 1000;
	m_stepMilliSecs = m_milliSecs;
	m_data = data;
	m_bIsSuspend = false;
	m_bIsAutoStop = true;
	return true;
}

bool KLogicTimer::start_m(DWORD name, Identity64 cookie, int msecs, void* data)
{
	m_name = name; m_cookie = cookie;
	m_milliSecs = msecs;
	m_stepMilliSecs = m_milliSecs;
	m_data = data;
	m_bIsSuspend = false;
	m_bIsAutoStop = true;
	return true;
}

bool KLogicTimer::restart()
{
	m_milliSecs += m_stepMilliSecs;
	return true;
}

void KLogicTimer::elapse(int milli_secs)
{
	m_milliSecs -= milli_secs;
}

bool KLogicTimer::isTimeout() const
{
	return m_milliSecs < 1;
}

//// KLogicTimerManager

KLogicTimerManager::KLogicTimerManager()
{
	base_type::m_receiver = this;
}

KLogicTimerManager::~KLogicTimerManager()
{

}

void KLogicTimerManager::breathe(int ms)
{
	base_type::breathe(ms);
}

KLogicTimer* KLogicTimerManager::getTimer(DWORD name, Identity64 cookie)
{
	return base_type::get(name, cookie);
}

bool KLogicTimerManager::startTimer(DWORD name, Identity64 cookie, float secs, void* data)
{
	return base_type::start(name, cookie, (int)secs, data) != NULL;
}

bool KLogicTimerManager::startTimerAt(DWORD name, Identity64 cookie, TIME_T when, void* data)
{
	return base_type::startAt(name, cookie, when, data) != NULL;
}

void KLogicTimerManager::stopTimer(DWORD name, Identity64 cookie)
{
	base_type::stop(name, cookie);
}

void KLogicTimerManager::clear()
{
	base_type::clear();
}