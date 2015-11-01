#include "DB_LoadGroups.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "../Cache/KGroupCache.h"
#include<JgErrorCode.h>
DB_LoadGroups::DB_LoadGroups():m_result(0),m_chatAcct(NULL)
{

}

DB_LoadGroups::~DB_LoadGroups()
{

}

void DB_LoadGroups::setAcct(KChatAccount* chatAcct)
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
	}
}

BOOL DB_LoadGroups::checkDepends()
{
	return TRUE;
}

BOOL DB_LoadGroups::execute(KSqlContext& ctx)//¼ÓÔØÈº
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_LoadGroups(?)";
	KSQLFieldDescribe params[1];
	params[0].m_name = "acctID";
	params[0].m_cdt = KSQLTypes::sql_c_uint64;

	if(!cmd.SetSQLStatement(sql))
	{
		m_result =JgErrorCode::err_SqlStatement;
		return FALSE;
	}
	if(!cmd.DescribeParameters(params, 1))
	{
		m_result =JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetInt64(0, m_chatAcct->m_acctID))
	{
		m_result = JgErrorCode::err_SqlParams;
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
		m_result = 0;
		return TRUE;
	}

	char nick[64];			int len3;
	char signature[256];	int len4;
	do
	{
		ChatGroup* grp = ChatGroup::Alloc();
		rs.GetInt64(0, *(INT64*)&grp->m_groupID);
		len3 = sizeof(nick); rs.GetString(1, nick, len3); grp->m_nick = nick;
		len4 = sizeof(signature); rs.GetString(2, signature, len4); grp->m_signature = signature;
		m_groups.push_back(grp);
	} while(rs.Next());
	return TRUE;
}

BOOL DB_LoadGroups::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_LoadGroups::onFinished acct:%llu result:%d",
		m_chatAcct->m_acctID, m_result);

	m_chatAcct->setFlag(KChatAccount::groups_loaded);

	if(m_result)
	{
		m_chatAcct->closeSocket();
		return FALSE;
	}

	m_chatAcct->m_state = KChatAccount::ready;
	while(!m_groups.empty())
	{
		ChatGroup* grp = m_groups.pop_front();
		UINT64 grpID = grp->m_groupID;
		KChatGroup* chatGrp = g_pApp->m_grpCache.find(grpID);
		if(!chatGrp)
		{
			chatGrp = KChatGroup::Alloc(); chatGrp->reset();
			chatGrp->copy(*grp);
			g_pApp->m_grpCache.add(chatGrp);
		}
		m_chatAcct->m_groups.insert_unique(chatGrp);
		chatGrp->m_members.insert_unique(m_chatAcct);
		chatGrp->loadDetail(NULL);
		ChatGroup::Free(grp);
	}

	m_chatAcct->sendGroupsInfo();
	return TRUE;
}

void DB_LoadGroups::reset()
{
	KSqlTask::reset();
	m_result = 0;
	this->setAcct(NULL);
	while(!m_groups.empty())
	{
		ChatGroup* grp = m_groups.pop_front();
		ChatGroup::Free(grp);
	}
}

void DB_LoadGroups::destroy()
{
	this->reset();
	DB_LoadGroups::Free(this);
}
