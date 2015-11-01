#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;

class DB_AddFriend
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_AddFriend,512>
{
public:
	DB_AddFriend();
	~DB_AddFriend();

public:
	void setAcct(KChatAccount* acct_a, KChatAccount* acct_b);

public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	KChatAccount* m_chatAcct;
	KChatAccount* m_friend;
};
