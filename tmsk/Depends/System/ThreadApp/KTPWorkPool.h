#ifndef __KTPWORKPOOL_H__
#define __KTPWORKPOOL_H__
#include "KThreadPool.h"
namespace System { namespace Thread {

	class KTPWorkPool : public KThreadPool
	{
	public:
		friend class KTPWorkerProc;
		class KTPWorkerProc : public WorkerProc
		{
		public:
			KTPWorkerProc();
			virtual ~KTPWorkerProc();
			virtual void  Execute(KThreadInstance* pThreadInstance);
		};

		KTPWorkPool();
		virtual ~KTPWorkPool();

		virtual void Initialize(unsigned int iMaxWorkerThreads,unsigned int iMaxPrioritys,KThreadUnitNodePool* pThreadUnitNodePool = NULL,int nThreadPriority = THREAD_PRI_NORMAL);
		virtual void Shutdown();
		virtual KThreadUnitNode* DequeueNode(WorkerProc* pWorkerProc);

	protected:		
		virtual KThreadPool::WorkerProc* CreateWorkerProc(unsigned int iMaxWorkerThreads);
	
	protected:	
		UINT32*									 m_pCurrentWorkflowList;
		unsigned short*							 m_pCurrentStageList;
		Sync::KRecursiveMutex					 m_DataLock;
	};

	
} /*Thread*/ } /* System */

#endif