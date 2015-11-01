#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;

class DB_QueryAccountByNick
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_QueryAccountByNick,512>
{
public:
	typedef JG_C::KSDSelfList<ChatAccount> AcctList;

public:
	DB_QueryAccountByNick();
	~DB_QueryAccountByNick();

public:
	void setAcct(KChatAccount* chatAcct, const  char * Nick);
public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	KChatAccount* m_chatAcct;
	AcctList m_members;
	string_64 m_nick;
};
