#include "KTableCache.h"
#include "KCacheObject.h"
#include <System/Misc/StrUtil.h>
#include <System/Misc/KBitset.h>
#include "KTableCacheCreator.h"

using namespace System::Sync;

KTableCache::KRefreshThread::KRefreshThread(KTableCache& cache)
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

KTableCache::KRefreshThread::~KRefreshThread()
{
	this->Stop();
}

////////////////////

KTableCache* KTableCache::CreateInstance(KSQLTableSchema& schema, const KSQLConnParam& cnnParam, const CreateParam& createParam)
{
	TableCacheSetting setting;
	setting.m_capacity = createParam.capacity;
	setting.m_flushInterval = createParam.flushInterval;
	setting.m_tbname = schema.m_name.c_str();
	setting.m_mmapfile = createParam.mmapfile;
	setting.m_schema = schema;
	setting._nextTaskSlot = 0;
	setting._connParam = &cnnParam;
	KTraditionalCacheCreator creator;
	return (KTableCache*)creator.Create(setting);
}

void KTableCache::KRefreshThread::Execute()
{
	TIME_T now = (TIME_T)time(NULL);
	TIME_T last_chk_cnn = now;
	const int chk_cnn_interval = 60; // second

	while(!m_stopDesired)
	{
		// 错误以及超过60秒检查重连机制
		if(m_cache.GetLastSqlError())
		{
			m_cache.ClearSqlError(); // 清空错误信息
			if(!m_cache.isConnActive())
			{
				if(m_cache.reconnect())
				{
					Log(LOG_WARN, "warn: re-connect to database for table cache %s.", m_cache.GetTableName());
					m_lastRefreshCheckOut = 0;
					m_lastRefreshDelete = 0;
					m_lastRefreshModify = 0;
				}
				else
				{
					Log(LOG_WARN, "warn: fail to re-connected to database for table cache %s.", m_cache.GetTableName());
					::Sleep(2000); // 等待2秒再重连
					continue;
				}
			}
		}
		else
		{
			now = (TIME_T)time(NULL);
			if(now - last_chk_cnn >= chk_cnn_interval)
			{
				last_chk_cnn = now;
				if(!m_cache.isConnActive())
				{
					if(m_cache.reconnect())
					{
						Log(LOG_WARN, "warn: re-connect to database for table cache %s.", m_cache.GetTableName());
						m_lastRefreshCheckOut = 0;
						m_lastRefreshDelete = 0;
						m_lastRefreshModify = 0;
					}
					else
					{
						Log(LOG_WARN, "warn: fail to re-connected to database for table cache %s.", m_cache.GetTableName());
						::Sleep(2000); // 等待2秒再重连
						continue;
					}
				}
			}
		}

		if(!m_cache.isConnActive())
		{
			if(m_cache.reconnect())
			{
				// 刚刚连上数据库
				Log(LOG_WARN, "warn: connected to database, refresh %s cache.", m_cache.GetTableName());
				m_lastRefreshCheckOut = 0;
				m_lastRefreshDelete = 0;
				m_lastRefreshModify = 0;
			}
			else
			{
				// 连不上数据库，等待2秒后重试
				Log(LOG_WARN, "warn: fail to re-connected to database, %s.", m_cache.GetTableName());
				::Sleep(2000);
				continue;
			}
		}

		DWORD recCount = m_cache.m_pFileHead->usedCount;

		// 先处理紧急队列
		KEmergencyEvent emergencyEvent;
		while(m_cache.QueuePop(emergencyEvent))
		{
			HANDLE hEmergency = emergencyEvent.hRec;
			KTableCache::KRecordHead* pRecHead = m_cache._GetRecordHead((DWORD)(uint_r)hEmergency);

			// 如果是已删除的记录，那么在第一时间处理，以免对修改做处理，浪费时间
			if(pRecHead->IsStatus(KTableCache::c_deleted))
			{
				if(!m_cache.RealDeleteRecord(hEmergency))
				{
					m_cache.OnEmergencyFinished(FALSE, emergencyEvent.pUserData);
					Log(LOG_WARN, "warn: refresh thread fail to delete %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hEmergency);
					if(!m_cache.isConnActive()) break;
				}
				else
				{
					m_cache.OnEmergencyFinished(TRUE, emergencyEvent.pUserData);
				}
				continue;
			}

			// 最常见的情况放到前面
			if(pRecHead->IsStatus(KTableCache::c_checkout))
			{
				if(!m_cache.RealCheckoutRecord(hEmergency))
				{
					m_cache.OnEmergencyFinished(FALSE, emergencyEvent.pUserData);
					Log(LOG_WARN, "warn: refresh thread fail to checkout %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hEmergency);
					if(!m_cache.isConnActive()) break;
				}
				else
				{
					m_cache.OnEmergencyFinished(TRUE, emergencyEvent.pUserData);
				}
				continue;
			}

			// 增加记录不会是最后一个操作，后面肯定有更新操作
			// 这个操作放在前面是时序的要求
			if(pRecHead->IsStatus(KTableCache::c_new_record))
			{
				if(!m_cache.RealInsertRecord(hEmergency))
				{
					m_cache.OnEmergencyFinished(FALSE, emergencyEvent.pUserData);
					Log(LOG_WARN, "warn: refresh thread fail to insert %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hEmergency);
					if(!m_cache.isConnActive()) break;
					continue;
				}
			}

			if(pRecHead->IsStatus(KTableCache::c_modified))
			{
				if(!m_cache.RealUpdateRecord(hEmergency))
				{
					m_cache.OnEmergencyFinished(FALSE, emergencyEvent.pUserData);
					Log(LOG_WARN, "warn: refresh thread fail to update %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hEmergency);
					if(!m_cache.isConnActive()) break;
				}
				else
				{
					m_cache.OnEmergencyFinished(TRUE, emergencyEvent.pUserData);
				}
				continue;
			}
			if(pRecHead->IsStatus(KTableCache::c_partial_modified))
			{
				if(!m_cache.RealPartialUpdateRecord(hEmergency))
				{
					m_cache.OnEmergencyFinished(FALSE, emergencyEvent.pUserData);
					Log(LOG_WARN, "warn: refresh thread fail to partial update %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hEmergency);
					if(!m_cache.isConnActive()) break;
				}
				else
				{
					m_cache.OnEmergencyFinished(TRUE, emergencyEvent.pUserData);
				}
				continue;
			}

			// 应该是正常流程处理了，这里算成功
			m_cache.OnEmergencyFinished(TRUE, emergencyEvent.pUserData);
		}

		// 第一遍是Checkout
		if(now >= m_lastRefreshCheckOut + m_refreshCheckoutInterval)
		{
			for(DWORD i=0; i<recCount && !m_stopDesired; i++)
			{
				HANDLE hRec = (HANDLE)i;
				KTableCache::KRecordHead* pRecHead = m_cache._GetRecordHead(i);

				if(pRecHead->IsFree()) continue;
				if(pRecHead->IsStatus(KTableCache::c_new_created)) continue;
				if(pRecHead->IsStatus(KTableCache::c_updating)) continue;
				if(pRecHead->IsStatus(KTableCache::c_loading)) continue;

				// 只有自己的这个进程才会把记录的状态设置为Saving，所以这里是出错了
				if(pRecHead->IsStatus(KTableCache::c_saving))
				{
					Log(LOG_WARN, "warn: refresh thread saving state %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
					pRecHead->ClearStatusBit(KTableCache::c_saving);
				}
				if(pRecHead->IsStatus(KTableCache::c_checkout))
				{
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
				KTableCache::KRecordHead* pRecHead = m_cache._GetRecordHead(i);

				if(pRecHead->IsFree()) continue;
				if(pRecHead->IsStatus(KTableCache::c_new_created)) continue;
				if(pRecHead->IsStatus(KTableCache::c_updating)) continue;
				if(pRecHead->IsStatus(KTableCache::c_loading)) continue;

				// 只有自己的这个进程才会把记录的状态设置为Saving，所以这里是出错了
				if(pRecHead->IsStatus(KTableCache::c_saving))
				{
					Log(LOG_WARN, "warn: refresh thread saving state %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
					pRecHead->ClearStatusBit(KTableCache::c_saving);
				}

				if(pRecHead->IsStatus(KTableCache::c_deleted))
				{
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
				KTableCache::KRecordHead* pRecHead = m_cache._GetRecordHead(i);

				if(pRecHead->IsFree()) continue;
				if(pRecHead->IsStatus(KTableCache::c_new_created)) continue;
				if(pRecHead->IsStatus(KTableCache::c_updating)) continue;
				if(pRecHead->IsStatus(KTableCache::c_loading)) continue;

				// 只有自己的这个进程才会把记录的状态设置为Saving，所以这里是出错了
				if(pRecHead->IsStatus(KTableCache::c_saving))
				{
					Log(LOG_WARN, "warn: refresh thread saving state %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
					pRecHead->ClearStatusBit(KTableCache::c_saving);
				}

				if(pRecHead->IsStatus(KTableCache::c_new_record))
				{
					if(!m_cache.RealInsertRecord(hRec))
					{
						Log(LOG_WARN, "warn: refresh thread insert %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
					}
				}

				if(pRecHead->IsStatus(KTableCache::c_modified))
				{
					if(!m_cache.RealUpdateRecord(hRec))
					{
						Log(LOG_WARN, "warn: refresh thread update %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
					}
					continue;
				}

				if(pRecHead->IsStatus(KTableCache::c_partial_modified))
				{
					if(!m_cache.RealPartialUpdateRecord(hRec))
					{
						Log(LOG_WARN, "warn: refresh thread partial update %s.0x%08X", m_cache.GetTableName(), (int)(uint_r)hRec);
					}
					continue;
				}
			}
			m_lastRefreshModify = (TIME_T)time(NULL);
		}

		m_cache.QueueWait(500);
	}
}

void KTableCache::KRefreshThread::Stop()
{
	m_stopDesired = TRUE;
	if(!KThread::GetTerminated())
	{
		KThread::WaitFor();
	}
	m_stopDesired = FALSE;
}


// KTableCacheBase

KTableCache::KTableCache()
	: m_refreshThread(*this)
	, m_sqlCmd(m_sqlCnn)
{
	m_sqlParams = NULL;
	m_pEmergencyObserver = NULL;
}

KTableCache::~KTableCache()
{
	//this->Finalize();
}

void KTableCache::SetEmergencyObserver(IEmergencyObserver* pObserver)
{
	m_pEmergencyObserver = pObserver;
}

BOOL KTableCache::Initialize(const KSQLConnParam& cnnParam, KSQLTableSchema& schema, const CreateParam& createParam)
{
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

	m_sqlCnn.SetAutoReconnect(TRUE);
	if(!m_sqlCnn.Connect(cnnParam))
	{
		Log(LOG_ERROR, "error: connect to database, %s", m_sqlCnn.GetLastErrorMsg());
		return FALSE;
	}

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

		if(pRecHead->IsStatus(c_deleted))
		{
			this->RealDeleteRecord(hRec);
		}
		else
		{
			if(pRecHead->IsStatus(c_new_record))
			{
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

void KTableCache::Finalize()
{
	m_refreshThread.Stop();
	KTableCacheReader::finalize();
	if(m_sqlParams)
	{
		delete [] m_sqlParams;
		m_sqlParams = NULL;
	}
}

void KTableCache::finalize()
{
	this->Finalize();
}

BOOL KTableCache::_IsValidHandle(HANDLE hRec)
{
	return (uint_r)hRec != invalid_hrec;
}

void KTableCache::_markAllPartialModified(HANDLE hRec)
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

int KTableCache::_getPartialModifiedCount(HANDLE hRec)
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

HANDLE KTableCache::FindRecord(const void* pKey, int len)
{
	ASSERT(this->_checkKeyLength(len));
	KAutoThreadMutex mx(m_mx);
	return this->_findRecord(pKey);
}

HANDLE KTableCache::LoadRecord(const void* pKey, int len)
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

HANDLE KTableCache::CreateRecord(const void* pKey, int len)
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

BOOL KTableCache::DiscardRecord(const void* pKey, int len)
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

BOOL KTableCache::DeleteRecord(const void* pKey, int len)
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

BOOL KTableCache::CheckoutRecord(const void* pKey, int len)
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

BOOL KTableCache::EmergencyCheckout(const void* pKey, int len, void* pUserData)
{
	ASSERT(this->_checkKeyLength(len));

	KAutoThreadMutex mx(m_mx);

	HANDLE hRec = this->_findRecord(pKey);
	if(!this->_IsValidHandle(hRec)) return FALSE;

	KRecordHead* pHead = this->GetRecordHead(hRec);
	if(!pHead)
	{
		this->OnEmergencyFinished(FALSE, pUserData);
		return FALSE;
	}

	pHead->m_used.version += 1;
	pHead->SetStatusBit(c_checkout);

	KEmergencyEvent event = {hRec, pUserData};
	m_emergencyQueue.push(event);

	return TRUE;
}

BOOL KTableCache::EmergencyCheckout(HANDLE hRec, void* pUserData)
{
	KAutoThreadMutex mx(m_mx);
	if(!this->_IsValidHandle(hRec)) return FALSE;

	KRecordHead* pHead = this->GetRecordHead(hRec);
	if(!pHead)
	{
		this->OnEmergencyFinished(FALSE, pUserData);
		return FALSE;
	}

	pHead->m_used.version += 1;
	pHead->SetStatusBit(c_checkout);

	KEmergencyEvent event = {hRec, pUserData};
	m_emergencyQueue.push(event);

	return TRUE;
}

BOOL KTableCache::DiscardRecord2(HANDLE hRec)
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

BOOL KTableCache::DeleteRecord2(HANDLE hRec)
{
	KAutoThreadMutex mx(m_mx);

	KRecordHead* pHead = this->GetRecordHead(hRec);
	if(!pHead) return FALSE;

	pHead->m_used.version += 1;
	pHead->SetStatusBit(c_deleted);
	return TRUE;
}

BOOL KTableCache::CheckoutRecord2(HANDLE hRec)
{
	KAutoThreadMutex mx(m_mx);

	KRecordHead* pHead = this->GetRecordHead(hRec);
	if(!pHead) return FALSE;

	pHead->m_used.version += 1;
	pHead->SetStatusBit(c_checkout);

	return TRUE;
}

HANDLE KTableCache::CreateRecordDirect()
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
	KCacheObject cacheObj;
	cacheObj.Attach(this, hRec);
	UINT64 lastId = m_sqlCnn.GetLastAutoIncID();
	this->SetAutoIncreaseKey(cacheObj, lastId);

	this->on_recordLoaded(hRec);
	pHead->ClearStatusBit(c_saving);

	return hRec;
}

HANDLE KTableCache::CreateRecordDirect(void* pKey, int len)
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

BOOL KTableCache::_LoadRecord(HANDLE hRec)
{
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
		case KSQLTypes::sql_c_timestamp:
			{
				MYSQL_TIME_K& timestamp = *(MYSQL_TIME_K*)pData;
				if(!m_sqlRs.GetTimestamp(colIdx, timestamp))
				{
					Log(LOG_ERROR, "error: sql: %s", m_sqlRs.GetLastErrorMsg());
					return FALSE;
				}
			} break;
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

BOOL KTableCache::_InsertRecord(HANDLE hRec, BOOL withKey)
{
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
					strcpy(cDef, defVal.c_str());
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
					if(!m_sqlCmd.SetTimestamp(paramIdx, datetime.Time()))
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

BOOL KTableCache::_UpdateRecord(HANDLE hRec)
{
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
		case KSQLTypes::sql_c_timestamp:
			{
				MYSQL_TIME_K timestamp = *(MYSQL_TIME_K*)pData;
				if(!m_sqlCmd.SetTimestamp(paramIdx, timestamp))
				{
					Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
					return FALSE;
				}
				break;
			}
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

BOOL KTableCache::_PartialUpdateRecord(HANDLE hRec)
{
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
		case KSQLTypes::sql_c_timestamp:
			{
				MYSQL_TIME_K timestamp = *(MYSQL_TIME_K*)pData;
				if(!m_sqlCmd.SetTimestamp(paramIdx, timestamp))
				{
					Log(LOG_DEBUG, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
					return FALSE;
				}
				break;
			}
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

BOOL KTableCache::_DeleteRecord(HANDLE hRec)
{
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

BOOL KTableCache::_fillDefaults(HANDLE hRec)
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
					char cDef[128]; strcpy(cDef, defVal.c_str());
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
					cacheObj.SetTimestamp(i, datetime.Time());
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

BOOL KTableCache::RealInsertRecord(HANDLE hRec)
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

BOOL KTableCache::RealUpdateRecord(HANDLE hRec)
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

BOOL KTableCache::RealPartialUpdateRecord(HANDLE hRec)
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

BOOL KTableCache::RealDeleteRecord(HANDLE hRec)
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

	//KCacheObject cacheObj;
	//cacheObj.Attach(this, hRec);

	this->on_unloadRecord(hRec);
	this->_FreeRecord(hRec);

	return TRUE;
}

BOOL KTableCache::RealCheckoutRecord(HANDLE hRec)
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

void KTableCache::ClearSqlError()
{
	KAutoThreadMutex mx(m_mx);
	m_sqlCnn.ClearError();
}

DWORD KTableCache::GetLastSqlError() const
{
	return m_sqlCnn.GetLastError();
}

const char* KTableCache::GetLastSqlErrorMessage() const
{
	return m_sqlCnn.GetLastErrorMsg();
}

BOOL KTableCache::isConnActive()
{
	KAutoThreadMutex mx(m_mx);
	m_sqlCmd.Reset();
	return m_sqlCnn.IsActive();
}

BOOL KTableCache::reconnect()
{
	KAutoThreadMutex mx(m_mx);
	return m_sqlCnn.Reconnect();
}

BOOL KTableCache::keepConnectionFresh()
{
	KAutoThreadMutex mx(m_mx);
	return m_sqlCnn.KeepConnectionFresh();
}

BOOL KTableCache::QueuePush(KEmergencyEvent& event)
{
	KAutoThreadMutex mx(m_mx);
	return m_emergencyQueue.push(event);
}

BOOL KTableCache::QueuePop(KEmergencyEvent& event)
{
	KAutoThreadMutex mx(m_mx);
	return m_emergencyQueue.pop(event);
}

BOOL KTableCache::QueueWait(int ms)
{
	return m_emergencyQueue.wait(ms);
}

void KTableCache::OnEmergencyFinished(BOOL success, void* pUserData)
{
	if(m_pEmergencyObserver)
	{
		m_pEmergencyObserver->OnEmergencyFinished(success, pUserData);
	}
}

BOOL KTableCache::_CreateInsertSql()
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

BOOL KTableCache::_CreateUpdateSql()
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

int KTableCache::_CreatePartialUpdateSQL(HANDLE hRec)
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

BOOL KTableCache::_CreateDeleteSql()
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

BOOL KTableCache::_CreateSelectSql()
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

HANDLE KTableCache::_AllocRecord()
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

HANDLE KTableCache::_AllocFromBottom()
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

HANDLE KTableCache::_AllocFromFree()
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

void KTableCache::_FreeRecord(HANDLE hRec)
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

void KTableCache::_initRecord(KRecordHead* pHead)
{
	memset(pHead+1, 0, m_recordDecl.m_recSize-sizeof(KRecordHead));
}