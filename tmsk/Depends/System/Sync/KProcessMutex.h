#ifndef _K_PROCESS_MUTEX_H_
#define _K_PROCESS_MUTEX_H_

#include "../KType.h"
#if defined(WIN32) || defined(WINDOWS)
#else
#include <semaphore.h>
#endif

class KProcessMutex
{
public:
	KProcessMutex();
	~KProcessMutex();

public:
	static const char* CreateMutexName(char* buf);

public:
	BOOL Initialize(const char* mutexName);
	void Finalize();

public:
	BOOL Lock();
	void Unlock();
	BOOL TryLock(int milliSec);

private:
#if defined (WIN32) || defined(WINDOWS)
	HANDLE m_hMutex;
#else
	sem_t* m_psem;
#endif
};

class KAutoProcessMutex
{
public:
	KProcessMutex& m_mx;
	KAutoProcessMutex(KProcessMutex& mx):m_mx(mx)
	{
		m_mx.Lock();
	}
	~KAutoProcessMutex()
	{
		m_mx.Unlock();
	}
};

#endif