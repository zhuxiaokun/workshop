#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;

class DB_LoadGroupDetail
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_LoadGroupDetail,512>
{
public:
	typedef JG_C::KSDSelfList<ChatAccount> AcctList;

public:
	DB_LoadGroupDetail();
	~DB_LoadGroupDetail();

public:
	void setAcct(UINT64 grpID, KChatAccount* chatAcct);

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
	AcctList m_members;
};
