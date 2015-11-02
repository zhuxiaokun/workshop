#include "KSocketDefine.h"
#include <System/Misc/StrUtil.h>
#include "Application.h"
#include <System/Misc/KStream.h>
#include <CommSession/KPropertyMap.h>
#include "verb_process.h"
//#include <redis/hiredis.h>
#include "app_event.h"

//volatile BOOL m_cnnReady

KSocket_LogExtractor::KSocket_LogExtractor():m_cnnReady(FALSE),m_lastBusy(false)
{
	this->SetUserData(Socket_LogExtractor);
	this->SetSendBufferSize(1024*20);
	this->SetRecvBuffSize(1024*20);
}

KSocket_LogExtractor::~KSocket_LogExtractor()
{

}

BOOL KSocket_LogExtractor::OnConnected()
{
	m_cnnReady = TRUE;
	this->PostPacket(s_nED_Connected, NULL, 0);
	return TRUE;
}

BOOL KSocket_LogExtractor::OnClosed()
{
	m_cnnReady = FALSE;
	this->PostPacket(s_nED_ConnClosed, NULL, 0);
	return TRUE;
}

BOOL KSocket_LogExtractor::OnAppPacket(const void* pPacket, int len, const KSocketAddress& fromAddr)
{
	return JgServerTcpSocket::OnAppPacket(pPacket, len, fromAddr);
}

void KSocket_LogExtractor::OnBreathe(int intervalTcks)
{
	if(m_recvIoBuff.Size() > 1024*1024)
	{
		if(!m_lastBusy)
		{
			JG_S::KAutoThreadMutex mx(m_mxBusy);
			if(!m_lastBusy)
			{
				m_lastBusy = true;
				DE_Busy packet = {1};
				this->Send(s_nDE_Busy, &packet, sizeof(packet));
			}
		}
		return;
	}
	if(m_recvIoBuff.Size() < 1024*10)
	{
		if(m_lastBusy)
		{
			JG_S::KAutoThreadMutex mx(m_mxBusy);
			if(m_lastBusy)
			{
				m_lastBusy = false;
				DE_Busy packet = {0};
				this->Send(s_nDE_Busy, &packet, sizeof(packet));
			}
		}
		return;
	}
}

void KSocket_LogExtractor::Reset()
{
	m_cnnReady = FALSE;
	{
		JG_S::KAutoThreadMutex mxBusy(m_mxBusy);
		m_lastBusy = false;
	}
	JgServerTcpSocket::Reset();
}

void KSocket_LogExtractor::Destroy()
{
	this->Reset();
	KSocket_LogExtractor::Free(this);
}

//// KPacketGate_DE::

KPacketGate_ED::KPacketGate_ED()
{
	this->Register(s_nED_Connected, &KPacketGate_ED::process_ED_Connected);
	this->Register(s_nED_ConnClosed, &KPacketGate_ED::process_ED_ConnClosed);
	this->Register(s_nED_Verb, &KPacketGate_ED::process_ED_Verb);
}

KPacketGate_ED::~KPacketGate_ED()
{

}

void KPacketGate_ED::process_ED_Connected(KSocket_LogExtractor* pSock, const void* pData, int len)
{
	Log(LOG_WARN, "log extractor connected from %s", pSock->GetRemoteAddress().toString().c_str());
	pSock->AddRef();
	//g_pApp->startTimer(AppTimer_Check_Socket_RecvBuffer, 0, 1.0f, (void*)pSock);
}

void KPacketGate_ED::process_ED_ConnClosed(KSocket_LogExtractor* pSock, const void* pData, int len)
{
	Log(LOG_WARN, "log extractor closed from %s", pSock->GetRemoteAddress().toString().c_str());
}

void KPacketGate_ED::process_ED_Verb(KSocket_LogExtractor* pSock, const void* pData, int len)
{
	g_pApp->m_frameLineCount++;
	const char* verbString = (const char*)pData;
	KPropertyMap m;
	m.parseLine((char*)verbString);
	if(m.empty()) return;
	process_verb(m, true);
}

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
	//if(!g_pApp->m_redisReady)
	//	return NULL;
	KSocket_LogExtractor* p =  KSocket_LogExtractor::Alloc();
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
