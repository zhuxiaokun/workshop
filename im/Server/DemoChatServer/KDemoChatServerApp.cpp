#include "KDemoChatServerApp.h"
#include "PacketGate/KSocketDefine.h"
#include <System/File/KFile.h>
#include "Database/DB_DeleteMessageByDate.h"

CREATE_LUAWRAPPER();
KDemoChatServerApp* g_pApp = NULL;

KDemoChatServerApp::KDemoChatServerApp():m_nextSequence(0)
{
	m_nowTicks = 0;
	m_nowTime = 0;
}

KDemoChatServerApp::~KDemoChatServerApp()
{

}

BOOL KDemoChatServerApp::initialize()
{
	LuaWraper.InitScript(0);
	const char* filename = "../Settings/ServerInfo/ServerSettings.lua";
	KInputFileStream fi(filename);
	if(!fi.Good())
	{
		Log(LOG_ERROR, "error: fail to open '%s'", filename);
		return FALSE;
	}

	if(!LuaWraper.doStream(fi))
	{
		Log(LOG_ERROR, "error: lua load stream for '%s'", filename);
		return FALSE;
	}
	m_serverSettings = LuaWraper["ServerSettings"]["DemoChatServer"];

	GetGlobalLogger().SetLogLevel(m_serverSettings["LogLevel"]);

	if(!this->_initializeDatabase())//初始化数据库
		return FALSE;

	if(!this->_initializeStorage())//初始化数据存数路径
		return FALSE;

	if(!this->_initializeNetwork())//初始化网络配置
		return FALSE;

	this->startTimerAt(Timer_RemoveExpiredMessage, 0, (TIME_T)time(NULL), NULL);//??????

	return TRUE;
}

void KDemoChatServerApp::breathe(int ms)
{
	m_nowTicks = GetTickCount();
	m_nowTime = time(NULL);
	KLogicTimerManager::breathe(ms);
	m_appStatus.breathe(ms);
	m_databaseAgent.breathe(ms);
	this->_networkBreathe(ms);
	m_storageAgent.Breathe(ms);
}

void KDemoChatServerApp::finalize()
{
	m_sockServer.Finalize();
	m_databaseAgent.finalize();
}

BOOL KDemoChatServerApp::_initializeNetwork()//初始化网络net
{
	int maxClient = m_serverSettings["MaxClient"];
	if(!m_sockServer.Initialize(maxClient, 5))
	{
		Log(LOG_ERROR, "error: initialize sock server");
		return FALSE;
	}

	const char* listenIp = m_serverSettings["BindAddress"]["public"]["ip"];//服务器ip
	int listenPort = m_serverSettings["BindAddress"]["public"]["port"];//端口号

	KSocket_TcpListen* pSock = new KSocket_TcpListen();
	pSock->Reset(); pSock->AddRef();
	if(!pSock->Listen(listenIp, listenPort))//如果监听该ip和端口失败
	{
		Log(LOG_ERROR, "error: tcp listen at %s:%u", listenIp, listenPort);
		return FALSE;
	}
	m_sockServer.Attach(pSock);//将该socket加到监听序列中,如果socket数已经达到设定的最大值,则添加失败
	return TRUE;
}





class TheSocketFilter : public JgSocketFilter
{
public:
	BOOL Accept(KSocket* pSock)
	{
		DWORD ud = pSock->GetUserData();//获取用户数据
		if(ud != enum_socket_client) return FALSE;
		KSocket_Client* clnSocket = (KSocket_Client*)pSock;
		return clnSocket->ReadAvail() > 0;
	}
};




BOOL KDemoChatServerApp::_initializeDatabase()
{
	KSQLConnParam connParam;
	connParam.SetHost(m_serverSettings["Database"]["host"]);
	connParam.SetDbName(m_serverSettings["Database"]["dbname"]);
	connParam.SetUserName(m_serverSettings["Database"]["username"]);
	connParam.SetPassword(m_serverSettings["Database"]["password"]);
	connParam.SetCharset(m_serverSettings["Database"]["charset"]);
	int threadCount = m_serverSettings["DatabaseThreadCount"];
	if(!m_databaseAgent.initialize(connParam, threadCount, &m_inputEvent))
	{
		Log(LOG_ERROR, "error: database agent initialize failed");
		return FALSE;
	}
	return TRUE;
}

BOOL KDemoChatServerApp::_initializeStorage()//初始化数据存储
{
	const char* dataDir = m_serverSettings["Storage"]["Location"];

	if(!JG_F::KFileUtil::IsFileExists(dataDir))
	{
		if(!JG_F::KFileUtil::MakeDir(dataDir))
		{
			Log(LOG_ERROR, "error: make dir %s", dataDir);
			return FALSE;
		}
	}

	if(!m_appStatus.initialize(dataDir))
	{
		Log(LOG_ERROR, "error: initialize application status");
		return FALSE;
	}
	int threadCount = m_serverSettings["Storage"]["ThreadCount"];
	return m_storageAgent.Initialize(dataDir, threadCount);
}

void KDemoChatServerApp::_networkBreathe(int ms)
{
	TheSocketFilter socketFilter;
	int_r num = m_sockServer.LockSockets(&socketFilter, m_socks, 20480);
	for(int_r i=0; i<num; i++)
	{
		DWORD dwStart = GetTickCount();
		JgPacketReceiver* r = m_socks[i];
		while(TRUE)
		{
			int_r packetNum = r->ReadPackets(m_packetBuffer, sizeof(m_packetBuffer), m_packets);//读包
			if(!packetNum) break;

			for(int_r k=0; k<packetNum; k++)
			{
				void* packet = m_packets[k];
				this->_processPacket(r->GetSocket(), packet);
			}

			if(r->_getUserData() == enum_socket_client)///??????????
				break;

			if(GetTickCount() > dwStart + 50)
				break;
		}
	}
	m_sockServer.UnlockSockets(m_socks, num);
}

void KDemoChatServerApp::_processPacket(KSocket* pSock, const void* packet)
{
	const JgPacketHead* pHead = (JgPacketHead*)packet;//获取包的头部
	int cmd = pHead->command;
	int len = pHead->length;
	const void* pData = pHead + 1;//数据部分

	int userData = pSock->GetUserData();
	switch(userData)
	{
	   case enum_socket_client:
		{
			KSocket_Client* clnSocket = (KSocket_Client*)pSock;
			m_gateCCht.Process(clnSocket, cmd, pData, len);
		 }
		break;
	};
}

bool KDemoChatServerApp::onTimer(DWORD name, Identity64 cookie, void* data)//
{
	switch(name)
	{
	case Timer_RemoveExpiredMessage://移除过期消息
		{
			KDatetime nowDate; nowDate.AddDay(-8);
			KLocalDatetime ldt = nowDate.GetLocalTime();
			int yyyymmdd = ldt.year*10000 + ldt.month*100 + ldt.day;
			
			DB_DeleteMessageByDate* task = DB_DeleteMessageByDate::Alloc();
			task->reset(); task->setMessage(yyyymmdd);
			m_databaseAgent.push(*task);

			TIME_T t = KDatetime::nextDayFirstSecond((TIME_T)time(NULL));
			KDatetime dt(t+3*60*60); // 凌晨 3 点
			this->startTimerAt(Timer_RemoveExpiredMessage, 0, (TIME_T)dt.Time(), NULL);
			break;
		}
	case Timer_LoadUnreadMessages://加载未读消息
		{
			Data_LoadUnreadMessages* timerData = (Data_LoadUnreadMessages*)data;
			KChatAccount* chatAcct = timerData->chatAcct;
			if(!chatAcct->hasFlag(KChatAccount::friends_loaded) || !chatAcct->hasFlag(KChatAccount::groups_loaded))
			{
				g_pApp->startTimer(KDemoChatServerApp::Timer_LoadUnreadMessages, chatAcct->m_acctID, 5.0f, timerData);
			}
			else
			{
				chatAcct->loadUnreadMessages(timerData->lastPersonMessage, timerData->lastGroupMessage);
				Data_LoadUnreadMessages::Free(timerData);
			}
			break;
		}
	}
	return true;
}

string_512 KDemoChatServerApp::buildMessageShortFilePath(const JgMessageTarget& target, const JgMessageID& mid) const
{
	char buf[512]; int n;
	switch(target.targetType)
	{
	case JgMessageTargetType::AVATAR://头像
		n = sprintf_k(buf, sizeof(buf), "avatar/%llu/%08d-%04d", target.targetID, mid.yyyy_mm_dd, mid.sequence);
		break;
	case JgMessageTargetType::PERSON://个人消息
		n = sprintf_k(buf, sizeof(buf), "private/%08d/person/%llu/%u", mid.yyyy_mm_dd, target.targetID, mid.sequence);
		break;
	case JgMessageTargetType::GROUP://群消息
		n = sprintf_k(buf, sizeof(buf), "private/%08d/group/%llu/%u", mid.yyyy_mm_dd, target.targetID, mid.sequence);
		break;
	case JgMessageTargetType::PUBLIC://发布公告
		n = sprintf_k(buf, sizeof(buf), "public/%08d/person/%llu/%u", mid.yyyy_mm_dd, target.targetID, mid.sequence);
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return string_512(buf, n);
}

string_512 KDemoChatServerApp::buildMessageFullFilePath(const JgMessageTarget& target, const JgMessageID& mid) const
{
	char buf[512];
	int n = sprintf_k(buf, sizeof(buf), "%s/%s",
		m_storageAgent.m_dataDir.c_str(),
		this->buildMessageShortFilePath(target, mid).c_str());
	return string_512(buf, n);
}
