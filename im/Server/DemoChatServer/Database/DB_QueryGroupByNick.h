#pragma once
#include <Database/KDatabaseAgent.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KSDList.h>
#include <KCommonStruct.h>
#include "../Cache/KStructDefine.h"

class KChatAccount;

class DB_QueryGroupByNick
	: public KSqlTask
	, public JG_M::KPortableStepPool<DB_QueryGroupByNick,512>
{
public:
	typedef JG_C::KSDSelfList<ChatGroup> GroupList;

public:
	DB_QueryGroupByNick();
	~DB_QueryGroupByNick();

public:
   void setAcct(KChatAccount* chatAcct, const char* grpNick);
public:
	virtual BOOL checkDepends();
	virtual BOOL execute(KSqlContext& ctx);
	virtual BOOL onFinished();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	UINT64 m_grpID;
	string_64  m_grpNick;
	KChatAccount* m_chatAcct;
	//ChatGroup m_chatGrp;
	GroupList    m_grpList;
};
