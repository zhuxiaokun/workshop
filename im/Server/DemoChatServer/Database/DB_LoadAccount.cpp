#include "DB_LoadAccount.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "DB_LoadGroups.h"
#include <JgErrorCode.h>

DB_LoadAccount::DB_LoadAccount():m_result(0),m_acctID(0),m_nextRequest(NULL)
{
	memset(&m_acctInfo, 0, sizeof(m_acctInfo));
}

DB_LoadAccount::~DB_LoadAccount()
{

}

void DB_LoadAccount::setNextRequest(JgMessageRequest* request)//设置下一个请求任务
{
	if(m_nextRequest == request) return;
	if(m_nextRequest)
	{
		delete m_nextRequest;
		m_nextRequest = NULL;
	}
	if(request)
	{
		m_nextRequest = request;
	}
}

BOOL DB_LoadAccount::checkDepends()
{
	return TRUE;
}

BOOL DB_LoadAccount::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;
	
	const char* sql = "call proc_LoadAccount(?)";
	KSQLFieldDescribe params[1];
	params[0].m_name = "acctID";
	params[0].m_cdt = KSQLTypes::sql_c_uint64;

	if(!cmd.SetSQLStatement(sql))
	{
		m_result = JgErrorCode::err_SqlStatement;
		return FALSE;
	}
	if(!cmd.DescribeParameters(params, 1))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetInt64(0, m_acctID))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_LoadAccount, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	rs.SetColumnLength(0, 32);
	rs.SetColumnLength(1, 64);
	rs.SetColumnLength(2, 64);
	rs.SetColumnLength(3, 256);
	rs.SetColumnLength(4, 32);

	if(!rs.Begin())
	{
		m_result = JgErrorCode::err_AccountNotFound;
		return TRUE;
	}

	char acctName[32];		int len1;
	char pwd[64];			int len2;
	char nick[64];			int len3;
	char signature[256];	int len4;
	char avatar[32];		int len5;

	m_acctInfo.m_acctID = m_acctID;
	ChatAccount* friendAcct = &m_acctInfo;

	len1 = sizeof(acctName);	rs.GetString(0, acctName, len1); friendAcct->m_acctName = acctName;
	len2 = sizeof(pwd);			rs.GetString(1, pwd, len2);
	len3 = sizeof(nick);		rs.GetString(2, nick, len3); friendAcct->m_nick = nick;
	len4 = sizeof(signature);	rs.GetString(3, signature, len4); friendAcct->m_signature = signature;
	len5 = sizeof(avatar);		rs.GetString(4, avatar, len5); friendAcct->m_avatar = avatar;
	reversehexstr(pwd, len2, &friendAcct->m_password, sizeof(friendAcct->m_password));
	return TRUE;
}




//数据库操作成功之后执行该操作
BOOL DB_LoadAccount::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_LoadAccount::onFinished acct:%llu result:%d", m_acctID, m_result);
	/*if(m_result) 
	{
		return FALSE;
	}
	*/
	KChatAccount* chatAcct = g_pApp->m_acctCache.find(m_acctID);
	if(!chatAcct&&!m_result)
	{
		chatAcct = KChatAccount::Alloc(); chatAcct->reset();
		chatAcct->copy(m_acctInfo); chatAcct->m_state = KChatAccount::with_info;
		g_pApp->m_acctCache.add(chatAcct);
	}
	
	if(m_nextRequest)
	{
		g_pApp->m_requestManager.processRequest(m_nextRequest);
		delete m_nextRequest; m_nextRequest = NULL;
	}
	return TRUE;
}




void DB_LoadAccount::reset()
{
	KSqlTask::reset();
	m_result = 0;
	m_acctID = 0;
	this->setNextRequest(NULL);
	memset(&m_acctInfo, 0, sizeof(m_acctInfo));
}

void DB_LoadAccount::destroy()
{
	this->reset();
	DB_LoadAccount::Free(this);
}
