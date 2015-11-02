#include "Application.h"
#include "app_event.h"

Application* g_pApp = NULL;

Application::Application()
{
	m_masterRedisIndex = 0;
}

Application::~Application()
{

}

bool Application::initialize(const char* cfgFile)
{
	if(!this->_loadConfigureLua(cfgFile))
		return false;

	if(!this->_connectToRedis())
		return false;
	
	if(!this->_initNetwork())
		return false;

	LuaWraper["analysis"] = &m_analysis;
	LuaWraper["ipAnalysis"] = &m_ipAnalysis;
	LuaWraper["devAnalysis"] = &m_devAnalysis;
	LuaWraper["redisEraser"] = &m_redisEraser;

	if(!KEventEntity_luaMaster::initialize("", NULL))
	{
		Log(LOG_CONSOLE|LOG_ERROR, "error: init lua entity");
		return false;
	}

	return true;
}

void Application::breathe(int ms)
{
	KEventEntity_luaMaster::breathe(ms);
	this->_networkBreathe(ms);
}

void Application::finalize()
{
	m_sockServer.Finalize();
	for(int i=0; i<m_redisArray.size(); i++)
	{
		KRedisConnection* cnn = m_redisArray.at(i);
		cnn->close(); delete cnn;
	}
	m_redisArray.clear();
}

void Application::reconnectToRedis()
{
	int n = m_redisArray.size();
	for(int i=0; i<n; i++)
	{
		KRedisConnection* cnn = m_redisArray.at(i);
		cnn->close();
		delete cnn;
	}	m_redisArray.clear();

	if(this->_connectToRedis())
		return;

	this->startTimer(AppTimer_ConnectToRedis, 0, 5.0f, NULL);
	return;
}

int Application::redisCount() const
{
	return m_redisArray.size();
}

int Application::masterRedisIndex() const
{
	return m_masterRedisIndex;
}

KRedis Application::redisAt(int index)
{
	KRedisConnection* cnn = m_redisArray.at(index);
	return KRedis(cnn->m_redisCtx);
}

KRedis Application::masterRedis()
{
	return this->redisAt(m_masterRedisIndex);
}

KRedisConnection* Application::masterRedisConnection()
{
	return m_redisArray.at(m_masterRedisIndex);
}

bool Application::onTimer(DWORD name, Identity64 cookie, const void* data)
{
	switch(name)
	{
	case AppTimer_ConnectToRedis:
		return this->onTimer_ConnectToRedis(name, cookie, data);
	}
	return KEventEntity_luaMaster::onTimer(name, cookie, data);
}

bool Application::getLuaClass(char* name, size_t len)
{
	strcpy_k(name, len, "macro_analysis_server");
	return true;
}

bool Application::setLuaRuntime()
{
	this->setTableField("this", (KEventEntity_luaMaster*)this);
	return true;
}

bool Application::_loadConfigureLua(const char* configureFile)
{
	int nret = LuaWraper.doScripts((char*)configureFile);
	if(nret == lwp_error)
	{
		Log(LOG_ERROR|LOG_CONSOLE, "error: load configure file '%s', %s", configureFile, LuaWraper.GetLastError());
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
	ASSERT(m_redisArray.empty());

	const  LMREG::CppTable& redisList = (const LMREG::CppTable&)m_configure["redis_server_list"];
	int n = redisList.size();

	for(int i=0; i<n; i++)
	{
		const LMREG::CppTable& item = (const LMREG::CppTable&)redisList[i];
		const char* redisIp = item["ip"];
		int redisPort = item["port"];

		KRedisConnection* cnn = new KRedisConnection();
		if(!cnn->connect(redisIp, redisPort))
		{
			Log(LOG_ERROR|LOG_CONSOLE, "error: connect to redis %s:%d", redisIp, redisPort);
			return false;
		}
		m_redisArray.push_back(cnn);
		this->_onRedisConnected(cnn);
	}

	m_masterRedisIndex = m_configure["master_redis"];
	if(m_masterRedisIndex < 0 || m_masterRedisIndex >= m_redisArray.size())
	{
		Log(LOG_CONSOLE|LOG_ERROR, "error: master_redis index %d invalid", m_masterRedisIndex);
		return false;
	}

	return true;
}

bool Application::_isRedisReady()
{
	int n = m_redisArray.size();
	if(!n) return false;
	for(int i=0; i<n; i++)
	{
		KRedisConnection* cnn = m_redisArray.at(i);
		if(!cnn->checkReady())
			return false;
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

void Application::_onRedisConnected(KRedisConnection* cnn)
{
	Log(LOG_WARN, "warn: REDIS connected.");
}

bool Application::onTimer_ConnectToRedis(DWORD name, Identity64 cookie, const void* data)
{
	if(this->_connectToRedis())
		return true;
	this->startTimer(AppTimer_ConnectToRedis, 0, 5.0f, NULL);
	return true;
}

class TheSocketFilter : public JgSocketFilter
{
public:
	BOOL Accept(KSocket* pSock)
	{
		if(pSock->GetUserData() != Socket_Client)
			return FALSE;
		return ((JgPacketReceiver*)(JgServerTcpSocket*)pSock)->ReadAvail() > 0;
	}
};

static TheSocketFilter g_socketFilter;
static JgPacketReceiver* g_socks[4096];

void Application::_networkBreathe(int ms)
{
	int n = m_sockServer.LockSockets(&g_socketFilter, g_socks, 4096);
	for(int i=0; i<n; i++)
	{
		JgPacketReceiver* r = g_socks[i];
		while(1)
		{
			int packetCount = r->ReadPackets(m_packetBuffer, sizeof(m_packetBuffer), m_packetArray);
			if(!packetCount) break;
			for(int k=0; k<packetCount; k++)
			{
				JgPacketHead* packet = (JgPacketHead*)m_packetArray.at(k);
				this->_processPacket(r->GetSocket(), packet);
			}
		}
	}
	m_sockServer.UnlockSockets(g_socks, n);
}

void Application::_processPacket(KSocket* pSock, JgPacketHead* packet)
{
	DWORD ud = pSock->GetUserData();
	switch(ud)
	{
	case Socket_Client:
		m_gateED.Process(pSock, packet->command, packet+1, packet->length);
		break;
	}
	return;
}
