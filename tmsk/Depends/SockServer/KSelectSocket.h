#ifndef _K_SELECT_SOCKET_H_
#define _K_SELECT_SOCKET_H_

#include "KSocketBase.h"
#if defined(_USE_SELECT_)
#include "KSockServer.h"

class KSelectSockServer;
class KSelectSocket : public KSocket
{
public:
	KSelectSocket();
	virtual ~KSelectSocket();

public:
	void Reset();
	void OnStartWork();
	KBaseSockServer* GetSockServer();

public:
	virtual void _onProcessActiveQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval);
	virtual void _onProcessClosedQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval);

public:
	virtual void Read() = 0;
	virtual void Write() = 0;

public:
	KSelectSockServer* m_pSockServer;
};

class KSelectTcpListenSocket : public KSelectSocket
{
public:
	KSelectTcpListenSocket();
	virtual ~KSelectTcpListenSocket();

public:
	BOOL Listen(const char* ipName, WORD port);
	BOOL Listen(DWORD ip, WORD port);
	BOOL Listen(WORD port);

public:
	void Reset();
	SocketType GetSocketType() const;
	int Send(const void* packet, int len);
	int SendTo(const void* packet, int len, const KSocketAddress& address);
	int SendImmediate(const void* packet, int len);
	int SendToImmediate(const void* packet, int len, const KSocketAddress& address);
	JG_C::KString<256> ToString() const;

public:
	void Read();
	void Write();

public:
	KSocketAddress m_localAddr;
};

class KSelectTcpSocket : public KSelectSocket, public KTcpSocketState
{
public:
	KSelectTcpSocket();
	virtual ~KSelectTcpSocket();

public:
	KSocket* GetSocket();
	void Reset();
	int Send(const void* packet, int len);
	int SendTo(const void* packet, int len, const KSocketAddress& address);
	int SendImmediate(const void* packet, int len);
	int SendToImmediate(const void* packet, int len, const KSocketAddress& address);

public:
	void Read();
	void Write();

public:
	BOOL IsWritable();

public:
	volatile BOOL m_inWriting;				// 这个 socket 是否处于写状态，保证只有一个线程写
	volatile BOOL m_writable;
};

class KSelectUdpSocket : public KSelectSocket
{
public:
	KSelectUdpSocket();
	virtual ~KSelectUdpSocket();

public:
	BOOL Listen(const char* ipName, WORD port);
	BOOL Listen(DWORD ip, WORD port);
	BOOL Listen(WORD port);
	void SetSendBufferSize(int kSize);

public:
	void Reset();
	SocketType GetSocketType() const;
	int Send(const void* packet, int len);
	int SendTo(const void* packet, int len, const KSocketAddress& address);
	int SendImmediate(const void* packet, int len);
	int SendToImmediate(const void* packet, int len, const KSocketAddress& address);
	JG_C::KString<256> ToString() const;

	BOOL OnInvalidPacket(const KSocketAddress* pFrom, ePacketReason reason);
	BOOL OnSocketError(DWORD errCode);

public:
	void Read();
	void Write();

public:
	BOOL IsWritable();
	BOOL AutoListen();

public:
	KSocketAddress m_peerAddr;    // m_sendBuff中当前数据发送的目标
	KIOBuffer m_sendIOBuffer;     // 发送的IO缓冲区，里面有目标地址
	KPieceBuffer<10240> m_sendBuf; // IOCP使用的发送缓冲区
	KPieceBuffer<10240> m_recvBuf; // IOCP使用的接收缓冲区
	volatile BOOL m_inWriting;
	volatile BOOL m_writable;
};

#endif
#endif