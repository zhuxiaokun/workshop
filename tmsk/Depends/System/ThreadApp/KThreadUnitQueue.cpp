#include "KThreadUnitQueue.h"
namespace System { namespace Thread {

	KThreadUnitQueue::KThreadUnitQueue()
		:m_pQueue(NULL)
		,m_pQueueMutex(NULL)
		,m_uiPriorityCount(0)
		,m_iCurThreadUnitCount(0)
	{
	}
	
	KThreadUnitQueue::~KThreadUnitQueue()
	{
	}

	void KThreadUnitQueue::Initialize(unsigned int iPriorityCount)
	{
		ASSERT(NULL == m_pQueue && NULL == m_pQueueMutex && 0<iPriorityCount);
		m_pQueue = new Collections::KSelfList<KThreadUnitNode>[iPriorityCount];
		m_pQueueMutex = new Sync::KRecursiveMutex[iPriorityCount];
		m_uiPriorityCount = iPriorityCount;
	}

	void KThreadUnitQueue::ClearAll()
	{
		if(NULL != m_pQueue)
		{
			delete[] m_pQueue;
			m_pQueue = NULL;
		}

		if(NULL != m_pQueueMutex)
		{
			delete[] m_pQueueMutex;
			m_pQueueMutex = NULL;
		}

		m_uiPriorityCount = 0;
		m_iCurThreadUnitCount = 0;
	}

	void KThreadUnitQueue::Enqueue(KThreadUnitNode* pNode, unsigned int iPriority)
	{
		ASSERT(iPriority<m_uiPriorityCount && NULL != pNode);
		m_pQueueMutex[iPriority].Lock();
		m_pQueue[iPriority].push_back(pNode);
		m_pQueueMutex[iPriority].Unlock();

		_KINTERLOCKEDINCREMENT(&m_iCurThreadUnitCount);
	}
	
	KThreadUnitNode* KThreadUnitQueue::Dequeue()
	{
		unsigned int iRetPriority = 0;
		if(m_uiPriorityCount<=0)
		{
			return NULL;
		}

		KThreadUnitNode* pRetNode = NULL;
		for(int i = (int)(m_uiPriorityCount-1);i>=0;--i)
		{
			m_pQueueMutex[i].Lock();
			pRetNode = m_pQueue[i].pop_front();
			m_pQueueMutex[i].Unlock();
			if(NULL != pRetNode)
			{
				_KINTERLOCKEDDECREMENT(&m_iCurThreadUnitCount);
				break;
			}
		}
		return pRetNode;
	}

	bool KThreadUnitQueue::IsEmpty()
	{
		return (0 == m_iCurThreadUnitCount);
	}

} /*Thread*/ } /* System */