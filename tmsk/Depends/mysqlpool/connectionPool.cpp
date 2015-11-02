#include "connectionPool.h"
#include <time.h>
#include <System/File/KFile.h>

namespace jg { namespace mysql {

	//// KSqlContext

	// jg::mysql::ConnectionPool* cnnPool;
	// KSQLCommand cmd;
	// KSQLResultSet rs;

	KSqlContext::KSqlContext() : pooledCnn(NULL),cnnPool(NULL)
	{

	}

	KSqlContext::~KSqlContext()
	{
		reset();
	}

	bool KSqlContext::prepare()
	{
		if(pooledCnn) reset();
		pooledCnn = cnnPool->getOrDefault(NULL, NULL);
		if(!pooledCnn) return false;
		cmd.SetConnection(pooledCnn->connection());
		return true;
	}

	bool KSqlContext::prepare(const char* policyName, LMREG::CppTable* params)
	{
		if(pooledCnn) reset();
		pooledCnn = cnnPool->getOrDefault(policyName, params);
		if(!pooledCnn) return false;
		cmd.SetConnection(pooledCnn->connection());
		return true;
	}

	void KSqlContext::reset()
	{
		if(pooledCnn)
		{
			cmd.Reset();
			cnnPool->release(pooledCnn);
			pooledCnn = NULL;
		}
	}

	void KSqlContext::destroy(bool ignore)
	{
		reset();
		if(!ignore)
			delete this;
	}

	bool KSqlContext::isActive() const
	{
		return pooledCnn ? pooledCnn->isAlive() : false;
	}

	bool KSqlContext::keepAlive()
	{
		return pooledCnn ? pooledCnn->keepAlive() : false;
	}

	//// ConnectionPool
	ConnectionPool::ConnectionPool()
	{
		m_stopDesired = false;
		m_lastModifiedTime = 0;
		m_nextLoadTime = time(NULL);
		m_nextCheckIdleTime = time(NULL);
	}

	ConnectionPool::~ConnectionPool()
	{
		finalize();
	}

	void ConnectionPool::Execute()
	{
		while(!m_stopDesired)
		{
			time_t nowTime = time(NULL);
			if(!m_conf_lua_file.empty() && nowTime >= m_nextLoadTime)
			{
				time_t lastModify = JG_F::KFileUtil::GetLastModified(m_conf_lua_file.c_str());
				if(lastModify != m_lastModifiedTime)
				{
					PoolSettings::ConnectionSettingArray cpy(m_settings.m_settings);
					if(!m_settings.reload(m_conf_lua_file.c_str()))
					{
						Log(LOG_WARN, "warn: fail reload pool setting file '%s', recover", m_conf_lua_file.c_str());
						m_settings.m_settings = cpy;
					}
					m_lastModifiedTime = lastModify;
				}
				m_nextLoadTime = nowTime + 10;
			}
			if(nowTime > m_nextCheckIdleTime)
			{
				JG_S::KAutoThreadMutex mx(m_mx);
				ConnectionMap::iterator it = m_cnnMap.begin(), ite = m_cnnMap.end();
				for(; it != ite; it++)
				{
					ConnectionList tmp;
					ConnectionList& lst = it->second;
					PooledConnection* cnn;
					while(cnn = lst.pop_front())
					{
						if(cnn->isExpire(nowTime))
						{
							Log(LOG_DEBUG, "debug: %s expire, abandon", cnn->toString().c_str());
							cnn->destroy();
						}
						else
						{
							tmp.push_back(cnn);
						}
					}
					while(cnn = tmp.pop_front())
					{
						lst.push_back(cnn);
					}
				}
				m_nextCheckIdleTime = nowTime + 30;
			}
			msleep(100);
		}
	}

	void ConnectionPool::stop()
	{
		m_stopDesired = true;
		JG_T::KThread::WaitFor();
	}

	bool ConnectionPool::initialize(const char* conf_lua_file, const KSQLConnParam* defaultCnn)
	{
		m_conf_lua_file = conf_lua_file;
		
		if(!m_settings.initialize(conf_lua_file, defaultCnn))
			return false;
		
		if(!m_conf_lua_file.empty())
			m_lastModifiedTime = JG_F::KFileUtil::GetLastModified(conf_lua_file);

		if(!m_settings.test())
			return false;
		
		JG_T::KThread::Run();
		return true;
	}

	void ConnectionPool::finalize()
	{
		this->stop();

		JG_S::KAutoThreadMutex mx(m_mx);
		ConnectionMap::iterator it = m_cnnMap.begin();
		ConnectionMap::iterator ite = m_cnnMap.end();
		for(; it!=ite; it++)
		{
			ConnectionList& lst = it->second;
			PooledConnection* cnn;
			while(cnn = lst.pop_front(), cnn)
				cnn->destroy();
		}
		m_cnnMap.clear();
	}

	PooledConnection* ConnectionPool::get(const char* policyName, LMREG::CppTable* params)
	{	
		PooledConnection* cnn = NULL;
		{
			JG_S::KAutoThreadMutex mx(m_mx);
			
			ConnectionSetting* cnnSetting = (ConnectionSetting*)m_settings.callPolicy(policyName, *params);
			if(!cnnSetting) return NULL;

			const char* cnnName = cnnSetting->m_name.c_str();

			ConnectionList& lst = m_cnnMap[cnnName];
			cnn = lst.pop_front();
			if(!cnn)
			{
				cnn = PooledConnection::Alloc(); cnn->reset();
				cnn->m_setting = cnnSetting;
			}
		}
		if(!cnn->keepAlive())
		{
			this->release(cnn);
			return NULL;
		}
		return cnn;
	}

	PooledConnection* ConnectionPool::getOrDefault(const char* policyName, LMREG::CppTable* params)
	{
		if(!m_settings.empty())
		{
			return this->get(policyName, params);
		}
		{
			PooledConnection* cnn = NULL;
			
			JG_S::KAutoThreadMutex mx(m_mx);

			ConnectionSetting* cnnSetting = m_settings.defaultConnection();
			if(!cnnSetting) return NULL;

			ConnectionList& lst = m_cnnMap[m_settings.defaultName()];

			cnn = lst.pop_front();
			if(!cnn)
			{
				cnn = PooledConnection::Alloc(); cnn->reset();
				cnn->m_setting = cnnSetting;
			}

			if(!cnn->keepAlive())
			{
				this->release(cnn);
				return NULL;
			}

			return cnn;
		}
	}

	void ConnectionPool::release(PooledConnection* cnn)
	{
		const char* cnnName = cnn->getName();
		ASSERT(cnnName);

		JG_S::KAutoThreadMutex mx(m_mx);

		ConnectionMap::iterator it = m_cnnMap.find(cnnName);
		ASSERT(it != m_cnnMap.end());

		ConnectionList& lst = it->second;
		lst.push_back(cnn);
	}

} }
