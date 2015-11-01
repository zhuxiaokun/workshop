#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;

class DB_RemoveFriend
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_RemoveFriend,512>
{
public:
	DB_RemoveFriend();
	~DB_RemoveFriend();

public:
	void setAcct(KChatAccount* chatAcct, KChatAccount* frd);

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
