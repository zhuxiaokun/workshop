#include "KTableCache_X_Thread.h"
#include "KTableCache.h"
using namespace System::Sync;

KTableCache_X_Thread::KTableCache_X_Thread(KTableCache_X& cache)
	: m_idleCounter(0)
	, m_flushSecs(cache.m_flushSecs)
	, m_stopDesired(FALSE)
	, m_tableCache(&cache)
{
	size_t len = KTableRecordData::calcCapacity(*cache.m_tableSchema);
	m_recData.resize(len);
}

KTableCache_X_Thread::~KTableCache_X_Thread()
{
	this->Stop();
}

void KTableCache_X_Thread::Execute()
{
	while(!m_stopDesired)
	{
		KTableTask* task = this->pop();
		if(!task)
		{
			this->incIdleCounter(100);
			this->onIdle();
			m_event.Block(200);
			continue;
		}
		
		//Log(LOG_DEBUG, "execute: %s %s", m_tableCache->GetTableName(), task->toString().c_str());

		if(!task->isIdleTask())
			this->decIdleCounter();

		time_t beginTime = time(NULL);
		task->execute(*this);
		time_t endTime = time(NULL);

		if(endTime - beginTime > 20)
			Log(LOG_WARN, "warn: %s execute takes %d seconds.", task->toString().c_str(), endTime-beginTime);

		if(!task->decWant())
			task->notifyFinish();

		if(!task->releaseRef())
			task->destroy();
	}
}

void KTableCache_X_Thread::Stop()
{
	m_stopDesired = TRUE;
	System::Thread::KThread::WaitFor();
	m_stopDesired = FALSE;
}

int_r KTableCache_X_Thread::assureCapacity(int_r count)
{
	// 2小时
	int_r n = this->_assureCapacity(2*60*60, count);
	if(n) return n;

	// 1小时
	n = this->_assureCapacity(60*60, count);
	if(n) return n;

	// 15分钟
	n = this->_assureCapacity(15*60, count);
	if(n) return n;

	// 不管时间了
	return this->_assureCapacity(0, count);
}

int KTableCache_X_Thread::incIdleCounter(int maxCounter)
{
	m_idleCounter = kmin(maxCounter, m_idleCounter+1);
	return m_idleCounter;
}

int KTableCache_X_Thread::decIdleCounter()
{
	m_idleCounter = kmax(0, m_idleCounter-1);
	return m_idleCounter;
}

int KTableCache_X_Thread::idleCounter() const
{
	return m_idleCounter;
}

void KTableCache_X_Thread::push(KTableTask* task)
{
	//Log(LOG_WARN, "push table:%s %s", m_tableCache->GetTableName(), task->toString().c_str());
	task->addRef();
	KAutoThreadMutex mx(m_mx);
	m_taskQueue.push_back(task);
	m_event.Set();
}

KTableTask* KTableCache_X_Thread::pop()
{
	KAutoThreadMutex mx(m_mx);
	return m_taskQueue.pop_front();
}

int_r KTableCache_X_Thread::_assureCapacity(int_r secs, int_r capacity)
{
	time_t nowTime = time(NULL);
	KTableCacheReader::KFileHead* fileHead = m_tableCache->m_pFileHead;

	int_r count = 0;
	int_r n = fileHead->usedCount;
	for(int_r i=0; i<n && count<capacity; i++)
	{
		HANDLE h = (HANDLE)i;
		KTableCacheReader::KRecordHead* head = m_tableCache->_GetRecordHead(i);
		if(head->IsActive()) continue;

		if(head->IsFree())
		{
			count++; continue;
		}

		if(head->IsStatus(KTableCacheReader::c_new_created)) continue;
		if(head->IsStatus(KTableCacheReader::c_partial_modified)) continue;
		if(head->IsStatus(KTableCacheReader::c_modified)) continue;
		if(head->IsStatus(KTableCacheReader::c_checkout)) continue;
		if(head->IsStatus(KTableCacheReader::c_deleted)) continue;
		if(head->IsStatus(KTableCacheReader::c_new_record)) continue;

		// 满足条件，可以释放
		if(nowTime - head->m_used.flushTime > secs)
		{
			m_tableCache->freeRecord(h);
			count++;
		}
	}
	return count;
}