#ifndef _K_SOCKETS_H_
#define _K_SOCKETS_H_

#include "KSocketBase.h"
#include "KIocpSocket.h"
#include "KEpollSocket.h"
#include "KSelectSocket.h"

#if defined(_USE_SELECT_)
	typedef KSelectTcpListenSocket KTcpListenSocket;
	typedef KSelectTcpSocket KTcpSocket;
	typedef KSelectUdpSocket KUdpSocket;
	typedef KSelectSockServer KSockServer;
#else
#if defined(WIN32) || defined(WINDOWS)
	typedef KIocpTcpListenSocket KTcpListenSocket;
	typedef KIocpTcpSocket KTcpSocket;
	typedef KIocpUdpSocket KUdpSocket;
	typedef KIocpSockServer KSockServer;
#else
	typedef KEpollTcpListenSocket KTcpListenSocket;
	typedef KEpollTcpSocket KTcpSocket;
	typedef KEpollUdpSocket KUdpSocket;
	typedef KEpollSockServer KSockServer;
#endif
#endif

class KServerTcpSocket;
class KNormalTcpListenSocket : public KTcpListenSocket
{
public:
	KNormalTcpListenSocket();
	virtual ~KNormalTcpListenSocket();

public:
	// Accept成功后，创建ServerTcpSocket对象
	// 返回的对象将用于SockServer::Attach
	virtual KServerTcpSocket* CreateSocketObject() = 0;

	// 在SockServer::Attach之前，对Socket对象进行必要的初始化
	virtual void PrepareSocket(KServerTcpSocket* pSock);

public:
	BOOL OnAccepted(SOCKET sock);
};

class KServerTcpSocket : public KTcpSocket
{
public:
	KServerTcpSocket();
	virtual ~KServerTcpSocket();

public:
	BOOL Attach(SOCKET sock);

public:
	KSocket::SocketType GetSocketType() const;
	::System::Collections::KString<256> ToString() const;
};

class KClientTcpSocket : public KTcpSocket
{
public:
	friend class KIocpSockServer;
	friend class KEpollSockServer;

public:
	enum { Default_ReConn_Interval = 30 };

public:
	KClientTcpSocket();
	virtual ~KClientTcpSocket();

public:
	virtual BOOL OnConnectError(DWORD errCode);

public:
	BOOL SetRemoteAddress(const char* addrName, WORD port);
	BOOL SetRemoteAddress(const KSocketAddress& addr);

	BOOL IsConnected() const;
	void SetReconnectInterval(int secs);

	// 把重连时间设置为0，这样这个Socket就不会尝试重连了
	void DisableReconnect();

	SocketType GetSocketType() const;

	void Reset();
	::System::Collections::KString<256> ToString() const;

public:
	virtual void _onProcessActiveQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval);
	virtual void _onProcessClosedQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval);

public:
	void OnStartWork();
	
	BOOL IsWouldBlock(DWORD errCode) const;

	// 主动连接，异步
	BOOL Connect();

	// 使用 select 确定连接是否建立
	int CheckConnected();

public:
	int m_reConnInterval;     // seconds
	DWORD m_nextConnTick;
	DWORD m_startConnTick;
};

#endif