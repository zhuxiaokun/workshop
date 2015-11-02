#pragma once
#include <EventSystem/KEventEntity.h>
#include <lua/KLuaWrap.h>
#include <System/Sync/KSync.h>
#include <SockServer/JgPacketSpecific.h>
#include <redis/hiredis.h>
#include "KSocketDefine.h"
#include <System/Misc/KStringtable.h>
#include "KGameAccount.h"
#include "KAnalysis.h"
#include "KIpAnalysis.h"
#include "KDevAnalysis.h"
#include <kredis/redisConnectionPool.h>
#include "KRedisEraser.h"

class Application;
extern Application* g_pApp;
#define g_stringPool (g_pApp->m_stringPool)

class Application
	: public KEventEntity_luaMaster
	, public KEventSource
{
public:
	typedef JG_C::DynArray<KRedisConnection*,256,256> RedisArray;

public:
	LMREG::CppTable m_configure;
	JG_S::KSync_PulseEvent m_inputEvent;

public:
	int m_masterRedisIndex;
	RedisArray m_redisArray;

public:
	KStringPool m_stringPool;
	KAnalysis m_analysis;
	KIpAnalysis m_ipAnalysis;
	KDevAnalysis m_devAnalysis;
	KRedisEraser m_redisEraser;

public:
	JgSockServer m_sockServer;
	char m_packetBuffer[100*1024];
	JgPacketReceiver::PacketPtrArray m_packetArray;
	KPacketGate_Client m_gateED;

public:
	Application();
	~Application();

public:
	bool initialize(const char* cfgFile);
	void breathe(int ms);
	void finalize();

public:
	void reconnectToRedis();

	int redisCount() const;
	int masterRedisIndex() const;
	KRedis redisAt(int index);
	KRedis masterRedis();
	KRedisConnection* masterRedisConnection();

public:
	virtual bool onTimer(DWORD name, Identity64 cookie, const void* data);
	virtual bool getLuaClass(char* name, size_t len);
	virtual bool setLuaRuntime();

private:
	bool _loadConfigureLua(const char* configureFile);
	bool _connectToRedis();
	bool _isRedisReady();
	bool _initNetwork();
	void _onRedisConnected(KRedisConnection* cnn);
	bool onTimer_ConnectToRedis(DWORD name, Identity64 cookie, const void* data);

private:
	void _networkBreathe(int ms);
	void _processPacket(KSocket* pSock, JgPacketHead* packet);

public:
	BeginDefLuaClassNoneGc(Application);
		InheritFrom(KEventEntity_luaMaster);
	EndDef;
};
