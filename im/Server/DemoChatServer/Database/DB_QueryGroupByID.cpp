#include "DB_QueryGroupByID.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "../Cache/KGroupCache.h"
#include <JgErrorCode.h>

DB_QueryGroupByID::DB_QueryGroupByID():m_result(0),m_grpID(0),m_chatAcct(NULL),m_nextRequest(NULL)
{

}

DB_QueryGroupByID::~DB_QueryGroupByID()
{

}

void DB_QueryGroupByID::setAcct(KChatAccount* chatAcct, const UINT64 GrpID)
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
	  if(GrpID>0)
	  {
		  m_grpID=GrpID;
	  }
}

BOOL DB_QueryGroupByID::checkDepends()
{
	return TRUE;
}

BOOL DB_QueryGroupByID::execute(KSqlContext& ctx)//从数据库中查找群
{


	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_QueryGroupByID(?)";
	KSQLFieldDescribe params[1];
	params[0].m_name = "grpID";
	params[0].m_cdt = KSQLTypes::sql_c_uint64;
	params[0].m_length = 64;

	if(!cmd.SetSQLStatement(sql))
	{
		m_result=JgErrorCode::err_SqlStatement;
		return FALSE;
	}
	if(!cmd.DescribeParameters(params, 1))
	{
		m_result =JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetInt64(0,m_grpID))
	{
		m_result =JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_LoadGroups, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!rs.Begin())
	{
		m_result=JgErrorCode::err_Group_NotFound;
		return TRUE;
	}

	char nick[64];			int len3;
	char signature[256];	int len4;
	rs.GetInt64(0, *(INT64*)&m_chatGrp.m_groupID);
	len3 = sizeof(nick); 
	rs.GetString(1, nick, len3); 
	m_chatGrp.m_nick = nick;
	len4 = sizeof(signature); 
	rs.GetString(2, signature, len4);
	m_chatGrp.m_signature = signature;
	return TRUE;
}
void DB_QueryGroupByID::setNextRequest(JgMessageRequest* request)//设置下一个请求任务
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

BOOL DB_QueryGroupByID::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_QueryGroupByID::onFinished acct:%llu result:%d",m_chatAcct->m_acctID, m_result);
	KChatGroup* chatGrp;
	chatGrp = KChatGroup::Alloc(); chatGrp->reset();
	chatGrp->copy(m_chatGrp);
	if(!g_pApp->m_grpCache.add(chatGrp))//如果加入内存失败
	{
		KChatGroup::Free(chatGrp);
	}

	if(m_nextRequest)
	{
		g_pApp->m_requestManager.processRequest(m_nextRequest);
		delete m_nextRequest;
		m_nextRequest = NULL;
	}

	return TRUE;
}

void DB_QueryGroupByID::reset()
{
	KSqlTask::reset();
	m_result =JgErrorCode::err_Success;
	this->setAcct(NULL,NULL);
}

void DB_QueryGroupByID::destroy()
{
	this->reset();
	DB_QueryGroupByID::Free(this);
}
