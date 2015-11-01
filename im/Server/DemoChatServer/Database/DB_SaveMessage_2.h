#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;
class KChatGroup;
class KChatMessage;

class DB_SaveMessage_2
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_SaveMessage_2,512>
{
public:
	DB_SaveMessage_2();
	~DB_SaveMessage_2();

public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

private:
	BOOL _savePersonPrivate(KSqlContext& ctx);
	BOOL _saveGroupPrivate(KSqlContext& ctx);

public:
	int m_result;
	JgMessageID m_messageID;
	JgMessageTarget m_messageTarget;
	JgMessageSource m_messageSource;
	JgMessageContentType::Type m_contentType;
	time_t m_messageTime;
};
