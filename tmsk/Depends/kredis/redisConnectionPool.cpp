#include "redisConnectionPool.h"
#include <time.h>
#include <System/File/KFile.h>

namespace jg { namespace redis {

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

	bool ConnectionPool::initialize(const char* conf_lua_file, const char* defaultIp, int defaultPort)
	{
		m_conf_lua_file = conf_lua_file;
		
		if(!m_settings.initialize(conf_lua_file, defaultIp, defaultPort))
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

	PooledConnection* ConnectionPool::_get(const char* policyName, LMREG::CppTable* params)
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
			this->_release(cnn);
			return NULL;
		}
		return cnn;
	}

	PooledConnection* ConnectionPool::_getOrDefault(const char* policyName, LMREG::CppTable* params)
	{
		if(!m_settings.empty())
		{
			return this->_get(policyName, params);
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
				this->_release(cnn);
				return NULL;
			}

			return cnn;
		}
	}

	void ConnectionPool::_release(PooledConnection* cnn)
	{
		const char* cnnName = cnn->getName();
		ASSERT(cnnName);

		JG_S::KAutoThreadMutex mx(m_mx);

		ConnectionMap::iterator it = m_cnnMap.find(cnnName);
		ASSERT(it != m_cnnMap.end());

		ConnectionList& lst = it->second;
		lst.push_back(cnn);
	}

	KRedisConnection* ConnectionPool::get(const char* policyName, LMREG::CppTable* params)
	{
		PooledConnection* p = this->_get(policyName, params);
		if(!p) return NULL;
		return &p->connection();
	}

	KRedisConnection* ConnectionPool::getOrDefault(const char* policyName, LMREG::CppTable* params)
	{
		PooledConnection* p = this->_getOrDefault(policyName, params);
		if(!p) return NULL;
		return &p->connection();
	}

	void ConnectionPool::release(KRedisConnection* cnn)
	{
		PooledConnection* p = (PooledConnection*)cnn;
		this->_release(p);
	}

}}

