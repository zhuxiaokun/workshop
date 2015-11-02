#include "KIOBuffer.h"

KIOBuffer::KIOBuffer()
{
	m_bytes = 0;
	m_pPool = NULL;
	this->SetCapacity(1024*40); // 缺省 40M 缓冲区
}

KIOBuffer::~KIOBuffer()
{
	this->Reset();
}

void KIOBuffer::SetBufferPool(KIOBuffPool_2048& pool)
{
	m_pPool = &pool;
}

BOOL KIOBuffer::SetCapacity(int kSize)
{
	ASSERT(m_queue.empty());
	m_maxPieces = (kSize*1024 + IO_BUFFER_PIECE_SIZE - 1) / IO_BUFFER_PIECE_SIZE;
	ASSERT(m_maxPieces >= 1);
	m_maxBytes = m_maxPieces * IO_BUFFER_PIECE_SIZE;
	return TRUE;
}

int KIOBuffer::Peek(void* pData, int len)
{
	int lenOrg = len;
	char* p = (char*)pData;

	if(m_queue.empty()) return 0;

	KBuffer_10k* piece = m_queue.begin();
	while(piece)
	{
		int nBytes = this->_PeekPiece(piece, p, len);
		piece = m_queue.next(piece);
		p += nBytes; len -= nBytes;
		if(len < 1) break;
	}

	return lenOrg - len;
}

// 一次最多从一个Piece中读取
int KIOBuffer::Read(void* pData, int len)
{
	int lenOrg = len;
	char* p = (char*)pData;

	while(len > 0)
	{
		if(m_queue.empty()) break;
		int nBytes = this->_Read(p, len);
		if(nBytes == 0) // 读不到数据了
			break;
		p += nBytes; len -= nBytes;
	}

	return lenOrg - len;
}

int KIOBuffer::Write(const void* pData, int len)
{
	int lenOrg = len;
	const char* p = (const char*)pData;

	while(len > 0)
	{
		int nBytes = this->_CurrentWriteAvail();
		if(nBytes > 0)
		{
			nBytes = this->_Write(p, len);
			p += nBytes; len -= nBytes;
		}
		else
		{
			if(!this->Inflate())
				break;
		}
	}
	return lenOrg - len;
}

// 强制写入，不受最大缓冲区大小的限制
int  KIOBuffer::Write_Force(const void* pData, int len)
{
	int lenOrg = len;
	const char* p = (const char*)pData;

	while(len > 0)
	{
		int nBytes = this->_CurrentWriteAvail();
		if(nBytes > 0)
		{
			nBytes = this->_Write(p, len);
			p += nBytes; len -= nBytes;
		}
		else
		{
			if(!this->Inflate_Force())
				break;
		}
	}
	return lenOrg - len;
}

// 这个函数一直都是在锁的范围内被调用的
// 如果第一片已经读取了一部分的话，那么他的 rp 指针需要考虑
// 怀疑与外网 2009.11.28 一次 GameServer 崩溃相关
int KIOBuffer::WriteAvail() const
{
	const KBuffer_10k* pPiece = m_queue.begin();
	if(!pPiece)
	{
		return m_maxBytes;
	}
	else
	{
		return m_maxBytes - (pPiece->m_rp + m_bytes);
	}
}

int KIOBuffer::Size() const
{
	return m_bytes;
}

void KIOBuffer::Reset()
{
	while(!m_queue.empty())
	{
		KBuffer_10k* pPiece = m_queue.pop_front();
		m_pPool->Free(pPiece);
	}
	m_bytes = 0;
}

int KIOBuffer::_PeekPiece(KBuffer_10k* piece, char* pData, int len)
{
	int rp = piece->m_rp;
	int nBytes = piece->Read(pData, len);
	piece->m_rp = rp;
	return nBytes;
}

// 只从顶部的Piece中读取
int KIOBuffer::_Read(void* pData, int len)
{
	ASSERT(!m_queue.empty());
	KBuffer_10k* pPiece = m_queue.begin();
	int nBytes = pPiece->Read(pData, len);
	m_bytes -= nBytes;
	ASSERT(m_bytes >= 0);
	this->Compact();
	return nBytes;
}

// 最多写一个 Piece
int KIOBuffer::_Write(const void* pData, int len)
{
	KBuffer_10k* pPiece = m_queue.rbegin();
	ASSERT(pPiece);
	int nBytes = pPiece->Write(pData, len);
	m_bytes += nBytes;
	return nBytes;
}

int KIOBuffer::_CurrentWriteAvail()
{
	KBuffer_10k* pPiece = m_queue.rbegin();
	if(!pPiece) return 0;
	return pPiece->WriteAvail();
}

BOOL KIOBuffer::Inflate()
{
	if(m_queue.size() >= m_maxPieces)
		return FALSE; // 达到最多内存块的数量

	KBuffer_10k* pPiece = m_pPool->Alloc();
	if(!pPiece) return FALSE; // 内存分不出来了 !!

	pPiece->Reset();
	m_queue.push_back(pPiece);
	
	return TRUE;
}

BOOL KIOBuffer::Inflate_Force()
{
	KBuffer_10k* pPiece = m_pPool->Alloc();
	if(!pPiece) return FALSE; // 内存分不出来了 !!

	pPiece->Reset();
	m_queue.push_back(pPiece);

	return TRUE;
}

// 只会回收掉队列头部的Piece
void KIOBuffer::Compact()
{
	//if(m_queue.empty()) return;

	KBuffer_10k* pPiece = m_queue.begin();
	if(!pPiece) return;

	// 第一片有可读数据
	if(pPiece->ReadAvail())
		return;

	if(pPiece == m_queue.rbegin())
	{
		// 最后一片保留，减少分配释放次数
		pPiece->Reset();
	}
	else
	{
		//if(pPiece->WriteAvail())
		//	return;
		m_queue.pop_front();
		m_pPool->Free(pPiece);
	}

	return;
}
