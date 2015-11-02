#include "KThreadProcessor.h"
#include "KTPWorkflow.h"
#include "KThreadProcessorUnit.h"
namespace System { namespace Thread {

	KThreadProcessor::KThreadProcessor()
		:m_ObjPools(this)
		,m_pManagerProc(NULL)
		,m_pManagerThread(NULL)
	{
		
	}
	
	KThreadProcessor::~KThreadProcessor()
	{
		Shutdown();
		ClearPool();
	}

	void KThreadProcessor::initialize(unsigned int iMaxWorkerThreads,unsigned int iPriorityCount)
	{
		ASSERT(m_ObjPools.IsInitialize());
		
		m_WorkerThreadPool.Initialize(iMaxWorkerThreads,1,m_ObjPools.GetUnitNodePool(),THREAD_PRI_HIGHEST);

		m_pManagerProc = new ManagerProc;
		m_pManagerThread = new KThreadInstance(m_pManagerProc);
		m_pManagerThread->SetPriority(THREAD_PRI_HIGHEST);
		m_pManagerProc->SetProcessor(this);
		m_pManagerThread->Run();

		m_SubmitSemaphore.Create();
		
		m_WorkflowEventQueue.Initialize(1);
		m_WorkflowPriorityQueue.Initialize(iPriorityCount);
		
	}

	void KThreadProcessor::Shutdown()
	{
		if(NULL != m_pManagerProc)
		{
			m_pManagerProc->Shutdown();
		}
		m_SubmitSemaphore.release_lock();
		
		if(NULL != m_pManagerThread)
		{
			m_pManagerThread->WaitFor();
			delete m_pManagerThread;
			m_pManagerThread = NULL;
		}
		if(NULL != m_pManagerProc)
		{
			delete m_pManagerProc;
			m_pManagerProc = NULL;
		}

		m_WorkerThreadPool.Shutdown();
	
		
	}

	bool KThreadProcessor::IsShutdown()
	{
		bool bShutdown = false;
		if( NULL != m_pManagerProc )
		{
			bShutdown = m_pManagerProc->IsShutdown();
		}
		return bShutdown;
	}

	void KThreadProcessor::ClearPool()
	{
		m_WorkflowNodePool.Destroy();
		m_ObjPools.Clear();
	}

	bool KThreadProcessor::Submit(KTPWorkflow* pWorkflow, unsigned int iPriority,WorkingMode iMode)
	{
		bool bRet = false;
		if(!pWorkflow->IsInThread() && !IsShutdown())
		{
			ASSERT(pWorkflow->GetThreadProcessor() == this);
			ASSERT(pWorkflow->GetTaskSize()>0);
			if(iMode != WORKINGMODE_MAINTHREAD)
			{
				pWorkflow->SetPriority(iPriority);
				pWorkflow->SetSerial(iMode == WORKINGMODE_SERIAL);
				pWorkflow->Reset();
				KThreadUnitNode* pNode = m_ObjPools.AllocUnitNode();
				pNode->Initialize(pWorkflow);
				pWorkflow->SetThreadState(KIThreadUnit::THREADSTATE_PENDING);
				m_WorkflowEventQueue.Enqueue(pNode,0);
				m_SubmitSemaphore.release_lock();
				bRet = true;
			}
			else
			{
				pWorkflow->Reset();
				pWorkflow->SetSerial(true);
				pWorkflow->SetThreadState(KIThreadUnit::THREADSTATE_PENDING);
				pWorkflow->SortTask();
				pWorkflow->Prepare();
				pWorkflow->ExecuteSerial();
				pWorkflow->SetThreadState(KIThreadUnit::THREADSTATE_COMPLETED);
			}
		}
		return bRet;
	}

	void KThreadProcessor::SubmitForComplete(KTPWorkflow* pkWorkflow)
	{
		ASSERT(NULL != pkWorkflow);

		KTPWorkflowNode	* pNode = m_WorkflowNodePool.Alloc();
		pNode->val = pkWorkflow;

		m_CompleteMutex.Lock();
		m_CompleteList.push_back(pNode);
		m_CompleteMutex.Unlock();

		m_SubmitSemaphore.release_lock();
	}

	bool KThreadProcessor::ExecuteProcessor(KIThreadProcessorUnit* pThreadProcessorUnit)
	{
		bool bRet = false;
		if(!pThreadProcessorUnit->IsInThread())
		{
			m_WorkerThreadPool.AddRequset(pThreadProcessorUnit,0);
			bRet = true;
		}
		else
		{
			int h = 0;
		}
		return bRet;
	}


	KThreadProcessor::ManagerProc::ManagerProc()
		:m_pProcessor(NULL)
	{

	}

	KThreadProcessor::ManagerProc::~ManagerProc()
	{

	}

	void  KThreadProcessor::ManagerProc::Execute(KThreadInstance* pThreadInstance)
	{
		m_bShutdown = false;
		while (!m_bShutdown)
		{
			m_pProcessor->m_SubmitSemaphore.bowait_and_lock();
			m_pProcessor->m_SubmitSemaphore.release_lock();

			KTPWorkflowNode* pCompleteNode = NULL;
			
			m_pProcessor->m_CompleteMutex.Lock();
			pCompleteNode = m_pProcessor->m_CompleteList.pop_front();
			m_pProcessor->m_CompleteMutex.Unlock();

			while(NULL != pCompleteNode)
			{
				if(NULL != pCompleteNode->val)
				{
					pCompleteNode->val->DealComplete();
					pCompleteNode->val->SetThreadState(KIThreadUnit::THREADSTATE_COMPLETED);
				
					//OutputDebugString("Completed!\n");
				}
				m_pProcessor->m_WorkflowNodePool.Free(pCompleteNode);
				
				m_pProcessor->m_SubmitSemaphore.bowait_and_lock();
				
				m_pProcessor->m_CompleteMutex.Lock();
				pCompleteNode = m_pProcessor->m_CompleteList.pop_front();
				m_pProcessor->m_CompleteMutex.Unlock();
				
			}
			
			if(!m_pProcessor->m_WorkflowEventQueue.IsEmpty())
			{
				 KThreadUnitNode* pThreadUnitNode[8];
				 UINT32 uiIdx = 0;
				 while (uiIdx < 8 && !m_pProcessor->m_WorkflowEventQueue.IsEmpty())
				 {	 
					 pThreadUnitNode[uiIdx++] = m_pProcessor->m_WorkflowEventQueue.Dequeue();
				 }
				 for (UINT32 ui = 0; ui < uiIdx; ui++)
				 {
					 KThreadUnitNode* pWorkflowNode = pThreadUnitNode[ui];
					 if(NULL !=  pWorkflowNode)
					 {
						KTPWorkflow* pWorkflow = (KTPWorkflow*)(pWorkflowNode->GetThreadUnit());
						m_pProcessor->m_WorkflowPriorityQueue.Enqueue( pWorkflowNode, pWorkflow->GetPriority());
						
						//OutputDebugString("m_WorkflowPriorityQueue.Enqueue!\n");
					 }
				 }
			}

			while (!m_pProcessor->m_WorkflowPriorityQueue.IsEmpty())
			{
				KThreadUnitNode* pThreadUnitNode = m_pProcessor->m_WorkflowPriorityQueue.Dequeue();
				ASSERT(NULL != pThreadUnitNode);
				pThreadUnitNode->Execute();
				m_pProcessor->m_ObjPools.FreeUnitNode(pThreadUnitNode);
				m_pProcessor->m_SubmitSemaphore.bowait_and_lock();
			}
		}

		KTPWorkflowNode* pCompleteNode = NULL;

		m_pProcessor->m_CompleteMutex.Lock();
		pCompleteNode = m_pProcessor->m_CompleteList.pop_front();
		m_pProcessor->m_CompleteMutex.Unlock();

		while(NULL != pCompleteNode)
		{
			if(NULL != pCompleteNode->val)
			{
				pCompleteNode->val->DealComplete();
				pCompleteNode->val->SetThreadState(KIThreadUnit::THREADSTATE_COMPLETED);
				//OutputDebugString("Completed!\n");
			}
			m_pProcessor->m_WorkflowNodePool.Free(pCompleteNode);
			m_pProcessor->m_CompleteMutex.Lock();
			pCompleteNode = m_pProcessor->m_CompleteList.pop_front();
			m_pProcessor->m_CompleteMutex.Unlock();

		}

	}



} /*Thread*/ } /* System */