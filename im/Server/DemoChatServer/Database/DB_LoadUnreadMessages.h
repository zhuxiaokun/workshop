#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;

class DB_LoadUnreadMessages
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_LoadUnreadMessages,512>
{
public:
	typedef System::Collections::KSDSelfList<MessageInformation> MessageList;

public:
	DB_LoadUnreadMessages();
	~DB_LoadUnreadMessages();

public:
	void setAcct(KChatAccount* chatAcct);

public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

private:
	bool _loadPersonMessages(KSqlContext& ctx);

public:
	int m_result;
	KChatAccount* m_chatAcct;
	JgMessageID m_lastPersonMessageID;
	JgMessageID m_lastGroupMessageID;
	MessageList m_messageList;
};
