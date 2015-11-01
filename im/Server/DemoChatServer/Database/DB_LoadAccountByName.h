#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"
#include "../Request/JgMessageRequest.h"

class KChatAccount;

class DB_LoadAccountByName: public KSqlTask, public JG_M::KPortableStepPool<DB_LoadAccountByName,512>
{
public:
	DB_LoadAccountByName();
	~DB_LoadAccountByName();

public:
	void setNextRequest(JgMessageRequest* request);

public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	string_64 m_acctName;
	ChatAccount m_acctInfo;
	JgMessageRequest* m_nextRequest;
};
