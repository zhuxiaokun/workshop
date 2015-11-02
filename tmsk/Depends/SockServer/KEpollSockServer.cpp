#include "KEpollSockServer.h"
#ifndef _USE_SELECT_
#if !defined(WIN32) && !defined(WINDOWS)
#include <signal.h>
#include <System/Log/log.h>
#include <System/Misc/StrUtil.h>
#include <System/Memory/KThreadLocalStorage.h>

#ifndef EPOLLRDHUP
#define EPOLLRDHUP 0x2000
#endif

KEpollIoThread::KEpollIoThread()
{
	m_stopDesired = FALSE;
	m_pSockServer = NULL;
	m_startSockIndex = -1;
	m_sockCount = 0;
	m_epollDesc = -1;
}

KEpollIoThread::~KEpollIoThread()
{
	this->Finalize();
}

BOOL KEpollIoThread::Initialize(KEpollSockServer* pSockServer, int startSockIdx, int sockCount)
{
	ASSERT(m_epollDesc == -1);

	m_pSockServer = pSockServer;
	m_startSockIndex = startSockIdx;
	m_sockCount = sockCount;

	m_epollDesc = epoll_create(m_sockCount);
	if(m_epollDesc == -1)
	{
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: epoll_create, %s", strerror_r2(errCode).c_str());
		return FALSE;
	}

	this->Run();
	return TRUE;
}

void KEpollIoThread::Finalize()
{
	this->Stop();
	if(m_epollDesc != -1)
	{
		close(m_epollDesc);
		m_epollDesc = -1;
	}	
}	

void KEpollIoThread::Execute()
{
	const int timeout = 10;
	m_lastBreatheTick = GetTickCount();
	struct epoll_event* pEvents = &m_pSockServer->m_events[m_startSockIndex];
	KEpollSocket** ppActiveSocks = &m_pSockServer->m_ppActiveSock[m_startSockIndex];

	DWORD startTick = GetTickCount();
	
	while(!m_stopDesired)
	{
		DWORD now = GetTickCount();
		if(now - startTick >= 2000)
		{
			Log(LOG_ALL, "warn: socket io thread breathing takes more than 2000 mills diff =%u", now - startTick);
		}
		startTick = now;

		int intervalTicks = now - m_lastBreatheTick;

		this->ProcessActiveQueue(now, intervalTicks);
		this->ProcessClosedQueue(now, intervalTicks);

		m_lastBreatheTick = GetTickCount();

		int num = epoll_wait(m_epollDesc, pEvents, m_sockCount, timeout);
		if(num > 0)
		{
			for(int i=0; i<num; i++)
			{
				struct epoll_event& evt = pEvents[i];
				KEpollSocket* pSock = (KEpollSocket*)evt.data.ptr;

				if(evt.events & EPOLLERR)
				{
					pSock->OnSocketError(pSock->GetSocketError());
					continue;
				}

				if(evt.events & EPOLLRDHUP)
				{// Stream socket peer closed connection, or shut down writing half of connection.
					pSock->OnSocketError(ECONNRESET); //pSock->Close();
					continue;
				}

				if(evt.events & EPOLLIN)
				{
					pSock->Read();
				}
				
				// 不可写入的SOCKET状态发生了变化，现在可以写入了
				if(evt.events & EPOLLOUT)
				{
					pSock->EnableWritable();
					if(pSock->m_debugState)
					{
						Log(LOG_WARN, "warn: %s, change to writable", pSock->ToString().c_str());
					}
				}
			}
		}
		else if(num == 0)
		{
			// timeout
		}
		else
		{
			DWORD errCode = GetLastError();
			if(errCode != EINTR)
			{
				Log(LOG_ERROR, "error: epoll_wait, %s", strerror_r2(errCode).c_str());
			}
		}
	}
}

void KEpollIoThread::Stop()
{
	m_stopDesired = TRUE;
	this->WaitFor();
	m_stopDesired = FALSE;
}

void KEpollIoThread::ProcessActiveQueue(DWORD nowTicks, int intervalTicks)
{
	KEpollSocket** ppActive = m_pSockServer->m_ppActiveSock;
	for(int i=0; i<m_sockCount; i++)
	{
		int idx = m_startSockIndex + i;
		KEpollSocket* pSock = ppActive[idx]; if(!pSock) continue;
		if(pSock->m_closeDesired) pSock->_doClose();
		pSock->_onProcessActiveQueue(m_pSockServer, nowTicks, intervalTicks);
	}
}

void KEpollIoThread::ProcessClosedQueue(DWORD nowTicks, int intervalTicks)
{
	KEpollSocket** ppClosedSock = m_pSockServer->m_ppClosedSock;
	for(int i=0; i<m_sockCount; i++)
	{
		int idx = m_startSockIndex + i;
		KEpollSocket* pSock = ppClosedSock[idx]; if(!pSock) continue;
		if(pSock->m_closeDesired) pSock->_doClose();
		pSock->_onProcessClosedQueue(m_pSockServer, nowTicks, intervalTicks);
	}
}

KEpollSockServer::KEpollSockServer()
{
	m_ioThreadNum = 0;
	m_socketCapacity = 0;
	m_socketCount = 0;
	m_ppIoThread = NULL;
	m_ppActiveSock = NULL;
	m_ppClosedSock = NULL;
	m_events = NULL;
	m_tlsSigPipe = -1;
}

KEpollSockServer::~KEpollSockServer()
{
	this->Finalize();
}

BOOL KEpollSockServer::Initialize(int socketCapacity, int ioThreadNum)
{
	m_ioThreadNum = ioThreadNum;
	m_socketCapacity = socketCapacity;

	int startSockIndex = 0;
	int sockCount = (m_socketCapacity+m_ioThreadNum-1) / m_ioThreadNum;

	m_ppActiveSock = new KEpollSocket*[m_socketCapacity]; ASSERT(m_ppActiveSock);
	memset(m_ppActiveSock, 0, sizeof(KEpollSocket*)*m_socketCapacity);

	m_ppClosedSock = new KEpollSocket*[m_socketCapacity]; ASSERT(m_ppClosedSock);
	memset(m_ppClosedSock, 0, sizeof(KEpollSocket*)*m_socketCapacity);

	m_events = new epoll_event[m_socketCapacity]; ASSERT(m_events);
	memset(m_events, 0, sizeof(epoll_event)*m_socketCapacity);

	m_ppIoThread = new KEpollIoThread*[m_ioThreadNum]; ASSERT(m_ppIoThread);
	for(int i=0; i<m_ioThreadNum; i++)
	{
		int sockNum = sockCount;
		if(startSockIndex + sockNum > m_socketCapacity)
		{
			sockNum = m_socketCapacity - startSockIndex;
		}
		 
		KEpollIoThread*& pIoThread = m_ppIoThread[i];
		pIoThread = new KEpollIoThread(); ASSERT(pIoThread);
		pIoThread->Initialize(this, startSockIndex, sockNum);

		startSockIndex += sockCount;
	}

	return TRUE;
}

void KEpollSockServer::Finalize()
{
	if(m_ppIoThread)
	{
		for(int i=0; i<m_ioThreadNum; i++)
		{
			KEpollIoThread* pThread = m_ppIoThread[i];
			pThread->Finalize();
			delete pThread;
		}
		delete [] m_ppIoThread;
		m_ppIoThread = NULL;
	}

	if(m_events)
	{
		delete [] m_events;
		m_events = NULL;
	}

	if(m_ppClosedSock)
	{
		for(int i=0; i<m_socketCapacity; i++)
		{
			KSocket* pSock = m_ppClosedSock[i];
			if(!pSock) continue;
			pSock->Close();
			pSock->Destroy();
			m_ppClosedSock[i] = NULL;
		}
		delete [] m_ppClosedSock;
		m_ppClosedSock = NULL;
	}

	if(m_ppActiveSock)
	{
		for(int i=0; i<m_socketCapacity; i++)
		{
			KSocket* pSock = m_ppActiveSock[i];
			if(!pSock) continue;
			pSock->Close();
			pSock->Destroy();
			m_ppActiveSock[i] = NULL;
		}
		delete [] m_ppActiveSock;
		m_ppActiveSock = NULL;
	}

	m_socketCount = 0;
	m_socketCapacity = 0;
	m_ioThreadNum = 0;
}

BOOL KEpollSockServer::Attach(KSocket* pSock)
{
	KEpollSocket* pEpollSock = (KEpollSocket*)pSock;
	pEpollSock->m_pSockServer = this;

	BOOL bRet = FALSE;
	{
		switch(pSock->GetSocketType())
		{
		case KSocket::SOCK_TCP_LISTEN:
			bRet = this->AttachTcpListenSocket((KTcpListenSocket*)pSock);
			break;
		case KSocket::SOCK_SERVER_TCP:
			bRet = this->AttachServerTcpSocket((KServerTcpSocket*)pSock);
			break;
		case KSocket::SOCK_CLIENT_TCP:
			bRet = this->AttachClientTcpSocket((KClientTcpSocket*)pSock);
			break;
		case KSocket::SOCK_UDP:
			bRet = this->AttachUdpSocket((KUdpSocket*)pSock);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	if(bRet)
	{
		pSock->OnAttached();
		pSock->m_socketState = EnumSocketState::fresh;
		if(!this->CanAcceptSocket())
			Log(LOG_WARN, "warn: sock server is full, can not accept more socket, %d/%d",
				m_socketCapacity, m_socketCount);
	}
	return bRet;
}

int KEpollSockServer::GetSocketCapacity() const
{
	return m_socketCapacity;
}

int KEpollSockServer::GetCurrentSocketCount() const
{
	return m_socketCount;
}

BOOL KEpollSockServer::_Detach(KSocket* pSock)
{
	if(pSock->m_refCount > 0) return FALSE;
	JG_S::KAutoThreadMutex mx(m_mxSockets);
	JG_S::KAutoThreadMutex mx2(pSock->m_mx);
	if(!pSock->_canDestroy()) return FALSE;
	m_ppClosedSock[pSock->m_sockIndex] = NULL;
	m_socketCount -= 1;
	return TRUE;
}

BOOL KEpollSockServer::CanAcceptSocket() const
{
	return m_socketCount < m_socketCapacity;
}

// This method is only called for client tcp socket
void KEpollSockServer::MoveSocketToActive(int sockIndex)
{
	JG_S::KAutoThreadMutex mx(m_mxSockets);
	ASSERT(m_ppClosedSock[sockIndex]);
	ASSERT(m_ppActiveSock[sockIndex] == NULL);
	KEpollSocket* pSock = m_ppClosedSock[sockIndex];
	m_ppActiveSock[sockIndex] = pSock;
	m_ppClosedSock[sockIndex] = NULL;
}

void KEpollSockServer::MoveSocketToClose(int sockIndex)
{
	JG_S::KAutoThreadMutex mx(m_mxSockets);
	ASSERT(m_ppActiveSock[sockIndex]);
	ASSERT(m_ppClosedSock[sockIndex] == NULL);
	KEpollSocket* pSock = m_ppActiveSock[sockIndex];
	ASSERT(pSock->m_sock == INVALID_SOCKET);
	m_ppActiveSock[sockIndex] = NULL;
	m_ppClosedSock[sockIndex] = pSock;
}

BOOL KEpollSockServer::AttachTcpListenSocket(KTcpListenSocket* pSock)
{
	JG_S::KAutoThreadMutex mx(m_mxSockets);

	int sockIndex = this->_GetAvailSockIndex();
	if(sockIndex < 0)
	{
		Log(LOG_WARN, "warn: SockServer full, %s", pSock->ToString().c_str());
		return FALSE;
	}

	pSock->m_sockIndex = sockIndex;
	pSock->m_pSockServer = this;
	
	m_ppActiveSock[sockIndex] = pSock;
	m_ppClosedSock[sockIndex] = NULL;
	m_socketCount += 1;

	ASSERT(pSock->m_sock != INVALID_SOCKET);
	return TRUE;
}

BOOL KEpollSockServer::AttachServerTcpSocket(KServerTcpSocket* pSock)
{
	JG_S::KAutoThreadMutex mx(m_mxSockets);

	int sockIndex = this->_GetAvailSockIndex();
	if(sockIndex < 0)
	{
		Log(LOG_WARN, "warn: SockServer full, %s", pSock->ToString().c_str());
		pSock->Close();
		return FALSE;
	}

	pSock->m_sockIndex = sockIndex;
	pSock->m_pSockServer = this;
	pSock->m_sendIOBuffer.SetBufferPool(m_pieceBufferPool);

	m_ppActiveSock[sockIndex] = pSock;
	m_ppClosedSock[sockIndex] = NULL;
	m_socketCount += 1;

	ASSERT(pSock->m_sock != INVALID_SOCKET);
	return TRUE;
}

BOOL KEpollSockServer::AttachUdpSocket(KUdpSocket* pSock)
{
	// 这个SOCKET尚未绑定到端口，自动找一个
	if(pSock->m_sock == INVALID_SOCKET)
	{
		pSock->AutoListen();
	}

	JG_S::KAutoThreadMutex mx(m_mxSockets);

	int sockIndex = this->_GetAvailSockIndex();
	if(sockIndex < 0)
	{
		Log(LOG_WARN, "warn: SockServer full, %s", pSock->ToString().c_str());
		pSock->Close();
		return FALSE;
	}

	pSock->m_sockIndex = sockIndex;
	pSock->m_pSockServer = this;
	pSock->m_sendIOBuffer.SetBufferPool(m_pieceBufferPool);

	m_ppActiveSock[sockIndex] = pSock;
	m_ppClosedSock[sockIndex] = NULL;
	m_socketCount += 1;

	ASSERT(pSock->m_sock != INVALID_SOCKET);
	return TRUE;
}

BOOL KEpollSockServer::AttachClientTcpSocket(KClientTcpSocket* pSock)
{
	ASSERT(pSock->m_sock == INVALID_SOCKET);

	JG_S::KAutoThreadMutex mx(m_mxSockets);

	int sockIndex = this->_GetAvailSockIndex();
	if(sockIndex < 0)
	{
		Log(LOG_WARN, "warn: SockServer full, %s", pSock->ToString().c_str());
		return FALSE;
	}

	pSock->m_sockIndex = sockIndex;
	pSock->m_pSockServer = this;
	pSock->m_sendIOBuffer.SetBufferPool(m_pieceBufferPool);

	m_ppActiveSock[sockIndex] = pSock;
	m_ppClosedSock[sockIndex] = NULL;
	m_socketCount += 1;

	return TRUE;
}

BOOL KEpollSockServer::AddToEpoll(KEpollSocket* pSock)
{
	ASSERT(pSock && pSock->m_sock != INVALID_SOCKET);
	
	KEpollIoThread& ioThread = this->GetResponsibilityThread(pSock->m_sockIndex);

	struct epoll_event evt;
	evt.data.ptr = pSock;
	evt.events = EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLET | EPOLLRDHUP;
	//evt.events = EPOLLIN | EPOLLERR | EPOLLET;

	int nRet = epoll_ctl(ioThread.m_epollDesc, EPOLL_CTL_ADD, pSock->m_sock, &evt);
	if(nRet == -1)
	{
		DWORD errCode = GetLastError();
		Log(LOG_WARN, "warn: %s add to epoll, %s", pSock->ToString().c_str(), strerror_r2(errCode).c_str());
		return FALSE;
	}

	return TRUE;
}

BOOL KEpollSockServer::DelFromEpoll(KEpollSocket* pSock)
{
	SOCKET sock = pSock->m_sock;
	if(sock == INVALID_SOCKET)
	{
		sock = pSock->m_sockCopy;
	}

	ASSERT(sock != INVALID_SOCKET);
	
	KEpollIoThread& ioThread = this->GetResponsibilityThread(pSock->m_sockIndex);

	if(epoll_ctl(ioThread.m_epollDesc, EPOLL_CTL_DEL, sock, NULL) == -1)
	{
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: epoll_ctrl del %s, %s", pSock->ToString().c_str(), strerror_r2(errCode).c_str());
		return FALSE;
	}

	return TRUE;
}

int KEpollSockServer::_GetAvailSockIndex()
{
	if(m_socketCount >= m_socketCapacity)
		return -1;

	int tryCount = m_ioThreadNum * 2;
	while(tryCount > 0)
	{
		int sockIndex = m_random.GetRandom(0, m_socketCapacity);
		if(!m_ppActiveSock[sockIndex] && !m_ppClosedSock[sockIndex])
		{
			return sockIndex;
		}
		tryCount -= 1;
	}
	
	for(int i=0; i<m_socketCapacity; i++)
	{
		if(!m_ppActiveSock[i] && !m_ppClosedSock[i])
		{
			return i;
		}
	}
	return -1;
}

KEpollIoThread& KEpollSockServer::GetResponsibilityThread(int sockIndex)
{
	ASSERT(sockIndex >= 0 && sockIndex < m_socketCapacity);
	
	int sockCount = (m_socketCapacity+m_ioThreadNum-1) / m_ioThreadNum;
	int threadNum = sockIndex / sockCount;
	
	return *m_ppIoThread[threadNum];
}

#endif
#endif
