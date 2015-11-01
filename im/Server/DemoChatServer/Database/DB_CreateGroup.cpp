#include "DB_CreateGroup.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "../Cache/KGroupCache.h"
#include "DB_LoadGroups.h"
#include<JgErrorCode.h>
DB_CreateGroup::DB_CreateGroup():m_result(0),m_grpID(0),m_chatAcct(NULL)
{
	memset(&m_chatGrp, 0, sizeof(m_chatGrp));
}

DB_CreateGroup::~DB_CreateGroup()
{

}

void DB_CreateGroup::setAcct(KChatAccount* chatAcct, const ChatGroup* chatGrp)
{
	if(m_chatAcct != chatAcct)
	{
		if(m_chatAcct)
		{
			m_chatAcct->releaseRef();
			m_chatAcct = NULL;
		}
		if(chatAcct)
		{
			chatAcct->addRef();
			m_chatAcct = chatAcct;
		}
	}
	if(chatGrp)
		memcpy(&m_chatGrp, chatGrp, sizeof(m_chatGrp));
	else
		memset(&m_chatGrp, 0, sizeof(m_chatGrp));
}

BOOL DB_CreateGroup::checkDepends()
{
	return TRUE;
}

BOOL DB_CreateGroup::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_CreateGroup(?,?)";
	KSQLFieldDescribe params[2];
	params[0].m_name = "nick";
	params[0].m_cdt = KSQLTypes::sql_c_string;
	params[0].m_length = 64;
	params[1].m_name = "siganature";
	params[1].m_cdt = KSQLTypes::sql_c_string;
	params[1].m_length = 256;

	if(!cmd.SetSQLStatement(sql))
	{
		m_result = JgErrorCode::err_SqlStatement;
		return FALSE;
	}
	if(!cmd.DescribeParameters(params, 2))
	{
		m_result =JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetString(0, m_chatGrp.m_nick.c_str()))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetString(1, m_chatGrp.m_signature.c_str()))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_CreateGroup, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!rs.Begin())
	{
		m_result =JgErrorCode::err_CreateGroup;
		return TRUE;
	}

	rs.GetInteger(0, m_result);
	rs.GetInt64(1, *(INT64*)&m_grpID);
	m_chatGrp.m_groupID = m_grpID;

	return TRUE;
}

BOOL DB_CreateGroup::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_CreateGroup::onFinished acct:%llu result:%d",
		m_chatAcct->m_acctID, m_result);

	if(m_result)
	{
		Log(LOG_ERROR, "error: CreateGroup '%s'", m_chatGrp.m_nick.c_str());
		ChtC_CreateGroup_Rsp rsp;
		rsp.result = m_result;
		m_chatAcct->send(s_nChtC_CreateGroup_Rsp, &rsp, sizeof(rsp));
		return FALSE;
	}

	KChatGroup* chatGrp = KChatGroup::Alloc(); chatGrp->reset();
	chatGrp->copy(m_chatGrp);
	g_pApp->m_grpCache.add(chatGrp);//加入到内存中

	{
		ChtC_CreateGroup_Rsp rsp;
		rsp.result = 0;
		m_chatAcct->send(s_nChtC_CreateGroup_Rsp, &rsp, sizeof(rsp));
	}
	
	m_chatAcct->joinGroup(chatGrp);
	return TRUE;
}

void DB_CreateGroup::reset()
{
	KSqlTask::reset();
	m_result = 0;
	m_grpID = 0;
	this->setAcct(NULL, NULL);
}

void DB_CreateGroup::destroy()
{
	this->reset();
	DB_CreateGroup::Free(this);
}
