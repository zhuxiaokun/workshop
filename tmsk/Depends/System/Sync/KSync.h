/********************************************************************
	created:	2007/10/22
	created:	22:10:2007   9:48
	filename: 	KSync.h
	file ext:	h
	author:		xuqin

	1.updated by xueyan 2007-11-15,调整了一个宏定义，注释了KSync_CSAuto中两
	个没有用的函数。
	
	purpose:	多线程之间的多种同步机制的跨平台实现
*********************************************************************/
#pragma once

// added by xueyan 
#include "../KType.h"
#include "../KMacro.h"

#if defined(WINDOWS)
	//#include "windows.h"
#else
	//#include "macro.h"
	#include <pthread.h>   
	#include <unistd.h>  
	#include <stdio.h>
	#include <sys/shm.h>
	#include <sys/time.h>
	#include <time.h>
	#include <sys/types.h>
	#include <sys/ipc.h>
	#include <sys/sem.h>
	#include <semaphore.h>
#endif


namespace System { namespace Sync {

class KSync_CS
{
public:
	KSync_CS()
	{
		#ifdef WINDOWS
			InitializeCriticalSection(&GetCS());
		#else
			pthread_mutexattr_t attr;
			pthread_mutexattr_init(&attr);
			pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
			pthread_mutex_init(&GetCS(), &attr);
		#endif
	}
	~KSync_CS()
	{
		#ifdef WINDOWS
			DeleteCriticalSection( &GetCS() );
		#else
			pthread_mutex_destroy( &GetCS() );
		#endif
	}
	inline void Lock() const
	{
		#ifdef WINDOWS
			EnterCriticalSection( (CRITICAL_SECTION*)&GetCS() );
		#else
			pthread_mutex_lock( (pthread_mutex_t*)&GetCS() );
		#endif
	}
	inline void Unlock() const
	{
		#ifdef WINDOWS
			LeaveCriticalSection( (CRITICAL_SECTION*)&GetCS() );
		#else
			pthread_mutex_unlock( (pthread_mutex_t*)&GetCS() );
		#endif
	}

#ifdef WINDOWS
	CRITICAL_SECTION& GetCS() const
	{
		return m_CS; 
	}
#else
	pthread_mutex_t &GetCS() const
	{
		return m_CS; 
	}
#endif

protected:
	#ifdef WINDOWS
		mutable CRITICAL_SECTION m_CS;
	#else
		mutable pthread_mutex_t m_CS;
	#endif
};

class KSync_CSAuto
{
public:
	KSync_CSAuto(KSync_CS &cCS) : m_pCS(&cCS.GetCS())
	{
		#ifdef WINDOWS
			EnterCriticalSection( m_pCS );
		#else
			pthread_mutex_lock( m_pCS );
		#endif
	}
	~KSync_CSAuto()
	{
		#ifdef WINDOWS
			LeaveCriticalSection( m_pCS );
		#else
			pthread_mutex_unlock( m_pCS );
		#endif
	}

protected:
	#ifdef WINDOWS
		CRITICAL_SECTION* m_pCS;
	#else
		pthread_mutex_t* m_pCS;
	#endif
};

#ifdef WINDOWS

	class KSync_Event
	{
	protected:
		KSync_Event(int a)
			:m_hEvent(NULL)
		{
			// do nothing 
		}
	public:
		KSync_Event() 
		{
			SetEvent(CreateEvent(0, TRUE, FALSE, 0));
		}
		~KSync_Event() 
		{
			CloseHandle(m_hEvent);
		}
		HANDLE GetEvent() const
		{
			return m_hEvent; 
		}
		void Set() 
		{
			::SetEvent(GetEvent()); 
		}
		bool Clear()
		{ 
			return ResetEvent(GetEvent()) == TRUE; 
		}
		bool IsSet()
		{
			return WaitForSingleObject(GetEvent(), 0) == WAIT_OBJECT_0; 
		}
		bool Block(unsigned int nTimeout = INFINITE) 
		{
			return WaitForSingleObject(GetEvent(), nTimeout) == WAIT_OBJECT_0; 
		}
	protected:
		void SetEvent(HANDLE hEvent) 
		{
			m_hEvent = hEvent; 
		}
	private:
		HANDLE m_hEvent;
	};

	class KSync_PulseEvent : public KSync_Event
	{
	public:
		KSync_PulseEvent()
			:KSync_Event(0)
		{
			SetEvent(CreateEvent(0, FALSE, FALSE, 0));
		}
	};

#else

	class KSync_Event
	{
	public:
		KSync_Event() 
		{
			pthread_cond_init( &m_COND,0 );
		}
		~KSync_Event() 
		{
			pthread_cond_destroy( &m_COND );
		}
		pthread_cond_t GetEvent()
		{
			return m_COND; 
		}
		void Set() 
		{
			pthread_cond_signal(&m_COND);
		}
		bool Block(pthread_mutex_t mutex)
		{
			pthread_cond_wait(&m_COND,&mutex);
			return true;
		}
	protected:
	private:
		pthread_cond_t	m_COND;
	};

	class KSync_PulseEvent
	{
	public:
		KSync_PulseEvent()
		{
			pthread_mutex_init(&m_mutex, NULL);
			pthread_cond_init(&m_cond, NULL);
		}
		~KSync_PulseEvent()
		{
			pthread_cond_destroy(&m_cond);
			pthread_mutex_destroy(&m_mutex);
		}
		int Block(int millsec)
		{
			struct timeval now;
			struct timespec timeout;
			
			gettimeofday(&now, NULL);
			
			int secs = millsec / 1000;
			millsec %= 1000;

			now.tv_sec += secs;
			now.tv_usec += millsec * 1000;

			secs = now.tv_usec / 1000000;
			now.tv_sec += secs;
			now.tv_usec %= 1000000;

			timeout.tv_sec = now.tv_sec;
			timeout.tv_nsec = now.tv_usec * 1000;

			pthread_mutex_lock(&m_mutex);
			int nRet = pthread_cond_timedwait(&m_cond, &m_mutex, &timeout);
			pthread_mutex_unlock(&m_mutex);

			return nRet;
		}
		void Set()
		{
			pthread_mutex_lock(&m_mutex);
			pthread_cond_broadcast(&m_cond);
			pthread_mutex_unlock(&m_mutex);
		}

	protected:
		pthread_mutex_t m_mutex;
		pthread_cond_t m_cond;
	};

#endif

#ifdef WINDOWS
	class KSemaphore
	{
	public:
		KSemaphore() 
		{
			m_hSemaphore = NULL;
		}
		~KSemaphore() 
		{
			Destroy();
		}
		BOOL Create()
		{
			if ( NULL == m_hSemaphore )
			{
				m_hSemaphore = ::CreateSemaphore(NULL,0,65000,NULL);
			}
			return TRUE;
		}
		BOOL Destroy()
		{
			if ( NULL != m_hSemaphore)
			{
				::CloseHandle( m_hSemaphore );
				m_hSemaphore = NULL;
			}
			return TRUE;
		}
		BOOL release_lock()
		{
			if ( NULL != m_hSemaphore )
			{
				::ReleaseSemaphore( m_hSemaphore, 1, NULL );
			}
			return TRUE;
		}
		BOOL bowait_and_lock()
		{
			if ( NULL != m_hSemaphore )
			{
				::WaitForSingleObject( m_hSemaphore, INFINITE );
			}
			return TRUE;
		}
	private:
		HANDLE						m_hSemaphore;
	};

#else

	class KSemaphore
	{
	public:
		KSemaphore() 
		{
		}
		~KSemaphore() 
		{
		}

		BOOL Create()
		{
			sem_init(&sem,0,0);
			return TRUE;
		}
		BOOL Destroy()
		{
			return TRUE;
		}
		BOOL bowait_and_lock()
		{
			sem_wait(&sem);
			return TRUE;
		}
		BOOL release_lock()
		{
			sem_post(&sem);
			return TRUE;
		}
	private:
		sem_t sem;
	};

#endif

/**
* 作为模板参数来实现非线程保护模式
*/
class KNoneLock
{
public:
	void Lock() const
	{
	}
	void Unlock() const
	{
	}
};
/**
 * 多线程互斥量
 */
typedef KSync_CS KMTLock;
typedef KSync_CS KThreadMutex;
typedef KSync_CSAuto KAutoThreadMutex;

template <typename L> class KAutoLock
{
public:
	typedef L lock_type;
	const lock_type& m_lock;

public:
	KAutoLock(const lock_type& lock):m_lock(lock)
	{
		m_lock.Lock();
	}
	~KAutoLock()
	{
		m_lock.Unlock();
	}
};

///// 线程可重入的互斥对象
//class KRecursiveMutex
//{
//public:
//	KThreadMutex m_mx;
//	volatile uint_r m_tid;		// 被持有的当前线程
//	volatile int m_refCount;	// 被当前线程锁了多少次
//
//private:
//	KRecursiveMutex(const KRecursiveMutex&);
//	KRecursiveMutex& operator = (const KRecursiveMutex&);
//
//public:
//	KRecursiveMutex();
//	~KRecursiveMutex();
//
//public:
//	void Lock() const ;
//	void Unlock() const;
//};

typedef KThreadMutex KRecursiveMutex;
typedef KAutoLock<KRecursiveMutex> KAutoRecursiveMutex;

// 原子函数

#ifdef WINDOWS

# define _KINTERLOCKEDINCREMENT(lpAddend) InterlockedIncrement (lpAddend)
# define _KINTERLOCKEDDECREMENT(lpAddend) InterlockedDecrement (lpAddend)
# define _KINTERLOCKEDEXCHANGEPOINTER(Destination,Value) InterlockedExchangePointer(((PVOID*)Destination),Value)

#else

typedef struct { volatile int counter; } k_atomic_t;
typedef struct { volatile void* counter; } k_atomic_ptr;

#if defined(__x64__) //x86_64
static __inline__ void k_atomic_inc(volatile LONG *vptr)
{
#if defined(__clang__)
    
#else
	k_atomic_t* v = (k_atomic_t*)vptr;
	__asm__ __volatile__(
		"lock;" "incl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
#endif
}
static __inline__ void k_atomic_dec(volatile LONG *vptr)
{
#if defined(__clang__)
    
#else
	k_atomic_t* v = (k_atomic_t*)vptr;
	__asm__ __volatile__(
		"lock;" "decl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
#endif
}
#else // i386
static inline void k_atomic_inc(volatile LONG *vptr)
{
#if defined(__clang__)
    
#else
	k_atomic_t* v = (k_atomic_t*)vptr;
	__asm__ __volatile__(
		"lock;" "incl %0"
		:"+m" (v->counter));
#endif
}
static inline void k_atomic_dec(volatile LONG *vptr)
{
#if defined(__clang__)
    
#else
	k_atomic_t* v = (k_atomic_t*)vptr;
	__asm__ __volatile__(
		"lock;" "decl %0"
		:"+m" (v->counter));
#endif
}
#endif

template <typename T> void k_atomic_set(T** vptr, void* i)
{
	k_atomic_ptr* v = (k_atomic_ptr*)vptr;
	v->counter = i;
}

#define _KINTERLOCKEDINCREMENT(vp) ::System::Sync::k_atomic_inc(vp)
#define _KINTERLOCKEDDECREMENT(vp) ::System::Sync::k_atomic_dec(vp)
#define _KINTERLOCKEDEXCHANGEPOINTER(Destination,Value) ::System::Sync::k_atomic_set(Destination,Value)

#endif

} /*Sync*/ } /* System */
