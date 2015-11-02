#ifndef __KTPJOBLIST_H__
#define __KTPJOBLIST_H__
#include "KThreadProcessorUnit.h"
#include "KTPWorkload.h"
namespace System { namespace Thread {

	class KTPTask;
	class KTPJobList : public KIThreadProcessorUnit
	{
	public:
		KTPJobList();
		virtual ~KTPJobList();
		virtual void ThreadExecute();
		virtual void CompleteInThread();
		void SetThreadState(KIThreadUnit::ThreadState iThreadState);
		void SetTask(KTPTask* pTask){ m_pTask = pTask; }
		KTPTask* GetTask() const{ return m_pTask; }
		KTPWorkload& GetWorkload(){return m_Workload;}		

	protected:
		KTPTask*		m_pTask;
		KTPWorkload		m_Workload;
	};

} /*Thread*/ } /* System */
#endif