#include "KSelectSocket.h"
#if defined(_USE_SELECT_)
#include <System/Misc/StrUtil.h>
#include <System/Log/log.h>
#include "KSelectSockServer.h"

KSelectSocket::KSelectSocket()
{
	m_pSockServer = NULL;
}

KSelectSocket::~KSelectSocket()
{

}

void KSelectSocket::Reset()
{
	m_pSockServer = NULL;
	KSocket::Reset();
}

void KSelectSocket::OnStartWork()
{
	KSocket::OnStartWork();
	m_readyForIO = TRUE;
}

KBaseSockServer* KSelectSocket::GetSockServer()
{
	return m_pSockServer;
}

void KSelectSocket::_onProcessActiveQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval)
{
	KSelectSockServer* selectSockServer = (KSelectSockServer*)sockServer;
	switch(m_socketState)
	{
	case EnumSocketState::none: break;
	case EnumSocketState::fresh:
		{
			this->OnStartWork();
			m_readyForIO = TRUE; this->OnConnected(); m_socketState = EnumSocketState::connected;
			this->Write(); // 发起第一个发送操作
			//this->Read(); // 发起第一个接收操作
		}
		break;
	case EnumSocketState::connecting:
		{
			this->OnStartWork();
			m_readyForIO = TRUE; this->OnConnected(); m_socketState = EnumSocketState::connected;
			this->Write();
			//this->Read();
		} break;
	case EnumSocketState::connected:
		this->Write();
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
		selectSockServer->MoveSocketToClose(m_sockIndex);
		break;
	case EnumSocketState::closed: // 不是从 closing 状态来的
		selectSockServer->MoveSocketToClose(m_sockIndex);
		break;
	case EnumSocketState::destroying:
		ASSERT(FALSE); break;
	default:
		ASSERT(FALSE); break;
	}
}

void KSelectSocket::_onProcessClosedQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval)
{
	KSelectSockServer* selectSockServer = (KSelectSockServer*)sockServer;
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
		if(selectSockServer->_Detach(this))
		{
			Log(LOG_WARN, "warn: destroy %s", this->ToString().c_str());
			this->OnDetached();
			this->Destroy();
		} break;
	default:
		ASSERT(FALSE); break;
	}
}

/// KSelectTcpListenSocket

KSelectTcpListenSocket::KSelectTcpListenSocket()
{
	memset(&m_localAddr, 0, sizeof(m_localAddr));
}

KSelectTcpListenSocket::~KSelectTcpListenSocket()
{

}

BOOL KSelectTcpListenSocket::Listen(const char* ipName, WORD port)
{
	DWORD ip = KSocket::GetIpByName(ipName);
	if(!ip) return FALSE;
	return this->Listen(ip, port);
}

BOOL KSelectTcpListenSocket::Listen(DWORD ip, WORD port)
{
	ASSERT(m_sock == INVALID_SOCKET);

	KSocketAddress sockAddr = {ip, port};
	sockaddr_in localAddr = KSocketAddress::ToSockAddr(sockAddr);

	SOCKET sock = KSocket::CreateTcpSocket();
	if(sock == INVALID_SOCKET) return FALSE;

	if(bind(sock, (sockaddr*)&localAddr, sizeof(localAddr)) != 0)
	{
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: bind to %s, %s",
			KSocketAddress::ToString(sockAddr).c_str(),
			strerror_r2(errCode).c_str());
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
	memcpy(&m_localAddr, &sockAddr, sizeof(KSocketAddress));

	return TRUE;
}

BOOL KSelectTcpListenSocket::Listen(WORD port)
{
	return this->Listen((DWORD)0, port);
}

void KSelectTcpListenSocket::Reset()
{
	memset(&m_localAddr, 0, sizeof(m_localAddr));
	KSelectSocket::Reset();
}

KSocket::SocketType KSelectTcpListenSocket::GetSocketType() const
{
	return KSocket::SOCK_TCP_LISTEN;
}

int KSelectTcpListenSocket::Send(const void* packet, int len)
{
	return 0;
}

int KSelectTcpListenSocket::SendTo(const void* packet, int len, const KSocketAddress& address)
{
	return 0;
}

int KSelectTcpListenSocket::SendImmediate(const void* packet, int len)
{
	return 0;
}

int KSelectTcpListenSocket::SendToImmediate(const void* packet, int len, const KSocketAddress& address)
{
	return 0;
}

JG_C::KString<256> KSelectTcpListenSocket::ToString() const
{
	char cTmp[128];
	sprintf(cTmp, "tcp listen %u at %s", m_sock, KSocketAddress::ToString(m_localAddr).c_str());
	return cTmp;
}

void KSelectTcpListenSocket::Read()
{
	sockaddr_in peerAddr;
	socklen_t addrLen = sizeof(peerAddr);

	while(TRUE)
	{
		SOCKET sock = accept(m_sock, (sockaddr*)&peerAddr, &addrLen);
		if(sock == INVALID_SOCKET) break;

		if(m_pSockServer->CanAcceptSocket())
		{
			KSocket::SetNonBlock(sock);
			KSocket::IgnoreSigPipe(sock);
			this->OnAccepted(sock);
		}
		else
		{
			closesocket(sock);
		}

		m_recvBytes += 1;
		m_lastestRecvTick = GetTickCount();
	}

	DWORD errCode = GetLastError();

#if defined(WIN32) || defined(WINDOWS)
	if(errCode == WSAEWOULDBLOCK) return;
#else
	if(errCode == EAGAIN || errCode == EWOULDBLOCK) return;

	// A connection has been aborted. or
	// The system call was interrupted by a signal that was caught before a valid connection arrived.
	if(errCode == ECONNABORTED || errCode == EINTR) return;
#endif

	// 出错了
	this->OnSocketError(errCode);
	return;
}

void KSelectTcpListenSocket::Write()
{
	return;
}

KSelectTcpSocket::KSelectTcpSocket()
{
	memset(&m_remoteAddress, 0, sizeof(m_remoteAddress));
	m_inWriting = FALSE;
	m_writable = TRUE;
}

KSelectTcpSocket::~KSelectTcpSocket()
{

}

KSocket* KSelectTcpSocket::GetSocket()
{
	return this;
}

void KSelectTcpSocket::Reset()
{
	//m_sendBuf.Reset();
	//m_recvBuf.Reset();
	//m_sendIOBuffer.Reset();
	//memset(&m_remoteAddress, 0, sizeof(m_remoteAddress));
	KTcpSocketState::Reset();
	m_inWriting = FALSE;
	m_writable = TRUE;
	KSelectSocket::Reset();
}

// 这里可能一次发多个包，如大包拆小后变成了数个
int KSelectTcpSocket::Send(const void* packet, int len)
{	
	if(m_sock == INVALID_SOCKET || !m_readyForIO)
	{
		return 0;
	}

	m_mx.Lock();
	int nBytes = m_sendIOBuffer.Write(packet, len);
	m_mx.Unlock();

	// Socket的IOBuffer满了，关闭连接
	if(nBytes < len)
	{
		Log(LOG_WARN, "warn: %s send io buffer full, close it", this->ToString().c_str());
		this->Close();
		return 0;
	}

	return nBytes;
}

int KSelectTcpSocket::SendTo(const void* packet, int len, const KSocketAddress& address)
{
	ASSERT(FALSE); return 0;
}

int KSelectTcpSocket::SendImmediate(const void* packet, int len)
{
	int nBytes = this->Send(packet, len);
	if(!nBytes) return 0;
	// 真正发送数据
	this->Write();
	return len;
}

int KSelectTcpSocket::SendToImmediate(const void* packet, int len, const KSocketAddress& address)
{
	ASSERT(FALSE); return 0;
}

void KSelectTcpSocket::Read()
{
	while(TRUE)
	{
		int len = m_recvBuf.WriteAvail();
		char* buff = m_recvBuf.GetWriteBuffer();
		ASSERT(len > 0);

		int readBytes = recv(m_sock, buff, len, 0);
		if(readBytes > 0)	// 收到了数据，通知
		{
			m_recvBytes += readBytes;
			m_lastestRecvTick = GetTickCount();

			m_recvBuf.m_wp += readBytes;

			const void* pData = m_recvBuf.GetReadBuffer();
			int dataLen = m_recvBuf.ReadAvail();

			int nBytes = this->OnRecvData(pData, dataLen, &m_remoteAddress);
			if(nBytes > 0)
			{
				m_recvBuf.m_rp += nBytes;
				m_recvBuf.Compact();
			}
			else if(nBytes < 0)
			{
				this->OnInvalidPacket(&m_remoteAddress, (ePacketReason)nBytes);
				m_recvBuf.Reset();
				return;
			}

			// 缓冲区中的数据都读完了，不需要再试
			if(readBytes < len) break;
		}
		else if(readBytes == 0) // 对端关闭了这个连接
		{
			Log(LOG_WARN, "warn: %s, peer closed, close it", this->ToString().c_str());
			this->Close();
			return;
		}
		else
		{
			DWORD errCode = GetLastError();

		#if defined(WIN32) || defined(WINDOWS)
			if(errCode == WSAEWOULDBLOCK) break;
		#else
			// The  socket  is  marked non-blocking and the receive operation would block,
			// or a receive timeout had been
			// set and the timeout expired before data was received.
			if(errCode == EAGAIN) break;

			// The receive was interrupted by delivery of a signal before any data were available.
			if(errCode == EINTR)
			{
				Log(LOG_WARN, "warn: %s, %s", this->ToString().c_str(), strerror_r2(errCode).c_str());
				return;
			}
		#endif
			this->OnSocketError(errCode);
			return;
		}
	}
}

void KSelectTcpSocket::Write()
{
	if(m_sock == INVALID_SOCKET) return;
	if(!m_readyForIO) return;

	// no data to send
	if(!this->HasDataToSend()) return;
	
	if(m_inWriting) return;

	m_mx.Lock();
	if(m_inWriting)
	{
		m_mx.Unlock();
		return;
	}
	m_inWriting = TRUE;
	m_mx.Unlock();

	if(!m_writable)
	{
		m_writable = this->IsWritable();
		if(!m_writable)
		{
			m_inWriting = FALSE;
			return;
		}
	}

	while(TRUE)
	{
		int len = m_sendBuf.ReadAvail();
		const char* pBuff = m_sendBuf.GetReadBuffer();

		if(len > 0)
		{
			int flags = 0;
#if !defined(WIN32) && !defined(__APPLE__)
			flags = MSG_NOSIGNAL;
#endif

			int nBytes = send(m_sock, pBuff, len, flags);
			if(nBytes == -1)
			{
				DWORD errCode = GetLastError();

			#if defined(WIN32) || defined(WINDOWS)
				if(errCode == WSAEWOULDBLOCK)
				{
					m_writable = FALSE;
					break;
				}
			#else
				// The socket is marked non-blocking and the requested operation would block.
				if(errCode == EAGAIN || errCode == EWOULDBLOCK)
				{
					m_writable = FALSE;
					break;
				}

				// A signal occurred before any data was transmitted.
				if(errCode == EINTR) break;
			#endif

				this->OnSocketError(errCode);
				break;
			}

			// 更改写缓冲区读指针
			m_sendBuf.m_rp += nBytes;

			m_sendBytes += nBytes;
			m_lastestSendTick = GetTickCount();

			if(nBytes < len)
			{
				m_sendBuf.Compact();
				break;
			}

			// 重置SendBuff
			m_sendBuf.Reset();
		}
		else
		{
			if(this->PrepareSendBuffer() < 1)
				break;
		}
	}

	m_inWriting = FALSE;
	return;
}

BOOL KSelectTcpSocket::IsWritable()
{
	fd_set wfds;
	struct timeval timeo = {0,0};

	FD_ZERO(&wfds);
	FD_SET(m_sock, &wfds);

	int nRet = select((int)m_sock+1, NULL, &wfds, NULL, &timeo);
	if(nRet == 1 && FD_ISSET(m_sock, &wfds)) return TRUE;
	return FALSE;
}

KSelectUdpSocket::KSelectUdpSocket()
{
	memset(&m_peerAddr, 0, sizeof(m_peerAddr));
	m_inWriting = FALSE;
	m_writable = TRUE;
}

KSelectUdpSocket::~KSelectUdpSocket()
{

}

BOOL KSelectUdpSocket::Listen(const char* ipName, WORD port)
{
	DWORD ip = KSocket::GetIpByName(ipName);
	if(!ip) return FALSE;
	return this->Listen(ip, port);
}

BOOL KSelectUdpSocket::Listen(DWORD ip, WORD port)
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
	if(nRet)
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

BOOL KSelectUdpSocket::Listen(WORD port)
{
	return this->Listen((DWORD)0, port);
}

void KSelectUdpSocket::SetSendBufferSize(int kSize)
{
	m_sendIOBuffer.SetCapacity(kSize);
}

void KSelectUdpSocket::Reset()
{
	m_sendBuf.Reset();
	m_recvBuf.Reset();
	m_sendIOBuffer.Reset();
	m_inWriting = FALSE;
	m_writable = TRUE;
	KSelectSocket::Reset();
}

KSocket::SocketType KSelectUdpSocket::GetSocketType() const
{
	return KSocket::SOCK_UDP;
}

int KSelectUdpSocket::Send(const void* packet, int len)
{
	ASSERT(FALSE); return 0;
}

int KSelectUdpSocket::SendTo(const void* packet, int len, const KSocketAddress& address)
{
	if(m_sock == INVALID_SOCKET) return 0;
	if(!m_readyForIO) return 0;

	BOOL bBufferFull = FALSE;

	int wanted = sizeof(KUdpPacketHead) + len;

	KUdpPacketHead packetHead;
	memcpy(&packetHead.m_peerAddr, &address, sizeof(KSocketAddress));
	packetHead.m_length = len;

	m_mx.Lock();

	if(m_sendIOBuffer.WriteAvail() < wanted)
	{
		m_mx.Unlock();
		Log(LOG_ERROR, "error: %s send io buffer full", this->ToString().c_str());
		return 0;
	}

	int nBytes = m_sendIOBuffer.Write(&packetHead, sizeof(KUdpPacketHead));
	ASSERT(nBytes == sizeof(KUdpPacketHead));
	nBytes = m_sendIOBuffer.Write(packet, len);
	ASSERT(nBytes == len);

	m_mx.Unlock();
	return len;
}

int KSelectUdpSocket::SendImmediate(const void* packet, int len)
{
	ASSERT(FALSE); return 0;
}

int KSelectUdpSocket::SendToImmediate(const void* packet, int len, const KSocketAddress& address)
{
	if(!this->SendTo(packet, len, address)) return 0;
	this->Write();
	return len;
}

JG_C::KString<256> KSelectUdpSocket::ToString() const
{
	char cTmp[128];
	sprintf(cTmp, "udp socket %u", m_sock);
	return cTmp;
}

BOOL KSelectUdpSocket::OnInvalidPacket(const KSocketAddress* pFrom, ePacketReason reason)
{
	Log(LOG_WARN, "warn: %s invalid packet from %s reason:%d",
		this->ToString().c_str(),
		KSocketAddress::ToString(*pFrom).c_str(),
		reason);
	return TRUE;
}

BOOL KSelectUdpSocket::OnSocketError(DWORD errCode)
{
	Log(LOG_WARN, "warn: %s socket error, %s",
		this->ToString().c_str(),
		strerror_r2(errCode).c_str());
	return TRUE;
}

void KSelectUdpSocket::Read()
{
	socklen_t addrLen;
	sockaddr_in peerAddr;
	KSocketAddress sockAddr;

	while(TRUE)
	{
		addrLen = sizeof(peerAddr);

		int len = m_recvBuf.WriteAvail();
		char* pBuff = m_recvBuf.GetWriteBuffer();
		int readBytes = recvfrom(m_sock, pBuff, len, 0, (sockaddr*)&peerAddr, &addrLen);

		sockAddr = KSocketAddress::FromSockAddr(peerAddr);

		if(readBytes > 0)
		{
			m_recvBytes += readBytes;
			m_lastestRecvTick = GetTickCount();

			m_recvBuf.m_wp += readBytes;

			const void* pData = m_recvBuf.GetReadBuffer();
			int dataLen = m_recvBuf.ReadAvail();

			int nBytes = this->OnRecvData(pData, dataLen, &sockAddr);
			if(nBytes > 0)
			{
				m_recvBuf.m_rp += nBytes;
				m_recvBuf.Compact();
			}
			else if(nBytes < 0)
			{
				this->OnInvalidPacket(&sockAddr, (ePacketReason)nBytes);
				m_recvBuf.Reset();
				return;
			}
		}
		else if(readBytes == 0)
		{
			Log(LOG_WARN, "warn: %s peer closed, close it", this->ToString().c_str());
			this->Close();
			return;
		}
		else
		{
			DWORD errCode = GetLastError();

#if defined(WIN32) || defined(WINDOWS)
			if(errCode == WSAEWOULDBLOCK) return;
#else
			// The  socket  is  marked non-blocking and the receive operation would block, 
			// or a receive timeout had been set and the timeout expired before data was received.
			if(errCode == EAGAIN) return;

			// The receive was interrupted by delivery of a signal before any data were available.
			if(errCode == EINTR)
			{
				Log(LOG_WARN, "warn: %s, %s", this->ToString().c_str(), strerror_r2(errCode).c_str());
				return;
			}
#endif

			this->OnSocketError(errCode);
			m_recvBuf.Reset();
			return;
		}
	}
}

void KSelectUdpSocket::Write()
{
	if(m_sock == INVALID_SOCKET) return;
	if(!m_readyForIO) return;
	if(m_sendBuf.ReadAvail() < 1 && m_sendIOBuffer.Size() < 1) return;

	if(m_inWriting) return;

	m_mx.Lock();
	if(m_inWriting) { m_mx.Unlock(); return; }
	m_inWriting = TRUE;
	m_mx.Unlock();

	if(!m_writable)
	{
		m_writable = this->IsWritable();
		if(!m_writable)
		{
			m_inWriting = FALSE;
			return;
		}
	}
	
	sockaddr_in peerAddr;
	socklen_t addrLen = sizeof(peerAddr);

	while(TRUE)
	{
		int len = m_sendBuf.ReadAvail();
		const char* pData = m_sendBuf.GetReadBuffer();

		if(len > 0)
		{
			int flags = 0;
#if !defined(WIN32) && !defined(__APPLE__)
			flags = MSG_NOSIGNAL;
#endif

			peerAddr = KSocketAddress::ToSockAddr(m_peerAddr);
			int nBytes = sendto(m_sock, pData, len, flags, (const sockaddr*)&peerAddr, addrLen);
			if(nBytes > 0)
			{
				m_sendBytes += nBytes;
				m_lastestSendTick = GetTickCount();

				ASSERT(nBytes == len);
				m_sendBuf.Reset();
			}
			else
			{
				ASSERT(nBytes == -1);
				DWORD errCode = GetLastError();

			#if defined(WIN32) || defined(WINDOWS)
				if(errCode == WSAEWOULDBLOCK)
				{
					m_writable = FALSE;
					break;
				}
			#else
				// The socket is marked non-blocking and the requested operation would block.
				if(errCode == EAGAIN || errCode == EWOULDBLOCK)
				{
					m_writable = FALSE;
					break;
				}
				// A signal occurred before any data was transmitted.
				if(errCode == EINTR) break;
			#endif

				this->OnSocketError(errCode);
				m_sendBuf.Reset();
				break;
			}

			m_sendBuf.Reset();
		}
		else
		{
			KUdpPacketHead packetHead;

			m_sendBuf.Reset();

			m_mx.Lock();
			int nBytes = m_sendIOBuffer.Read(&packetHead, sizeof(KUdpPacketHead));
			if(nBytes < sizeof(KUdpPacketHead))
			{
				ASSERT(nBytes == 0);
				m_mx.Unlock();
				break;
			}

			int packetLength = packetHead.m_length;
			memcpy(&m_peerAddr, &packetHead.m_peerAddr, sizeof(KSocketAddress));

			int len = m_sendBuf.WriteAvail();
			char* pBuff = m_sendBuf.GetWriteBuffer();

			nBytes = m_sendIOBuffer.Read(pBuff, packetLength);
			ASSERT(nBytes == packetLength);
			m_sendBuf.m_wp = packetLength;

			m_mx.Unlock();
		}
	}

	m_inWriting = FALSE;
}

BOOL KSelectUdpSocket::IsWritable()
{
	fd_set wfds;
	struct timeval timeo = {0,0};

	FD_ZERO(&wfds);
	FD_SET(m_sock, &wfds);

	int nRet = select((int)m_sock+1, NULL, &wfds, NULL, &timeo);
	if(nRet == 1 && FD_ISSET(m_sock, &wfds)) return TRUE;
	return FALSE;
}

BOOL KSelectUdpSocket::AutoListen()
{
	WORD port = 35000;
	while(TRUE)
	{
		if(this->Listen(port)) return TRUE;
		port += 1;
	}
	return FALSE;
}

#endif