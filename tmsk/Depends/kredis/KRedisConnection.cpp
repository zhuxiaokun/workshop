#include "KRedisConnection.h"
#include <System/KPlatform.h>
#include <System/Log/log.h>
#include <time.h>

namespace jg { namespace redis {

	//// KRedisSesson

	KRedisConnection::KRedisConnection():m_redisCtx(NULL),m_cnnTime(0)
	{

	}

	KRedisConnection::~KRedisConnection()
	{

	}

	bool KRedisConnection::connect(const char* redisIp, int redisPort)
	{
		ASSERT(!m_redisCtx);

		struct timeval tv = {1, 0};
		redisContext* redisCtx = redisConnectWithTimeout(redisIp, redisPort, tv);
		if(redisCtx->err)
		{
			Log(LOG_ERROR, "error: connect to REDIS server, %s", redisCtx->errstr);
			return false;
		}

		redisReply* reply;

		reply = (redisReply*)redisCommand(redisCtx, "PING");
		if(!reply || !reply->str)
		{
			Log(LOG_CONSOLE|LOG_DEBUG, "PING: no reply");
			freeReplyObject(reply);
			redisFree(redisCtx);
			return false;
		}

		if(stricmp(reply->str, "PONG"))
		{
			Log(LOG_CONSOLE|LOG_DEBUG, "PING recv '%s'", reply->str);
			freeReplyObject(reply);
			redisFree(redisCtx);
			return false;
		}

		freeReplyObject(reply);

		m_redisCtx = redisCtx;
		m_cnnTime = time(NULL);
		return true;
	}

	bool KRedisConnection::isReady() const
	{
		return m_redisCtx != NULL;
	}

	bool KRedisConnection::checkReady()
	{
		if(!m_redisCtx)
			return false;

		redisReply* reply;
		redisContext* redisCtx = m_redisCtx;

		reply = (redisReply*)redisCommand(redisCtx, "PING");
		if(!reply || !reply->str)
		{
			Log(LOG_CONSOLE|LOG_DEBUG, "PING: no reply");
			freeReplyObject(reply);
			return false;
		}

		if(stricmp(reply->str, "PONG"))
		{
			Log(LOG_CONSOLE|LOG_DEBUG, "PING recv '%s'", reply->str);
			freeReplyObject(reply);
			return false;
		}

		freeReplyObject(reply);
		return true;
	}

	void KRedisConnection::close()
	{
		if(!m_redisCtx)
			return;
		redisFree(m_redisCtx);
		m_redisCtx = NULL;
		m_cnnTime = 0;
		return;
	}

	//// PooledConnection

	PooledConnection::PooledConnection()
	{

	}

	PooledConnection::~PooledConnection()
	{
		reset();
	}

	void PooledConnection::reset()
	{
		base_type::close();
		m_setting = NULL;
	}

	void PooledConnection::destroy()
	{
		reset();
		PooledConnection::Free(this);
	}

	JG_C::KString<256> PooledConnection::toString() const
	{
		if(!m_setting) return "<empty>";
		char buf[256];
		int n = sprintf_k(buf, sizeof(buf), "(%s) %s:%u max:%d",
			m_setting->m_name.c_str(),
			m_setting->m_ip.c_str(), m_setting->m_port,
			m_setting->m_maxConnection);
		return JG_C::KString<256>(buf, n);
	}

	const char* PooledConnection::getName() const
	{
		if(!m_setting) return NULL;
		return m_setting->m_name.c_str();
	}

	bool PooledConnection::connect(ConnectionSetting* cnnSetting)
	{
		if(m_setting != cnnSetting)
			m_setting = cnnSetting;

		base_type::close();

		const char* ip = m_setting->m_ip.c_str();
		int port = m_setting->m_port;

		return base_type::connect(ip, port);
	}

	bool PooledConnection::reconnect()
	{
		base_type::close();
		return this->connect(m_setting);
	}

	bool PooledConnection::isAlive()
	{
		return base_type::isReady();
	}

	bool PooledConnection::keepAlive()
	{
		if(base_type::isReady())
			return true;
		base_type::close();
		return this->connect(m_setting);
	}

	bool PooledConnection::isExpire(time_t nowTime) const
	{
		if(!base_type::m_cnnTime) return false;
		if(!m_setting->m_lifeTime) return false;
		return m_cnnTime + m_setting->m_lifeTime < nowTime;
	}

}}

