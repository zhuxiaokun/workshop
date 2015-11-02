#pragma once
#include <System/KPlatform.h>
#include <lua/KLuaTableHolder.h>
#include <lua/CppTable.h>
#include <EventSystem/KEventEntity.h>
#include "app_defines.h"
#include <System/Charset/KCharset.h>
#include <System/Collections/DynArray.h>
#include "KSocketDefine.h"
#include <System/Memory/RingBuffer.h>
#include <redis/hiredis.h>
#include <CommSession/KPropertyMap.h>

class Application;
extern Application* g_pApp;

class Application
	: public KEventSource
	, public KEventEntity
{
public:
	KCharset* m_charset;
	LMREG::CppTable m_configure;
	JG_S::KSync_PulseEvent m_inputEvent;
	int m_frameLineCount;
	INT64 m_totalLineCount;

public:
	volatile BOOL m_redisReady;
	redisContext* m_redisCtx;
	RingBuffer<char> m_tmpVerbBuffer;

public:
	JgSockServer m_sockServer;
	char m_packetBuffer[100*1024];
	JgPacketReceiver::PacketPtrArray m_packetArray;
	KPacketGate_ED m_gateED;
	
public:
	Application();
	~Application();

public:
	bool initialize(const char* configureLuaFile);
	void breathe(int interval);
	void finalize();
	void reconnectToRedis();
	void saveVerbToTmpBuffer(KPropertyMap& m);

public:
	virtual bool onEvent(int evtId, const void* data, size_t len);
	virtual bool onTimer(DWORD name, Identity64 cookie, const void* data);

private:
	bool _loadConfigureLua(const char* configureFile);
	bool _connectToRedis();
	bool _isRedisReady();
	bool _initNetwork();
	void processPacket(KSocket* pSock, JgPacketHead* packet);
	void _closeAllClient();
	void onRedisConnected();
	
private:
	bool onTimer_ConnectToRedis(DWORD name, Identity64 cookie, const void* data);
	bool onTimer_Check_Socket_RecvBuffer(DWORD name, Identity64 cookie, const void* data);
};
