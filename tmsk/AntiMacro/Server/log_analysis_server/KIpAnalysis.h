#pragma once
#include "KGameAccount.h"
#include "redis_scan.h"

class KServerOfIp
	: public JG_C::SelfListNode<KServerOfIp>
	, public JG_M::KPortableStepPool<KServerOfIp,1024>
{
public:
	typedef JG_C::KSDSelfList<InnerPlayer> player_list;
	
public:
	const char* m_name;
	const char* m_ipName;
	player_list m_playerList;

public:
	KServerOfIp();
	~KServerOfIp();

public:
	void append(const char* acctPlayer, int lastActive);
	void reset();

public:
	const char* name() const;
	const char* ip() const { return m_ipName; }
	bool foreach(l_func func, l_stackval ctx);

public:
	BeginDefLuaClassNoneGc(KServerOfIp);
		DefMemberFunc(name);
		DefMemberFunc(ip);
		DefMemberFunc(foreach);
	EndDef;
};

class KIp : public JG_M::KPortableStepPool<KIp,1024>
{
public:
	typedef JG_C::KSelfList<KServerOfIp> ServerList;
	typedef JG_C::KMapByTree_staticPooled< ccstr,KServerOfIp*,1024,KccstrCmp> ServerMap;

public:
	const char* m_name;
	ServerList m_serverList;
	ServerMap m_serverMap;

public:
	KIp();
	~KIp();

public:
	KServerOfIp* addServer(const char* serverName);
	const char* name() const { return m_name; }
	int getServerCount();
	int getAcctCount();
	bool foreachServer(l_func func, l_stackval ctx);

public:
	void freeServer(KServerOfIp* p);
	void reset();

public:
	KServerOfIp* getServerObject(const char* serverName);
	bool foreachPlayer(l_func func, l_stackval ctx);

public:
	BeginDefLuaClassNoneGc(KIp);
		DefMemberFunc(name);
		DefMemberFunc(getServerCount);
		DefMemberFunc(getAcctCount);
		DefMemberFunc(foreachServer);
		DefMemberFunc(getServerObject);
		DefMemberFunc(foreachPlayer);
	EndDef;
};

class KIpAnalysis
{
public:
	typedef JG_C::KMapByTree_staticPooled<ccstr,char> NameMap;
	typedef JG_C::KHashTable< ccstr,NameMap,KccstrCmp,KStringHasher<ccstr> > IpNameTable;
	typedef JG_C::KHashTable< ccstr,KIp*,KccstrCmp,KStringHasher<ccstr> > IpObjectTable;

public:
	KLargeNameList m_ipNameList;
	KLargeNameList m_serverNameList;
	NameMap m_serverNameMap;
	NameMap m_ipNameMap;
	IpNameTable m_ipNameTable;
	IpObjectTable m_ipObjTable;
	
public:
	KIpAnalysis();
	~KIpAnalysis();

public:
	void reset();

public:
	bool foreachIpName(l_func func, l_stackval ctx);
	bool foreachIpObject(l_func func, l_stackval ctx);
	bool foreachIpServer(l_func func, l_stackval ctx);

public:
	KIp* getIpObject(const char* ip);

public:
	const char* _addIpName(const char* ip);
	const char* _addServerName(const char* server);
	bool _addIpServer(const char* ip, const char* server);
	
private:
	KIp* allocIp(const char* ip);
	void freeIp(KIp* p);

public:
	BeginDefLuaClassNoneGc(KIpAnalysis);
		DefMemberFunc(reset);
		DefMemberFunc(foreachIpName);
		DefMemberFunc(foreachIpObject);
		DefMemberFunc(getIpObject);
	EndDef;
};
