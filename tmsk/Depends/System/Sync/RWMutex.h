#ifndef _RW_MUTEX_H_
#define _RW_MUTEX_H_

#include "KSync.h"
#include "../Random/KRandom.h"

class RWMutex
{
public:
	RWMutex();
	~RWMutex();

	BOOL WaitRead();
	BOOL WaitWrite();

	BOOL WaitRead(int ms);
	BOOL WaitWrite(int ms);
	void ReadPost();
	void WritePost();

	BOOL WaitWriteInRead(int ms);
	void WritePost2Read();

private:
	::System::Sync::KThreadMutex m_mx;
	::System::Sync::KSync_PulseEvent m_event;
	volatile int m_num;
	volatile BOOL m_bWriteWaiting;
	::System::Random::KRandom m_random;
};

class AutoRMutex
{
public:
	AutoRMutex(RWMutex& rwmx);
	~AutoRMutex();

	BOOL WaitWrite();
	void WritePost();

private:
	BOOL m_bInWrite;
	RWMutex& m_rwmx;
};

class AutoWMutex
{
public:
	AutoWMutex(RWMutex& rwmx);
	~AutoWMutex();

private:
	RWMutex& m_rwmx;
};

#endif