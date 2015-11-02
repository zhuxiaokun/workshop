#include <lua/KLuaWrap.h>
#include "KGameAccount.h"
#include "Application.h"
#include <CommSession/KPropertyMap.h>

static const char* time2str(int t)
{
	static char buf[32];
	KDatetime dt(t);
	JG_C::KString<32> s = dt.ToString();
	sprintf_k(buf, sizeof(buf), "%s", s.c_str());
	return buf;
}

static void loadActiveServerSet(int start, const char* acct)
{
	char buf[256];
	sprintf_k(buf, sizeof(buf), "acct.%s", acct);

	KRedis redis = g_pApp->masterRedis();

	KActiveServerSet set;
	set.loadFromRedis(redis, start, buf);
}

static void loadAccount(int hours, const char* acct)
{
	KRedis redis = g_pApp->masterRedis();
	KAccount* gameAcct = KAccount::Alloc();
	gameAcct->load(redis, hours, acct);
	//gameAcct->reset(); KAccount::Free(gameAcct);
	LuaWraper["currAcct"] = gameAcct;
}

static void ftest()
{
	char s[] = "in= out=";
	KPropertyMap m;
	m.parseLine(s);
	const char* a = m.get("in");
	const char* b = m.get("out");
	return;
}

void register_lua_utils()
{
	LuaWraper["loadActiveServerSet"] = loadActiveServerSet;
	LuaWraper["loadAccount"] = loadAccount;
	LuaWraper["ftest"] = ftest;
	LuaWraper["time2str"] = time2str;
}
