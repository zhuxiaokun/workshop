#include "Application.h"
#include "app_events.h"
#include <System/File/KFile.h>
#include <lua/KLuaWrap.h>
#include "line_processor.h"

//KStatusFile m_statusFile;
//KLuaTableWraper m_luaConfigureTable;

Application* g_pApp = NULL;

Application::Application()
{
	m_charset = NULL;
	m_msFrameSleep = 0;
	m_useLuaDirFunc = false;
	m_cfgTable = NULL;
}

Application::~Application()
{

}

bool Application::isDatabaseReady()
{
	return this->getTableField_b("cnnReady", false);
}

bool Application::getLuaClass(char* name, size_t len)
{
	strcpy_k(name, len, "application_configure");
	return true;
}

bool Application::setLuaRuntime()
{
	this->setTableField("this", (KEventEntity_luaMaster*)this);
	return true;
}

bool Application::initialize(const char* configureLuaFile)
{
	m_charset = KCharset::getCharset(KLanguage::lang_chinese);

	if(!this->_loadConfigureLua(configureLuaFile))
		return false;

	int logLevel = m_configure["log_level"];
	GetGlobalLogger().SetLogLevel(logLevel);

	LuaWraper["analysis"] = &m_analysis;
	LuaWraper["ipAnalysis"] = &m_ipAnalysis;
	LuaWraper["devAnalysis"] = &m_devAnalysis;

	init_process_table();

	if(!KEventEntity_luaMaster::initialize("", NULL))
	{
		Log(LOG_CONSOLE|LOG_ERROR, "error: initialize LUA event entity!");
		return false;
	}

	return true;
}

void Application::breathe(int interval)
{
	KEventEntity_luaMaster::breathe(interval);
	//m_msFrameSleep = 20;
	//int n = m_arrLogFileReader.size();
	//for(int i=0; i<n; i++)
	//{
	//	KLogFileReader* r = m_arrLogFileReader.at(i);
	//	r->breathe(interval);
	//	if(r->m_breatheLineCount)
	//		break;
	//}
}

void Application::finalize()
{
	m_statusFile.finalize();
}

bool Application::_loadConfigureLua(const char* configureFile)
{
	int nret = LuaWraper.doScripts((char*)configureFile);
	if(nret == lwp_error)
	{
		Log(LOG_ERROR, "error: load configure file '%s', %s", configureFile, LuaWraper.GetLastError());
		return false;
	}

	m_cfgTable = new KLuaTableWraper("application_configure");
	KLuaTableWraper& cfgTable = *m_cfgTable;

	//const char* fieldName = "status_file";
	//if(!cfgTable.hasTableField(fieldName, luaString_t))
	//{
	//	Log(LOG_ERROR, "error: %s not found in configure", fieldName);
	//	return false;
	//}

	//fieldName = "log_process_server";
	//if(!cfgTable.hasTableField(fieldName, luaTable_t))
	//{
	//	Log(LOG_ERROR, "error: %s not found in configure", fieldName);
	//	return false;
	//}

	//fieldName = "watch_folders";
	//if(!cfgTable.hasTableField(fieldName, luaTable_t))
	//{
	//	Log(LOG_ERROR, "error: %s not found in configure", fieldName);
	//	return false;
	//}

	m_configure = LuaWraper["application_configure"];
	m_useLuaDirFunc = m_cfgTable->hasTableField("getLogDir", luaFunction_t);

	return true;
}

bool Application::_initStatusFile(const char* filePath)
{
	if(!m_statusFile.initialize(filePath))
		return false;

	LMREG::CppTable watches = m_configure["watch_folders"];

	int n = watches.size();
	for(int i=0; i<n; i++)
	{
		LMREG::CppTable item = watches.getVal(i);
		const char* dirName = item["dir"];
		const char* fileName = item["file"];
		int yyyymmdd = item["from"];
		m_statusFile.addLogFile(dirName, fileName, yyyymmdd);
	}

	m_statusFile.removeUnusedFiles();

	if(!m_statusFile.getLogFileCount())
	{
		Log(LOG_ERROR, "error: no log file watched.");
		return false;
	}

	return true;
}
