#pragma once
#include <System/KType.h>
#include <System/KMacro.h>
#include <System/Sync/KSync.h>
#include <System/Collections/KString.h>

class KTableCache;
class KTableCache_X_Thread;
typedef System::Collections::KString<128> TableTaskName;

class KTableTask;
class KTableTaskOwner
{
public:
	virtual ~KTableTaskOwner() { }
	virtual void on_tableTask_finish(KTableTask* task);
	virtual void on_tableTask_timeout(KTableTask* task);
};

class KTableTask
{
public:
	// 一个TableTask最多可以挂接到多少个TableTaskQueue上
	enum { max_task_queue = 24 };

public:
	KTableTask();
	virtual ~KTableTask();

public:
	virtual bool isIdleTask() const { return false; }

	virtual void execute(KTableCache_X_Thread& theThread) = 0;
	virtual void destroy() { delete this; }

	virtual void on_finish()  { }
	virtual void on_timeout() { }

	virtual TableTaskName toString() const { return TableTaskName("KTableTask"); }

public:
	void notifyFinish();
	void notifyTimeout();

public:
	void setTaskOwner(KTableTaskOwner* taskOwner)
	{
		m_pTaskOwner = taskOwner;
	}
	void want(int_r wantCount)
	{
		m_wantCount = wantCount;
	}

	int_r addRef();
	int_r releaseRef();
	int_r addWant();
	int_r decWant();

public:
	DWORD m_seq;
	uint_r m_errCode;
	int_r m_refCount;
	int_r m_wantCount;
	int_r m_msRemain;
	KTableTaskOwner* m_pTaskOwner;
	System::Sync::KThreadMutex m_mx;
	KTableTask* m_nextTask[max_task_queue];
};
