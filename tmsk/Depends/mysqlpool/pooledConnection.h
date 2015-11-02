#pragma once
#include <mysqldb/KDatabase.h>
#include <System/Collections/KList.h>
#include "poolSettings.h"
#include <System/Memory/KStepObjectPool.h>
#include <System/Sync/KSync.h>

namespace jg { namespace mysql {

class PooledConnection
	: public JG_C::SelfListNode<PooledConnection>
	, public JG_M::KPortableStepPool<PooledConnection,64,JG_S::KMTLock>
{
public:
	KSQLConnection m_connection;
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
	bool isAlive() const;
	bool keepAlive();
	bool isExpire(time_t nowTime) const;

public:
	KSQLConnection& connection()
	{
		return m_connection;
	}
};

}}
