#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;

class DB_QueryGroupByID
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_QueryGroupByID,512>
{
public:
	typedef JG_C::KSDSelfList<ChatGroup> GroupList;

public:
	DB_QueryGroupByID();
	~DB_QueryGroupByID();

public:
	void setAcct(KChatAccount* chatAcct, const  UINT64 grpID);
	void setNextRequest(JgMessageRequest* request);
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
	JgMessageRequest* m_nextRequest;
};
