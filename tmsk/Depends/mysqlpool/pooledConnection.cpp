#include "pooledConnection.h"

namespace jg { namespace mysql {

	//KSQLConnection* m_connection;
	//KRefPtr<ConnectionSetting> m_setting;

	PooledConnection::PooledConnection()
	{

	}

	PooledConnection::~PooledConnection()
	{
		reset();
	}

	void PooledConnection::reset()
	{
		m_connection.Close();
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
		int n = sprintf_k(buf, sizeof(buf), "(%s) %s:%u db:%s charset:%s",
			m_setting->m_name.c_str(),
			m_setting->m_cnnParam.GetHost(), m_setting->m_cnnParam.GetServerPort(),
			m_setting->m_cnnParam.GetDbName(), m_setting->m_cnnParam.GetCharset());
		return JG_C::KString<256>(buf, n);
	}

	const char* PooledConnection::getName() const
	{
		if(!m_setting) return NULL;
		return m_setting->m_name.c_str();
	}

	bool PooledConnection::connect(ConnectionSetting* cnnSetting)
	{
		if(m_setting != cnnSetting) m_setting = cnnSetting;
		m_connection.Close();
		if(!m_connection.Connect(cnnSetting->m_cnnParam))
		{
			Log(LOG_ERROR, "error: connect %s, %s", toString().c_str(), m_connection.GetLastErrorMsg());
			return false;
		}
		if(!m_connection.SelectDatabase(cnnSetting->m_cnnParam.GetDbName()))
		{
			Log(LOG_ERROR, "error: connect %s, %s", toString().c_str(), m_connection.GetLastErrorMsg());
			m_connection.Close();
			return false;
		}
		return true;
	}

	bool PooledConnection::isAlive() const
	{
		return !!m_connection.IsActive();
	}

	bool PooledConnection::keepAlive()
	{
		if(m_connection.IsActive())
			return true;
		m_connection.Close();
		return this->connect(m_setting);
	}

	bool PooledConnection::isExpire(time_t nowTime) const
	{
		if(!m_connection.m_cnnTime) return false;
		if(!m_setting->m_lifeTime) return false;
		return m_connection.m_cnnTime + m_setting->m_lifeTime < nowTime;
	}

} }
