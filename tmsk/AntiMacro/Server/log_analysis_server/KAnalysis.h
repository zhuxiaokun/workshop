#pragma once
#include "KGameAccount.h"
#include "redis_scan.h"

class KAnalysis
{
public: // analysis about
	typedef JG_C::KSelfList<KAccount> AcctList;
	typedef JG_C::KHashTable< ccstr,KAccount*,KccstriCmp,KStringHasher<ccstr> ,JG_S::KNoneLock,10240,10240*256> AcctMap;

public:
	KLargeNameList m_acctNameList;
	KLargeNameList m_activeNameList;
	KLargeNameList m_deactiveNameList;
	AcctList m_activeAcctList;
	AcctMap m_activeAcctMap;

public:
	KAnalysis();
	~KAnalysis();

public:
	void reset();

public:
	KAccount* allocAcct();
	void freeAcct(KAccount* acct);

public:
	bool loadData(const char* dirName, const char* platformName, const char* fileName, const char* ftime, const char* ttime);

private:
	KAccount* newAccount(const char* acctName);

public:
	bool on_tradeLine(time_t time, KPropertyMap& lmap);

public:
	KAccount* getAccount(const char* acctName);
	KAccount* getAcctObject(const char* acctName) { return this->getAccount(acctName); }
	bool removeAcct(const char* name, int hours);

public:
	KLargeNameList* allNameList() { return &m_acctNameList; }
	KLargeNameList* activeNameList() { return &m_activeNameList; }
	KLargeNameList* deactiveNameList() { return &m_deactiveNameList; }

public:
	bool foreachAcct(lua_func func, l_stackval ctx);

public:
	BeginDefLuaClassNoneGc(KAnalysis);
		DefMemberFunc(reset);
		DefMemberFunc(loadData);
		DefMemberFunc(getAccount);
		DefMemberFunc(getAcctObject);
		DefMemberFunc(removeAcct);
		DefMemberFunc(allNameList);
		DefMemberFunc(activeNameList);
		DefMemberFunc(deactiveNameList);
		DefMemberFunc(foreachAcct);
	EndDef;
};
