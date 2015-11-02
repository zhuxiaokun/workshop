#ifndef __KTHREADPOOL_H__
#define __KTHREADPOOL_H__

#include "KThreadInstance.h"
#include "KThreadFunctor.h"
#include "KThreadUnitQueue.h"

namespace System { namespace Thread {
	
	class KThreadPool
	{
	public:
		friend class WorkerProc;
		class ICooperater
		{
		public:
			ICooperater(){}
			virtual ~ICooperater(){}
			
			virtual void preAllThreadsStarted() = 0;
			virtual void postAllThreadsStarted() = 0;
			
			virtual void registerThread(KThreadInstance* pThreadInstance) = 0;
				virtual void unregisterThread(KThreadInstance* pThreadInstance) = 0;	
		};

		class WorkerProc : public KIThreadFunctor
		{
		public:
			WorkerProc();
			virtual ~WorkerProc();
			virtual void  Execute(KThreadInstance* pThreadInstance);
			void SetParentThreadPool(KThreadPool* pThreadPool);
			void Shutdown() {m_bShutdown = true;}
		
			unsigned int GetThreadID() {return m_uiThreadID; }
			void SetThreadID(unsigned int uiNewThreadID) {m_uiThreadID = uiNewThreadID; }
		protected:
			KThreadPool*				m_pThreadPool;
			volatile bool				m_bShutdown;
			volatile unsigned int	    m_uiThreadID;
		};

	public:
		KThreadPool();
		virtual ~KThreadPool();
		virtual void Initialize(unsigned int iMaxWorkerThreads,unsigned int iMaxPrioritys,KThreadUnitNodePool* pThreadUnitNodePool = NULL,int nThreadPriority = THREAD_PRI_NORMAL);
		virtual void Shutdown();
		virtual void AddRequset(KIThreadUnit* pThreadUnit, unsigned int iPriority);
		virtual KThreadUnitNode* DequeueNode(WorkerProc* pWorkerProc);

		void registerCooperater(ICooperater* pCooperater);
		ICooperater*		 GetCooperater() {return m_pCooperater;}
		KThreadUnitNodePool* GetNodePool(){return m_pThreadUnitNodePool;}
	protected:		
		virtual WorkerProc* CreateWorkerProc(unsigned int iMaxWorkerThreads);
	protected:
		KThreadInstance** m_pWorkerThreads;
		WorkerProc*		 m_pWorkerProcs;		
		unsigned int	 m_uiMaxWorkerThreads;

		KThreadUnitQueue m_ThreadUnitQueue;
		
		// 
		Sync::KSemaphore		 m_ThreadUnitSemaphore;

		// pool
		KThreadUnitNodePool*	 m_pThreadUnitNodePool;
		volatile bool			 m_bAttachPool;

		//
		ICooperater*             m_pCooperater;          
	};

} /*Thread*/ } /* System */

#endif