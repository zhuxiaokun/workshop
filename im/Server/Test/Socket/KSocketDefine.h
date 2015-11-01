#pragma once
#include <SockServer/JgPacketSpecific.h>
#include <PacketGate/chat_client_packet.h>
#include "JgClientTcpSocket.h"

class KTestClient;

class KSocket_Chat : public jg::net::JgClientTcpSocket
{
public:
	volatile BOOL m_cnnReady;

public:
	KSocket_Chat();
	~KSocket_Chat();

public:
	void Reset();
	BOOL OnConnected();
	BOOL OnConnectError(DWORD errCode);
	BOOL OnClosed();
	//string_256 ToString() const;
};
