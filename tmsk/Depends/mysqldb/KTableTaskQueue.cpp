#include "KTableTaskQueue.h"

KTableTaskQueue::KTableTaskQueue()
	: m_firstTask(NULL)
	, m_lastTask(NULL)
{

}

KTableTaskQueue::~KTableTaskQueue()
{

}

void KTableTaskQueue::setNextSlot(int_r slot)
{
	ASSERT(slot >= 0 && slot < KTableTask::max_task_queue);
	m_nextDirector.m_nextSlot = slot;
}

void KTableTaskQueue::push_back(KTableTask* task)
{
	m_nextDirector.nextTask(task) = NULL;
	if(m_lastTask)
	{
		m_nextDirector.nextTask(m_lastTask) = task;
		m_lastTask = task;
	}
	else
	{
		m_firstTask = m_lastTask = task;
	}
}

KTableTask* KTableTaskQueue::pop_front()
{
	if(m_firstTask)
	{
		KTableTask* task = m_firstTask;
		m_firstTask = m_nextDirector.nextTask(m_firstTask);
		if(!m_firstTask) m_lastTask = NULL;
		m_nextDirector.nextTask(task) = NULL;
		return task;
	}
	return NULL;
}