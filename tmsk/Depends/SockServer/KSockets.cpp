#include "KSockets.h"
#include <System/Log/log.h>
#include <System/Misc/StrUtil.h>
#include "KSockServer.h"
#include "KIocpSockServer.h"
#include "KEpollSockServer.h"
#include "KSelectSockServer.h"

using namespace System::Collections;

KNormalTcpListenSocket::KNormalTcpListenSocket()
{

}

KNormalTcpListenSocket::~KNormalTcpListenSocket()
{

}

void KNormalTcpListenSocket::PrepareSocket(KServerTcpSocket* pSock)
{
	return;
}

BOOL KNormalTcpListenSocket::OnAccepted(SOCKET sock)
{
	KServerTcpSocket* pSock = this->CreateSocketObject();
	if(!pSock)
	{// 创建Socket对象失败，放弃这个连接
		closesocket(sock);
		return FALSE;
	}

	if(!pSock->Attach(sock))
	{
		closesocket(sock);
		pSock->Destroy();
		return FALSE;
	}

	this->PrepareSocket(pSock);

	if(!m_pSockServer->Attach(pSock))
	{// 大概是SockServer满了，放弃这个连接
		pSock->Destroy();
		return FALSE;
	}
	
	return TRUE;
}

KServerTcpSocket::KServerTcpSocket()
{

}

KServerTcpSocket::~KServerTcpSocket()
{

}

KSocket::SocketType KServerTcpSocket::GetSocketType() const
{
	return KSocket::SOCK_SERVER_TCP;
}

BOOL KServerTcpSocket::Attach(SOCKET sock)
{
	this->Reset();

	char buf[128];
	socklen_t socklen = sizeof(buf);
	sockaddr_in& remoteAddr = *(sockaddr_in*)&buf;
	int nRet = getpeername(sock, (sockaddr*)&remoteAddr, &socklen);
	if(nRet != 0)
	{
		DWORD errCode = GetLastError();
		Log(LOG_WARN, "warn: getpeername(%u) %s", sock, strerror_r2(errCode).c_str());
		return FALSE;
	}

	m_sock = sock;
	m_remoteAddress = KSocketAddress::FromSockAddr(remoteAddr);

	return TRUE;
}

KString<256> KServerTcpSocket::ToString() const
{
	char cTmp[256];
	sprintf(cTmp, "Server TCP Socket %u remote: %s",
		m_sock != INVALID_SOCKET ? m_sock : m_sockCopy,
		KSocketAddress::ToString(m_remoteAddress).c_str());
	return cTmp;
}

KClientTcpSocket::KClientTcpSocket()
{
	// 重连时间间隔 ( Second )
	m_reConnInterval = 10;    // seconds
	m_nextConnTick = 0;
	m_startConnTick = (DWORD)-1;
}

KClientTcpSocket::~KClientTcpSocket()
{

}

BOOL KClientTcpSocket::OnConnectError(DWORD errCode)
{
	return TRUE;
}

BOOL KClientTcpSocket::SetRemoteAddress(const char* addrName, WORD port)
{
	DWORD dwIp = KSocket::GetIpByName(addrName);
	if(dwIp == 0)
	{
		Log(LOG_WARN, "warn: fail get ip address for %s", addrName);
		return FALSE;
	}
	m_remoteAddress.m_ip = dwIp;
	m_remoteAddress.m_port = port;
	return TRUE;
}

BOOL KClientTcpSocket::SetRemoteAddress(const KSocketAddress& addr)
{
	m_remoteAddress = addr;
	return TRUE;
}

BOOL KClientTcpSocket::IsConnected() const
{
	return m_sock != INVALID_SOCKET && m_socketState == EnumSocketState::connected;
}

void KClientTcpSocket::SetReconnectInterval(int secs)
{
	m_reConnInterval = secs;
}

void KClientTcpSocket::DisableReconnect()
{
	m_reConnInterval = 0;
}

KSocket::SocketType KClientTcpSocket::GetSocketType() const
{
	return KSocket::SOCK_CLIENT_TCP;
}

void KClientTcpSocket::Reset()
{
	m_nextConnTick = 0;
	m_startConnTick = (DWORD)-1;
	KTcpSocket::Reset();
}

KString<256> KClientTcpSocket::ToString() const
{
	char cTmp[256];
	sprintf_k(cTmp, sizeof(cTmp), "Client TCP Socket %u remote: %s",
		m_sock != INVALID_SOCKET ? m_sock : m_sockCopy,
		KSocketAddress::ToString(m_remoteAddress).c_str());
	return cTmp;
}

void KClientTcpSocket::_onProcessActiveQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval)
{
	switch(m_socketState)
	{
	case EnumSocketState::fresh:
		sockServer->MoveSocketToClose(m_sockIndex);
		break;
	default:
		KTcpSocket::_onProcessActiveQueue(sockServer, nowTicks, msInterval);
		break;
	}
}

void KClientTcpSocket::_onProcessClosedQueue(KBaseSockServer* sockServer, DWORD nowTicks, int msInterval)
{
	switch(m_socketState)
	{
	case EnumSocketState::fresh:
		{
			if(!this->Connect())
			{
				this->OnConnectError(GetLastError());
				m_socketState = EnumSocketState::closed;
				if(m_reConnInterval) m_nextConnTick = nowTicks + m_reConnInterval * 1000;
			}
			else
			{
				m_socketState = EnumSocketState::connecting;
			}
		} break;
	case EnumSocketState::connecting:
		{
			int n = this->CheckConnected();
			if(n == 0)
			{
				break;
			}
			else if(n == 1)
			{
				sockServer->MoveSocketToActive(m_sockIndex);
			}
			else
			{
				this->_Close();
				m_socketState = EnumSocketState::closed;
				m_nextConnTick = nowTicks + m_reConnInterval*1000;
			}
		} break;
	case EnumSocketState::closed:
		{
			if(!m_reConnInterval)
			{
				KTcpSocket::_onProcessClosedQueue(sockServer, nowTicks, msInterval);
				return;
			}

			// 时间没到，继续等待
			if(nowTicks < m_nextConnTick) return;

			// 开始一个异步的连接过程
			m_nextConnTick = nowTicks + m_reConnInterval * 1000;
			if(!this->Connect())
			{
				this->OnConnectError(GetLastError());
				m_nextConnTick = nowTicks + m_reConnInterval * 1000;
				m_socketState = EnumSocketState::closed;
			}
			else
			{
				m_socketState = EnumSocketState::connecting;
			}
		} break;
	default:
		KTcpSocket::_onProcessClosedQueue(sockServer, nowTicks, msInterval);
		break;
	}
}

void KClientTcpSocket::OnStartWork()
{
	KTcpSocket::OnStartWork();
}

BOOL KClientTcpSocket::IsWouldBlock(DWORD errCode) const
{
#if defined(WIN32) || defined(WINDOWS)
	return errCode == WSAEWOULDBLOCK;
#else
	return errCode == EINPROGRESS;
#endif
}

BOOL KClientTcpSocket::Connect()
{
	ASSERT(m_sock == INVALID_SOCKET);
	m_startConnTick = GetTickCount();

	SOCKET sock = KSocket::CreateTcpSocket();
	if(sock == INVALID_SOCKET)
	{
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: KSocket::CreateTcpSocket errCode:%u, %s", errCode, strerror_r2(errCode).c_str());
		return FALSE;
	}

	sockaddr_in peer = KSocketAddress::ToSockAddr(m_remoteAddress);
	int socklen = sizeof(peer);

	int nRet = connect(sock, (sockaddr*)&peer, socklen);
	if(nRet == 0 || this->IsWouldBlock(GetLastError()))
	{
		m_sock = sock;
		return TRUE;
	}

	DWORD errCode = GetLastError();
	Log(LOG_DEBUG, "debug: connect errCode:%u, %s", errCode, strerror_r2(errCode).c_str());
	closesocket(sock);

	return FALSE;
}

#if defined(WIN32)
int KClientTcpSocket::CheckConnected()
{
	fd_set wfds; FD_ZERO(&wfds);
	fd_set efds; FD_ZERO(&efds);
	struct timeval timedout= {0, 0};

	FD_SET(m_sock, &wfds);
	FD_SET(m_sock, &efds);

	int nRet = select((int)m_sock+1, NULL, &wfds, &efds, &timedout);
	if(nRet > 0)
	{
		if(FD_ISSET(m_sock, &efds))
		{
			this->OnConnectError(this->GetSocketError());
			return -1;
		}
		if(FD_ISSET(m_sock, &wfds))
		{
			// linux下面返回可写也不一定连接成功了，还需要其他的检查
			DWORD err = this->GetSocketError();
			if(!err)
			{
				return 1;
			}
			else
			{
				this->OnConnectError(err);
				return -1;
			}
		}
		
		// ASSERT(FALSE); // 应该是不可达的
		Log(LOG_ERROR, "error: check connected, select return %d, check fail, %s", nRet, this->ToString().c_str());
		this->OnConnectError(9999);
		return -1;
	}
	else if(nRet == 0)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}
#else
#include <poll.h>
int KClientTcpSocket::CheckConnected()
{
	struct pollfd fds[1];
	fds[0].fd = m_sock;
	fds[0].events = POLLOUT | POLLERR | POLLNVAL | POLLHUP;
	fds[0].revents = 0;
	int nRet = poll(fds, 1, 0);
	switch(nRet)
	{
	case 0:
		return 0;
	case 1:
		if(fds[0].revents & POLLOUT)
		{
			return 1;
		}
		else if(fds[0].revents)
		{
			DWORD errCode = this->GetSocketError();
			this->OnConnectError(errCode);
			return -1;
		}
		else
		{
			Log(LOG_ERROR, "error: check connected, select return %d, check fail, %s", nRet, this->ToString().c_str());
			DWORD errCode = this->GetSocketError();
			this->OnConnectError(errCode);
			return -1;
		}
	case -1:
		{
			DWORD errCode = GetLastError();
			this->OnConnectError(errCode);
			return -1;
		}
	default:
		{
			Log(LOG_ERROR, "error: check connected, select return %d, check fail, %s", nRet, this->ToString().c_str());
			DWORD errCode = this->GetSocketError();
			this->OnConnectError(errCode);
			return -1;
		}
	}
	return 0;
}
#endif
