#include "tableCacheUsePool.h"
#include <mysqldb/KCacheObject.h>
#include <System/Misc/StrUtil.h>
#include <System/Misc/KBitset.h>
#include <mysqldb/KCacheObject.h>
#include "usePoolTableCacheCreator.h"

namespace jg { namespace mysql {

	using namespace System::Sync;

	TableCacheUsePool::KRefreshThread::KRefreshThread(TableCacheUsePool& cache)
		: m_cache(cache)
		, m_refreshUpdateInterval(5)
		, m_refreshDeleteInterval(5)
		, m_refreshCheckoutInterval(5)
		, m_lastRefreshModify(0)
		, m_lastRefreshCheckOut(0)
		, m_lastRefreshDelete(0)
		, m_stopDesired(FALSE)
	{

	}

	TableCacheUsePool::KRefreshThread::~KRefreshThread()
	{
		this->Stop();
	}

	//TableCacheUsePool* TableCacheUsePool::CreateInstance(KSQLTableSchema& schema, funcPrepareSqlContext funcPrepareCtx, const CreateParam& createParam)
	//{
	//	TableCacheSetting setting;
	//	setting.m_capacity = createParam.capacity;
	//	setting.m_flushInterval = createParam.flushInterval;
	//	setting.m_tbname = schema.m_name.c_str();
	//	setting.m_mmapfile = createParam.mmapfile;
	//	setting.m_schema = schema;
	//	setting._nextTaskSlot = 0;
	//	setting._connParam = (KSQLConnParam*)funcPrepareCtx;
	//	KTraditionalCacheCreator creator;
	//	return (TableCacheUsePool*)creator.Create(setting);
	//}

	void TableCacheUsePool::KRefreshThread::Execute()
	{
		TIME_T now = (TIME_T)time(NULL);
		TIME_T last_chk_cnn = now;
		const int chk_cnn_interval = 60; // second

		while(!m_stopDesired)
		{
			DWORD recCount = m_cache.m_pFileHead->usedCount;

			// 第一遍是Checkout
			if(now >= m_lastRefreshCheckOut + m_refreshCheckoutInterval)
			{
				for(DWORD i=0; i<recCount && !m_stopDesired; i++)
				{
					HANDLE hRec = (HANDLE)i;
					TableCacheUsePool::KRecordHead* pRecHead = m_cache._GetRecordHead(i);

					if(pRecHead->IsFree()) continue;
					if(pRecHead->IsStatus(TableCacheUsePool::c_new_created)) continue;
					if(pRecHead->IsStatus(TableCacheUsePool::c_updating)) continue;
					if(pRecHead->IsStatus(TableCacheUsePool::c_loading)) continue;

					// 只有自己的这个进程才会把记录的状态设置为Saving，所以这里是出错了
					if(pRecHead->IsStatus(TableCacheUsePool::c_saving))
					{
						Log(LOG_WARN, "warn: refresh thread saving state %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
						pRecHead->ClearStatusBit(TableCacheUsePool::c_saving);
					}

					KCacheObject cacheObj; cacheObj.Attach(&m_cache, hRec);
					if(pRecHead->IsStatus(TableCacheUsePool::c_checkout))
					{
						KRefPtr<KSqlContext> sqlCtx(&m_cache.m_sqlCtx, true);
						if(!m_cache.prepareSqlContext(&cacheObj, NULL))
						{
							Log(LOG_ERROR, "error: tablceCache '%s' refresh, database not avail", m_cache.GetTableName());
							msleep(5000);
							continue;
						}
						if(!m_cache.RealCheckoutRecord(hRec))
						{
							Log(LOG_WARN, "warn: refresh thread checkout %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
						}
					}
				}
				m_lastRefreshCheckOut = (TIME_T)time(NULL);
			}

			// 第二遍是Delete
			if(now >= m_lastRefreshDelete + m_refreshDeleteInterval)
			{
				for(DWORD i=0; i<recCount && !m_stopDesired; i++)
				{
					HANDLE hRec = (HANDLE)i;
					TableCacheUsePool::KRecordHead* pRecHead = m_cache._GetRecordHead(i);

					if(pRecHead->IsFree()) continue;
					if(pRecHead->IsStatus(TableCacheUsePool::c_new_created)) continue;
					if(pRecHead->IsStatus(TableCacheUsePool::c_updating)) continue;
					if(pRecHead->IsStatus(TableCacheUsePool::c_loading)) continue;

					// 只有自己的这个进程才会把记录的状态设置为Saving，所以这里是出错了
					if(pRecHead->IsStatus(TableCacheUsePool::c_saving))
					{
						Log(LOG_WARN, "warn: refresh thread saving state %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
						pRecHead->ClearStatusBit(TableCacheUsePool::c_saving);
					}

					KCacheObject cacheObj; cacheObj.Attach(&m_cache, hRec);
					if(pRecHead->IsStatus(TableCacheUsePool::c_deleted))
					{
						KRefPtr<KSqlContext> sqlCtx(&m_cache.m_sqlCtx, true);
						if(!m_cache.prepareSqlContext(&cacheObj, NULL))
						{
							Log(LOG_ERROR, "error: tablceCache '%s' refresh, database not avail", m_cache.GetTableName());
							msleep(5000);
							continue;
						}
						if(!m_cache.RealDeleteRecord(hRec))
						{
							Log(LOG_WARN, "warn: refresh thread delete %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
						}
					}
				}
				m_lastRefreshDelete = (TIME_T)time(NULL);
			}

			// 第三遍是Modified
			if(now >= m_lastRefreshModify + m_refreshUpdateInterval)
			{
				for(DWORD i=0; i<recCount && !m_stopDesired; i++)
				{
					HANDLE hRec = (HANDLE)i;
					TableCacheUsePool::KRecordHead* pRecHead = m_cache._GetRecordHead(i);

					if(pRecHead->IsFree()) continue;
					if(pRecHead->IsStatus(TableCacheUsePool::c_new_created)) continue;
					if(pRecHead->IsStatus(TableCacheUsePool::c_updating)) continue;
					if(pRecHead->IsStatus(TableCacheUsePool::c_loading)) continue;

					// 只有自己的这个进程才会把记录的状态设置为Saving，所以这里是出错了
					if(pRecHead->IsStatus(TableCacheUsePool::c_saving))
					{
						Log(LOG_WARN, "warn: refresh thread saving state %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
						pRecHead->ClearStatusBit(TableCacheUsePool::c_saving);
					}

					KCacheObject cacheObj; cacheObj.Attach(&m_cache, hRec);
					if(pRecHead->IsStatus(TableCacheUsePool::c_new_record))
					{
						KRefPtr<KSqlContext> sqlCtx(&m_cache.m_sqlCtx, true);
						if(!m_cache.prepareSqlContext(&cacheObj, NULL))
						{
							Log(LOG_ERROR, "error: tablceCache '%s' refresh, database not avail", m_cache.GetTableName());
							msleep(5000);
							continue;
						}
						if(!m_cache.RealInsertRecord(hRec))
						{
							Log(LOG_WARN, "warn: refresh thread insert %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
						}
					}

					if(pRecHead->IsStatus(TableCacheUsePool::c_modified))
					{
						KRefPtr<KSqlContext> sqlCtx(&m_cache.m_sqlCtx, true);
						if(!m_cache.prepareSqlContext(&cacheObj, NULL))
						{
							Log(LOG_ERROR, "error: tablceCache '%s' refresh, database not avail", m_cache.GetTableName());
							msleep(5000);
							continue;
						}
						if(!m_cache.RealUpdateRecord(hRec))
						{
							Log(LOG_WARN, "warn: refresh thread update %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
						}
						continue;
					}

					if(pRecHead->IsStatus(TableCacheUsePool::c_partial_modified))
					{
						KRefPtr<KSqlContext> sqlCtx(&m_cache.m_sqlCtx, true);
						if(!m_cache.prepareSqlContext(&cacheObj, NULL))
						{
							Log(LOG_ERROR, "error: tablceCache '%s' refresh, database not avail", m_cache.GetTableName());
							msleep(5000);
							continue;
						}
						if(!m_cache.RealPartialUpdateRecord(hRec))
						{
							Log(LOG_WARN, "warn: refresh thread partial update %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
						}
						continue;
					}
				}
				m_lastRefreshModify = (TIME_T)time(NULL);
			}
			msleep(200);
		}
	}

	void TableCacheUsePool::KRefreshThread::Stop()
	{
		m_stopDesired = TRUE;
		if(!KThread::GetTerminated())
		{
			KThread::WaitFor();
		}
		m_stopDesired = FALSE;
	}


	// TableCacheUsePool

	TableCacheUsePool::TableCacheUsePool()
		: m_refreshThread(*this)
	{
		m_sqlParams = NULL;
		m_pEmergencyObserver = NULL;
	}

	TableCacheUsePool::~TableCacheUsePool()
	{
		//this->Finalize();
	}

	TableCacheUsePool* TableCacheUsePool::CreateInstance(KSQLTableSchema& schema, funcPrepareSqlContext funcPrepareCtx, const CreateParam& createParam)
	{
		TableCacheSetting setting;
		setting.m_capacity = createParam.capacity;
		setting.m_flushInterval = createParam.flushInterval;
		setting.m_tbname = schema.m_name.c_str();
		setting.m_mmapfile = createParam.mmapfile;
		setting.m_schema = schema;
		setting._nextTaskSlot = 0;
		setting._connParam = (KSQLConnParam*)funcPrepareCtx;
		UsePoolTableCacheCreator creator;
		return (TableCacheUsePool*)creator.Create(setting);
	}

	BOOL TableCacheUsePool::Initialize(funcPrepareSqlContext funcPrepareCtx, KSQLTableSchema& schema, const CreateParam& createParam)
	{
		ASSERT(funcPrepareCtx); m_funcPrepareCtx = funcPrepareCtx;

		if(!KTableCacheReader::initialize(schema, createParam))
			return FALSE;

		int pknum = m_tableSchema->GetPrimaryKeyFieldCount();
		int fieldCount = m_tableSchema->GetFieldCount();
		if(pknum < 1 || pknum > 2)
		{
			Log(LOG_FATAL, "fatal: primary key num can only be 1 or 2, table:%s", m_tableSchema->m_name.c_str());
			return FALSE;
		}

		m_sqlParams = new KSQLFieldDescribe[m_tableSchema->GetFieldCount()];

		this->_CreateInsertSql();
		this->_CreateUpdateSql();
		this->_CreateDeleteSql();
		this->_CreateSelectSql();

		KCacheObject cacheObj;
		for(DWORD i=0; i<m_pFileHead->usedCount; i++)
		{
			HANDLE hRec = (HANDLE)i;
			KRecordHead* pRecHead = this->_GetRecordHead(i);

			if(pRecHead->IsFree()) continue;

			cacheObj.Attach(this, hRec);
			if(pRecHead->IsStatus(c_new_created))
			{
				this->_FreeRecord(hRec);
				continue;
			}
			if(pRecHead->IsStatus(c_loading))
			{
				this->_FreeRecord(hRec);
				continue;
			}
			if(pRecHead->IsStatus(c_saving))
			{
				pRecHead->ClearStatusBit(c_saving);
			}

			m_funcPrepareCtx(&m_sqlCtx, &cacheObj, NULL);
			KRefPtr<KSqlContext> sqlCtx(&m_sqlCtx, true);

			if(pRecHead->IsStatus(c_deleted))
			{
				this->RealDeleteRecord(hRec);
			}
			else
			{
				if(pRecHead->IsStatus(c_new_record))
				{
					m_funcPrepareCtx(&m_sqlCtx, &cacheObj, NULL);
					this->RealInsertRecord(hRec);
				}
				if(pRecHead->IsStatus(c_modified))
				{
					this->RealUpdateRecord(hRec);
				}
				if(pRecHead->IsStatus(c_partial_modified))
				{
					this->RealPartialUpdateRecord(hRec);
				}
				if(pRecHead->IsStatus(c_checkout))
				{
					this->RealCheckoutRecord(hRec);
				}
			}
		}

		m_pFileHead->usedCount = 0;
		m_pFileHead->firstFree = invalid_recno;

		m_refreshThread.m_refreshUpdateInterval = (int)createParam.flushInterval;
		m_refreshThread.m_refreshDeleteInterval = (int)createParam.flushInterval;
		m_refreshThread.m_refreshCheckoutInterval = (int)createParam.flushInterval;
		m_refreshThread.Run();

		return TRUE;
	}

	void TableCacheUsePool::Finalize()
	{
		m_refreshThread.Stop();
		KTableCacheReader::finalize();
		if(m_sqlParams)
		{
			delete [] m_sqlParams;
			m_sqlParams = NULL;
		}
	}

	void TableCacheUsePool::finalize()
	{
		this->Finalize();
	}

	BOOL TableCacheUsePool::_IsValidHandle(HANDLE hRec)
	{
		return (uint_r)hRec != invalid_hrec;
	}

	bool TableCacheUsePool::prepareSqlContext(KCacheObject* cacheObj, LMREG::CppTable* args)
	{
		m_sqlCtx.reset();
		ASSERT(m_funcPrepareCtx);
		return m_funcPrepareCtx(&m_sqlCtx, cacheObj, args);
	}

	void TableCacheUsePool::_markAllPartialModified(HANDLE hRec)
	{
		KRecordHead* pHead = this->GetRecordHead(hRec);
		if(!pHead) return;

		int n = m_tableSchema->GetFieldCount();
		KBitset bitset(pHead+1, n);

		for(int i=0; i<n; i++)
		{
			const KSQLTableField* col = m_tableSchema->GetField(i);
			if(!col->m_primaryKey)
			{
				bitset.SetBit(i);
			}
		}

		pHead->ClearStatusBit(c_modified);
		pHead->SetStatusBit(c_partial_modified);
	}

	int TableCacheUsePool::_getPartialModifiedCount(HANDLE hRec)
	{
		KRecordHead* pHead = this->GetRecordHead(hRec);
		if(!pHead) return 0;

		int n = m_tableSchema->GetFieldCount();

		int sum = 0;
		KBitset bitset(pHead+1, n);
		for(int i=0; i<n; i++)
		{
			if(bitset.GetBit(i))
			{
				sum++;
			}
		}
		return sum;
	}

	HANDLE TableCacheUsePool::FindRecord(const void* pKey, int len)
	{
		ASSERT(this->_checkKeyLength(len));
		KAutoThreadMutex mx(m_mx);
		return this->_findRecord(pKey);
	}

	HANDLE TableCacheUsePool::LoadRecord(const void* pKey, int len)
	{
		ASSERT(this->_checkKeyLength(len));

		// 分配和释放以及数据库操作会在多个线程中被调用，所以需要保护
		KAutoThreadMutex mx(m_mx);

		KCacheObject cacheObj;

		HANDLE hRec = (HANDLE)invalid_hrec;
		KRecordHead* pHead = NULL;

		hRec = this->_findRecord(pKey);
		if(this->_IsValidHandle(hRec))
		{
			this->on_unloadRecord(hRec);
			this->_FreeRecord(hRec);
			Log(LOG_ERROR, "error: pkey:%s table:%s find an exist record", this->Key2String(pKey).c_str(), m_tableSchema->m_name.c_str());
		}

		// 先分配一条记录的内存
		hRec = this->_AllocRecord();
		if(!this->_IsValidHandle(hRec))
		{
			Log(LOG_ERROR, "error: %s shared memory is used out for pkey:%s", this->GetTableName(), this->Key2String(pKey).c_str());
			return (HANDLE)invalid_hrec;
		}

		pHead = this->_GetRecordHead((DWORD)(uint_r)hRec);
		pHead->SetStatusBit(c_loading);

		//this->_initRecord(pHead);
		//memset(pHead+1, 0, m_recSize-sizeof(KRecordHead));

		cacheObj.Attach(this, hRec);
		this->SetPrimaryKey(cacheObj, pKey);

		if(!this->_LoadRecord(hRec))
		{
			Log(LOG_ERROR, "error: %s load record data for objectID pkey:%s", this->GetTableName(), this->Key2String(pKey).c_str());
			this->_FreeRecord(hRec);
			return (HANDLE)invalid_hrec;
		}

		if(!this->IsPrimaryKey(cacheObj, pKey))
		{// 不可达
			Log(LOG_ERROR, "error: %s load record data for objectID pkey:%s, pkey mis-match", this->GetTableName(), this->Key2String(pKey).c_str());
			this->_FreeRecord(hRec);
			return (HANDLE)invalid_hrec;
		}

		this->on_recordLoaded(hRec);
		pHead->ClearStatusBit(c_loading);

		return hRec;
	}

	HANDLE TableCacheUsePool::CreateRecord(const void* pKey, int len)
	{
		ASSERT(this->_checkKeyLength(len));

		// 分配和释放以及数据库操作会在多个线程中被调用，所以需要保护
		KAutoThreadMutex mx(m_mx);

		HANDLE hRec;
		KRecordHead* pHead = NULL;
		KCacheObject cacheObj;

		hRec = this->_findRecord(pKey);
		if(this->_IsValidHandle(hRec))
		{
			pHead = this->_GetRecordHead((uint_r)hRec);
			if(pHead->IsStatus(c_deleted))
			{
				pHead->ClearStatusBit(c_deleted);
				pHead->SetStatusBit(c_updating);
				pHead->m_used.version += 1;
				this->_fillDefaults(hRec);
				pHead->SetStatusBit(c_modified);
				pHead->ClearStatusBit(c_updating);
				return hRec;
			}
			else
			{
				Log(LOG_ERROR, "error: CreateRecord, pkey:%s table:%s find an exist record", this->Key2String(pKey).c_str(), m_tableSchema->m_name.c_str());
				return INVALID_HREC;
			}
		}

		hRec = this->_AllocRecord();
		if(!this->_IsValidHandle(hRec))
		{
			Log(LOG_ERROR, "error: %s memory map file is used out for pkey:%s", this->GetTableName(), this->Key2String(pKey).c_str());
			return (HANDLE)invalid_hrec;
		}

		pHead = this->_GetRecordHead((DWORD)(uint_r)hRec);
		//this->_initRecord(pHead);

		pHead->SetStatusBit(c_new_created);

		cacheObj.Attach(this, hRec);
		this->SetPrimaryKey(cacheObj, pKey);

		this->on_recordLoaded(hRec);

		pHead->SetStatusBit(c_new_record);
		pHead->ClearStatusBit(c_new_created);

		return hRec;
	}

	BOOL TableCacheUsePool::DiscardRecord(const void* pKey, int len)
	{
		ASSERT(this->_checkKeyLength(len));

		KAutoThreadMutex mx(m_mx);

		HANDLE hRec = this->_findRecord(pKey);
		if(!this->_IsValidHandle(hRec)) return TRUE;

		KRecordHead* pHead = this->GetRecordHead(hRec);
		if(!pHead) return FALSE;

		KCacheObject cacheObj;
		ASSERT(cacheObj.Attach(this, hRec));

		this->on_unloadRecord(hRec);
		this->_FreeRecord(hRec);

		return TRUE;
	}

	BOOL TableCacheUsePool::DeleteRecord(const void* pKey, int len)
	{
		ASSERT(this->_checkKeyLength(len));

		KAutoThreadMutex mx(m_mx);

		HANDLE hRec = this->_findRecord(pKey);
		if(!this->_IsValidHandle(hRec)) return TRUE;

		KRecordHead* pHead = this->GetRecordHead(hRec);
		if(!pHead) return FALSE;

		pHead->m_used.version += 1;
		pHead->SetStatusBit(c_deleted);
		return TRUE;
	}

	BOOL TableCacheUsePool::CheckoutRecord(const void* pKey, int len)
	{
		ASSERT(this->_checkKeyLength(len));

		KAutoThreadMutex mx(m_mx);

		HANDLE hRec = this->_findRecord(pKey);
		if(!this->_IsValidHandle(hRec)) return FALSE;

		KRecordHead* pHead = this->GetRecordHead(hRec);
		if(!pHead) return FALSE;

		pHead->m_used.version += 1;
		pHead->SetStatusBit(c_checkout);

		return TRUE;
	}

	//BOOL TableCacheUsePool::EmergencyCheckout(const void* pKey, int len, void* pUserData)
	//{
	//	ASSERT(this->_checkKeyLength(len));

	//	KAutoThreadMutex mx(m_mx);

	//	HANDLE hRec = this->_findRecord(pKey);
	//	if(!this->_IsValidHandle(hRec)) return FALSE;

	//	KRecordHead* pHead = this->GetRecordHead(hRec);
	//	if(!pHead)
	//	{
	//		this->OnEmergencyFinished(FALSE, pUserData);
	//		return FALSE;
	//	}

	//	pHead->m_used.version += 1;
	//	pHead->SetStatusBit(c_checkout);

	//	KEmergencyEvent event = {hRec, pUserData};
	//	m_emergencyQueue.push(event);

	//	return TRUE;
	//}

	//BOOL TableCacheUsePool::EmergencyCheckout(HANDLE hRec, void* pUserData)
	//{
	//	KAutoThreadMutex mx(m_mx);
	//	if(!this->_IsValidHandle(hRec)) return FALSE;

	//	KRecordHead* pHead = this->GetRecordHead(hRec);
	//	if(!pHead)
	//	{
	//		this->OnEmergencyFinished(FALSE, pUserData);
	//		return FALSE;
	//	}

	//	pHead->m_used.version += 1;
	//	pHead->SetStatusBit(c_checkout);

	//	KEmergencyEvent event = {hRec, pUserData};
	//	m_emergencyQueue.push(event);

	//	return TRUE;
	//}

	BOOL TableCacheUsePool::DiscardRecord2(HANDLE hRec)
	{
		KAutoThreadMutex mx(m_mx);

		KRecordHead* pHead = this->GetRecordHead(hRec);
		if(!pHead) return FALSE;

		KCacheObject cacheObj;
		ASSERT(cacheObj.Attach(this, hRec));

		this->on_unloadRecord(hRec);
		this->_FreeRecord(hRec);

		return TRUE;
	}

	BOOL TableCacheUsePool::DeleteRecord2(HANDLE hRec)
	{
		KAutoThreadMutex mx(m_mx);

		KRecordHead* pHead = this->GetRecordHead(hRec);
		if(!pHead) return FALSE;

		pHead->m_used.version += 1;
		pHead->SetStatusBit(c_deleted);
		return TRUE;
	}

	BOOL TableCacheUsePool::CheckoutRecord2(HANDLE hRec)
	{
		KAutoThreadMutex mx(m_mx);

		KRecordHead* pHead = this->GetRecordHead(hRec);
		if(!pHead) return FALSE;

		pHead->m_used.version += 1;
		pHead->SetStatusBit(c_checkout);

		return TRUE;
	}

	HANDLE TableCacheUsePool::CreateRecordDirect()
	{
		// 分配和释放以及数据库操作会在多个线程中被调用，所以需要保护
		KAutoThreadMutex mx(m_mx);

		HANDLE hRec;
		KRecordHead* pHead = NULL;
		hRec = this->_AllocRecord();
		if(!this->_IsValidHandle(hRec))
		{
			Log(LOG_ERROR, "error: %s memory map file is used out", this->GetTableName());
			return INVALID_HREC;
		}

		pHead = this->_GetRecordHead((DWORD)(uint_r)hRec);

		pHead->SetStatusBit(c_saving);
		if(!this->_InsertRecord(hRec, FALSE))
		{
			this->_FreeRecord(hRec);
			return INVALID_HREC;
		}

		//// 设置主键的值
		KCacheObject cacheObj; cacheObj.Attach(this, hRec);
		KSQLConnection& m_sqlCnn = m_sqlCtx.pooledCnn->connection();
		UINT64 lastId = m_sqlCnn.GetLastAutoIncID();
		this->SetAutoIncreaseKey(cacheObj, lastId);

		this->on_recordLoaded(hRec);
		pHead->ClearStatusBit(c_saving);

		return hRec;
	}

	HANDLE TableCacheUsePool::CreateRecordDirect(void* pKey, int len)
	{
		ASSERT(this->_checkKeyLength(len));

		// 分配和释放以及数据库操作会在多个线程中被调用，所以需要保护
		KAutoThreadMutex mx(m_mx);

		HANDLE hRec;
		KRecordHead* pHead = NULL;

		hRec = this->_findRecord(pKey);
		if(this->_IsValidHandle(hRec))
		{
			pHead = this->_GetRecordHead((uint_r)hRec);
			if(!pHead->IsStatus(c_deleted))
			{
				Log(LOG_ERROR, "error: CreateRecordDirect, pkey:%s table:%s find an exist record", this->Key2String(pKey).c_str(), m_tableSchema->m_name.c_str());
				return INVALID_HREC;
			}

			// 数据库中还没有创建记录
			if(pHead->IsStatus(c_new_record))
			{
				pHead->ClearStatus();
				pHead->m_used.version += 1;
				pHead->SetStatusBit(c_saving);
				this->_fillDefaults(hRec);
				if(!this->_InsertRecord(hRec, TRUE))
				{
					Log(LOG_ERROR, "error: CreateRecordDirect table:%s pkey:%s", m_tableSchema->m_name.c_str(), this->Key2String(pKey).c_str());
					this->on_unloadRecord(hRec);
					this->_FreeRecord(hRec);
					return INVALID_HREC;
				}
				pHead->ClearStatusBit(c_saving);
				return hRec;
			}
			else
			{
				pHead->ClearStatusBit(c_deleted);
				pHead->m_used.version += 1;
				pHead->SetStatusBit(c_updating);
				this->_fillDefaults(hRec);
				pHead->SetStatusBit(c_modified);
				pHead->ClearStatusBit(c_updating);
				return hRec;
			}
		}
		else
		{
			hRec = this->_AllocRecord();
			if(!this->_IsValidHandle(hRec))
			{
				Log(LOG_ERROR, "error: %s memory map file is used out for pkey:%s", this->GetTableName(), this->Key2String(pKey).c_str());
				return INVALID_HREC;
			}

			pHead = this->_GetRecordHead((DWORD)(uint_r)hRec);

			KCacheObject cacheObj; // 设置主键的值
			cacheObj.Attach(this, hRec);
			this->SetPrimaryKey(cacheObj, pKey);

			pHead->SetStatusBit(c_saving);

			if(!this->_InsertRecord(hRec, TRUE))
			{
				this->_FreeRecord(hRec);
				return INVALID_HREC;
			}

			this->on_recordLoaded(hRec);
			pHead->ClearStatusBit(c_saving);

			return hRec;
		}
	}

	BOOL TableCacheUsePool::_LoadRecord(HANDLE hRec)
	{
		KSQLCommand& m_sqlCmd = m_sqlCtx.cmd;
		KSQLConnection& m_sqlCnn = m_sqlCtx.pooledCnn->connection();
		KSQLResultSet& m_sqlRs = m_sqlCtx.rs;

		int fieldCount = m_tableSchema->GetFieldCount();
		int begField = m_tableSchema->GetPrimaryKeyFieldCount();

		for(int i=0; i<begField; i++)
		{
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			KSQLFieldDescribe& describe = m_sqlParams[i];

			describe.m_name = pfld->m_name;
			describe.m_length = pfld->m_length;
			describe.m_cdt = pfld->m_cdt;
		}

		if(!m_sqlCmd.SetSQLStatement(m_sqlSelect.c_str()))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}
		if(!m_sqlCmd.DescribeParameters(m_sqlParams, begField))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		KCacheObject cacheObj;
		cacheObj.Attach(this, hRec);

		if(!this->SetSQLParam(m_sqlCmd, 0, cacheObj))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		if(!m_sqlCmd.ExecuteQuery(m_sqlRs))
		{
			Log(LOG_ERROR, "error: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		for(int i=begField; i<fieldCount; i++)
		{
			int colIdx = i - begField;
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			KSQLFieldDescribe& describe = m_sqlParams[colIdx];

			describe.m_name = pfld->m_name;
			describe.m_length = pfld->m_length;
			describe.m_cdt = pfld->m_cdt;
		}

		if(!m_sqlRs.DescribeColumns(m_sqlParams, fieldCount-begField))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlRs.GetLastErrorMsg());
			return FALSE;
		}

		if(!m_sqlRs.Begin())
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlRs.GetLastErrorMsg());
			return FALSE;
		}

		KRecordHead* pHead = this->_GetRecordHead((DWORD)(uint_r)hRec);

		for(int i=begField; i<fieldCount; i++)
		{
			int colIdx = i - begField;
			KSQLFieldDescribe& describe = m_sqlParams[colIdx];
			char* pData = (char*)pHead + m_recordDecl.m_dataOffsets[i];
			switch(describe.m_cdt)
			{
			case KSQLTypes::sql_c_string:
			case KSQLTypes::sql_c_varstring:
			case KSQLTypes::sql_c_binary:
				{
					int len = describe.m_length;
					WORD& actualLength = *(WORD*)pData; pData += 2;
					if(!m_sqlRs.GetBinary(colIdx, pData, len))
					{
						Log(LOG_ERROR, "error: sql: %s", m_sqlRs.GetLastErrorMsg());
						return FALSE;
					}
					actualLength = len;
				} break;
				//case KSQLTypes::sql_c_varbinary:
				//	{
				//		void* data = NULL;
				//		int data_len = m_sqlRs._getBinary(colIdx, &data);
				//		if(data_len < 1)
				//		{
				//			*(HANDLE*)pData = INVALID_HREC;
				//		}
				//		else
				//		{
				//			KVarBinary vb = this->_getVarBinary(pHead, i);
				//			ASSERT(vb.isValid());
				//			vb.setValue(data, data_len);
				//		}
				//	} break;
			default:
				{
					if(!m_sqlRs.GetBinary(colIdx, pData, describe.m_length))
					{
						Log(LOG_ERROR, "error: sql: %s", m_sqlRs.GetLastErrorMsg());
						return FALSE;
					}
				} break;
			}
		}

		return TRUE;
	}

	BOOL TableCacheUsePool::_InsertRecord(HANDLE hRec, BOOL withKey)
	{
		KSQLCommand& m_sqlCmd = m_sqlCtx.cmd;
		KSQLConnection& m_sqlCnn = m_sqlCtx.pooledCnn->connection();
		KSQLResultSet& m_sqlRs = m_sqlCtx.rs;

		char cTmp[64];
		int fieldCount = m_tableSchema->GetFieldCount();

		for(int i=0; i<fieldCount; i++)
		{
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			KSQLFieldDescribe& describe = m_sqlParams[i];

			describe.m_name = pfld->m_name;
			describe.m_length = pfld->m_length;
			describe.m_cdt = pfld->m_cdt;
		}

		if(!m_sqlCmd.SetSQLStatement(m_sqlInsert.c_str()))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}
		if(!m_sqlCmd.DescribeParameters(m_sqlParams, fieldCount))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		KCacheObject cacheObj;
		cacheObj.Attach(this, hRec);

		// 首先设置主键的值
		int begField = m_tableSchema->GetPrimaryKeyFieldCount();
		if(withKey)
		{	
			if(!this->SetSQLParam(m_sqlCmd, 0, cacheObj))
			{
				Log(LOG_ERROR, "error: sql: %s, set primary key param", m_sqlCmd.GetLastErrorMsg());
				return FALSE;
			}
		}
		else
		{
			for(int_r i=0; i<begField; i++)
				m_sqlCmd.SetNull((int)i);
		}

		// 设置缺省值
		for(int i=begField; i<fieldCount; i++)
		{
			int paramIdx = i;
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			const System::Collections::KString<32>& defVal = pfld->m_defaultVal;

			switch(pfld->m_cdt)
			{
			case KSQLTypes::sql_c_int8:
			case KSQLTypes::sql_c_uint8:
			case KSQLTypes::sql_c_int16:
			case KSQLTypes::sql_c_uint16:
			case KSQLTypes::sql_c_int32:
			case KSQLTypes::sql_c_uint32:
			case KSQLTypes::sql_c_int64:
			case KSQLTypes::sql_c_uint64:
			case KSQLTypes::sql_c_float32:
			case KSQLTypes::sql_c_float64:
				{
					if(defVal.size() < 1) m_sqlCmd.SetNull(paramIdx);
					else
					{
						INT64 val = str2i64(defVal.c_str());
						if(!m_sqlCmd.SetInt64(paramIdx, val))
						{
							Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
							return FALSE;
						}
					}
					break;
				}
			case KSQLTypes::sql_c_timestamp:
				{// 先不考虑
					if(defVal.size() < 1) m_sqlCmd.SetNull(paramIdx);
					else
					{
						char cDef[128];
						strcpy_k(cDef, sizeof(cDef), defVal.c_str());
						char* pc = _trim(cDef);

						KDatetime datetime;
						if(pc[0] >= '0' && pc[0] <= '9')
						{
							int len = (int)strlen(cTmp);
							if(len < 9)
							{// 没有时间部分
								strcat(pc, " 0:0:0");
							}
							if(!datetime.SetDatetimeString(pc))
							{
								return FALSE;
							}
						}
						else
						{
							if(_strnicmp(pc, "now()", 3) && _strnicmp(pc, "current_date()", 12))
							{
								return FALSE;
							}
						}
						if(!m_sqlCmd.SetTimestamp(paramIdx, (TIME_T)datetime.Time()))
						{
							Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
							return FALSE;
						}
					}
					break;
				}
			case KSQLTypes::sql_c_string:
			case KSQLTypes::sql_c_varstring:
			case KSQLTypes::sql_c_binary:
				//case KSQLTypes::sql_c_varbinary:
				{
					if(defVal.size() < 1)
					{
						if(pfld->m_nullable) m_sqlCmd.SetNull(paramIdx);
						else
						{
							Log(LOG_ERROR, "error: sql: %s can not be null", pfld->m_name.c_str());
							return FALSE;
						}
					}
					else
					{
						if(!m_sqlCmd.SetBinary(paramIdx, defVal.c_str(), (int)defVal.size()))
						{
							Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
							return FALSE;
						}
					}
					break;
				}
			}
		}

		// 执行SQL
		if(!m_sqlCmd.Execute())
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		return TRUE;
	}

	BOOL TableCacheUsePool::_UpdateRecord(HANDLE hRec)
	{
		KSQLCommand& m_sqlCmd = m_sqlCtx.cmd;
		KSQLConnection& m_sqlCnn = m_sqlCtx.pooledCnn->connection();
		KSQLResultSet& m_sqlRs = m_sqlCtx.rs;

		KRecordHead* pHead = this->_GetRecordHead((DWORD)(uint_r)hRec);
		DWORD currVer = pHead->m_used.version;

		int begField = m_tableSchema->GetPrimaryKeyFieldCount();
		int fieldCount = m_tableSchema->GetFieldCount();

		// 非组建的字段个数，non primary-key count
		int npc = fieldCount - begField;

		for(int i=0; i<fieldCount; i++)
		{
			int paramIdx = (i<begField) ? (i+npc) : (i-begField);
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			KSQLFieldDescribe& describe = m_sqlParams[paramIdx];

			describe.m_name = pfld->m_name;
			describe.m_length = pfld->m_length;
			describe.m_cdt = pfld->m_cdt;
		}

		if(!m_sqlCmd.SetSQLStatement(m_sqlUpdate.c_str()))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}
		if(!m_sqlCmd.DescribeParameters(m_sqlParams, fieldCount))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		// 设置字段的值
		for(int i=0; i<fieldCount; i++)
		{
			int paramIdx = (i<begField) ? (i+npc) : (i-begField);

			KSQLFieldDescribe& describe = m_sqlParams[paramIdx];
			const KSQLTableField* pfld = m_tableSchema->GetField(i);

			char* pData = (char*)pHead +m_recordDecl.m_dataOffsets[i];
			switch(describe.m_cdt)
			{
			case KSQLTypes::sql_c_string:
			case KSQLTypes::sql_c_varstring:
			case KSQLTypes::sql_c_binary:
				{
					WORD actualLength = *(WORD*)pData; pData += 2;
					if(!actualLength)
					{
						if(pfld->m_nullable) m_sqlCmd.SetNull(paramIdx);
						else
						{
							Log(LOG_ERROR, "error: sql: field %s can not be null", pfld->m_name.c_str());
							return FALSE;
						}
					}
					else
					{
						if(!m_sqlCmd.SetBinary(paramIdx, pData, actualLength))
						{
							Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
							return FALSE;
						}
					}
				} break;
				//case KSQLTypes::sql_c_varbinary:
				//	{
				//		KVarBinary vb = this->_getVarBinary(pHead, i);
				//		ASSERT(vb.isValid());
				//		if(vb.hasData())
				//		{
				//			KSQLFieldDescribe* sqlParam = m_sqlCmd._getParameter(paramIdx);
				//			sqlParam->_buffer;
				//			sqlParam->_actualLength = vb.getValue(sqlParam->_buffer, sqlParam->m_length);
				//			if(sqlParam->_actualLength < 1)
				//			{
				//				m_sqlCmd.SetNull(paramIdx);
				//			}
				//		}
				//		else
				//		{
				//			m_sqlCmd.SetNull(paramIdx);
				//		}

				//	} break;
			default:
				{
					if(!m_sqlCmd.SetBinary(paramIdx, pData, describe.m_length))
					{
						Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
						return FALSE;
					}
					break;
				}
			}
		}

		if(pHead->IsStatus(c_updating) || pHead->m_used.version != currVer)
		{//数据已经发生了变化，不用执行了
			return TRUE;
		}

		// 执行SQL语句
		if(!m_sqlCmd.Execute())
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		if(!pHead->IsStatus(c_updating) && pHead->m_used.version == currVer)
		{
			pHead->ClearStatusBit(c_modified);
			pHead->ClearStatusBit(c_partial_modified);
			KBitset bitset(pHead+1, m_tableSchema->GetFieldCount());
			bitset.ClearAll();
		}

		return TRUE;
	}

	BOOL TableCacheUsePool::_PartialUpdateRecord(HANDLE hRec)
	{
		KSQLCommand& m_sqlCmd = m_sqlCtx.cmd;
		KSQLConnection& m_sqlCnn = m_sqlCtx.pooledCnn->connection();
		KSQLResultSet& m_sqlRs = m_sqlCtx.rs;

		KRecordHead* pHead = this->_GetRecordHead((DWORD)(uint_r)hRec);
		DWORD currVer = pHead->m_used.version;

		int begField = m_tableSchema->GetPrimaryKeyFieldCount();
		int paramCount = this->_CreatePartialUpdateSQL(hRec);
		int npc = paramCount - begField;

		int paramNum = 0;
		int fieldCount = m_tableSchema->GetFieldCount();
		KBitset bitset(pHead+1, fieldCount);

		for(int i=0; i<fieldCount; i++)
		{
			// 非主键，未修改
			if(i >= begField && !bitset.GetBit(i)) continue;

			int paramIdx = (i<begField) ? (i+npc) : (paramNum-begField);
			paramNum++; // 处理了几个参数

			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			KSQLFieldDescribe& describe = m_sqlParams[paramIdx];

			describe.m_name = pfld->m_name;
			describe.m_length = pfld->m_length;
			describe.m_cdt = pfld->m_cdt;
		}
		if(!m_sqlCmd.SetSQLStatement(m_sqlPartialUpdate.c_str()))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}
		if(!m_sqlCmd.DescribeParameters(m_sqlParams, paramCount))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		// 设置字段的值
		paramNum = 0;
		for(int i=0; i<fieldCount; i++)
		{
			if(i >= begField && !bitset.GetBit(i)) continue;

			int paramIdx = (i<begField) ? (i+npc) : (paramNum-begField);
			paramNum++; // 处理了几个参数

			KSQLFieldDescribe& describe = m_sqlParams[paramIdx];
			const KSQLTableField* pfld = m_tableSchema->GetField(i);

			char* pData = (char*)pHead +m_recordDecl.m_dataOffsets[i];
			switch(describe.m_cdt)
			{
			case KSQLTypes::sql_c_string:
			case KSQLTypes::sql_c_varstring:
			case KSQLTypes::sql_c_binary:
				{
					WORD actualLength = *(WORD*)pData; pData += 2;
					if(!actualLength)
					{
						if(pfld->m_nullable) m_sqlCmd.SetNull(paramIdx);
						else
						{
							Log(LOG_ERROR, "error: sql: field %s can not be null", pfld->m_name.c_str());
							return FALSE;
						}
					}
					else
					{
						if(!m_sqlCmd.SetBinary(paramIdx, pData, actualLength))
						{
							Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
							return FALSE;
						}
					}
				} break;
				//case KSQLTypes::sql_c_varbinary:
				//	{
				//		KVarBinary vb = this->_getVarBinary(pHead, i);
				//		ASSERT(vb.isValid());
				//		if(vb.hasData())
				//		{
				//			KSQLFieldDescribe* sqlParam = m_sqlCmd._getParameter(paramIdx);
				//			sqlParam->_buffer;
				//			sqlParam->_actualLength = vb.getValue(sqlParam->_buffer, sqlParam->m_length);
				//			if(sqlParam->_actualLength < 1)
				//			{
				//				m_sqlCmd.SetNull(paramIdx);
				//			}
				//		}
				//		else
				//		{
				//			m_sqlCmd.SetNull(paramIdx);
				//		}

				//	} break;
			default:
				{
					if(!m_sqlCmd.SetBinary(paramIdx, pData, describe.m_length))
					{
						Log(LOG_DEBUG, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
						return FALSE;
					}
					break;
				}
			}
		}

		if(pHead->IsStatus(c_updating) || pHead->m_used.version != currVer)
		{//数据已经发生了变化，不用执行了
			return TRUE;
		}

		// 执行SQL语句
		if(!m_sqlCmd.Execute())
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		if(!pHead->IsStatus(c_updating) && pHead->m_used.version == currVer)
		{
			pHead->ClearStatusBit(c_partial_modified);
			bitset.ClearAll();
		}

		return TRUE;
	}

	BOOL TableCacheUsePool::_DeleteRecord(HANDLE hRec)
	{
		KSQLCommand& m_sqlCmd = m_sqlCtx.cmd;
		KSQLConnection& m_sqlCnn = m_sqlCtx.pooledCnn->connection();
		KSQLResultSet& m_sqlRs = m_sqlCtx.rs;

		KCacheObject cacheObj;
		cacheObj.Attach(this, hRec);

		int begField = m_tableSchema->GetPrimaryKeyFieldCount();
		int fcount = m_tableSchema->GetFieldCount();

		for(int i=0; i<begField; i++)
		{
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			KSQLFieldDescribe& describe = m_sqlParams[i];

			describe.m_name = pfld->m_name;
			describe.m_length = pfld->m_length;
			describe.m_cdt = pfld->m_cdt;
		}

		if(!m_sqlCmd.SetSQLStatement(m_sqlDelete.c_str()))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		if(!m_sqlCmd.DescribeParameters(m_sqlParams, begField))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		if(!this->SetSQLParam(m_sqlCmd, 0, cacheObj))
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		if(!m_sqlCmd.Execute())
		{
			Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
			return FALSE;
		}

		return TRUE;
	}

	BOOL TableCacheUsePool::_fillDefaults(HANDLE hRec)
	{
		char cTmp[64];
		KRecordHead* pHead = this->_GetRecordHead((uint_r)hRec);
		int fieldCount = m_tableSchema->GetFieldCount();

		KCacheObject cacheObj;
		cacheObj.Attach(this, hRec);

		int begField = m_tableSchema->GetPrimaryKeyFieldCount();

		// 设置缺省值
		for(int i=begField; i<fieldCount; i++)
		{
			int paramIdx = i;
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			const System::Collections::KString<32>& defVal = pfld->m_defaultVal;

			switch(pfld->m_cdt)
			{
			case KSQLTypes::sql_c_int8:
			case KSQLTypes::sql_c_uint8:
			case KSQLTypes::sql_c_int16:
			case KSQLTypes::sql_c_uint16:
			case KSQLTypes::sql_c_int32:
			case KSQLTypes::sql_c_uint32:
			case KSQLTypes::sql_c_int64:
			case KSQLTypes::sql_c_uint64:
			case KSQLTypes::sql_c_float32:
			case KSQLTypes::sql_c_float64:
				{
					if(defVal.size() < 1)
					{
						cacheObj.SetInt64(i, 0);
					}
					else
					{
						INT64 val = str2i64(defVal.c_str());
						cacheObj.SetInt64(i, val);
					}
					break;
				}
			case KSQLTypes::sql_c_timestamp:
				{
					if(defVal.size() < 1)
					{
						cacheObj.SetInt64(i, time(NULL)); // 使用当前时间
					}
					else
					{
						char cDef[128]; strcpy_k(cDef, sizeof(cDef), defVal.c_str());
						char* pc = _trim(cDef);

						KDatetime datetime;
						if(pc[0] >= '0' && pc[0] <= '9')
						{
							int len = (int)strlen(cTmp);
							if(len < 9)
							{// 没有时间部分
								strcat(pc, " 0:0:0");
							}
							if(!datetime.SetDatetimeString(pc))
							{
								return FALSE;
							}
						}
						else
						{
							if(_strnicmp(pc, "now()", 3) && _strnicmp(pc, "current_date()", 12))
							{
								return FALSE;
							}
						}
						cacheObj.SetInt64(i, datetime.Time());
					}
					break;
				}
			case KSQLTypes::sql_c_string:
			case KSQLTypes::sql_c_varstring:
			case KSQLTypes::sql_c_binary:
				//case KSQLTypes::sql_c_varbinary:
				{
					if(defVal.size() < 1)
					{
						this->_setFieldData(hRec, i, NULL, 0);
					}
					else
					{
						cacheObj.SetBinary(i, defVal.c_str(), (int)defVal.size());
					}
					break;
				}
			}
		}

		return TRUE;
	}

	BOOL TableCacheUsePool::RealInsertRecord(HANDLE hRec)
	{
		KAutoThreadMutex mx(m_mx);
		KRecordHead* pHead = this->GetRecordHead(hRec);

		// 如果该记录的状态是正在改变，或者为发生改变，什么都不干
		if(pHead->IsStatus(c_updating) || !pHead->IsStatus(c_new_record))
		{
			return TRUE;
		}

		pHead->SetStatusBit(c_saving);

		if(!this->_InsertRecord(hRec, TRUE))
		{
			pHead->ClearStatusBit(c_saving);
			return FALSE;
		}

		pHead->ClearStatusBit(c_new_record);
		pHead->ClearStatusBit(c_saving);

		return TRUE;
	}

	BOOL TableCacheUsePool::RealUpdateRecord(HANDLE hRec)
	{
		// 分配和释放以及数据库操作会在多个线程中被调用，所以需要保护
		KAutoThreadMutex mx(m_mx);

		KRecordHead* pHead = this->GetRecordHead(hRec);

		// 如果该记录的状态是正在改变，或者为发生改变，什么都不干
		if(pHead->IsStatus(c_updating) || !pHead->IsStatus(c_modified))
		{
			return TRUE;
		}

		pHead->SetStatusBit(c_saving);

		if(!this->_UpdateRecord(hRec))
		{
			pHead->ClearStatusBit(c_saving);
			return FALSE;
		}

		pHead->ClearStatusBit(c_saving);
		return TRUE;
	}

	BOOL TableCacheUsePool::RealPartialUpdateRecord(HANDLE hRec)
	{
		// 分配和释放以及数据库操作会在多个线程中被调用，所以需要保护
		KAutoThreadMutex mx(m_mx);

		KRecordHead* pHead = this->GetRecordHead(hRec);

		// 如果该记录的状态是正在改变，什么都不干
		if(pHead->IsStatus(c_updating))
			return TRUE;

		if(pHead->IsStatus(c_modified))
		{	// 做全部Update的处理
			pHead->SetStatusBit(c_saving);
			if(!this->_UpdateRecord(hRec))
			{
				pHead->ClearStatusBit(c_saving);
				return FALSE;
			}
			pHead->ClearStatusBit(c_saving);
			return TRUE;
		}

		if(!pHead->IsStatus(c_partial_modified))
			return TRUE;

		pHead->SetStatusBit(c_saving);

		if(!this->_PartialUpdateRecord(hRec))
		{
			pHead->ClearStatusBit(c_saving);
			return FALSE;
		}

		pHead->ClearStatusBit(c_saving);
		return TRUE;
	}

	BOOL TableCacheUsePool::RealDeleteRecord(HANDLE hRec)
	{
		int pknum = m_tableSchema->GetPrimaryKeyFieldCount();

		// 分配和释放以及数据库操作会在多个线程中被调用，所以需要保护
		KAutoThreadMutex mx(m_mx);

		KRecordHead* pHead = this->GetRecordHead(hRec);

		// 如果该记录的状态不是已经删除，什么都不干
		if(!pHead)
		{
			Log(LOG_ERROR, "error: table %s delete record:%u, invlaid record", m_tableSchema->m_name.c_str(), hRec);
			return TRUE;
		}

		if(!pHead->IsStatus(c_deleted))
			return TRUE;

		pHead->SetStatusBit(c_saving);
		if(!this->_DeleteRecord(hRec))
		{
			Log(LOG_ERROR, "error: table %s delete record %u", m_tableSchema->m_name.c_str(), hRec);
		}

		this->on_unloadRecord(hRec);
		this->_FreeRecord(hRec);
		return TRUE;
	}

	BOOL TableCacheUsePool::RealCheckoutRecord(HANDLE hRec)
	{
		int pknum = m_tableSchema->GetPrimaryKeyFieldCount();

		// 分配和释放以及数据库操作会在多个线程中被调用，所以需要保护
		KAutoThreadMutex mx(m_mx);

		KRecordHead* pHead = this->GetRecordHead(hRec);
		if(!pHead)
		{
			Log(LOG_ERROR, "error: RealCheckoutRecord table:%s record:%u", m_tableSchema->m_name.c_str(), hRec);
			return FALSE;
		}

		KCacheObject cacheObj;
		cacheObj.Attach(this, hRec);

		pHead->SetStatusBit(c_saving);

		if(pHead->IsStatus(c_new_record))
		{
			if(!this->_InsertRecord(hRec, TRUE))
			{
				Log(LOG_ERROR, "error: table:%s insert record:%u", m_tableSchema->m_name.c_str(), hRec);
				this->on_unloadRecord(hRec);
				this->_FreeRecord(hRec);
				return FALSE;
			}
		}

		if(pHead->IsStatus(c_modified))
		{
			if(!this->_UpdateRecord(hRec))
			{
				Log(LOG_ERROR, "error: table:%s update record:%u", m_tableSchema->m_name.c_str(), hRec);
			}
		}
		if(pHead->IsStatus(c_partial_modified))
		{
			if(!this->_PartialUpdateRecord(hRec))
			{
				Log(LOG_ERROR, "error: table:%s partial update record:%u", m_tableSchema->m_name.c_str(), hRec);
			}
		}

		this->on_unloadRecord(hRec);
		this->_FreeRecord(hRec);

		return TRUE;
	}

	//void TableCacheUsePool::ClearSqlError()
	//{
	//	KAutoThreadMutex mx(m_mx);
	//	m_sqlCnn.ClearError();
	//}

	//DWORD TableCacheUsePool::GetLastSqlError() const
	//{
	//	return m_sqlCnn.GetLastError();
	//}

	//const char* TableCacheUsePool::GetLastSqlErrorMessage() const
	//{
	//	return m_sqlCnn.GetLastErrorMsg();
	//}

	//BOOL TableCacheUsePool::isConnActive()
	//{
	//	KAutoThreadMutex mx(m_mx);
	//	m_sqlCmd.Reset();
	//	return m_sqlCnn.IsActive();
	//}

	//BOOL TableCacheUsePool::reconnect()
	//{
	//	KAutoThreadMutex mx(m_mx);
	//	return m_sqlCnn.Reconnect();
	//}

	//BOOL TableCacheUsePool::keepConnectionFresh()
	//{
	//	KAutoThreadMutex mx(m_mx);
	//	return m_sqlCnn.KeepConnectionFresh();
	//}

	//BOOL TableCacheUsePool::QueuePush(KEmergencyEvent& event)
	//{
	//	KAutoThreadMutex mx(m_mx);
	//	return m_emergencyQueue.push(event);
	//}

	//BOOL TableCacheUsePool::QueuePop(KEmergencyEvent& event)
	//{
	//	KAutoThreadMutex mx(m_mx);
	//	return m_emergencyQueue.pop(event);
	//}

	//BOOL TableCacheUsePool::QueueWait(int ms)
	//{
	//	return m_emergencyQueue.wait(ms);
	//}

	//void TableCacheUsePool::OnEmergencyFinished(BOOL success, void* pUserData)
	//{
	//	if(m_pEmergencyObserver)
	//	{
	//		m_pEmergencyObserver->OnEmergencyFinished(success, pUserData);
	//	}
	//}

	BOOL TableCacheUsePool::_CreateInsertSql()
	{
		m_sqlInsert = "Insert into ";
		m_sqlInsert.append(m_tableSchema->m_dbName.c_str()).append('.').append(m_tableSchema->m_name.c_str());
		m_sqlInsert.append('(');
		for(int i=0; i<m_tableSchema->GetFieldCount(); i++)
		{
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			if(i) m_sqlInsert.append(',');
			m_sqlInsert.append(pfld->m_name.c_str());
		}
		m_sqlInsert.append(')');
		m_sqlInsert.append(" Values(");
		for(int i=0; i<m_tableSchema->GetFieldCount(); i++)
		{
			if(i) m_sqlInsert.append(',');
			m_sqlInsert.append('?');
		}
		m_sqlInsert.append(')');
		return TRUE;
	}

	BOOL TableCacheUsePool::_CreateUpdateSql()
	{
		int begField = m_tableSchema->GetPrimaryKeyFieldCount();
		int fcount = m_tableSchema->GetFieldCount();

		m_sqlUpdate = "Update ";
		m_sqlUpdate.append(m_tableSchema->m_dbName.c_str()).append('.').append(m_tableSchema->m_name.c_str());
		m_sqlUpdate.append(" set ");

		for(int i=begField; i<m_tableSchema->GetFieldCount(); i++)
		{
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			if(i - begField) m_sqlUpdate.append(',');
			m_sqlUpdate.append(pfld->m_name.c_str()).append("=?");
		}
		m_sqlUpdate.append(" Where ");

		for(int i=0; i<begField; i++)
		{
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			if(i) m_sqlUpdate.append(" and ");
			m_sqlUpdate.append(pfld->m_name.c_str());
			m_sqlUpdate.append("=?");
		}

		return TRUE;
	}

	int TableCacheUsePool::_CreatePartialUpdateSQL(HANDLE hRec)
	{
		int paramCount = 0;
		int begField = m_tableSchema->GetPrimaryKeyFieldCount();
		int fieldCount = m_tableSchema->GetFieldCount();

		KRecordHead* pHead = this->_GetRecordHead((DWORD)(uint_r)hRec);
		KBitset bitset(pHead+1, fieldCount);

		m_sqlPartialUpdate = "Update ";
		m_sqlPartialUpdate.append(m_tableSchema->m_dbName.c_str()).append('.').append(m_tableSchema->m_name.c_str());
		m_sqlPartialUpdate.append(" set ");

		for(int i=begField; i<m_tableSchema->GetFieldCount(); i++)
		{
			if(!bitset.GetBit(i)) continue;
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			if(paramCount) m_sqlPartialUpdate.append(',');
			m_sqlPartialUpdate.append(pfld->m_name.c_str()).append("=?");
			paramCount += 1;
		}

		if(!paramCount) // 没有找到修改过的字段，应该是出错了
			return 0;

		m_sqlPartialUpdate.append(" Where ");
		for(int i=0; i<begField; i++)
		{	
			if(i) m_sqlPartialUpdate.append(" and ");
			m_sqlPartialUpdate.append(m_tableSchema->GetField(i)->m_name.c_str());
			m_sqlPartialUpdate.append("=?");
			paramCount += 1;
		}

		return paramCount;
	}

	BOOL TableCacheUsePool::_CreateDeleteSql()
	{
		m_sqlDelete = "Delete from ";
		m_sqlDelete.append(m_tableSchema->m_dbName.c_str()).append('.').append(m_tableSchema->m_name.c_str());
		m_sqlDelete.append(" Where ");

		int begField = m_tableSchema->GetPrimaryKeyFieldCount();
		for(int i=0; i<begField; i++)
		{
			if(i) m_sqlDelete.append(" and ");
			m_sqlDelete.append(m_tableSchema->GetField(i)->m_name.c_str());
			m_sqlDelete.append("=?");
		}

		return TRUE;
	}

	BOOL TableCacheUsePool::_CreateSelectSql()
	{
		int begField = m_tableSchema->GetPrimaryKeyFieldCount();
		int fcount = m_tableSchema->GetFieldCount();

		m_sqlSelect = "Select ";
		for(int i=begField; i<m_tableSchema->GetFieldCount(); i++)
		{
			if(i - begField) m_sqlSelect.append(',');
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			m_sqlSelect.append(pfld->m_name.c_str());

		}

		m_sqlSelect.append(" From ");
		m_sqlSelect.append(m_tableSchema->m_dbName.c_str()).append('.').append(m_tableSchema->m_name.c_str());
		m_sqlSelect.append(" Where ");

		for(int i=0; i<begField; i++)
		{
			if(i) m_sqlSelect.append(" and ");
			const KSQLTableField* pfld = m_tableSchema->GetField(i);
			m_sqlSelect.append(pfld->m_name.c_str());
			m_sqlSelect.append("=?");
		}

		return TRUE;
	}

	HANDLE TableCacheUsePool::_AllocRecord()
	{
		HANDLE hRec = (HANDLE)invalid_hrec;
		if(m_pFileHead->firstFree != invalid_recno)
		{
			hRec = this->_AllocFromFree();
		}
		else
		{
			hRec = this->_AllocFromBottom();
		}
		if(!this->_IsValidHandle(hRec))
		{
			// 记日志
			return (HANDLE)invalid_hrec;
		}

		KRecordHead* pHead = this->_GetRecordHead((DWORD)(uint_r)hRec);
		this->_initRecord(pHead);

		return hRec;
	}

	HANDLE TableCacheUsePool::_AllocFromBottom()
	{
		DWORD recNo = m_pFileHead->usedCount;
		if(recNo >= m_pFileHead->capacity) return (HANDLE)invalid_hrec;

		HANDLE hRec = (HANDLE)recNo;
		KRecordHead* pHead = this->_GetRecordHead(recNo);
		pHead->SetUsed(0);
		pHead->ClearStatus();
		m_pFileHead->usedCount += 1;

		return hRec;
	}

	HANDLE TableCacheUsePool::_AllocFromFree()
	{
		DWORD recNo = m_pFileHead->firstFree;
		HANDLE hRec = (HANDLE)recNo;

		KRecordHead* pHead = this->_GetRecordHead(recNo);
		if(!pHead->IsFree())
		{
			Log(LOG_ERROR, "error: %s invalid cache status, free list point to an used record %u", this->GetTableName(), recNo);
			return (HANDLE)invalid_hrec;
		}

		m_pFileHead->firstFree = pHead->GetNextNo();
		pHead->SetUsed(0);
		pHead->ClearStatus();

		return hRec;
	}

	void TableCacheUsePool::_FreeRecord(HANDLE hRec)
	{
		KRecordHead* pHead = this->_GetRecordHead((DWORD)(uint_r)hRec);
		if(pHead->IsFree())
		{
			Log(LOG_ERROR, "error: table:%s free a un-used record %x", m_tableSchema->m_name.c_str(), hRec);
			return;
		}

		pHead->SetFree(m_pFileHead->firstFree);
		m_pFileHead->firstFree = (DWORD)(uint_r)hRec;
	}

	void TableCacheUsePool::_initRecord(KRecordHead* pHead)
	{
		memset(pHead+1, 0, m_recordDecl.m_recSize-sizeof(KRecordHead));
	}

} }
