#include "KStorageThread.h"

//volatile BOOL m_stopDesired;
//KStorageTask::TaskQueue* m_inQueue;
//KStorageTask::TaskQueue* m_outQueue;

KStorageThread::KStorageThread()
{
	m_stopDesired = FALSE;
	m_inQueue = NULL;
	m_outQueue = NULL;
}

KStorageThread::~KStorageThread()
{
	this->Finalize();
}

bool KStorageThread::Initialize(KStorageTask::TaskQueue& inQueue, KStorageTask::TaskQueue& outQueue)
{
	m_inQueue = &inQueue;
	m_outQueue = &outQueue;
	this->Run();
	return true;
}

void KStorageThread::Execute()
{
	KStorageTask* task;
	while(!m_stopDesired)
	{	
		if(!m_inQueue->pop(task))
		{
			m_inQueue->wait(50);
			continue;
		}
		task->execute();
		m_outQueue->push(task);
	}
}

void KStorageThread::Stop()
{
	m_stopDesired = TRUE;
	System::Thread::KThread::WaitFor();
}

void KStorageThread::Finalize()
{
	this->Stop();
}
