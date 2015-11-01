#include "DB_JoinGroup.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "../Cache/KGroupCache.h"
#include "DB_LoadGroups.h"
#include<JgErrorCode.h>
DB_JoinGroup::DB_JoinGroup():m_result(0),m_chatAcct(NULL),m_chatGrp(NULL)
{

}

DB_JoinGroup::~DB_JoinGroup()
{

}

void DB_JoinGroup::setAcct(KChatAccount* chatAcct, KChatGroup* chatGrp)
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
	if(m_chatGrp != chatGrp)
	{
		if(m_chatGrp)
		{
			m_chatGrp->releaseRef();
			m_chatGrp = NULL;
		}
		if(chatGrp)
		{
			chatGrp->addRef();
			m_chatGrp = chatGrp;
		}
	}
}

BOOL DB_JoinGroup::checkDepends()
{
	return TRUE;
}

BOOL DB_JoinGroup::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_JoinGroup(?,?)";
	KSQLFieldDescribe params[2];
	params[0].m_name = "acctID";
	params[0].m_cdt = KSQLTypes::sql_c_uint64;
	params[1].m_name = "grpID";
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
		m_result =JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetInt64(1, m_chatGrp->m_groupID))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_JoinGroup, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!rs.Begin())
	{
		m_result =JgErrorCode::err_Success;
		return TRUE;
	}
	rs.GetInteger(0, m_result);
	return TRUE;
}

BOOL DB_JoinGroup::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_JoinGroup::onFinished acct:%llu grp:%llu result:%d",m_chatAcct->m_acctID, m_chatGrp->m_groupID, m_result);

	if(m_result)
	{
		Log(LOG_ERROR, "error: JoinGroup '%s' and '%s'", m_chatAcct->m_acctName.c_str(), m_chatGrp->m_nick.c_str());
		return FALSE;
	}

	return TRUE;
}

void DB_JoinGroup::reset()
{
	KSqlTask::reset();
	m_result = 0;
	this->setAcct(NULL, NULL);
}

void DB_JoinGroup::destroy()
{
	this->reset();
	DB_JoinGroup::Free(this);
}
