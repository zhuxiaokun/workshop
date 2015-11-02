#include "KEpollSocket.h"
#ifndef _USE_SELECT_
#if !defined(WIN32) && !defined(WINDOWS)
#include <System/Log/log.h>
#include <System/Misc/StrUtil.h>
#include "KEpollSockServer.h"

KEpollSocket::KEpollSocket()
{
	m_pSockServer = NULL;
	m_writable = FALSE;
	m_inWriting = FALSE;
	this->Reset();
}

KEpollSocket::~KEpollSocket()
{

}

KBaseSockServer* KEpollSocket::GetSockServer()
{
	return m_pSockServer;
}

void KEpollSocket::Reset()
{
	m_pSockServer = NULL;
	m_writable = FALSE;
	m_inWriting = FALSE;
	KSocket::Reset();
}

void KEpollSocket::OnStartWork()
{
	m_inWriting = FALSE;
	KSocket::OnStartWork();
}

void KEpollSocket::EnableWritable()
{
	m_writable = TRUE;
}

void KEpollSocket::DisableWritable()
{
	m_writable = FALSE;
}

void KEpollSocket::_onProcessActiveQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval)
{
	KEpollSockServer* epollSockServer = (KEpollSockServer*)sockServer;
	switch(m_socketState)
	{
	case EnumSocketState::none: break;
	case EnumSocketState::fresh:
		this->OnStartWork();
		m_readyForIO = TRUE; this->OnConnected(); m_socketState = EnumSocketState::connected;
		epollSockServer->AddToEpoll(this); // 只有加入到EPOLL中后，才能真正开始IO
		break;
	case EnumSocketState::connecting:
		{
			ASSERT(this->GetSocketType() == KSocket::SOCK_CLIENT_TCP);
			this->OnStartWork();
			m_readyForIO = TRUE; this->OnConnected(); m_socketState = EnumSocketState::connected;
			epollSockServer->AddToEpoll(this); // 只有加入到EPOLL中后，才能真正开始IO
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
		m_pSockServer->DelFromEpoll(this);
		m_socketState = EnumSocketState::closed;
		this->OnClosed();
		epollSockServer->MoveSocketToClose(m_sockIndex);
		break;
	case EnumSocketState::closed: // 不是从 connecting 状态来的
		epollSockServer->MoveSocketToClose(m_sockIndex);
		break;
	case EnumSocketState::destroying:
		ASSERT(FALSE); break;
	default:
		ASSERT(FALSE); break;
	}
}

void KEpollSocket::_onProcessClosedQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval)
{
	KEpollSockServer* epollSockServer = (KEpollSockServer*)sockServer;
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
		if(epollSockServer->_Detach(this))
		{
			Log(LOG_WARN, "warn: destroy %s", this->ToString().c_str());
			this->OnDetached();
			this->Destroy();
		} break;
	default:
		ASSERT(FALSE); break;
	}
}

/// KEpollTcpListenSocket

KEpollTcpListenSocket::KEpollTcpListenSocket():m_listenIp(0),m_listenPort(0)
{

}

KEpollTcpListenSocket::~KEpollTcpListenSocket()
{

}

BOOL KEpollTcpListenSocket::Listen(WORD port)
{
	return this->Listen((DWORD)INADDR_ANY, port);
}

BOOL KEpollTcpListenSocket::Listen(DWORD ip, WORD port)
{
	ASSERT(m_sock == INVALID_SOCKET);

	KSocketAddress sockAddr = {ip, port};
	sockaddr_in localAddr = KSocketAddress::ToSockAddr(sockAddr);

	SOCKET sock = KSocket::CreateTcpSocket();
	if(sock == INVALID_SOCKET) return FALSE;

	if(bind(sock, (sockaddr*)&localAddr, sizeof(localAddr)) != 0)
	{
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: bind to %s, %s", KSocketAddress::ToString(sockAddr).c_str(), strerror_r2(errCode).c_str());
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

BOOL KEpollTcpListenSocket::Listen(const char* addrName, WORD port)
{
	DWORD dwIp = KSocket::GetIpByName(addrName);
	if(dwIp == 0) return FALSE;
	return this->Listen(dwIp, port);
}

KSocket::SocketType KEpollTcpListenSocket::GetSocketType() const
{
	return KSocket::SOCK_TCP_LISTEN;
}

int KEpollTcpListenSocket::Send(const void* packet, int len)
{
	ASSERT(FALSE);
	return 0;
}

int KEpollTcpListenSocket::SendTo(const void* packet, int len, const KSocketAddress& address)
{
	ASSERT(FALSE);
	return 0;
}

void KEpollTcpListenSocket::Reset()
{
	m_listenIp = 0;
	m_listenPort = 0;
	KEpollSocket::Reset();
}

JG_C::KString<256> KEpollTcpListenSocket::ToString() const
{
	char cTmp[256];
	KSocketAddress addr = {m_listenIp, m_listenPort};
	sprintf_k(cTmp, sizeof(cTmp), "TCP Listen Socket %u listen: %s", m_sock != INVALID_SOCKET ? m_sock : m_sockCopy, KSocketAddress::ToString(addr).c_str());
	return cTmp;
}

void KEpollTcpListenSocket::Read()
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

	// 读完了
	if(errCode == EAGAIN) return;

	switch(errCode)
	{
	case EMFILE://system file limit
		ASSERT(FALSE); break;
	case ENFILE:// user file limit
		ASSERT(FALSE); break;
	case ENOBUFS:
		ASSERT(FALSE); break;
	case ENOMEM:
		ASSERT(FALSE); break;
	case EPERM: //firewall settings
		ASSERT(FALSE); break;
	};

	// 出错了
	this->OnSocketError(errCode);

	return;
}

void KEpollTcpListenSocket::Write()
{
	return;
}

void KEpollTcpListenSocket::OnStartWork()
{
	KEpollSocket::OnStartWork();
}

KEpollTcpSocket::KEpollTcpSocket()
{
	m_remoteAddress.m_ip = 0;
	m_remoteAddress.m_port = 0;
}

KEpollTcpSocket::~KEpollTcpSocket()
{

}

KSocket* KEpollTcpSocket::GetSocket()
{
	return this;
}

int KEpollTcpSocket::Send(const void* packet, int len)
{
	if(m_sock == INVALID_SOCKET || !m_readyForIO)
	{
		return 0;
	}

	m_mx.Lock();
	int nBytes = m_sendIOBuffer.Write(packet, len);
	m_mx.Unlock();

	if(nBytes < len)
	{// Socket的IOBuffer满了，关闭连接
		Log(LOG_WARN, "warn: %s send io buffer full, close it", this->ToString().c_str());
		this->Close();
		return 0;
	}

	return nBytes;
}

int KEpollTcpSocket::SendTo(const void* packet, int len, const KSocketAddress& address)
{
	ASSERT(FALSE);
	return 0;
}

// 不支持多线程并发进入Write函数
int KEpollTcpSocket::SendImmediate(const void* packet, int len)
{
	if(!this->Send(packet, len)) return 0;
	//this->Write();
	return len;
}

void KEpollTcpSocket::Reset()
{
	KTcpSocketState::Reset();
	KEpollSocket::Reset();
}

// 从SOCKET中读取数据
void KEpollTcpSocket::Read()
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
		}
		else if(readBytes == 0) // 对端关闭了这个连接
		{
			Log(LOG_WARN, "warn: %s, peer closed, close it", this->ToString().c_str());
			this->OnSocketError(ECONNRESET); //this->Close();
			return;
		}
		else
		{
			DWORD errCode = GetLastError();
			
			// The receive was interrupted by delivery of a signal before any data were available.
			if(errCode == EINTR)
			{
				Log(LOG_WARN, "warn: %s, %s", this->ToString().c_str(), strerror_r2(errCode).c_str());
				continue;
			}

			// The  socket  is  marked non-blocking and the receive operation would block,
			// or a receive timeout had been
			// set and the timeout expired before data was received.
			if(errCode == EAGAIN) return;

			this->OnSocketError(errCode);
			return;
		}
	}
}

void KEpollTcpSocket::Write()
{
	// 检查Socket是否已经关闭了
	if(m_sock == INVALID_SOCKET || !m_readyForIO) return;

	// 不可写
	if(!m_writable) return;
	if(m_sendPaused) return;

	// 现在没有数据等待发送
	if(!this->HasDataToSend()) return;

	// 别的线程正在发送
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
		m_inWriting = FALSE;
		return;
	}

	while(TRUE)
	{
		int len = m_sendBuf.ReadAvail();
		const char* pBuff = m_sendBuf.GetReadBuffer();

		if(len > 0)
		{
			//int wantSend = len < 256 ? len : 256;
			int nBytes = send(m_sock, pBuff, len, MSG_NOSIGNAL);
			
			if(nBytes == -1)
			{
				DWORD errCode = GetLastError();

				// A signal occurred before any data was transmitted.
				if(errCode == EINTR)
				{
					Log(LOG_WARN, "warn: %s, %s", this->ToString().c_str(), strerror_r2(errCode).c_str());
					continue;
				}

				// The socket is marked non-blocking and the requested operation would block.
				if(errCode == EAGAIN || errCode == EWOULDBLOCK)
				{
					if(m_debugState)
					{
						Log(LOG_WARN, "warn: %s would block, send buf:%d", this->ToString().c_str(), m_sendIOBuffer.Size());
					}
					
					this->OnWouldBlock();
					this->DisableWritable();

					break;
				}

				this->OnSocketError(errCode);
				break;
			}

			m_sendBytes += nBytes;
			m_lastestSendTick = GetTickCount();

			// 更改写缓冲区读指针
			m_sendBuf.m_rp += nBytes;
			m_sendBuf.Compact();
		}
		else
		{
			if(this->PrepareSendBuffer() < 1)
				break;
		}
	}

	m_inWriting = FALSE;
}

void KEpollTcpSocket::OnStartWork()
{
	m_sendIOBuffer.Reset();
	m_sendBuf.Reset();
	m_recvBuf.Reset();
	KEpollSocket::OnStartWork();
}

KEpollUdpSocket::KEpollUdpSocket()
{
	m_peerAddr.m_ip = 0;
	m_peerAddr.m_port = 0;
}

KEpollUdpSocket::~KEpollUdpSocket()
{

}

void KEpollUdpSocket::SetSendBufferSize(int kSize)
{
	m_sendIOBuffer.SetCapacity(kSize);
}

BOOL KEpollUdpSocket::Attach(SOCKET sock)
{
	ASSERT(sock != INVALID_SOCKET);
	m_sock = sock;
	return TRUE;
}

BOOL KEpollUdpSocket::AutoListen()
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

BOOL KEpollUdpSocket::Listen(WORD port)
{
	return this->Listen(INADDR_ANY, port);
}

BOOL KEpollUdpSocket::Listen(DWORD ip, WORD port)
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
	{	// ??????
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

KSocket::SocketType KEpollUdpSocket::GetSocketType() const
{
	return KSocket::SOCK_UDP;
}

int KEpollUdpSocket::Send(const void* packet, int len)
{
	ASSERT(FALSE);
	return 0;
}

int KEpollUdpSocket::SendTo(const void* packet, int len, const KSocketAddress& address)
{
	if(m_sock == INVALID_SOCKET || !m_readyForIO) return 0;

	BOOL bBufferFull = FALSE;
	int wanted = sizeof(KUdpPacketHead) + len;

	KUdpPacketHead packetHead;
	memcpy(&packetHead.m_peerAddr, &address, sizeof(KSocketAddress));
	packetHead.m_length = len;

	m_mx.Lock();
	if(m_sendIOBuffer.WriteAvail() < wanted)
	{
		bBufferFull = TRUE;
	}
	else
	{
		int nBytes = m_sendIOBuffer.Write(&packetHead, sizeof(KUdpPacketHead));
		ASSERT(nBytes == sizeof(KUdpPacketHead));
		nBytes = m_sendIOBuffer.Write(packet, len);
		ASSERT(nBytes == len);
	}
	m_mx.Unlock();

	if(bBufferFull)
	{
		Log(LOG_WARN, "warn: %s send buffer full, close it", this->ToString().c_str());
		//this->Close();
		return 0;
	}

	//this->Write();
	return len;
}

int KEpollUdpSocket::SendToImmediate(const void* packet, int len, const KSocketAddress& address)
{
	if(!this->SendTo(packet, len, address)) return 0;
	this->Write();
	return len;
}

void KEpollUdpSocket::Reset()
{
	m_peerAddr.m_ip = 0;
	m_peerAddr.m_port = 0;
	m_sendIOBuffer.Reset();
	m_sendBuf.Reset();
	m_recvBuf.Reset();
	KEpollSocket::Reset();
}

JG_C::KString<256> KEpollUdpSocket::ToString() const
{
	char cTmp[128];
	sprintf(cTmp, "UDP %u", m_sock != INVALID_SOCKET ? m_sock : m_sockCopy);
	return cTmp;
}

void KEpollUdpSocket::Read()
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
				//return; // 需要继续读取
			}
		}
		else if(readBytes == 0)
		{
			Log(LOG_WARN, "warn: %s peer closed, close it", this->ToString().c_str());
			//this->Close();
			//return;
		}
		else
		{
			DWORD errCode = GetLastError();
			
			// The  socket  is  marked non-blocking and the receive operation would block, 
			// or a receive timeout had been set and the timeout expired before data was received.
			if(errCode == EAGAIN) return;
			
			// The receive was interrupted by delivery of a signal before any data were available.
			if(errCode == EINTR)
			{
				Log(LOG_WARN, "warn: %s, %s", this->ToString().c_str(), strerror_r2(errCode).c_str());
				return;
			}

			this->OnSocketError(errCode);
			m_recvBuf.Reset();
			return;
		}
	}
}

void KEpollUdpSocket::Write()
{
	if(m_sock == INVALID_SOCKET) return;
	
	if(!m_writable) return;
	if(m_sendPaused) return;

	if(m_sendBuf.ReadAvail() < 1 && m_sendIOBuffer.Size() < 1) return;

	if(m_inWriting) return;

	m_mx.Lock();
	if(m_inWriting) { m_mx.Unlock(); return; }
	m_inWriting = TRUE;
	m_mx.Unlock();

	if(!m_writable)
	{
		m_inWriting = FALSE;
		return;
	}

	sockaddr_in peerAddr;
	socklen_t addrLen = sizeof(peerAddr);
	
	while(TRUE)
	{
		int len = m_sendBuf.ReadAvail();
		const char* pData = m_sendBuf.GetReadBuffer();

		if(len > 0)
		{
			peerAddr = KSocketAddress::ToSockAddr(m_peerAddr);
			int nBytes = sendto(m_sock, pData, len, MSG_NOSIGNAL, (const sockaddr*)&peerAddr, addrLen);
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
				
				// The socket is marked non-blocking and the requested operation would block.
				if(errCode == EAGAIN || errCode == EWOULDBLOCK)
				{
					Log(LOG_WARN, "warn: %s would block", this->ToString().c_str());
					//m_lastBlockTick = GetTickCount();
					//m_writable = FALSE;
					
					this->DisableWritable();
					this->OnWouldBlock();

					break;
				}

				// A signal occurred before any data was transmitted.
				if(errCode == EINTR)
				{
					Log(LOG_WARN, "warn: %s, %s", this->ToString().c_str(), strerror_r2(errCode).c_str());
					continue;
				}

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

void KEpollUdpSocket::OnStartWork()
{
	m_sendIOBuffer.Reset();
	m_sendBuf.Reset();
	m_recvBuf.Reset();
	KEpollSocket::OnStartWork();
}

#endif
#endif