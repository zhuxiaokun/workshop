#include "KTPObjPool.h"

namespace System { namespace Thread {
	
	KTPObjPool::KTPObjPool(KThreadProcessor* pThreadProcessor)
		:m_pThreadProcessor(pThreadProcessor)
		, m_pThreadUnitNodePool(NULL)
		,m_pWorkflowPool(NULL)
		,m_pTaskPool(NULL)
		,m_pJobListPool(NULL)
		,m_pStreamPool(NULL)
		,m_uFlags(0)
		
	{

	}
	KTPObjPool::~KTPObjPool()
	{
		Clear();
	}

	void KTPObjPool::Clear()
	{
		if(!GetBit(ATTACH_UNITNODEPOOL) && NULL != m_pThreadUnitNodePool)
		{
			m_pThreadUnitNodePool->Destroy();
			delete m_pThreadUnitNodePool;
		}
		m_pThreadUnitNodePool = NULL;

		if(!GetBit(ATTACH_WORKFLOWPOOL) && NULL != m_pWorkflowPool)
		{
			m_pWorkflowPool->Destroy();
			delete m_pWorkflowPool;
		}
		m_pWorkflowPool = NULL;

		if(!GetBit(ATTACH_TASKPOOL) && NULL != m_pTaskPool)
		{
			m_pTaskPool->Destroy();
			delete m_pTaskPool;
		}
		m_pTaskPool = NULL;

		if(!GetBit(ATTACH_JOBLISTPOOL) && NULL != m_pJobListPool)
		{
			m_pJobListPool->Destroy();
			delete m_pJobListPool;
		}
		m_pJobListPool = NULL;

		if(!GetBit(ATTACH_STREAMPOOL) && NULL != m_pStreamPool)
		{
			m_pStreamPool->Destroy();
			delete m_pStreamPool;
		}
		m_pStreamPool = NULL;
	}

	void KTPObjPool::Initialize(KThreadUnitNodePool* pThreadUnitNodePool,KTPWorkflowPool* pWorkflowPool,
		KTPTaskPool* pTaskPool,KTPStreamPool* pStreamPool,KTPJobListPool* pJobListPool)
	{
		if(NULL == pThreadUnitNodePool)
		{
			m_pThreadUnitNodePool = new KThreadUnitNodePool;
		}
		else
		{
			m_pThreadUnitNodePool = pThreadUnitNodePool;
			SetBit(true,ATTACH_UNITNODEPOOL);
		}

		if(NULL == pWorkflowPool)
		{
			m_pWorkflowPool = new KTPWorkflowPool;
		}
		else
		{
			m_pWorkflowPool = pWorkflowPool;
			SetBit(true,ATTACH_WORKFLOWPOOL);
		}

		if(NULL == pTaskPool)
		{
			m_pTaskPool = new KTPTaskPool;
		}
		else
		{
			m_pTaskPool = pTaskPool;
			SetBit(true,ATTACH_TASKPOOL);
		}
		
		if(NULL == pStreamPool)
		{
			m_pStreamPool = new KTPStreamPool;
		}
		else
		{
			m_pStreamPool = pStreamPool;
			SetBit(true,ATTACH_STREAMPOOL);
		}

		if(NULL == pJobListPool)
		{
			m_pJobListPool = new KTPJobListPool;
		}
		else
		{
			m_pJobListPool = pJobListPool;
			SetBit(true,ATTACH_JOBLISTPOOL);
		}
		
		
	}

} /*Thread*/ } /* System */