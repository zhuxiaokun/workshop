#include "KSocketDefine.h"
#include <System/Misc/StrUtil.h>

//volatile BOOL m_cnnReady

KSocket_LogDispatcher::KSocket_LogDispatcher():m_cnnReady(FALSE),m_remoteBusy(false)
{
	this->SetUserData(Socket_LogDispatcher_Server);
	this->SetSendBufferSize(1024*20);
	this->SetRecvBuffSize(1024*10);
	this->SetReconnectInterval(10);
}

KSocket_LogDispatcher::~KSocket_LogDispatcher()
{

}

BOOL KSocket_LogDispatcher::isReady() const
{
	return m_cnnReady && !m_remoteBusy && m_sendIOBuffer.Size() < 1024*1024;
}
BOOL KSocket_LogDispatcher::OnConnected()
{
	m_cnnReady = TRUE;
	this->PostPacket(s_nED_Connected, NULL, 0);
	return TRUE;
}

BOOL KSocket_LogDispatcher::OnClosed()
{
	m_cnnReady = FALSE;
	this->PostPacket(s_nED_ConnClosed, NULL, 0);
	return TRUE;
}

BOOL KSocket_LogDispatcher::OnConnectError(DWORD errCode)
{
	this->PostPacket(s_nED_ConnError, &errCode, sizeof(errCode));
	return TRUE;
}

BOOL KSocket_LogDispatcher::OnAppPacket(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	JgPacketHead* head = (JgPacketHead*)pPacket;
	if(head->command == s_nDE_Busy)
	{
		bool lastBusy = m_remoteBusy;
		DE_Busy* packet = (DE_Busy*)(head + 1);
		m_remoteBusy = packet->busy ? true : false;
		if(lastBusy != m_remoteBusy)
			Log(LOG_CONSOLE|LOG_WARN, "warn: remote busy changed from '%s' to '%s'",
				lastBusy ? "busy" : "idle",
				m_remoteBusy ? "busy" : "idle");
	}
	return JgClientTcpSocket::OnAppPacket(pPacket, len, fromAddr);
}

//// KPacketGate_DE::

KPacketGate_DE::KPacketGate_DE()
{
	this->Register(s_nDE_Connected, &KPacketGate_DE::process_DE_Connected);
	this->Register(s_nDE_ConnClosed, &KPacketGate_DE::process_DE_ConnClosed);
	this->Register(s_nDE_ConnError, &KPacketGate_DE::process_DE_ConnError);
	this->Register(s_nDE_Verb_Ack, &KPacketGate_DE::process_DE_Verb_Ack);
	this->Register(s_nDE_Busy, &KPacketGate_DE::process_DE_Busy);
}

KPacketGate_DE::~KPacketGate_DE()
{

}

void KPacketGate_DE::process_DE_Connected(KSocket_LogDispatcher* pSock, const void* pData, int len)
{
	Log(LOG_WARN, "connected to log dispatcher server %s", pSock->GetRemoteAddress().toString().c_str());
	pSock->m_remoteBusy = false;
}

void KPacketGate_DE::process_DE_ConnClosed(KSocket_LogDispatcher* pSock, const void* pData, int len)
{
	Log(LOG_WARN, "closed from log dispatcher server %s", pSock->GetRemoteAddress().toString().c_str());
	pSock->m_remoteBusy = false;
}

void KPacketGate_DE::process_DE_ConnError(KSocket_LogDispatcher* pSock, const void* pData, int len)
{
	DWORD errCode = *(DWORD*)pData;
	Log(LOG_DEBUG, "error, connect to log dispatcher server %s, %s",
		pSock->GetRemoteAddress().toString().c_str(),
		strerror_r2(errCode).c_str());
}

void KPacketGate_DE::process_DE_Verb_Ack(KSocket_LogDispatcher* pSock, const void* pData, int len)
{
	// not used.
}

void KPacketGate_DE::process_DE_Busy(KSocket_LogDispatcher* pSock, const void* pData, int len)
{
	//DE_Busy* packet = (DE_Busy*)pData;
	//bool lastBusy = pSock->m_remoteBusy;
	//pSock->m_remoteBusy = packet->busy != 0;
	//if(lastBusy != pSock->m_remoteBusy)
	//	Log(LOG_CONSOLE|LOG_WARN, "warn: remote busy changed from '%s' to '%s'",
	//		lastBusy ? "busy" : "idle",
	//		pSock->m_remoteBusy ? "busy" : "idle");
}
