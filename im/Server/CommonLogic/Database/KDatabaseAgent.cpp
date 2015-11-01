#include "KDatabaseAgent.h"
/// KDatabaseAgent

KDatabaseAgent::KDatabaseAgent():m_threadNum(0),m_threads(NULL),m_syncEvent(NULL)
{

}

KDatabaseAgent::~KDatabaseAgent()
{
	this->finalize();
}

BOOL KDatabaseAgent::initialize(const KSQLConnParam& connParam, int threadNum, JG_S::KSync_PulseEvent* syncEvent)
{
	ASSERT(!m_threads);

	m_syncEvent = syncEvent;
	m_threadNum = threadNum;
	m_threads = new KDatabaseThread[m_threadNum];

	for(int i=0; i<m_threadNum; i++)
	{
		KDatabaseThread& t = m_threads[i];
		if(!t.initialize(connParam, syncEvent)) return FALSE;
	}

	return TRUE;
}

void KDatabaseAgent::finalize()
{
	if(m_threads)
	{
		for(int i=0; i<m_threadNum; i++)
		{
			KDatabaseThread& t = m_threads[i];
			t.finalize();
		}
		delete [] m_threads;
		m_threadNum = 0;
		m_threads = NULL;
	}
}

void KDatabaseAgent::breathe(int interval)
{
	KSqlTask* task;
	for(int i=0; i<m_threadNum; i++)
	{
		KDatabaseThread& t = m_threads[i];
		while(task=t.pop(), task)
		{
			task->onFinished();
			task->destroy();
		}
	}
}

void KDatabaseAgent::push(KSqlTask& task)
{
	int idx = m_rand.GetRandom(0, m_threadNum);
	KDatabaseThread& t = m_threads[idx];
	t.push(task);
}
