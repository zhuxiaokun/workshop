#include <System/Misc/KBitset.h>
#include "KCacheObject.h"
#include "KTableCache.h"

BOOL KDBTimestamp::Set(TIME_T t)
{
	val = t;
	return TRUE;
}

TIME_T KDBTimestamp::Get() const
{
	return (TIME_T)val;
}

KCacheObject::KCacheObject():m_hRec(INVALID_HREC),m_pCache(NULL)
{

}

KCacheObject::~KCacheObject()
{
	m_hRec = INVALID_HREC;
	m_pCache = NULL;
}

void KCacheObject::TrimRecordData()
{
	return;
}

BOOL KCacheObject::Attach(KTableCacheReader* pCache, HANDLE hRec)
{
	if(!pCache->IsValidHandle(hRec)) return FALSE;
	m_pCache = pCache;
	m_hRec = hRec;
	return TRUE;
}

void KCacheObject::Detach()
{
	m_pCache = NULL;
	m_hRec = INVALID_HREC;
}

BOOL KCacheObject::BeginUpdate()
{
	KTableCache::KRecordHead* pHead = m_pCache->GetRecordHead(m_hRec);
	if(!pHead) return FALSE;
	pHead->m_used.version += 1;
	pHead->SetStatusBit(KTableCache::c_updating);
	return TRUE;
}

BOOL KCacheObject::EndUpdate()
{
	KTableCache::KRecordHead* pHead = m_pCache->GetRecordHead(m_hRec);
	if(!pHead) return FALSE;

	this->TrimRecordData(); // 调整变长字段的长度
	
	pHead->SetStatusBit(KTableCache::c_modified);

	pHead->m_used.version += 1; // 一次修改增加两个版本号
	pHead->ClearStatusBit(KTableCache::c_updating);
	return TRUE;
}

BOOL KCacheObject::BeginUpdate(int fieldIdx)
{
	KTableCache::KRecordHead* pHead = m_pCache->GetRecordHead(m_hRec);
	if(!pHead) return FALSE;
	pHead->m_used.version += 1;
	pHead->SetStatusBit(KTableCache::c_updating);
	return TRUE;
}

BOOL KCacheObject::EndUpdate(int fieldIdx)
{
	KTableCache::KRecordHead* pHead = m_pCache->GetRecordHead(m_hRec);
	if(!pHead) return FALSE;

	this->TrimRecordData(); // 调整变长字段的长度

	KBitset bitset(pHead+1, m_pCache->m_tableSchema->GetFieldCount());
	ASSERT(bitset.SetBit(fieldIdx));

	pHead->SetStatusBit(KTableCache::c_partial_modified);
	pHead->ClearStatusBit(KTableCache::c_updating);
	return TRUE;
}

BOOL KCacheObject::Delete()
{
	KTableCache::KRecordHead* pHead = m_pCache->GetRecordHead(m_hRec);
	if(!pHead) return FALSE;
	pHead->m_used.version += 1;
	// pHead->ClearStatus(); // 删除的记录就没有必要保存了
	pHead->SetStatusBit(KTableCache::c_deleted);
	return TRUE;
}

BOOL KCacheObject::Checkout()
{
	KTableCache::KRecordHead* pHead = m_pCache->GetRecordHead(m_hRec);
	if(!pHead) return FALSE;
	pHead->m_used.version += 1;
	pHead->SetStatusBit(KTableCache::c_checkout);
	return TRUE;
}

BOOL KCacheObject::SetData(KTableRecordData& recData)
{
	KTableCache::KRecordHead* head = m_pCache->GetRecordHead(m_hRec);
	if(!head || head->IsFree()) return FALSE;

	KTableRecordData::Field* field = recData.begin();
	int_r pkNum = m_pCache->m_tableSchema->GetPrimaryKeyFieldCount();
	KBitset bitset(head+1, m_pCache->m_tableSchema->GetFieldCount());

	head->SetStatusBit(KTableCacheReader::c_updating);
	head->m_used.version = recData.version();
	while(field)
	{
		if((int)field->fieldIndex >= pkNum)
		{
			head->SetStatusBit(KTableCacheReader::c_partial_modified);
			bitset.SetBit(field->fieldIndex);
			ASSERT(m_pCache->_setFieldData(m_hRec, field->fieldIndex, &field->value, field->length));
		}
		field = recData.next(field);
	}
	head->ClearStatusBit(KTableCacheReader::c_updating);
	return TRUE;
}

BOOL KCacheObject::SetUpdateData(KTableRecordData& recData)
{
	KTableCache::KRecordHead* head = m_pCache->GetRecordHead(m_hRec);
	if(!head || head->IsFree()) return FALSE;

	KTableRecordData::Field* field = recData.begin();
	int_r pkNum = m_pCache->m_tableSchema->GetPrimaryKeyFieldCount();
	KBitset bitset(head+1, m_pCache->m_tableSchema->GetFieldCount());

	head->m_used.version++;
	head->SetStatusBit(KTableCacheReader::c_updating);
	while(field)
	{
		if((int)field->fieldIndex >= pkNum)
		{
			head->SetStatusBit(KTableCacheReader::c_partial_modified);
			bitset.SetBit(field->fieldIndex);
			ASSERT(m_pCache->_setFieldData(m_hRec, field->fieldIndex, &field->value, field->length));
		}
		field = recData.next(field);
	}
	head->ClearStatusBit(KTableCacheReader::c_updating);
	return TRUE;
}

BOOL KCacheObject::GetInteger(const char* colName, int& value)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->GetInteger(colIdx, value);
}

BOOL KCacheObject::GetInt64(const char* colName, INT64& value)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->GetInt64(colIdx, value);
}

BOOL KCacheObject::GetFloat32(const char* colName, float& value)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->GetFloat32(colIdx, value);
}

BOOL KCacheObject::GetFloat64(const char* colName, double& value)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->GetFloat64(colIdx, value);
}

BOOL KCacheObject::GetString(const char* colName, char* value, int len, int& readed)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->GetString(colIdx, value, len, readed);
}

BOOL KCacheObject::GetBinary(const char* colName, void* value, int len, int& readed)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->GetBinary(colIdx, value, len, readed);
}

BOOL KCacheObject::GetBinary(const char* colName, int offset, void* value, int len, int& readed)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->GetBinary(colIdx, offset, value, len, readed);
}

BOOL KCacheObject::GetTimestamp(const char* colName, time_t& value)
{
	INT64 v;
	if(!this->GetInt64(colName, v))
		return FALSE;
	value = v;
	return TRUE;
}

BOOL KCacheObject::GetInteger(int colIdx, int& value)
{
	char cTmp[64] = {0};
	int readed = 0;

	const KSQLTableField* pfld = m_pCache->m_tableSchema->GetField(colIdx);
	if(!pfld) return FALSE;
	
	if(!m_pCache->_GetBinary(m_hRec, colIdx, 0, cTmp, sizeof(cTmp), readed))
		return FALSE;

	switch(pfld->m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint8:
		{
			unsigned char val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int16:
		{
			short val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int32:
		{
			int val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_timestamp:
		{
			INT64 val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (INT)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint64:
		{
			UINT64 val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (INT)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float32:
		{
			float val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (int)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float64:
		{
			double val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (int)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			return FALSE;
		}
	}
	return FALSE;
}

BOOL KCacheObject::GetInt64(int colIdx, INT64& value)
{
	char cTmp[64];
	int readed = 0;

	const KSQLTableField* pfld = m_pCache->m_tableSchema->GetField(colIdx);
	if(!pfld) return FALSE;

	if(!m_pCache->_GetBinary(m_hRec, colIdx, 0, cTmp, sizeof(cTmp), readed))
		return FALSE;

	switch(pfld->m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint8:
		{
			unsigned char val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int16:
		{
			short val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int32:
		{
			int val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_timestamp:
		{
			INT64 val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint64:
		{
			UINT64 val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float32:
		{
			float val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (INT64)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float64:
		{
			double val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (INT64)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			return FALSE;
		}
	}
	return FALSE;
}
BOOL KCacheObject::GetFloat32(int colIdx, float& value)
{
	char cTmp[64];
	int readed = 0;

	const KSQLTableField* pfld = m_pCache->m_tableSchema->GetField(colIdx);
	if(!pfld) return FALSE;

	if(!m_pCache->_GetBinary(m_hRec, colIdx, 0, cTmp, sizeof(cTmp), readed))
		return FALSE;

	switch(pfld->m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint8:
		{
			unsigned char val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int16:
		{
			short val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int32:
		{
			int val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (FLOAT)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (FLOAT)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_timestamp:
		{
			INT64 val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (FLOAT)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint64:
		{
			UINT64 val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (FLOAT)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float32:
		{
			float val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float64:
		{
			double val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (FLOAT)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			return FALSE;
		}
	}
	return FALSE;
}
BOOL KCacheObject::GetFloat64(int colIdx, double& value)
{
	char cTmp[64];
	int readed = 0;

	const KSQLTableField* pfld = m_pCache->m_tableSchema->GetField(colIdx);
	if(!pfld) return FALSE;

	if(!m_pCache->_GetBinary(m_hRec, colIdx, 0, cTmp, sizeof(cTmp), readed))
		return FALSE;

	switch(pfld->m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint8:
		{
			unsigned char val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int16:
		{
			short val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int32:
		{
			int val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_timestamp:
		{
			INT64 val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (double)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint64:
		{
			UINT64 val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = (double)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float32:
		{
			float val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float64:
		{
			double val;
			ASSERT(readed == sizeof(val));
			memcpy(&val, cTmp, sizeof(val));
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			return FALSE;
		}
	}
	return FALSE;
}

BOOL KCacheObject::GetString(int colIdx, char* value, int len, int& readed)
{
	const KSQLTableField* pfld = m_pCache->m_tableSchema->GetField(colIdx);
	if(!pfld) return FALSE;

	if(!m_pCache->_GetBinary(m_hRec, colIdx, 0, value, len, readed))
		return FALSE;

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
			return FALSE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			MYSQL_TIME_K val; ASSERT(readed == sizeof(val));
			memcpy(&val, value, sizeof(val));
			KDatetime datetime(val);
			System::Collections::KString<32> str = datetime.ToString();
			if(len < (int)str.size() + 1) return FALSE;
			readed = str.size(); memcpy(value, str.c_str(), readed); value[readed] = '\0';
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
		{
			if(len < readed+1) return FALSE;
			value[readed] = '\0';
			return TRUE;
		}
	case KSQLTypes::sql_c_binary:
		{
			return TRUE;
		}
	}
	return FALSE;
}

BOOL KCacheObject::GetBinary(int colIdx, void* value, int len, int& readed)
{
	return m_pCache->_GetBinary(m_hRec, colIdx, 0, value, len, readed);
}

BOOL KCacheObject::GetBinary(int colIdx, int offset, void* value, int len, int& readed)
{
	return m_pCache->_GetBinary(m_hRec, colIdx, offset, value, len, readed);
}

BOOL KCacheObject::GetTimestamp(int colIdx, time_t& value)
{
	INT64 v;
	if(!this->GetInt64(colIdx, v))
		return FALSE;
	value = v;
	return TRUE;
}

BOOL KCacheObject::SetInteger(const char* colName, int value)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->SetInteger(colIdx, value);
}

BOOL KCacheObject::SetInt64(const char* colName, INT64 value)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->SetInt64(colIdx, value);
}

BOOL KCacheObject::SetFloat32(const char* colName, float value)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->SetFloat32(colIdx, value);
}

BOOL KCacheObject::SetFloat64(const char* colName, double value)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->SetFloat64(colIdx, value);
}

BOOL KCacheObject::SetString(const char* colName, const char* value, int len)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->SetString(colIdx, value, len);
}

BOOL KCacheObject::SetBinary(const char* colName, const void* value, int len)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->SetBinary(colIdx, value, len);
}

BOOL KCacheObject::SetBinary(const char* colName, int offset, const void* value, int len)
{
	int colIdx = m_pCache->GetFieldIndex(colName);
	if(colIdx < 0) return FALSE;
	return this->SetBinary(colIdx, offset, value, len);
}

BOOL KCacheObject::SetTimestamp(const char* colName, time_t value)
{
	return this->SetInt64(colName, value);
}

BOOL KCacheObject::SetInteger(int colIdx, int value)
{
	const KSQLTableField* pfld = m_pCache->m_tableSchema->GetField(colIdx);
	if(!pfld) return FALSE;
	
	switch(pfld->m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val;
			val = value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint8:
		{
			unsigned char val;
			val = value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_int16:
		{
			short val;
			val = value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val;
			val = value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_int32:
		{
			int val;
			val = value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val;
			val = value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_timestamp:
		{
			INT64 val;
			val = value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint64:
		{
			UINT64 val;
			val = value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_float32:
		{
			float val;
			val = (FLOAT)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_float64:
		{
			double val;
			val = value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			return FALSE;
		}
	}
	return FALSE;
}

BOOL KCacheObject::SetInt64(int colIdx, INT64 value)
{
	const KSQLTableField* pfld = m_pCache->m_tableSchema->GetField(colIdx);
	if(!pfld) return FALSE;

	switch(pfld->m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val;
			val = (CHAR)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint8:
		{
			unsigned char val;
			val = (BYTE)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_int16:
		{
			short val;
			val = (SHORT)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val;
			val = (USHORT)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_int32:
		{
			int val;
			val = (LONG)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val;
			val = (ULONG)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_timestamp:
		{
			INT64 val;
			val = value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint64:
		{
			UINT64 val;
			val = value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_float32:
		{
			float val;
			val = (FLOAT)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_float64:
		{
			double val;
			val = (double)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			return FALSE;
		}
	}
	return FALSE;
}

BOOL KCacheObject::SetFloat32(int colIdx, float value)
{
	const KSQLTableField* pfld = m_pCache->m_tableSchema->GetField(colIdx);
	if(!pfld) return FALSE;

	switch(pfld->m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val;
			val = (char)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint8:
		{
			unsigned char val;
			val = (unsigned char)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_int16:
		{
			short val;
			val = (short)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val;
			val = (unsigned short)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_int32:
		{
			int val;
			val = (int)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val;
			val = (unsigned int)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_timestamp:
		{
			INT64 val;
			val = (INT64)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint64:
		{
			UINT64 val;
			val = (UINT64)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_float32:
		{
			float val;
			val = (float)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_float64:
		{
			double val;
			val = (double)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			return FALSE;
		}
	}
	return FALSE;
}

BOOL KCacheObject::SetFloat64(int colIdx, double value)
{
	const KSQLTableField* pfld = m_pCache->m_tableSchema->GetField(colIdx);
	if(!pfld) return FALSE;

	switch(pfld->m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val;
			val = (char)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint8:
		{
			unsigned char val;
			val = (unsigned char)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_int16:
		{
			short val;
			val = (short)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val;
			val = (unsigned short)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_int32:
		{
			int val;
			val = (int)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val;
			val = (unsigned int)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_timestamp:
		{
			INT64 val;
			val = (INT64)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_uint64:
		{
			UINT64 val;
			val = (UINT64)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_float32:
		{
			float val;
			val = (float)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_float64:
		{
			double val;
			val = (double)value;
			return m_pCache->_SetBinary(m_hRec, colIdx, 0, &val, sizeof(val));
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			return FALSE;
		}
	}
	return FALSE;
}

BOOL KCacheObject::SetString(int colIdx, const char* value, int len)
{
	if(len == 0) len = (int)strlen(value);
	const KSQLTableField* pfld = m_pCache->m_tableSchema->GetField(colIdx);
	if(!pfld) return FALSE;
	if(pfld->m_cdt == KSQLTypes::sql_c_timestamp)
	{
		KDatetime datetime;
		if(!datetime.SetDatetimeString(value)) return FALSE;
		MYSQL_TIME_K mt = datetime.Time();
		return m_pCache->_SetBinary(m_hRec, colIdx, 0, &mt, sizeof(mt));
	}
	else
	{
		return m_pCache->_SetBinary(m_hRec, colIdx, 0, value, len);
	}
}

BOOL KCacheObject::SetBinary(int colIdx, const void* value, int len)
{
	return m_pCache->_SetBinary(m_hRec, colIdx, 0, value, len);
}

BOOL KCacheObject::SetBinary(int colIdx, int offset, const void* value, int len)
{
	return m_pCache->_SetBinary(m_hRec, colIdx, offset, value, len);
}

BOOL KCacheObject::SetTimestamp(int colIdx, time_t value)
{
	return this->SetInt64(colIdx, value);
}
