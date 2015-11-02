#pragma once
#include "KGameAccount.h"
#include "redis_scan.h"

class KAnalysis
{
public: // analysis about
	typedef JG_C::KSelfList<KAccount> AcctList;
	typedef JG_C::KHashTable< ccstr,KAccount*,KccstriCmp,KStringHasher<ccstr> > AcctMap;

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
	bool scanAcctList(int hours);
	bool scanPatternAcctList(int hours, const char* acct_pattern);
	bool loadAcct(const char* name, int hours);
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
		DefMemberFunc(scanAcctList);
		DefMemberFunc(scanPatternAcctList);
		DefMemberFunc(loadAcct);
		DefMemberFunc(getAccount);
		DefMemberFunc(getAcctObject);
		DefMemberFunc(removeAcct);
		DefMemberFunc(allNameList);
		DefMemberFunc(activeNameList);
		DefMemberFunc(deactiveNameList);
		DefMemberFunc(foreachAcct);
	EndDef;
};
