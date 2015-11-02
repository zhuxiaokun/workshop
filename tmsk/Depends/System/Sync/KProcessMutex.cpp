#include "KProcessMutex.h"
#include "KSync.h"
#include <stdio.h>

//using namespace ::System::Sync;

#if defined(WIN32) || defined(WINDOWS)
//
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

KProcessMutex::KProcessMutex()
{
#if defined (WIN32) || defined(WINDOWS)
	m_hMutex = NULL;
#else
	m_psem = SEM_FAILED;
#endif
}

KProcessMutex::~KProcessMutex()
{
	this->Finalize();
}

const char* KProcessMutex::CreateMutexName(char* buf)
{
	static int m_num = 0;
	static JG_S::KThreadMutex m_mx;
	const char m_baseName[] = "_p_mx_";

	int myNum;

	m_mx.Lock();
	myNum = m_num++;
	m_mx.Unlock();

	sprintf(buf, "%s%04d", m_baseName, myNum);
	return buf;
}

BOOL KProcessMutex::Initialize(const char* mutexName)
{
#if defined (WIN32) || defined(WINDOWS)
	m_hMutex = ::CreateMutex(NULL, FALSE, mutexName);
	if(!m_hMutex) return FALSE;
#else
	m_psem = sem_open(mutexName, O_CREAT, 0777, 1);
	if(m_psem == SEM_FAILED) return FALSE;
#endif
	return TRUE;
}

void KProcessMutex::Finalize()
{
#if defined (WIN32) || defined(WINDOWS)
	if(m_hMutex)
	{
		::CloseHandle(m_hMutex);
		m_hMutex = NULL;
	}
#else
	if(m_psem != SEM_FAILED)
	{
		sem_close(m_psem);
		m_psem = SEM_FAILED;
	}
#endif
}

BOOL KProcessMutex::Lock()
{
#if defined (WIN32) || defined(WINDOWS)
	DWORD dwRet = ::WaitForSingleObject(m_hMutex, INFINITE);
	return dwRet == WAIT_OBJECT_0;
#else
	return sem_wait(m_psem) == 0;
#endif
}

void KProcessMutex::Unlock()
{
#if defined (WIN32) || defined(WINDOWS)
	::ReleaseMutex(m_hMutex);
#else
	sem_post(m_psem);
#endif
}

BOOL KProcessMutex::TryLock(int milliSec)
{
#if defined (WIN32) || defined(WINDOWS)
	DWORD dwRet = ::WaitForSingleObject(m_hMutex, milliSec);
	return dwRet == WAIT_OBJECT_0;
#else
	return sem_trywait(m_psem) == 0;
#endif
}