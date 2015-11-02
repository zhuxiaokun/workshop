#include "KGameAccount.h"
#include "Application.h"
#include <System/Misc/StrUtil.h>
#include <CommSession/KPropertyMap.h>

// level in analysis log to game level
static int toGameLevel(const char* s)
{
	int n = str2int(s);
	if(n > 65535)
		return (n >> 16)*100 + (n & 0xffff);
	else
		return n;
}

////
LuaString KTrade::ilist()
{
	char buf[1024];
	int pos = 0;
	ItemMap::iterator it = m_ilist.begin();
	for(; it!=m_ilist.end(); it++)
	{
		int t = it->first;
		int c = it->second;
		int n = sprintf_k(&buf[pos], sizeof(buf)-pos, "%s%d:%d", pos?",":"", t, c); pos += n;
		int avail = sizeof(buf) - pos;
		if(avail < 20) break;
	}
	buf[pos] = '\0';
	return LuaString(buf, pos);
}

LuaString KTrade::olist()
{
	char buf[1024];
	int pos = 0;
	ItemMap::iterator it = m_olist.begin();
	for(; it!=m_olist.end(); it++)
	{
		int t = it->first;
		int c = it->second;
		int n = sprintf_k(&buf[pos], sizeof(buf)-pos, "%s%d:%d", pos?",":"", t, c); pos += n;
		int avail = sizeof(buf) - pos;
		if(avail < 20) break;
	}
	buf[pos] = '\0';
	return LuaString(buf, pos);
}

bool KTrade::foreachInItem(l_func func, l_stackval ctx)
{
	ItemMap::iterator it = m_ilist.begin();
	for(; it!=m_ilist.end(); it++)
	{
		int t = it->first;
		int c = it->second;
		if(!func.r_invoke<bool,int,int,l_stackval>(t, c, ctx))
			return false;
	}
	return true;
}

bool KTrade::foreachOutItem(l_func func, l_stackval ctx)
{
	ItemMap::iterator it = m_olist.begin();
	for(; it!=m_olist.end(); it++)
	{
		int t = it->first;
		int c = it->second;
		if(!func.r_invoke<bool,int,int,l_stackval>(t, c, ctx))
			return false;
	}
	return true;
}

int KTrade::has_i_l(lua_State* L)
{
	if(!lua_isuserdata(L, 1) || lua_islightuserdata(L, 1))
	{
		luaL_argerror(L, 1, "KTrade object expected.");
		return 0;
	}

	int n = lua_gettop(L);
	KTrade* obj = LMREG::read<KTrade*>(L, 1);
	ItemMap::iterator it = obj->m_ilist.end();
	for(int i=2; i<=n; i++)
	{
		int typeID = lua_tointeger(L, i);
		if(obj->m_ilist.find(typeID) != it)
			return true;
	}
	return false;
}

int KTrade::has_o_l(lua_State* L)
{
	if(!lua_isuserdata(L, 1) || lua_islightuserdata(L, 1))
	{
		luaL_argerror(L, 1, "KTrade object expected.");
		return 0;
	}
	int n = lua_gettop(L);
	KTrade* obj = LMREG::read<KTrade*>(L, 1);
	ItemMap::iterator it = obj->m_olist.end();
	for(int i=2; i<=n; i++)
	{
		int typeID = lua_tointeger(L, i);
		if(obj->m_olist.find(typeID) != it)
			return true;
	}
	return false;
}

void KTrade::regLuaMethods(lua_State* L)
{
	LMREG::push_meta(L, LMREG::class_name<KTrade>::name());
	if(lua_istable(L, -1))
	{
		// has_i
		lua_pushstring(L, "has_i");
		lua_pushcclosure(L, KTrade::has_i_l, 0);
		lua_rawset(L, -3);
		// has_o
		lua_pushstring(L, "has_o");
		lua_pushcclosure(L, KTrade::has_o_l, 0);
		lua_rawset(L, -3);
	}
	lua_pop(L, 1);
}

//// 

KServer::KServer():m_serverName(NULL)
{
}

KServer::~KServer()
{
	this->reset();
}

KPlayer* KServer::newPlayer()
{
	KPlayer* p = KPlayer::Alloc();
	m_playerArray.push_back(p);
	return p;
}

KLogin* KServer::newLogin()
{
	KLogin* p = KLogin::Alloc();
	m_loginArray.push_back(p);
	return p;
}

KTrade* KServer::newTrade()
{
	KTrade* p = KTrade::Alloc();
	m_tradeArray.push_back(p);
	return p;
}

void KServer::reset()
{
	m_serverName = NULL;
	while(!m_playerArray.empty())
		this->freePlayer(m_playerArray.pop_back());
	while(!m_loginArray.empty())
		this->freeLogin(m_loginArray.pop_back());
	while(!m_tradeArray.empty())
		this->freeTrade(m_tradeArray.pop_back());
}

bool KServer::foreachPlayer(LMREG::l_func func, l_stackval ctx)
{
	int n = m_playerArray.size();
	for(int i=0; i<n; i++)
	{
		KPlayer* player = m_playerArray.at(i);
		if(!func.r_invoke<bool,KPlayer*,l_stackval>(player,ctx))
			return false;
	}
	return true;
}

bool KServer::foreachLogin(LMREG::l_func func, l_stackval ctx)
{
	int n = m_loginArray.size();
	for(int i=0; i<n; i++)
	{
		KLogin* login = m_loginArray.at(i);
		if(!func.r_invoke<bool,KLogin*,l_stackval>(login,ctx))
			return false;
	}
	return true;
}

bool KServer::foreachTrade(LMREG::l_func func, l_stackval ctx)
{
	int n = m_tradeArray.size();
	for(int i=0; i<n; i++)
	{
		KTrade* trade = m_tradeArray.at(i);
		if(!func.r_invoke<bool,KTrade*,l_stackval>(trade,ctx))
			return false;
	}
	return true;
}


// KGameAccount

KAccount::KAccount()
	: m_startTime(0)
	, m_acctName(NULL)
	, m_lastActive(0)
{

}

KAccount::~KAccount()
{
	this->reset();
}

void KAccount::reset()
{
	m_startTime = 0;
	m_acctName = NULL;
	m_lastActive = 0;
	while(!m_serverMap.empty())
	{
		ServerMap::iterator it = m_serverMap.begin();
		this->freeServer(it->second);
		m_serverMap.erase(it);
	}
}

KTrade* KAccount::on_tradeLine(time_t time, KPropertyMap& lmap)
{
	const char* serverName = lmap.get("server");
	KServer* pServer = this->newServer(serverName);
	KTrade* pTrade = pServer->newTrade();

	int map = str2int(lmap.get("map"));
	const char* target = lmap.get("target");
	const char* srcPlayer = lmap.get("srcPlayer");
	const char* dstPlayer = lmap.get("dstPlayer");
	const char* sip = lmap.get("sip");
	const char* tip = lmap.get("tip");
	const char* in_items = lmap.get("in");
	const char* out_items = lmap.get("out");

	pTrade->m_serverName = pServer->name();
	pTrade->m_time = time;
	pTrade->m_source = m_acctName;
	pTrade->m_target = g_stringPool.get(target);
	pTrade->m_srcPlayer = g_stringPool.get(srcPlayer);
	pTrade->m_dstPlayer = g_stringPool.get(dstPlayer);
	if(sip) pTrade->m_sip = g_stringPool.get(sip);
	if(tip) pTrade->m_tip = g_stringPool.get(tip);
	pTrade->m_map = map;
	pTrade->m_sVipLevel = str2int(lmap.get("sVipLevel"));
	pTrade->m_tVipLevel = str2int(lmap.get("tVipLevel"));
	pTrade->m_friendDegree = str2int(lmap.get("friendDegree"));

	char* ss[128];
	char* ilist = _trim((char*)in_items);
	if(ilist[0])
	{
		int n = split(ilist, ":,", ss, sizeof(ss)/sizeof(char*));
		{
			for(int i=0; i<n; i+=2)
			{
				int itemType = str2int(ss[i]);
				int itemCount = str2int(ss[i+1]);
				pTrade->m_ilist[itemType] = itemCount;
			}
		}
	}
	char* olist = _trim((char*)out_items);
	if(olist[0])
	{
		int n = split(olist, ":,", ss, sizeof(ss)/sizeof(char*));
		{
			for(int i=0; i<n; i+=2)
			{
				int itemType = str2int(ss[i]);
				int itemCount = str2int(ss[i+1]);
				pTrade->m_olist[itemType] = itemCount;
			}
		}
	}

	if(strcmp(m_acctName, "ANZHI201510202014173k8G82uK54") == 0)
	{
		Log(LOG_DEBUG, ">>>> source=%s target=%s in=%s out=%s", pTrade->m_source, pTrade->m_target, pTrade->ilist().c_str(), pTrade->olist().c_str());
	}

	return pTrade;
}

KTrade* KAccount::on_passiveTradeLine(KTrade* theTrade)
{
	const char* serverName = theTrade->m_serverName;

	KServer* pServer = this->newServer(serverName);
	KTrade* pTrade = pServer->newTrade();

	pTrade->m_serverName = pServer->name();
	pTrade->m_time = theTrade->m_time;
	pTrade->m_source = m_acctName;
	pTrade->m_target = theTrade->m_source;
	pTrade->m_srcPlayer = theTrade->m_dstPlayer;
	pTrade->m_dstPlayer = theTrade->m_srcPlayer;
	pTrade->m_sip = theTrade->m_tip;
	pTrade->m_tip = theTrade->m_sip;
	pTrade->m_map = theTrade->m_map;
	pTrade->m_sVipLevel = theTrade->m_tVipLevel;
	pTrade->m_tVipLevel = theTrade->m_sVipLevel;
	pTrade->m_friendDegree = theTrade->m_friendDegree;
	pTrade->m_ilist = theTrade->m_olist;
	pTrade->m_olist = theTrade->m_ilist;

	if(strcmp(pTrade->m_target, "ANZHI201510202014173k8G82uK54") == 0)
	{
		Log(LOG_DEBUG, ">>>> source=%s target=%s in=%s out=%s", pTrade->m_source, pTrade->m_target, pTrade->ilist().c_str(), pTrade->olist().c_str());
	}

	return pTrade;
}

KServer* KAccount::newServer(const char* serverName)
{
	ServerMap::iterator it = m_serverMap.find(serverName);
	if(it != m_serverMap.end())
		return it->second;
	KServer* pServer = KServer::Alloc();
	const char* name = g_stringPool.get(serverName);
	pServer->m_serverName = name;
	m_serverMap[name] = pServer;
	return pServer;
}

bool KAccount::foreachServer(LMREG::l_func func, l_stackval ctx)
{
	ServerMap::iterator it = m_serverMap.begin();
	for(; it!=m_serverMap.end(); it++)
	{
		KServer* pserver = it->second;
		if(!func.r_invoke<bool,const char*,KServer*,l_stackval>(it->first, pserver, ctx))
			return false;
	}
	return true;
}

int KAccount::getTradeCount()
{
	int sum = 0;
	ServerMap::iterator it = m_serverMap.begin();
	for(; it!=m_serverMap.end(); it++)
	{
		KServer* pserver = it->second;
		sum += pserver->tradeCount();
	}
	return sum;
}

bool KAccount::foreachTrade(l_func func, l_stackval ctx)
{
	ServerMap::iterator it = m_serverMap.begin();
	for(; it!=m_serverMap.end(); it++)
	{
		KServer* pserver = it->second;
		if(!pserver->foreachTrade(func, ctx))
			return false;
	}
	return true;
}

int KAccount::countTrade(l_func func, l_stackval ctx)
{
	int n = 0;
	ServerMap::iterator it = m_serverMap.begin();
	for(; it!=m_serverMap.end(); it++)
	{
		KServer* pserver = it->second;
		int tradeCount = pserver->tradeCount();
		for(int i=0; i<tradeCount; i++)
		{
			KTrade* tradeObj = pserver->tradeAt(i);
			if(func.r_invoke<bool,KTrade*,l_stackval>(tradeObj, ctx))
				n++;
		}
	}
	return n;
}

KPlayer* KAccount::getPlayer(const char* serverName, const char* player)
{
	ServerMap::iterator it = m_serverMap.find(serverName);
	if(it == m_serverMap.end())
		return NULL;

	KServer* pServer = it->second;
	int n = pServer->playerCount();
	for(int i=0; i<n; i++)
	{
		KPlayer* p = pServer->playerAt(i);
		if(stricmp(p->name(), player) == 0)
			return p;
	}
	return NULL;
}
