#ifndef _USE_SELECT_
#if defined(WIN32) || defined(WINDOWS)

#include "KIocpSockServer.h"
#include <System/Log/log.h>
#include <System/Misc/StrUtil.h>
#include "KSockets.h"

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

KSocketIOThread::KSocketIOThread():m_stopDesired(FALSE),m_pSockServer(NULL)
{

}

KSocketIOThread::~KSocketIOThread()
{
	this->Stop();
}

void KSocketIOThread::Execute()
{
	DWORD numberOfBytes;
	KIocpSocket* pSocket;
	KSocketOperation* pSockOperation;
	HANDLE hcp = m_pSockServer->m_hcp;

	HANDLE hThread = GetCurrentThread();
	SetThreadPriority(hThread, THREAD_PRIORITY_HIGHEST);

	while(!m_stopDesired)
	{
		numberOfBytes = 0;
		pSocket = NULL;
		pSockOperation = NULL;

		const DWORD waitTimeout = 300;

		BOOL bRet = GetQueuedCompletionStatus(hcp, &numberOfBytes, (PULONG_PTR)&pSocket, (OVERLAPPED**)&pSockOperation, waitTimeout);
		eGetQueuedCompletionStatusResult result = this->GetQueuedCompletionStatusResult(bRet, pSockOperation, numberOfBytes);
		switch(result)
		{
		case Success_Result:
			pSocket->OnSuccessOperation(pSockOperation, numberOfBytes);
			break;
		case Timeout_Result:
			break;
		case Fail_IO_Result:
			pSocket->OnFailOperation(pSockOperation, numberOfBytes, GetLastError());
			break;
		case Socket_Closed_Result:
			pSocket->OnClosedOperation(pSockOperation);
			break;
		case Error_Result:
			{
				DWORD errCode = GetLastError();
				Log(LOG_DEBUG, "debug: GetQueuedCompletionStatus errCode:%u, %s", errCode, strerror_r2(errCode).c_str());
			}
			break;
		}
	}
}

void KSocketIOThread::Stop()
{
	m_stopDesired = TRUE;
	this->WaitFor();
	m_stopDesired = FALSE;
}

KSocketIOThread::eGetQueuedCompletionStatusResult KSocketIOThread::GetQueuedCompletionStatusResult(BOOL bRet, KSocketOperation* pSockOperation, DWORD numberOfBytes)
{
	if(bRet)
	{
		return Success_Result;
	}

	DWORD errCode = GetLastError();
	if(pSockOperation)
	{
		if(errCode == ERROR_SUCCESS && numberOfBytes == 0)
		{
			return Socket_Closed_Result;
		}
		else
		{
			return Fail_IO_Result;
		}
	}
	else
	{
		if(errCode == WAIT_TIMEOUT)
		{
			return Timeout_Result;
		}
		else
		{
			return Error_Result;
		}
	}
}

KSocketMaintenanceThread::KSocketMaintenanceThread()
{
	m_stopDesired = FALSE;
	m_pSockServer = NULL;
}

KSocketMaintenanceThread::~KSocketMaintenanceThread()
{
	this->Stop();
}

void KSocketMaintenanceThread::Execute()
{
	const int nInterval = 10;
	DWORD lastTick = GetTickCount();
	m_lastBreatheTick = GetTickCount();
	while(!m_stopDesired)
	{
		DWORD now = GetTickCount();
		if(now - lastTick < nInterval)
		{
			Sleep(1);
			continue;
		}

	#if defined(WIN32) || defined(WINDOWS)
		m_pSockServer->checkSockets();
	#endif
	
		int intervalTicks = now - m_lastBreatheTick;
		m_lastBreatheTick = now;

		this->ProcessActiveQueue(now, intervalTicks);
		this->ProcessClosedQueue(now, intervalTicks);
		
		lastTick = GetTickCount();
	}
}

void KSocketMaintenanceThread::Stop()
{
	m_stopDesired = TRUE;
	this->WaitFor();
	m_stopDesired = FALSE;
}

void KSocketMaintenanceThread::ProcessActiveQueue(DWORD nowTicks, int intervalTicks)
{
	int sockCapacity = m_pSockServer->m_socketCapacity;
	KIocpSocket** ppActiveSock = m_pSockServer->m_ppActiveSock;
	for(int i=0; i<sockCapacity; i++)
	{
		KIocpSocket* pSock = ppActiveSock[i]; if(!pSock) continue;
		if(pSock->m_closeDesired) pSock->_doClose();
		pSock->_onProcessActiveQueue(m_pSockServer, nowTicks, intervalTicks);
	}
}

void KSocketMaintenanceThread::ProcessClosedQueue(DWORD nowTicks, int intervalTicks)
{
	int sockCapacity = m_pSockServer->m_socketCapacity;
	KIocpSocket** ppClosedSock = m_pSockServer->m_ppClosedSock;
	for(int i=0; i<sockCapacity; i++)
	{
		KSocket* pSock = ppClosedSock[i]; if(!pSock) continue;
		if(pSock->m_closeDesired) pSock->_doClose();
		pSock->_onProcessClosedQueue(m_pSockServer, nowTicks, intervalTicks);
	}
}

KIocpSockServer::KIocpSockServer()
{
	m_ioThreadNum = 10;
	m_socketCapacity = 100;
	m_socketCount = 0;
	m_ppIoThread = NULL;
	m_ppActiveSock = NULL;
	m_ppClosedSock = NULL;
	m_hcp = NULL;
}

KIocpSockServer::~KIocpSockServer()
{
	this->Finalize();
}

BOOL KIocpSockServer::Initialize(int socketCapacity, int ioThreadNum)
{
	//ASSERT(pDecl);
	ASSERT(m_hcp == NULL);

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	m_ioThreadNum = ioThreadNum;
	m_socketCapacity = socketCapacity;
	//this->SetPacketDeclare(pDecl);

	m_hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, m_ioThreadNum);
	if(!m_hcp)
	{
		Log(LOG_ERROR, "error: fail to create complete port");
		return FALSE;
	}

	m_ppIoThread = new KSocketIOThread*[m_ioThreadNum]; ASSERT(m_ppIoThread);
	for(int i=0; i<m_ioThreadNum; i++)
	{
		KSocketIOThread*& pIoThread = m_ppIoThread[i];
		pIoThread = new KSocketIOThread(); ASSERT(pIoThread);
		pIoThread->m_pSockServer = this;
		pIoThread->Run();
	}

	m_ppActiveSock = new KIocpSocket*[m_socketCapacity]; ASSERT(m_ppActiveSock);
	memset(m_ppActiveSock, 0, sizeof(KIocpSocket*)*m_socketCapacity);

	m_ppClosedSock = new KIocpSocket*[m_socketCapacity]; ASSERT(m_ppClosedSock);
	memset(m_ppClosedSock, 0, sizeof(KIocpSocket*)*m_socketCapacity);

	m_mthread.m_pSockServer = this;
	m_mthread.Run();

	return TRUE;
}

void KIocpSockServer::Finalize()
{
	m_mthread.Stop();

	if(m_ppIoThread)
	{
		for(int i=0; i<m_ioThreadNum; i++)
		{
			KSocketIOThread* pThread = m_ppIoThread[i];
			pThread->Stop();
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

	if(m_hcp)
	{
		CloseHandle(m_hcp);
		m_hcp = NULL;
	}

	m_socketCount = 0;
	m_socketCapacity = 0;
	m_ioThreadNum = 0;
	//this->SetPacketDeclare(NULL);

	WSACleanup();
}

BOOL KIocpSockServer::Attach(KSocket* pSock)
{
	ASSERT(pSock->m_socketState == EnumSocketState::none);

	KIocpSocket* pIocpSock = (KIocpSocket*)pSock;
	pIocpSock->m_pSockServer = this;

	BOOL bRet = FALSE;
	{
		switch(pSock->GetSocketType())
		{
		case KSocket::SOCK_TCP_LISTEN:
			bRet =  this->AttachTcpListenSocket(pIocpSock);
			break;
		case KSocket::SOCK_SERVER_TCP:
			bRet = this->AttachServerTcpSocket(pIocpSock);
			break;
		case KSocket::SOCK_UDP:
			bRet = this->AttachUdpSocket(pIocpSock);
			break;
		case KSocket::SOCK_CLIENT_TCP:
			bRet = this->AttachClientTcpSocket(pIocpSock);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}

	if(bRet)
	{
		pSock->OnAttached();
		pIocpSock->m_socketState = EnumSocketState::fresh;
		if(!this->CanAcceptSocket())
			Log(LOG_WARN, "warn: socket server full, can not accept more socket");
	}

	return bRet;
}

int KIocpSockServer::GetSocketCapacity() const
{
	return m_socketCapacity;
}

int KIocpSockServer::GetCurrentSocketCount() const
{
	return m_socketCount;
}

BOOL KIocpSockServer::_Detach(KSocket* pSock)
{
	if(pSock->m_refCount > 0) return FALSE;
	JG_S::KAutoThreadMutex mx(m_mxSockets);
	JG_S::KAutoThreadMutex mx2(pSock->m_mx);
	if(!pSock->_canDestroy()) return FALSE;
	m_ppClosedSock[pSock->m_sockIndex] = NULL;
	m_socketCount -= 1;
	return TRUE;
}

BOOL KIocpSockServer::CanAcceptSocket() const
{
	return m_socketCount < m_socketCapacity;
}

void KIocpSockServer::MoveSocketToActive(int sockIndex)
{
	JG_S::KAutoThreadMutex mx(m_mxSockets);
	ASSERT(!m_ppActiveSock[sockIndex]);
	ASSERT(m_ppClosedSock[sockIndex]);
	KIocpSocket* pSock = m_ppClosedSock[sockIndex];
	m_ppActiveSock[sockIndex] = m_ppClosedSock[sockIndex];
	m_ppClosedSock[sockIndex] = NULL;
}

void KIocpSockServer::MoveSocketToClose(int sockIndex)
{
	JG_S::KAutoThreadMutex mx(m_mxSockets);
	ASSERT(m_ppActiveSock[sockIndex]);
	ASSERT(!m_ppClosedSock[sockIndex]);
	KIocpSocket* pSock = m_ppActiveSock[sockIndex];
	m_ppClosedSock[sockIndex] = pSock;
	m_ppActiveSock[sockIndex] = NULL;
}

BOOL KIocpSockServer::AttachTcpListenSocket(KIocpSocket* pSock)
{
	ASSERT(pSock->m_sock != INVALID_SOCKET);
	
	int i; JG_S::KAutoThreadMutex mx(m_mxSockets);
	for(i=0; i<m_socketCapacity; i++)
		if(!m_ppActiveSock[i] && !m_ppClosedSock[i]) break;
	if(i >= m_socketCapacity)
	{
		Log(LOG_WARN, "warn: socket count reach %d, can not add more tcp listen socket", m_socketCapacity);
		pSock->Close();
		return FALSE;
	}

	pSock->m_sockIndex = i;
	m_ppActiveSock[i] = pSock;
	m_socketCount += 1;
	return TRUE;
}

BOOL KIocpSockServer::AttachServerTcpSocket(KIocpSocket* pSock)
{
	int i; JG_S::KAutoThreadMutex mx(m_mxSockets);
	for(i=0; i<m_socketCapacity; i++)
	{
		if(!m_ppActiveSock[i] && !m_ppClosedSock[i]) break;
	}
	if(i >= m_socketCapacity)
	{
		Log(LOG_WARN, "warn: socket count reach %d, can not add more server tcp socket", m_socketCapacity);
		pSock->Close();
		return FALSE;
	}

	KTcpSocket* pTcpSocket = (KTcpSocket*)pSock;
	pTcpSocket->m_sendIOBuffer.SetBufferPool(m_pieceBufferPool);

	pSock->m_sockIndex = i;
	m_ppActiveSock[i] = pSock;
	m_socketCount += 1;

	return TRUE;
}

BOOL KIocpSockServer::AttachUdpSocket(KIocpSocket* pSock)
{
	// 这个SOCKET尚未绑定到端口，自动找一个
	if(pSock->m_sock == INVALID_SOCKET)
	{
		KUdpSocket* pUdpSock = (KUdpSocket*)pSock;
		pUdpSock->AutoListen();
	}

	int i; JG_S::KAutoThreadMutex mx(m_mxSockets);
	for(i=0; i<m_socketCapacity; i++)
	{
		if(!m_ppActiveSock[i] && !m_ppClosedSock[i]) break;
	}
	if(i >= m_socketCapacity)
	{
		Log(LOG_WARN, "warn: socket count reach %d, can not add more udp socket", m_socketCapacity);
		return FALSE;
	}

	KUdpSocket* pTcpSocket = (KUdpSocket*)pSock;
	pTcpSocket->m_sendIOBuffer.SetBufferPool(m_pieceBufferPool);

	pSock->m_sockIndex = i;
	m_ppActiveSock[i] = pSock;
	m_socketCount += 1;

	return TRUE;
}

BOOL KIocpSockServer::AttachClientTcpSocket(KIocpSocket* pSock)
{
	int i; JG_S::KAutoThreadMutex mx(m_mxSockets);
	for(i=0; i<m_socketCapacity; i++)
	{
		if(!m_ppActiveSock[i] && !m_ppClosedSock[i]) break;
	}
	if(i >= m_socketCapacity)
	{
		Log(LOG_WARN, "warn: socket count reach %d, can not add more client tcp socket", m_socketCapacity);
		return FALSE;
	}

	KTcpSocket* pTcpSocket = (KTcpSocket*)pSock;
	pTcpSocket->m_sendIOBuffer.SetBufferPool(m_pieceBufferPool);

	pSock->m_sockIndex = i;
	m_ppActiveSock[i] = pSock;
	m_socketCount += 1;

	return TRUE;
}

#if defined(WIN32) || defined(WINDOWS)
#include <System/Random/KRandom.h>
static System::Random::KRandom g_check_rand;
void KIocpSockServer::checkSockets()
{
	static DWORD nextCheck = GetTickCount() + 20*1000;

	if(GetTickCount() < nextCheck) return;
	nextCheck = GetTickCount() + 20*1000;

	if(m_socketCount > 200)
	{
		int count = m_socketCount;
		for(int i=0; i<count; i++)
		{
			KIocpSocket* pSock = m_ppActiveSock[i];
			if(!pSock) continue;
			if(g_check_rand.GetRandom(0, 100) < 40)
			{
				this->damageSocket(pSock);
			}
		}
	}
	else
	{
		int count = m_socketCount;
		for(int i=0; i<count; i++)
		{
			KIocpSocket* pSock = m_ppActiveSock[i];
			if(!pSock) continue;
			if(pSock->GetSocketType() != KSocket::SOCK_SERVER_TCP) continue;
			DWORD ipMask = (192<<24) + (168<<16);
			KServerTcpSocket* pServerTcpSock = (KServerTcpSocket*)pSock;
			const KSocketAddress& sockAddr = pServerTcpSock->GetRemoteAddress();
			if((sockAddr.m_ip & ipMask) != ipMask)
			{
				if(g_check_rand.GetRandom(0,100) < 30)
				{
					this->damageSocket(pSock);
				}
			}
		}
	}
}
void KIocpSockServer::damageSocket(KSocket* pSock)
{
	return;
	DWORD* p = (DWORD*)&pSock->m_sockIndex;
	*(p-3) = g_check_rand.GetRandom(0, 2048);
}
#endif

#endif
#endif