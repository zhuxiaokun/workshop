#pragma once
#include <System/KPlatform.h>
#include "KStatusFile.h"
#include <lua/KLuaTableHolder.h>
#include <lua/CppTable.h>
#include <EventSystem/KEventEntity.h>
#include "app_defines.h"
#include <System/Charset/KCharset.h>
#include "KLogFileReader.h"
#include <System/Collections/DynArray.h>
#include "KSocketDefine.h"
#include <System/Collections/KTree.h>
#include <System/Misc/KStringtable.h>

class Application;
extern Application* g_pApp;

typedef KPair<string_512,string_32> LogFileWatchUnit;
typedef JG_C::KVector<LogFileWatchUnit> WatchFileVector;

class Application : public KEventEntity
{
public:
	enum AppTimer
	{
		Timer_SearchActiveGame = 100,
	};

public:
	typedef JG_C::KString<512> string_512;
	typedef JG_C::KVector<string_512> string_list_512;
	typedef JG_C::DynArray<KLogFileReader*> LogFileReaderArray;
	typedef JG_C::KMapByTree<ccstr,bool,KccstriCmp> ProcessNameMap;
	typedef JG_C::DynArray<ccstr> WatchFileArray;

public:
	KStringPool m_strpool;
	KCharset* m_charset;
	KStatusFile m_statusFile;
	LMREG::CppTable m_configure;
	LogFileReaderArray m_arrLogFileReader;
	WatchFileArray m_watchFileArray;
	ProcessNameMap m_watchProcesses;
	int m_msFrameSleep;

public:
	JgSockServer m_sockServer;
	KSocket_LogDispatcher* m_logDispatcher;
	char m_packetBuffer[100*1024];
	JgPacketReceiver::PacketPtrArray m_packetArray;
	KPacketGate_DE m_gateDE;

public:
	Application();
	~Application();

public:
	bool initialize(const char* configureLuaFile);
	void breathe(int interval);
	void finalize();

public:
	bool onInitialize(const char* initAttrs, KStreamSession* ss);
	bool onTimer(DWORD name, Identity64 cookie, const void* data);

private:
	bool _loadConfigureLua(const char* configureFile);
	bool _initStatusFile(const char* filePath);
	bool _initNetwork();

public:
	bool isDispatcherReady() const;
	bool sendToLogDispatcher(int cmd, const void* data, int len);

public:
	bool isProcessCared(const char* procName);
	string_list_512 searchActiveGameLogFolders();
	int watchActiveGameLog();

public:
	bool _searchLogFilePath(const char* logDir, WatchFileVector& watchList);
};
