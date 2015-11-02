#pragma once
#include "KTableCacheReader.h"
#include <System/Thread/KThread.h>
#include <System/Sync/KSync.h>
#include <System/Log/log.h>
#include <System/Collections/KQueue.h>
#include "KVarBinary.h"

/**
* 紧急处理记录被处理后，Oberver将会得到通知
*/
struct KEmergencyEvent
{
	HANDLE hRec;
	void* pUserData;
};
class IEmergencyObserver
{
public:
	virtual ~IEmergencyObserver() { }
	virtual BOOL OnEmergencyFinished(BOOL success, void* pUserData) = 0;
};

class KCacheObject;
class KTableCache : public KTableCacheReader
{
public:
	friend class KCacheObject;
	friend class KCacheRefreshThread;
	friend class KCacheManager;
	class KRefreshThread : public System::Thread::KThread
	{
	public:
		KRefreshThread(KTableCache& cache);
		~KRefreshThread();

	public:
		void Execute();  // 线程函数
		void Stop();

	public:
		KTableCache& m_cache;
		int m_refreshUpdateInterval;
		int m_refreshDeleteInterval;
		int m_refreshCheckoutInterval;
		TIME_T m_lastRefreshModify;
		TIME_T m_lastRefreshCheckOut;
		TIME_T m_lastRefreshDelete;
		volatile BOOL m_stopDesired;
	};

public:
	KTableCache();
	virtual ~KTableCache();

public: // 创建接口
	static KTableCache* CreateInstance(KSQLTableSchema& schema, const KSQLConnParam& cnnParam, const CreateParam& createParam);
	
public:	// 对外接口
	void SetEmergencyObserver(IEmergencyObserver* pObserver);
	BOOL Initialize(const KSQLConnParam& cnnParam, KSQLTableSchema& schema, const CreateParam& createParam);
	void Finalize();

public:
	void finalize();

public: // 内部使用
	void _markAllPartialModified(HANDLE hRec);
	int _getPartialModifiedCount(HANDLE hRec);

public:
	/**
	* 参数中带上主键数据类型的长度是为了安全上的考虑
	* 可以在模板派生类中进行长度检查，以尽早发现使用上的错误
	*/
	HANDLE FindRecord(const void* pKey, int len);
	HANDLE LoadRecord(const void* pKey, int len);
	HANDLE CreateRecord(const void* pKey, int len);
	BOOL DiscardRecord(const void* pKey, int len);
	BOOL DeleteRecord(const void* pKey, int len);
	BOOL CheckoutRecord(const void* pKey, int len);
	BOOL EmergencyCheckout(const void* pKey, int len, void* pUserData);

	BOOL EmergencyCheckout(HANDLE hRec, void* pUserData);
	BOOL DiscardRecord2(HANDLE hRec);
	BOOL DeleteRecord2(HANDLE hRec);
	BOOL CheckoutRecord2(HANDLE hRec);

	// 自动生成主键(identity字段)
	HANDLE CreateRecordDirect();

	// 以pKey为主键创建记录
	HANDLE CreateRecordDirect(void* pKey, int len);
	
public:
	BOOL _IsValidHandle(HANDLE hRec);

public:
	virtual BOOL on_recordLoaded(HANDLE h) = 0;
	virtual BOOL on_unloadRecord(HANDLE h) = 0;

public: // 以下部分内部使用 // 检查PKEY::key_type的长度
	virtual BOOL _checkKeyLength(int len) const = 0;

	virtual HANDLE _findRecord(const void* pKey) = 0;
	virtual HANDLE findRecord(KTableRecordData& recData) = 0;

	virtual BOOL SetPrimaryKey(KCacheObject& cache, const void* pKey) = 0;
	virtual BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache) = 0;

	virtual System::Collections::KString<512> Key2String(const void* pKey) const = 0;
	virtual System::Collections::KString<512> GetKeyString(HANDLE hRec) const = 0;
	virtual System::Collections::KString<512> GetKeyString(KTableRecordData& recData) const = 0;
	virtual BOOL GetPrimaryKey(KCacheObject& cache, void* pKey) = 0;
	virtual BOOL IsPrimaryKey(KCacheObject& cache, const void* pKey) = 0;
	virtual BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key) = 0;

protected:
	// 数据库操作，加载一条记录到内存中
	BOOL _LoadRecord(HANDLE hRec);

	// 数据库操作，插入一条记录
	BOOL _InsertRecord(HANDLE hRec, BOOL withKey);

	// 数据库操作，以内存内容更新数据库中的记录
	BOOL _UpdateRecord(HANDLE hRec);

	// 数据库操作，以内存内容更新数据库中的记录，仅更新标记为修改了的字段
	BOOL _PartialUpdateRecord(HANDLE hRec);

	// 数据库操作，construction数据库中删除记录
	BOOL _DeleteRecord(HANDLE hRec);

protected:
	BOOL _fillDefaults(HANDLE hRec);

public: // 同步线程使用
	BOOL RealInsertRecord(HANDLE hRec);
	BOOL RealUpdateRecord(HANDLE hRec);
	BOOL RealPartialUpdateRecord(HANDLE hRec);
	BOOL RealDeleteRecord(HANDLE hRec);
	BOOL RealCheckoutRecord(HANDLE hRec);

public:
	void ClearSqlError();
	DWORD GetLastSqlError() const;
	const char* GetLastSqlErrorMessage() const;

public:
	BOOL isConnActive();
	BOOL reconnect();
	BOOL keepConnectionFresh();

protected:
	// 从紧急队列中取出一个
	BOOL QueuePush(KEmergencyEvent& event);
	BOOL QueuePop(KEmergencyEvent& event);
	BOOL QueueWait(int ms);
	void OnEmergencyFinished(BOOL success, void* pUserData);

private:
	BOOL _CreateInsertSql();
	BOOL _CreateUpdateSql();
	int  _CreatePartialUpdateSQL(HANDLE hRec); // 返回参数个数
	BOOL _CreateDeleteSql();
	BOOL _CreateSelectSql();

private:
	HANDLE  _AllocRecord();
	HANDLE  _AllocFromBottom();
	HANDLE  _AllocFromFree();
	void    _FreeRecord(HANDLE hRec);
	void	_initRecord(KRecordHead* pHead);

private:
	KSQLCommand m_sqlCmd;
	KSQLResultSet m_sqlRs;
	KSQLConnection m_sqlCnn;
	System::Collections::KDString<256> m_sqlInsert;
	System::Collections::KDString<256> m_sqlUpdate;
	System::Collections::KDString<256> m_sqlPartialUpdate;
	System::Collections::KDString<256> m_sqlDelete;
	System::Collections::KDString<256> m_sqlSelect;
	KSQLFieldDescribe* m_sqlParams;

	System::Sync::KThreadMutex m_mx;
	KRefreshThread m_refreshThread;

	// 及时处理队列
	System::Collections::KQueueFifo<KEmergencyEvent,System::Sync::KMTLock,512> m_emergencyQueue;
	IEmergencyObserver* m_pEmergencyObserver;
};