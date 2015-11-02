#include "KTPJobList.h"
#include "KTPKernel.h"
#include "KTPTask.h"
namespace System { namespace Thread {
	
	KTPJobList::KTPJobList()
		:m_pTask(NULL)
	{
	}

	KTPJobList::~KTPJobList()
	{
	}

	void KTPJobList::ThreadExecute()
	{
		m_Workload.GetKernel()->Execute(m_Workload);
	}
	
	void KTPJobList::CompleteInThread()
	{
		SetThreadState(THREADSTATE_COMPLETED);

		m_pTask->NotifyJobListComplete();
	}

	void KTPJobList::SetThreadState(KIThreadUnit::ThreadState iThreadState)
	{
		m_ThreadStatusMutex.Lock();
		m_ThreadState = iThreadState;

        char str[256];
		sprintf(str,"KTPJobList::SetThreadState %d\n",iThreadState);
		//OutputDebugString(str);
		m_ThreadStatusMutex.Unlock();
		
	}

} /*Thread*/ } /* System */