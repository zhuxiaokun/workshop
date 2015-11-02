#include "KDevAnalysis.h"
#include "Application.h"

//// KServerOfDev

KServerOfDev::KServerOfDev():m_name(NULL),m_devName(NULL)
{

}

KServerOfDev::~KServerOfDev()
{
	this->reset();
}

void KServerOfDev::append(const char* acctPlayer, int lastActive)
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

void KServerOfDev::reset()
{
	JG_C::SelfListNode<KServerOfDev>::reset();
	m_name = NULL;
	m_devName = NULL;
	while(!m_playerList.empty())
	{
		InnerPlayer* p = m_playerList.pop_front();
		InnerPlayer::Free(p);
	}
}

const char* KServerOfDev::name() const
{
	return m_name;
}

bool KServerOfDev::foreach(LMREG::l_func func, l_stackval ctx)
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

//// KDev

//const char* m_ip;
//ServerList m_serverList;

KDev::KDev():m_name(NULL)
{

}

KDev::~KDev()
{
	this->reset();
}

KServerOfDev* KDev::addServer(const char* serverName)
{
	ServerMap::iterator it = m_serverMap.find(serverName);
	if(it != m_serverMap.end())
		return it->second;

	KServerOfDev* pServer = KServerOfDev::Alloc();
	pServer->m_name = g_stringPool.get(serverName);
	pServer->m_devName = m_name;
	m_serverList.push_back(pServer);
	return pServer;
}

int KDev::getServerCount()
{
	return m_serverList.size();
}

int KDev::getAcctCount()
{
	int sum = 0;
	KServerOfDev* pServer = m_serverList.begin();
	while(pServer)
	{
		sum += pServer->m_playerList.size();
		pServer = m_serverList.next(pServer);
	}
	return sum;
}

bool KDev::foreachServer(l_func func, l_stackval ctx)
{
	KServerOfDev* pServer = m_serverList.begin();
	while(pServer)
	{
		if(!func.r_invoke<bool,KServerOfDev*,l_stackval>(pServer,ctx))
			return false;
		pServer = m_serverList.next(pServer);
	}
	return true;
}

void KDev::freeServer(KServerOfDev* p)
{
	p->reset();
	KServerOfDev::Free(p);
}

void KDev::reset()
{
	m_name = NULL;
	while(!m_serverList.empty())
	{
		KServerOfDev* p = m_serverList.pop_front();
		this->freeServer(p);
	}
	m_serverMap.clear();
}

KServerOfDev* KDev::getServerObject(const char* serverName)
{
	ServerMap::iterator it = m_serverMap.find(serverName);
	if(it == m_serverMap.end())
		return NULL;
	return it->second;
}

bool KDev::foreachPlayer(l_func func,l_stackval ctx)
{
	KServerOfDev* pServer = m_serverList.begin();
	while(pServer)
	{
		if(!pServer->foreach(func, ctx))
			return false;
		pServer = m_serverList.next(pServer);
	}
	return true;
}

//// KDevAnalysis

KDevAnalysis::KDevAnalysis()
{

}

KDevAnalysis::~KDevAnalysis()
{

}

void KDevAnalysis::reset()
{
	m_devNameList.reset();
	m_serverNameList.reset();
	m_serverNameMap.clear();
	m_devNameMap.clear();
	m_devNameTable.clear();
	while(!m_devObjTable.empty())
	{
		DevObjectTable::iterator it = m_devObjTable.begin();
		KDev* p = it->second;
		this->freeDev(p);
		m_devObjTable.erase(it);
	}
}

bool KDevAnalysis::foreachDevName(LMREG::l_func func, l_stackval ctx)
{
	DevNameTable::iterator it = m_devNameTable.begin();
	for(; it != m_devNameTable.end(); it++)
	{
		const char* ip = it->first;
		if(!func.r_invoke<bool,const char*,l_stackval>(ip, ctx))
			return false;
	}
	return true;
}

bool KDevAnalysis::foreachDevObject(LMREG::l_func func, l_stackval ctx)
{
	DevObjectTable::iterator it = m_devObjTable.begin();
	for(; it != m_devObjTable.end(); it++)
	{
		KDev* p = it->second;
		if(!func.r_invoke<bool,KDev*,l_stackval>(p, ctx))
			return false;
	}
	return true;
}

bool KDevAnalysis::foreachDevServer(l_func func, l_stackval ctx)
{
	DevNameTable::iterator it = m_devNameTable.begin();
	for(; it != m_devNameTable.end(); it++)
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

KDev* KDevAnalysis::getDevObject(const char* dev)
{
	DevObjectTable::iterator it = m_devObjTable.find(dev);
	if(it == m_devObjTable.end())
		return NULL;
	return it->second;
}

const char* KDevAnalysis::_addDevName(const char* dev)
{
	NameMap::iterator it = m_devNameMap.find(dev);
	if(it != m_devNameMap.end())
		return it->first;
	dev = m_devNameList.append(dev);
	m_devNameMap[dev] = '\0';
	return dev;
}

const char* KDevAnalysis::_addServerName(const char* server)
{
	NameMap::iterator it = m_serverNameMap.find(server);
	if(it != m_serverNameMap.find(server))
		return it->first;
	server = m_serverNameList.append(server);
	m_serverNameMap[server] = '\0';
	return server;
}

bool KDevAnalysis::_addDevServer(const char* dev, const char* server)
{
	dev = this->_addDevName(dev);
	server = this->_addServerName(server);

	DevNameTable::iterator it = m_devNameTable.find(dev);
	if(it != m_devNameTable.end())
	{
		NameMap& nm = it->second;
		nm[server] = '\0';
		return true;
	}
	else
	{
		NameMap& nm = m_devNameTable[dev];
		nm[server] = '\0';
		return true;
	}
}

KDev* KDevAnalysis::allocDev(const char* dev)
{
	KDev* p = KDev::Alloc();
	p->m_name = dev;
	return p;
}

void KDevAnalysis::freeDev(KDev* p)
{
	p->reset();
	KDev::Free(p);
}
