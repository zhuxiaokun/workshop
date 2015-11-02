#include "KTableCache_X.h"
#include "KCacheObject.h"
#include <System/Log/log.h>
#include "KTableCache_X_Thread.h"
#include <System/Misc/KBitset.h>

KTableCache_X::KTableCache_X():m_flushSecs(5),m_thread(NULL)
{

}

KTableCache_X::~KTableCache_X()
{
	//this->finalize();
}

BOOL KTableCache_X::initialize(KSQLTableSchema& schema, const CreateParam& createParam)
{
	if(!KTableCacheReader::initialize(schema, createParam))
		return FALSE;
	
	m_flushSecs = createParam.flushInterval;
	int_r n = m_pFileHead->usedCount;
	for(int_r i=0; i<n; i++)
	{
		KRecordHead* head = this->_GetRecordHead(i);
		if(!head || head->IsFree()) continue;
		if(!head->Version()) // 刚分配的，还没填充数据
		{
			this->_freeRecord((HANDLE)i);
		}
		else
		{
			head->ClearStatusBit(c_updating);
			head->ClearStatusBit(c_saving);
			head->ClearStatusBit(c_loading);
			this->on_recordLoaded((HANDLE)i);
		}
	}

	return TRUE;
}

void KTableCache_X::finalize()
{
	if(m_thread)
	{
		delete m_thread;
		m_thread = NULL;
	}
	KTableCacheReader::finalize();
}

HANDLE KTableCache_X::findRecord(const void* pkey, size_t len)
{
	if(!this->_checkKeyLength((int)len))
		return INVALID_HREC;
	return this->_findRecord(pkey);
}

HANDLE KTableCache_X::allocRecord()
{
	HANDLE hRec = INVALID_HREC;
	if(m_pFileHead->firstFree != invalid_recno)
	{
		hRec = this->_allocFromFree();
	}
	else
	{
		hRec = this->_allocFromBottom();
	}

	if(hRec == INVALID_HREC)
	{
		// 记日志
		return INVALID_HREC;
	}

	return hRec;
}

HANDLE KTableCache_X::allocRecord(KTableRecordData& recordData)
{
	HANDLE hRec = this->allocRecord();
	if(hRec == INVALID_HREC) return INVALID_HREC;
	ASSERT(this->copy(hRec, recordData));
	this->on_recordLoaded(hRec);
	return hRec;
}

void KTableCache_X::freeRecord(HANDLE hRec)
{
	KRecordHead* pHead = this->_GetRecordHead((uint_r)hRec);
	if(pHead->IsFree())
	{
		Log(LOG_ERROR, "error: table:%s free a un-used record %x", this->GetTableName(), hRec);
		return;
	}
	this->on_unloadRecord(hRec);
	pHead->SetFree(m_pFileHead->firstFree);
	m_pFileHead->firstFree = (DWORD)(uint_r)hRec;
}

BOOL KTableCache_X::copyKey(HANDLE h, KTableRecordData& recordData)
{
	KRecordHead* head = this->GetRecordHead(h);
	if(!head) return FALSE;

	int i = 0;
	int pkNum = m_tableSchema->GetPrimaryKeyFieldCount();
	KTableRecordData::Field* field = recordData.begin();
	while(field && i<pkNum)
	{
		int_r fieldIndex = field->fieldIndex;
		ASSERT(this->_setFieldData(h, field->fieldIndex, &field->value, field->length));
		field = recordData.next(field);
		i++;
	}

	if(i < pkNum)
	{
		Log(LOG_ERROR, "error: table:%s copyKey key count %d not enough", this->GetTableName(), i);
		return FALSE;
	}

	return TRUE;
}

BOOL KTableCache_X::copy(HANDLE h, KTableRecordData& recordData)
{
	KRecordHead* head = this->GetRecordHead(h);
	if(!head) return FALSE;

	head->m_used.version = recordData.version();
	KTableRecordData::Field* field = recordData.begin();
	while(field)
	{
		int_r fieldIndex = field->fieldIndex;
		ASSERT(this->_setFieldData(h, field->fieldIndex, &field->value, field->length));
		field = recordData.next(field);
	}

	return TRUE;
}

BOOL KTableCache_X::copyData(HANDLE h, KTableRecordData& recordData)
{
	KRecordHead* head = this->GetRecordHead(h);
	if(!head) return FALSE;

	int_r pkNum = m_tableSchema->GetPrimaryKeyFieldCount();
	head->m_used.version = recordData.version();
	KTableRecordData::Field* field = recordData.begin();
	while(field)
	{
		int_r fieldIndex = field->fieldIndex;
		if(fieldIndex >= pkNum)
			ASSERT(this->_setFieldData(h, field->fieldIndex, &field->value, field->length));
		field = recordData.next(field);
	}

	return TRUE;
}

BOOL KTableCache_X::dump(HANDLE h, KTableRecordData& recordData)
{
	KRecordHead* head = this->GetRecordHead(h);
	if(!head) return FALSE;

	recordData.clear();
	recordData.setVersion(head->m_used.version);

	int_r n = m_tableSchema->GetFieldCount();
	for(int_r i=0; i<n; i++)
	{
		void* pData;
		size_t len = this->_getFieldData(h, i, &pData);
		recordData.append(i, pData, len);
	}

	return TRUE;
}

BOOL KTableCache_X::dump(HANDLE h, int_r cols[], int_r count, KTableRecordData& recordData)
{
	KRecordHead* head = this->GetRecordHead(h);
	if(!head) return FALSE;

	recordData.clear();
	recordData.setVersion(head->m_used.version);

	for(int_r i=0; i<count; i++)
	{
		void* pData;
		int_r fieldIndex = cols[i];
		size_t len = this->_getFieldData(h, fieldIndex, &pData);
		recordData.append(i, pData, len);
	}

	return TRUE;
}

BOOL KTableCache_X::dumpData(HANDLE hSrc, HANDLE hDst)
{
	int_r pkNum = m_tableSchema->GetPrimaryKeyFieldCount();
	KRecordHead* headSrc = this->GetRecordHead(hSrc);
	KRecordHead* headDst = this->GetRecordHead(hDst);
	if(!headSrc || !headDst) return FALSE;

	int_r offset = m_recordDecl.m_dataOffsets[pkNum];
	size_t len = m_recordDecl.m_recSize - offset;
	
	void* src = (char*)headSrc + offset;
	void* dst = (char*)headDst + offset;
	memcpy(dst, src, len);

	return TRUE;
}

BOOL KTableCache_X::updateData(HANDLE h, KTableRecordData& recordData)
{
	KRecordHead* head = this->GetRecordHead(h);
	if(!head) return FALSE;

	int_r modColCount = 0;
	int_r pkNum = m_tableSchema->GetPrimaryKeyFieldCount();
	int fieldCount = m_tableSchema->GetFieldCount();
	KBitset bitset(head+1, fieldCount);

	head->SetStatusBit(c_updating);
	head->m_used.version = recordData.version();
	KTableRecordData::Field* field = recordData.begin();
	while(field)
	{
		int_r fieldIndex = field->fieldIndex;
		if(fieldIndex >= pkNum)
		{
			ASSERT(this->_setFieldData(h, field->fieldIndex, &field->value, field->length));
			bitset.SetBit((int)fieldIndex); modColCount++;
		}
		field = recordData.next(field);
	}
	if(modColCount) head->SetStatusBit(c_partial_modified);
	head->ClearStatusBit(c_updating);

	return TRUE;
}

HANDLE KTableCache_X::_allocFromBottom()
{
	DWORD recNo = m_pFileHead->usedCount;
	if(recNo >= m_pFileHead->capacity) return INVALID_HREC;

	HANDLE hRec = (HANDLE)recNo;
	KRecordHead* pHead = this->_GetRecordHead(recNo);

	pHead->SetUsed(0);
	pHead->ClearStatus();
	memset(pHead+1, 0, m_recordDecl.m_recSize-sizeof(KRecordHead));

	m_pFileHead->usedCount += 1;
	return hRec;
}

HANDLE KTableCache_X::_allocFromFree()
{
	DWORD recNo = m_pFileHead->firstFree;
	HANDLE hRec = (HANDLE)recNo;

	KRecordHead* pHead = this->_GetRecordHead(recNo);
	if(!pHead->IsFree())
	{
		Log(LOG_ERROR, "error: %s invalid cache status, free list point to an used record %u", this->GetTableName(), recNo);
		return INVALID_HREC;
	}

	m_pFileHead->firstFree = pHead->GetNextNo();

	memset(pHead, 0, sizeof(KRecordHead));
	pHead->SetUsed(0);
	pHead->ClearStatus();
	memset(pHead+1, 0, m_recordDecl.m_recSize-sizeof(KRecordHead));

	return hRec;
}

void KTableCache_X::_freeRecord(HANDLE hRec)
{
	KRecordHead* pHead = this->_GetRecordHead((uint_r)hRec);
	if(pHead->IsFree())
	{
		Log(LOG_ERROR, "error: table:%s free a un-used record %x", this->GetTableName(), hRec);
		return;
	}
	pHead->SetFree(m_pFileHead->firstFree);
	m_pFileHead->firstFree = (DWORD)(uint_r)hRec;
}