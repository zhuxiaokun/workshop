#pragma once
#include <System/KType.h>
#include <System/KMacro.h>
#include "KTableTask.h"

struct K_nextTableTask
{
	int_r m_nextSlot;
	K_nextTableTask():m_nextSlot(0)
	{

	}
	KTableTask*& nextTask(KTableTask* task)
	{
		return task->m_nextTask[m_nextSlot];
	}
};

class KTableTaskQueue
{
public:
	KTableTaskQueue();
	~KTableTaskQueue();

public:
	void setNextSlot(int_r slot);
	void push_back(KTableTask* task);
	KTableTask* pop_front();

public:
	KTableTask* m_firstTask;
	KTableTask* m_lastTask;
	K_nextTableTask m_nextDirector;
};

class KTableTaskQueue_MT : public KTableTaskQueue
{
public:
	System::Sync::KThreadMutex m_mx;
	void push_back(KTableTask* task)
	{
		System::Sync::KAutoThreadMutex mx(m_mx);
		KTableTaskQueue::push_back(task);
	}
	KTableTask* pop_front()
	{
		System::Sync::KAutoThreadMutex mx(m_mx);
		return KTableTaskQueue::pop_front();
	}
};