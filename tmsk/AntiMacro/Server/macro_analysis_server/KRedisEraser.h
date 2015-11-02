#pragma once
#include "redis_scan.h"
#include <kredis/redisConnectionPool.h>
#include <lua/KLuaWrap.h>

class KRedisEraser
{
public:
	KRedisConnection* m_cnn;

public:
	KRedisEraser();
	~KRedisEraser();

public:
	// 删除timeLine之前的数据
	bool eraseAccount(KRedisConnection* cnn, time_t timeLine);
	bool eraseIpAcctList(KRedisConnection* cnn, time_t timeLine);
	bool eraseDevAcctList(KRedisConnection* cnn, time_t timeLine);
	bool eraseLoginList(KRedisConnection* cnn, time_t timeLine);
	bool eraseTradeList(KRedisConnection* cnn, time_t timeLine);

private:
	bool _getExpireAcct(const char* acctKey, time_t timeLine, bool* acctExpire, KLargeNameList& serverList);
	bool _removeAcct(const char* acctName, bool expire, KLargeNameList& slist);
	bool _getExpireAtIpServer(const char* ipServerKey, time_t timeLine, bool* expirep, KLargeNameList& acctList);
	bool _getExpireAtDevServer(const char* devServerKey, time_t timeLine, bool* expirep, KLargeNameList& acctList);
	bool _removeIpServer(const char* ipsKey, bool expire, KLargeNameList& acctList);
	bool _removeDevServer(const char* devsKey, bool expire, KLargeNameList& acctList);
	bool _findExpireLogin(const char* loginKey, time_t timeLine, int* endp);
	bool _findExpireTrade(const char* tradeKey, time_t timeLine, int* endp);

public:
	bool eraseAccount_l(int timeLine);
	bool eraseIpAcctList_l(int timeLine);
	bool eraseDevAcctList_l(int timeLine);
	bool eraseLoginList_l(int timeLine);
	bool eraseTradeList_l(int timeLine);

public:
	bool eraseHistory(int hours);

public:
	BeginDefLuaClassNoneGc(KRedisEraser);
		DefMemberFunc2(eraseAccount_l, "eraseAccount");
		DefMemberFunc2(eraseIpAcctList_l, "eraseIpAcctList");
		DefMemberFunc2(eraseDevAcctList_l, "eraseDevAcctList");
		DefMemberFunc2(eraseLoginList_l, "eraseLoginList");
		DefMemberFunc2(eraseTradeList_l, "eraseTradeList");
		DefMemberFunc(eraseHistory);
	EndDef;
};
