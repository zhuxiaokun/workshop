#pragma once
#include <System/KType.h>
#include <lua/KLuaTableHolder.h>
#include <redis/hiredis.h>
#include <System/Collections/KTree.h>
#include <System/Collections/DynArray.h>
#include "redis_scan.h"
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KList.h>
#include <CommSession/KPropertyMap.h>

struct InnerPlayer : public JG_M::KPortableMemoryPool<InnerPlayer,1024>
{
public:
	const char* m_serverName;
	const char* m_acctName;
	const char* m_player;
	int m_lastActive;
	InnerPlayer* m_pNextNode;

public:
	InnerPlayer()
		: m_serverName(NULL)
		, m_acctName(NULL)
		, m_player(NULL)
		, m_lastActive(0)
		, m_pNextNode(NULL)
	{
	}

public:
	const char* serverName() const { return m_serverName; }
	const char* acctName() const { return m_acctName; }
	const char* name() const { return m_player; }
	int lastActive() const { return m_lastActive; }

public:
	BeginDefLuaClassNoneGc(InnerPlayer);
		DefMemberFunc(serverName);
		DefMemberFunc(acctName);
		DefMemberFunc(name);
		DefMemberFunc(lastActive);
	EndDef;
};

class KPlayer
	: public JG_M::KPortableStepPool<KPlayer,1024>
{
public:
	const char* m_acctName;
	const char* m_serverName;
	const char* m_playerName;
	int m_level;
	int m_viplevel;
	time_t m_lastActive;

public:
	KPlayer()
		: m_acctName(NULL)
		, m_serverName(NULL)
		, m_playerName(NULL)
		, m_level(0)
		, m_viplevel(0)
		, m_lastActive(0)
	{
	}
	void reset()
	{
		m_acctName = NULL;
		m_serverName = NULL;
		m_playerName = NULL;
		m_level = 0;
		m_viplevel = 0;
		m_lastActive = 0;
	}

public:
	const char* acctName() const { return m_acctName; }
	const char* serverName() const { return m_serverName; }
	const char* name() const { return m_playerName; }
	int level() const { return m_level; }
	int viplevel() const { return m_viplevel; }
	int lastActive() const { return (int)m_lastActive; }

public:
	BeginDefLuaClassNoneGc(KPlayer);
		DefMemberFunc(acctName);
		DefMemberFunc(serverName);
		DefMemberFunc(name);
		DefMemberFunc(level);
		DefMemberFunc(viplevel);
		DefMemberFunc(lastActive);
	EndDef;
};
class KLogin
	: public JG_M::KPortableStepPool<KLogin,1024>
{
public:
	const char* m_serverName;
	const char* m_acctName;
	const char* m_player;
	time_t m_begin;
	int m_duration;
	int m_level;
	int m_viplevel;
	int m_map;
	const char* m_ip;

public:
	KLogin()
		: m_acctName(NULL)
		, m_serverName(NULL)
		, m_player(NULL)
		, m_begin(0)
		, m_duration(0)
		, m_level(0)
		, m_viplevel(0)
		, m_map(0)
		, m_ip(NULL)
	{
	}
	void reset()
	{
		m_acctName = NULL;
		m_serverName = NULL;
		m_player = NULL;
		m_begin = 0;
		m_duration = 0;
		m_level = 0;
		m_viplevel = 0;
		m_map = 0;
		m_ip = NULL;
	}

public:
	const char* acctName() const { return m_acctName; }
	const char* serverName() const { return m_serverName; }
	const char* player() const { return m_player; }
	int begin() const { return (int)m_begin; }
	int duration() const { return m_duration; }
	int level() const { return m_level; }
	int viplevel() const { return m_viplevel; }
	int map() const { return m_map; }
	const char* ip() const { return m_ip; }

public:
	BeginDefLuaClassNoneGc(KLogin);
		DefMemberFunc(acctName);
		DefMemberFunc(serverName);
		DefMemberFunc(player);
		DefMemberFunc(begin);
		DefMemberFunc(duration);
		DefMemberFunc(level);
		DefMemberFunc(viplevel);
		DefMemberFunc(map);
		DefMemberFunc(ip);
	EndDef;
};
class KTrade
	: public JG_M::KPortableStepPool<KTrade,1024>
{
public:
	typedef JG_C::KMapByTree_staticPooled<int,int> ItemMap;

public:
	const char* m_serverName;
	time_t m_time;
	const char* m_source;
	const char* m_target;
	const char* m_srcPlayer;
	const char* m_dstPlayer;
	int m_map;
	ItemMap m_ilist;
	ItemMap m_olist;
	int m_sVipLevel;
	int m_tVipLevel;
	const char* m_sip;
	const char* m_tip;
	int m_friendDegree;

public:
	KTrade()
		: m_serverName(NULL)
		, m_time(0)
		, m_source(NULL)
		, m_target(NULL)
		, m_srcPlayer(NULL)
		, m_dstPlayer(NULL)
		, m_map(0)
		, m_sVipLevel(0)
		, m_tVipLevel(0)
		, m_sip(NULL)
		, m_tip(NULL)
		, m_friendDegree(0)
	{
	}
	void reset()
	{
		m_serverName = NULL;
		m_time = 0;
		m_source = m_target = NULL;
		m_srcPlayer = m_dstPlayer = NULL;
		m_sip = m_tip = NULL;
		m_map = 0;
		m_ilist.clear();
		m_olist.clear();
		m_sVipLevel = m_tVipLevel = 0;
		m_friendDegree = 0;
	}

public:
	const char* serverName() const { return m_serverName; }
	int time() const { return (int)m_time; }
	const char* source() const { return m_source; }
	const char* target() const { return m_target; }
	const char* srcPlayer() const { return m_srcPlayer; }
	const char* dstPlayer() const { return m_dstPlayer; }
	const char* sip() const { return m_sip; }
	const char* tip() const { return m_tip; }
	int map() const { return m_map; }
	LuaString ilist();
	LuaString olist();
	int sVipLevel() const { return m_sVipLevel; }
	int tVipLevel() const { return m_tVipLevel; }
	int friendDegree() const { return m_friendDegree; }

	bool foreachInItem(l_func func, l_stackval ctx);
	bool foreachOutItem(l_func func, l_stackval ctx);

	bool empty_i() { return m_ilist.empty(); }
	bool empty_o() { return m_olist.empty(); }

private:
	static int has_i_l(lua_State* L);
	static int has_o_l(lua_State* L);
	static void regLuaMethods(lua_State* L);

public:
	BeginDefLuaClassNoneGc(KTrade);
		DefMemberFunc(serverName);
		DefMemberFunc(time);
		DefMemberFunc(source);
		DefMemberFunc(target);
		DefMemberFunc(srcPlayer);
		DefMemberFunc(dstPlayer);
		DefMemberFunc(map);
		DefMemberFunc(ilist);
		DefMemberFunc(olist);
		DefMemberFunc(sip);
		DefMemberFunc(tip);
		DefMemberFunc(sVipLevel);
		DefMemberFunc(tVipLevel);
		DefMemberFunc(friendDegree);
		DefMemberFunc(foreachInItem);
		DefMemberFunc(foreachOutItem);
		DefMemberFunc(empty_i);
		DefMemberFunc(empty_o);
		KTrade::regLuaMethods(L);
	EndDef;
};
class KServer
	: public JG_M::KPortableStepPool<KServer,1024>
{
public:
	typedef JG_C::DynArray<KPlayer*,256,256> PlayerArray;
	typedef JG_C::DynArray<KLogin*,256,256> LoginArray;
	typedef JG_C::DynArray<KTrade*,256,256> TradeArray;

public:
	const char* m_serverName;
	PlayerArray m_playerArray;
	LoginArray m_loginArray;
	TradeArray m_tradeArray;

public:
	KServer();
	~KServer();

public:
	KPlayer* newPlayer();
	KLogin* newLogin();
	KTrade* newTrade();
	void freePlayer(KPlayer* p) { p->reset(); KPlayer::Free(p); }
	void freeLogin(KLogin* p)   { p->reset(); KLogin::Free(p);  }
	void freeTrade(KTrade* p)   { p->reset(); KTrade::Free(p);  }
	void reset();

public:
	const char* name() const { return m_serverName; }
	int playerCount() const { return m_playerArray.size(); }
	int loginCount() const { return m_loginArray.size(); }
	int tradeCount() const { return m_tradeArray.size(); }

	KPlayer* playerAt(int i) { return m_playerArray.at(i); }
	KLogin* loginAt(int i) { return m_loginArray.at(i); }
	KTrade* tradeAt(int i) { return m_tradeArray.at(i); }

	bool foreachPlayer(l_func func, l_stackval ctx);
	bool foreachLogin(l_func func, l_stackval ctx);
	bool foreachTrade(l_func func, l_stackval ctx);

public:
	BeginDefLuaClassNoneGc(KServer);
		DefMemberFunc(name);
		DefMemberFunc(playerCount);
		DefMemberFunc(loginCount);
		DefMemberFunc(tradeCount);
		DefMemberFunc(playerAt);
		DefMemberFunc(loginAt);
		DefMemberFunc(tradeAt);
		DefMemberFunc(foreachPlayer);
		DefMemberFunc(foreachLogin);
		DefMemberFunc(foreachTrade);
	EndDef;
};

class KAccount
	: public JG_C::SelfListNode<KAccount>
	, public JG_M::KPortableMemoryPool<KAccount,1024>
{
public:
	typedef JG_C::SelfListNode<KAccount> list_node_type;
	typedef JG_C::KMapByTree_staticPooled<ccstr,KServer*,1024,KccstrCmp> ServerMap;

public:
	time_t m_startTime;

public:
	const char* m_acctName;
	time_t m_lastActive;
	ServerMap m_serverMap;

public:
	KAccount();
	~KAccount();

public:
	void reset();
	bool isReady() const { return m_acctName != NULL; }

public:
	KTrade* on_tradeLine(time_t time, KPropertyMap& lmap);
	KTrade* on_passiveTradeLine(KTrade* theTrade);

private:
	KServer* newServer(const char* serverName);
	void freeServer(KServer* p) { p->reset(); KServer::Free(p); }

public:
	const char* name() const { return m_acctName; }
	int lastActive() const { return (int)m_lastActive; }
	bool foreachServer(l_func func, l_stackval ctx);
	int serverCount() const { return m_serverMap.size(); }

public:
	int getTradeCount();
	bool foreachTrade(l_func func, l_stackval ctx);
	int countTrade(l_func func, l_stackval ctx);
	KPlayer* getPlayer(const char* serverName, const char* player);

public:
	BeginDefLuaClassNoneGc(KAccount);
		DefMemberFunc(isReady);
		DefMemberFunc(name);
		DefMemberFunc(lastActive);
		DefMemberFunc(foreachServer);
		DefMemberFunc(serverCount);
		DefMemberFunc(getTradeCount);
		DefMemberFunc(foreachTrade);
		DefMemberFunc(countTrade);
		DefMemberFunc(getPlayer);
	EndDef;
};
