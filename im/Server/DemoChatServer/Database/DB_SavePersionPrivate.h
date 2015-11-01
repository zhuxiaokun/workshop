#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"
#include "../Cache/KAccountCache.h"

class KChatAccount;
class KChatGroup;

class DB_SavePersonPrivate
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_SavePersonPrivate,512>
{
public:
	DB_SavePersonPrivate();
	~DB_SavePersonPrivate();

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
	UINT64 m_receiverID;
	UINT64 m_senderID;
	string_256 m_messageFile;
	KChatMessage* m_message;
};
