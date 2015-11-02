/********************************************************************
history:
created:	2008/12/12
created:	12:12:2008

filename: 	\Athena\Main\Inc\KServerSettings.h
file path:	\Athena\Main\Inc\
file base:	KServerSettings
file ext:	h

author:		Calvin
copyright:  极光互动 2007-2008

purpose:	所有服务器的配置文件
comment:    
*********************************************************************/
#pragma once

#include <System/KPlatform.h>
#include <System/tinyxml/xmldom.h>
#include <System/Log/log.h>
#include <System/Misc/StrUtil.h>

#define CS_SETTINGS_PATH			"../Settings/ServerInfo/"
#define SETTINGS_FILE_PATH			"../Settings/ServerInfo/ServerSettings.xml"
#define DATABASE_FILE_PATH			"../Settings/ServerInfo/DatabaseSettings.xml"

enum SERVER_SETTINGS_INFO
{
	enum_SSI_ServerStart = 0,
	enum_SSI_LauncherServer,
	enum_SSI_LoginServer,
	enum_SSI_PassportServer,
	enum_SSI_CharacterServer,
	enum_SSI_LogServer,
	enum_SSI_MonitorManager,
	enum_SSI_WorldServer,
	enum_SSI_ChatServer,
	enum_SSI_DBAgent,
	enum_SSI_GameServer,
	enum_SSI_AIServer,
	enum_SSI_BillingServer,
	enum_SSI_MailServer,
	enum_SSI_HackServer,

	enum_SSI_Count,
};

enum DB_SETTINGS_INFO
{
	enum_DSI_DBStart = 0,
	enum_DSI_AccountDB,
	enum_DSI_GameDB,
	enum_DSI_LogDB,
	enum_DSI_CountDB,
	enum_DSI_BillingDB,
	enum_DSI_MailDB,
	enum_DSI_NickDB,
	enum_DSI_Count,
};

enum SERVER_LISTEN_TYPE
{
	enum_SBS_Private,
	enum_SBS_Public,
	enum_SBS_Count,
};

struct KServerSettingsInfo
{
	JG_C::KString<32> Name;
	INT				ID;
	INT				Type;
	INT				MaxPlayer;
	JG_C::KString<16> PublicAddr;
	int				PublicPort;
	JG_C::KString<16> PrivateAddr;
	int				PrivatePort;
	INT				MaxDBLink;
	INT				LogLevel;
};


struct KDatabaseSettingsInfo
{
	JG_C::KString<16>	Addr;
	int					Port;
	JG_C::KString<32>	DBName;
	JG_C::KString<32>	DBUser;
	JG_C::KString<32>	DBPassword;
	JG_C::KString<32>	CharSet;
};

class KServerSetting
{
public:
	KServerSetting(){};
	~KServerSetting(){};

	BOOL Init()
	{
		if(!InitializeServerSettings(SETTINGS_FILE_PATH))
		{
			Log(LOG_ERROR , "InitializeServerSettings %s Error" , SETTINGS_FILE_PATH);
			return FALSE;
		}

		if(!InitializeDatabaseSettings(DATABASE_FILE_PATH))
		{
			Log(LOG_ERROR , "InitializeDatabaseSettings %s Error" , SETTINGS_FILE_PATH);
			return FALSE;
		}
		return TRUE;
	}


public:

	void SetServerID(enum SERVER_SETTINGS_INFO eServerType,INT nID)
	{
		ASSERT(eServerType >= enum_SSI_ServerStart && eServerType < enum_SSI_Count);
		m_ServerSettings[eServerType].ID = nID;
	}

	void SetServerIP(enum SERVER_LISTEN_TYPE eListenType , enum SERVER_SETTINGS_INFO eServerType,char* strIP)
	{
		ASSERT(eServerType >= enum_SSI_ServerStart && eServerType < enum_SSI_Count);
		if (eListenType == enum_SBS_Public)		 m_ServerSettings[eServerType].PublicAddr = strIP;
		else		 m_ServerSettings[eServerType].PrivateAddr = strIP;
	}

	void SetServerPort(enum SERVER_LISTEN_TYPE eListenType , enum SERVER_SETTINGS_INFO eServerType,WORD wPort)
	{
		ASSERT(eServerType >= enum_SSI_ServerStart && eServerType < enum_SSI_Count);
		if (eListenType == enum_SBS_Public)		m_ServerSettings[eServerType].PublicPort = wPort;
		else		m_ServerSettings[eServerType].PrivatePort = wPort;
	}


	INT GetServerID(enum SERVER_SETTINGS_INFO eServerType)
	{
		ASSERT(eServerType >= enum_SSI_ServerStart && eServerType < enum_SSI_Count);
		return m_ServerSettings[eServerType].ID;
	}

	const char* GetServerName(enum SERVER_SETTINGS_INFO eServerType)
	{
		ASSERT(eServerType >= enum_SSI_ServerStart && eServerType < enum_SSI_Count);
		return m_ServerSettings[eServerType].Name.c_str();
	}

	const char* GetServerIP(enum SERVER_LISTEN_TYPE eListenType , enum SERVER_SETTINGS_INFO eServerType)
	{
		ASSERT(eServerType >= enum_SSI_ServerStart && eServerType < enum_SSI_Count);
		if (eListenType == enum_SBS_Public)		return m_ServerSettings[eServerType].PublicAddr.c_str();
		else		return m_ServerSettings[eServerType].PrivateAddr.c_str();
	}

	INT GetServerPort(enum SERVER_LISTEN_TYPE eListenType , enum SERVER_SETTINGS_INFO eServerType)
	{
		ASSERT(eServerType >= enum_SSI_ServerStart && eServerType < enum_SSI_Count);
		if (eListenType == enum_SBS_Public)		return m_ServerSettings[eServerType].PublicPort;
		else		return m_ServerSettings[eServerType].PrivatePort;
	}

	INT GetMaxPlayer(enum SERVER_SETTINGS_INFO eServerType)
	{
		ASSERT(eServerType >= enum_SSI_ServerStart && eServerType < enum_SSI_Count);
		return m_ServerSettings[eServerType].MaxPlayer;
	}

	INT GetMaxDBLink(enum SERVER_SETTINGS_INFO eServerType)
	{
		ASSERT(eServerType >= enum_SSI_ServerStart && eServerType < enum_SSI_Count);
		return m_ServerSettings[eServerType].MaxDBLink;
	}

	INT GetLogLevel(enum SERVER_SETTINGS_INFO eServerType)
	{
		ASSERT(eServerType >= enum_SSI_ServerStart && eServerType < enum_SSI_Count);
		return m_ServerSettings[eServerType].LogLevel;
	}

	const char* GetDBIp(enum DB_SETTINGS_INFO eDBType)
	{
		ASSERT(eDBType >= enum_DSI_DBStart && eDBType < enum_DSI_Count);
		return m_DBSettings[eDBType].Addr.c_str();
	}

	const char* GetDBName(enum DB_SETTINGS_INFO eDBType)
	{
		ASSERT(eDBType >= enum_DSI_DBStart && eDBType < enum_DSI_Count);
		return m_DBSettings[eDBType].DBName.c_str();
	}

	const char* GetDBUser(enum DB_SETTINGS_INFO eDBType)
	{
		ASSERT(eDBType >= enum_DSI_DBStart && eDBType < enum_DSI_Count);
		return m_DBSettings[eDBType].DBUser.c_str();
	}

	const char* GetDBPassword(enum DB_SETTINGS_INFO eDBType)
	{
		ASSERT(eDBType >= enum_DSI_DBStart && eDBType < enum_DSI_Count);
		return m_DBSettings[eDBType].DBPassword.c_str();
	}

	const char* GetDBCharSet(enum DB_SETTINGS_INFO eDBType)
	{
		ASSERT(eDBType >= enum_DSI_DBStart && eDBType < enum_DSI_Count);
		return m_DBSettings[eDBType].CharSet.c_str();
	}


	INT GetDBPort(enum DB_SETTINGS_INFO eDBType)
	{
		ASSERT(eDBType >= enum_DSI_DBStart && eDBType < enum_DSI_Count);
		return m_DBSettings[eDBType].Port;
	}	


private:
	BOOL InitializeServerSettings(const char* xmlCfgFile)
	{
		XMLElementNode m_xmlSetting;

		XMLDomParser parser;
		parser.Initialize("GB2312");

		KInputFileStream fi;
		if(!fi.Open(xmlCfgFile))
		{
			return FALSE;
		}
		if(!parser.Parse(fi, m_xmlSetting))
		{
			printf("Error %s" , parser.GetErrorMsg());
			return FALSE;
		}

		XMLNodeColl coll;
		coll = m_xmlSetting.QueryChildren("ServerSettings/Server");
		for(int i=0; i < coll.Size(); i++)
		{
			XMLElementNode* pXml = (XMLElementNode*)coll.GetNode(i);
			int type	= (int)pXml->QueryAtt("Type")->Integer();

			m_ServerSettings[type].ID			= pXml->QueryAtt("ID")->Integer();
			m_ServerSettings[type].Name			= pXml->QueryAtt("Name")->c_str();
			m_ServerSettings[type].PrivateAddr	= pXml->QueryAtt("PrivateIP")->c_str();
			m_ServerSettings[type].PrivatePort	= pXml->QueryAtt("PrivatePort")->Integer();
			m_ServerSettings[type].PublicAddr	= pXml->QueryAtt("PublicIP")->c_str();
			m_ServerSettings[type].PublicPort	= pXml->QueryAtt("PublicPort")->Integer();
			m_ServerSettings[type].LogLevel		= pXml->QueryAtt("LogLevel")->Integer();
			
			switch(type)
			{
			case enum_SSI_LoginServer:
			case enum_SSI_CharacterServer:
			case enum_SSI_WorldServer:
			case enum_SSI_GameServer:
			case enum_SSI_ChatServer:
				{
					m_ServerSettings[type].MaxPlayer = (int)pXml->QueryAtt("MaxPlayer")->Integer();
				};
			}

			switch(type)
			{
			case enum_SSI_LoginServer:
			case enum_SSI_CharacterServer:
			case enum_SSI_MailServer:
				{
					m_ServerSettings[type].MaxDBLink = (int)pXml->QueryAtt("MaxDBLink")->Integer();
				}
				break;
			}			
		}

		return TRUE;
	}
	
	BOOL InitializeDatabaseSettings(const char* xmlCfgFile)
	{
		XMLElementNode m_xmlSetting;

		XMLDomParser parser;
		parser.Initialize("GB2312");

		KInputFileStream fi;
		if(!fi.Open(xmlCfgFile))
		{
			return FALSE;
		}
		if(!parser.Parse(fi, m_xmlSetting))
		{
			printf("Error %s" , parser.GetErrorMsg());
			return FALSE;
		}

		XMLNodeColl coll;
		coll = m_xmlSetting.QueryChildren("DatabaseSettings/Database");
		for(int i=0; i < coll.Size(); i++)
		{
			XMLElementNode* pXml = (XMLElementNode*)coll.GetNode(i);
			int type	= (int)pXml->QueryAtt("Type")->Integer();
			m_DBSettings[type].Addr		= pXml->QueryAtt("IP")->c_str();
			m_DBSettings[type].Port		= pXml->QueryAtt("Port")->Integer();
			m_DBSettings[type].DBName		= pXml->QueryAtt("DBName")->c_str();
			m_DBSettings[type].DBUser		= pXml->QueryAtt("DBUser")->c_str();
			m_DBSettings[type].DBPassword	= pXml->QueryAtt("DBPassword")->c_str();
			m_DBSettings[type].CharSet		= pXml->QueryAtt("DBChatSet")->c_str();
		}

		return TRUE;
	}


	void Finalize();

private:
	KServerSettingsInfo		m_ServerSettings[enum_SSI_Count];
	KDatabaseSettingsInfo	m_DBSettings[enum_DSI_Count];
	JG_S::KThreadMutex	m_mx;
};


