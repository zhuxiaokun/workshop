#include "KRedisEraser.h"
#include "Application.h"
#include <CommSession/KPropertyMap.h>

////
class key_scan_visitor : public scan_visitor
{
public:
	KLargeNameList& m_nameList;
	key_scan_visitor(KLargeNameList& keyNameList):m_nameList(keyNameList) { }
	virtual bool accept(const char* key)
	{
		m_nameList.append(key);
		return true;
	}
};

//// KRedisEraser

KRedisEraser::KRedisEraser():m_cnn(NULL)
{

}

KRedisEraser::~KRedisEraser()
{

}

bool KRedisEraser::eraseAccount(KRedisConnection* cnn, time_t timeLine)
{
	m_cnn = cnn;
	KRedis redis(cnn->m_redisCtx);

	KLargeNameList acctKeyList;
	key_scan_visitor v(acctKeyList);

	redis.scan("acct.*", v);
	
	bool expire;
	KLargeNameList serverList;

	KLargeNameList::iterator it = acctKeyList.begin();
	for(; it != acctKeyList.end(); it++)
	{
		const char* acctKey = *it;

		expire = false;
		serverList.reset();

		this->_getExpireAcct(acctKey, timeLine, &expire, serverList);

		if(serverList.size())
		{
			const char* acctName = acctKey + strlen("acct.");
			this->_removeAcct(acctName, expire, serverList);
		}
	}

	acctKeyList.reset();
	return true;
}

bool KRedisEraser::eraseIpAcctList(KRedisConnection* cnn, time_t timeLine)
{
	KLargeNameList keyList;

	m_cnn = cnn;
	KRedis redis(m_cnn->m_redisCtx);

	key_scan_visitor v(keyList);
	redis.scan("server.ip.*", v);

	bool expire;
	KLargeNameList acctList;

	KLargeNameList::iterator it = keyList.begin();
	for(; it != keyList.end(); it++)
	{
		const char* ipsKey = *it;

		expire = false;
		acctList.reset();

		this->_getExpireAtIpServer(ipsKey, timeLine, &expire, acctList);

		if(expire || acctList.size())
			this->_removeIpServer(ipsKey, expire, acctList);
	}

	return true;
}

bool KRedisEraser::eraseDevAcctList(KRedisConnection* cnn, time_t timeLine)
{
	KLargeNameList keyList;

	m_cnn = cnn;
	KRedis redis(m_cnn->m_redisCtx);

	key_scan_visitor v(keyList);
	redis.scan("server.dev.*", v);

	bool expire;
	KLargeNameList acctList;

	KLargeNameList::iterator it = keyList.begin();
	for(; it != keyList.end(); it++)
	{
		const char* ipsKey = *it;

		expire = false;
		acctList.reset();

		this->_getExpireAtDevServer(ipsKey, timeLine, &expire, acctList);

		if(expire || acctList.size())
			this->_removeDevServer(ipsKey, expire, acctList);
	}

	return true;
}

bool KRedisEraser::eraseLoginList(KRedisConnection* cnn, time_t timeLine)
{
	KLargeNameList keyList;

	m_cnn = cnn;
	KRedis redis(m_cnn->m_redisCtx);

	key_scan_visitor v(keyList);
	redis.scan("login.server.acct.*", v);

	KLargeNameList::iterator it = keyList.begin();
	for(; it != keyList.end(); it++)
	{
		const char* key = *it;
		int bindex = 0;
		if(this->_findExpireLogin(key, timeLine, &bindex))
		{
			redis.ltrim(key, bindex, -1);
		}
	}

	return true;
}

bool KRedisEraser::eraseTradeList(KRedisConnection* cnn, time_t timeLine)
{
	KLargeNameList keyList;

	m_cnn = cnn;
	KRedis redis(m_cnn->m_redisCtx);

	key_scan_visitor v(keyList);
	redis.scan("trade.server.acct.*", v);

	KLargeNameList::iterator it = keyList.begin();
	for(; it != keyList.end(); it++)
	{
		const char* key = *it;
		int bindex = 0;
		if(this->_findExpireTrade(key, timeLine, &bindex))
		{
			redis.ltrim(key, bindex, -1);
		}
	}

	return true;
}

class acctExpire_hscan_visitor : public hscan_visitor
{
public:
	time_t m_timeLine;
	bool* m_acctExpire;
	KLargeNameList& m_slist;
	acctExpire_hscan_visitor(time_t timeLine, bool* acctExpire, KLargeNameList& slist)
		: m_timeLine(timeLine)
		, m_acctExpire(acctExpire)
		, m_slist(slist)
	{
		*m_acctExpire = false;
	}
	virtual bool accept(const char* field, const char* val)
	{
		const int prefix_len = 18; // strlen("lastActive.server.")
		if(stricmp(field, "lastActive") == 0)
		{
			time_t t = (time_t)str2i64(val);
			if(t < m_timeLine)
			{
				*m_acctExpire = true;
			}
		}
		else
		{
			if(*m_acctExpire || (time_t)str2i64(val) < m_timeLine)
			{
				m_slist.append(field + prefix_len);
			}
		}
		return true;
	}
};

bool KRedisEraser::_getExpireAcct(const char* acctKey, time_t timeLine, bool* acctExpire, KLargeNameList& serverList)
{
	ASSERT(m_cnn->m_redisCtx);

	KRedis redis(m_cnn->m_redisCtx);
	acctExpire_hscan_visitor v(timeLine, acctExpire, serverList);

	redis.hscan(acctKey, "*", v);
	return true;
}

bool KRedisEraser::_removeAcct(const char* acctName, bool expire, KLargeNameList& slist)
{
	KLargeNameList keyNameList;
	KLargeNameList::iterator it = slist.begin();

	char key[128];
	int n = 0, c = 0;
	const char* keys[8];

	ASSERT(m_cnn);
	KRedis redis(m_cnn->m_redisCtx);

	for(; it != slist.end(); it++)
	{
		const char* serverName = *it;
		if(expire) { n = sprintf_k(key, sizeof(key), "acct.%s", acctName); keys[c++] = keyNameList.append(key); }
		n = sprintf_k(key, sizeof(key), "player.server.acct.%s.%s", acctName, serverName); keys[c++] = keyNameList.append(key);
		n = sprintf_k(key, sizeof(key), "login.server.acct.%s.%s", acctName, serverName); keys[c++] = keyNameList.append(key);
		n = sprintf_k(key, sizeof(key), "trade.server.acct.%s.%s", acctName, serverName); keys[c++] = keyNameList.append(key);
		redis.del(keys, c);
	}

	if(!expire)
	{
		n = sprintf_k(key, sizeof(key), "acct.%s", acctName);
		const char* acctKey = keyNameList.append(key);

		c = 0;
		it = slist.begin();
		int count = slist.size();
		for(; it != slist.end(); it++)
		{
			const char* serverName = *it;
			if(c == sizeof(keys)/sizeof(ccstr))
			{
				redis.hdel(acctKey, keys, c);
				c = 0;
			}
			n = sprintf_k(key, sizeof(key), "lastActive.server.%s", serverName);
			keys[c++] = keyNameList.append(key);
		}

		if(c)
			redis.hdel(acctKey, keys, c);
	}

	return true;
}

class ips_hscan_visitor : public hscan_visitor
{
public:
	time_t m_timeLine;
	bool* m_expirep;
	KLargeNameList& m_acctList;
	ips_hscan_visitor(time_t timeLine, bool* expirep, KLargeNameList& nameList)
		: m_timeLine(timeLine)
		, m_expirep(expirep)
		, m_acctList(nameList)
	{
		*expirep = true;
	}
	virtual bool accept(const char* field, const char* val)
	{
		time_t t = (time_t)str2i64(val);
		if(t < m_timeLine)
		{
			m_acctList.append(field);
		}
		else
		{
			*m_expirep = false;
		}
		return true;
	}
};

bool KRedisEraser::_getExpireAtIpServer(const char* ipServerKey, time_t timeLine, bool* expirep, KLargeNameList& acctList)
{
	*expirep = false;

	ASSERT(m_cnn);
	KRedis redis(m_cnn->m_redisCtx);

	ips_hscan_visitor v(timeLine, expirep, acctList);
	redis.hscan(ipServerKey, "*", v);

	return true;
}

class devs_hscan_visitor : public hscan_visitor
{
public:
	time_t m_timeLine;
	bool* m_expirep;
	KLargeNameList& m_acctList;
	devs_hscan_visitor(time_t timeLine, bool* expirep, KLargeNameList& nameList)
		: m_timeLine(timeLine)
		, m_expirep(expirep)
		, m_acctList(nameList)
	{
		*expirep = true;
	}
	virtual bool accept(const char* field, const char* val)
	{
		time_t t = (time_t)str2i64(val);
		if(t < m_timeLine)
		{
			m_acctList.append(field);
		}
		else
		{
			*m_expirep = false;
		}
		return true;
	}
};

bool KRedisEraser::_getExpireAtDevServer(const char* devServerKey, time_t timeLine, bool* expirep, KLargeNameList& acctList)
{
	*expirep = false;

	ASSERT(m_cnn);
	KRedis redis(m_cnn->m_redisCtx);

	devs_hscan_visitor v(timeLine, expirep, acctList);
	redis.hscan(devServerKey, "*", v);

	return true;
}

bool KRedisEraser::_removeIpServer(const char* ipsKey, bool expire, KLargeNameList& acctList)
{
	ASSERT(m_cnn);
	KRedis redis(m_cnn->m_redisCtx);

	if(expire)
	{
		redis.del(&ipsKey, 1);
		return true;
	}
	
	int c = 0;
	const char* keys[8];

	KLargeNameList::iterator it = acctList.begin();
	for(; it != acctList.end(); it++)
	{
		const char* acctName = *it;
		if(c == sizeof(keys)/sizeof(ccstr))
		{
			redis.hdel(ipsKey, keys, c);
			c = 0;
		}
		keys[c++] = acctName;
	}
	
	if(c)
		redis.hdel(ipsKey, keys, c);

	return true;
}

class loginExpire_lscan_visitor : public lscan_visitor
{
public:
	int* m_countp;
	time_t m_timeLine;

	char buf[1024];
	KPropertyMap m;
	bool open;
	
	loginExpire_lscan_visitor(time_t timeLine, int* countp)
		: m_timeLine(timeLine)
		, m_countp(countp),open(false)
	{
		*m_countp = 0;
	}
	virtual bool accept(const char* val)
	{
		m.clear();

		strcpy_k(buf, sizeof(buf), val);
		m.parseLine(buf);
		
		const char* verb = m.get("verb");

		if(stricmp(verb , "login") == 0)
		{
			if(open)
			{
				*m_countp += 1;
				return true;
			}
			open = true;
		}
		else
		{
			const char* tc = m.get("time");
			time_t t = (time_t)str2i64(tc);
			if(t >= m_timeLine) return false;
			if(open) *m_countp += 2;
			else *m_countp += 1;
			open = false;
		}

		return true;
	}
};

bool KRedisEraser::_removeDevServer(const char* devsKey, bool expire, KLargeNameList& acctList)
{
	ASSERT(m_cnn);
	KRedis redis(m_cnn->m_redisCtx);

	if(expire)
	{
		redis.del(&devsKey, 1);
		return true;
	}

	int c = 0;
	const char* keys[8];

	KLargeNameList::iterator it = acctList.begin();
	for(; it != acctList.end(); it++)
	{
		const char* acctName = *it;
		if(c == sizeof(keys)/sizeof(ccstr))
		{
			redis.hdel(devsKey, keys, c);
			c = 0;
		}
		keys[c++] = acctName;
	}

	if(c)
		redis.hdel(devsKey, keys, c);

	return true;
}

bool KRedisEraser::_findExpireLogin(const char* loginKey, time_t timeLine, int* endp)
{
	ASSERT(m_cnn);
	KRedis redis(m_cnn->m_redisCtx);

	int lineCount = 0;
	loginExpire_lscan_visitor v(timeLine, &lineCount);
	redis.lrange(loginKey, 0, -1, v);

	*endp = lineCount;
	return true;
}

class tradeExpire_lscan_visitor : public lscan_visitor
{
public:
	int* m_countp;
	time_t m_timeLine;

	char buf[1024];
	KPropertyMap m;
	bool open;

	tradeExpire_lscan_visitor(time_t timeLine, int* countp)
		: m_timeLine(timeLine)
		, m_countp(countp)
	{
		*m_countp = 0;
	}
	virtual bool accept(const char* val)
	{
		m.clear();

		strcpy_k(buf, sizeof(buf), val);
		m.parseLine(buf);

		const char* verb = m.get("verb");

		const char* tc = m.get("time");
		time_t t = (time_t)str2i64(tc);
		if(t >= m_timeLine) return false;
		*m_countp += 1;

		return true;
	}
};

bool KRedisEraser::_findExpireTrade(const char* tradeKey, time_t timeLine, int* endp)
{
	ASSERT(m_cnn);
	KRedis redis(m_cnn->m_redisCtx);

	int lineCount = 0;
	tradeExpire_lscan_visitor v(timeLine, &lineCount);
	redis.lrange(tradeKey, 0, -1, v);

	*endp = lineCount;
	return true;
}

bool KRedisEraser::eraseAccount_l(int timeLine)
{
	KRedisConnection* cnn = g_pApp->masterRedisConnection();
	return this->eraseAccount(cnn, timeLine);
}

bool KRedisEraser::eraseIpAcctList_l(int timeLine)
{
	KRedisConnection* cnn = g_pApp->masterRedisConnection();
	return this->eraseIpAcctList(cnn, timeLine);
}

bool KRedisEraser::eraseDevAcctList_l(int timeLine)
{
	KRedisConnection* cnn = g_pApp->masterRedisConnection();
	return this->eraseDevAcctList(cnn, timeLine);
}

bool KRedisEraser::eraseLoginList_l(int timeLine)
{
	KRedisConnection* cnn = g_pApp->masterRedisConnection();
	return this->eraseLoginList(cnn, timeLine);
}

bool KRedisEraser::eraseTradeList_l(int timeLine)
{
	KRedisConnection* cnn = g_pApp->masterRedisConnection();
	return this->eraseTradeList(cnn, timeLine);
}

bool KRedisEraser::eraseHistory(int hours)
{
	KDatetime dt;
	dt.AddSecond(-hours*KDatetime::hour_seconds);
	time_t t = (time_t)dt.Time();
	this->eraseAccount_l((int)t);
	this->eraseIpAcctList_l((int)t);
	this->eraseDevAcctList_l((int)t);
	this->eraseLoginList_l((int)t);
	this->eraseTradeList_l((int)t);
	return true;
}
