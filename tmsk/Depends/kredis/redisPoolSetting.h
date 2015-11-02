#pragma once
#include <System/Misc/KRefObject.h>
#include <System/Collections/KString.h>
#include <lua/KLuaTableHolder.h>
#include <System/Collections/DynArray.h>


namespace jg { namespace redis {

	typedef JG_C::KString<32> ip_name;
	typedef JG_C::KString<64> ConnectionName;

	class ConnectionSetting;
	bool operator < (const ConnectionSetting& a, const ConnectionSetting& b);

	class ConnectionSetting : public KRef
	{
	public:
		ConnectionName m_name;
		int m_lifeTime;
		int m_maxConnection;
		ip_name m_ip;
		unsigned short m_port;

	public:
		ConnectionSetting();
		~ConnectionSetting();

	public:
		bool init(const char* name, KLuaTableWraper& table);
		bool test();
		void clear();
	};

	class PoolSettings
	{
	public:
		typedef KRefPtr<ConnectionSetting> SettingPtr;
		typedef JG_C::KDynSortedObjectArray<SettingPtr> ConnectionSettingArray;
		ConnectionSettingArray m_settings;
		SettingPtr m_defaultConnection;
		int m_lifetime;

	public:
		KLuaWrap* m_luaWrapper;
		KLuaTableWraper* m_policyTable;

	public:
		PoolSettings();
		~PoolSettings();

	public:
		bool initialize(const char* conf_lua_file, const char* defaultIp=NULL, int defaultPort=0);
		bool reload(const char* conf_lua_file);
		void finalize();

	public:
		bool test();

	public:
		bool empty() const { return m_settings.empty(); }
		const char* defaultName() const { return "_dEf_"; }
		ConnectionSetting* defaultConnection() { return m_defaultConnection; }

		const ConnectionSetting* get(const char* connectionName) const;
		const ConnectionSetting* getOrDefault(const char* connectionName);
		const ConnectionSetting* callPolicy(const char* policyName, const LMREG::CppTable& params);

	public:
		ConnectionSetting* _get(const char* connectionName);
		bool _add(ConnectionSetting* cnnSetting);
	};
	
}}
