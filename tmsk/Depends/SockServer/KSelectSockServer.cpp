#include "KSelectSockServer.h"
#if defined(_USE_SELECT_)
#include <System/Misc/StrUtil.h>
#include <System/Log/log.h>
#include "KSockets.h"

KSelectIoThread::KSelectIoThread()
{
	m_startIndex = -1;
	m_sockCount = 0;
	m_pSockServer = NULL;
	m_stopDesired = FALSE;
}

KSelectIoThread::~KSelectIoThread()
{

}

BOOL KSelectIoThread::Initialize(KSelectSockServer* pSockServer, int startSockIdx, int sockCount)
{
	m_pSockServer = pSockServer;
	m_startIndex = startSockIdx;
	m_sockCount = sockCount;
	this->Run();
	return TRUE;
}

void KSelectIoThread::Finalize()
{
	this->Stop();
	m_pSockServer = NULL;
	m_startIndex = 0;
	m_sockCount = 0;
	return;
}

void KSelectIoThread::Execute()
{
	DWORD now;
	int waitTicks;
	struct timeval timeo;
	int checkInterval = 50;
	DWORD lastCheckTick = GetTickCount();
	KSelectSocket** ppSock = m_pSockServer->m_ppActiveSock;

	while(!m_stopDesired)
	{
		DWORD nowTicks = GetTickCount();
		int msInterval = nowTicks - lastCheckTick;

		this->ProcessActiveQueue(nowTicks, msInterval);
		this->ProcessClosedQueue(nowTicks, msInterval);

		int maxSock = this->Build_fdset();
		if(!maxSock)
		{
			waitTicks = lastCheckTick+checkInterval - nowTicks;
			lastCheckTick = nowTicks;
			if(waitTicks > 0) Sleep(waitTicks);
			continue;
		}

		lastCheckTick = nowTicks;
		
		timeo.tv_sec = 0;
		timeo.tv_usec = 50*1000;
		int nRet = select(maxSock+1, &m_rfds, NULL, &m_efds, &timeo);

		if(nRet > 0)
		{
			for(int i=0; i<m_sockCount; i++)
			{
				int idx = m_startIndex + i;
				KSelectSocket* pSock = ppSock[idx];
				if(!pSock) continue;
				SOCKET sock = pSock->m_sock;
				if(FD_ISSET(sock, &m_efds))
				{
					// oob data arrival
					// or connect failed
				}
				if(FD_ISSET(sock, &m_rfds))
				{
					pSock->Read();
				}
			}
		}
		else if(nRet == 0)
		{

		}
		else
		{
			DWORD errCode = GetLastError();
			Log(LOG_ERROR, "error: select(), %s", strerror_r2(errCode).c_str());
		}
	}
}

void KSelectIoThread::Stop()
{
	m_stopDesired = TRUE;
	KThread::WaitFor();
}

int KSelectIoThread::Build_fdset()
{
	int maxSock = 0;
	KSelectSocket** ppSock = m_pSockServer->m_ppActiveSock;

	FD_ZERO(&m_rfds);
	FD_ZERO(&m_efds);

	for(int i=0; i<m_sockCount; i++)
	{
		int idx = m_startIndex + i;
		KSelectSocket* pSock = ppSock[idx];
		if(!pSock || !pSock->m_readyForIO) continue;

		FD_SET(pSock->m_sock, &m_rfds);
		FD_SET(pSock->m_sock, &m_efds);

		if((int)pSock->m_sock > maxSock)
		{
			maxSock = (int)pSock->m_sock;
		}
	}
	
	return maxSock;
}

void KSelectIoThread::ProcessActiveQueue(DWORD nowTicks, int msInterval)
{
	KSelectSocket** ppSock = m_pSockServer->m_ppActiveSock;
	for(int i=0; i<m_sockCount; i++)
	{
		int idx = m_startIndex + i;
		KSelectSocket* pSock = ppSock[idx]; if(!pSock) continue;
		if(pSock->m_closeDesired) pSock->_doClose();
		pSock->_onProcessActiveQueue(m_pSockServer, nowTicks, msInterval);
	}
}

void KSelectIoThread::ProcessClosedQueue(DWORD nowTicks, int msInterval)
{
	KSelectSocket** ppSock = m_pSockServer->m_ppClosedSock;
	for(int i=0; i<m_sockCount; i++)
	{
		int idx = m_startIndex + i;
		KSelectSocket* pSock = ppSock[idx]; if(!pSock) continue;
		pSock->_onProcessClosedQueue(m_pSockServer, nowTicks, msInterval);
	}
}

/// KSelectSockServer

KSelectSockServer::KSelectSockServer()
{
	m_ioThreadNum = 0;
	m_socketCapacity = 0;
	m_socketCount = 0;
	m_ppIoThread = NULL;
	m_ppActiveSock = NULL;
	m_ppClosedSock = NULL;
}

KSelectSockServer::~KSelectSockServer()
{
	this->Finalize();
}

BOOL KSelectSockServer::Initialize(int socketCapacity, int ioThreadNum)
{
#if defined(WIN32) || defined(WINDOWS)
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
#endif

	m_ioThreadNum = ioThreadNum;
	m_socketCapacity = socketCapacity;

	int startSockIndex = 0;
	int sockCount = (m_socketCapacity+m_ioThreadNum-1) / m_ioThreadNum;

	m_ppActiveSock = new KSelectSocket*[m_socketCapacity]; ASSERT(m_ppActiveSock);
	memset(m_ppActiveSock, 0, sizeof(KSelectSocket*)*m_socketCapacity);

	m_ppClosedSock = new KSelectSocket*[m_socketCapacity]; ASSERT(m_ppClosedSock);
	memset(m_ppClosedSock, 0, sizeof(KSelectSocket*)*m_socketCapacity);

	m_ppIoThread = new KSelectIoThread*[m_ioThreadNum]; ASSERT(m_ppIoThread);
	for(int i=0; i<m_ioThreadNum; i++)
	{
		int sockNum = sockCount;
		if(startSockIndex + sockNum > m_socketCapacity)
		{
			sockNum = m_socketCapacity - startSockIndex;
		}

		KSelectIoThread*& pIoThread = m_ppIoThread[i];
		pIoThread = new KSelectIoThread(); ASSERT(pIoThread);
		pIoThread->Initialize(this, startSockIndex, sockNum);

		startSockIndex += sockCount;
	}

	return TRUE;
}

void KSelectSockServer::Finalize()
{
	if(m_ppIoThread)
	{
		for(int i=0; i<m_ioThreadNum; i++)
		{
			KSelectIoThread* pThread = m_ppIoThread[i];
			pThread->Finalize();
			delete pThread;
		}
		delete [] m_ppIoThread;
		m_ppIoThread = NULL;
	}

	if(m_ppClosedSock)
	{
		for(int i=0; i<m_socketCapacity; i++)
		{
			KSocket* pSock = m_ppClosedSock[i];
			if(!pSock) continue;
			pSock->Close();
			pSock->Destroy();
			m_ppActiveSock[i] = NULL;
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

#if defined(WIN32) || defined(WINDOWS)
	WSACleanup();
#endif
}

BOOL KSelectSockServer::Attach(KSocket* pSock)
{
	BOOL bRet = FALSE;
	{
		switch(pSock->GetSocketType())
		{
		case KSocket::SOCK_TCP_LISTEN:
			bRet = this->AttachTcpListen((KTcpListenSocket*)pSock);
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
			Log(LOG_WARN, "warn: sock server is full, can not accept more socket");
	}

	return bRet;
}

BOOL KSelectSockServer::CanAcceptSocket() const
{
	return m_socketCount < m_socketCapacity;
}

BOOL KSelectSockServer::_Detach(KSocket* pSock)
{
	if(pSock->m_refCount > 0) return FALSE;
	JG_S::KAutoThreadMutex mx(m_mxSockets);
	JG_S::KAutoThreadMutex mx2(pSock->m_mx);
	if(!pSock->_canDestroy()) return FALSE;
	m_ppClosedSock[pSock->m_sockIndex] = NULL;
	m_socketCount -= 1;
	return TRUE;
}

void KSelectSockServer::MoveSocketToActive(int sockIndex)
{
	KSelectSocket** ppActiveSock = m_ppActiveSock;
	KSelectSocket** ppClosedSock = m_ppClosedSock;

	JG_S::KAutoThreadMutex mx(m_mxSockets);

	ASSERT(!ppActiveSock[sockIndex]);
	ASSERT(ppClosedSock[sockIndex]);

	KSelectSocket* pSock = ppClosedSock[sockIndex];
	ASSERT(pSock->GetSocketType() == KSocket::SOCK_CLIENT_TCP);

	ppActiveSock[sockIndex] = ppClosedSock[sockIndex];
	ppClosedSock[sockIndex] = NULL;
}

void KSelectSockServer::MoveSocketToClose(int sockIndex)
{
	KSelectSocket** ppActiveSock = m_ppActiveSock;
	KSelectSocket** ppClosedSock = m_ppClosedSock;

	JG_S::KAutoThreadMutex mx(m_mxSockets);

	ASSERT(ppActiveSock[sockIndex]);
	ASSERT(ppClosedSock[sockIndex] == NULL);

	KSelectSocket* pSock = ppActiveSock[sockIndex];
	ASSERT(pSock->m_sock == INVALID_SOCKET);

	ppActiveSock[sockIndex] = NULL;
	ppClosedSock[sockIndex] = pSock;
}

BOOL KSelectSockServer::AttachTcpListen(KTcpListenSocket* pSock)
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

	return TRUE;
}

BOOL KSelectSockServer::AttachServerTcpSocket(KServerTcpSocket* pSock)
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
	pSock->m_sendIOBuffer.SetBufferPool(m_pieceBufferPool);

	m_ppActiveSock[sockIndex] = pSock;
	m_ppClosedSock[sockIndex] = NULL;
	m_socketCount += 1;

	return TRUE;
}

BOOL KSelectSockServer::AttachClientTcpSocket(KClientTcpSocket* pSock)
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

BOOL KSelectSockServer::AttachUdpSocket(KUdpSocket* pSock)
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

int KSelectSockServer::_GetAvailSockIndex()
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

#endif
