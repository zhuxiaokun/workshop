#include "Application.h"
#include <System/File/KFile.h>
#include <lua/KLuaWrap.h>
//#include <redis/hiredis.h>
#include "app_event.h"

Application* g_pApp = NULL;

Application::Application()
{
	m_nowTime = time(NULL);
	m_charset = NULL;
	m_frameLineCount = 0;
	m_totalLineCount = 0;
	m_redisReady = FALSE;
	m_tmpVerbBuffer.SetCapacity(1024*1024*2);
	m_dirName = NULL;
	m_platformName = NULL;
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

	//if(!this->_connectToRedis())
	//	return false;

	if(!this->_initNetwork())
		return false;

	this->startTimer(AppTimer_CleanDeactiveFile, 0, 30.0f, NULL);
	
	return true;
}

class TheSocketFilter : public JgSocketFilter
{
public:
	BOOL Accept(KSocket* pSock)
	{
		if(pSock->GetUserData() != Socket_LogExtractor)
			return FALSE;
		return ((JgPacketReceiver*)(JgServerTcpSocket*)pSock)->ReadAvail() > 0;
	}
};

static TheSocketFilter g_socketFilter;
static JgPacketReceiver* g_socks[4096];
void Application::breathe(int interval)
{
	m_nowTime = time(NULL);
	m_frameLineCount = 0;
	KEventEntity::breathe(interval);
	int n = m_sockServer.LockSockets(&g_socketFilter, g_socks, 4096);
	for(int i=0; i<n; i++)
	{
		JgPacketReceiver* r = g_socks[i];
		int packetCount = r->ReadPackets(m_packetBuffer, sizeof(m_packetBuffer), m_packetArray);
		if(!packetCount) break;
		for(int k=0; k<packetCount; k++)
		{
			JgPacketHead* packet = (JgPacketHead*)m_packetArray.at(k);
			this->processPacket(r->GetSocket(), packet);
		}
	}
	m_sockServer.UnlockSockets(g_socks, n);
	m_totalLineCount += m_frameLineCount;
}

void Application::finalize()
{
	m_sockServer.Finalize();
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

	const char* fieldName = "listen";
	if(!cfgTable.hasTableField(fieldName, luaTable_t))
	{
		Log(LOG_ERROR, "error: %s not found in configure", fieldName);
		return false;
	}

	m_configure = LuaWraper["application_configure"];

	m_dirName = m_configure["data_dir"];
	m_platformName = m_configure["platform_name"];
	const char* fileName = m_configure["base_filename"];

	char buf[128];
	sprintf_k(buf, sizeof(buf), fileName);

	char* pDot = strrchr(buf, '.');
	if(pDot)
	{
		m_fileExt = pDot; *pDot = '\0';
		m_baseFileName = buf;
	}
	else
	{
		m_fileExt = "";
		m_baseFileName = fileName;
	}

	return true;
}

bool Application::_initNetwork()
{
	const char* listenIp = m_configure["listen"]["ip"];
	int port = m_configure["listen"]["port"];

	if(!m_sockServer.Initialize(4096, 5))
	{
		Log(LOG_ERROR, "error: initialize sockServer.");
		return false;
	}

	KSocket_Listener* pSock = new KSocket_Listener();
	if(!pSock->Listen(listenIp, port))
	{
		Log(LOG_ERROR, "error: listen at %s:%u", listenIp, port);
		return false;
	}
	m_sockServer.Attach(pSock);
	
	return true;
}

void Application::processPacket(KSocket* pSock, JgPacketHead* packet)
{
	DWORD ud = pSock->GetUserData();
	switch(ud)
	{
	case Socket_LogExtractor:
		m_gateED.Process(pSock, packet->command, packet+1, packet->length);
		break;
	}
	return;
}

bool Application::onEvent(int evtId, const void* data, size_t len)
{
	return true;
}

bool Application::onTimer(DWORD name, Identity64 cookie, const void* data)
{
	switch(name)
	{
	case AppTimer_Check_Socket_RecvBuffer:
		return this->onTimer_Check_Socket_RecvBuffer(name, cookie, data);
	case AppTimer_CleanDeactiveFile:
		return this->onTimer_CleanDeactiveFile(name, cookie, data);
	}
	return true;
}

class TheSocketFilter2 : public JgSocketFilter
{
public:
	BOOL Accept(KSocket* pSock)
	{
		if(pSock->GetUserData() != Socket_LogExtractor)
			return FALSE;
		return TRUE;
	}
};

static TheSocketFilter2 g_socketFilter2;
static JgPacketReceiver* g_socks_2[4096];
void Application::_closeAllClient()
{
	int n = m_sockServer.LockSockets(&g_socketFilter2, g_socks_2, 4096);
	for(int i=0; i<n; i++)
	{
		JgPacketReceiver* r = g_socks_2[i];
		r->GetSocket()->Close();
	}
	m_sockServer.UnlockSockets(g_socks_2, n);
}

void Application::saveVerbToTmpBuffer(KPropertyMap& m)
{
	char line[2048];
	int len = m.serialize(line, sizeof(line));
	m_tmpVerbBuffer.Write((const char*)&len, sizeof(len));
	m_tmpVerbBuffer.Write((const char*)&line, len);
}

bool Application::onTimer_Check_Socket_RecvBuffer(DWORD name, Identity64 cookie, const void* data)
{
	KSocket_LogExtractor* pSock = (KSocket_LogExtractor*)data;
	if(pSock->isReady())
	{
		if(pSock->ReadAvail() > 1024*1024)
		{
			if(!pSock->m_lastBusy)
			{
				DE_Busy packet = {1};
				pSock->Send(s_nDE_Busy, &packet, sizeof(packet));
				pSock->m_lastBusy = true;
			}
		}
		else if(pSock->ReadAvail() < 1024*10)
		{
			if(pSock->m_lastBusy)
			{
				DE_Busy packet = {0};
				pSock->Send(s_nDE_Busy, &packet, sizeof(packet));
				pSock->m_lastBusy = false;
			}
		}
		this->startTimer(AppTimer_Check_Socket_RecvBuffer, 0, 1.0f, (void*)data);
	}
	else
	{
		pSock->ReleaseRef();
	}
	return true;
}

bool Application::onTimer_CleanDeactiveFile(DWORD name, Identity64 cookie, const void* data)
{
	m_logWriter.removeDeactiveFiles(time(NULL));
	this->startTimer(AppTimer_CleanDeactiveFile, 0, 30.0f, (void*)data);
	return true;
}

void process_verb(KPropertyMap& m, bool fromNetwork);

const char* Application::getDataDir()
{
	return m_dirName;
}

const char* Application::getPlatformName()
{
	return m_platformName;
}

const char* Application::getBaseFileName()
{
	return m_baseFileName.c_str();
}

const char* Application::getLogFileExt()
{
	return m_fileExt.c_str();
}
