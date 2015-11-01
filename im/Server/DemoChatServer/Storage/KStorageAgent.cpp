#include "KStorageAgent.h"

KStorageAgent::KStorageAgent()
{
	m_threadCount = 0;
	m_threads = NULL;
}

KStorageAgent::~KStorageAgent()
{
	this->Finalize();
}

bool KStorageAgent::Initialize(const char* dataDir, int threadCount)
{
	m_dataDir = dataDir;
	m_threadCount = kmax(1, kmin(threadCount, 5));
	m_threads = new KStorageThread[m_threadCount];
	for(int i=0; i<m_threadCount; i++)
	{
		KStorageThread& thd = m_threads[i];
		thd.Initialize(m_inQueue, m_outQueue);
	}
	return true;
}

void KStorageAgent::Breathe(int ms)
{
	int i = 0;
	KStorageTask* task;
	while(i++ < 128 && m_outQueue.pop(task))
	{
		task->onFinish();
		task->destroy();
	}
}

void KStorageAgent::Finalize()
{
	if(!m_threads) return;
	for(int i=0; i<m_threadCount; i++)
	{
		KStorageThread& thd = m_threads[i];
		thd.Finalize();
	}
	delete m_threads;
	m_threads = NULL;
}

void KStorageAgent::Push(KStorageTask* task)
{
	m_inQueue.push(task);
}
