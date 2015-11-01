#include "DB_AddFriend.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "DB_LoadGroups.h"
#include<JgErrorCode.h>
DB_AddFriend::DB_AddFriend():m_result(0),m_chatAcct(NULL),m_friend(NULL)
{

}

DB_AddFriend::~DB_AddFriend()
{

}

void DB_AddFriend::setAcct(KChatAccount* acct_a, KChatAccount* acct_b)
{
	if(m_chatAcct != acct_a)
	{
		if(m_chatAcct)
		{
			m_chatAcct->releaseRef();
			m_chatAcct = NULL;
		}
		if(acct_a)
		{
			acct_a->addRef();
			m_chatAcct = acct_a;
		}
	}
	if(m_friend != acct_b)
	{
		if(m_friend)
		{
			m_friend->releaseRef();
			m_friend = NULL;
		}
		if(acct_b)
		{
			acct_b->addRef();
			m_friend = acct_b;
		}
	}
}

BOOL DB_AddFriend::checkDepends()
{
	return TRUE;
}

BOOL DB_AddFriend::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_AddFriend(?,?)";
	KSQLFieldDescribe params[2];
	params[0].m_name = "id1";
	params[0].m_cdt = KSQLTypes::sql_c_uint64;
	params[1].m_name = "id2";
	params[1].m_cdt = KSQLTypes::sql_c_uint64;

	if(!cmd.SetSQLStatement(sql))
	{
		m_result =JgErrorCode::err_SqlStatement;
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
	if(!cmd.SetInt64(1, m_friend->m_acctID))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_AddFriend, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!rs.Begin())
	{
		Log(LOG_ERROR, "error: DB_AddFriend, %s", cmd.GetLastErrorMsg());
		m_result = 0;
		return TRUE;
	}

	rs.GetInteger(0, m_result);
	return TRUE;
}

BOOL DB_AddFriend::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_AddFriend::onFinished acct:%llu friend:%llu result:%d",
		m_chatAcct->m_acctID, m_friend->m_acctID, m_result);

	if(m_result)
		Log(LOG_ERROR, "error: addFriend '%s' and '%s'", m_chatAcct->m_acctName.c_str(), m_friend->m_acctName.c_str());
	return TRUE;
}

void DB_AddFriend::reset()
{
	KSqlTask::reset();
	m_result = 0;
	this->setAcct(NULL, NULL);
}

void DB_AddFriend::destroy()
{
	this->reset();
	DB_AddFriend::Free(this);
}
