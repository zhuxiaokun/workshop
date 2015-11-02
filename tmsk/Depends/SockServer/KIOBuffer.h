#ifndef _K_IO_BUFFER_H_
#define _K_IO_BUFFER_H_

#include <System/KType.h>
#include <System/KMacro.h>
#include <System/Memory/KMemoryRecordset.h>
#include <System/Sync/KSync.h>
#include <System/Collections/DynArray.h>
#include <System/Memory/KNew.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Memory/RingBuffer.h>
#include <System/Collections/KSDList.h>

template <size_t cap>
struct KPieceBufferTmpl : public KPieceBuffer<cap>
{
	KPieceBufferTmpl* m_pNextNode;
};

#define IO_BUFFER_PIECE_SIZE 10240
typedef KPieceBufferTmpl<2048> KBuffer2048;
typedef KPieceBufferTmpl<IO_BUFFER_PIECE_SIZE> KBuffer_10k;
typedef JG_M::KStepObjectPool<KBuffer_10k,128,JG_S::KMTLock> KIOBuffPool_2048;

class KIOBuffer
{
public:
	typedef JG_C::KSDSelfList<KBuffer_10k> BufferQueue;

public:
	KIOBuffer();
	~KIOBuffer();

public:
	// 最大512K，无数据时不会占用内存
	void SetBufferPool(KIOBuffPool_2048& pool);
	BOOL SetCapacity(int kSize);
	int  Peek(void* pData, int len);
	int  Read(void* pData, int len);

	int  Write(const void* pData, int len);
	int  Write_Force(const void* pData, int len);

	int  WriteAvail() const;
	int  Size() const;
	void Reset();

private:
	//int _PeekPiece(int pieceIdx, char* pData, int len);
	int _PeekPiece(KBuffer_10k* piece, char* pData, int len);
	int _Read(void* pData, int len);
	int _Write(const void* pData, int len);
	int _CurrentWriteAvail();

	BOOL Inflate();
	BOOL Inflate_Force();

	void Compact();  // 把读完的Piece回收到POOL中

public:
	volatile int m_bytes;			// 当前 Buffer 中存储的字节数

	int m_maxPieces;				// 最多使用内存片数量
	int m_maxBytes;

	KIOBuffPool_2048* m_pPool;		// 分配内存片的内存池
	BufferQueue m_queue;
};

template <size_t piece_size> class KPieceListBuffer
{
public:
	typedef KPieceBufferTmpl<piece_size> piece_type;
	typedef JG_C::KSDSelfList<piece_type> piece_list;

public:
	int m_bytes;
	piece_list m_queue;

public:
	virtual piece_type* allocPiece() = 0;
	virtual void freePiece(piece_type* p) = 0;

public:
	KPieceListBuffer():m_bytes(0)
	{

	}
	int size() const
	{
		return m_bytes;
	}
	int pieceCount() const
	{
		return m_queue.size();
	}
	bool empty() const
	{
		return !!m_queue.empty();
	}
	const void* dataPtr() const
	{
		const piece_type* piece = m_queue.begin();
		if(!piece) return NULL;
		return &piece->m_buffer;
	}
	int peek(void* pData, int len)
	{
		int readed = 0;
		char* dst = (char*)pData;
		piece_type* piece = m_queue.begin();
		while(piece && len > 0)
		{
			int n = piece->peek(dst, len); dst += n; len -= n; readed += n;
			piece = m_queue.next(piece);
		}
		return readed;
	}
	int read(void* pData, int len)
	{
		int readed = 0;
		piece_type* piece = NULL;
		char* dst = (char*)pData;
		while(len > 0)
		{
			piece = m_queue.begin(); if(!piece) break;
			int n = piece->Read(dst, len);
			dst += n; len -= n; readed += n; m_bytes -= n;
			if(len > 0)
				if(piece->m_pNextNode)
				{
					m_queue.pop_front();
					this->freePiece(piece);
				}
				else
				{
					piece->Reset();
					break;
				}
		}
		return readed;
	}
	int write(const void* pData, int len)
	{
		if(len < 1) return 0;

		int writed = 0;
		const char* src = (const char*)pData;

		piece_type* piece = m_queue.rbegin();
		if(!piece || piece->WriteAvail() < 1)
		{
			piece = this->allocPiece(); ASSERT(piece); piece->Reset();
			m_queue.push_back(piece);
		}
		while(len > 0)
		{
			int n = piece->Write(src, len);
			src += n; len -= n; writed += n; m_bytes += n;
			if(len > 0)
			{
				piece = this->allocPiece(); ASSERT(piece); piece->Reset();
				m_queue.push_back(piece);
			}
		}
		return writed;
	}
	void reset()
	{
		m_bytes = 0;
		while(!m_queue.empty())
		{
			piece_type* piece = m_queue.pop_front();
			this->freePiece(piece);
		}
	}
};

#endif
