#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;
class KChatGroup;

class DB_JoinGroup
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_JoinGroup,512>
{
public:
	DB_JoinGroup();
	~DB_JoinGroup();

public:
	void setAcct(KChatAccount* chatAcct, KChatGroup* chatGrp);

public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	KChatAccount* m_chatAcct;
	KChatGroup* m_chatGrp;
};
