#pragma once
#include <System/KPlatform.h>
#include <SockServer/JgPacketSpecific.h>
#include <Database/KDatabaseAgent.h>
#include <lua/KLuaWrap.h>
#include "PacketGate/KPacketGate_CCht.h"
#include "Cache/KAccountCache.h"
#include "Cache/KGroupCache.h"
#include "Request/JgRequestManager.h"
#include "Storage/KStorageAgent.h"
#include "KApplicationStatus.h"
#include <Timer/KLogicTimer.h>

class KDemoChatServerApp;
extern KDemoChatServerApp* g_pApp;

class KDemoChatServerApp
	: public KLogicTimerManager
{
public:
	enum
	{
		Timer_RemoveExpiredMessage,
		Timer_LoadUnreadMessages,
	};
	struct Data_LoadUnreadMessages : public JG_M::KPortableMemoryPool<Data_LoadUnreadMessages,1024>
	{
		KChatAccount* chatAcct;
		INT64 lastPersonMessage;
		INT64 lastGroupMessage;
	};

public:
	KDemoChatServerApp();
	~KDemoChatServerApp();

public:
	BOOL initialize();
	void breathe(int ms);
	void finalize();

private:
	BOOL _initializeNetwork();
	BOOL _initializeDatabase();
	BOOL _initializeStorage();
	void _networkBreathe(int ms);
	void _processPacket(KSocket* pSock, const void* packet);

public:
	bool onTimer(DWORD name, Identity64 cookie, void* data);
	string_512 buildMessageShortFilePath(const JgMessageTarget& target, const JgMessageID& mid) const;
	string_512 buildMessageFullFilePath(const JgMessageTarget& target, const JgMessageID& mid) const;

public:
	LMREG::CppTable m_serverSettings;
	JgSockServer m_sockServer;
	KDatabaseAgent m_databaseAgent;
	KStorageAgent m_storageAgent;
	KChatAccountCache m_acctCache;
	KChatGroupCache m_grpCache;
	JgRequestManager m_requestManager;
	KChatCandidateManager m_candidateManager;
	JG_S::KSync_PulseEvent m_inputEvent;

public:
	DWORD m_nextSequence;
	KPacketGate_CCht m_gateCCht;
	System::Collections::DynArray<void*> m_packets;
	char m_packetBuffer[102400];
	JgPacketReceiver* m_socks[20480];

public:
	DWORD m_nowTicks;
	time_t m_nowTime;
	KApplicationStatus m_appStatus;
	JgMessageID lastMessageID() { return m_appStatus.lastMessageID(); }
	JgMessageID nextMessageID() { return m_appStatus.nextMessageID(); }
};
