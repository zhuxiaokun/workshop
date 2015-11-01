#include "KSocketDefine.h"
#include <PacketGate/chat_client_packet.h>
#include "../KDemoChatServerApp.h"

/// KSocket_Client

KSocket_Client::KSocket_Client():m_cnnReady(FALSE),m_chatAcct(NULL)
{
	this->SetUserData(enum_socket_client);
	this->SetSendBufferSize(1024*10);
	this->SetRecvBuffSize(1024*10);
}

KSocket_Client::~KSocket_Client()
{

}

BOOL KSocket_Client::IsReady() const
{
	return m_cnnReady;
}

void KSocket_Client::Reset()
{
	m_cnnReady = FALSE;
	m_chatAcct = NULL;
	JgServerTcpSocket::Reset();
}

void KSocket_Client::Destroy()
{
	this->Reset();
	KSocket_Client::Free(this);
}

BOOL KSocket_Client::OnConnected()
{
	m_cnnReady = TRUE;
	this->PostPacket(s_nCCht_Connected, NULL, 0);
	return TRUE;
}

BOOL KSocket_Client::OnClosed()
{
	m_cnnReady = FALSE;
	this->PostFinalPacket(s_nCCht_ConnClosed, NULL, 0);
	return TRUE;
}

/// KSocket_TcpListen

KSocket_TcpListen::KSocket_TcpListen()
{
	this->SetUserData(enum_socket_listen);
}

KSocket_TcpListen::~KSocket_TcpListen()
{

}

KServerTcpSocket* KSocket_TcpListen::CreateSocketObject()
{
	KSocket_Client* pSock = KSocket_Client::Alloc();
	pSock->Reset();
	return pSock;
}

BOOL KSocket_TcpListen::OnClosed()
{
	Log(LOG_ERROR, "error: %s closed", this->ToString().c_str());
	return TRUE;
}
