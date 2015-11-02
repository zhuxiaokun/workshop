#include "KMemoryRecordset.h"
#include "KMemory.h"

namespace System { namespace Memory {

KMemoryRecordset::KMemoryRecordset():m_resposible(FALSE)
{
	memset(&m_header, 0, sizeof(m_header));
	m_myHeader = &m_header;

	m_myHeader->capacity = 0;
	m_myHeader->firstFree = invalid_rec_no;
	m_myHeader->recSize = 0;
	m_myHeader->usedCount = 0;

	m_pRecordset = NULL;
	m_endPointer = NULL;
}

KMemoryRecordset::~KMemoryRecordset()
{
	this->Finalize();
}

BOOL KMemoryRecordset::Initialize(int recSize, int capacity)
{
	ASSERT(recSize > 0);

	memset(&m_header, 0, sizeof(m_header));
	m_myHeader = &m_header;

	// 与 sizeof(void*) 对齐，避免出现对齐引起的问题
	m_myHeader->capacity = capacity;
	m_myHeader->firstFree = invalid_rec_no;
	m_myHeader->recSize = _align_size_(recSize); //(recSize+sizeof(long))&(~(sizeof(long)-1));
	m_myHeader->usedCount = 0;

	DWORD memorySize = m_myHeader->recSize * m_myHeader->capacity;
	m_pRecordset = (char*)malloc_k(memorySize);
	if(!m_pRecordset) return FALSE;
	m_endPointer = m_pRecordset + memorySize;

	m_resposible = TRUE;
	return TRUE;
}

BOOL KMemoryRecordset::Attach(RecordsetHeader& header, void* startAddr)
{
	ASSERT(!(header.recSize & (sizeof(void*)-1)));

	m_myHeader = &header;
	m_pRecordset = (char*)startAddr;
	m_resposible = FALSE;
	m_endPointer = (char*)startAddr + (m_myHeader->capacity*m_myHeader->recSize);

	return TRUE;
}

void KMemoryRecordset::Detach()
{
	if(m_resposible) return;
	m_myHeader = &m_header;
	m_pRecordset = NULL;
	m_endPointer = NULL;
	m_resposible = TRUE;
}

void KMemoryRecordset::Finalize()
{
	this->Clear();
	if(m_resposible)
	{
		if(m_pRecordset)
		{
			free_k(m_pRecordset);
			m_pRecordset = NULL;
		}
		m_resposible = FALSE;
	}
}

void KMemoryRecordset::Clear()
{
	m_myHeader->usedCount = 0;
	m_myHeader->firstFree = invalid_rec_no;
}

BOOL KMemoryRecordset::IsResponsible(const void* pData) const
{
	return pData >= m_pRecordset && pData < m_endPointer;
}

HANDLE KMemoryRecordset::AllocRecord()
{
	if(m_myHeader->firstFree == invalid_rec_no)
	{	// 空闲列表为空 
		return this->AllocBottom();	
	}
	else
	{	
		return this->AllocFree();
	}
}

BOOL KMemoryRecordset::FreeRecord(HANDLE hData)
{
	if(!hData) return FALSE;

	// 不是我的内存
	if(!this->IsResponsible(hData))
		return FALSE;

	MemoryRecord* pRecord = (MemoryRecord*)hData;

	pRecord->SetNextNo(m_myHeader->firstFree);
	m_myHeader->firstFree = this->RecNoFromHandle(hData);

	return TRUE;
}

void* KMemoryRecordset::GetRecordData(HANDLE hData)
{
	if(!hData) return NULL;
	return (void*)hData;
}

HANDLE KMemoryRecordset::GetHandle(void* p)
{
	if(!this->IsResponsible(p))
		return NULL;
	return (HANDLE)p;
}

int KMemoryRecordset::GetRecordNo(HANDLE h)
{
	return this->RecNoFromHandle(h);
}

HANDLE KMemoryRecordset::GetHandle(int recordNo)
{
	return this->HandleFromRecNo(recordNo);
}

HANDLE KMemoryRecordset::HandleFromRecNo(int recNo)
{
	if(recNo < 0 || recNo >= (int)m_myHeader->capacity) return NULL;
	return HANDLE(m_pRecordset + recNo * m_myHeader->recSize);
}

DWORD KMemoryRecordset::RecNoFromHandle(HANDLE hData)
{
	if(!hData) return (DWORD)-1;
	return DWORD((char*)hData-m_pRecordset) / m_myHeader->recSize;
}

HANDLE KMemoryRecordset::AllocBottom()
{
	int recNo = m_myHeader->usedCount;
	if(recNo >= (int)m_myHeader->capacity) return NULL;
	HANDLE hData = this->HandleFromRecNo(recNo);
	m_myHeader->usedCount += 1;
	return hData;
}

HANDLE KMemoryRecordset::AllocFree()
{
	int recNo = m_myHeader->firstFree;
	if(recNo == invalid_rec_no) return NULL;

	HANDLE hData = this->HandleFromRecNo(recNo);
	MemoryRecord* pRec = (MemoryRecord*)hData;
	m_myHeader->firstFree = pRec->NextNo();

	return hData;
}

}}