#include "KIpAnalysis.h"
#include "Application.h"

//// KServerOfIp

KServerOfIp::KServerOfIp():m_name(NULL),m_ipName(NULL)
{

}

KServerOfIp::~KServerOfIp()
{
	this->reset();
}

void KServerOfIp::append(const char* acctPlayer, int lastActive)
{
	char buf[256];
	strcpy_k(buf, sizeof(buf), acctPlayer);
	char* pdot = strrchr(buf, '.'); *pdot = '\0';

	acctPlayer = g_stringPool.get(buf);
	const char* player = g_stringPool.get(pdot + 1);

	InnerPlayer* p = InnerPlayer::Alloc();
	p->m_serverName = m_name;
	p->m_acctName = acctPlayer;
	p->m_player = player;
	p->m_lastActive = lastActive;

	m_playerList.push_back(p);
}

void KServerOfIp::reset()
{
	JG_C::SelfListNode<KServerOfIp>::reset();
	m_name = NULL;
	m_ipName = NULL;
	while(!m_playerList.empty())
	{
		InnerPlayer* p = m_playerList.pop_front();
		InnerPlayer::Free(p);
	}
}

const char* KServerOfIp::name() const
{
	return m_name;
}

bool KServerOfIp::foreach(LMREG::l_func func, l_stackval ctx)
{
	InnerPlayer* p = m_playerList.begin();
	while(p)
	{
		if(!func.r_invoke<bool,InnerPlayer*,l_stackval>(p, ctx))
			return false;
		p = m_playerList.next(p);
	}
	return true;
}

//// KIp

//const char* m_ip;
//ServerList m_serverList;

KIp::KIp():m_name(NULL)
{

}

KIp::~KIp()
{
	this->reset();
}

KServerOfIp* KIp::addServer(const char* serverName)
{
	ServerMap::iterator it = m_serverMap.find(serverName);
	if(it != m_serverMap.end())
		return it->second;

	KServerOfIp* pServer = KServerOfIp::Alloc();
	pServer->m_name = g_stringPool.get(serverName);
	pServer->m_ipName = m_name;
	m_serverList.push_back(pServer);
	return pServer;
}

int KIp::getServerCount()
{
	return m_serverList.size();
}

int KIp::getAcctCount()
{
	int sum = 0;
	KServerOfIp* pServer = m_serverList.begin();
	while(pServer)
	{
		sum += pServer->m_playerList.size();
		pServer = m_serverList.next(pServer);
	}
	return sum;
}

bool KIp::foreachServer(l_func func, l_stackval ctx)
{
	KServerOfIp* pServer = m_serverList.begin();
	while(pServer)
	{
		if(!func.r_invoke<bool,KServerOfIp*,l_stackval>(pServer,ctx))
			return false;
		pServer = m_serverList.next(pServer);
	}
	return true;
}

class server_scan_visitor : public scan_visitor
{
public:
	int prefix_len;
	KLargeNameList& m_lst;
	server_scan_visitor(const char* ip, KLargeNameList& serverNameList):m_lst(serverNameList)
	{
		char prefix[128];
		prefix_len = sprintf_k(prefix, sizeof(prefix), "server.ip.%s.", ip);
	}
	virtual bool accept(const char* key)
	{
		m_lst.append(key + prefix_len);
		return true;
	}
};

bool KIp::scanServerList(KRedis& redis, KLargeNameList& serverNameList)
{
	char pattern[128];
	sprintf_k(pattern, sizeof(pattern), "server.ip.%s.*", m_name);
	server_scan_visitor v(m_name, serverNameList);
	redis.scan(pattern, v);
	return true;
}

class serverOfIp_visitor : public hscan_visitor
{
public:
	KServerOfIp* m_pServer;
	serverOfIp_visitor(KServerOfIp* p):m_pServer(p) { }
	bool accept(const char* field, const char* val)
	{
		const char* acct = field;
		int time = str2int(val);
		m_pServer->append(acct, time);
		return true;
	}
};

bool KIp::load(KRedis& redis, KLargeNameList& serverNameList)
{
	char keyName[128];
	KLargeNameList::iterator it = serverNameList.begin();
	for(; it != serverNameList.end(); it++)
	{
		const char* serverName = *it;
		KServerOfIp* pServer = this->addServer(serverName);
		sprintf_k(keyName, sizeof(keyName), "server.ip.%s.%s", m_name, serverName);

		serverOfIp_visitor v(pServer);
		if(!redis.hscan(keyName, "*", v))
			Log(LOG_DEBUG, "debug: hscan %s, %s", keyName, redis.m_ctx->errstr);
	}
	return true;
}

void KIp::freeServer(KServerOfIp* p)
{
	p->reset();
	KServerOfIp::Free(p);
}

void KIp::reset()
{
	m_name = NULL;
	while(!m_serverList.empty())
	{
		KServerOfIp* p = m_serverList.pop_front();
		this->freeServer(p);
	}
	m_serverMap.clear();
}

KServerOfIp* KIp::getServerObject(const char* serverName)
{
	ServerMap::iterator it = m_serverMap.find(serverName);
	if(it == m_serverMap.end())
		return NULL;
	return it->second;
}

bool KIp::foreachPlayer(l_func func, l_stackval ctx)
{
	KServerOfIp* pServer = m_serverList.begin();
	while(pServer)
	{
		if(!pServer->foreach(func, ctx))
			return false;
		pServer = m_serverList.next(pServer);
	}
	return true;
}

//// KIpAnalysis

KIpAnalysis::KIpAnalysis()
{

}

KIpAnalysis::~KIpAnalysis()
{

}

void KIpAnalysis::reset()
{
	m_ipNameList.reset();
	m_serverNameList.reset();
	m_serverNameMap.clear();
	m_ipNameMap.clear();
	m_ipNameTable.clear();
	while(!m_ipObjTable.empty())
	{
		IpObjectTable::iterator it = m_ipObjTable.begin();
		KIp* p = it->second;
		this->freeIp(p);
		m_ipObjTable.erase(it);
	}
}

class ipServer_visitor : public scan_visitor
{
public:
	char ip[256];
	static const int prefix_len = 10; //strlen("server.ip.")
	ipServer_visitor() { }
	bool accept(const char* key)
	{
		const char* p0 = key + prefix_len;
		
		const char* p = p0;
		p = strchr(p, '.'); if(!p) return true; p += 1;
		p = strchr(p, '.'); if(!p) return true; p += 1;
		p = strchr(p, '.'); if(!p) return true; p += 1;
		p = strchr(p, '.'); if(!p) return true;

		const char* server = p + 1;

		int len = (int)(p - p0);
		memcpy(ip, p0, len); ip[len] = '\0';
		
		g_pApp->m_ipAnalysis._addIpServer(ip, server);
		return true;
	}
};

bool KIpAnalysis::scanIpList()
{
	//this->reset();
	KRedis redis = g_pApp->masterRedis();
	ipServer_visitor v;
	const char* pattern = "server.ip.*";
	redis.scan(pattern, v);
	return true;
}

bool KIpAnalysis::scanPatternIpList(const char* ip_pattern)
{
	KRedis redis = g_pApp->masterRedis();
	ipServer_visitor v;
	char pattern[128];
	sprintf_k(pattern, sizeof(pattern), "server.ip.%s.*", ip_pattern);
	redis.scan(pattern, v);
	return true;
}

KIp* KIpAnalysis::loadIp(const char* ip)
{
	IpObjectTable::iterator itt = m_ipObjTable.find(ip);
	if(itt != m_ipObjTable.end())
		return itt->second;

	IpNameTable::iterator it = m_ipNameTable.find(ip);
	if(it == m_ipNameTable.end())
		return NULL;
	
	ip = it->first;
	KIp* pIp = this->allocIp(ip);

	char keyName[128];

	//// master redis
	{
		KRedis redis = g_pApp->masterRedis();
		NameMap& nm = it->second;
		NameMap::iterator it2 = nm.begin();
		for(; it2 != nm.end(); it2++)
		{
			const char* serverName = it2->first;
			KServerOfIp* pServer = pIp->addServer(serverName);
			sprintf_k(keyName, sizeof(keyName), "server.ip.%s.%s", ip, serverName);

			serverOfIp_visitor v(pServer);
			if(!redis.hscan(keyName, "*", v))
			{
				this->freeIp(pIp);
				Log(LOG_CONSOLE|LOG_ERROR, "error: hscan %s, %s", keyName, redis.m_ctx->errstr);
				return NULL;
			}
		}
	}
	/// other redis
	int n = g_pApp->redisCount();
	int masterIndex = g_pApp->masterRedisIndex();
	for(int i=0; i<n; i++)
	{
		if(i != masterIndex)
		{
			KRedis redis = g_pApp->redisAt(i);
			KLargeNameList serverNameList;
			if(pIp->scanServerList(redis, serverNameList))
			{
				pIp->load(redis, serverNameList);
			}
		}
	}
	
	m_ipObjTable[ip] = pIp;
	return pIp;
}

bool KIpAnalysis::foreachIpName(LMREG::l_func func, l_stackval ctx)
{
	IpNameTable::iterator it = m_ipNameTable.begin();
	for(; it != m_ipNameTable.end(); it++)
	{
		const char* ip = it->first;
		if(!func.r_invoke<bool,const char*,l_stackval>(ip, ctx))
			return false;
	}
	return true;
}

bool KIpAnalysis::foreachIpObject(LMREG::l_func func, l_stackval ctx)
{
	IpObjectTable::iterator it = m_ipObjTable.begin();
	for(; it != m_ipObjTable.end(); it++)
	{
		KIp* p = it->second;
		if(!func.r_invoke<bool,KIp*,l_stackval>(p, ctx))
			return false;
	}
	return true;
}

bool KIpAnalysis::foreachIpServer(l_func func, l_stackval ctx)
{
	IpNameTable::iterator it = m_ipNameTable.begin();
	for(; it != m_ipNameTable.end(); it++)
	{
		const char* ip = it->first;
		NameMap& nm = it->second;
		NameMap::iterator i2 = nm.begin();
		for(; i2!=nm.end(); i2++)
		{
			const char* sn = i2->first;
			if(!func.r_invoke<bool,const char*,const char*,l_stackval>(ip,sn,ctx))
				return false;
		}
	}
	return true;
}

KIp* KIpAnalysis::getIpObject(const char* ip)
{
	IpObjectTable::iterator it = m_ipObjTable.find(ip);
	if(it == m_ipObjTable.end())
		return NULL;
	return it->second;
}

const char* KIpAnalysis::_addIpName(const char* ip)
{
	NameMap::iterator it = m_ipNameMap.find(ip);
	if(it != m_ipNameMap.end())
		return it->first;
	ip = m_ipNameList.append(ip);
	m_ipNameMap[ip] = '\0';
	return ip;
}

const char* KIpAnalysis::_addServerName(const char* server)
{
	NameMap::iterator it = m_serverNameMap.find(server);
	if(it != m_serverNameMap.find(server))
		return it->first;
	server = m_serverNameList.append(server);
	m_serverNameMap[server] = '\0';
	return server;
}

bool KIpAnalysis::_addIpServer(const char* ip, const char* server)
{
	ip = this->_addIpName(ip);
	server = this->_addServerName(server);

	IpNameTable::iterator it = m_ipNameTable.find(ip);
	if(it != m_ipNameTable.end())
	{
		NameMap& nm = it->second;
		nm[server] = '\0';
		return true;
	}
	else
	{
		NameMap& nm = m_ipNameTable[ip];
		nm[server] = '\0';
		return true;
	}
}

KIp* KIpAnalysis::allocIp(const char* ip)
{
	KIp* p = KIp::Alloc();
	p->m_name = ip;
	return p;
}

void KIpAnalysis::freeIp(KIp* p)
{
	p->reset();
	KIp::Free(p);
}
