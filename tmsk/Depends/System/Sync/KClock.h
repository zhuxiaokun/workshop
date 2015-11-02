/********************************************************************
	created:	2008/03/19
	created:	19:3:2008   9:51
	filename: 	d:\CVSRoot(J)\Athena\Base\System\Sync\KClock.h
	file path:	d:\CVSRoot(J)\Athena\Base\System\Sync
	file base:	KClock
	file ext:	h
	author:		dengkai
	
	purpose:	
*********************************************************************/

#pragma once

#include "../KType.h"
//#ifdef _CLIENT
namespace BASE_CLOCK
{

class BASE_Clock2
{
public:
	BASE_Clock2() : m_deltaT(0.0f), beginTime(0.0f)
	{
		I_BeginTime();
	}
	virtual ~BASE_Clock2() {}
	virtual void Process()
	{
		m_deltaT = I_EndTime();
		I_BeginTime();
	}
	const float GetDeltaT(void)
	{
		return (float)m_deltaT;
	}
	double I_FloatTime( void )
	{
		static double freq = 0.0;
		static __int64 firstCount;
		__int64	curCount;

		if (freq == 0.0)
		{
			__int64 perfFreq;
			QueryPerformanceFrequency( (LARGE_INTEGER*)&perfFreq );
			QueryPerformanceCounter( (LARGE_INTEGER*)&firstCount );
			freq = 1.0 / (double)perfFreq;
		}

		QueryPerformanceCounter ( (LARGE_INTEGER*)&curCount );
		curCount -= firstCount;
		double time = (double)curCount * freq;
		return time;	
	}
private:
	double m_deltaT;
	double beginTime;

	void I_BeginTime( void )
	{
		beginTime = I_FloatTime();
	}
	double I_EndTime( void )
	{
		return ( I_FloatTime() - beginTime );
	}
};

};
// using namespace BASE_CLOCK;
//#endif