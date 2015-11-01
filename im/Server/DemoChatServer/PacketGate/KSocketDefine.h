#pragma once
#include <SockServer/JgPacketSpecific.h>

enum EnumSocketUserData
{
	enum_socket_client,
	enum_socket_listen,
};

class KChatAccount;
class KSocket_Client
	: public JgServerTcpSocket
	, public JG_M::KPortableStepPool<KSocket_Client,1024,JG_S::KMTLock>
{
public:
	KSocket_Client();
	~KSocket_Client();

public:
	BOOL IsReady() const;
	void Reset();
	void Destroy();
	BOOL OnConnected();
	BOOL OnClosed();

public:
	volatile BOOL m_cnnReady;
	KChatAccount* m_chatAcct;
};

class KSocket_TcpListen : public KNormalTcpListenSocket
{
public:
	KSocket_TcpListen();
	~KSocket_TcpListen();

public:
	KServerTcpSocket* CreateSocketObject();
	BOOL OnClosed();
};
