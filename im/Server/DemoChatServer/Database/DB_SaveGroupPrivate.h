#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;
class KChatGroup;
class KChatMessage;

class DB_SaveGroupPrivate
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_SaveGroupPrivate,512>
{
public:
	DB_SaveGroupPrivate();
	~DB_SaveGroupPrivate();

public:
	void setMessage(KChatMessage* message);

public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	UINT64 m_groupID;
	UINT64 m_senderID;
	string_256 m_messageFile;
	KChatMessage* m_message;
};
