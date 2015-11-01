#pragma once
#include "KStorageTask.h"
#include <System/Thread/KThread.h>

class KStorageThread : public System::Thread::KThread
{
public:
	KStorageThread();
	~KStorageThread();

public:
	bool Initialize(KStorageTask::TaskQueue& inQueue, KStorageTask::TaskQueue& outQueue);
	void Execute();
	void Stop();
	void Finalize();

public:
	volatile BOOL m_stopDesired;
	KStorageTask::TaskQueue* m_inQueue;
	KStorageTask::TaskQueue* m_outQueue;
};
