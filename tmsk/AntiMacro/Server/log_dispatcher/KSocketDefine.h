#pragma once
#include <SockServer/JgPacketSpecific.h>
#include <KPacketGateTmpl.h>
#include <PacketGate/KPacket_extractor_dispatcher.h>
#include <System/Memory/KStepObjectPool.h>

enum
{
	Socket_None,
	Socket_LogExtractor,
	Socket_Listener,
};

class KSocket_LogExtractor
	: public JgServerTcpSocket
	, public JG_M::KPortableStepPool<KSocket_LogExtractor,256,JG_S::KMTLock>
{
public:
	bool m_lastBusy;
	JG_S::KThreadMutex m_mxBusy;
	volatile BOOL m_cnnReady;

public:
	KSocket_LogExtractor();
	~KSocket_LogExtractor();

public:
	BOOL isReady() const { return m_cnnReady; }
	BOOL OnConnected();
	BOOL OnClosed();
	BOOL OnAppPacket(const void* pPacket, int len, const KSocketAddress& fromAddr);
	void OnBreathe(int intervalTcks);
	
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

class KPacketGate_ED
	: public KPacketGateTmpl<KPacketGate_ED,KSocket_LogExtractor,s_nDE_PacketStart,s_nDE_PacketEnd>
{
public:
	KPacketGate_ED();
	~KPacketGate_ED();

public:
	void process_ED_Connected(KSocket_LogExtractor* pSock, const void* pData, int len);
	void process_ED_ConnClosed(KSocket_LogExtractor* pSock, const void* pData, int len);
	void process_ED_Verb(KSocket_LogExtractor* pSock, const void* pData, int len);
};
