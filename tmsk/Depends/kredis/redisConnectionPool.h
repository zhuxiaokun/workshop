#pragma once
#include "redisPoolSetting.h"
#include "KRedisConnection.h"
#include <System/Sync/KSync.h>
#include <System/Collections/KMapByVector.h>
#include <System/Thread/KThread.h>
#include <System/Collections/KList.h>

namespace jg { namespace redis {
	class KRedisConnection;
	class ConnectionPool;
}}

typedef jg::redis::KRedisConnection KRedisConnection;
typedef jg::redis::ConnectionPool KRedisConnectionPool;

namespace jg { namespace redis {

	class ConnectionPool : public JG_T::KThread
	{
	public:
		typedef JG_C::KSelfList<PooledConnection> ConnectionList;
		typedef JG_C::KMapByVector<ConnectionName,ConnectionList> ConnectionMap;

	public:
		bool m_stopDesired;
		ConnectionMap m_cnnMap;
		JG_S::KThreadMutex m_mx;
		PoolSettings m_settings;
		JG_C::KString<512> m_conf_lua_file;
		time_t m_lastModifiedTime;
		time_t m_nextLoadTime;
		time_t m_nextCheckIdleTime;

	public:
		ConnectionPool();
		~ConnectionPool();

	public:
		virtual void Execute();
		void stop();

	public:
		bool initialize(const char* conf_lua_file, const char* defaultIp, int defaultPort);
		void finalize();

	private:
		PooledConnection* _get(const char* policyName, LMREG::CppTable* params);
		PooledConnection* _getOrDefault(const char* policyName, LMREG::CppTable* params);
		void _release(PooledConnection* cnn);

	public:
		KRedisConnection* get(const char* policyName, LMREG::CppTable* params);
		KRedisConnection* getOrDefault(const char* policyName, LMREG::CppTable* params);
		void release(KRedisConnection* cnn);
	};
}}
