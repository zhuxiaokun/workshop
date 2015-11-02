#ifndef _K_SOCKET_SERVER_EPOLL_H_
#define _K_SOCKET_SERVER_EPOLL_H_

#include "KEpollSocket.h"

#ifndef _USE_SELECT_
#if !defined(WIN32) && !defined(WINDOWS)
#include <System/Random/KRandom.h>
#include <System/Sync/KSync.h>
#include <System/Thread/KThread.h>
#include "KSockServer.h"
#include "KSockets.h"

class KEpollIoThread : public System::Thread::KThread
{
public:
	KEpollIoThread();
	~KEpollIoThread();

public:
	BOOL Initialize(KEpollSockServer* pSockServer, int startSockIdx, int sockCount);
	void Finalize();

public:
	void Execute();
	void Stop();
	void ProcessActiveQueue(DWORD nowTicks, int intervalTicks);
	void ProcessClosedQueue(DWORD nowTicks, int intervalTicks);

public:
	volatile BOOL m_stopDesired;
	KEpollSockServer* m_pSockServer;

	int m_startSockIndex;
	int m_sockCount;
	
	// epoll file descriptor
	int m_epollDesc;
	DWORD m_lastBreatheTick;
};

class KEpollSockServer : public KBaseSockServer
{
public:
	KEpollSockServer();
	virtual ~KEpollSockServer();

public:
	BOOL Initialize(int socketCapacity, int ioThreadNum);
	void Finalize();

	BOOL Attach(KSocket* pSock);
	int GetSocketCapacity() const;
	int GetCurrentSocketCount() const;

public:
	virtual BOOL _Detach(KSocket* pSock);

public:
	// 是否还能接受更多的Socket
	BOOL CanAcceptSocket() const;
	void MoveSocketToActive(int sockIndex);
	void MoveSocketToClose(int sockIndex);

	BOOL AttachTcpListenSocket(KTcpListenSocket* pSock);
	BOOL AttachServerTcpSocket(KServerTcpSocket* pSock);
	BOOL AttachUdpSocket(KUdpSocket* pSock);
	BOOL AttachClientTcpSocket(KClientTcpSocket* pSock);

	BOOL AddToEpoll(KEpollSocket* pSock);
	BOOL DelFromEpoll(KEpollSocket* pSock);

public:
	int _GetAvailSockIndex();
	KEpollIoThread& GetResponsibilityThread(int sockIndex);

public:
	int m_ioThreadNum;
	int m_socketCapacity;
	int m_socketCount;

	System::Random::KRandom m_random;
	KEpollIoThread** m_ppIoThread;

	JG_S::KThreadMutex m_mxSockets;
	KEpollSocket** m_ppActiveSock;
	KEpollSocket** m_ppClosedSock;

	struct epoll_event* m_events;

	int m_tlsSigPipe;
};

#endif
#endif
#endif