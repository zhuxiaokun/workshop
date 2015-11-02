#pragma once
#include <SockServer/JgPacketSpecific.h>
#include <KPacketGateTmpl.h>
#include <PacketGate/KPacket_extractor_dispatcher.h>

enum
{
	Socket_None,
	Socket_LogDispatcher_Server,
};

class KSocket_LogDispatcher : public JgClientTcpSocket
{
public:
	bool m_remoteBusy;
	volatile BOOL m_cnnReady;

public:
	KSocket_LogDispatcher();
	~KSocket_LogDispatcher();

public:
	BOOL isReady() const;
	BOOL OnConnected();
	BOOL OnClosed();
	BOOL OnConnectError(DWORD errCode);
	BOOL OnAppPacket(const void* pPacket, int len, const KSocketAddress& fromAddr);
};

class KPacketGate_DE
	: public KPacketGateTmpl<KPacketGate_DE,KSocket_LogDispatcher,s_nDE_PacketStart,s_nDE_PacketEnd>
{
public:
	KPacketGate_DE();
	~KPacketGate_DE();

public:
	void process_DE_Connected(KSocket_LogDispatcher* pSock, const void* pData, int len);
	void process_DE_ConnClosed(KSocket_LogDispatcher* pSock, const void* pData, int len);
	void process_DE_ConnError(KSocket_LogDispatcher* pSock, const void* pData, int len);
	void process_DE_Verb_Ack(KSocket_LogDispatcher* pSock, const void* pData, int len);
	void process_DE_Busy(KSocket_LogDispatcher* pSock, const void* pData, int len);
};
