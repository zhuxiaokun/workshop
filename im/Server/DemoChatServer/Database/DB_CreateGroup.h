#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;
class KChatGroup;

class DB_CreateGroup
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_CreateGroup,512>
{
public:
	DB_CreateGroup();
	~DB_CreateGroup();

public:
	void setAcct(KChatAccount* chatAcct, const ChatGroup* chatGrp);

public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	UINT64 m_grpID;
	KChatAccount* m_chatAcct;
	ChatGroup m_chatGrp;
};
