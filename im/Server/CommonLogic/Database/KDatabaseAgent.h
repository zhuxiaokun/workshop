#pragma once
#include <System/Misc/rand.h>
#include "KDatabaseThread.h"

/// KDatabaseAgent

class KDatabaseAgent
{
public:
	KDatabaseAgent();
	~KDatabaseAgent();

public:
	BOOL initialize(const KSQLConnParam& connParam, int threadNum, JG_S::KSync_PulseEvent* syncEvent=NULL);
	void finalize();
	void breathe(int interval);

public:
	void push(KSqlTask& task);


public:
	int m_threadNum;
	KDatabaseThread* m_threads;
	System::Misc::CRandom m_rand;
	JG_S::KSync_PulseEvent* m_syncEvent;
};
