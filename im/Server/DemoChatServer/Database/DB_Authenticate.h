#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;

class DB_Authenticate
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_Authenticate,512>
{
public:
	DB_Authenticate();
	~DB_Authenticate();

public:
	void setAcct(KChatAccount* chatAcct);

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
