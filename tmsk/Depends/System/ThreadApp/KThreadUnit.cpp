#include "KThreadUnit.h"
namespace System { namespace Thread {

	KIThreadUnit::KIThreadUnit()
		:m_pAttachNode(NULL)
		,m_ThreadState(THREADSTATE_IDLE)
	{
	}

	KIThreadUnit::~KIThreadUnit()
	{
		DetachThread();
	}

	void KIThreadUnit::DetachThread()
	{
		KThreadUnitNode* pAttachNode = NULL;
		
		_KINTERLOCKEDEXCHANGEPOINTER(reinterpret_cast<PVOID *>(&pAttachNode),m_pAttachNode);
		if(NULL != pAttachNode)
		{
			pAttachNode->DetachThreadUnit();
		}

		//m_pAttachNode = NULL;
		_KINTERLOCKEDEXCHANGEPOINTER(reinterpret_cast<PVOID *>(&m_pAttachNode),NULL);
		
		if(IsInThread())
		{
			SetThreadState(THREADSTATE_IDLE);
		}

	}

	 void KIThreadUnit::CompleteInThread()
	 {
		SetThreadState(THREADSTATE_COMPLETED);
	 }

	void KIThreadUnit::SetThreadState(ThreadState iThreadState)
	{
		m_ThreadStatusMutex.Lock();
		m_ThreadState = iThreadState;
		m_ThreadStatusMutex.Unlock();
	}

	KThreadUnitNode::KThreadUnitNode()
		:m_prevNode(NULL)
		,m_nextNode(NULL)
		,m_pThreadUnit(NULL)
	{	
	}

	KThreadUnitNode::~KThreadUnitNode()
	{
		
	}

	void KThreadUnitNode::DetachThreadUnit()
	{
		m_Mutex.Lock();
		m_pThreadUnit = NULL;
		m_Mutex.Unlock();

	}
	void KThreadUnitNode::Initialize(KIThreadUnit* pThreadUnit)
	{
		m_Mutex.Lock();	
		ASSERT(NULL != pThreadUnit && NULL == pThreadUnit->m_pAttachNode)
		m_pThreadUnit = pThreadUnit;
		pThreadUnit->m_pAttachNode = this;
		m_prevNode = NULL;
		m_nextNode = NULL;
		m_Mutex.Unlock();
	}
	void KThreadUnitNode::Execute()
	{
		m_Mutex.Lock();	
		if(NULL != m_pThreadUnit)
		{
			m_pThreadUnit->SetThreadState(KIThreadUnit::THREADSTATE_RUNNING);
			m_pThreadUnit->ThreadExecute();
			//m_pThreadUnit->m_pAttachNode = NULL;
			_KINTERLOCKEDEXCHANGEPOINTER(reinterpret_cast<PVOID *>( &(m_pThreadUnit->m_pAttachNode)),NULL);
			m_pThreadUnit->CompleteInThread();
		}
		m_Mutex.Unlock();

	}
	void KThreadUnitNode::SetThreadState(KIThreadUnit::ThreadState iThreadState)
	{
		m_Mutex.Lock();	
		if(NULL != m_pThreadUnit)
		{
			m_pThreadUnit->SetThreadState(iThreadState);
		}
		m_Mutex.Unlock();
	}

} /*Thread*/ } /* System */