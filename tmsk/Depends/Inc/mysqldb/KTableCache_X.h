#pragma once

#include "KTableCacheReader.h"
#include "KTableRecordData.h"

class KCacheObject;
class KTableCache_X_Thread;

/**
 * DBAgent使用的TableCache
 * 每个实例有KTableCacheThread配合
 * 所有实际的操作都是由Thread完成的
 */
class KTableCache_X : public KTableCacheReader
{
public:
	KTableCache_X();
	~KTableCache_X();

public:
	BOOL initialize(KSQLTableSchema& schema, const CreateParam& createParam);
	void finalize();

public:
	HANDLE findRecord(const void* pkey, size_t len);
	HANDLE allocRecord();
	HANDLE allocRecord(KTableRecordData& recordData);
	void freeRecord(HANDLE h);

public:
	// 拷贝主键数据到 h
	BOOL copyKey(HANDLE h, KTableRecordData& recordData);

	// 从KTableRecordData读数据并填充
	BOOL copy(HANDLE h, KTableRecordData& recordData);
	BOOL copyData(HANDLE h, KTableRecordData& recordData);
	
	// 把数据拷贝到KTableRecordData
	BOOL dump(HANDLE h, KTableRecordData& recordData);
	BOOL dump(HANDLE h, int_r cols[], int_r count, KTableRecordData& recordData);
	BOOL dumpData(HANDLE hSrc, HANDLE hDst); // 把hSrc中的数据拷贝到hDst

	virtual BOOL dumpKey(const void* pkey, size_t len, KTableRecordData& recData) = 0;

	// 记标志位
	BOOL updateData(HANDLE h, KTableRecordData& recData);

public:
	virtual BOOL on_recordLoaded(HANDLE h) = 0;
	virtual BOOL on_unloadRecord(HANDLE h) = 0;
	
public:// 内部使用
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
	
public:
	HANDLE _allocFromBottom();
	HANDLE _allocFromFree();
	void _freeRecord(HANDLE h); // 无 on_recordUnloaded

public:
	int_r m_flushSecs;
	KTableCache_X_Thread* m_thread;
};