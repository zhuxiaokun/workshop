#include "KBitset.h"
#include "../Memory/KMemory.h"
#include <string.h>

KBitset::KBitset(int bitCount)
{
	m_bitCount = bitCount;
	m_byteCount = KBitset::GetByteCount(bitCount);

	if(bitCount < sizeof(DWORD))
	{
		m_bits = 0;
		m_pData = &m_bits;
		m_memOwner = FALSE;
	}
	else
	{
		m_pData = (DWORD*)::malloc_k(m_byteCount);
		memset(m_pData, 0, m_byteCount);
		m_memOwner = TRUE;
	}
}

KBitset::KBitset(void* pData, int bitCount)
{
	m_bitCount = bitCount;
	m_byteCount = KBitset::GetByteCount(bitCount);
	m_pData = (DWORD*)pData;
	m_memOwner = FALSE;
}

KBitset::~KBitset()
{
	if(m_memOwner && m_pData)
	{
		::free_k(m_pData);
		m_pData = NULL;
	}
}

int KBitset::GetByteCount(int bitCount)
{
	int byteCount = (bitCount + 7) >> 3;
	return _align_size_(byteCount);
}

int KBitset::GetBitCount() const
{
	return m_bitCount;
}

void KBitset::SetAll()
{
	memset(m_pData, 0xff, m_byteCount);
}

void KBitset::ClearAll()
{
	memset(m_pData, 0, m_byteCount);
}

BOOL KBitset::SetBit(int bitno)
{
	if(bitno < 0 || bitno >= m_bitCount) return FALSE;
	int idx = bitno >> 5;
	int bno = bitno & 0x1f;
	m_pData[idx] |= (1 << bno);
	return TRUE;
}

BOOL KBitset::ClearBit(int bitno)
{
	if(bitno < 0 || bitno >= m_bitCount) return FALSE;
	int idx = bitno >> 5;
	int bno = bitno & 0x1f;
	m_pData[idx] &= ~(1 << bno);
	return TRUE;
}

BOOL KBitset::GetBit(int bitno) const
{
	if(bitno < 0 || bitno >= m_bitCount) return FALSE;
	DWORD* p = (DWORD*)m_pData;
	int idx = bitno >> 5;
	int bno = bitno & 0x1f;
	return p[idx] & (1 << bno);
}