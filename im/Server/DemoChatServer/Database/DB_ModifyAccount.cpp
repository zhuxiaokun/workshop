#include "DB_ModifyAccount.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include<JgErrorCode.h>
DB_ModifyAccount::DB_ModifyAccount():m_result(0),m_chatAcct(NULL)
{

}

DB_ModifyAccount::~DB_ModifyAccount()
{

}

void DB_ModifyAccount::setAcct(KChatAccount* chatAcct, const ChatAccount* acctInfo)
{
	if(m_chatAcct == chatAcct) return;
	if(m_chatAcct)
	{
		m_chatAcct->releaseRef();
		m_chatAcct = NULL;
	}
	if(chatAcct)
	{
		chatAcct->addRef();
		m_chatAcct = chatAcct;
		memcpy(&m_acctInfo, acctInfo, sizeof(m_acctInfo));
	}
	else
	{
		memset(&m_acctInfo, 0, sizeof(m_acctInfo));
	}
}

BOOL DB_ModifyAccount::checkDepends()
{
	return TRUE;
}

BOOL DB_ModifyAccount::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_ModifyAccount(?,?,?)";

	KSQLFieldDescribe params[3];
	params[0].m_name = "id";
	params[0].m_cdt = KSQLTypes::sql_c_uint64;
	params[1].m_name = "nick";
	params[1].m_cdt = KSQLTypes::sql_c_string;
	params[1].m_length = 64;
	params[2].m_name = "sig";
	params[2].m_cdt = KSQLTypes::sql_c_string;
	params[2].m_length = 256;

	if(!cmd.SetSQLStatement(sql))
	{
		m_result = JgErrorCode::err_SqlStatement;
		return FALSE;
	}
	if(!cmd.DescribeParameters(params, 3))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetInt64(0, m_chatAcct->m_acctID))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetString(1, m_acctInfo.m_nick.c_str()))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetString(2, m_acctInfo.m_signature.c_str()))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_ModifyAccount, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!rs.Begin())
	{
		Log(LOG_ERROR, "error: DB_ModifyAccount, %s", cmd.GetLastErrorMsg());
		m_result = JgErrorCode::err_ModifyAccount;
		return TRUE;
	}

	rs.GetInteger(0, m_result);
	return TRUE;
}

BOOL DB_ModifyAccount::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_ModifyAccount::onFinished acct:%llu result:%d",
		m_chatAcct->m_acctID, m_result);

	if(m_result)
	{
		Log(LOG_ERROR, "error: DB_ModifyAccount acct:%llu result %d", m_chatAcct->m_acctID, m_result);
		return FALSE;
	}
	
	m_chatAcct->m_nick = m_acctInfo.m_nick;
	m_chatAcct->m_signature = m_acctInfo.m_signature;

	return TRUE;
}

void DB_ModifyAccount::reset()
{
	KSqlTask::reset();
	m_result = 0;
	this->setAcct(NULL, NULL);
}

void DB_ModifyAccount::destroy()
{
	this->reset();
	DB_ModifyAccount::Free(this);
}
