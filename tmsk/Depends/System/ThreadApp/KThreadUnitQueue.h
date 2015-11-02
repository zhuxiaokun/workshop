#ifndef __KTHREADUNITQUEUE_H__
#define __KTHREADUNITQUEUE_H__
#include "KThreadUnit.h"
#include "../Sync/KSync.h"
namespace System { namespace Thread {

	class KThreadUnitQueue
	{
	public:
		KThreadUnitQueue();
		virtual ~KThreadUnitQueue();
		virtual void Initialize(unsigned int iPriorityCount);
		virtual void ClearAll();
		void Enqueue(KThreadUnitNode* pNode, unsigned int iPriority);
		KThreadUnitNode* Dequeue();
		bool IsEmpty();
		long getCurThreadUnitCount(){return m_iCurThreadUnitCount;}
	protected:
		Collections::KSelfList<KThreadUnitNode>* m_pQueue;
		Sync::KRecursiveMutex*					 m_pQueueMutex;
		unsigned int							 m_uiPriorityCount;
		
		volatile LONG							 m_iCurThreadUnitCount;
	};

} /*Thread*/ } /* System */

#endif