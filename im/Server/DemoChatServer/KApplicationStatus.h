#pragma once
#include <System/File/KMMap.h>
#include <JgMessageDefine.h>

struct ApplicationData
{
	int yyyy_mm_dd;
	time_t expireTime;
	unsigned int daySequence;
};

class KApplicationStatus
{
public:
	KApplicationStatus();
	~KApplicationStatus();

public:
	bool initialize(const char* dataDir);
	void breathe(int ms);
	void finalize();
	JgMessageID lastMessageID();
	JgMessageID nextMessageID();

public:
	KMMap m_mmap;
	ApplicationData* m_appData;
};
