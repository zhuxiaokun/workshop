#include "DB_LeaveGroup.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include<JgErrorCode.h>
DB_LeaveGroup::DB_LeaveGroup():m_result(0),m_grpID(0),m_chatAcct(NULL)
{

}

DB_LeaveGroup::~DB_LeaveGroup()
{

}

void DB_LeaveGroup::setAcct(UINT64 grpID, KChatAccount* chatAcct)
{
	m_grpID = grpID;
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
	}
}

BOOL DB_LeaveGroup::checkDepends()
{
	return TRUE;
}

BOOL DB_LeaveGroup::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_LeaveGroup(?,?)";
	KSQLFieldDescribe params[2];
	params[0].m_name = "acctID";
	params[0].m_cdt = KSQLTypes::sql_c_uint64;
	params[1].m_name = "grpID";
	params[1].m_cdt = KSQLTypes::sql_c_uint64;

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
		m_result =JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetInt64(1, m_grpID))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_LeaveGroup, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!rs.Begin())
	{
		m_result = 0;
		return TRUE;
	}

	rs.GetInteger(0, m_result);
	return TRUE;
}

BOOL DB_LeaveGroup::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_LeaveGroup::onFinished acct:%llu grp:%llu result:%d",
		m_chatAcct->m_acctID, m_grpID, m_result);

	if(m_result)
	{
		Log(LOG_ERROR, "error: DB_LeaveGroup result:%d", m_result);
		return FALSE;
	}

	KChatGroup* chatGrp = g_pApp->m_grpCache.find(m_grpID);
	if(!chatGrp) return TRUE;

	return TRUE;
}

void DB_LeaveGroup::reset()
{
	KSqlTask::reset();
	m_result = 0;
	m_grpID = 0;
	this->setAcct(0, NULL);
}

void DB_LeaveGroup::destroy()
{
	this->reset();
	DB_LeaveGroup::Free(this);
}
