#include "DB_LoadUnreadGroupMessages.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "DB_LoadGroups.h"
#include <JgErrorCode.h>
#include <KGlobalFunction.h>
#include "../Storage/FS_LoadUnreadMessages.h"

DB_LoadUnreadGroupMessages::DB_LoadUnreadGroupMessages():m_result(0),m_chatAcct(NULL)
{
	memset(&m_lastGroupMessageID, 0, sizeof(m_lastGroupMessageID));
}

DB_LoadUnreadGroupMessages::~DB_LoadUnreadGroupMessages()
{

}

void DB_LoadUnreadGroupMessages::setAcct(KChatAccount* chatAcct)
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

void DB_LoadUnreadGroupMessages::addAcctGroups(KChatAccount* chatAcct)
{
	if(chatAcct)
	{
		int n = chatAcct->m_groups.size();
		for(int i=0; i<n; i++)
		{
			KChatGroup* grp = (KChatGroup*)chatAcct->m_groups.at(i);
			m_groupIDs.push_back(grp->m_groupID);
		}
	}
}

void DB_LoadUnreadGroupMessages::addTargetGroup(UINT64 grpID)
{
	m_groupIDs.push_back(grpID);
}

BOOL DB_LoadUnreadGroupMessages::checkDepends()
{
	return m_chatAcct->isSocketReady();
}

BOOL DB_LoadUnreadGroupMessages::execute(KSqlContext& ctx)
{
	if(m_groupIDs.empty()) return TRUE;
	UINT64 grpID = m_groupIDs.pop_back();
	if(!this->_loadGroupMessages(ctx, grpID)) return FALSE;
	return TRUE;
}

BOOL DB_LoadUnreadGroupMessages::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_LoadUnreadGroupMessages::onFinished acct:%llu result:%d",
		m_chatAcct->m_acctID, m_result);

	if(!m_chatAcct->isSocketReady() || m_result)
	{
		m_chatAcct->clearFlag(KChatAccount::loading_unread_messages);
		return TRUE;
	}

	if(!m_groupIDs.empty())
	{
		DB_LoadUnreadGroupMessages* task = DB_LoadUnreadGroupMessages::Alloc(); task->reset();
		task->setAcct(m_chatAcct);
		task->m_lastGroupMessageID = m_lastGroupMessageID;
		while(!m_groupIDs.empty()) task->addTargetGroup(m_groupIDs.pop_back());
		while(!m_messageList.empty()) task->m_messageList.push_back(m_messageList.pop_front());
		g_pApp->m_databaseAgent.push(*task);
	}
	else
	{
		if(m_messageList.empty())
		{
			m_chatAcct->clearFlag(KChatAccount::loading_unread_messages);
			return TRUE;
		}

		bool hasPersonMessage = false;
		bool hasGroupMessage = false;

		FS_LoadUnreadMessages* task = FS_LoadUnreadMessages::Alloc(); task->reset();
		task->setChatAcct(m_chatAcct);
		while(!m_messageList.empty())
		{
			MessageInformation* minfo = m_messageList.pop_front();
			task->m_messageList.push_back(minfo);
			switch(minfo->target.targetType)
			{
			case JgMessageTargetType::PERSON: hasPersonMessage = true; break;
			case JgMessageTargetType::GROUP: hasGroupMessage = true; break;
			}
		}
		g_pApp->m_storageAgent.Push(task);

		if(!hasGroupMessage)
		{
			if(m_lastGroupMessageID.yyyy_mm_dd)
				m_chatAcct->setLastGroupMessage(m_lastGroupMessageID);
			else
				m_chatAcct->setLastGroupMessage(g_pApp->lastMessageID());
		}
	}

	return TRUE;
}

void DB_LoadUnreadGroupMessages::reset()
{
	this->setAcct(NULL);
	m_groupIDs.clear();
	while(!m_messageList.empty()) MessageInformation::Free(m_messageList.pop_front());
	KSqlTask::reset();
}

void DB_LoadUnreadGroupMessages::destroy()
{
	this->reset();
	DB_LoadUnreadGroupMessages::Free(this);
}

bool DB_LoadUnreadGroupMessages::_loadGroupMessages(KSqlContext& ctx, UINT64 grpID)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_Load_UnreadGroupMessages(?,?)";

	KSQLFieldDescribe params[2];

	params[0].m_name = "grpID";
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
	if(!cmd.SetInt64(0, grpID))
	{
		m_result = JgErrorCode::err_SqlParams;
		return false;
	}

	if(!cmd.SetInt64(1, MessageID2Int64(m_lastGroupMessageID)))
	{
		m_result = JgErrorCode::err_SqlParams;
		return false;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_LoadUnreadGroupMessages, %s", cmd.GetLastErrorMsg());
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
	INT64 groupID;

	do
	{
		rs.GetInt64(0, messageID);
		rs.GetInt64(1, senderID);
		rs.GetInteger(2, contentType);
		int len = sizeof(messageFile); rs.GetString(3, messageFile, len);
		rs.GetInt64(4, recvTime);
		rs.GetInt64(5, groupID);

		MessageInformation* minfo = MessageInformation::Alloc(); memset(minfo, 0, sizeof(MessageInformation));
		minfo->messageID = Int642MessageID(messageID);
		minfo->source.sourceType = JgMessageTargetType::PERSON;
		minfo->source.sourceID = senderID;
		minfo->target.targetType = JgMessageTargetType::GROUP;
		minfo->target.targetID = groupID;
		minfo->contentType = (JgMessageContentType::Type)contentType;
		strcpy_k(minfo->filePath, sizeof(minfo->filePath), messageFile);
		minfo->messageTime = recvTime;

		m_messageList.push_back(minfo);
	} while(rs.Next());

	return true;
}
