#ifndef _K_SOCKET_SERVER_IOCP_H_
#define _K_SOCKET_SERVER_IOCP_H_

#ifndef _USE_SELECT_
#if defined(WIN32) || defined(WINDOWS)

#include "KSockServer.h"
#include "KIocpSocket.h"
#include <System/Sync/KSync.h>
#include <System/Thread/KThread.h>

class KIocpSockServer;

/**
* Socket IO 线程
* 如果是 IOCP, 则执行 GetQueuedCompletionStatus
* 如果是 epoll, 待定
*
*/
class KSocketIOThread : public System::Thread::KThread
{
public:
	enum eGetQueuedCompletionStatusResult
	{
		Success_Result,
		Timeout_Result,
		Fail_IO_Result,
		Socket_Closed_Result,
		Error_Result,
	};

public:
	KSocketIOThread();
	~KSocketIOThread();

public:
	void Execute();
	void Stop();

protected:
	eGetQueuedCompletionStatusResult GetQueuedCompletionStatusResult(BOOL bRet, KSocketOperation* pOperation, DWORD numberOfBytes);

public:
	volatile BOOL m_stopDesired;
	KIocpSockServer* m_pSockServer;
};

/**
* Socket维护线程
*
* ActiveSocket队列
*     所有的Socket在Attach后都是先放到Active队列中，由维护线程
*     根据Socket的当前状态把它移到Close队列中
*     如果是ClientTcpSocket，为连接，转移到CloseSocket队列中
*     不在发送操作中，但是有发送包的，发起一个发送操作
*
*/
class KSocketMaintenanceThread : public System::Thread::KThread
{
public:
	KSocketMaintenanceThread();
	~KSocketMaintenanceThread();

public:
	void Execute();
	void Stop();

private:
	void ProcessActiveQueue(DWORD nowTicks, int intervalTicks);
	void ProcessClosedQueue(DWORD nowTicks, int intervalTicks);

public:
	volatile BOOL m_stopDesired;
	KIocpSockServer* m_pSockServer;
	DWORD m_lastBreatheTick;
};

/**
* 内部保持两个Socket队列
* ActiveSocket队列，所有具备IO能力的Socket
* ClosedSocket队列，所有不具备IO能力的Socket
*/
class KIocpSockServer : public KBaseSockServer
{
public:
	KIocpSockServer();
	virtual ~KIocpSockServer();

public:
	BOOL Initialize(int socketCapacity, int ioThreadNum);
	void Finalize();

	// 外部线程调用，可能是多个线程并发的
	BOOL Attach(KSocket* pSock);
	int GetSocketCapacity() const;
	int GetCurrentSocketCount() const;

public:
	virtual BOOL _Detach(KSocket* pSock);

public:
	// 还可以接受更多的Socket吗 ?
	BOOL CanAcceptSocket() const;

	void MoveSocketToActive(int sockIndex);
	void MoveSocketToClose(int sockIndex);

	BOOL AttachTcpListenSocket(KIocpSocket* pSock);
	BOOL AttachServerTcpSocket(KIocpSocket* pSock);
	BOOL AttachUdpSocket(KIocpSocket* pSock);
	BOOL AttachClientTcpSocket(KIocpSocket* pSock);

#if defined(WIN32) || defined(WINDOWS)
public:
	void checkSockets();
	void damageSocket(KSocket* pSock);
#endif

public:
	int m_ioThreadNum;
	int m_socketCapacity;

	// 当前管理了多少Socket
	int m_socketCount;

	KSocketIOThread** m_ppIoThread;
	KSocketMaintenanceThread m_mthread;

public:
	System::Sync::KThreadMutex m_mxSockets;
	KIocpSocket** m_ppActiveSock;
	KIocpSocket** m_ppClosedSock;

public:
	HANDLE m_hcp;
};

#endif
#endif
#endif