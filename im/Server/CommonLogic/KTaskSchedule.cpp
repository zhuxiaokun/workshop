#include <System/KPlatform.h>
#include "KTaskSchedule.h"

KTaskObject::~KTaskObject()
{

}

void KTaskObject::Destrot_Delete()
{
	delete this;
}

KTaskSchedule::KTaskSchedule()
{

}

KTaskSchedule::~KTaskSchedule()
{

}

BOOL KTaskSchedule::Register(KTaskObject* pTask)
{
	DWORD nextTick = pTask->GetNextActivateTick();
	if(!nextTick)
	{
		pTask->Destroy();
		return FALSE;
	}

	int c = m_taskQueue.size();
	
	for(int i=0; i<c; i++)
	{
		if(m_taskQueue[i] == pTask) return FALSE;
	}

	m_taskQueue.insert(pTask);
	return TRUE;
}

void KTaskSchedule::Breathe(int interval)
{
	m_taskArray.clear();

	DWORD now = GetTickCount();
	int c = m_taskQueue.size();
	int idx = c - 1;

	while(idx >= 0)
	{
		KTaskObject* pTask = m_taskQueue[idx];
		DWORD nextTick = pTask->GetNextActivateTick();
		if(!nextTick)
		{
			m_taskQueue.erase(idx);
			pTask->Destroy();
			idx -= 1;
			continue;
		}

		// 没有到时的任务
		if(now < nextTick) break;

		m_taskQueue.erase(idx);
		pTask->OnActivate();

		nextTick = pTask->GetNextActivateTick();
		if(!nextTick)
		{
			pTask->Destroy();
		}
		else
		{
			m_taskArray.push_back(pTask);
		}

		idx -= 1;
	}

	c = m_taskArray.size();
	for(int i=0; i<c; i++)
	{
		KTaskObject* pTask = m_taskArray[i];
		m_taskQueue.insert(pTask);
	}
	m_taskArray.clear();

	return;
}
