#include "poolSettings.h"

namespace jg { namespace mysql {

	// hash_i64(val, bucketCount)
	static int hash_i64(lua_State* L)
	{
		if(!lua_isnumber(L, 1))
		{
			luaL_argerror(L, 1, "hash_i64 arg 1 must be a number");
			return 0;
		}
		if(!lua_isnumber(L, 2))
		{
			luaL_argerror(L, 1, "hash_i64 arg 2 must be a number");
			return 0;
		}
		UINT64 val = LMREG::read<UINT64>(L, 1);
		int bucketCount = LMREG::read<int>(L, 2);
		LMREG::push(L, (int)(val & 0x7fffffff) % bucketCount);
		return 1;
	}
	static int hash_string(lua_State* L)
	{
		if(!lua_isstring(L, 1))
		{
			luaL_argerror(L, 1, "hash_string arg 1 must be a string");
			return 0;
		}
		if(!lua_isnumber(L, 2))
		{
			luaL_argerror(L, 1, "hash_string arg 2 must be a number");
			return 0;
		}
		const char* val = LMREG::read<const char*>(L, 1);
		int bucketCount = LMREG::read<int>(L, 2);
		LMREG::push(L, (int)(ELFHash(val) & 0x7fffffff) % bucketCount);
		return 1;
	}

	//// ConnectionSetting

	bool operator < (const ConnectionSetting& a, const ConnectionSetting& b)
	{
		return stricmp(a.m_name.c_str(), b.m_name.c_str()) < 0;
	}

	ConnectionSetting::ConnectionSetting():m_maxConnection(0),m_lifeTime(0)
	{

	}

	ConnectionSetting::~ConnectionSetting()
	{

	}

	bool ConnectionSetting::init(const char* name, KLuaTableWraper& table)
	{
		this->clear();

		m_name = name;
		const char* str; int n;
		
		str = table.getTableField_s("server", NULL);
		if(!str)
		{
			Log(LOG_ERROR, "error: connection setting (%s) 'server' expected.", name);
			return false;
		}
		m_cnnParam.SetHost(str);

		n = table.getTableField_i32("port", KSQLConnParam::MYSQL_DEFAULT_PORT);
		m_cnnParam.SetServerPort(n);

		str = table.getTableField_s("dbname", NULL);
		if(!str)
		{
			Log(LOG_ERROR, "error: connection setting (%s) 'dbname' expected.", name);
			return false;
		}
		m_cnnParam.SetDbName(str);

		str = table.getTableField_s("user", NULL);
		if(!str)
		{
			Log(LOG_ERROR, "error: connection setting (%s) 'user' expected.", name);
			return false;
		}
		m_cnnParam.SetUserName(str);

		str = table.getTableField_s("password", NULL);
		if(!str)
		{
			Log(LOG_ERROR, "error: connection setting (%s) 'password' expected.", name);
			return false;
		}
		m_cnnParam.SetPassword(str);

		str = table.getTableField_s("charset", "utf8");
		m_cnnParam.SetCharset(str);

		n = table.getTableField_i32("max", 10);
		m_maxConnection = n;

		return true;
	}

	bool ConnectionSetting::test()
	{
		KSQLConnection cnn;
		if(cnn.Connect(m_cnnParam)) return true;
		Log(LOG_ERROR, "error: test connection (%s) %s:%u, %s",
			m_name.c_str(),
			m_cnnParam.GetHost(), m_cnnParam.GetServerPort(),
			cnn.GetLastErrorMsg());
		return false;
	}

	void ConnectionSetting::clear()
	{
		m_name.clear();
		m_lifeTime = 0;
		m_maxConnection = 0;
		m_cnnParam = KSQLConnParam();
	}

	//// PoolSettings

	class connection_visitor : public KLuaTableVisitor
	{
	public:
		PoolSettings& m_settings;
		connection_visitor(PoolSettings& settings):m_settings(settings)
		{
		}
		bool onpair(lua_State* L, int key, int val)
		{
			if(!lua_isstring(L, key)) return true;
			const char* connectionName = lua_tostring(L, key);
			KLuaTableWraper table(val);
			KRefPtr<ConnectionSetting> cnnSetting = new ConnectionSetting();
			if(!cnnSetting->init(connectionName, table)) return false;
			cnnSetting->m_lifeTime = m_settings.m_lifetime;
			m_settings._add(cnnSetting);
			return true;
		}
	};

	PoolSettings::PoolSettings():m_lifetime(0),m_luaWrapper(NULL),m_policyTable(NULL)
	{
		m_luaWrapper = new KLuaWrap();
		m_luaWrapper->InitScript(0);
		lua_register(m_luaWrapper->m_l, "hash_i64", hash_i64);
		lua_register(m_luaWrapper->m_l, "hash_string", hash_string);
		m_policyTable = new KLuaTableWraper(m_luaWrapper->m_l, LUA_GLOBALSINDEX);
	}

	PoolSettings::~PoolSettings()
	{

	}

	bool PoolSettings::initialize(const char* conf_lua_file, const KSQLConnParam* defaultConnection)
	{
		if(defaultConnection)
		{
			ConnectionSetting* setting = new ConnectionSetting();
			setting->m_name = this->defaultName();
			setting->m_cnnParam = *defaultConnection;
			setting->m_lifeTime = 30*60;
			setting->m_maxConnection = 0x7fffffff;
			m_defaultConnection = setting;
		}
		if(!conf_lua_file)
		{
			ASSERT(defaultConnection);
			return true;
		}
		return this->reload(conf_lua_file);
	}

	void PoolSettings::finalize()
	{
		m_defaultConnection = NULL;
		m_settings.clear();
	}

	bool PoolSettings::reload(const char* conf_lua_file)
	{
		m_settings.clear();
		{
			KInputFileStream fi(conf_lua_file);
			if(!fi.Good())
			{
				Log(LOG_ERROR, "error: open pool setting file '%s'", conf_lua_file);
				return false;
			}
			lua_State* L = LuaWraper.m_l;
			{
				KLuaStackRecover sr(L);
				int errfunc = LMREG::lua_geterrfunc(L);
				lua_newtable(L);
				{
					KLuaTableWraper ctxtbl(-1);
					ctxtbl.setMetatable("_G");
				}
				if(!LuaWraper.loadStream(fi))
				{
					Log(LOG_ERROR, "error: parse pool setting file '%s', %s", conf_lua_file, lua_tostring(L,-1));
					return false;
				}
				lua_pushvalue(L, -2);
				lua_setfenv(L, -2);
				if(lua_pcall(L, 0, 0, errfunc) != lwp_success)
				{
					Log(LOG_ERROR, "error: execute pool setting file '%s'", conf_lua_file);
					return false;
				}
				{
					KLuaTableWraper table(-1);
					m_lifetime = table.getTableField_i32("lifetime", 30*60);
					if(!m_defaultConnection && table.hasTableField("default", luaTable_t))
					{
						table.attach("default");
						KRefPtr<ConnectionSetting> cnnSetting = new ConnectionSetting();
						if(!cnnSetting->init(defaultName(), table))
							return false;
						cnnSetting->m_lifeTime = m_lifetime;
						m_defaultConnection = cnnSetting;
					}
				}
				{
					KLuaTableWraper table(m_luaWrapper->m_l, -1);
					table.attach("connections");
					connection_visitor v(*this);
					if(!table.foreachpair(v))
						return false;
				}
				{
					KLuaTableWraper table(m_luaWrapper->m_l, -1);
					table.attach("policy");
					*m_policyTable = table;
				}
			}
		}
		return true;
	}

	bool PoolSettings::test()
	{
		if(m_defaultConnection)
		{
			if(!m_defaultConnection->test())
				return false;
		}
		int n = m_settings.size();
		for(int i=0; i<n; i++)
		{
			ConnectionSetting* cnnSetting = m_settings.at(i);
			if(!cnnSetting->test()) return false;
		}
		return true;
	}

	const ConnectionSetting* PoolSettings::get(const char* connectionName) const
	{
		ConnectionSetting tmp; tmp.m_name = connectionName;
		int pos = m_settings.find(KRefPtr<ConnectionSetting>(&tmp,true));
		if(pos < 0) return NULL;
		return m_settings.at(pos);
	}
	
	const ConnectionSetting* PoolSettings::getOrDefault(const char* connectionName)
	{
		const ConnectionSetting* setting = this->get(connectionName);
		return setting ? setting : m_defaultConnection;
	}

	const ConnectionSetting* PoolSettings::callPolicy(const char* policyName, const LMREG::CppTable& params)
	{
		const char* ret;
		if(!m_policyTable->retTableCall(ret, policyName, params))
			return NULL;
		return this->get(ret);
	}

	ConnectionSetting* PoolSettings::_get(const char* connectionName)
	{
		ConnectionSetting tmp; tmp.m_name = connectionName;
		int pos = m_settings.find(KRefPtr<ConnectionSetting>(&tmp,true));
		if(pos < 0) return NULL;
		return m_settings.at(pos);
	}

	bool PoolSettings::_add(ConnectionSetting* cnnSetting)
	{
		int pos = m_settings.find(cnnSetting);
		if(pos >= 0) m_settings[pos] = cnnSetting;
		else m_settings.insert_unique(cnnSetting);
		return true;
	}
}}
