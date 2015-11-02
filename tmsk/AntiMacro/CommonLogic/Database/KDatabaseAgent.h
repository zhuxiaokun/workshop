#ifndef _K_DATABASE_AGENT_H_
#define _K_DATABASE_AGENT_H_

#include <System/Thread/KThread.h>
#include <System/Collections/KQueue.h>
#include <System/Memory/KStepObjectPool.h>
#include <mysqldb/KDatabase.h>

// ���ݿ������������
struct KSqlContext
{
	KSQLConnection cnn;
	KSQLCommand cmd;
	KSQLResultSet rs;
};

/**
 * ����һ�����͵����ݿ����
 * �����Ϊ���ݿⲻ���ö�����ʧ�ܣ��������ݿ�����������һ��
 * ���ԣ��ö���Ӧ���ǿ��������
 */
class KSqlTask
{
public:
	KSqlTask();
	KSqlTask(int taskKind);
	virtual ~KSqlTask();

public:
	int getTaskKind() const { return m_taskKind; }

public:
	// ������ʱ������������������Ƿ�����, �緢��ò�����Player�Ƿ����ߵȵ�
	virtual BOOL checkDepends() = 0;
	virtual BOOL execute(KSqlContext& ctx) = 0;

public:
	virtual void reset();
	virtual void destroy();	// �����ڴ�ض���Ļ��պ͸���

public:
	DWORD getLastError() const;
	void setLastError(DWORD errCode);

protected:
	int m_errCode;
	int m_taskKind;
};

/// KDatabaseAgent ///

class KDatabaseAgent : public ::System::Thread::KThread
{
public:
	typedef ::System::Collections::KQueueFifo<KSqlTask*,::System::Sync::KMTLock,256> TaskQueue;
	typedef TaskQueue ResultQueue;

public:
	KDatabaseAgent();
	~KDatabaseAgent();

public:
	virtual BOOL initialize(const KSQLConnParam& cnnParam);
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
};

#endif
