#include "KAnalysis.h"
#include "Application.h"
#include "redis_scan.h"

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

class name_visitor : public scan_visitor
{
public:
	KAnalysis& m_analysis;
	enum { prefix_len = 5 }; // acct.
	name_visitor(KAnalysis& n):m_analysis(n)
	{
	}
	bool accept(const char* key)
	{
		const char* acctName = key + prefix_len;
		m_analysis.m_acctNameList.append(acctName);
		return true;
	}
};

bool KAnalysis::scanAcctList(int hours)
{
	//this->reset();

	name_visitor v(*this);
	KRedis redis = g_pApp->masterRedis();
	
	if(!redis.scan("acct.*", v))
	{
		Log(LOG_CONSOLE|LOG_ERROR, "error: scan account name from redis");
		return false;
	}

	KDatetime dt;
	dt.AddSecond(-hours*KDatetime::hour_seconds);

	int startTime = (int)dt.Time();

	char acctKey[128];
	char lastActive[64];

	KLargeNameList::iterator it = m_acctNameList.begin();
	for(; it != m_acctNameList.end(); it++)
	{
		const char* name = *it;
		sprintf_k(acctKey, sizeof(acctKey), "acct.%s", name);
		
		if(!redis.hget(acctKey, "lastActive", lastActive, sizeof(lastActive)))
		{
			if(!redis.isReady())
				return false;
			m_deactiveNameList.append(name);
		}
		else
		{
			int t = str2int(lastActive);
			if(t > startTime) m_activeNameList.append(name);
			else m_deactiveNameList.append(name);
		}
	}

	return true;
}

bool KAnalysis::scanPatternAcctList(int hours, const char* acct_pattern)
{
	name_visitor v(*this);
	KRedis redis = g_pApp->masterRedis();

	char pattern[128];
	sprintf_k(pattern, sizeof(pattern), "acct.%s", acct_pattern);

	if(!redis.scan(pattern, v))
	{
		Log(LOG_CONSOLE|LOG_ERROR, "error: scan account name from redis");
		return false;
	}

	KDatetime dt;
	dt.AddSecond(-hours*KDatetime::hour_seconds);

	int startTime = (int)dt.Time();

	char acctKey[128];
	char lastActive[64];

	KLargeNameList::iterator it = m_acctNameList.begin();
	for(; it != m_acctNameList.end(); it++)
	{
		const char* name = *it;
		sprintf_k(acctKey, sizeof(acctKey), "acct.%s", name);

		if(!redis.hget(acctKey, "lastActive", lastActive, sizeof(lastActive)))
		{
			if(!redis.isReady())
				return false;
			m_deactiveNameList.append(name);
		}
		else
		{
			int t = str2int(lastActive);
			if(t > startTime) m_activeNameList.append(name);
			else m_deactiveNameList.append(name);
		}
	}

	return true;
}

bool KAnalysis::loadAcct(const char* name, int hours)
{
	if(m_activeAcctMap.find(name) != m_activeAcctMap.end())
		return true;

	KAccount* acct = this->allocAcct();
	
	//// master redis
	{
		KRedis redis = g_pApp->masterRedis();
		if(!acct->load(redis, hours, name))
		{
			this->freeAcct(acct);
			return false;
		}
	}

	// other redis
	int n = g_pApp->redisCount();
	int masterIndex = g_pApp->masterRedisIndex();
	for(int i=0; i<n; i++)
	{
		if(i != masterIndex)
		{
			KRedis redis = g_pApp->redisAt(i);
			if(!acct->load(redis, hours, name))
				Log(LOG_DEBUG, "debug: load acct from redis index %d", i);
		}
	}

	m_activeAcctList.push_back(acct);
	m_activeAcctMap[acct->m_acctName] = acct;

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
