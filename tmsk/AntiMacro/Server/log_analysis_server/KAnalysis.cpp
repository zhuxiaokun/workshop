#include "KAnalysis.h"
#include "Application.h"
#include "redis_scan.h"
#include "AnalysisContext.h"

//// KAnalysis

KAnalysis::KAnalysis()
{

}

KAnalysis::~KAnalysis()
{
	this->reset();
}

void KAnalysis::reset()
{
	m_acctNameList.reset();
	m_activeNameList.reset();
	m_deactiveNameList.reset();
	while(!m_activeAcctList.empty())
	{
		KAccount* acct = m_activeAcctList.pop_front();
		this->freeAcct(acct);
	}
	m_activeAcctMap.clear();
}

KAccount* KAnalysis::allocAcct()
{
	return KAccount::Alloc();
}

void KAnalysis::freeAcct(KAccount* acct)
{
	acct->reset();
	KAccount::Free(acct);
}

bool KAnalysis::loadData(const char* dirName, const char* platformName, const char* fileName, const char* ftime, const char* ttime)
{
	this->reset();

	KDatetime ft;
	if(!ft.SetDatetimeString(ftime))
	{
		Log(LOG_CONSOLE|LOG_ERROR, "error: invalid date time string %s, should as 'YYYY-MM-DD HH:mm:ss'", ftime);
		return false;
	}

	KDatetime tt;
	if(ttime)
	{
		if(!tt.SetDatetimeString(ttime))
		{
			Log(LOG_CONSOLE|LOG_ERROR, "error: invalid date time string %s, should as 'YYYY-MM-DD HH:mm:ss'", ttime);
			return false;
		}
	}

	AnalysisContext context;
	if(!context.initialize(dirName, platformName, fileName, ft, tt))
	{
		Log(LOG_CONSOLE|LOG_ERROR, "error: initialize analysis context.");
		return false;
	}

	context.process();
	context.finalize();

	return true;
}

KAccount* KAnalysis::newAccount(const char* acctName)
{
	AcctMap::iterator it = m_activeAcctMap.find(acctName);
	if(it != m_activeAcctMap.end())
		return it->second;

	KAccount* acct = this->allocAcct();
	acct->m_acctName = g_stringPool.get(acctName);
	m_activeAcctMap[acct->m_acctName] = acct;
	m_activeAcctList.push_back(acct);

	return acct;
}

bool KAnalysis::on_tradeLine(time_t time, KPropertyMap& lmap)
{
	const char* source = lmap.get("source");
	if(!source)
		return false;

	KAccount* srcAcct = this->newAccount(source);
	KTrade* pTrade = srcAcct->on_tradeLine(time, lmap);

	const char* target = lmap.get("target");
	if(!target)
		return false;

	KAccount* targetAcct = this->newAccount(target);
	targetAcct->on_passiveTradeLine(pTrade);

	return true;
}

KAccount* KAnalysis::getAccount(const char* acctname)
{
	AcctMap::iterator it = m_activeAcctMap.find(acctname);
	if(it == m_activeAcctMap.end())
		return NULL;
	return it->second;
}

bool KAnalysis::removeAcct(const char* name, int hours)
{
	Log(LOG_CONSOLE|LOG_DEBUG, "debug: remove account '%s' from redis", name);
	return true;
}

bool KAnalysis::foreachAcct(lua_func func, l_stackval ctx)
{
	KAccount* acct = m_activeAcctList.begin();
	while(acct)
	{
		if(!func.r_invoke<bool,KAccount*,l_stackval>(acct, ctx))
			return false;
		acct = m_activeAcctList.next(acct);
	}
	return true;
}
