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
