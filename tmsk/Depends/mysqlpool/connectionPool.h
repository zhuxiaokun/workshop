#pragma once
#include "pooledConnection.h"
#include <System/Sync/KSync.h>
#include "poolSettings.h"
#include <System/Collections/KMapByVector.h>
#include <System/Thread/KThread.h>

namespace jg { namespace mysql {

	// 数据库操作的上下文
	class ConnectionPool;
	class KSqlContext : public KRef
	{
	public:
		PooledConnection* pooledCnn;
		ConnectionPool* cnnPool;
		KSQLCommand cmd;
		KSQLResultSet rs;

	public:
		KSqlContext();
		~KSqlContext();

	public:
		bool prepare();
		bool prepare(const char* policyName, LMREG::CppTable* params);
		void reset();
		void destroy(bool ignore);

	public:
		bool isActive() const;
		bool keepAlive();
	};
	
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
		bool initialize(const char* conf_lua_file, const KSQLConnParam* defaultCnn);
		void finalize();

	public:
		PooledConnection* get(const char* policyName, LMREG::CppTable* params);
		PooledConnection* getOrDefault(const char* policyName, LMREG::CppTable* params);
		void release(PooledConnection* cnn);
	};

}}
