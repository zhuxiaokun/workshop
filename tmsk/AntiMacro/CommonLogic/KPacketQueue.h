#ifndef _K_PACKET_QUEUE_H_
#define _K_PACKET_QUEUE_H_

#include <System/Memory/RingBuffer.h>
#include <System/Sync/KSync.h>
#include <System/Collections/KList.h>
#include <SockServer/JgPacketSpecific.h>

//
// 假定这里面存的一定是完整包
//
template <size_t pieceSize=2048,size_t pieceStep=32>
class KPacketQueue
{
private:
	struct Piece : public KPieceBuffer<pieceSize>
	{
		Piece* m_prevNode;
		Piece* m_nextNode;
		void Reset()
		{
			KPieceBuffer<pieceSize>::Reset();
			m_prevNode = m_nextNode = NULL;
		}
	};
	typedef System::Memory::KStepObjectPool<Piece,pieceStep> PiecePool;
	typedef System::Collections::KSelfList<Piece> PieceQueue;

public:
	KPacketQueue()
	{

	}
	~KPacketQueue()
	{
		while(!m_pieceQueue.empty())
		{
			Piece* piece = m_pieceQueue.pop_front();
			m_piecePool.Free(piece);
		}
	}

public:
	JgPacketHead* read()
	{
		int n = 0;
		char* p = &m_buf_10k[0];
		JgPacketHead* packetHeader = (JgPacketHead*)p;

		n = this->_read(p, sizeof(JgPacketHead));
		if(!n) return NULL;

		p += n; ASSERT(n == sizeof(JgPacketHead));

		// 既然有数据，那么一定是完整包
		int readed = 0;
		int len = packetHeader->length;
		while(readed < len)
		{
			n = this->_read(p, len-readed); ASSERT(n);
			p += n; readed += n;
		}

		return packetHeader;
	}
	int write(int cmd, const void* data, int len)
	{
		int n = 0;
		char* p = &m_buf_10k[0];
		
		JgPacketHead* packetHeader = (JgPacketHead*)p;
		memset(packetHeader, 0, sizeof(JgPacketHead));

		packetHeader->command = cmd;
		packetHeader->length = len;
		if(len) memcpy(packetHeader+1, data, len);

		len += sizeof(JgPacketHead);

		int writed = 0;
		while(writed < len)
		{
			n = this->_write(p, len-writed); ASSERT(n);
			p += n; writed += n;
		}

		return writed;
	}

private:
	int _read(void* buf, int len)
	{
		Piece* piece = m_pieceQueue.begin();
		if(!piece) return 0;
		if(!piece->ReadAvail() && !piece->WriteAvail())
		{
			m_pieceQueue.erase(piece);
			m_piecePool.Free(piece);
			piece = m_pieceQueue.begin();
			if(!piece) return 0;
		}
		return piece->Read(buf, len);
	}
	int _write(const void* buf, int len)
	{
		Piece* piece = m_pieceQueue.rbegin();
		if(!piece || !piece->WriteAvail())
		{
			piece = m_piecePool.Alloc(); piece->Reset();
			m_pieceQueue.push_back(piece);
		}
		return piece->Write(buf, len);
	}

public:
	// 写入和读出包使用的buffer
	char m_buf_10k[10240];
	PieceQueue m_pieceQueue;

public:
	static PiecePool m_piecePool;
};

template <size_t n,size_t m>
typename KPacketQueue<n,m>::PiecePool KPacketQueue<n,m>::m_piecePool;

#endif
