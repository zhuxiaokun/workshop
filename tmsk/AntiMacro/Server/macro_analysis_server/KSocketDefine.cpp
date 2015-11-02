#include "KSocketDefine.h"
#include <System/Misc/StrUtil.h>
#include "Application.h"
#include <System/Misc/KStream.h>
#include <CommSession/KPropertyMap.h>
#include <redis/hiredis.h>

//volatile BOOL m_cnnReady

KSocket_Client::KSocket_Client():m_cnnReady(FALSE)
{
	this->SetUserData(Socket_Client);
	this->SetSendBufferSize(1024*20);
	this->SetRecvBuffSize(1024*10);
}

KSocket_Client::~KSocket_Client()
{

}

BOOL KSocket_Client::OnConnected()
{
	m_cnnReady = TRUE;
	this->PostPacket(s_nED_Connected, NULL, 0);
	return TRUE;
}

BOOL KSocket_Client::OnClosed()
{
	m_cnnReady = FALSE;
	this->PostPacket(s_nED_ConnClosed, NULL, 0);
	return TRUE;
}

BOOL KSocket_Client::OnAppPacket(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	if(JgServerTcpSocket::OnAppPacket(pPacket, len, fromAddr))
	{
		g_pApp->m_inputEvent.Set();
		return TRUE;
	}
	return FALSE;
}

void KSocket_Client::Reset()
{
	m_cnnReady = FALSE;
	JgServerTcpSocket::Reset();
}

void KSocket_Client::Destroy()
{
	this->Reset();
	KSocket_Client::Free(this);
}

//// KPacketGate_DE::

KPacketGate_Client::KPacketGate_Client()
{
	this->Register(s_nED_Connected, &KPacketGate_Client::process_ED_Connected);
	this->Register(s_nED_ConnClosed, &KPacketGate_Client::process_ED_ConnClosed);
	//this->Register(s_nED_Verb, &KPacketGate_ED::process_ED_Verb);
}

KPacketGate_Client::~KPacketGate_Client()
{

}

void KPacketGate_Client::process_ED_Connected(KSocket_Client* pSock, const void* pData, int len)
{
	Log(LOG_WARN, "client connected from %s", pSock->GetRemoteAddress().toString().c_str());
}

void KPacketGate_Client::process_ED_ConnClosed(KSocket_Client* pSock, const void* pData, int len)
{
	Log(LOG_WARN, "client closed from %s", pSock->GetRemoteAddress().toString().c_str());
}

//void KPacketGate_ED::process_ED_Verb(KSocket_Client* pSock, const void* pData, int len)
//{
//	const char* verbString = (const char*)pData;
//	KPropertyMap m;
//	m.parseLine((char*)verbString);
//	if(m.empty()) return;
//	process_verb(m, true);
//}

//// KSocket_Listener

KSocket_Listener::KSocket_Listener()
{
	this->SetUserData(Socket_Listener);
}

KSocket_Listener::~KSocket_Listener()
{

}

KServerTcpSocket* KSocket_Listener::CreateSocketObject()
{
	KSocket_Client* p =  KSocket_Client::Alloc();
	p->Reset();
	return p;
}

BOOL KSocket_Listener::OnClosed()
{
	Log(LOG_ERROR, "error: listen socket closed, create a new one");

	const char* listenIp = g_pApp->m_configure["listen"]["ip"];
	int port = g_pApp->m_configure["listen"]["port"];

	KSocket_Listener* pSock = new KSocket_Listener();
	if(!pSock->Listen(listenIp, port))
	{
		Log(LOG_ERROR, "error: fail to listen %s:%d", listenIp, port);
		return FALSE;
	}

	g_pApp->m_sockServer.Attach(pSock);
	return TRUE;
}
