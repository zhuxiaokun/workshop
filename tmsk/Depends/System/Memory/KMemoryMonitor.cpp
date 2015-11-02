#include "KMemoryMonitor.h"
#include "../Collections/KString.h"
#include "../Log/log.h"
#include "KThreadLocalStorage.h"
#ifdef _CHK_MEM_LEAK_

extern int traceback_str(const CallStack& callstack, int n, char* buf, size_t length);

namespace System { namespace Memory {

KMemoryMonitor* g_mmonitor = NULL;

KMemoryRecorder::KMemoryRecorder()
	: m_current(&m_buff1)
	, m_proc(&m_buff2)
	, m_inqueue(FALSE)
{
	KMemoryMonitor::getInstance();
	g_mmonitor->addRecorder(this);
}

KMemoryRecorder::~KMemoryRecorder()
{

}

void KMemoryRecorder::push_alloc(const CallStack& callStack, const void* p, size_t size)
{
	size_t want = sizeof(AllocRec);
	size_t avail = m_current->_avail();
	if(avail < want) if(!this->swap()) return;
	AllocRec& rec = *(AllocRec*)m_current->_end();
	rec.type = enum_alloc_rec; rec.size = size; rec.p = p;
	memcpy(&rec.callStack, &callStack, sizeof(CallStack));
	uint_r& flag = *(uint_r*)&rec;
	m_current->m_size += want;
}

void KMemoryRecorder::push_free(const CallStack& callStack, const void* p)
{
	size_t want = sizeof(FreeRec);
	size_t avail = m_current->_avail();
	if(avail < want) if(!this->swap()) return;
	FreeRec& rec = *(FreeRec*)m_current->_end();
	rec.type = enum_free_rec; rec.size = 0; rec.p = p;
	memcpy(&rec.callStack, &callStack, sizeof(CallStack));
	m_current->m_size += want;
}

void KMemoryRecorder::push_realloc(const CallStack& callStack, const void* p, const void* np, size_t size)
{
	size_t want = sizeof(ReallocRec);
	size_t avail = m_current->_avail();
	if(avail < want) if(!this->swap()) return;
	ReallocRec& rec = *(ReallocRec*)m_current->_end();
	rec.type = enum_realloc_rec; rec.size = size; rec.p = p; rec.pn = np;
	memcpy(&rec.callStack, &callStack, sizeof(CallStack));
	m_current->m_size += want;
}

BOOL KMemoryRecorder::pop(uint_r& pos, AllocRec*& allocRec, FreeRec*& freeRec, ReallocRec*& reallocRec)
{
	return this->_pop(m_proc, pos, allocRec, freeRec, reallocRec);
}

BOOL KMemoryRecorder::_pop(KDataBuffer* buff, uint_r& pos, AllocRec*& allocRec, FreeRec*& freeRec, ReallocRec*& reallocRec)
{
	allocRec = NULL;
	freeRec = NULL;
	reallocRec = NULL;
	if(pos >= buff->m_size) return FALSE;
	uint_r& flag = *(uint_r*)buff->_at(pos);

	uint_r t = flag & 3;
	switch(t)
	{
	case enum_alloc_rec:
		ASSERT(pos+sizeof(AllocRec) <= buff->m_size);
		allocRec = (AllocRec*)&flag;
		pos += sizeof(AllocRec);
		break;
	case enum_free_rec:
		ASSERT(pos+sizeof(FreeRec) <= buff->m_size);
		freeRec = (FreeRec*)&flag;
		pos += sizeof(FreeRec);
		break;
	case enum_realloc_rec:
		ASSERT(pos+sizeof(ReallocRec) <= buff->m_size);
		reallocRec = (ReallocRec*)&flag;
		pos += sizeof(ReallocRec);
		break;
	}

	return TRUE;
}

BOOL KMemoryRecorder::swap()
{
	if(m_inqueue)
	{
		if(m_event.Block(50)) return FALSE;
		KDataBuffer* p = m_current;
		m_current = m_proc; m_proc = p;
		m_current->clear();
		g_mmonitor->push(this);
		return TRUE;
	}
	else
	{
		m_event.Block(0);
		KDataBuffer* p = m_current;
		m_current = m_proc; m_proc = p;
		m_current->clear();
		g_mmonitor->push(this);
		return TRUE;
	}
}

void KMemoryRecorder::reset()
{
	m_current->clear();
	m_proc->clear();
	m_inqueue = FALSE;
	m_event.Set();
}

////
KMemoryMonitor::KMemoryMonitor():m_tls(0),m_threadID(0),m_stopDesired(FALSE)
{
	m_tls = KThreadLocalStorage::GetInstance().AllocStorage();
	m_recorders.attach(m_buff, 1024, 0);
	//this->Run();
}

KMemoryMonitor::~KMemoryMonitor()
{
	this->stop();
}

KMemoryMonitor* KMemoryMonitor::getInstance()
{
	volatile static BOOL binit = FALSE;
	static ::System::Sync::KThreadMutex mx;
	if(g_mmonitor) return g_mmonitor;
	::System::Sync::KAutoThreadMutex amx(mx);
	if(g_mmonitor) return g_mmonitor;
	g_mmonitor = new KMemoryMonitor();
	return g_mmonitor;
}

void KMemoryMonitor::Execute()
{
	m_threadID = getCurrentThreadId();

	m_queue.clear();
	size_t n = m_recorders.size();
	for(int i=0; i<n; i++) m_recorders[i]->reset();

	KMemoryRecorder* recorder;
	while(!m_stopDesired)
	{
		recorder = m_queue.pop_front();
		if(!recorder) { m_event.Block(10); continue; }
		this->handle_recorder_buff(recorder, recorder->m_proc);
		recorder->m_inqueue = FALSE;
		recorder->m_event.Set();
	}
	{	
		while(recorder=m_queue.pop_front(), recorder)
		{
			this->handle_recorder_buff(recorder, recorder->m_proc);
			recorder->m_inqueue = FALSE;
			recorder->m_event.Set();
		}
	}
	n = m_recorders.size();
	for(int i=0; i<n; i++)
	{
		recorder = m_recorders[i];
		KDataBuffer* buff = recorder->m_current;
		this->handle_recorder_buff(recorder, buff);
		recorder->reset();
	}

	this->dumpResult();
}

void KMemoryMonitor::start()
{
	if(this->GetThreadId()) return;
	m_stopDesired = FALSE;
	this->Run();
}

void KMemoryMonitor::stop()
{
	m_stopDesired = TRUE;
	this->WaitFor();
	m_threadID = 0;
}

void KMemoryMonitor::push(KMemoryRecorder* recorder)
{
	if(m_stopDesired) return;
	if(this->GetThreadId())
	{
		recorder->m_inqueue = TRUE;
		m_queue.push_back(recorder);
		m_event.Set();
	}
}

void KMemoryMonitor::handle_allocRec(const CallStack& callStack, const void* p, size_t size)
{
	mu u;
	u.size = size;
	memcpy(&u.callStack, &callStack, sizeof(CallStack));
	m_allocMap[p] = u;
}

void KMemoryMonitor::handle_freeRec(const CallStack& callStack, const void* p)
{
	ResultMap::iterator it = m_allocMap.find(p);
	if(it != m_allocMap.end())
	{
		m_allocMap.erase(p);
	}
	else
	{
		mu u;
		u.size = 0;
		memcpy(&u.callStack, &callStack, sizeof(CallStack));
		m_freeMap[p] = u;
	}
}

void KMemoryMonitor::handle_reallocRec(const CallStack& callStack, const void* p, const void* pn, size_t size)
{
	if(!size) pn = NULL;
	if(p) this->handle_freeRec(callStack, p);
	if(pn) this->handle_allocRec(callStack, pn, size);
}

void KMemoryMonitor::dumpResult()
{
	char buf[2048];
	if(!m_allocMap.empty())
	{
		ResultMap::iterator it = m_allocMap.begin();
		ResultMap::iterator ite = m_allocMap.end();
		while(it != ite)
		{
			const void* p = it->first;
			mu& u = it->second;
			size_t size = u.size; it++;
			traceback_str(u.callStack, sizeof(u.callStack)/sizeof(uint_r), buf, sizeof(buf));
			Log(LOG_WARN, "memory: malloc, size:%u ptr:%08p, %s", (int)size, p, buf);
		}
		m_allocMap.clear();
	}
	if(!m_freeMap.empty())
	{
		ResultMap::iterator it = m_freeMap.begin();
		ResultMap::iterator ite = m_freeMap.end();
		while(it != ite)
		{
			const void* p = it->first;
			mu& u = it->second;
			traceback_str(u.callStack, sizeof(u.callStack)/sizeof(uint_r), buf, sizeof(buf));
			Log(LOG_WARN, "memory: free, ptr:%08p, %s", p, buf);
			it++;
		}
		m_freeMap.clear();
	}
}

KMemoryRecorder* KMemoryMonitor::getThreadRecorder()
{
	uint_r tid = getCurrentThreadId();
	if(!m_threadID || tid == m_threadID) return NULL;

	void* p = NULL;
	KThreadLocalStorage& tls = KThreadLocalStorage::GetInstance();
	if(tls.GetValue(m_tls, p) && p)
		return (KMemoryRecorder*)p;
	
	p = malloc(sizeof(KMemoryRecorder));
	ASSERT(p);
	new (p) KMemoryRecorder();
	tls.SetValue(m_tls, p);
	return (KMemoryRecorder*)p;
}

void KMemoryMonitor::addRecorder(KMemoryRecorder* recorder)
{
	::System::Sync::KAutoThreadMutex mx(m_mx);
	if(m_recorders.size() < 1024)
		m_recorders.push_back(recorder);
}

void KMemoryMonitor::handle_recorder_buff(KMemoryRecorder* recorder, KDataBuffer* buff)
{
	uint_r pos = 0;
	KMemoryRecorder::FreeRec* freeRec;
	KMemoryRecorder::AllocRec* allocRec;
	KMemoryRecorder::ReallocRec* reallocRec;
	while(recorder->_pop(buff, pos, allocRec, freeRec, reallocRec))
	{
		if(allocRec)
		{
			this->handle_allocRec(allocRec->callStack, allocRec->p, allocRec->size);
		}
		else if(freeRec)
		{
			this->handle_freeRec(freeRec->callStack, freeRec->p);
		}
		else if(reallocRec)
		{
			this->handle_reallocRec(reallocRec->callStack, reallocRec->p, reallocRec->pn, reallocRec->size);
		}
	}
	buff->clear();
}

}};
#endif