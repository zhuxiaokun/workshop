#include "DB_LoadGroupDetail.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include <JgErrorCode.h>

DB_LoadGroupDetail::DB_LoadGroupDetail():m_result(0),m_grpID(0),m_chatAcct(NULL)
{

}

DB_LoadGroupDetail::~DB_LoadGroupDetail()
{

}

void DB_LoadGroupDetail::setAcct(UINT64 grpID, KChatAccount* chatAcct)
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

BOOL DB_LoadGroupDetail::checkDepends()
{
	return TRUE;
}

BOOL DB_LoadGroupDetail::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_LoadGroupDetail(?)";
	KSQLFieldDescribe params[1];
	params[0].m_name = "grpID";
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
	if(!cmd.SetInt64(0, m_grpID))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_LoadGroupDetail, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!rs.Begin())
	{
		m_result = 0;
		return TRUE;
	}

	char acctName[32];		int len1;
	char pwd[64];			int len2;
	char nick[64];			int len3;
	char signature[256];	int len4;
	char avatar[32];	int len5;
	do
	{
		ChatAccount* member = ChatAccount::Alloc(); member->clear();
		rs.GetInt64(0, *(INT64*)&member->m_acctID);
		len1 = sizeof(acctName); rs.GetString(1, acctName, len1); member->m_acctName = acctName;
		len2 = sizeof(pwd); rs.GetString(2, pwd, len2); reversehexstr(pwd, len2, &member->m_password, sizeof(member->m_password));
		len3 = sizeof(nick); rs.GetString(3, nick, len3); member->m_nick = nick;
		len4 = sizeof(signature); rs.GetString(4, signature, len4); member->m_signature = signature;
		len5 = sizeof(avatar); rs.GetString(5, avatar, len5); member->m_avatar = avatar;
		m_members.push_back(member);
	} while(rs.Next());

	return TRUE;
}

BOOL DB_LoadGroupDetail::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_LoadGroupDetail::onFinished grp:%llu result:%d", m_grpID, m_result);

	if(m_result)
	{
		Log(LOG_ERROR, "error: DB_LoadGroupDetail result:%d", m_result);
		return FALSE;
	}

	KChatGroup* chatGrp = g_pApp->m_grpCache.find(m_grpID);
	ASSERT(chatGrp);

	while(!m_members.empty())
	{
		ChatAccount* acct = m_members.pop_front();
		KChatAccount* chatAcct = g_pApp->m_acctCache.find(acct->m_acctID);
		if(!chatAcct)
		{
			chatAcct = KChatAccount::Alloc(); chatAcct->reset();
			chatAcct->copy(*acct); chatAcct->m_state = KChatAccount::with_info;
			g_pApp->m_acctCache.add(chatAcct);
		}
		else if(chatAcct->m_state == KChatAccount::authenticate)
		{
			chatAcct->copy(*acct);
		}
		chatGrp->m_members.insert_unique(chatAcct);
		ChatAccount::Free(acct);
	}

	chatGrp->setFlag(KChatGroup::detail_loaded);
	chatGrp->answerWaitDetailQueue();

	return TRUE;
}

void DB_LoadGroupDetail::reset()
{
	KSqlTask::reset();
	m_result = 0;
	m_grpID = 0;
	this->setAcct(0, NULL);
	while(!m_members.empty())
	{
		ChatAccount* acct = m_members.pop_front();
		ChatAccount::Free(acct);
	}
}

void DB_LoadGroupDetail::destroy()
{
	this->reset();
	DB_LoadGroupDetail::Free(this);
}
