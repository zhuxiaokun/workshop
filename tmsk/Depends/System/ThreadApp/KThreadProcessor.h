#ifndef __KTHREADPROCESSOR_H__
#define __KTHREADPROCESSOR_H__
#include "KThreadPool.h"
#include "KTPObjPool.h"
#include "KTPWorkPool.h"
namespace System { namespace Thread {

	class KTPWorkflow;
	class KIThreadProcessorUnit;
	class KThreadProcessor
	{
	public:
		enum WorkingMode
		{
			WORKINGMODE_MAINTHREAD,
			WORKINGMODE_SERIAL,
			WORKINGMODE_PARALLEL
		};
		//friend class KTPWorkflow;
		class ManagerProc : public KIThreadFunctor
		{
		public:
			ManagerProc();
			virtual ~ManagerProc();
			virtual void  Execute(KThreadInstance* pThreadInstance);
			void SetProcessor(KThreadProcessor* pProcessor){m_pProcessor = pProcessor;}
			void Shutdown() {m_bShutdown = true;}
			bool IsShutdown(){return m_bShutdown;}
		protected:
			KThreadProcessor*	  m_pProcessor;
			volatile bool		  m_bShutdown;
		};
	public:
		KThreadProcessor();
		virtual ~KThreadProcessor();
		// 
		virtual void initialize(unsigned int iMaxWorkerThreads,unsigned int iPriorityCount);
		virtual void Shutdown();
		virtual bool Submit(KTPWorkflow* pWorkflow, unsigned int iPriority, WorkingMode iMode = WORKINGMODE_PARALLEL);
		virtual bool ExecuteProcessor(KIThreadProcessorUnit* pThreadProcessorUnit);
		
		virtual void SubmitForComplete(KTPWorkflow* pkWorkflow);
	
		inline KTPObjPool&  GetObjPool(){return m_ObjPools;}

		bool IsShutdown();
		void ClearPool();
	protected:
		
		// submit
		Sync::KSemaphore		 m_SubmitSemaphore;
		KThreadUnitQueue		 m_WorkflowEventQueue;
		KThreadUnitQueue         m_WorkflowPriorityQueue;
		ManagerProc*			 m_pManagerProc;
		KThreadInstance*		 m_pManagerThread;
		
		typedef Collections::ListNode<KTPWorkflow*> KTPWorkflowNode;
		typedef Memory::KStepObjectPool<KTPWorkflowNode,64,Sync::KThreadMutex> KTPWorkflowNodePool;
		Collections::KSelfList<KTPWorkflowNode>  m_CompleteList;
		Sync::KRecursiveMutex					 m_CompleteMutex;
		KTPWorkflowNodePool						 m_WorkflowNodePool;

		// Worker
		KTPWorkPool				 m_WorkerThreadPool;
		KThreadUnitNodePool*	 m_pThreadUnitNodePool;

		// pool
		KTPObjPool				 m_ObjPools;

	};

} /*Thread*/ } /* System */

#endif