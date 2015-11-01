#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;

class DB_LoadGroups
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_LoadGroups,512>
{
public:
	typedef JG_C::KSDSelfList<ChatGroup> GroupList;

public:
	DB_LoadGroups();
	~DB_LoadGroups();

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
	KChatAccount* m_chatAcct;
	GroupList m_groups;
};
