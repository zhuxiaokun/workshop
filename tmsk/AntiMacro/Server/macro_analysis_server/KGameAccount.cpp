#include "KGameAccount.h"
#include "Application.h"
#include <System/Misc/StrUtil.h>
#include <CommSession/KPropertyMap.h>

// level in analysis log to game level
static int toGameLevel(const char* s)
{
	int n = str2int(s);
	return (n >> 16)*100 + (n & 0xffff);
}

// KActiveServerSet

KActiveServerSet::KActiveServerSet()
	: m_start(0)
{

}

KActiveServerSet::~KActiveServerSet()
{

}

bool KActiveServerSet::loadFromRedis(KRedis& redis, time_t start, const char* redisAcctKey)
{
	m_array.clear();
	m_start = start;

	KRedis& rs = redis;

	if(!rs.hscan(redisAcctKey, "lastActive.server.*", *this))
		return false;

	return true;
}

bool KActiveServerSet::accept(const char* field, const char* val)
{
	int pattern_len = strlen("lastActive.server.");
	time_t t = (time_t)str2i64(val);
	if(t >= m_start)
	{
		const char* server = g_stringPool.get(field + pattern_len);
		if(m_array.insert_unique(server) >= 0)
			Log(LOG_DEBUG, "debug: add active server '%s'", server);
	}
	return true;
}

int KActiveServerSet::size() const
{
	return m_array.size();
}

const char* KActiveServerSet::at(int idx) const
{
	return m_array.at(idx);
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
	m_activeServerSet.reset();
	while(!m_serverMap.empty())
	{
		ServerMap::iterator it = m_serverMap.begin();
		this->freeServer(it->second);
		m_serverMap.erase(it);
	}
}

class h_player_visitor : public hscan_visitor
{
public:
	char m_line[1024];
	KPropertyMap m_m;
	time_t m_startTime;
	KServer* m_pServer;
	const char* m_acctName;

	h_player_visitor(const char* acctName, KServer* pServer, time_t start)
		: m_acctName(acctName)
		, m_pServer(pServer)
		, m_startTime(start)
	{
	}
	bool accept(const char* field, const char* val)
	{
		m_m.clear();
		
		int len = strcpy_k(m_line, sizeof(m_line), val+1);
		m_line[len-1] = '\0';

		m_m.parseLine(m_line);

		const char* playerName = field;
		const char* viplevel = m_m.get("viplevel");
		int level = toGameLevel(m_m.get("level"));
		int lastActive = (int)str2i64(m_m.get("lastActive"));
		if(lastActive < m_startTime)
			return true;

		KPlayer* player = m_pServer->newPlayer();
		player->m_acctName = m_acctName;
		player->m_serverName = m_pServer->name();
		player->m_playerName = g_stringPool.get(playerName);
		player->m_level = level;
		player->m_viplevel = viplevel ? str2int(viplevel) : 0;
		player->m_lastActive = lastActive;
		
		return true;
	}
};

class l_login_visitor : public lscan_visitor
{
public:
	char m_line[2048];
	KPropertyMap m_map, m_mapPrev;
	KServer* m_pServer;
	const char* m_acctName;
	time_t m_startTime, m_endTime;

	l_login_visitor(const char* acctName, KServer* pServer, time_t start)
		: m_acctName(acctName)
		, m_pServer(pServer)
		, m_startTime(start)
	{
	}
	bool accept(const char* val)
	{
		m_map.clear();
		
		int len = strcpy_k(m_line, sizeof(m_line), val);

		m_map.parseLine(m_line);

		// login:  time,verb,server,account,player,level,map,ip
		// logout: time,verb,server,account,ip

		const char* verb = m_map.get("verb");

		if(stricmp(verb, "login") == 0)
		{
			m_mapPrev = m_map;
		}
		else if(stricmp(verb, "logout") == 0)
		{
			if(m_mapPrev.empty())
				return true;

			int logoutTime = str2int(m_map.get("time"));
			if(logoutTime < m_startTime)
				return true;

			int loginTime = str2int(m_mapPrev.get("time"));

			KLogin* pLogin = m_pServer->newLogin();

			int duration = logoutTime - loginTime;
			int level = toGameLevel(m_mapPrev.get("level"));
			int map = str2int(m_mapPrev.get("map"));
			const char* ip = m_mapPrev.get("ip");
			const char* viplevel = m_mapPrev.get("viplevel");

			pLogin->m_acctName = m_acctName;
			pLogin->m_serverName = m_pServer->name();
			pLogin->m_begin = loginTime;
			pLogin->m_duration = duration;
			pLogin->m_level = level;
			pLogin->m_viplevel = viplevel ? str2int(viplevel) : 0;
			pLogin->m_map = map;
			pLogin->m_ip = g_stringPool.get(ip);
			
			m_mapPrev.clear();
		}
		return true;
	}
	void afterDone()
	{
		if(m_mapPrev.empty())
			return;

		int loginTime = str2int(m_mapPrev.get("time"));

		KLogin* pLogin = m_pServer->newLogin();

		int duration = (int)time(NULL) - loginTime;
		int level = toGameLevel(m_mapPrev.get("level"));
		int map = str2int(m_mapPrev.get("map"));
		const char* ip = m_mapPrev.get("ip");

		pLogin->m_begin = loginTime;
		pLogin->m_duration = duration;
		pLogin->m_level = level;
		pLogin->m_map = map;
		pLogin->m_ip = g_stringPool.get(ip);

		m_mapPrev.clear();
	}
};

class l_trade_visitor : public lscan_visitor
{
public:
	char m_line[2048];
	KPropertyMap m_map;
	const char* m_acctName;
	KServer* m_pServer;
	time_t m_startTime;

	l_trade_visitor(const char* acctName,KServer* pServer, time_t start)
		: m_acctName(acctName)
		, m_pServer(pServer)
		, m_startTime(start)
	{
	}
	bool accept(const char* val)
	{
		m_map.clear();
		
		int len = strcpy_k(m_line, sizeof(m_line), val);
		m_map.parseLine(m_line);

		// trade: time,map,target,in,out

		int time = str2int(m_map.get("time"));
		if(time < m_startTime)
			return true;

		KTrade* pTrade = m_pServer->newTrade();

		int map = str2int(m_map.get("map"));
		const char* target = m_map.get("target");
		const char* srcPlayer = m_map.get("srcPlayer");
		const char* dstPlayer = m_map.get("dstPlayer");
		const char* sip = m_map.get("sip");
		const char* tip = m_map.get("tip");
		const char* in_items = m_map.get("in");
		const char* out_items = m_map.get("out");

		pTrade->m_serverName = m_pServer->name();
		pTrade->m_time = time;
		pTrade->m_source = m_acctName;
		pTrade->m_target = g_stringPool.get(target);
		pTrade->m_srcPlayer = g_stringPool.get(srcPlayer);
		pTrade->m_dstPlayer = g_stringPool.get(dstPlayer);
		if(sip) pTrade->m_sip = g_stringPool.get(sip);
		if(tip) pTrade->m_tip = g_stringPool.get(tip);
		pTrade->m_map = map;
		pTrade->m_sVipLevel = str2int(m_map.get("sVipLevel"));
		pTrade->m_tVipLevel = str2int(m_map.get("tVipLevel"));
		pTrade->m_friendDegree = str2int(m_map.get("friendDegree"));

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

		return true;
	}
};

bool KAccount::load(KRedis& redis, int hours, const char* acctName)
{
	KDatetime dtStart;
	dtStart.AddSecond(-hours*KDatetime::hour_seconds);

	m_startTime = (time_t)dtStart.Time();
	m_acctName = g_stringPool.get(acctName);

	char acctKey[128];
	sprintf_k(acctKey, sizeof(acctKey), "acct.%s", acctName);

	char lastActive[32];
	if(!redis.hget(acctKey, "lastActive", lastActive, sizeof(lastActive)))
		return false;

	m_lastActive = (time_t)str2int(lastActive);

	if(!m_activeServerSet.loadFromRedis(redis, m_startTime, acctKey))
		return false;

	int n = m_activeServerSet.size();
	for(int i=0; i<n; i++)
	{
		const char* serverName = m_activeServerSet.at(i);
		KServer* pServer = this->newServer(serverName);

		// player.server.acct.<acct>.<server>
		{
			char playerKey[128];
			sprintf_k(playerKey, sizeof(playerKey), "player.server.acct.%s.%s", acctName, serverName);

			h_player_visitor v(m_acctName, pServer, m_startTime);
			redis.hscan(playerKey, "*", v);
		}
		// login.server.acct.<acct>.<server>
		{
			char loginKey[128];
			sprintf_k(loginKey, sizeof(loginKey), "login.server.acct.%s.%s", acctName, serverName);

			l_login_visitor v(m_acctName, pServer, m_startTime);
			redis.lrange(loginKey, 0, -1, v);
			v.afterDone();
		}
		// trade.server.acct.<acct>.<server>
		{
			char tradeKey[128];
			sprintf_k(tradeKey, sizeof(tradeKey), "trade.server.acct.%s.%s", acctName, serverName);

			l_trade_visitor v(m_acctName, pServer, m_startTime);
			redis.lrange(tradeKey, 0, -1, v);
		}
	}

	return true;
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
