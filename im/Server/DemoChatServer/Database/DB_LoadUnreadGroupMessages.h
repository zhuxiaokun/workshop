#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;

class DB_LoadUnreadGroupMessages
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_LoadUnreadGroupMessages,512>
{
public:
	typedef JG_C::DynArray<UINT64> GroupIdentityArray;
	typedef System::Collections::KSDSelfList<MessageInformation> MessageList;

public:
	DB_LoadUnreadGroupMessages();
	~DB_LoadUnreadGroupMessages();

public:
	void setAcct(KChatAccount* chatAcct);
	void addAcctGroups(KChatAccount* chatAcct);
	void addTargetGroup(UINT64 grpID);

public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

private:
	bool _loadGroupMessages(KSqlContext& ctx, UINT64 grpID);

public:
	int m_result;
	KChatAccount* m_chatAcct;
	GroupIdentityArray m_groupIDs;
	JgMessageID m_lastGroupMessageID;
	MessageList m_messageList;
};
