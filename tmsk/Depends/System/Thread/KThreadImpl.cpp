
#include "KThread.h"
#include "KThreadImpl.h"
#include <time.h>

///////////////////////////////////////////////////////////////////////////////
// class KThreadImpl
namespace System {			namespace Thread {

KThreadImpl::KThreadImpl(KThread *pThread) :
						m_Thread(*pThread),
						m_dwThreadId(0),
						m_bFinished(false),
						m_nTermElapsedSecs(0),
						m_bFreeOnTerminate(false),
						m_bTerminated(false),
						m_nReturnValue(0)
{
}

KThreadImpl::~KThreadImpl()
{
	//
}

void KThreadImpl::Execute()
{
	m_Thread.Execute();
}

void KThreadImpl::BeforeTerminate()
{
	m_Thread.BeforeTerminate();
}

void KThreadImpl::BeforeKill()
{
	m_Thread.BeforeKill();
}

//-----------------------------------------------------------------------------
// 描述: 如果线程已运行，则抛出异常
//-----------------------------------------------------------------------------
void KThreadImpl::CheckNotRunning()
{
	ASSERT(0 == m_dwThreadId );
}

//-----------------------------------------------------------------------------
// 描述: 通知线程退出
//-----------------------------------------------------------------------------
void KThreadImpl::Terminate()
{
	try { BeforeTerminate(); } catch (...) {}

	if (!m_bTerminated) m_nTermElapsedSecs = (TIME_T)time(NULL);
	m_bTerminated = true;
}

//-----------------------------------------------------------------------------
// 描述: 取得从调用 Terminate 到当前共经过多少时间(秒)
//-----------------------------------------------------------------------------
int KThreadImpl::GetTermElapsedSecs() const
{
	int nResult = 0;

	// 如果已经通知退出，但线程还活着
	if (m_bTerminated && m_dwThreadId != 0)
	{
		nResult = (TIME_T)time(NULL) - m_nTermElapsedSecs;
	}

	return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 设置是否 Terminate
//-----------------------------------------------------------------------------
void KThreadImpl::SetTerminated(BOOL bValue)
{
	if (bValue != m_bTerminated)
	{
		if (bValue)
			Terminate();
		else
		{
			m_bTerminated = false;
			m_nTermElapsedSecs = 0;
		}
	}
}

} /*Thread*/				} /* System */