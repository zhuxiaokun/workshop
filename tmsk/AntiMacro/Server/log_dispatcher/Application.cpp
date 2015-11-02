#include "Application.h"
#include <System/File/KFile.h>
#include <lua/KLuaWrap.h>
#include <redis/hiredis.h>
#include "app_event.h"

Application* g_pApp = NULL;

Application::Application()
{
	m_charset = NULL;
	m_frameLineCount = 0;
	m_totalLineCount = 0;
	m_redisReady = FALSE;
	m_redisCtx = NULL;
	m_tmpVerbBuffer.SetCapacity(1024*1024*2);
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

	if(!this->_connectToRedis())
		return false;

	if(!this->_initNetwork())
		return false;
	
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
	if(m_redisCtx)
	{
		redisFree(m_redisCtx);
		m_redisCtx = NULL;
	}
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
	return true;
}

bool Application::_connectToRedis()
{
	ASSERT(!m_redisCtx);

	const char* redisIp = m_configure["redis_server"]["ip"];
	unsigned short redisPort = (unsigned short)(int)m_configure["redis_server"]["port"];

	struct timeval tv = {1, 0};
	redisContext* redisCtx = redisConnectWithTimeout(redisIp, redisPort, tv);
	if(redisCtx->err)
	{
		Log(LOG_ERROR, "error: connect to REDIS server, %s", redisCtx->errstr);
		return false;
	}

	redisReply* reply;

	reply = (redisReply*)redisCommand(redisCtx, "PING");
	if(!reply || !reply->str)
	{
		Log(LOG_CONSOLE|LOG_DEBUG, "PING: no reply");
		freeReplyObject(reply);
		redisFree(redisCtx);
		return false;
	}

	if(stricmp(reply->str, "PONG"))
	{
		Log(LOG_CONSOLE|LOG_DEBUG, "PING recv '%s'", reply->str);
		freeReplyObject(reply);
		redisFree(redisCtx);
		return false;
	}

	freeReplyObject(reply);
	
	m_redisCtx = redisCtx;
	m_redisReady = TRUE;

	this->onRedisConnected();
	return true;
}

bool Application::_isRedisReady()
{
	if(!m_redisCtx)
		return false;

	redisReply* reply;

	redisContext* redisCtx = m_redisCtx;
	reply = (redisReply*)redisCommand(redisCtx, "PING");
	if(!reply || !reply->str)
	{
		Log(LOG_CONSOLE|LOG_DEBUG, "PING: no reply");
		freeReplyObject(reply);
		return false;
	}

	if(stricmp(reply->str, "PONG"))
	{
		Log(LOG_CONSOLE|LOG_DEBUG, "PING recv '%s'", reply->str);
		freeReplyObject(reply);
		return false;
	}

	freeReplyObject(reply);
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
	case AppTimer_ConnectToRedis:
		return this->onTimer_ConnectToRedis(name, cookie, data);
	case AppTimer_Check_Socket_RecvBuffer:
		return this->onTimer_Check_Socket_RecvBuffer(name, cookie, data);
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

void Application::reconnectToRedis()
{
	this->_closeAllClient();

	if(m_redisCtx)
	{
		redisFree(m_redisCtx);
		m_redisCtx = NULL;
		m_redisReady = FALSE;
	}

	if(this->_connectToRedis())
		return;

	this->startTimer(AppTimer_ConnectToRedis, 0, 5.0f, NULL);
	return;
}

void Application::saveVerbToTmpBuffer(KPropertyMap& m)
{
	char line[2048];
	int len = m.serialize(line, sizeof(line));
	m_tmpVerbBuffer.Write((const char*)&len, sizeof(len));
	m_tmpVerbBuffer.Write((const char*)&line, len);
}

bool Application::onTimer_ConnectToRedis(DWORD name, Identity64 cookie, const void* data)
{
	if(this->_connectToRedis())
		return true;
	this->startTimer(AppTimer_ConnectToRedis, 0, 5.0f, NULL);
	return true;
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


void process_verb(KPropertyMap& m, bool fromNetwork);
void Application::onRedisConnected()
{
	int len = 0;
	char line[2048];

	while(1)
	{
		if(m_tmpVerbBuffer.ReadAvailable() < sizeof(int))
			break;

		m_tmpVerbBuffer.Read(sizeof(int), (char*)&len);
		if(m_tmpVerbBuffer.ReadAvailable() < len)
		{
			Log(LOG_ERROR, "error: in consist verb buffer data");
			m_tmpVerbBuffer.Reset();
			break;
		}
		m_tmpVerbBuffer.Read(len, (char*)&line);
		line[len] = '\0';

		KPropertyMap m;
		if(!m.parseLine(line))
		{
			Log(LOG_ERROR, "error: in consist verb buffer data, '%s'", line);
			m_tmpVerbBuffer.Reset();
			break;
		}

		process_verb(m, false);
	}
}
