#ifndef _K_SOCKET_SERVER_H_
#define _K_SOCKET_SERVER_H_

#include "KSocketBase.h"

class KBaseSockServer
{
public:
	KBaseSockServer();
	virtual ~KBaseSockServer();

public:
	virtual BOOL Initialize(int socketCapacity, int ioThreadNum) = 0;
	virtual void Finalize() = 0;

	// 外部线程调用，可能是多个线程并发的
	virtual BOOL Attach(KSocket* pSock) = 0;
	virtual BOOL _Detach(KSocket* pSock) = 0;

	virtual void MoveSocketToActive(int sockIndex) = 0;
	virtual void MoveSocketToClose(int sockIndex) = 0;

public:
	KIOBuffPool_2048 m_pieceBufferPool;
};

#endif