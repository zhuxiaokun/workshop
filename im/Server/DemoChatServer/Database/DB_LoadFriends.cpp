#include "DB_LoadFriends.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "DB_LoadGroups.h"
#include <JgErrorCode.h>

DB_LoadFriends::DB_LoadFriends():m_result(0),m_chatAcct(NULL)
{

}

DB_LoadFriends::~DB_LoadFriends()
{

}

void DB_LoadFriends::setAcct(KChatAccount* chatAcct)
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

BOOL DB_LoadFriends::checkDepends()
{
	return m_chatAcct->isSocketReady();
}

BOOL DB_LoadFriends::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;
	
	const char* sql = "call proc_LoadFriends(?)";
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
	if(!cmd.SetInt64(0, m_chatAcct->m_acctID))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_LoadFriends, %s", cmd.GetLastErrorMsg());
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
	char avatar[32];		int len5;
	do
	{
		ChatAccount* friendAcct = ChatAccount::Alloc();
		rs.GetInt64(0, *(INT64*)&friendAcct->m_acctID);
		len1 = sizeof(acctName); rs.GetString(1, acctName, len1); friendAcct->m_acctName = acctName;
		len2 = sizeof(pwd); rs.GetString(2, pwd, len2);
		reversehexstr(pwd, len2, &friendAcct->m_password, sizeof(friendAcct->m_password));
		len3 = sizeof(nick); rs.GetString(3, nick, len3); friendAcct->m_nick = nick;
		len4 = sizeof(signature); rs.GetString(4, signature, len4); friendAcct->m_signature = signature;
		len5 = sizeof(avatar); rs.GetString(5, avatar, len5); friendAcct->m_avatar = avatar;
		m_friends.push_back(friendAcct);
	} while(rs.Next());

	return TRUE;
}

BOOL DB_LoadFriends::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_LoadFriends::onFinished acct:%llu result:%d",
		m_chatAcct->m_acctID, m_result);

	m_chatAcct->setFlag(KChatAccount::friends_loaded);

	if(m_result)
	{
		m_chatAcct->closeSocket();
		return FALSE;
	}

	while(!m_friends.empty())
	{
		ChatAccount* acct = m_friends.pop_front();
		
		UINT64 acctID = acct->m_acctID;
		KChatAccount* chatAcct = g_pApp->m_acctCache.find(acctID);
		if(!chatAcct)
		{
			chatAcct = KChatAccount::Alloc(); chatAcct->reset();
			chatAcct->copy(*acct); chatAcct->m_state = KChatAccount::with_info;
			g_pApp->m_acctCache.add(chatAcct);
			m_chatAcct->m_friends.insert_unique(chatAcct);
		}
		else
		{
			chatAcct->copy(*acct);
			m_chatAcct->m_friends.insert_unique(chatAcct);
		}
		ChatAccount::Free(acct);
	}
	
	m_chatAcct->sendFriendsInfo();
	m_chatAcct->loadGroups();

	return TRUE;
}

void DB_LoadFriends::reset()
{
	KSqlTask::reset();
	this->setAcct(NULL);
	while(!m_friends.empty())
	{
		ChatAccount* ptr = m_friends.pop_front();
		ChatAccount::Free(ptr);
	}
}

void DB_LoadFriends::destroy()
{
	this->reset();
	DB_LoadFriends::Free(this);
}
