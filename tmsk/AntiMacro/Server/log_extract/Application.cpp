#include "Application.h"
#include "app_events.h"
#include <System/File/KFile.h>
#include <lua/KLuaWrap.h>
#include "line_processor.h"
#include <lua/KLuaTableHolder.h>

//KStatusFile m_statusFile;
//KLuaTableWraper m_luaConfigureTable;

Application* g_pApp = NULL;

Application::Application():m_logDispatcher(NULL)
{
	m_charset = NULL;
	m_msFrameSleep = 0;
}

Application::~Application()
{

}

bool Application::initialize(const char* configureLuaFile)
{
	m_charset = KCharset::getCharset(KLanguage::lang_chinese);

	if(!this->_loadConfigureLua(configureLuaFile))
		return false;

	int logLevel = m_configure["log_level"];
	GetGlobalLogger().SetLogLevel(logLevel);

	const char* filePath = m_configure["status_file"];
	if(!this->_initStatusFile(filePath))
		return false;

	init_process_table();

	if(!this->_initNetwork())
		return false;

	int autoSearchWatch = m_configure["searchFilePath"];
	if(autoSearchWatch)
	{
		const char* dirName = m_configure["log_dir"];
		int yyyymmdd = m_configure["yyyymmdd"];
		WatchFileVector watchList;
		this->_searchLogFilePath(dirName, watchList);
		int n = watchList.size();
		for(int i=0; i<n; i++)
		{
			LogFileWatchUnit& watch = watchList.at(i);
			const char* pathName = watch.first.c_str();
			const char* fileName = watch.second.c_str();
			m_statusFile.addLogFile(pathName, fileName, yyyymmdd);
		}
	}

	int n = m_statusFile.getLogFileCount();
	for(int i=0; i<n; i++)
	{
		KLogFileReader* r = new KLogFileReader(m_statusFile.m_logFiles[i]);
		m_arrLogFileReader.push_back(r);
	}

	if(!KEventEntity::initialize("", NULL))
	{
		Log(LOG_ERROR, "error: event entity initialize.");
		return false;
	}
	
	return true;
}

void Application::breathe(int interval)
{
	KEventEntity::breathe(interval);

	m_msFrameSleep = 20;
	int n = m_arrLogFileReader.size();
	for(int i=0; i<n; i++)
	{
		KLogFileReader* r = m_arrLogFileReader.at(i);
		r->breathe(interval);
		if(r->m_frameLineCount)
			break;
	}

	if(m_logDispatcher)
	{
		int packetCount = m_logDispatcher->ReadPackets(m_packetBuffer, sizeof(m_packetBuffer), m_packetArray);
		for(int i=0; i<packetCount; i++)
		{
			void* packet = m_packetArray.at(i);
			JgPacketHead* head = (JgPacketHead*)packet;
			int cmd = head->command;
			const void* data = (void*)(head + 1);
			int length = head->length;
			m_gateDE.Process(m_logDispatcher, cmd, data, length);
		}
	}
}

void Application::finalize()
{
	m_sockServer.Finalize();
	m_statusFile.finalize();
}

bool Application::onInitialize(const char* initAttrs, KStreamSession* ss)
{
	this->startTimer(Timer_SearchActiveGame, 0, 00.0f, NULL);
	return true;
}

bool Application::onTimer(DWORD name, Identity64 cookie, const void* data)
{
	switch(name)
	{
	case Timer_SearchActiveGame:
		this->watchActiveGameLog();
		this->startTimer(Timer_SearchActiveGame, 0, 20.0f, NULL);
		break;
	default:
		break;
	}
	return true;
}

bool Application::_loadConfigureLua(const char* configureFile)
{
	int nret = LuaWraper.doScripts((char*)configureFile);
	if(nret == lwp_error)
	{
		Log(LOG_ERROR, "error: load configure file '%s', %s", configureFile, LuaWraper.GetLastError());
		return false;
	}

	KLuaTableWraper cfgTable("application_configure");

	const char* fieldName = "status_file";
	if(!cfgTable.hasTableField(fieldName, luaString_t))
	{
		Log(LOG_ERROR, "error: %s not found in configure", fieldName);
		return false;
	}

	fieldName = "log_process_server";
	if(!cfgTable.hasTableField(fieldName, luaTable_t))
	{
		Log(LOG_ERROR, "error: %s not found in configure", fieldName);
		return false;
	}

	fieldName = "watch_folders";
	if(!cfgTable.hasTableField(fieldName, luaTable_t))
	{
		Log(LOG_ERROR, "error: %s not found in configure", fieldName);
		return false;
	}

	m_configure = LuaWraper["application_configure"];

	LMREG::CppTable watchFiles = m_configure["watch_files"];
	int fileCount = watchFiles.size();
	for(int k=0; k<fileCount; k++)
	{
		const char* fileName = m_strpool.get(watchFiles[k]);
		m_watchFileArray.push_back(fileName);
	}

	LMREG::CppTable watchProcesses = m_configure["watch_processes"];
	int n = watchProcesses.size();
	for(int i=0; i<n; i++)
	{
		const char* procName = m_strpool.get(watchProcesses[i]);
		m_watchProcesses[procName] = true;
	}

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

	//m_statusFile.removeUnusedFiles();

	//if(!m_statusFile.getLogFileCount())
	//{
	//	Log(LOG_ERROR, "error: no log file watched.");
	//	return false;
	//}

	return true;
}

bool Application::isDispatcherReady() const
{
	return m_logDispatcher && m_logDispatcher->isReady();
}

bool Application::sendToLogDispatcher(int cmd, const void* data, int len)
{
	if(!m_logDispatcher)
	{
		Log(LOG_ERROR, "error: send to log dispatcher while not ready");
		return false;
	}
	m_logDispatcher->Send(cmd, data, len);
	return true;
}

bool Application::_initNetwork()
{
	const char* remoteIp = m_configure["log_process_server"]["ip"];
	int port = m_configure["log_process_server"]["port"];

	if(!m_sockServer.Initialize(1024, 2))
	{
		Log(LOG_ERROR, "error: initialize sockServer.");
		return false;
	}
	
	KSocket_LogDispatcher* pSock = new KSocket_LogDispatcher();
	if(!pSock->SetRemoteAddress(remoteIp, (WORD)port))
	{
		Log(LOG_ERROR, "error: invalid log dispatcher server address %s:% d", remoteIp, port);
		return false;
	}

	pSock->AddRef(); m_logDispatcher = pSock;
	m_sockServer.Attach(pSock);

	return true;
}

bool Application::isProcessCared(const char* procName)
{
	return m_watchProcesses.find(procName) != m_watchProcesses.end();
}

#include <Psapi.h>
Application::string_list_512 Application::searchActiveGameLogFolders()
{
	string_list_512 pathList;
	
	DWORD pids[1024];
	DWORD readed = 0;
	BOOL bRet = EnumProcesses(pids, sizeof(pids), &readed);
	if(!bRet)
	{
		Log(LOG_ERROR, "error: EnumProcesses, errCode:%d", GetLastError());
		return pathList;
	}

	char buf[512];
	int n = (int)(readed/sizeof(DWORD));
	for(int i=0; i<n; i++)
	{
		DWORD pid = pids[i];
		HANDLE hProc =  OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
		if(!hProc)
		{
			Log(LOG_DEBUG, "debug: fail open process pid:%u, errCode:%u", pid, GetLastError());
			continue;
		}
		DWORD len = sizeof(buf);
		BOOL bRet = QueryFullProcessImageName(hProc, 0, buf, &len);
		if(!bRet)
		{
			CloseHandle(hProc);
			Log(LOG_DEBUG, "debug: fail GetProcessImageFileName pid:%u, errCode:%u", pid, GetLastError());
			continue;
		}
		const char* shortName = JG_F::KFileUtil::basename_r(buf);
		if(g_pApp->isProcessCared(shortName))
		{
			char dirName[512];
			JG_F::KFileUtil::dirname_r(buf, dirName);
			strcat(dirName, "\\log");
			if(JG_F::KFileUtil::IsDir(dirName))
			{
				pathList.push_back(dirName);
			}
		}
		CloseHandle(hProc);
	}
	return pathList;
}

int Application::watchActiveGameLog()
{
	int addCount = 0;

	string_list_512 pathList = this->searchActiveGameLogFolders();
	int n = pathList.size();
	if(!n) return 0;

	int fileCount = m_watchFileArray.size();

	KDatetime dtNow; dtNow.AddDay(-5);
	KLocalDatetime ldt = dtNow.GetLocalTime();
	int yyyymmdd = ldt.year*10000 + ldt.month*100 + ldt.day;
	
	for(int i=0; i<n; i++)
	{
		const char* dirName = pathList.at(i).c_str();
		for(int k=0; k<fileCount; k++)
		{
			const char* fileName = m_watchFileArray[k];
			if(!m_statusFile.hasLogFile(dirName, fileName))
			{
				m_statusFile.addLogFile(dirName, fileName, yyyymmdd); addCount++;
				Log(LOG_ERROR, "debug: add watch file [%s%c%s]", dirName, PATH_SEPERATOR, fileName);
			}
		}
	}
	return addCount;
}

static JG_F::KDirectory::FileList fileList;
static void listDir(const char* dirName, KLuaTableWraper& tbl, WatchFileVector& watchList);
class TheFilter : public JG_F::KFileFilter
{
public:
	KLuaTableWraper& m_tbl;
	WatchFileVector& m_watchList;

	TheFilter(KLuaTableWraper& tbl, WatchFileVector& lst):m_tbl(tbl),m_watchList(lst)
	{
	
	}

	BOOL Accept(const JG_F::KFileData& fileData)
	{
		char fileName[32];
		const char* sname = fileData.m_shortName;
		
		char ch0 = sname[0];
		char ch1 = sname[1];
		char ch2 = sname[2];
		
		if(ch0 == '.' && ch1 == '\0')
			return FALSE;
		
		if(ch0 == '.' && ch1 == '.' && ch2 == '\0')
			return FALSE;

		JG_F::KFileData::FilePath filePath = fileData.getPath();
		if(fileData._isDir())
		{
			listDir(filePath.c_str(), m_tbl, m_watchList);
			return FALSE;
		}

		bool bRet = false;
		m_tbl.retTableCall<bool,const char*>(bRet, "isPathAccept", filePath.c_str());
		if(bRet)
		{
			const char* underline = strrchr(sname, '_');
			const char* dot = strrchr(sname, '.');
			
			int pos = 0;
			const char* pc = sname;
			
			while(pc < underline) { fileName[pos++] = *pc++; } pc = dot;
			while(*pc) fileName[pos++] = *pc++;
			fileName[pos] = '\0';

			m_watchList.push_back(LogFileWatchUnit(fileData.m_dirName, fileName));
		}

		return FALSE;
	}
};

static void listDir(const char* dirName, KLuaTableWraper& tbl, WatchFileVector& watchList)
{
	JG_F::KDirectory dir;
	dir.Open(dirName);
	TheFilter filter(tbl, watchList);
	dir.ListFile(fileList, &filter);
}

bool Application::_searchLogFilePath(const char* logDir, WatchFileVector& watchList)
{
	KLuaTableWraper tbl("application_configure");
	listDir(logDir, tbl, watchList);
	return true;
}
