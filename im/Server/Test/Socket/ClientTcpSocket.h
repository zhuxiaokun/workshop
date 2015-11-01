#pragma once
#include <SockServer/JgPacketSpecific.h>
#include <System/Memory/KPieceBufferStream.h>
#include <KCommonStruct.h>

namespace jg { namespace net {
//--------------------------------------------------------------------------
	struct SocketState
	{
		enum State
		{
			none,
			ready,
			connecting,
			connected,
			closing,
			closed,
		};
	};

	class ClientTcpSocket
	{
	public:
		typedef
			KPieceBufferStream<4096,piece_buffer_stream::alloc_type< piece_buffer_stream::piece_type<4096>,64,JG_S::KNoneLock> >
			PieceBufferStream;

	public:
		enum
		{
			send_buff_size = 2048,
			recv_buff_size = 2048,
		};

	public:
		ClientTcpSocket();
		virtual ~ClientTcpSocket();

	public:
		bool Connect(const char* ip, int port);
		bool Connect(const KSocketAddress& remoteAddr);
		void Close();
		bool SendData(const char* data, int len);

	public:
		void Breathe(int ms);

	public:
		void  SetUserData(int_r ud) { m_userData = ud; }
		int_r GetUserData() const { return m_userData; }

	public:
		int AddRef() { return ++m_refCount; }
		int ReleaseRef() { return --m_refCount; }
		int RefCount()const { return m_refCount; }

	public:
		KSocketAddress GetLocalAddress() const;
		KSocketAddress GetRemoteAddress() const;
		bool IsReady() const;

	public:
		virtual BOOL OnConnected();
		virtual BOOL OnConnectError(DWORD errCode);
		virtual BOOL OnSocketError(DWORD errCode);
		virtual BOOL OnInvalidPacket(const KSocketAddress* pFrom, int reason);
		virtual BOOL OnClosed();
		virtual string_256 ToString() const;
		virtual void Reset();
		virtual void Destroy();

	protected:
		virtual int OnRecvData(void* data, int count);

	protected:
		void _close();

		virtual bool _create();
		void _passiveClose();

		int _recv();
		int _send(const void* data, int len);

		bool _doSend();
		bool _doRecv();

		bool _checkRead();
		bool _checkWrite();
		bool _checkError();

	private:
		bool _disableBlock();
		bool _enableLinger();
		bool _keepAlive();
		bool _isWouldBlock(DWORD errCode);

	public:
		int m_refCount;
		int m_socketIndex;
		int_r m_userData;
		SOCKET m_socket, m_socket_cpy;
		SocketState::State m_socketState;
		KSocketAddress m_localAddr;
		KSocketAddress m_remoteAddr;
		PieceBufferStream m_sendPieceBuff;
		int m_sendOffset;
		int m_recvOffset;
		int m_sendBuffSize;
		char m_innerRecvBuff[recv_buff_size];
		char m_innerSendBuff[send_buff_size];
	};
//--------------------------------------------------------------------------
} }
