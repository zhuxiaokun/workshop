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
//#include <redis/hiredis.h>
#include <CommSession/KPropertyMap.h>
#include <System/Misc/KStringtable.h>
#include "LogFileWriter.h"

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
	KStringPool m_strPool;
	LogFileWriter m_logWriter;

public:
	volatile BOOL m_redisReady;
	RingBuffer<char> m_tmpVerbBuffer;

public:
	time_t m_nowTime;
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
	void saveVerbToTmpBuffer(KPropertyMap& m);

public:
	virtual bool onEvent(int evtId, const void* data, size_t len);
	virtual bool onTimer(DWORD name, Identity64 cookie, const void* data);

private:
	bool _loadConfigureLua(const char* configureFile);
	bool _initNetwork();
	void processPacket(KSocket* pSock, JgPacketHead* packet);
	void _closeAllClient();
	
private:
	bool onTimer_Check_Socket_RecvBuffer(DWORD name, Identity64 cookie, const void* data);
	bool onTimer_CleanDeactiveFile(DWORD name, Identity64 cookie, const void* data);

public:
	const char* m_dirName;
	const char* m_platformName;
	string_64 m_baseFileName;
	string_32 m_fileExt;

	const char* getDataDir();
	const char* getPlatformName();
	const char* getBaseFileName();
	const char* getLogFileExt();
};
