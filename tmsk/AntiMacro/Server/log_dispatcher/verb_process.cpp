#include "verb_process.h"
#include <redis/hiredis.h>
#include "Application.h"

static JG_S::KThreadMutex g_mx;
static BOOL g_initialized = FALSE;
static VerbFunctionMap g_verbMap;

string_512 reply2string(redisReply* reply)
{
	int n = 0;
	char buf[512];
	switch(reply->type)
	{
	case REDIS_REPLY_STRING:
		{
			n = sprintf_k(buf, sizeof(buf), "%s", reply->str);
			break;
		}
	case REDIS_REPLY_ARRAY:
		{
			n = sprintf_k(buf, sizeof(buf), "array count %d", (int)reply->elements);
			break;
		}
	case REDIS_REPLY_INTEGER:
		{
			n = sprintf_k(buf, sizeof(buf), "%lld", reply->integer);
			break;
		}
	case REDIS_REPLY_NIL:
		{
			n = sprintf_k(buf, sizeof(buf), "nil");
			break;
		}
	case REDIS_REPLY_STATUS:
		{
			n = sprintf_k(buf, sizeof(buf), "status %s", reply->str ? reply->str : "");
			break;
		}
	case REDIS_REPLY_ERROR:
		{
			n = sprintf_k(buf, sizeof(buf), "error %s", reply->str ? reply->str : "");
			break;
		}
	}
	return string_512(buf, n);
}

static INT64 g_lineCount = 0;
static void init_verbMap();
void process_verb(KPropertyMap& m, bool fromNetwork)
{
	if(!g_initialized)
		init_verbMap();

	const char* verb = m.get("verb");
	if(!verb) return;

	g_lineCount++;
	if(g_lineCount % 1000 == 0)
		Log(LOG_ERROR|LOG_CONSOLE, "info: proces line count %lld", g_lineCount);

	if(!g_pApp->m_redisReady)
	{
		g_pApp->saveVerbToTmpBuffer(m);
		return;
	}

	VerbFunctionMap::iterator it = g_verbMap.find(verb);
	if(it == g_verbMap.end())
		return;

	it->second(m, fromNetwork);
}

void process_login(KPropertyMap& m, bool fromNetwork)
{
	const char* time = m.get("time");
	const char* server = m.get("server");
	const char* account = m.get("account");
	const char* player = m.get("player");
	const char* level = m.get("level");
	const char* map = m.get("map");
	const char* ip = m.get("ip");
	const char* dev = m.get("dev_id");
	const char* platform = m.get("platform");

	char cTmp[1024];
	char line[1024];
	KPropertyMap tmpMap;

	int replyCount = 0;
	redisReply* reply = NULL;
	redisContext* redisCtx = g_pApp->m_redisCtx;

	{//// debug
		Log(LOG_DEBUG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

		sprintf_k(cTmp, sizeof(cTmp), "hset acct.%s lastActive %s", account, time);
		redisAppendCommand(redisCtx, "hset acct.%s lastActive %s", account, time); replyCount++;
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		sprintf_k(cTmp, sizeof(cTmp), "hset acct.%s lastActive.server.%s %s", account, server, time);
		redisAppendCommand(redisCtx, "hset acct.%s lastActive.server.%s %s", account, server, time); replyCount++;
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		sprintf_k(line, sizeof(line), "\"level=%s lastActive=%s\"", level, time);
		sprintf_k(cTmp, sizeof(cTmp),  "hset player.server.acct.%s.%s %s %s", account, server, player, line);
		redisAppendCommand(redisCtx, "hset player.server.acct.%s.%s %s %s", account, server, player, line); replyCount++;
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		tmpMap.clear();
		tmpMap.set("time", time);
		tmpMap.set("verb", "login");
		tmpMap.set("player", player);
		tmpMap.set("level", level);
		tmpMap.set("map", map);
		tmpMap.set("ip", ip);
		tmpMap.serialize(line, sizeof(line));
		
		sprintf_k(cTmp, sizeof(cTmp), "rpush login.server.acct.%s.%s %s", account, server, line);
		redisAppendCommand(redisCtx, "rpush login.server.acct.%s.%s %s", account, server, line); replyCount++;
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		// 当前状态
		sprintf_k(cTmp, sizeof(cTmp), "hset server.ip.%s.%s %s.%s %s", ip, server, account, player, time);
		redisAppendCommand(redisCtx, "hset server.ip.%s.%s %s.%s %s", ip, server, account, player, time); replyCount++;
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		// 当前状态
		if(dev[0])
		{
			sprintf_k(cTmp, sizeof(cTmp), "hset server.dev.%s.%s %s.%s %s", dev, server, account, player, time);
			redisAppendCommand(redisCtx, "hset server.dev.%s.%s %s.%s %s", dev, server, account, player, time); replyCount++;
			Log(LOG_DEBUG, ">>>> %s", cTmp);
		}
	}

	for(int i=0; i<replyCount; i++)
	{
		int nRet = redisGetReply(redisCtx, (void**)&reply);
		if(nRet != REDIS_OK)
		{
			Log(LOG_ERROR, "error: process verb '%s', save to redis, %s", line, redisCtx->errstr);
			if(reply) freeReplyObject(reply);
			if(!i && fromNetwork) g_pApp->saveVerbToTmpBuffer(m);
			g_pApp->reconnectToRedis();
			return;
		}
		if(reply)
		{
			Log(LOG_DEBUG, "debug: redis reply: %s", reply2string(reply).c_str());
			freeReplyObject(reply);
		}
	}

	return;
}

void process_logout(KPropertyMap& m, bool fromNetwork)
{
	const char* time = m.get("time");
	const char* server = m.get("server");
	const char* account = m.get("account");
	const char* player = m.get("player");
	const char* dev = m.get("dev_id");
	const char* ip = m.get("ip");

	char cTmp[1024];
	char line[1024];
	KPropertyMap tmpMap;

	int replyCount = 0;
	redisReply* reply = NULL;
	redisContext* redisCtx = g_pApp->m_redisCtx;

	{//// debug
		Log(LOG_DEBUG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

		sprintf_k(cTmp, sizeof(cTmp), "hset acct.%s lastActive %s", account, time);
		redisAppendCommand(redisCtx, "hset acct.%s lastActive %s", account, time); replyCount++;
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		sprintf_k(cTmp, sizeof(cTmp), "hset acct.%s lastActive.server.%s %s", account, server, time);
		redisAppendCommand(redisCtx, "hset acct.%s lastActive.server.%s %s", account, server, time); replyCount++;
		Log(LOG_DEBUG, ">>>> %s", cTmp);
		
		tmpMap.clear();
		tmpMap.set("time", time);
		tmpMap.set("verb", "logout");
		tmpMap.serialize(line, sizeof(line));

		sprintf_k(cTmp, sizeof(cTmp), "rpush login.server.acct.%s.%s %s", account, server, line);
		redisAppendCommand(redisCtx, "rpush login.server.acct.%s.%s %s", account, server, line); replyCount++;
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		sprintf_k(cTmp, sizeof(cTmp), "hdel server.ip.%s.%s %s.%s", ip, server, account, player);
		redisAppendCommand(redisCtx, "hdel server.ip.%s.%s %s.%s", ip, server, account, player); replyCount++;
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		if(dev[0]) // dev可能是空
		{
			sprintf_k(cTmp, sizeof(cTmp), "hdel server.dev.%s.%s %s.%s", dev, server, account, player);
			redisAppendCommand(redisCtx, "hdel server.dev.%s.%s %s.%s", dev, server, account, player); replyCount++;
			Log(LOG_DEBUG, ">>>> %s", cTmp);
		}
	}

	for(int i=0; i<replyCount; i++)
	{
		int nRet = redisGetReply(redisCtx, (void**)&reply);
		if(nRet != REDIS_OK)
		{
			Log(LOG_ERROR, "error: process verb '%s', save to redis, %s", line, redisCtx->errstr);
			if(reply) freeReplyObject(reply);
			if(!i && fromNetwork) g_pApp->saveVerbToTmpBuffer(m);
			g_pApp->reconnectToRedis();
			return;
		}
		if(reply)
		{
			Log(LOG_DEBUG, "debug: redis reply: %s", reply2string(reply).c_str());
			freeReplyObject(reply);
		}
	}
}

void process_trade(KPropertyMap& m, bool fromNetwork)
{
	const char* time = m.get("time");
	const char* server = m.get("server");
	const char* src = m.get("source");
	const char* target = m.get("target");
	const char* srcPlayer = m.get("srcPlayer");
	const char* dstPlayer = m.get("dstPlayer");
	const char* svlevel = m.get("sviplevel");
	const char* tvlevel = m.get("tviplevel");
	const char* friendDegree = m.get("friendDegree");
	const char* in_item = m.get("in");
	const char* out_item = m.get("out");
	const char* map = m.get("map");
	const char* sip = m.get("sip");
	const char* tip = m.get("tip");

	char cTmp[1024];
	char line[1024];
	KPropertyMap tmpMap;

	redisReply* reply = NULL;
	redisContext* redisCtx = g_pApp->m_redisCtx;

	{//// debug
		Log(LOG_DEBUG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");

		sprintf_k(cTmp, sizeof(cTmp), "hset acct.%s lastActive %s", src, time);
		redisAppendCommand(redisCtx, "hset acct.%s lastActive %s", src, time);
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		sprintf_k(cTmp, sizeof(cTmp), "hset acct.%s lastActive.server.%s %s", src, server, time);
		redisAppendCommand(redisCtx, "hset acct.%s lastActive.server.%s %s", src, server, time);
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		sprintf_k(cTmp, sizeof(cTmp), "hset acct.%s lastActive %s", target, time);
		redisAppendCommand(redisCtx, "hset acct.%s lastActive %s", target, time);
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		sprintf_k(cTmp, sizeof(cTmp), "hset acct.%s lastActive.server.%s %s", target, server, time);
		redisAppendCommand(redisCtx, "hset acct.%s lastActive.server.%s %s", target, server, time);
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		tmpMap.clear();
		tmpMap.set("time", time);
		tmpMap.set("target", target);
		tmpMap.set("srcPlayer", srcPlayer);
		tmpMap.set("dstPlayer", dstPlayer);
		tmpMap.set("sVipLevel", svlevel);
		tmpMap.set("tVipLevel", tvlevel);
		if(sip) tmpMap.set("sip", sip);
		if(tip) tmpMap.set("tip", tip);
		tmpMap.set("friendDegree", friendDegree);
		tmpMap.set("in", in_item);
		tmpMap.set("out", out_item);
		tmpMap.set("map", map);
		tmpMap.serialize(line, sizeof(line));

		sprintf_k(cTmp, sizeof(cTmp), "rpush trade.server.acct.%s.%s %s", src, server, line);
		redisAppendCommand(redisCtx, "rpush trade.server.acct.%s.%s %s", src, server, line);
		Log(LOG_DEBUG, ">>>> %s", cTmp);

		tmpMap.clear();
		tmpMap.set("time", time);
		tmpMap.set("target", src);
		tmpMap.set("srcPlayer", dstPlayer);
		tmpMap.set("dstPlayer", srcPlayer);
		tmpMap.set("sVipLevel", tvlevel);
		tmpMap.set("tVipLevel", svlevel);
		if(tip) tmpMap.set("sip", tip);
		if(sip) tmpMap.set("tip", sip);
		tmpMap.set("friendDegree", friendDegree);
		tmpMap.set("in", out_item);
		tmpMap.set("out", in_item);
		tmpMap.set("map", map);
		tmpMap.serialize(line, sizeof(line));

		sprintf_k(cTmp, sizeof(cTmp), "rpush trade.server.acct.%s.%s %s", target, server, line);
		redisAppendCommand(redisCtx, "rpush trade.server.acct.%s.%s %s", target, server, line);
		Log(LOG_DEBUG, ">>>> %s", cTmp);
	}

	int replyCount = 6;
	for(int i=0; i<replyCount; i++)
	{
		int nRet = redisGetReply(redisCtx, (void**)&reply);
		if(nRet != REDIS_OK)
		{
			Log(LOG_ERROR, "error: process verb '%s', save to redis, %s", line, redisCtx->errstr);
			if(reply) freeReplyObject(reply);
			if(!i && fromNetwork) g_pApp->saveVerbToTmpBuffer(m);
			g_pApp->reconnectToRedis();
			return;
		}
		if(reply)
		{
			Log(LOG_DEBUG, "debug: redis reply: %s", reply2string(reply).c_str());
			freeReplyObject(reply);
		}
	}
}

static void init_verbMap()
{
	if(g_initialized)
		return;

	JG_S::KAutoThreadMutex mx(g_mx);
	if(g_initialized)
		return;

	g_verbMap["login"] = process_login;
	g_verbMap["logout"] = process_logout;
	g_verbMap["trade"] = process_trade;

	g_initialized = TRUE;
}
