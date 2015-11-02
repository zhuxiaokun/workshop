#include "KSQLTableCache_X_Thread.h"
#include "KTableCache.h"
#include <System/Log/log.h>
#include <System/Misc/KBitset.h>
#include "KCacheObject.h"
#include <System/Misc/StrUtil.h>

typedef KTableCache::KRecordHead KRecordHead;

KSQLTableCache_X_Thread::KSQLTableCache_X_Thread(KTableCache_X& tableCache, const KSQLConnParam& connParam)
	: KTableCache_X_Thread(tableCache)
	, m_sqlParams(NULL)
	, m_connParam(connParam)
	, m_cnnReady(FALSE)
	, m_lastTryConnect(0)
{
	int fieldCount = tableCache.m_tableSchema->GetFieldCount();
	m_sqlParams = new KSQLFieldDescribe[fieldCount];
}

KSQLTableCache_X_Thread::~KSQLTableCache_X_Thread()
{
	if(m_sqlParams)
	{
		delete [] m_sqlParams;
		m_sqlParams = NULL;
	}
}

void KSQLTableCache_X_Thread::Execute()
{
	if(!m_sqlCnn.Connect(m_connParam))
	{
		Log(LOG_ERROR, "error: conect to database, %s", m_sqlCnn.GetLastErrorMsg());
		return;
	}

	m_cnnReady = TRUE;

	m_sqlCmd.SetConnection(m_sqlCnn);
	this->_CreateSelectSql();
	this->_CreateUpdateSql();
	this->_CreateInsertSql();
	this->_CreateDeleteSql();

	// 由Task自己保证数据库连接是可用的
	KTableCache_X_Thread::Execute();
}

BOOL KSQLTableCache_X_Thread::_CreateSelectSql()
{
	KSQLTableSchema& m_schema = *m_tableCache->m_tableSchema;
	int begField = m_schema.GetPrimaryKeyFieldCount();
	int fcount = m_schema.GetFieldCount();

	m_sqlSelect = "Select ";
	for(int i=begField; i<m_schema.GetFieldCount(); i++)
	{
		if(i - begField) m_sqlSelect.append(',');
		const KSQLTableField* pfld = m_schema.GetField(i);
		m_sqlSelect.append(pfld->m_name.c_str());

	}

	m_sqlSelect.append(" From ");
	m_sqlSelect.append(m_schema.m_dbName.c_str()).append('.').append(m_schema.m_name.c_str());
	m_sqlSelect.append(" Where ");

	for(int i=0; i<begField; i++)
	{
		if(i) m_sqlSelect.append(" and ");
		const KSQLTableField* pfld = m_schema.GetField(i);
		m_sqlSelect.append(pfld->m_name.c_str());
		m_sqlSelect.append("=?");
	}

	return TRUE;
}

BOOL KSQLTableCache_X_Thread::_CreateUpdateSql()
{
	KSQLTableSchema& m_schema = *m_tableCache->m_tableSchema;
	int begField = m_schema.GetPrimaryKeyFieldCount();
	int fcount = m_schema.GetFieldCount();

	m_sqlUpdate = "Update ";
	m_sqlUpdate.append(m_schema.m_dbName.c_str()).append('.').append(m_schema.m_name.c_str());
	m_sqlUpdate.append(" set ");

	for(int i=begField; i<m_schema.GetFieldCount(); i++)
	{
		const KSQLTableField* pfld = m_schema.GetField(i);
		if(i - begField) m_sqlUpdate.append(',');
		m_sqlUpdate.append(pfld->m_name.c_str()).append("=?");
	}
	m_sqlUpdate.append(" Where ");

	for(int i=0; i<begField; i++)
	{
		const KSQLTableField* pfld = m_schema.GetField(i);
		if(i) m_sqlUpdate.append(" and ");
		m_sqlUpdate.append(pfld->m_name.c_str());
		m_sqlUpdate.append("=?");
	}

	return TRUE;
}

int_r KSQLTableCache_X_Thread::_CreatePartialUpdateSQL(HANDLE hRec)
{
	KSQLTableSchema& m_schema = *m_tableCache->m_tableSchema;
	int paramCount = 0;
	int begField = m_schema.GetPrimaryKeyFieldCount();
	int fieldCount = m_schema.GetFieldCount();

	KRecordHead* pHead = m_tableCache->_GetRecordHead((uint_r)hRec);
	KBitset bitset(pHead+1, fieldCount);

	m_sqlPartialUpdate = "Update ";
	m_sqlPartialUpdate.append(m_schema.m_dbName.c_str()).append('.').append(m_schema.m_name.c_str());
	m_sqlPartialUpdate.append(" set ");

	for(int i=begField; i<m_schema.GetFieldCount(); i++)
	{
		if(!bitset.GetBit(i)) continue;
		const KSQLTableField* pfld = m_schema.GetField(i);
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
		m_sqlPartialUpdate.append(m_schema.GetField(i)->m_name.c_str());
		m_sqlPartialUpdate.append("=?");
		paramCount += 1;
	}

	return paramCount;
}

BOOL KSQLTableCache_X_Thread::_CreateInsertSql()
{
	KSQLTableSchema& m_schema = *m_tableCache->m_tableSchema;
	m_sqlInsert = "Insert into ";
	m_sqlInsert.append(m_schema.m_dbName.c_str()).append('.').append(m_schema.m_name.c_str());
	m_sqlInsert.append('(');
	for(int i=0; i<m_schema.GetFieldCount(); i++)
	{
		const KSQLTableField* pfld = m_schema.GetField(i);
		if(i) m_sqlInsert.append(',');
		m_sqlInsert.append(pfld->m_name.c_str());
	}
	m_sqlInsert.append(')');
	m_sqlInsert.append(" Values(");
	for(int i=0; i<m_schema.GetFieldCount(); i++)
	{
		if(i) m_sqlInsert.append(',');
		m_sqlInsert.append('?');
	}
	m_sqlInsert.append(')');
	return TRUE;
}

BOOL KSQLTableCache_X_Thread::_CreateDeleteSql()
{
	KSQLTableSchema& m_schema = *m_tableCache->m_tableSchema;
	m_sqlDelete = "Delete from ";
	m_sqlDelete.append(m_schema.m_dbName.c_str()).append('.').append(m_schema.m_name.c_str());
	m_sqlDelete.append(" Where ");

	int begField = m_schema.GetPrimaryKeyFieldCount();
	for(int i=0; i<begField; i++)
	{
		if(i) m_sqlDelete.append(" and ");
		m_sqlDelete.append(m_schema.GetField(i)->m_name.c_str());
		m_sqlDelete.append("=?");
	}

	return TRUE;
}

////// 以msTimeout为间隔尝试重连数据库
BOOL KSQLTableCache_X_Thread::_assureConnection(int_r msTimeout)
{
	if(m_sqlCnn.IsActive()) return TRUE;
	m_cnnReady = FALSE;

	time_t now = time(NULL);
	if((now-m_lastTryConnect)*1000 < msTimeout)
		return FALSE;

	m_lastTryConnect = now;
	if(m_sqlCnn.Reconnect())
	{
		m_cnnReady = TRUE;
		Log(LOG_WARN, "warn: re-connect to database %s.%s OK!", m_connParam.m_host.c_str(), m_connParam.m_dbName.c_str());
		return TRUE;
	}

	Log(LOG_ERROR, "error: fail to connect to database, %s.%s", m_connParam.m_host.c_str(), m_connParam.m_dbName.c_str());
	return FALSE;
}

BOOL KSQLTableCache_X_Thread::load(HANDLE hRec, KTableRecordData& recData)
{
	ASSERT(m_tableCache->findRecord(recData) == INVALID_HREC);

	if(!m_tableCache->copyKey(hRec, recData))
		return FALSE;

	KSQLTableSchema& m_schema = *m_tableCache->m_tableSchema;
	int fieldCount = m_schema.GetFieldCount();
	int begField = m_schema.GetPrimaryKeyFieldCount();

	for(int i=0; i<begField; i++)
	{
		const KSQLTableField* pfld = m_schema.GetField(i);
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
	cacheObj.Attach(m_tableCache, hRec);

	if(!m_tableCache->SetSQLParam(m_sqlCmd, 0, cacheObj))
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
		const KSQLTableField* pfld = m_schema.GetField(i);
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

	KRecordHead* pHead = m_tableCache->_GetRecordHead((uint_r)hRec);

	for(int i=begField; i<fieldCount; i++)
	{
		int colIdx = i - begField;
		KSQLFieldDescribe& describe = m_sqlParams[colIdx];
		char* pData = (char*)pHead + m_tableCache->m_recordDecl.m_dataOffsets[i];
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
				break;
			}
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

	pHead->m_used.version = 1; // 新加载的记录版本号为1
	pHead->m_used.loadTime = (DWORD)time(NULL);
	pHead->m_used.flushTime = pHead->m_used.loadTime;
	m_tableCache->on_recordLoaded(hRec);

	return TRUE;
}

BOOL KSQLTableCache_X_Thread::realCreate(HANDLE h, KTableRecordData& pkey)
{
	KTableCacheReader::KRecordHead* head = m_tableCache->GetRecordHead(h);
	if(!head || head->IsFree()) return FALSE;
	ASSERT(m_tableCache->copyKey(h, pkey));
	head->m_used.version = 1;
	head->m_used.loadTime = (DWORD)time(NULL);
	head->m_used.flushTime = head->m_used.loadTime;
	head->SetStatusBit(KTableCacheReader::c_new_record);
	if(this->realInsert(h))
	{
		m_tableCache->on_recordLoaded(h);
		return TRUE;
	}
	return FALSE;
}

BOOL KSQLTableCache_X_Thread::realInsert(HANDLE h)
{
	KRecordHead* pHead = m_tableCache->GetRecordHead(h);
	if(!pHead) return FALSE;
	
	// 如果该记录的状态是正在改变，或者未发生改变，什么都不干
	if(pHead->IsStatus(KTableCacheReader::c_updating) || !pHead->IsStatus(KTableCacheReader::c_new_record))
	{
		return FALSE;
	}

	INT64 currVer = pHead->Version();
	pHead->SetStatusBit(KTableCacheReader::c_saving);

	if(!this->_insert(h))
	{
		pHead->ClearStatusBit(KTableCacheReader::c_saving);
		return FALSE;
	}
	
	pHead->m_used.flushTime = (DWORD)time(NULL);
	pHead->ClearStatusBit(KTableCacheReader::c_new_record);
	pHead->ClearStatusBit(KTableCacheReader::c_saving);

	return TRUE;
}

BOOL KSQLTableCache_X_Thread::realDelete(HANDLE hRec)
{
	KSQLTableSchema& m_schema = *m_tableCache->m_tableSchema;
	int pknum = m_schema.GetPrimaryKeyFieldCount();

	KRecordHead* pHead = m_tableCache->GetRecordHead(hRec);
	if(!pHead)
	{
		Log(LOG_ERROR, "error: table %s delete record:%u, invlaid record", m_schema.m_name.c_str(), hRec);
		return TRUE;
	}

	pHead->SetStatusBit(KTableCacheReader::c_saving);
	if(!this->_delete(hRec))
	{
		Log(LOG_ERROR, "error: table %s delete record %p", m_schema.m_name.c_str(), hRec);
	}

	m_tableCache->freeRecord(hRec);
	return TRUE;
}

BOOL KSQLTableCache_X_Thread::realUpdate(HANDLE hRec)
{
	KRecordHead* pHead = m_tableCache->GetRecordHead(hRec);

	// 如果该记录的状态是正在改变，或者为发生改变，什么都不干
	if(pHead->IsStatus(KTableCacheReader::c_updating) || !pHead->IsStatus(KTableCacheReader::c_modified))
	{
		return FALSE;
	}

	pHead->SetStatusBit(KTableCacheReader::c_saving);

	if(!this->_update(hRec))
	{
		pHead->ClearStatusBit(KTableCacheReader::c_saving);
		return FALSE;
	}

	pHead->ClearStatusBit(KTableCacheReader::c_modified);
	pHead->ClearStatusBit(KTableCacheReader::c_saving);
	return TRUE;
}

BOOL KSQLTableCache_X_Thread::realPartialUpdate(HANDLE hRec)
{
	KRecordHead* pHead = m_tableCache->GetRecordHead(hRec);

	// 如果该记录的状态是正在改变，什么都不干
	if(pHead->IsStatus(KTableCacheReader::c_updating))
		return TRUE;

	if(pHead->IsStatus(KTableCacheReader::c_modified))
	{	// 做全部Update的处理
		pHead->SetStatusBit(KTableCacheReader::c_saving);
		if(!this->_update(hRec))
		{
			pHead->ClearStatusBit(KTableCacheReader::c_saving);
			return FALSE;
		}
		pHead->ClearStatusBit(KTableCacheReader::c_modified);
		pHead->ClearStatusBit(KTableCacheReader::c_saving);
		return TRUE;
	}

	if(!pHead->IsStatus(KTableCacheReader::c_partial_modified))
		return TRUE;

	pHead->SetStatusBit(KTableCacheReader::c_saving);

	if(!this->_partialUpdate(hRec))
	{
		pHead->ClearStatusBit(KTableCacheReader::c_saving);
		return FALSE;
	}

	pHead->ClearStatusBit(KTableCacheReader::c_partial_modified);
	pHead->ClearStatusBit(KTableCacheReader::c_saving);
	return TRUE;
}

BOOL KSQLTableCache_X_Thread::_insert(HANDLE h)
{
	char cTmp[64];

	KSQLTableSchema& m_schema = *m_tableCache->m_tableSchema;
	int fieldCount = m_schema.GetFieldCount();

	for(int i=0; i<fieldCount; i++)
	{
		const KSQLTableField* pfld = m_schema.GetField(i);
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
	cacheObj.Attach(m_tableCache, h);

	// 首先设置主键的值
	if(!m_tableCache->SetSQLParam(m_sqlCmd, 0, cacheObj))
	{
		Log(LOG_ERROR, "error: sql: %s, set primary key param", m_sqlCmd.GetLastErrorMsg());
		return FALSE;
	}

	// 设置缺省值
	int begField = m_schema.GetPrimaryKeyFieldCount();
	for(int i=begField; i<fieldCount; i++)
	{
		int paramIdx = i;
		const KSQLTableField* pfld = m_schema.GetField(i);
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
					m_sqlCmd.SetNull(paramIdx);
				}
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
				if(defVal.size() < 1)
				{
					m_sqlCmd.SetNull(paramIdx);
				}
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
			{
				if(defVal.size() < 1)
				{
					if(pfld->m_nullable)
					{
						m_sqlCmd.SetNull(paramIdx);
					}
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

BOOL KSQLTableCache_X_Thread::_delete(HANDLE h)
{
	KCacheObject cacheObj;
	cacheObj.Attach(m_tableCache, h);

	KSQLTableSchema& m_schema = *m_tableCache->m_tableSchema;
	int begField = m_schema.GetPrimaryKeyFieldCount();
	int fcount = m_schema.GetFieldCount();

	for(int i=0; i<begField; i++)
	{
		const KSQLTableField* pfld = m_schema.GetField(i);
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

	if(!m_tableCache->SetSQLParam(m_sqlCmd, 0, cacheObj))
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

BOOL KSQLTableCache_X_Thread::_update(HANDLE h)
{
	KSQLTableSchema& m_schema = *m_tableCache->m_tableSchema;
	KRecordHead* pHead = m_tableCache->_GetRecordHead((uint_r)h);

	INT64 currVer = pHead->Version();

	int begField = m_schema.GetPrimaryKeyFieldCount();
	int fieldCount = m_schema.GetFieldCount();

	// 非组建的字段个数，non primary-key count
	int npc = fieldCount - begField;

	for(int i=0; i<fieldCount; i++)
	{
		int paramIdx = (i<begField) ? (i+npc) : (i-begField);
		const KSQLTableField* pfld = m_schema.GetField(i);
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
		const KSQLTableField* pfld = m_schema.GetField(i);

		char* pData = (char*)pHead + m_tableCache->m_recordDecl.m_dataOffsets[i];
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

	if(pHead->IsStatus(KTableCacheReader::c_updating) || pHead->Version() != currVer)
	{//数据已经发生了变化，不用执行了
		return FALSE;
	}

	// 执行SQL语句
	if(!m_sqlCmd.Execute())
	{
		Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!pHead->IsStatus(KTableCacheReader::c_updating) && pHead->Version() == currVer)
	{
		pHead->ClearStatusBit(KTableCacheReader::c_modified);
		pHead->ClearStatusBit(KTableCacheReader::c_partial_modified);
		KBitset bitset(pHead+1, m_schema.GetFieldCount());
		bitset.ClearAll();
	}

	pHead->m_used.flushTime = (DWORD)time(NULL);
	return TRUE;
}

BOOL KSQLTableCache_X_Thread::_partialUpdate(HANDLE h)
{
	KSQLTableSchema& m_schema = *m_tableCache->m_tableSchema;
	KRecordHead* pHead = m_tableCache->_GetRecordHead((uint_r)h);
	INT64 currVer = pHead->Version();

	int begField = m_schema.GetPrimaryKeyFieldCount();
	int paramCount = (int)this->_CreatePartialUpdateSQL(h);
	int npc = paramCount - begField;

	int paramNum = 0;
	int fieldCount = m_schema.GetFieldCount();
	KBitset bitset(pHead+1, fieldCount);

	if(!paramCount)
	{
		if(!pHead->IsStatus(KTableCacheReader::c_updating) && pHead->Version() == currVer)
		{
			pHead->ClearStatusBit(KTableCacheReader::c_partial_modified);
			bitset.ClearAll();
		}
		pHead->m_used.flushTime = (DWORD)time(NULL);
		return TRUE;
	}

	for(int i=0; i<fieldCount; i++)
	{
		// 非主键，未修改
		if(i >= begField && !bitset.GetBit(i)) continue;

		int paramIdx = (i<begField) ? (i+npc) : (paramNum-begField);
		paramNum++; // 处理了几个参数

		const KSQLTableField* pfld = m_schema.GetField(i);
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
		const KSQLTableField* pfld = m_schema.GetField(i);

		char* pData = (char*)pHead + m_tableCache->m_recordDecl.m_dataOffsets[i];
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

	if(pHead->IsStatus(KTableCacheReader::c_updating) || pHead->Version() != currVer)
	{//数据已经发生了变化，不用执行了
		return FALSE;
	}

	// 执行SQL语句
	if(!m_sqlCmd.Execute())
	{
		Log(LOG_ERROR, "error: sql: %s", m_sqlCmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!pHead->IsStatus(KTableCacheReader::c_updating) && pHead->Version() == currVer)
	{
		pHead->ClearStatusBit(KTableCacheReader::c_partial_modified);
		bitset.ClearAll();
	}

	pHead->m_used.flushTime = (DWORD)time(NULL);
	return TRUE;
}