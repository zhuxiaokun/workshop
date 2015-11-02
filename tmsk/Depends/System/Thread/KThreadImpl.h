/********************************************************************
	created:	2008/04/25
	created:	25:4:2008   13:45
	filename: 	Athena\Base\System\Thread\KThreadImpl.h
	file path:	Athena\Base\System\Thread
	file base:	KThreadImpl
	file ext:	h
	author:		xueyan
	
	purpose:	抽象了线程的所有操作
*********************************************************************/

#pragma once

#include "../KType.h"
#include "../KMacro.h"
//#include "../SockHeader.h"

#ifndef WINDOWS
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#else
#pragma warning(disable:4355)
#endif

namespace System {			namespace Thread {

///////////////////////////////////////////////////////////////////////////////
// class KThreadImpl - 平台线程实现基类

class KThread;

// 线程优先级
enum 
{
	THREAD_PRI_IDLE         = 0,
	THREAD_PRI_LOWEST       = 1,
	THREAD_PRI_BELOWNORMAL  = 2,
	THREAD_PRI_NORMAL       = 3,
	THREAD_PRI_HIGHER       = 4,
	THREAD_PRI_HIGHEST      = 5,
	THREAD_PRI_TIMECRITICAL = 6
};

class KThreadImpl
{
	friend class KThread;

protected:
	KThread&		m_Thread;					// 相关联的 KThread 对象
	uint_r			m_dwThreadId;                // 线程ID
	BOOL			m_bFinished;				// 线程是否已完成了线程函数的执行
	int				m_nTermElapsedSecs;         // 从调用 Terminate 到当前共经过多少时间(秒)
	BOOL			m_bFreeOnTerminate;			// 线程退出时是否同时释放类对象
	BOOL			m_bTerminated;				// 是否应退出的标志
	int				m_nReturnValue;             // 线程返回值 (可在 Execute 函数中修改此值，函数 WaitFor 返回此值)

protected:
	void			Execute();
	void			BeforeTerminate();
	void			BeforeKill();

	void			CheckNotRunning();
public:
	KThreadImpl(KThread *pThread);
	virtual ~KThreadImpl();

	virtual void	Run() = 0;
	virtual void	Exit(bool bForce) = 0;	// 2010/05/19韩玉信添加的专用退出线程函数：参数意义是是否强制杀死线程，一般由“该线程是否正在工作”来确定是和平退出还是强制退出
	virtual void	Terminate();
	virtual void	Kill() = 0;
	virtual int		WaitFor() = 0;
	virtual void	Sleep(INT nMisSeconds) = 0;

	// 属性 (getter)
	KThread*		GetThread() { return (KThread*)&m_Thread; }
	DWORD			GetThreadId() const { return (DWORD)m_dwThreadId; }
	int				GetTerminated() const { return m_bTerminated; }
	int				GetReturnValue() const { return m_nReturnValue; }
	BOOL			GetFreeOnTerminate() const { return m_bFreeOnTerminate; }
	int				GetTermElapsedSecs() const;
	// 属性 (setter)
	void			SetThreadId(DWORD dwValue) { m_dwThreadId = dwValue; }
	void			SetFinished(BOOL bValue) { m_bFinished = bValue; }
	void			SetTerminated(BOOL bValue);
	void			SetReturnValue(int nValue) { m_nReturnValue = nValue; }
	void			SetFreeOnTerminate(BOOL bValue) { m_bFreeOnTerminate = bValue; }
};


} /*Thread*/				} /* System */

// using namespace ::System::Thread;