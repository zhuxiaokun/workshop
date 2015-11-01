#include "DB_Authenticate.h"
#include <System/Misc/StrUtil.h>
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "DB_LoadFriends.h"
#include <JgErrorCode.h>

DB_Authenticate::DB_Authenticate():m_result(0),m_chatAcct(NULL)
{

}

DB_Authenticate::~DB_Authenticate()
{

}

void DB_Authenticate::setAcct(KChatAccount* chatAcct)
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

BOOL DB_Authenticate::checkDepends()
{
	return m_chatAcct->isSocketReady();
}

BOOL DB_Authenticate::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	char sql[] = "call proc_AuthenticateAccount(?,?)";

	if(!cmd.SetSQLStatement(sql))
	{
		Log(LOG_ERROR, "error: DB_Authenticate, %s", cmd.GetLastErrorMsg());
		m_result = JgErrorCode::err_SqlStatement;
		return FALSE;
	}

	KSQLFieldDescribe params[2];
	params[0].m_name = "acct";
	params[0].m_cdt = KSQLTypes::sql_c_string;
	params[0].m_length = 32;
	params[1].m_name = "pwd";
	params[1].m_cdt = KSQLTypes::sql_c_string;
	params[1].m_length= 32;

	if(!cmd.DescribeParameters(params, 2))
	{
		Log(LOG_ERROR, "error: DB_Authenticate describe parameters");
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	char buf[64];
	tohexstr(m_chatAcct->m_password, sizeof(m_chatAcct->m_password), buf, sizeof(buf));

	cmd.SetString(0, m_chatAcct->m_acctName.c_str());
	cmd.SetString(1, buf);

	if(!cmd.ExecuteQuery(rs))
	{
		Log(LOG_ERROR, "error: DB_Authenticate %s", cmd.GetLastErrorMsg());
		m_result = JgErrorCode::err_SqlExecute;
		return FALSE;
	}

	rs.SetColumnLength(2, 32);
	rs.SetColumnLength(3, 256);
	rs.SetColumnLength(4, 32);

	if(!rs.Begin())
	{
		m_result = JgErrorCode::err_AccountNotFound;
		return FALSE;
	}
	
	rs.GetInteger(0, m_result);
	if(m_result) return TRUE;

	rs.GetInt64(1, *(INT64*)&m_acctInfo.m_acctID);

	char nick[64]; int nicklen = sizeof(nick);
	rs.GetString(2, nick, nicklen);
	m_acctInfo.m_nick = nick;

	char signature[256]; int len2 = sizeof(signature);
	rs.GetString(3, signature, len2);
	m_acctInfo.m_signature = signature;

	char avatar[32]; len2 = sizeof(avatar);
	rs.GetString(4, avatar, len2);
	m_acctInfo.m_avatar = avatar;

	return TRUE;
}

BOOL DB_Authenticate::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_Authenticate::onFinished acct:%llu result:%d",
		m_chatAcct->m_acctID, m_result);

	{
		ChtC_LoginRsp rsp;
		rsp.result = m_result;
		m_chatAcct->send(s_nChtC_LoginRsp, &rsp, sizeof(rsp));
	}
	
	if(m_result)
	{
		m_chatAcct->closeSocket(5);
		this->setAcct(NULL);
		return TRUE;
	}

	g_pApp->m_candidateManager.remove(m_chatAcct);

	m_chatAcct->m_acctID = m_acctInfo.m_acctID;
	m_chatAcct->m_nick = m_acctInfo.m_nick;
	m_chatAcct->m_signature = m_acctInfo.m_signature;
	m_chatAcct->m_avatar = m_acctInfo.m_avatar;

	{
		ChtC_AcctInfo rsp;
		rsp.passport = m_chatAcct->m_acctID;
		strcpy_k(rsp.nick, sizeof(rsp.nick), m_chatAcct->m_nick.c_str());
		strcpy_k(rsp.signature, sizeof(rsp.signature), m_chatAcct->m_signature.c_str());
		strcpy_k(rsp.avatar, sizeof(rsp.avatar), m_chatAcct->m_avatar.c_str());
		m_chatAcct->send(ChtC_AcctInfo::s_nCode, &rsp, sizeof(rsp));
	}

	KChatAccount* chatAcct = g_pApp->m_acctCache.find(m_chatAcct->m_acctID);
	if(chatAcct)
	{
		KSocket_Client* clnSocket = m_chatAcct->m_clnSocket;
		
		chatAcct->setSocket(clnSocket);
		this->setAcct(chatAcct);

		m_chatAcct->setSocket(NULL);
		KChatAccount::Free(m_chatAcct);
	}
	else
	{
		g_pApp->m_acctCache.add(m_chatAcct);
	}
	
	m_chatAcct->loadFriends();
	m_chatAcct->loadGroups();
	return TRUE;
}

void DB_Authenticate::reset()
{
	KSqlTask::reset();
	m_result = 0;
	memset(&m_acctInfo, 0, sizeof(m_acctInfo));
	this->setAcct(NULL);
}

void DB_Authenticate::destroy()
{
	this->reset();
	DB_Authenticate::Free(this);
}
