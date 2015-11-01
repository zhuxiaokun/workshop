#include "DB_LoadUnreadMessages.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "DB_LoadGroups.h"
#include <JgErrorCode.h>
#include <KGlobalFunction.h>
#include "../Storage/FS_LoadUnreadMessages.h"
#include "DB_LoadUnreadGroupMessages.h"

DB_LoadUnreadMessages::DB_LoadUnreadMessages():m_result(0),m_chatAcct(NULL)
{
	memset(&m_lastPersonMessageID, 0, sizeof(m_lastPersonMessageID));
	memset(&m_lastGroupMessageID, 0, sizeof(m_lastGroupMessageID));
}

DB_LoadUnreadMessages::~DB_LoadUnreadMessages()
{

}

void DB_LoadUnreadMessages::setAcct(KChatAccount* chatAcct)
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

BOOL DB_LoadUnreadMessages::checkDepends()
{
	return m_chatAcct->isSocketReady();
}

BOOL DB_LoadUnreadMessages::execute(KSqlContext& ctx)
{
	if(!this->_loadPersonMessages(ctx)) return FALSE;
	return TRUE;
}

BOOL DB_LoadUnreadMessages::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_LoadUnreadMessages::onFinished acct:%llu result:%d",
		m_chatAcct->m_acctID, m_result);

	if(!m_chatAcct->isSocketReady() || m_result)
	{
		m_chatAcct->clearFlag(KChatAccount::loading_unread_messages);
		return TRUE;
	}

	DB_LoadUnreadGroupMessages* task = DB_LoadUnreadGroupMessages::Alloc(); task->reset();
	task->setAcct(m_chatAcct);
	task->addAcctGroups(m_chatAcct);
	task->m_lastGroupMessageID = m_lastGroupMessageID;
	while(!m_messageList.empty()) task->m_messageList.push_back(m_messageList.pop_front());
	g_pApp->m_databaseAgent.push(*task);

	return TRUE;
}

void DB_LoadUnreadMessages::reset()
{
	this->setAcct(NULL);
	while(!m_messageList.empty()) MessageInformation::Free(m_messageList.pop_front());
	KSqlTask::reset();
}

void DB_LoadUnreadMessages::destroy()
{
	this->reset();
	DB_LoadUnreadMessages::Free(this);
}

bool DB_LoadUnreadMessages::_loadPersonMessages(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_Load_PersonUnreadMessages(?,?)";

	KSQLFieldDescribe params[2];
	
	params[0].m_name = "acctID";
	params[0].m_cdt = KSQLTypes::sql_c_uint64;
	params[1].m_name = "mid";
	params[1].m_cdt = KSQLTypes::sql_c_uint64;

	if(!cmd.SetSQLStatement(sql))
	{
		m_result = JgErrorCode::err_SqlStatement;
		return false;
	}
	if(!cmd.DescribeParameters(params, 2))
	{
		m_result = JgErrorCode::err_SqlParams;
		return false;
	}
	if(!cmd.SetInt64(0, m_chatAcct->m_acctID))
	{
		m_result = JgErrorCode::err_SqlParams;
		return false;
	}
	if(!cmd.SetInt64(1, MessageID2Int64(m_lastPersonMessageID)))
	{
		m_result = JgErrorCode::err_SqlParams;
		return false;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_LoadUnreadMessages, %s", cmd.GetLastErrorMsg());
		return false;
	}

	rs.SetColumnLength(3, 128);

	if(!rs.Begin())
	{
		m_result = 0;
		return true;
	}

	INT64 messageID;
	INT64 senderID;
	int contentType;
	char messageFile[128];
	INT64 recvTime;

	do
	{
		rs.GetInt64(0, messageID);
		rs.GetInt64(1, senderID);
		rs.GetInteger(2, contentType);
		int len = sizeof(messageFile); rs.GetString(3, messageFile, len);
		rs.GetInt64(4, recvTime);

		MessageInformation* minfo = MessageInformation::Alloc(); memset(minfo, 0, sizeof(MessageInformation));
		minfo->messageID = Int642MessageID(messageID);
		minfo->source.sourceType = JgMessageTargetType::PERSON;
		minfo->source.sourceID = senderID;
		minfo->target = m_chatAcct->toMessageTarget();
		minfo->contentType = (JgMessageContentType::Type)contentType;
		strcpy_k(minfo->filePath, sizeof(minfo->filePath), messageFile);
		minfo->messageTime = recvTime;

		m_messageList.push_back(minfo);
	} while(rs.Next());

	return true;
}
