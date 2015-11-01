#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"
#include <JgMessageDefine.h>

class KChatAccount;

class DB_DeleteMessageByDate
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_DeleteMessageByDate,512>
{
public:
	DB_DeleteMessageByDate();
	~DB_DeleteMessageByDate();

public:
	void setMessage(int yyyymmdd);

public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	int m_yyyymmdd;
};
