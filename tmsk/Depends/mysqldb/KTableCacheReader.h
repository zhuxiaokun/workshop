#pragma once

#include <System/KType.h>
#include <System/KMacro.h>
#include <System/File/KMMap.h>
#include "KDatabase.h"
#include "KTableRecordData.h"

#define INVALID_HREC ((HANDLE)(KTableCacheReader::invalid_hrec))

struct TableCacheSetting
{
	size_t m_capacity;
	int_r m_flushInterval;
	const char* m_tbname;
	const char* m_mmapfile;
	KSQLTableSchema m_schema;
	int_r _nextTaskSlot;
	const KSQLConnParam* _connParam; // 临时使用
	///----------------------------
	void clear()
	{
		m_capacity = 0;
		m_flushInterval = 0;
		m_tbname = NULL;
		m_mmapfile = NULL;
		m_schema.Clear();
		_nextTaskSlot = 0;
		_connParam = NULL;
	}
};

class KTableCacheReader
{
friend class KCacheObject;
public:
	enum
	{
		invalid_hrec  = 0x7fffffff,
		invalid_recno = 0x7fffffff,
	};
	enum RecordStatus
	{
		c_new_created		= 0,  // 这条记录仅仅存在内存中，数据库中还没有
		c_updating			= 1,  // 应用程序正在修改记录中的数据
		c_modified			= 2,  // 这条记录被修改过了
		c_deleted			= 3,  // 这条记录需要删除
		c_checkout			= 4,  // 不需要保存在Cache里面了
		c_saving			= 5,  // 正在把记录保存到数据库
		c_loading			= 6,  // 正在从数据库加载
		c_partial_modified  = 7,  // 这条记录有部分字段被修改了
		c_new_record		= 8,  // 内存中新创建的记录，等待后台线程写入
		c_orphan			= 9,  // 被GameServer遗弃的记录，由于DataServer不在线，而玩家下线造成的
		c_clone				= 10, // 表示是clone的数据，不需要flush
		c_status_max
	};
	struct KRecordHead
	{
		union
		{
			struct
			{
				UINT64 used    : 1;
				UINT64 active  : 1;		// 是否活跃数据 ( 玩家在线 )
				UINT64 status  : 14;
				UINT64 version : 48;
				DWORD  loadTime;		// DataServer本地时间, LoadRecord的时间
				DWORD  flushTime;		// 进程的本地时间, FlushRecord的时间
			} m_used;
			struct
			{
				UINT64 used    : 1;
				UINT64 next     : 31;
				UINT64 reserved : 32;
				UINT64 reserved2;
			} m_free;
		};
		inline BOOL IsFree()
		{
			return !m_free.used;
		}
		inline BOOL IsActive() const
		{
			return m_used.used && m_used.active;
		}
		inline INT64 Version() const
		{
			return m_used.version;
		}
		inline void SetUsed(UINT64 ver)
		{
			m_used.used = 1;
			m_used.version = ver;
			m_used.status = 0;
		}
		inline void SetFree(DWORD nextNo)
		{
			m_free.used = 0;
			m_free.next = nextNo;
		}
		inline DWORD GetNextNo()
		{
			ASSERT(!m_free.used);
			return m_free.next;
		}
		inline void SetStatusBit(RecordStatus bitNo)
		{
			m_used.status |= ((UINT64)1 << bitNo);
		}
		inline void ClearStatusBit(RecordStatus bitNo)
		{
			m_used.status &= ~((UINT64)1<<bitNo);
		}
		inline void ClearStatus()
		{
			m_used.status = 0;
		}
		inline BOOL IsStatus(RecordStatus statusBit)
		{
			return (m_used.used) && (m_used.status&((UINT64)1<<statusBit));
		}
	};

public:
	struct CreateParam
	{
		const char* mmapfile;
		int_r capacity;
		int_r flushInterval;		// flush间隔(秒), flush的策略改成空闲(idle)时干
	};
	struct KFileHead
	{
		volatile DWORD usedCount;
		DWORD firstFree;
		DWORD recSize;			// 记录的长度，包括记录的头部
		DWORD capacity;			// 最多可容纳多少条记录
	};
	struct RecordDecl
	{
		int_r m_recSize;
		int_r m_modifyFlagSize;
		int_r m_recordDataLength;
		int_r* m_dataOffsets;
	};

public:
	KTableCacheReader();
	~KTableCacheReader();

public:
	virtual BOOL initialize(KSQLTableSchema& schema, const CreateParam& createParam);
	virtual void finalize();

public:
	// 清除记录中的状态: head status, field bitset
	void clearStatus(HANDLE h);

	BOOL discardRecord(HANDLE h);	// 丢弃cache中的数据，置标志位
	BOOL deleteRecord(HANDLE h);	// 删除记录，置标志位

public:
	const char* GetTableName() const;
	int GetFieldIndex(const char* fieldName) const;
	size_t GetRecordDataLength() const;
	void* GetRecordData(HANDLE h);
	void* _getRecordData(KRecordHead* head);
	void* _getColumnData(KRecordHead* head, int fieldIndex);

public:
	BOOL dumpKeyData(HANDLE h, KTableRecordData& recData);
	// 写入修改过的数据，不包括主键
	BOOL dumpFreshData(HANDLE h, KTableRecordData& recData);

public:
	BOOL IsValidHandle(HANDLE h);
	KRecordHead* GetRecordHead(HANDLE h);
	KRecordHead* _GetRecordHead(uint_r recNo);
	BOOL _GetBinary(HANDLE hRec, int fldIdx, int offset, void* value, int len, int& readed);
	BOOL _SetBinary(HANDLE hRec, int fldIdx, int offset, const void* value, int len);

public:
	static size_t calcRecordSize(const KSQLTableSchema& schema);
	size_t _getFieldData(HANDLE h, int_r fieldIndex, void** ppData);
	BOOL _setFieldData(HANDLE h, int_r fieldIndex, const void* pData, size_t len);

public:
	KMMap m_mmap;
	KFileHead* m_pFileHead;
	KSQLTableSchema m_schema_cpy;
	KSQLTableSchema* m_tableSchema;
	RecordDecl m_recordDecl;
	char* m_pStartMemory;
	char* m_pEndMemory;
};