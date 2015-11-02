#pragma once
#include <System/Collections/KSDList.h>
#include <System/Thread/KThread.h>
#include "KTableTask.h"
#include "KTableRecordData.h"
#include "KTableCache_X.h"
#include "KTableTaskQueue.h"

class KTableCache_X_Thread : public System::Thread::KThread
{
private:
	KTableCache_X_Thread();
	KTableCache_X_Thread& operator=(const KTableCache_X_Thread& o);

public:
	KTableCache_X_Thread(KTableCache_X& cache);
	~KTableCache_X_Thread();

public:
	void Execute();
	void Stop();
	virtual void onIdle() { }

public: // 提供一个缺省的释放策略
	virtual int_r assureCapacity(int_r count);

public:
	int incIdleCounter(int maxCounter);
	int decIdleCounter();
	int idleCounter() const;

	void push(KTableTask* task);
	KTableTask* pop();

public:
	void setNextTaskSlot(int_r nextTaskSlot) { m_taskQueue.setNextSlot(nextTaskSlot); }

protected:
	int_r _assureCapacity(int_r secs, int_r count);

public:
	int m_idleCounter; // idle 计数器
	int_r m_flushSecs;
	KTableTaskQueue m_taskQueue;
	volatile BOOL m_stopDesired;
	KTableCache_X* m_tableCache;
	KTableRecordData m_recData;
	System::Sync::KThreadMutex m_mx;
	System::Sync::KSync_PulseEvent m_event;
};
