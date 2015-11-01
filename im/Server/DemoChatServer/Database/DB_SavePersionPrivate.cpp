#include "DB_SavePersionPrivate.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "../Cache/KGroupCache.h"
#include "DB_LoadGroups.h"
#include <JgErrorCode.h>
#include <KGlobalFunction.h>

DB_SavePersonPrivate::DB_SavePersonPrivate()
{
	m_result = 0;
	m_receiverID = 0;
	m_senderID = 0;
	m_message = NULL;
}

DB_SavePersonPrivate::~DB_SavePersonPrivate()
{

}

void DB_SavePersonPrivate::setMessage(KChatMessage* message)
{
	m_message = message;
	if(m_message)
	{
		m_receiverID = m_message->m_target.targetID;
		m_senderID = m_message->m_sender->m_acctID;
	}
}

BOOL DB_SavePersonPrivate::checkDepends()
{
	return TRUE;
}

BOOL DB_SavePersonPrivate::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_SavePersonPrivateMessage(?,?,?,?,?,?)";
	KSQLFieldDescribe params[6];

	params[0].m_name = "mid";
	params[0].m_cdt = KSQLTypes::sql_c_uint64;
	params[1].m_name = "recvID";
	params[1].m_cdt = KSQLTypes::sql_c_uint64;
	params[2].m_name = "senderID";
	params[2].m_cdt = KSQLTypes::sql_c_uint64;
	params[3].m_name = "contentType";
	params[3].m_cdt = KSQLTypes::sql_c_int32;
	params[4].m_name = "msgFile";
	params[4].m_cdt = KSQLTypes::sql_c_string;
	params[4].m_length = 32;
	params[5].m_name = "recvTime";
	params[5].m_cdt = KSQLTypes::sql_c_uint64;

	if(!cmd.SetSQLStatement(sql))
	{
		m_result = JgErrorCode::err_SqlStatement;
		return FALSE;
	}
	if(!cmd.DescribeParameters(params, 6))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	char fileName[256];
	int namelen = sprintf_k(fileName, sizeof(fileName), "private/%08d/person/%llu/%u",
		m_message->m_messageID.yyyy_mm_dd,
		m_message->m_target.targetID,
		m_message->m_messageID.sequence);

	m_messageFile.assign(fileName, namelen);

	if(    !cmd.SetInt64  (0, MessageID2Int64(m_message->m_messageID))
		|| !cmd.SetInt64  (1, m_receiverID)
		|| !cmd.SetInt64  (2, m_senderID)
		|| !cmd.SetInteger(3, m_message->m_contentType)
		|| !cmd.SetString (4, fileName, namelen)
		|| !cmd.SetInt64  (5, time(NULL)))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_SavePersonPrivate, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	return TRUE;
}

BOOL DB_SavePersonPrivate::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_SavePersonPrivate::onFinished acct:%llu result:%d", m_receiverID, m_result);
	if(m_result) { }
	return TRUE;
}

void DB_SavePersonPrivate::reset()
{
	KSqlTask::reset();
	m_result = 0;
	m_receiverID = 0;
	m_senderID = 0;
	m_messageFile.clear();
	m_message = NULL;
}

void DB_SavePersonPrivate::destroy()
{
	this->reset();
	DB_SavePersonPrivate::Free(this);
}
