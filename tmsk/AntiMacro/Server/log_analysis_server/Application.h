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
#include <System/Misc/KStringtable.h>
#include "KGameAccount.h"
#include "KAnalysis.h"
#include "KIpAnalysis.h"
#include "KDevAnalysis.h"

class Application;
extern Application* g_pApp;
#define g_stringPool (g_pApp->m_strpool)

class Application : public KEventEntity_luaMaster
{
public:
	typedef JG_C::DynArray<KLogFileReader*> LogFileReaderArray;

public:
	KCharset* m_charset;
	KStatusFile m_statusFile;
	LMREG::CppTable m_configure;
	LogFileReaderArray m_arrLogFileReader;
	int m_msFrameSleep;
	_KStringPool<10240> m_strpool;

public:
	KAnalysis m_analysis;
	KIpAnalysis m_ipAnalysis;
	KDevAnalysis m_devAnalysis;

public:
	bool m_useLuaDirFunc;
	KLuaTableWraper* m_cfgTable;

public:
	Application();
	~Application();

public:
	bool initialize(const char* configureLuaFile);
	void breathe(int interval);
	void finalize();

public:
	bool isDatabaseReady();

public:
	bool getLuaClass(char* name, size_t len);
	bool setLuaRuntime();

private:
	bool _loadConfigureLua(const char* configureFile);
	bool _initStatusFile(const char* filePath);
};
