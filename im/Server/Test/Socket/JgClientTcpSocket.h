#pragma once
#include "ClientTcpSocket.h"

namespace jg { namespace net {
//--------------------------------------------------------------------------
	
	typedef System::Collections::DynArray<void*> JgPacketArray;

	class JgClientTcpSocket : public ClientTcpSocket
	{
	public:
		JgClientTcpSocket();
		~JgClientTcpSocket();

	protected:
		virtual bool _buildPacket(int cmd, const void* data, int len, PieceBufferStream& output);

	public:
		bool Send(int cmd, const void* data, int len);
		bool PostPacket(int cmd, const void* data, int len);

	public:
		int ReadPackets(void* pBuff, int len, JgPacketArray& arr);
		int ReadPackets(void* pBuff, int len, JgPacketArray& arr, int maxPackets);

	public:
		int OnRecvData(void* data, int count);
		void Reset();

	protected:
		bool _create();
		void _onNetPacket(int cmd, int len, const void* data);

	public:
		bool m_assembling;
		PieceBufferStream m_recvPieceBuff;
		PieceBufferStream m_assemblePieceBuff;
	};

//--------------------------------------------------------------------------
} }
