#include "KTableCacheReader.h"
#include <System/Misc/KBitset.h>
#include <System/Log/log.h>
#include <System/Misc/KBitset.h>

KTableCacheReader::KTableCacheReader()
{
	m_pFileHead = NULL;
	m_tableSchema = NULL;
	memset(&m_recordDecl, 0, sizeof(m_recordDecl));
	m_pStartMemory = NULL;
	m_pEndMemory = NULL;
}

KTableCacheReader::~KTableCacheReader()
{
	//this->finalize();
}

BOOL KTableCacheReader::initialize(KSQLTableSchema& schema, const CreateParam& createParam)
{
	m_schema_cpy = schema;
	m_tableSchema = &m_schema_cpy;

	int fieldCount = m_tableSchema->GetFieldCount();
	if(fieldCount < 1)
	{
		Log(LOG_ERROR, "error: table %s schema has no field", schema.m_name.c_str());
		return FALSE;
	}

	int_r recordDataLength = 0;
	int_r* dataOffsets = new int_r[fieldCount];
	int_r modifyFlagSize = KBitset::GetByteCount(fieldCount);
	int_r startOffset = sizeof(KRecordHead) + modifyFlagSize;
	
	for(int i=0; i<fieldCount; i++)
	{
		const KSQLTableField* fld = m_tableSchema->GetField(i);
		dataOffsets[i] = startOffset + recordDataLength;
		switch(fld->m_cdt)
		{
		case KSQLTypes::sql_c_string:
		case KSQLTypes::sql_c_varstring:
		case KSQLTypes::sql_c_binary:
			{	// 在字段前面加一个WORD来表示数据的真实长度
				recordDataLength += sizeof(WORD) + fld->m_length;
			} break;
		case KSQLTypes::sql_c_timestamp:
			{
				recordDataLength += sizeof(MYSQL_TIME_K);
				break;
			}
		default:
			{
				recordDataLength += KSQLTypes::GetCDataWidth(fld->m_cdt);
			} break;
		}
	}

	m_recordDecl.m_recSize = _align_size_(startOffset + recordDataLength);
	m_recordDecl.m_modifyFlagSize = modifyFlagSize;
	m_recordDecl.m_recordDataLength = recordDataLength;
	m_recordDecl.m_dataOffsets = dataOffsets;

	const char* filename = createParam.mmapfile;
	int_r recordCapacity = createParam.capacity;
	size_t dwSize = sizeof(KFileHead) + m_recordDecl.m_recSize*recordCapacity;
	if(!m_mmap.Initialize(filename, 0, (DWORD)dwSize))
	{
		Log(LOG_ERROR, "error: initialize memory map file, %d:%s", GetLastError(), filename);
		return FALSE;
	}

	m_pFileHead = (KFileHead*)m_mmap.GetMemory();
	m_pStartMemory = (char*)(m_pFileHead+1);
	m_pEndMemory = (char*)m_pFileHead + dwSize - m_recordDecl.m_recSize;

	if(m_mmap.GetOpenStatus() == KMMap::enum_CreateNew
		|| m_mmap.GetOpenStatus() == KMMap::enum_ModifyExists
		|| m_pFileHead->recSize != (DWORD)m_recordDecl.m_recSize
		|| m_pFileHead->capacity != (DWORD)recordCapacity)
	{
		m_pFileHead->usedCount = 0;
		m_pFileHead->firstFree = invalid_recno;
		m_pFileHead->recSize = (DWORD)m_recordDecl.m_recSize;
		m_pFileHead->capacity = (DWORD)recordCapacity;
	}

	return TRUE;
}

void KTableCacheReader::finalize()
{
	m_mmap.Finalize();
	m_pFileHead = NULL;
	if(m_recordDecl.m_dataOffsets) delete [] m_recordDecl.m_dataOffsets;
	memset(&m_recordDecl, 0, sizeof(m_recordDecl));
	m_pStartMemory = m_pEndMemory = NULL;
}

void KTableCacheReader::clearStatus(HANDLE h)
{
	KRecordHead* head = this->GetRecordHead(h);
	ASSERT(head);
	head->ClearStatus();
	KBitset bitset(head+1, m_tableSchema->GetFieldCount());
	bitset.ClearAll();
}

BOOL KTableCacheReader::discardRecord(HANDLE h)
{
	return TRUE;
}

BOOL KTableCacheReader::deleteRecord(HANDLE h)
{
	return TRUE;
}

const char* KTableCacheReader::GetTableName() const
{
	return m_tableSchema->m_name.c_str();
}

int KTableCacheReader::GetFieldIndex(const char* fieldName) const
{
	return (int)m_tableSchema->GetFieldIndex(fieldName);
}

size_t KTableCacheReader::GetRecordDataLength() const
{
	return m_recordDecl.m_recordDataLength;
}

void* KTableCacheReader::GetRecordData(HANDLE h)
{
	KRecordHead* head = this->GetRecordHead(h);
	if(!head) return NULL;
	return this->_getRecordData(head);
}

void* KTableCacheReader::_getRecordData(KRecordHead* head)
{
	return (char*)(head+1) + m_recordDecl.m_modifyFlagSize;
}

void* KTableCacheReader::_getColumnData(KRecordHead* head, int fieldIndex)
{
	return (char*)head + m_recordDecl.m_dataOffsets[fieldIndex];
}

BOOL KTableCacheReader::IsValidHandle(HANDLE h)
{
	uint_r recNo = (uint_r)h;
	if(recNo == invalid_hrec) return FALSE;
	if(recNo >= m_pFileHead->usedCount) return FALSE;
	KRecordHead* pHead = this->_GetRecordHead(recNo);
	if(pHead->IsFree()) return FALSE;
	return TRUE;
}

KTableCacheReader::KRecordHead* KTableCacheReader::GetRecordHead(HANDLE h)
{
	if(!this->IsValidHandle(h))
	{
		Log(LOG_WARN, "warn: %s invalid record handle 0x%p", this->GetTableName(), h);
		return NULL;
	}
	return this->_GetRecordHead((uint_r)h);
}

KTableCacheReader::KRecordHead* KTableCacheReader::_GetRecordHead(uint_r recNo)
{
	return (KRecordHead*)(m_pStartMemory+recNo*m_recordDecl.m_recSize);
}

BOOL KTableCacheReader::_GetBinary(HANDLE hRec, int fldIdx, int offset, void* value, int len, int& readed)
{
	void* pData;
	int length = (int)this->_getFieldData(hRec, fldIdx, &pData);
	if(length < offset) return FALSE;

	readed = kmin(len, length-offset);
	memcpy(value, pData, readed);
	return TRUE;
}

BOOL KTableCacheReader::_SetBinary(HANDLE hRec, int fldIdx, int offset, const void* value, int len)
{
	const KSQLTableField* fld = m_tableSchema->GetField(fldIdx);
	switch(fld->m_cdt)
	{
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			void* pData;
			if(offset+len > fld->m_length) return FALSE;
			this->_getFieldData(hRec, fldIdx, &pData);
			memcpy((char*)pData+offset, value, len);
			*(WORD*)((char*)pData-sizeof(WORD)) = (WORD)(offset+len);
			return TRUE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			void* pData;
			if(offset+len > sizeof(MYSQL_TIME_K)) return FALSE;
			this->_getFieldData(hRec, fldIdx, &pData);
			memcpy((char*)pData+offset, value, len);
			return TRUE;
		}
	default:
		{
			void* pData;
			if(offset+len > fld->m_length) return FALSE;
			this->_getFieldData(hRec, fldIdx, &pData);
			memcpy((char*)pData+offset, value, len);
			return TRUE;
		}
	}
}

size_t KTableCacheReader::calcRecordSize(const KSQLTableSchema& schema)
{
	int fieldCount = schema.GetFieldCount();
	
	int_r recordDataLength = 0;
	int_r modifyFlagSize = KBitset::GetByteCount(fieldCount);
	int_r startOffset = sizeof(KRecordHead) + modifyFlagSize;

	for(int i=0; i<fieldCount; i++)
	{
		const KSQLTableField* fld = schema.GetField(i);
		switch(fld->m_cdt)
		{
		case KSQLTypes::sql_c_string:
		case KSQLTypes::sql_c_varstring:
		case KSQLTypes::sql_c_binary:
			recordDataLength += sizeof(WORD) + fld->m_length;
			break;
		case KSQLTypes::sql_c_timestamp:
			recordDataLength += sizeof(MYSQL_TIME_K);
			break;
		default:
			recordDataLength += KSQLTypes::GetCDataWidth(fld->m_cdt);
			break;
		}
	}

	return _align_size_(startOffset + recordDataLength);
}

size_t KTableCacheReader::_getFieldData(HANDLE h, int_r fieldIndex, void** ppData)
{
	const KSQLTableField* pfld = m_tableSchema->GetField((int)fieldIndex);
	ASSERT(pfld);

	KRecordHead* pHead = this->_GetRecordHead((uint_r)h);
	char* pData = (char*)pHead + m_recordDecl.m_dataOffsets[fieldIndex];
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
			*ppData = pData;
			return (size_t)pfld->m_length;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			*ppData = pData;
			return sizeof(MYSQL_TIME_K);
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			size_t len = *(WORD*)pData;
			pData += sizeof(WORD);
			*ppData = pData;
			return len;
		}
	}
	return 0;
}

BOOL KTableCacheReader::_setFieldData(HANDLE h, int_r fieldIndex, const void* value, size_t len)
{
	const KSQLTableField* pfld = m_tableSchema->GetField((int)fieldIndex);
	KRecordHead* pHead = this->_GetRecordHead((uint_r)h);
	char* pData = (char*)pHead + m_recordDecl.m_dataOffsets[fieldIndex];
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
			ASSERT(len == pfld->m_length);
			memcpy(pData, value, len);
			return TRUE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			ASSERT(len == sizeof(MYSQL_TIME_K));
			memcpy(pData, value, len);
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			WORD& actualLength = *(WORD*)pData;
			pData += sizeof(WORD);
			if(len > pfld->m_length) return FALSE;
			if(len > 0) memcpy(pData, value, len);
			actualLength = (WORD)len;
			return TRUE;
		}
	}
	return TRUE;
}

BOOL KTableCacheReader::dumpKeyData(HANDLE h, KTableRecordData& recData)
{
	recData.clear();

	KSQLTableSchema& schema = *m_tableSchema;
	int_r pknum = schema.GetPrimaryKeyFieldCount();

	KRecordHead* head = this->GetRecordHead(h);
	ASSERT(!head->IsFree());

	recData.setVersion(head->Version());
	for(int_r i=0; i<pknum; i++)
		recData.append(this, h, i);

	return TRUE;
}

BOOL KTableCacheReader::dumpFreshData(HANDLE h, KTableRecordData& recData)
{
	KRecordHead* head = this->GetRecordHead(h);
	if(!head) return FALSE;

	int_r pkNum = m_tableSchema->GetPrimaryKeyFieldCount();
	int_r fldCount = m_tableSchema->GetFieldCount();

	void* pData;
	if(head->IsStatus(c_modified))
	{
		for(int_r i=pkNum; i<fldCount; i++)
		{
			size_t len = this->_getFieldData(h, i, &pData);
			recData.append(i, pData, len);
		}
	}
	else if(head->IsStatus(c_partial_modified))
	{
		KBitset bitset(head+1, (int)fldCount);
		for(int_r i=pkNum; i<fldCount; i++)
		{
			if(bitset.GetBit((int)i))
			{
				size_t len = this->_getFieldData(h, i, &pData);
				recData.append(i, pData, len);
			}
		}
	}
	return TRUE;
}