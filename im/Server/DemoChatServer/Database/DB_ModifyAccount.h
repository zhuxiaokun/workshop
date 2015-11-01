#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;

class DB_ModifyAccount
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_ModifyAccount,512>
{
public:
	DB_ModifyAccount();
	~DB_ModifyAccount();

public:
	void setAcct(KChatAccount* chatAcct, const ChatAccount* acctInfo);

public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	ChatAccount m_acctInfo;
	KChatAccount* m_chatAcct;
};
