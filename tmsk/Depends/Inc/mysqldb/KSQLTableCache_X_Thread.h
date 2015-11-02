#pragma once
#include "KDatabase.h"
#include "KTableCache_X_Thread.h"
#include "KTableCacheReader.h"

class KSQLTableCache_X_Thread : public KTableCache_X_Thread
{
public:
	typedef KTableCacheReader::KRecordHead KRecordHead;

private:
	KSQLTableCache_X_Thread();
	KSQLTableCache_X_Thread(const KSQLTableCache_X_Thread&);

public:
	KSQLTableCache_X_Thread(KTableCache_X& tableCache, const KSQLConnParam& connParam);
	~KSQLTableCache_X_Thread();

public:
	void Execute();

public:
	BOOL _CreateSelectSql();
	BOOL _CreateUpdateSql();
	int_r _CreatePartialUpdateSQL(HANDLE hRec); // 返回参数个数
	BOOL _CreateInsertSql();
	BOOL _CreateDeleteSql();
	BOOL _assureConnection(int_r msTimeout); // 超时时间

public:
	BOOL load(HANDLE h, KTableRecordData& pkey);
	BOOL realCreate(HANDLE h, KTableRecordData& pkey);
	BOOL realInsert(HANDLE h);
	BOOL realDelete(HANDLE h);
	BOOL realUpdate(HANDLE h);
	BOOL realPartialUpdate(HANDLE h);

private:
	BOOL _insert(HANDLE h);
	BOOL _delete(HANDLE h);
	BOOL _update(HANDLE h);
	BOOL _partialUpdate(HANDLE h);

public:
	KSQLConnParam m_connParam;
	KSQLCommand m_sqlCmd;
	KSQLResultSet m_sqlRs;
	KSQLConnection m_sqlCnn;
	KSQLFieldDescribe* m_sqlParams;
	volatile BOOL m_cnnReady;
	time_t m_lastTryConnect; // 上次尝试连接的时间
	System::Collections::KDString<256> m_sqlInsert;
	System::Collections::KDString<256> m_sqlUpdate;
	System::Collections::KDString<256> m_sqlPartialUpdate;
	System::Collections::KDString<256> m_sqlDelete;
	System::Collections::KDString<256> m_sqlSelect;
};