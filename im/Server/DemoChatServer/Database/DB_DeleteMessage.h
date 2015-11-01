#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"
#include <JgMessageDefine.h>

class KChatAccount;

class DB_DeleteMessage
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_DeleteMessage,512>
{
public:
	DB_DeleteMessage();
	~DB_DeleteMessage();

public:
	void setMessage(JgMessageTarget target, JgMessageID messageID);

public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

private:
	string_512 _buildMessageFile();

public:
	int m_result;
	JgMessageID m_messageID;
	JgMessageTarget m_messageTarget;
};
