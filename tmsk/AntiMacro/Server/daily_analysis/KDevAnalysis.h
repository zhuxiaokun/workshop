#pragma once
#include "KGameAccount.h"
#include "redis_scan.h"

class KServerOfDev
	: public JG_C::SelfListNode<KServerOfDev>
	, public JG_M::KPortableStepPool<KServerOfDev,1024>
{
public:
	typedef JG_C::KSDSelfList<InnerPlayer> player_list;
	
public:
	const char* m_name;
	const char* m_devName;
	player_list m_playerList;

public:
	KServerOfDev();
	~KServerOfDev();

public:
	void append(const char* acctPlayer, int lastActive);
	void reset();

public:
	const char* name() const;
	const char* dev() const { return m_devName; }
	bool foreach(l_func func, l_stackval ctx);

public:
	BeginDefLuaClassNoneGc(KServerOfDev);
		DefMemberFunc(name);
		DefMemberFunc(dev);
		DefMemberFunc(foreach);
	EndDef;
};

class KDev : public JG_M::KPortableStepPool<KDev,1024>
{
public:
	typedef JG_C::KSelfList<KServerOfDev> ServerList;
	typedef JG_C::KMapByTree_staticPooled< ccstr,KServerOfDev*,1024,KccstrCmp> ServerMap;

public:
	const char* m_name;
	ServerList m_serverList;
	ServerMap m_serverMap;

public:
	KDev();
	~KDev();

public:
	KServerOfDev* addServer(const char* serverName);
	const char* name() const { return m_name; }
	int getServerCount();
	int getAcctCount();
	bool foreachServer(l_func func, l_stackval ctx);

public:
	bool scanServerList(KRedis& redis, KLargeNameList& serverNameList);
	bool load(KRedis& redis, KLargeNameList& serverNameList);

public:
	void freeServer(KServerOfDev* p);
	void reset();

public:
	KServerOfDev* getServerObject(const char* serverName);
	bool foreachPlayer(l_func func,l_stackval ctx);

public:
	BeginDefLuaClassNoneGc(KDev);
		DefMemberFunc(name);
		DefMemberFunc(getServerCount);
		DefMemberFunc(getAcctCount);
		DefMemberFunc(foreachServer);
		DefMemberFunc(getServerObject);
		DefMemberFunc(foreachPlayer);
	EndDef;
};

class KDevAnalysis
{
public:
	typedef JG_C::KMapByTree_staticPooled<ccstr,char> NameMap;
	typedef JG_C::KHashTable< ccstr,NameMap,KccstrCmp,KStringHasher<ccstr> > DevNameTable;
	typedef JG_C::KHashTable< ccstr,KDev*,KccstrCmp,KStringHasher<ccstr> > DevObjectTable;

public:
	KLargeNameList m_devNameList;
	KLargeNameList m_serverNameList;
	NameMap m_serverNameMap;
	NameMap m_devNameMap;
	DevNameTable m_devNameTable;
	DevObjectTable m_devObjTable;
	
public:
	KDevAnalysis();
	~KDevAnalysis();

public:
	void reset();

public:
	bool scanDevList();
	bool scanPatternDevList(const char* dev_pattern);
	KDev* loadDev(const char* dev);
	bool foreachDevName(l_func func, l_stackval ctx);
	bool foreachDevObject(l_func func, l_stackval ctx);
	bool foreachDevServer(l_func func, l_stackval ctx);

public:
	KDev* getDevObject(const char* dev);

public:
	const char* _addDevName(const char* dev);
	const char* _addServerName(const char* server);
	bool _addDevServer(const char* dev, const char* server);
	
private:
	KDev* allocDev(const char* dev);
	void freeDev(KDev* p);

public:
	BeginDefLuaClassNoneGc(KDevAnalysis);
		DefMemberFunc(reset);
		DefMemberFunc(scanDevList);
		DefMemberFunc(scanPatternDevList);
		DefMemberFunc(loadDev);
		DefMemberFunc(foreachDevName);
		DefMemberFunc(foreachDevObject);
		DefMemberFunc(getDevObject);
	EndDef;
};
