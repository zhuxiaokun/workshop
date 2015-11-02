#ifndef _K_IOCP_SOCKET_H_
#define _K_IOCP_SOCKET_H_

#ifndef _USE_SELECT_
#if defined(WIN32) || defined(WINDOWS)

#include "KSocketBase.h"

struct KSocketOperation
{
	enum OperationType
	{
		Invalid_Op,
		Send_Op, 
		Recv_Op, 
		Accept_Op 
	};

	OVERLAPPED m_overlapped;
	volatile OperationType m_opType;
	DWORD m_initiateTick;

	static const char* OperationName(OperationType opType);
};

struct KSendOperation : public KSocketOperation
{
	WSABUF m_wsaBuf;
};

struct KRecvOperation : public KSocketOperation
{
	WSABUF m_wsaBuf;
};

struct KAcceptOperation : public KSocketOperation
{
	char* m_recvBuf;
	DWORD m_length;
	DWORD m_byteRecved;
	SOCKET m_acceptSock;
	sockaddr_in* m_pLocalAddr;
	sockaddr_in* m_pRemoteAddr;
};

struct KUdpSendOperation : public KSocketOperation
{
	WSABUF m_wsaBuf;
	sockaddr_in m_peerAddr;
};

struct KUdpRecvOperation : public KSocketOperation
{
	WSABUF m_wsaBuf;
	sockaddr_in m_fromAddr;
	int m_addrLength;
};

class KIocpSockServer;
class KIocpSocket : public KSocket
{
public:
	KIocpSocket();
	virtual ~KIocpSocket();

public:
	KBaseSockServer* GetSockServer();
	void Reset();

public:
	void OnStartWork();

	// 发起各种类型的IO操作
	// 由程序逻辑保证实际的操作不会并发
	virtual BOOL InitiateSendOperation() = 0;
	virtual BOOL InitiateRecvOperation() = 0;
	//virtual BOOL InitiateAcceptOperation() = 0;

	// 收到了IOCP的通知
	virtual BOOL OnSuccessOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes) = 0;
	virtual BOOL OnFailOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes, DWORD errCode) = 0;
	virtual BOOL OnClosedOperation(KSocketOperation* pSockOperation) = 0;

public:
	virtual void _onProcessActiveQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval);
	virtual void _onProcessClosedQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval);

public:
	virtual BOOL _canDestroy()
	{
		return m_refCount < 1
			&& m_sendOperation.m_opType == KSocketOperation::Invalid_Op
			&& m_recvOperation.m_opType == KSocketOperation::Invalid_Op
			&& m_acceptOperation.m_opType == KSocketOperation::Invalid_Op;
	}

public:
	KSendOperation m_sendOperation;
	KRecvOperation m_recvOperation;
	KAcceptOperation m_acceptOperation;
	KIocpSockServer* m_pSockServer;
};

class KIocpTcpListenSocket : public KIocpSocket
{
public:
	KIocpTcpListenSocket();
	virtual ~KIocpTcpListenSocket();

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

	::System::Collections::KString<256> ToString() const;

public:
	BOOL InitiateSendOperation();
	BOOL InitiateRecvOperation();

	BOOL OnSuccessOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes);
	BOOL OnFailOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes, DWORD errCode);
	BOOL OnClosedOperation(KSocketOperation* pSockOperation);
	
	// handle start work notification
	void OnStartWork();

public:
	BOOL _InitiateRecvOperation();

public:
	DWORD m_listenIp;
	WORD  m_listenPort;
	char m_recvBuf[256];	
};

class KIocpTcpSocket : public KIocpSocket, public KTcpSocketState
{
public:
	KIocpTcpSocket();
	virtual ~KIocpTcpSocket();

public:
	KSocket* GetSocket();

	int Send(const void* packet, int len);
	int SendTo(const void* packet, int len, const KSocketAddress& address);

	int SendImmediate(const void* packet, int len);
	int SendToImmediate(const void* packet, int len, const KSocketAddress& address) { return 0; }

	void Reset();

public:
	BOOL InitiateSendOperation();
	BOOL InitiateRecvOperation();
	BOOL InitiateAcceptOperation();

	BOOL OnSuccessOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes);
	BOOL OnFailOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes, DWORD errCode);
	BOOL OnClosedOperation(KSocketOperation* pSockOperation);
	
	// handle start work notification
	void OnStartWork();

public:
	BOOL _InitiateSendOperation();
	BOOL _InitiateRecvOperation();

public:
	KSendOperation m_sendOperation;
	KRecvOperation m_recvOperation;
};

class KIocpUdpSocket : public KIocpSocket
{
public:
	KIocpUdpSocket();
	virtual ~KIocpUdpSocket();

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
	::System::Collections::KString<256> ToString() const;

public:
	// 从35000开始自动选择一个端口
	BOOL AutoListen();

	BOOL InitiateSendOperation();
	BOOL InitiateRecvOperation();
	BOOL InitiateAcceptOperation();

	BOOL OnSuccessOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes);
	BOOL OnFailOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes, DWORD errCode);
	BOOL OnClosedOperation(KSocketOperation* pSockOperation);
	
	void OnStartWork();

public:
	// 向IOCP发起发送和接收操作的请求
	BOOL _InitiateSendOperation();
	BOOL _InitiateRecvOperation();

public:
	KIOBuffer m_sendIOBuffer;				// 待发送数据队列
	KPieceBuffer<10240> m_sendBuf;			// 发送缓冲区
	KPieceBuffer<10240> m_recvBuf;			// 接收缓冲区

	KUdpSendOperation m_sendOperation;		// 当前的发送操作
	KUdpRecvOperation m_recvOperation;		// 当前的接收操作
};

#endif
#endif
#endif