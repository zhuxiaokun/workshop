#pragma once
#include <System/Thread/KThread.h>
#include <System/Collections/KQueue.h>
#include <System/Memory/KStepObjectPool.h>
#include <mysqldb/KDatabase.h>

// 数据库操作的上下文
struct KSqlContext
{
	KSQLConnection cnn;
	KSQLCommand cmd;
	KSQLResultSet rs;
};

/**
 * 代表一种类型的数据库操作
 * 如果因为数据库不可用而导致失败，将在数据库重连后重试一次
 * 所以，该对象应该是可以重入的
 */
class KSqlTask
{
public:
	KSqlTask();
	virtual ~KSqlTask();


public:
	// 处理结果时，检查所依赖的条件是否满足, 如发起该操作的Player是否还在线等等
	virtual BOOL checkDepends() = 0;
	virtual BOOL execute(KSqlContext& ctx) = 0;
	virtual BOOL onFinished()=0;

public:
	virtual void reset();
	virtual void destroy();	// 用于内存池对象的回收和复用

public:
	DWORD getLastError() const;
	void setLastError(DWORD errCode);

protected:
	int m_errCode;
};

/// KDatabaseThread ///

class KDatabaseThread : public ::System::Thread::KThread
{
public:
	typedef ::System::Collections::KQueueFifo<KSqlTask*,::System::Sync::KMTLock,256> TaskQueue;
	typedef TaskQueue ResultQueue;

public:
	KDatabaseThread();
	~KDatabaseThread();

public:
	virtual BOOL initialize(const KSQLConnParam& cnnParam, JG_S::KSync_PulseEvent* syncEvent=NULL);
	virtual void finalize();

public:
	void push(KSqlTask& task);
	KSqlTask* pop();

public:
	void Execute();
	void Stop();
	void Clear();

protected:
	BOOL isConnActive();
	BOOL reconnect();

public:
	KSqlContext m_ctx;
	TaskQueue m_taskQueue;
	ResultQueue m_resultQueue;
	volatile BOOL m_stopDesired;
	JG_S::KSync_PulseEvent* m_syncEvent;
};
