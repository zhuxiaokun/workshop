#include "KTPWorkPool.h"
#include "KThreadProcessorUnit.h"
namespace System { namespace Thread {

	KTPWorkPool::KTPWorkPool()
		:m_pCurrentWorkflowList(NULL)
		,m_pCurrentStageList(NULL)
	{
		
	}

	KTPWorkPool::~KTPWorkPool()
	{

	}

	void KTPWorkPool::Initialize(unsigned int iMaxWorkerThreads,unsigned int iMaxPrioritys,KThreadUnitNodePool* pThreadUnitNodePool,int nThreadPriority)
	{
		if(NULL == m_pCurrentWorkflowList)
		{
			m_pCurrentWorkflowList = new UINT32[iMaxWorkerThreads];
			memset(m_pCurrentWorkflowList,0,sizeof(UINT32)*iMaxWorkerThreads);
		}

		if(NULL == m_pCurrentStageList)
		{
			m_pCurrentStageList = new unsigned short[iMaxWorkerThreads];
			memset(m_pCurrentStageList,0,sizeof(unsigned short)*iMaxWorkerThreads);
		}
		KThreadPool::Initialize(iMaxWorkerThreads, iMaxPrioritys, pThreadUnitNodePool,nThreadPriority);
	}

	void KTPWorkPool::Shutdown()
	{
		KThreadPool::Shutdown();
		if(NULL == m_pCurrentWorkflowList)
		{
			delete m_pCurrentWorkflowList;
			m_pCurrentWorkflowList = NULL;
		}

		if(NULL == m_pCurrentStageList)
		{
			delete m_pCurrentStageList;
			m_pCurrentStageList = NULL;
		}
	}

	KThreadUnitNode* KTPWorkPool::DequeueNode(WorkerProc* pWorkerProc)
	{
		//return KThreadPool::DequeueNode(pWorkerProc);
		m_ThreadUnitSemaphore.bowait_and_lock();
		KThreadUnitNode* pThreadUnitNode = m_ThreadUnitQueue.Dequeue();
		
		if(NULL != pThreadUnitNode)
		{
			KIThreadProcessorUnit* pThreadProcessorUnit = (KIThreadProcessorUnit*)(pThreadUnitNode->GetThreadUnit());

			m_DataLock.Lock();
			m_pCurrentWorkflowList[pWorkerProc->GetThreadID()] = pThreadProcessorUnit->GetWorkflowID();
			m_pCurrentStageList[pWorkerProc->GetThreadID()] = pThreadProcessorUnit->GetStage();
			m_DataLock.Unlock();

			if(pThreadProcessorUnit->IsNeedYield())
			{
				unsigned int i;
				bool bWait = true;

				while (bWait)
				{
					for (i = 0; i < m_uiMaxWorkerThreads; i++)
					{

						if (pThreadProcessorUnit->GetWorkflowID() == m_pCurrentWorkflowList[i] &&
							pThreadProcessorUnit->GetStage() > m_pCurrentStageList[i])
						{
							break; // break out of for loop
						}

					}

					if (i == m_uiMaxWorkerThreads)
					{
						bWait = false;
					}
					else
					{
						yieldThread_k();
					}
				}
			}
			
		}
		
		return pThreadUnitNode;
	}

	KThreadPool::WorkerProc* KTPWorkPool::CreateWorkerProc(unsigned int iMaxWorkerThreads)
	{
		return new KTPWorkerProc[iMaxWorkerThreads];
	}

	KTPWorkPool::KTPWorkerProc::KTPWorkerProc()
	{
		
	}

	KTPWorkPool::KTPWorkerProc::~KTPWorkerProc()
	{
		
	}

	
	void  KTPWorkPool::KTPWorkerProc::Execute(KThreadInstance* pThreadInstance)
	{
		if( NULL != m_pThreadPool->GetCooperater() )
		{
			m_pThreadPool->GetCooperater()->registerThread(pThreadInstance);
		}
		m_bShutdown = false;
		while (!m_bShutdown)
		{
			KThreadUnitNode* pThreadUnitNode = m_pThreadPool->DequeueNode(this);
			if(NULL != pThreadUnitNode)
			{
				pThreadUnitNode->Execute();
				m_pThreadPool->GetNodePool()->Free(pThreadUnitNode);
				
				((KTPWorkPool*)m_pThreadPool)->m_pCurrentWorkflowList[m_uiThreadID] = 0;
				((KTPWorkPool*)m_pThreadPool)->m_pCurrentStageList[m_uiThreadID] = 0; 
			}
		}

		if( NULL != m_pThreadPool->GetCooperater() )
		{
			m_pThreadPool->GetCooperater()->unregisterThread(pThreadInstance);
		}
	}

	

} /*Thread*/ } /* System */