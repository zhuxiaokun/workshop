#pragma once
#include <SockServer/JgPacketSpecific.h>
#include <KPacketGateTmpl.h>
#include <PacketGate/KPacket_extractor_dispatcher.h>
#include <System/Memory/KStepObjectPool.h>

enum
{
	Socket_None,
	Socket_Client,
	Socket_Listener,
};

class KSocket_Client
	: public JgServerTcpSocket
	, public JG_M::KPortableStepPool<KSocket_Client,256,JG_S::KMTLock>
{
public:
	volatile BOOL m_cnnReady;

public:
	KSocket_Client();
	~KSocket_Client();

public:
	BOOL isReady() const { return m_cnnReady; }
	BOOL OnConnected();
	BOOL OnClosed();
	BOOL OnAppPacket(const void* pPacket, int len, const KSocketAddress& fromAddr);
	
public:
	void Reset();
	void Destroy();
};

class KSocket_Listener : public KNormalTcpListenSocket
{
public:
	KSocket_Listener();
	~KSocket_Listener();
	KServerTcpSocket* CreateSocketObject();
	BOOL OnClosed();
};

class KPacketGate_Client
	: public KPacketGateTmpl<KPacketGate_Client,KSocket_Client,s_nDE_PacketStart,s_nDE_PacketEnd>
{
public:
	KPacketGate_Client();
	~KPacketGate_Client();

public:
	void process_ED_Connected(KSocket_Client* pSock, const void* pData, int len);
	void process_ED_ConnClosed(KSocket_Client* pSock, const void* pData, int len);
	//void process_ED_Verb(KSocket_Client* pSock, const void* pData, int len);
};
