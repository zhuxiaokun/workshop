#ifndef _K_EPOLL_SOCKET_H_
#define _K_EPOLL_SOCKET_H_

#include "KSocketBase.h"

#ifndef _USE_SELECT_
#if !defined(WIN32) && !defined(WINDOWS)

#include <sys/epoll.h>

class KBaseSockServer;
class KEpollSockServer;

class KEpollSocket : public KSocket
{
public:
	KEpollSocket();
	virtual ~KEpollSocket();

public:
	KBaseSockServer* GetSockServer();
	void Reset();

public:
	// recv
	virtual void Read() = 0;

	// send
	virtual void Write() = 0;
	
	void OnStartWork();
	void EnableWritable();
	void DisableWritable();

public:
	virtual void _onProcessActiveQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval);
	virtual void _onProcessClosedQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval);

public:
	KEpollSockServer* m_pSockServer;
	volatile BOOL m_writable;
	volatile BOOL m_inWriting;
};

class KEpollTcpListenSocket : public KEpollSocket
{
public:
	KEpollTcpListenSocket();
	virtual ~KEpollTcpListenSocket();

public:
	BOOL Listen(WORD port);							// 监听所有Interface的指定端口
	BOOL Listen(DWORD ip, WORD port);				// 在指定IP地址上监听端口
	BOOL Listen(const char* addrName, WORD port);	// 监听制定域名上的端口

public:
	SocketType GetSocketType() const;

	int Send(const void* packet, int len);
	int SendTo(const void* packet, int len, const KSocketAddress& address);

	int SendImmediate(const void* packet, int len) { return 0; }
	int SendToImmediate(const void* packet, int len, const KSocketAddress& address) { return 0; }

	void Reset();
	JG_C::KString<256> ToString() const;

public:
	void Read();
	void Write();
	void OnStartWork();

public:
	DWORD m_listenIp;
	WORD  m_listenPort;
};

class KEpollTcpSocket : public KEpollSocket, public KTcpSocketState
{
public:
	KEpollTcpSocket();
	virtual ~KEpollTcpSocket();

public:
	KSocket* GetSocket();

public:
	int  Send(const void* packet, int len);
	int  SendTo(const void* packet, int len, const KSocketAddress& address);
	int SendImmediate(const void* packet, int len);
	int SendToImmediate(const void* packet, int len, const KSocketAddress& address) { return 0; }
	void Reset();

public:
	void Read();
	void Write();
	void OnStartWork();

public:
	//
};

class KEpollUdpSocket : public KEpollSocket
{
public:
	KEpollUdpSocket();
	virtual ~KEpollUdpSocket();

public:
	void SetSendBufferSize(int kSize);
	BOOL Attach(SOCKET sock);
	BOOL Listen(WORD port);
	BOOL Listen(DWORD ip, WORD port);

public:
	SocketType GetSocketType() const;

	int Send(const void* packet, int len);
	int SendTo(const void* packet, int len, const KSocketAddress& address);

	int SendImmediate(const void* packet, int len) { return 0; }
	int SendToImmediate(const void* packet, int len, const KSocketAddress& address);

	void Reset();
	JG_C::KString<256> ToString() const;

public:
	// 从35000开始自动选择一个端口
	BOOL AutoListen();

	void Read();
	void Write();
	void OnStartWork();

public:
	KSocketAddress m_peerAddr;    // m_sendBuff中当前数据发送的目标
	KIOBuffer m_sendIOBuffer;     // 发送的IO缓冲区，里面有目标地址
	KPieceBuffer<10240> m_sendBuf; // IOCP使用的发送缓冲区
	KPieceBuffer<10240> m_recvBuf; // IOCP使用的接收缓冲区
};

#endif
#endif
#endif