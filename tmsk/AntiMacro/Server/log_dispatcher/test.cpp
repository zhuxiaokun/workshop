#include <System/KType.h>
#include <System/SockHeader.h>
#include <System/Log/log.h>

#pragma pack(push,1)
#include <redis/hiredis.h>
#pragma pack(pop)

int testRedis(int argc, char* argv[])
{
	struct timeval tv = {1, 0};
	redisContext* redisCtx = redisConnectWithTimeout("127.0.0.1", 6379, tv);
	if(redisCtx->err)
	{
		Log(LOG_ERROR, "error: connect to REDIS server, %s", redisCtx->errstr);
		return 1;
	}

	redisReply* reply;

	reply = (redisReply*)redisCommand(redisCtx, "PING");
	if(reply) Log(LOG_CONSOLE|LOG_DEBUG, "PING: %s %d", reply->str, reply->len);
	freeReplyObject(reply);

	/* Set a key */
	reply = (redisReply *)redisCommand(redisCtx, "SET %s %s", "foo", "hello world");
	Log(LOG_DEBUG, "SET: %s %d", reply->str, reply->len);
	freeReplyObject(reply);

	reply = (redisReply *)redisCommand(redisCtx, "GET foo");
	Log(LOG_DEBUG, "GET foo: %s %d", reply->str, reply->len);
	freeReplyObject(reply);

	redisFree(redisCtx);
	return 0;
}
