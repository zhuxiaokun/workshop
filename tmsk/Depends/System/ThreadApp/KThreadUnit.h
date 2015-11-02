#ifndef __KTHREADUNIT_H__
#define __KTHREADUNIT_H__
#include "../Collections/KList.h"
#include "../Sync/KSync.h"
#include "../Memory/KStepObjectPool.h"
namespace System { namespace Thread {

class KThreadUnitNode;

class KIThreadUnit
{
public:
	friend class KThreadUnitNode;
	enum ThreadState
	{
		THREADSTATE_IDLE,
		THREADSTATE_PENDING,
		THREADSTATE_RUNNING,
		THREADSTATE_COMPLETED, // 完成
		THREADSTATE_ABORTED    // 今后取代badResource
	};

	KIThreadUnit();
	virtual ~KIThreadUnit();
	virtual void ThreadExecute() = 0;
	virtual void CompleteInThread();
	virtual void DetachThread();

	KThreadUnitNode* GetNode(){return m_pAttachNode;} 

	virtual void SetThreadState(ThreadState iThreadState);
	ThreadState GetThreadState(){return m_ThreadState;}
	bool IsInThread(){ return (THREADSTATE_PENDING == m_ThreadState || THREADSTATE_RUNNING == m_ThreadState);}
protected:
	KThreadUnitNode* m_pAttachNode;
	
	volatile ThreadState m_ThreadState;
	mutable Sync::KRecursiveMutex m_ThreadStatusMutex;
};

class KThreadUnitNode
{
public:
	KThreadUnitNode();
	~KThreadUnitNode();
	void Initialize(KIThreadUnit* pThreadUnit);
	void DetachThreadUnit();
	void Execute();
	void SetThreadState(KIThreadUnit::ThreadState iThreadState);
	KIThreadUnit* GetThreadUnit() {return m_pThreadUnit;}
public:	
	KThreadUnitNode* m_prevNode;
	KThreadUnitNode* m_nextNode;
protected:	
	KIThreadUnit*	 m_pThreadUnit;
	mutable Sync::KRecursiveMutex	 m_Mutex;
};


typedef  Memory::KStepObjectPool<KThreadUnitNode,64,Sync::KThreadMutex> KThreadUnitNodePool;

} /*Thread*/ } /* System */
#endif