#ifndef _K_SELECT_SOCKSERVER_H_
#define _K_SELECT_SOCKSERVER_H_

#include "KSelectSocket.h"
#if defined(_USE_SELECT_)
#include <System/SockHeader.h>
#include "KSockServer.h"
#include <System/Thread/KThread.h>
#include "KSockets.h"
#include <System/Random/KRandom.h>

class KSelectIoThread : public System::Thread::KThread
{
public:
	KSelectIoThread();
	~KSelectIoThread();

public:
	BOOL Initialize(KSelectSockServer* pSockServer, int startSockIdx, int sockCount);
	void Finalize();

public:
	void Execute();
	void Stop();

public:
	int Build_fdset();
	void ProcessActiveQueue(DWORD nowTicks, int msInterval);
	void ProcessClosedQueue(DWORD nowTicks, int msInterval);

public:
	fd_set m_rfds;
	fd_set m_efds;
	int m_startIndex;
	int m_sockCount;
	KSelectSockServer* m_pSockServer;
	volatile BOOL m_stopDesired;
};

class KSelectSockServer : public KBaseSockServer
{
public:
	KSelectSockServer();
	~KSelectSockServer();

public:
	BOOL Initialize(int socketCapacity, int ioThreadNum);
	void Finalize();

public:
	BOOL Attach(KSocket* pSock);
	BOOL CanAcceptSocket() const;
	
public:
	virtual BOOL _Detach(KSocket* pSock);
	virtual void MoveSocketToActive(int sockIndex);
	virtual void MoveSocketToClose(int sockIndex);

public:
	BOOL AttachTcpListen(KTcpListenSocket* pSock);
	BOOL AttachServerTcpSocket(KServerTcpSocket* pSock);
	BOOL AttachClientTcpSocket(KClientTcpSocket* pSock);
	BOOL AttachUdpSocket(KUdpSocket* pSock);
	int _GetAvailSockIndex();

public:
	int m_ioThreadNum;
	int m_socketCapacity;
	int m_socketCount;
	KSelectIoThread** m_ppIoThread;

	JG_S::KThreadMutex m_mxSockets;
	KSelectSocket** m_ppActiveSock;
	KSelectSocket** m_ppClosedSock;
	System::Random::KRandom m_random;
};

#endif
#endif