#include "KThreadPool.h"
namespace System { namespace Thread {

	KThreadPool::KThreadPool()
		:m_pWorkerThreads(NULL)
		,m_pWorkerProcs(NULL)		
		,m_uiMaxWorkerThreads(0)
		,m_pThreadUnitNodePool(NULL)
		,m_bAttachPool(false)
		,m_pCooperater(NULL)
	{
	}

	KThreadPool::~KThreadPool()
	{
		Shutdown();
	}

	void KThreadPool::Initialize(unsigned int iMaxWorkerThreads,unsigned int iMaxPrioritys,KThreadUnitNodePool* pThreadUnitNodePool,int nThreadPriority)
	{
		ASSERT(NULL == m_pWorkerProcs && NULL == m_pWorkerThreads && iMaxWorkerThreads>0);
		m_pWorkerThreads = new KThreadInstance*[iMaxWorkerThreads];
		m_pWorkerProcs = CreateWorkerProc(iMaxWorkerThreads);
		m_uiMaxWorkerThreads = iMaxWorkerThreads;

		m_ThreadUnitQueue.Initialize(iMaxPrioritys);
		m_ThreadUnitSemaphore.Create();
		
		if(NULL == pThreadUnitNodePool)
		{
			m_pThreadUnitNodePool = new KThreadUnitNodePool;
			m_bAttachPool = false;
		}
		else
		{
			m_pThreadUnitNodePool = pThreadUnitNodePool;
			m_bAttachPool = true;
		}

		if(NULL != m_pCooperater)
		{
			m_pCooperater->preAllThreadsStarted();
		}
		for(unsigned int i = 0;i<m_uiMaxWorkerThreads;++i)
		{
			m_pWorkerThreads[i] = new KThreadInstance(m_pWorkerProcs+i);
			m_pWorkerThreads[i]->SetPriority(nThreadPriority);
			m_pWorkerProcs[i].SetParentThreadPool(this);
			m_pWorkerProcs[i].SetThreadID(i);
			m_pWorkerThreads[i]->Run();
		}
		if(NULL != m_pCooperater)
		{
			m_pCooperater->postAllThreadsStarted();
		}
	}

	void KThreadPool::Shutdown()
	{
		for(unsigned int i = 0;i<m_uiMaxWorkerThreads;++i)
		{
			m_pWorkerProcs[i].Shutdown();
		}

		for(unsigned int i = 0;i<m_uiMaxWorkerThreads;++i)
		{
			m_ThreadUnitSemaphore.release_lock();
		}

		for(unsigned int i = 0;i<m_uiMaxWorkerThreads;++i)
		{
			m_pWorkerThreads[i]->WaitFor();
			delete m_pWorkerThreads[i];
		}
		if(NULL != m_pWorkerThreads)
		{
			delete[] m_pWorkerThreads;
			m_pWorkerThreads = NULL;
		}

		if(NULL != m_pWorkerProcs)
		{
			delete[]m_pWorkerProcs;
			m_pWorkerProcs = NULL;
		}
		m_ThreadUnitQueue.ClearAll();
		m_uiMaxWorkerThreads = 0;
		m_ThreadUnitSemaphore.Destroy();
		
		if(NULL != m_pThreadUnitNodePool && !m_bAttachPool)
		{
			m_pThreadUnitNodePool->Destroy();
			delete m_pThreadUnitNodePool;
		}

		m_pThreadUnitNodePool = NULL;

		m_pCooperater = NULL;
	}

	void  KThreadPool::AddRequset(KIThreadUnit* pThreadUnit, unsigned int iPriority)
	{
		ASSERT(!pThreadUnit->IsInThread())
		KThreadUnitNode* pNode = m_pThreadUnitNodePool->Alloc();
		pNode->Initialize(pThreadUnit);

		pThreadUnit->SetThreadState(KIThreadUnit::THREADSTATE_PENDING);

		m_ThreadUnitQueue.Enqueue(pNode, iPriority);
		m_ThreadUnitSemaphore.release_lock();
	}

	KThreadUnitNode* KThreadPool::DequeueNode(WorkerProc* pWorkerProc)
	{
		m_ThreadUnitSemaphore.bowait_and_lock();
		KThreadUnitNode* pThreadUnitNode = m_ThreadUnitQueue.Dequeue();
		return pThreadUnitNode;
	}

	void KThreadPool::registerCooperater(ICooperater* pCooperater)
	{
		// must be called before called Initialize function
		ASSERT(NULL == m_pCooperater &&NULL == m_pWorkerProcs && NULL != pCooperater);
		m_pCooperater = pCooperater;
	}

	KThreadPool::WorkerProc* KThreadPool::CreateWorkerProc(unsigned int iMaxWorkerThreads)
	{
		return new WorkerProc[iMaxWorkerThreads];
	}
	
	KThreadPool::WorkerProc::WorkerProc()
		:m_pThreadPool(NULL)
		,m_bShutdown(false)
		,m_uiThreadID(0)
	{

	}

	KThreadPool::WorkerProc::~WorkerProc()
	{
		m_pThreadPool = NULL;
	}

	void  KThreadPool::WorkerProc::Execute(KThreadInstance* pThreadInstance)
	{
		if( NULL != m_pThreadPool->m_pCooperater )
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
			}
		}

		if( NULL != m_pThreadPool->m_pCooperater )
		{
			m_pThreadPool->GetCooperater()->unregisterThread(pThreadInstance);
		}
	}

	void KThreadPool::WorkerProc::SetParentThreadPool(KThreadPool* pThreadPool)
	{
		m_pThreadPool = pThreadPool;
	}

	
} /*Thread*/ } /* System */