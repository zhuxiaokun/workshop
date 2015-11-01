#include "DB_RemoveFriend.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include<JgErrorCode.h>
DB_RemoveFriend::DB_RemoveFriend():m_result(0),m_chatAcct(NULL),m_friend(NULL)
{

}

DB_RemoveFriend::~DB_RemoveFriend()
{

}

void DB_RemoveFriend::setAcct(KChatAccount* chatAcct, KChatAccount* frd)
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
	if(m_friend != frd)
	{
		if(m_friend)
		{
			m_friend->releaseRef();
			m_friend = NULL;
		}
		if(frd)
		{
			frd->addRef();
			m_friend = frd;
		}
	}
}

BOOL DB_RemoveFriend::checkDepends()
{
	return TRUE;
}

BOOL DB_RemoveFriend::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_RemoveFriend(?,?)";

	KSQLFieldDescribe params[2];
	params[0].m_name = "id";  params[0].m_cdt = KSQLTypes::sql_c_uint64;
	params[1].m_name = "frd"; params[1].m_cdt = KSQLTypes::sql_c_uint64;

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
	if(!cmd.SetInt64(1, m_friend->m_acctID))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_RemoveFriend, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!rs.Begin())
	{
		Log(LOG_ERROR, "error: DB_RemoveFriend, %s", cmd.GetLastErrorMsg());
		m_result =JgErrorCode::err_RemoveFriend;
		return TRUE;
	}

	rs.GetInteger(0, m_result);
	return TRUE;
}

BOOL DB_RemoveFriend::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_RemoveFriend::onFinished acct:%llu friend:%llu result:%d",
		m_chatAcct->m_acctID, m_friend->m_acctID, m_result);

	if(m_result)
	{
		Log(LOG_ERROR, "error: DB_RemoveFriend acct:%llu %llu result %d", m_chatAcct->m_acctID, m_friend->m_acctID, m_result);
		return FALSE;
	}
	
	return TRUE;
}

void DB_RemoveFriend::reset()
{
	KSqlTask::reset();
	m_result = 0;
	this->setAcct(NULL, NULL);
}

void DB_RemoveFriend::destroy()
{
	this->reset();
	DB_RemoveFriend::Free(this);
}
