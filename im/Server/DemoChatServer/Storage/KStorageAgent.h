#pragma once
#include "KStorageThread.h"
#include <KCommonStruct.h>

class KStorageAgent
{
public:
	KStorageAgent();
	~KStorageAgent();

public:
	bool Initialize(const char* dataDir, int threadCount);
	void Breathe(int ms);
	void Finalize();

public:
	void Push(KStorageTask* task);
	
public:
	string_512 m_dataDir;
	int m_threadCount;
	KStorageThread* m_threads;
	KStorageTask::TaskQueue m_inQueue;
	KStorageTask::TaskQueue m_outQueue;
};
