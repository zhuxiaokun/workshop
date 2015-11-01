#include "DB_SetAccountAvatar.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "../Cache/KGroupCache.h"
#include "DB_LoadGroups.h"
#include <JgErrorCode.h>

DB_SetAccountAvatar::DB_SetAccountAvatar():m_result(0),m_chatAcct(NULL)
{

}

DB_SetAccountAvatar::~DB_SetAccountAvatar()
{

}

void DB_SetAccountAvatar::setAcct(KChatAccount* chatAcct)
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
}

BOOL DB_SetAccountAvatar::checkDepends()
{
	return TRUE;
}

BOOL DB_SetAccountAvatar::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_SetAccountAvatar(?,?)";

	KSQLFieldDescribe params[2];
	
	params[0].m_name = "acctID";
	params[0].m_cdt = KSQLTypes::sql_c_uint64;
	params[1].m_name = "avatar";
	params[1].m_cdt = KSQLTypes::sql_c_string;
	params[1].m_length = 32;

	if(!cmd.SetSQLStatement(sql))
	{
		m_result = JgErrorCode::err_SqlStatement;
		return FALSE;
	}
	if(!cmd.DescribeParameters(params, 2))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetInt64(0, m_chatAcct->m_acctID))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetString(1, m_chatAcct->m_avatar.c_str()))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result =JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_SetAccountAvatar, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!rs.Begin())
	{
		m_result = JgErrorCode::err_AvatarNotSet;
		return TRUE;
	}

	m_result = 0;
	return TRUE;
}

BOOL DB_SetAccountAvatar::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_SetAccountAvatar::onFinished acct:%s result:%d", m_chatAcct->toString().c_str(), m_result);
	return TRUE;
}

void DB_SetAccountAvatar::reset()
{
	KSqlTask::reset();
	m_result = 0;
	this->setAcct(NULL);
}

void DB_SetAccountAvatar::destroy()
{
	this->reset();
	DB_SetAccountAvatar::Free(this);
}
