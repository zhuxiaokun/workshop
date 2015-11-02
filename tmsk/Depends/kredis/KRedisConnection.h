#pragma once
#include <redis/hiredis.h>
#include <System/Collections/KList.h>
#include "redisPoolSetting.h"

namespace jg { namespace redis {

	class KRedisConnection
	{
	public:
		redisContext* m_redisCtx;
		time_t m_cnnTime;

	public:
		KRedisConnection();
		~KRedisConnection();

	public:
		bool connect(const char* ip, int port);
		bool isReady() const;
		bool checkReady();
		void close();
	};

	class PooledConnection
		: private KRedisConnection
		, public JG_C::SelfListNode<PooledConnection>
		, public JG_M::KPortableStepPool<PooledConnection,8>
	{
	public:
		typedef KRedisConnection base_type;
		typedef JG_C::SelfListNode<PooledConnection> list_node;

	public:
		KRefPtr<ConnectionSetting> m_setting;

	public:
		PooledConnection();
		~PooledConnection();

	public:
		void reset();
		void destroy();
		JG_C::KString<256> toString() const;

	public:
		const char* getName() const;
		bool connect(ConnectionSetting* cnnSetting);
		bool reconnect();
		bool isAlive();
		bool keepAlive();
		bool isExpire(time_t nowTime) const;

	public:
		KRedisConnection& connection()
		{
			return *(base_type*)this;
		}
	};

}}

