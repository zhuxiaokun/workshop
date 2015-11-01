#include "DB_QueryAccountByNick.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "../Cache/KGroupCache.h"
#include <JgErrorCode.h>

DB_QueryAccountByNick::DB_QueryAccountByNick():m_result(0),m_chatAcct(NULL)
{

}

DB_QueryAccountByNick::~DB_QueryAccountByNick()
{

}

void DB_QueryAccountByNick::setAcct(KChatAccount* chatAcct, const char* Nick)
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
	strcpy_k(m_nick, sizeof(m_nick),Nick);
}

BOOL DB_QueryAccountByNick::checkDepends()
{
	return TRUE;
}

BOOL DB_QueryAccountByNick::execute(KSqlContext& ctx)//从数据库中查找群
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call pro_QueryAccountByNick(?)";
	KSQLFieldDescribe params[1];
	params[0].m_name = "nick";
	params[0].m_cdt = KSQLTypes::sql_c_string;
   params[0].m_length = 64;
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
	if(!cmd.SetString(0,m_nick.c_str()))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_QueryAccountByNick, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!rs.Begin())
	{
		m_result = 0;
		return TRUE;
	}
	
	char acctName[32];		int len1;
	char nick[64];			int len3;
	char signature[256];	int len4;
	char avatar[32];	int len5;
	do
	{
		ChatAccount* member = ChatAccount::Alloc();
		rs.GetInt64(0, *(INT64*)&member->m_acctID);
		len1 = sizeof(acctName); rs.GetString(1, acctName, len1); member->m_acctName = acctName;
		len3 = sizeof(nick); rs.GetString(2, nick, len3); member->m_nick = nick;
		len4 = sizeof(signature); rs.GetString(3, signature, len4); member->m_signature = signature;
		len5 = sizeof(avatar); rs.GetString(4, avatar, len5); member->m_avatar = avatar;
		m_members.push_back(member);
	} while(rs.Next());

	return TRUE;
}
BOOL DB_QueryAccountByNick::onFinished()
{
	
	Log(LOG_DEBUG, "debug: DB_QueryAccountByNick::onFinished  result:%d",m_result);

	if(m_result)
	{
		Log(LOG_ERROR, "error:  DB_QueryAccountByNick result:%d", m_result);
	}
	KBufferOutputStream<1024*32> so;
	int count=m_members.size();
	so.WriteShort(count);
	while(!m_members.empty())
	{
		ChatAccount* chatAcct = m_members.pop_front();
		ChtC_AcctListInfo::AcctMember  mem;
		mem.passport=chatAcct->m_acctID;
		strcpy_k(mem.nick, sizeof(mem.nick), chatAcct->m_nick.c_str());
		strcpy_k(mem.signature, sizeof(mem.signature), chatAcct->m_signature.c_str());
		strcpy_k(mem.avatar, sizeof(mem.avatar), chatAcct->m_avatar.c_str());
		so.WriteByteArray(&mem,sizeof(mem));
		ChatAccount::Free(chatAcct);
	}
	if(m_chatAcct->isSocketReady())
     m_chatAcct->send(s_nChC_QueryAccountByNick_Rsp,so.data(),so.size());
	return TRUE;
}

void DB_QueryAccountByNick::reset()
{
	KSqlTask::reset();
	m_result =JgErrorCode::err_Success;
	while(!m_members.empty())
	{
		ChatAccount * chatAcc=m_members.pop_front();
		ChatAccount::Free(chatAcc);
	}
	this->setAcct(NULL,NULL);
}

void DB_QueryAccountByNick::destroy()
{
	this->reset();
	DB_QueryAccountByNick::Free(this);
}
