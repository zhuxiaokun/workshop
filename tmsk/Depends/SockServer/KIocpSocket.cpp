#ifndef _USE_SELECT_
#if defined(WIN32) || defined(WINDOWS)

#include "KIocpSocket.h"
#include <System/Log/log.h>
#include <System/Misc/StrUtil.h>
#include <mswsock.h>
#include "KIocpSockServer.h"

using namespace System::Sync;
using namespace System::Collections;

const char* KSocketOperation::OperationName(KSocketOperation::OperationType opType)
{
	switch(opType)
	{
	case Invalid_Op: return "Invalid_Op";
	case Send_Op:    return "Send_Op";
	case Recv_Op:    return "Recv_Op";
	case Accept_Op:  return "Accept_Op";
	default:         ASSERT(FALSE);
	}
	return NULL;
}

KIocpSocket::KIocpSocket():m_pSockServer(NULL)
{
	m_sendOperation.m_opType = KSocketOperation::Invalid_Op;	
	m_recvOperation.m_opType = KSocketOperation::Invalid_Op;
	m_acceptOperation.m_opType = KSocketOperation::Invalid_Op;
}

KIocpSocket::~KIocpSocket()
{

}

KBaseSockServer* KIocpSocket::GetSockServer()
{
	return m_pSockServer;
}

void KIocpSocket::Reset()
{
	m_pSockServer = NULL;
	m_sendOperation.m_opType = KSocketOperation::Invalid_Op;
	m_recvOperation.m_opType = KSocketOperation::Invalid_Op;
	m_acceptOperation.m_opType = KSocketOperation::Invalid_Op;
	KSocket::Reset();
}

void KIocpSocket::OnStartWork()
{
	m_readyForIO = TRUE;
	KSocket::OnStartWork();
}

void KIocpSocket::_onProcessActiveQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval)
{
	KIocpSockServer* iocpSockServer = (KIocpSockServer*)sockServer;
	switch(m_socketState)
	{
	case EnumSocketState::none: break;
	case EnumSocketState::fresh:
		{
			HANDLE hcp = CreateIoCompletionPort((HANDLE)m_sock, iocpSockServer->m_hcp, (ULONG_PTR)this, iocpSockServer->m_ioThreadNum);
			if(!hcp)
			{
				DWORD errCode = GetLastError();
				Log(LOG_WARN, "warn: CreateIoCompletionPort %s, %s", this->ToString().c_str(), strerror_r2(errCode).c_str());
				this->Close();
				return;
			}
			this->OnStartWork();
			m_readyForIO = TRUE; this->OnConnected(); m_socketState = EnumSocketState::connected;
			this->InitiateSendOperation(); // 发起第一个发送操作
			this->InitiateRecvOperation(); // 发起第一个接收操作
		}
		break;
	case EnumSocketState::connecting:
		{
			this->OnStartWork();
			HANDLE hcp = CreateIoCompletionPort((HANDLE)m_sock, iocpSockServer->m_hcp, (ULONG_PTR)this, iocpSockServer->m_ioThreadNum);
			if(!hcp)
			{
				DWORD errCode = GetLastError();
				Log(LOG_WARN, "warn: CreateIoCompletionPort %s, %s", this->ToString().c_str(), strerror_r2(errCode).c_str());
				this->Close();
				return;
			}
			m_readyForIO = TRUE; this->OnConnected(); m_socketState = EnumSocketState::connected;
			this->InitiateSendOperation();
			this->InitiateRecvOperation();
		} break;
	case EnumSocketState::connected:
		this->InitiateSendOperation();
		this->InitiateRecvOperation();
		if(m_delayCloseTick && nowTicks >= m_delayCloseTick) // 延迟关闭的时间到了
		{
			this->Close();
		}
		else
		{
			this->OnBreathe(msInterval);
			this->OnActiveState();
		}
		break;
	case EnumSocketState::closing:
		m_socketState = EnumSocketState::closed;
		this->OnClosed();
		iocpSockServer->MoveSocketToClose(m_sockIndex);
		break;
	case EnumSocketState::closed: // 不是从 closing 状态来的
		iocpSockServer->MoveSocketToClose(m_sockIndex);
		break;
	case EnumSocketState::destroying:
		ASSERT(FALSE); break;
	default:
		ASSERT(FALSE); break;
	}
}

void KIocpSocket::_onProcessClosedQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval)
{
	KIocpSockServer* iocpSockServer = (KIocpSockServer*)sockServer;
	switch(m_socketState)
	{
	case EnumSocketState::none:
	case EnumSocketState::fresh:
	case EnumSocketState::connecting:
	case EnumSocketState::connected:
	case EnumSocketState::closing:
		ASSERT(FALSE); break;
	case EnumSocketState::closed:
		if((nowTicks-m_closeTick) >= 5000)
			m_socketState = EnumSocketState::destroying;
		break;
	case EnumSocketState::destroying:
		if(iocpSockServer->_Detach(this))
		{
			Log(LOG_WARN, "warn: destroy %s", this->ToString().c_str());
			this->OnDetached();
			this->Destroy();
		} break;
	default:
		ASSERT(FALSE); break;
	}
}

//// KIocpTcpListenSocket

KIocpTcpListenSocket::KIocpTcpListenSocket()
{
	this->Reset();
}

KIocpTcpListenSocket::~KIocpTcpListenSocket()
{

}

KSocket::SocketType KIocpTcpListenSocket::GetSocketType() const
{
	return KSocket::SOCK_TCP_LISTEN;
}

int KIocpTcpListenSocket::Send(const void* packet, int len)
{
	ASSERT(FALSE);
	return 0;
}

int KIocpTcpListenSocket::SendTo(const void* packet, int len, const KSocketAddress& address)
{
	ASSERT(FALSE);
	return 0;
}

void KIocpTcpListenSocket::Reset()
{
	m_listenIp = 0;
	m_listenPort = 0;
	memset(&m_acceptOperation, 0, sizeof(m_acceptOperation));
	KIocpSocket::Reset();
}

BOOL KIocpTcpListenSocket::InitiateSendOperation()
{
	//ASSERT(FALSE);
	return FALSE;
}

BOOL KIocpTcpListenSocket::InitiateRecvOperation()
{
	if(m_sock == INVALID_SOCKET) return FALSE;
	if(m_acceptOperation.m_opType != KSocketOperation::Invalid_Op) return TRUE;
	{
		KAutoRecursiveMutex mx(m_mx);
		if(m_acceptOperation.m_opType != KSocketOperation::Invalid_Op) return TRUE;
		m_acceptOperation.m_opType = KSocketOperation::Accept_Op;
	}
	return this->_InitiateRecvOperation();
}

BOOL KIocpTcpListenSocket::OnSuccessOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes)
{
	ASSERT(&m_acceptOperation == pSockOperation);

	m_recvBytes += 1; // ???????????? 1
	m_lastestRecvTick = GetTickCount();

	SOCKET newSock = m_acceptOperation.m_acceptSock;

	if(!m_pSockServer->CanAcceptSocket())
	{
		closesocket(newSock);

		// 已经在Accept状态中，直接使用内部发起操作的函数
		this->_InitiateRecvOperation();
		return TRUE;
	}

	int nret = setsockopt(newSock,
		SOL_SOCKET,
		SO_UPDATE_ACCEPT_CONTEXT,
		(char*)&m_sock,
		sizeof(m_sock));

	if(nret)
	{ // 出错了
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: %s setsockopt, %s", this->ToString().c_str(), strerror_r2(errCode).c_str());
		closesocket(newSock);
	}
	else
	{
		this->OnAccepted(m_acceptOperation.m_acceptSock);
	}

	// 已经在Accept状态中，直接使用内部发起操作的函数
	this->_InitiateRecvOperation();
	return TRUE;
}

BOOL KIocpTcpListenSocket::OnFailOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes, DWORD errCode)
{
	JG_S::KAutoThreadMutex mx(m_mx);
	ASSERT(&m_acceptOperation == pSockOperation);

	this->OnSocketError(errCode);

	// ????????Socket??????????????????????close
	closesocket(m_acceptOperation.m_acceptSock);
	m_acceptOperation.m_acceptSock = INVALID_SOCKET;
	m_acceptOperation.m_opType = KSocketOperation::Invalid_Op;

	return TRUE;
}

// ???????????????? Close ??? Socket ??????????
BOOL KIocpTcpListenSocket::OnClosedOperation(KSocketOperation* pSockOperation)
{
	ASSERT(&m_acceptOperation == pSockOperation);

	// ????????Socket??????????????????????close
	closesocket(m_acceptOperation.m_acceptSock);
	m_acceptOperation.m_acceptSock = INVALID_SOCKET;
	m_acceptOperation.m_opType = KSocketOperation::Invalid_Op;

	return TRUE;
}

KString<256> KIocpTcpListenSocket::ToString() const
{
	char cTmp[256];
	KSocketAddress addr = {m_listenIp, m_listenPort};
	sprintf(cTmp, "TCP Listen Socket %u listen: %s",
		m_sock != INVALID_SOCKET ? m_sock : m_sockCopy,
		KSocketAddress::ToString(addr).c_str());
	return cTmp;
}

BOOL KIocpTcpListenSocket::Listen(WORD port)
{
	return this->Listen((DWORD)INADDR_ANY, port);
}

BOOL KIocpTcpListenSocket::Listen(DWORD ip, WORD port)
{
	ASSERT(m_sock == INVALID_SOCKET);

	KSocketAddress sockAddr = {ip, port};
	sockaddr_in localAddr = KSocketAddress::ToSockAddr(sockAddr);

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == INVALID_SOCKET) return FALSE;

	if(bind(sock, (sockaddr*)&localAddr, sizeof(localAddr)) != 0)
	{
		closesocket(sock);
		return FALSE;
	}

	struct linger lgr;
	lgr.l_onoff  = 1;
	lgr.l_linger = 0;
	setsockopt(sock, SOL_SOCKET, SO_LINGER, (char*)&lgr, sizeof(lgr));

	if(listen(sock, 10) != 0)
	{
		closesocket(sock);
		return FALSE;
	}

	m_sock = sock;
	m_listenIp = ip;
	m_listenPort = port;

	return TRUE;
}

BOOL KIocpTcpListenSocket::Listen(const char* addrName, WORD port)
{
	DWORD dwIp = KSocket::GetIpByName(addrName);
	if(dwIp == 0) return FALSE;
	return this->Listen(dwIp, port);
}

void KIocpTcpListenSocket::OnStartWork()
{
	memset(&m_acceptOperation, 0, sizeof(m_acceptOperation));
	KIocpSocket::OnStartWork();
}

BOOL KIocpTcpListenSocket::_InitiateRecvOperation()
{
	SOCKET sock = KSocket::CreateTcpSocket();
	if(sock == INVALID_SOCKET)
	{
		DWORD errCode = GetLastError();
		Log(LOG_WARN, "warn: WSASocket errCode:%u, %s", errCode, strerror_r2(errCode).c_str());

		m_acceptOperation.m_opType = KSocketOperation::Invalid_Op;
		return FALSE;
	}

	m_acceptOperation.m_initiateTick = GetTickCount();
	m_acceptOperation.m_recvBuf = &m_recvBuf[0];
	m_acceptOperation.m_length = sizeof(m_recvBuf);
	m_acceptOperation.m_acceptSock = sock;
	m_acceptOperation.m_pLocalAddr = (sockaddr_in*)&m_recvBuf[0];
	m_acceptOperation.m_pRemoteAddr = (sockaddr_in*)&m_recvBuf[sizeof(sockaddr_in)+16];

	BOOL bRet = AcceptEx(m_sock,
		m_acceptOperation.m_acceptSock,
		m_acceptOperation.m_recvBuf,
		0,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		&m_acceptOperation.m_byteRecved,
		(OVERLAPPED*)&m_acceptOperation);

	if(bRet)
	{	// 立刻成功
		// IOCP 会收到这个通知，此处不许处理
		//this->OnSuccessOperation(&m_acceptOperation, m_acceptOperation.m_byteRecved);
	}
	else
	{
		DWORD errCode = WSAGetLastError();
		if(errCode == ERROR_IO_PENDING) return TRUE;
		this->OnFailOperation(&m_acceptOperation, 0, errCode);
		return FALSE;
	}

	return TRUE;	
}

//// KIocpTcpSocket

KIocpTcpSocket::KIocpTcpSocket()
{
	this->Reset();
}

KIocpTcpSocket::~KIocpTcpSocket()
{

}

KSocket* KIocpTcpSocket::GetSocket()
{
	return this;
}

int KIocpTcpSocket::Send(const void* packet, int len)
{
	if(m_sock == INVALID_SOCKET || !m_readyForIO) return 0;

	m_mx.Lock();
	int nBytes = m_sendIOBuffer.Write(packet, len);
	m_mx.Unlock();

	if(nBytes < len)
	{	// 发送缓冲区满了，断开这个连接
		Log(LOG_WARN, "warn: %s send buffer full, close it", this->ToString().c_str());
		this->Close();
		return 0;
	}

	// 不发起发送操作
	//this->InitiateSendOperation();
	return len;
}

int KIocpTcpSocket::SendTo(const void* packet, int len, const KSocketAddress& address)
{
	ASSERT(FALSE);
	return 0;
}

int KIocpTcpSocket::SendImmediate(const void* packet, int len)
{
	if(m_sock == INVALID_SOCKET || !m_readyForIO) return 0;

	m_mx.Lock();
	int nBytes = m_sendIOBuffer.Write(packet, len);
	m_mx.Unlock();

	if(nBytes < len)
	{	// 发送缓冲区满了，断开这个连接
		Log(LOG_WARN, "warn: %s send buffer full, close it", this->ToString().c_str());
		this->Close();
		return 0;
	}

	// 发起发送操作
	this->InitiateSendOperation();
	return len;
}

void KIocpTcpSocket::Reset()
{
	//m_sendBuf.Reset();
	//m_recvBuf.Reset();
	//m_sendIOBuffer.Reset();
	KTcpSocketState::Reset();
	memset(&m_sendOperation, 0, sizeof(m_sendOperation)); // clear send operation
	memset(&m_recvOperation, 0, sizeof(m_recvOperation)); // clear recv operation
	KSocket::Reset();
}

BOOL KIocpTcpSocket::InitiateSendOperation()
{
	if(m_sock == INVALID_SOCKET) return FALSE;
	if(!this->HasDataToSend()) return TRUE;

	if(m_sendPaused) return TRUE;

	if(m_sendOperation.m_opType != KSocketOperation::Invalid_Op) return TRUE;
	{
		KAutoRecursiveMutex mx(m_mx);
		if(m_sendOperation.m_opType != KSocketOperation::Invalid_Op) return TRUE;
		m_sendOperation.m_opType = KSocketOperation::Send_Op;
	}
	return this->_InitiateSendOperation();
}

BOOL KIocpTcpSocket::InitiateRecvOperation()
{
	if(m_sock == INVALID_SOCKET) return FALSE;
	if(m_recvOperation.m_opType != KSocketOperation::Invalid_Op) return TRUE;
	{
		KAutoRecursiveMutex mx(m_mx);
		if(m_recvOperation.m_opType != KSocketOperation::Invalid_Op) return TRUE;
		m_recvOperation.m_opType = KSocketOperation::Recv_Op;
	}
	return this->_InitiateRecvOperation();
}

BOOL KIocpTcpSocket::InitiateAcceptOperation()
{
	ASSERT(FALSE);
	return FALSE;
}

BOOL KIocpTcpSocket::OnSuccessOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes)
{
	if(pSockOperation->m_opType == KSocketOperation::Send_Op)
	{
		ASSERT(&m_sendOperation == pSockOperation);

		if(numberOfBytes == 0)
		{	// 一个成功发送操作，只发了0字节，不知是否代表对端关闭 ?
			Log(LOG_WARN, "warn: %s send 0 byte, close it", this->ToString().c_str());
			m_sendOperation.m_opType = KSocketOperation::Invalid_Op;
			this->Close();
			return TRUE;
		}

		m_sendBytes += numberOfBytes;
		m_lastestSendTick = GetTickCount();

		//KPacketDeclare* pDecl = m_pSockServer->GetPacketDeclare();

		const void* pData = m_sendBuf.GetReadBuffer();
		this->OnSendData(pData, numberOfBytes, &m_remoteAddress);

		// 更新发送缓冲区的读指针
		m_sendBuf.m_rp += (short)numberOfBytes;
		m_sendBuf.Compact();

		return this->_InitiateSendOperation();
	}
	else if(pSockOperation->m_opType == KSocketOperation::Recv_Op)
	{
		ASSERT(&m_recvOperation == pSockOperation);

		m_recvBytes += numberOfBytes;
		m_lastestRecvTick = GetTickCount();

		if(numberOfBytes == 0) // 对方关闭这个连接了
		{
			Log(LOG_WARN, "warn: %s peer closed, close it", this->ToString().c_str());
			m_recvOperation.m_opType = KSocketOperation::Invalid_Op;
			this->OnSocketError(WSAECONNRESET); // this->Close();
			return TRUE;
		}

		//KPacketDeclare* pDecl = m_pSockServer->GetPacketDeclare();

		// 更新接收缓冲区的写指针
		m_recvBuf.m_wp += (short)numberOfBytes;

		const char* pData = m_recvBuf.GetReadBuffer();
		int dataLen = m_recvBuf.ReadAvail();

		int nBytes = this->OnRecvData(pData, dataLen, &m_remoteAddress);
		if(nBytes < 0)
		{	// 出错了
			this->OnInvalidPacket(&m_remoteAddress, (ePacketReason)nBytes);
			m_recvOperation.m_opType = KSocketOperation::Invalid_Op;
			return FALSE;
		}
		else if(nBytes > 0)
		{
			m_recvBuf.m_rp += nBytes;
			m_recvBuf.Compact();
		}

		// 继续向IOCP发起接收操作
		return this->_InitiateRecvOperation();
	}

	ASSERT(FALSE);
	return FALSE;
}

BOOL KIocpTcpSocket::OnFailOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes, DWORD errCode)
{
	JG_S::KAutoThreadMutex mx(m_mx);
	if(pSockOperation->m_opType == KSocketOperation::Send_Op)
	{
		if(&m_sendOperation == pSockOperation)
		{
			this->OnSocketError(errCode);
			m_sendOperation.m_opType = KSocketOperation::Invalid_Op;
		}
	}
	else if(pSockOperation->m_opType == KSocketOperation::Recv_Op)
	{
		if(&m_recvOperation == pSockOperation)
		{
			this->OnSocketError(errCode);
			m_recvOperation.m_opType = KSocketOperation::Invalid_Op;
		}
	}
	//else
	//{
	//	ASSERT(FALSE);
	//}
	return TRUE;
}

BOOL KIocpTcpSocket::OnClosedOperation(KSocketOperation* pSockOperation)
{
	if(pSockOperation->m_opType == KSocketOperation::Send_Op)
	{
		if(&m_sendOperation == pSockOperation)
		{
			this->Close();
			m_sendOperation.m_opType = KSocketOperation::Invalid_Op;
		}
	}
	else if(pSockOperation->m_opType == KSocketOperation::Recv_Op)
	{
		if(&m_recvOperation == pSockOperation)
		{
			this->Close();
			m_recvOperation.m_opType = KSocketOperation::Invalid_Op;
		}
	}
	else
	{
		ASSERT(FALSE);
	}

	return TRUE;
}

BOOL KIocpTcpSocket::_InitiateSendOperation()
{
	int rv = m_sendBuf.ReadAvail();

	if(rv < 1)
	{
		rv = this->PrepareSendBuffer();
	}

	if(rv < 1) // 没有数据可发
	{
		m_sendOperation.m_opType = KSocketOperation::Invalid_Op;
		return TRUE;
	}

	m_sendOperation.m_initiateTick = GetTickCount();
	m_sendOperation.m_wsaBuf.buf = (char*)m_sendBuf.GetReadBuffer();
	m_sendOperation.m_wsaBuf.len = rv;

	DWORD numberOfBytes = 0;
	int nRet = WSASend(m_sock,
		&m_sendOperation.m_wsaBuf,
		1,
		&numberOfBytes,
		0,
		(WSAOVERLAPPED*)&m_sendOperation,
		NULL);

	if(nRet == 0) // 立刻完成了
	{
		// IOCP 会收到这个通知，这里不需要处理
		return TRUE;
	}
	else
	{
		DWORD errCode = WSAGetLastError();
		if(errCode == WSA_IO_PENDING) return TRUE;

		// 操作失败了
		this->OnFailOperation(&m_sendOperation, 0, errCode);
		return FALSE;
	}

	// 不可达
	return TRUE;
}

BOOL KIocpTcpSocket::_InitiateRecvOperation()
{
	//KPacketDeclare* pDecl = m_pSockServer->GetPacketDeclare();

	int wv = m_recvBuf.WriteAvail();
	//ASSERT(wv >= pDecl->GetMaxLength());

	m_recvOperation.m_initiateTick = GetTickCount();
	m_recvOperation.m_wsaBuf.buf = m_recvBuf.GetWriteBuffer();
	m_recvOperation.m_wsaBuf.len = wv;

	DWORD dwFlags = 0;
	DWORD numberOfBytes = 0;
	int nRet = WSARecv(m_sock,
		&m_recvOperation.m_wsaBuf,
		1,
		&numberOfBytes,
		&dwFlags,
		(WSAOVERLAPPED*)&m_recvOperation,
		NULL);

	if(nRet == 0) // 立刻完成了
	{
		// IOCP会收到这个通知，所以这里不需要处理
		return TRUE;
	}
	else
	{
		DWORD errCode = WSAGetLastError();
		if(errCode == WSA_IO_PENDING) return TRUE;

		// 失败了
		this->OnFailOperation(&m_recvOperation, 0, errCode);
		return FALSE;
	}

	// 不可达
	ASSERT(FALSE);
	return FALSE;
}

void KIocpTcpSocket::OnStartWork()
{
	m_sendIOBuffer.Reset();
	m_sendBuf.Reset();
	m_recvBuf.Reset();
	memset(&m_sendOperation, 0, sizeof(m_sendOperation));
	memset(&m_recvOperation, 0, sizeof(m_recvOperation));
	KIocpSocket::OnStartWork();
}

KIocpUdpSocket::KIocpUdpSocket()
{
	KIocpUdpSocket::Reset();
}

KIocpUdpSocket::~KIocpUdpSocket()
{

}

void KIocpUdpSocket::SetSendBufferSize(int kSize)
{
	m_sendIOBuffer.SetCapacity(kSize);
}

BOOL KIocpUdpSocket::Attach(SOCKET sock)
{
	ASSERT(m_sock != INVALID_SOCKET);
	m_sock = sock;
	return TRUE;
}

BOOL KIocpUdpSocket::AutoListen()
{
	WORD port = 35000;
	while(TRUE)
	{
		if(this->Listen(port)) return TRUE;
		port += 1;
	}
	ASSERT_I(FALSE);
	return FALSE;
}

BOOL KIocpUdpSocket::Listen(WORD port)
{
	return this->Listen(INADDR_ANY, port);
}

BOOL KIocpUdpSocket::Listen(DWORD ip, WORD port)
{
	this->_Close();

	SOCKET sock = KSocket::CreateUdpSocket();
	if(sock == INVALID_SOCKET)
	{
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: create udp socket, %s", strerror_r2(errCode).c_str());
		return FALSE;
	}

	KSocketAddress addr = {ip, port};
	sockaddr_in sockAddr = KSocketAddress::ToSockAddr(addr);
	int nameLength = sizeof(sockAddr);

	int nRet = bind(sock, (sockaddr*)&sockAddr, nameLength);
	if(nRet) // 出错了
	{
		DWORD errCode = GetLastError();

		Log(LOG_ERROR, "error: bind udp socket at %s, %s",
			KSocketAddress::ToString(addr).c_str(),
			strerror_r2(errCode).c_str());

		closesocket(sock);
		return FALSE;
	}

	m_sock = sock;
	return TRUE;
}

KSocket::SocketType KIocpUdpSocket::GetSocketType() const
{
	return KSocket::SOCK_UDP;
}

int KIocpUdpSocket::Send(const void* packet, int len)
{
	ASSERT(FALSE);
	return 0;
}

int KIocpUdpSocket::SendTo(const void* packet, int len, const KSocketAddress& address)
{
	if(m_sock == INVALID_SOCKET || !m_readyForIO) return 0;

	int nBytes;
	BOOL bBufferFull = FALSE;

	// 发送缓冲区中先写入目的地址，然后是包数据
	KUdpPacketHead packetHead;
	memcpy(&packetHead.m_peerAddr, &address, sizeof(KSocketAddress));
	packetHead.m_length = len;

	m_mx.Lock();
	nBytes = m_sendIOBuffer.Write(&packetHead, sizeof(KUdpPacketHead));
	if(nBytes == sizeof(KUdpPacketHead))
	{
		nBytes = m_sendIOBuffer.Write(packet, len);
		if(nBytes < len) bBufferFull = TRUE;
	}
	else
	{
		bBufferFull = TRUE;
	}
	m_mx.Unlock();

	if(bBufferFull) // 发送缓冲区满了，关闭这个链接，清空发送缓冲区
	{
		Log(LOG_WARN, "warn: %s send buffer full, close it", this->ToString().c_str());

		m_mx.Lock();
		m_sendIOBuffer.Reset();
		m_mx.Unlock();

		this->Close();
		return 0;
	}

	// 不发起发送操作
	//this->InitiateSendOperation();
	return len;
}

int KIocpUdpSocket::SendToImmediate(const void* packet, int len, const KSocketAddress& address)
{
	if(m_sock == INVALID_SOCKET || !m_readyForIO) return 0;

	int nBytes;
	BOOL bBufferFull = FALSE;

	// 发送缓冲区中先写入目的地址，然后是包数据
	KUdpPacketHead packetHead;
	memcpy(&packetHead.m_peerAddr, &address, sizeof(KSocketAddress));
	packetHead.m_length = len;

	m_mx.Lock();
	nBytes = m_sendIOBuffer.Write(&packetHead, sizeof(KUdpPacketHead));
	if(nBytes == sizeof(KUdpPacketHead))
	{
		nBytes = m_sendIOBuffer.Write(packet, len);
		if(nBytes < len) bBufferFull = TRUE;
	}
	else
	{
		bBufferFull = TRUE;
	}
	m_mx.Unlock();

	if(bBufferFull) // 发送缓冲区满了，关闭这个链接，清空发送缓冲区
	{
		Log(LOG_WARN, "warn: %s send buffer full, close it", this->ToString().c_str());

		m_mx.Lock();
		m_sendIOBuffer.Reset();
		m_mx.Unlock();

		this->Close();
		return 0;
	}

	// 发起发送操作
	this->InitiateSendOperation();
	return len;
}

void KIocpUdpSocket::Reset()
{
	m_sendBuf.Reset();
	m_recvBuf.Reset();
	m_sendIOBuffer.Reset();
	memset(&m_sendOperation, 0, sizeof(m_sendOperation));
	memset(&m_recvOperation, 0, sizeof(m_recvOperation));
	KSocket::Reset();
}

BOOL KIocpUdpSocket::InitiateSendOperation()
{
	if(m_sock == INVALID_SOCKET) return FALSE;
	if(m_sendIOBuffer.Size() < 1) return TRUE;

	if(m_sendOperation.m_opType != KSocketOperation::Invalid_Op) return TRUE;

	m_mx.Lock();
	if(m_sendOperation.m_opType != KSocketOperation::Invalid_Op)
	{
		m_mx.Unlock();
		return TRUE;
	}
	m_sendOperation.m_opType = KSocketOperation::Send_Op;
	m_mx.Unlock();
	
	return this->_InitiateSendOperation();
}

BOOL KIocpUdpSocket::InitiateRecvOperation()
{
	if(m_sock == INVALID_SOCKET) return FALSE;
	if(m_recvOperation.m_opType != KSocketOperation::Invalid_Op) return TRUE;

	m_mx.Lock();
	if(m_recvOperation.m_opType != KSocketOperation::Invalid_Op)
	{
		m_mx.Unlock();
		return TRUE;
	}
	m_recvOperation.m_opType = KSocketOperation::Recv_Op;
	m_mx.Unlock();
	
	return this->_InitiateRecvOperation();
}

BOOL KIocpUdpSocket::InitiateAcceptOperation()
{
	ASSERT(FALSE);
	return FALSE;
}

BOOL KIocpUdpSocket::OnSuccessOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes)
{
	if(pSockOperation->m_opType == KSocketOperation::Send_Op)
	{
		ASSERT(&m_sendOperation == pSockOperation);

		m_sendBytes += numberOfBytes;
		m_lastestSendTick = GetTickCount();

		// UDP 是完整包发送的，不存在发送部分包的情况
		const void* pData = m_sendBuf.GetReadBuffer();
		int dataLen = m_sendBuf.ReadAvail();
		ASSERT(numberOfBytes == dataLen);

		KSocketAddress peerAddr = KSocketAddress::FromSockAddr(m_sendOperation.m_peerAddr);
		this->OnSendData(pData, dataLen, &peerAddr);
		m_sendBuf.Reset();

		return this->_InitiateSendOperation();
	}
	else if(pSockOperation->m_opType == KSocketOperation::Recv_Op)
	{
		ASSERT(&m_recvOperation == pSockOperation);

		m_recvBytes += numberOfBytes;
		m_lastestRecvTick = GetTickCount();

		KSocketAddress peerAddr = KSocketAddress::FromSockAddr(m_recvOperation.m_fromAddr);

		m_recvBuf.m_wp += (short)numberOfBytes;

		// UDP 是完整包接收的，不存在接收部分包的情况
		const void* pData = m_recvBuf.GetReadBuffer();
		int dataLen = m_recvBuf.ReadAvail();

		int nBytes = this->OnRecvData(pData, dataLen, &peerAddr);
		if(nBytes > 0)
		{
			m_recvBuf.m_rp += nBytes;
			m_recvBuf.Reset();			// UDP一次只接收一个包
		}
		else if(nBytes < 0)
		{
			this->OnInvalidPacket(&peerAddr, (ePacketReason)nBytes);

			// 重置接收缓冲区，UDP可能不会因为一个错误包而关闭连接
			m_recvBuf.Reset();

			// 重新发起接收操作
			this->_InitiateRecvOperation();
			return TRUE;
		}

		// 重新发起读操作
		this->_InitiateRecvOperation();
		return TRUE;
	}

	// 不可达
	ASSERT(FALSE);
	return FALSE;	
}

BOOL KIocpUdpSocket::OnFailOperation(KSocketOperation* pSockOperation, DWORD numberOfBytes, DWORD errCode)
{
	JG_S::KAutoThreadMutex mx(m_mx);
	if(pSockOperation->m_opType == KSocketOperation::Send_Op)
	{
		if(&m_sendOperation == pSockOperation)
		{
			this->OnSocketError(errCode);
			m_sendOperation.m_opType = KSocketOperation::Invalid_Op;
			//// 重新发起接收操作，在函数里面检查OnSocketError是否关闭了SOCKET
			//return this->_InitiateSendOperation();
		}
		return TRUE;
	}
	else if(pSockOperation->m_opType == KSocketOperation::Recv_Op)
	{
		if(&m_recvOperation == pSockOperation)
		{
			this->OnSocketError(errCode);
			m_recvOperation.m_opType = KSocketOperation::Invalid_Op;
			//// 发起接收操作，里面将检查SOCKET是否关闭
			//return this->_InitiateRecvOperation();
		}
		return TRUE;
	}
	//else
	//{
	//	ASSERT(FALSE);
	//}
	return TRUE;
}

BOOL KIocpUdpSocket::OnClosedOperation(KSocketOperation* pSockOperation)
{
	if(pSockOperation->m_opType == KSocketOperation::Send_Op)
	{
		if(&m_sendOperation == pSockOperation) // 是我的操作
		{
			this->Close();
			m_sendOperation.m_opType = KSocketOperation::Invalid_Op;
		}
		return TRUE;
	}
	else if(pSockOperation->m_opType == KSocketOperation::Recv_Op)
	{
		if(&m_recvOperation == pSockOperation)// 是我的操作
		{
			this->Close();
			m_recvOperation.m_opType = KSocketOperation::Invalid_Op;
		}
		return TRUE;
	}
	else
	{
		ASSERT(FALSE);
	}
	return TRUE;
}

void KIocpUdpSocket::OnStartWork()
{
	m_sendIOBuffer.Reset();
	m_sendBuf.Reset();
	m_recvBuf.Reset();
	memset(&m_sendOperation, 0, sizeof(m_sendOperation));
	memset(&m_recvOperation, 0, sizeof(m_recvOperation));
	KIocpSocket::OnStartWork();
}

KString<256> KIocpUdpSocket::ToString() const
{
	char cTmp[128];
	sprintf(cTmp, "UDP %u", m_sock != INVALID_SOCKET ? m_sock : m_sockCopy);
	return cTmp;
}

BOOL KIocpUdpSocket::_InitiateSendOperation()
{
	if(m_sock == INVALID_SOCKET)
	{
		m_sendOperation.m_opType = KSocketOperation::Invalid_Op;
		return FALSE;
	}

	// 也许是上次的数据未发送出去
	KUdpPacketHead packetHead;

	int nBytes;
	int rv = m_sendBuf.ReadAvail();
	if(rv < 1)
	{
		m_sendBuf.Reset();

		char* pData = m_sendBuf.GetWriteBuffer();
		int len = m_sendBuf.WriteAvail();

		m_mx.Lock();

		// 先读取发送包的KUdpPacketHead信息
		nBytes = m_sendIOBuffer.Read(&packetHead, sizeof(KUdpPacketHead));
		if(nBytes == sizeof(KUdpPacketHead))
		{
			// 读取包头信息，以决定整个包的长度
			int packetLength = packetHead.m_length;
			nBytes = m_sendIOBuffer.Read(pData, packetLength);
			ASSERT(nBytes == packetLength);

			m_sendOperation.m_peerAddr = KSocketAddress::ToSockAddr(packetHead.m_peerAddr);

			m_sendBuf.m_wp += packetLength;
			rv = packetLength;
		}
		else
		{
			ASSERT(nBytes == 0);
		}

		m_mx.Unlock();
	}

	if(rv < 1) // 没有数据需要发送
	{
		m_sendOperation.m_opType = KSocketOperation::Invalid_Op;
		return TRUE;
	}

	m_sendOperation.m_initiateTick = GetTickCount();
	m_sendOperation.m_wsaBuf.buf = (char*)m_sendBuf.GetReadBuffer();
	m_sendOperation.m_wsaBuf.len = rv;
	//m_sendOperation.m_peerAddr = KSocketAddress::ToSockAddr(peerAddr);

	DWORD numberOfBytes = 0;
	int nRet = WSASendTo(m_sock,
		&m_sendOperation.m_wsaBuf,
		1,
		&numberOfBytes,
		0,
		(const sockaddr*)&m_sendOperation.m_peerAddr,
		sizeof(m_sendOperation.m_peerAddr),
		(WSAOVERLAPPED*)&m_sendOperation,
		NULL);

	if(nRet == 0)
	{// 立即成功，IOCP将受到这个消息
		// this->OnSuccessOperation(&m_sendOperation, numberOfBytes);
		return TRUE;
	}
	else
	{
		DWORD errCode = WSAGetLastError();
		if(errCode == WSA_IO_PENDING) return TRUE;

		// 
		this->OnFailOperation(&m_sendOperation, 0, errCode);
		return FALSE;
	}

	return TRUE;
}

BOOL KIocpUdpSocket::_InitiateRecvOperation()
{
	if(m_sock == INVALID_SOCKET)
	{
		m_recvOperation.m_opType = KSocketOperation::Invalid_Op;
		return FALSE;
	}

	//KPacketDeclare* pDecl = m_pSockServer->GetPacketDeclare();

	int wv = m_recvBuf.WriteAvail();
	//ASSERT(wv >= pDecl->GetMaxLength());

	m_recvOperation.m_initiateTick = GetTickCount();
	m_recvOperation.m_wsaBuf.buf = m_recvBuf.GetWriteBuffer();
	m_recvOperation.m_wsaBuf.len = wv;
	m_recvOperation.m_addrLength = sizeof(m_recvOperation.m_fromAddr);

	DWORD numberOfBytes = 0;
	DWORD dwFlags = 0;
	int nRet = WSARecvFrom(m_sock,
		&m_recvOperation.m_wsaBuf,
		1,
		&numberOfBytes,
		&dwFlags,
		(sockaddr*)&m_recvOperation.m_fromAddr,
		&m_recvOperation.m_addrLength,
		(WSAOVERLAPPED*)&m_recvOperation,
		NULL);

	if(nRet == 0)
	{	// 立即成功
		//this->OnSuccessOperation(&m_recvOperation, numberOfBytes);
		return TRUE;
	}
	else
	{
		DWORD errCode = WSAGetLastError();
		if(errCode == WSA_IO_PENDING) return TRUE;
		/*this->Close();*/
		this->OnFailOperation(&m_recvOperation, 0, errCode);
		return FALSE;
	}

	ASSERT(FALSE);
	return FALSE;
}

#endif
#endif