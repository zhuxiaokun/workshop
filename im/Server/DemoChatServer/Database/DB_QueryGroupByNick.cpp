#include "DB_QueryGroupByNick.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "../Cache/KGroupCache.h"
#include <JgErrorCode.h>

DB_QueryGroupByNick::DB_QueryGroupByNick():m_result(0),m_grpID(0),m_chatAcct(NULL),m_grpNick(NULL)
{

}

DB_QueryGroupByNick::~DB_QueryGroupByNick()
{

}

void DB_QueryGroupByNick::setAcct(KChatAccount* chatAcct, const char* grpNick)
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
	strcpy_k(m_grpNick, sizeof(m_grpNick),grpNick);
}

BOOL DB_QueryGroupByNick::checkDepends()
{
	return TRUE;
}

BOOL DB_QueryGroupByNick::execute(KSqlContext& ctx)//从数据库中查找群
{


	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_QueryGroupByNick(?)";
	KSQLFieldDescribe params[1];
	params[0].m_name = "nick";
	params[0].m_cdt = KSQLTypes::sql_c_string;
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
	if(!cmd.SetString(0, m_grpNick.c_str()))
	{
		m_result =JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_QueryGroupsByNick, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	if(!rs.Begin())
	{
		m_result=JgErrorCode::err_Group_NotFound;
		return TRUE;
	}

	   char nick[64];			int len3;
	   char signature[256];	int len4;

	   do{
		   ChatGroup * grp=ChatGroup::Alloc();
	        rs.GetInt64(0, *(INT64*)&grp->m_groupID);
		    len3 = sizeof(nick); 
		    rs.GetString(1, nick, len3); 
		    grp->m_nick = nick;
		   len4 = sizeof(signature); 
		   rs.GetString(2, signature, len4);
		   grp->m_signature = signature;
		   m_grpList.push_back(grp);
	   }while(rs.Next());
	   return TRUE;
}

BOOL DB_QueryGroupByNick::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_QueryGroupByNick::onFinished acct:%llu result:%d",m_chatAcct->m_acctID, m_result);

	if(m_result)
	{
		Log(LOG_ERROR, "error:  DB_QueryAccountByNick result:%d", m_result);
	}

	KBufferOutputStream<1024*32> so;
	int count=m_grpList.size();
	so.WriteShort(count);
	
	while(!m_grpList.empty())
	{
		ChatGroup * grp=m_grpList.pop_front();
		KChatGroup* chatGrp = g_pApp->m_grpCache.find(grp->m_groupID);
		if(!chatGrp)
		{
			chatGrp=KChatGroup::Alloc();
			chatGrp->reset();
			chatGrp->copy(*grp);
			g_pApp->m_grpCache.add(chatGrp);
		}
		ChatGroup::Free(grp);

		ChtC_GroupInfo::Group mem;
		mem.groupID=chatGrp->m_groupID;
		strcpy_k(mem.nick, sizeof(mem.nick), chatGrp->m_nick.c_str());
		strcpy_k(mem.signature, sizeof(mem.signature), chatGrp->m_signature.c_str());
		so.WriteByteArray(&mem,sizeof(mem));
	}
	if(m_chatAcct->isSocketReady())
	m_chatAcct->send(s_nChC_QueryGroupByNick_Rsp,so.data(),so.size());
	return TRUE;
}

void DB_QueryGroupByNick::reset()
{
	KSqlTask::reset();
	m_result =JgErrorCode::err_Success;

	while(!m_grpList.empty())
	{
		ChatGroup *  chatGrp=m_grpList.pop_front();
		ChatGroup::Free(chatGrp);
	}
	this->setAcct(NULL,NULL);
}

void DB_QueryGroupByNick::destroy()
{
	this->reset();
	DB_QueryGroupByNick::Free(this);
}
