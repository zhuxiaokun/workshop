#include "KBuffer64k.h"

KBuffer64k::KBufferArray KBuffer64k::m_bufferArray;

char* KBuffer64k::WriteData(const void* pData, size_t len)
{
	KBuffer64k* pPiece = NULL;
	int c = m_bufferArray.size();
	if(c < 1)
	{
		pPiece = KBuffer64k::Alloc();
		pPiece->Reset();
		m_bufferArray.push_back(pPiece);
	}
	else
	{
		pPiece = m_bufferArray[c-1];
	}

	if(pPiece->WriteAvail() < (int_r)len)
	{
		pPiece = KBuffer64k::Alloc();
		pPiece->Reset();
		m_bufferArray.push_back(pPiece);
	}

	char* pRet = pPiece->GetWriteBuffer();
	pPiece->Write(pData, len);
	pPiece->m_wp += len;

	return pRet;
}