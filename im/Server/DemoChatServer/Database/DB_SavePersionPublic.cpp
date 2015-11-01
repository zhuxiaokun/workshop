#include "DB_SavePersionPublic.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "../Cache/KGroupCache.h"
#include "DB_LoadGroups.h"
#include <JgErrorCode.h>
#include <KGlobalFunction.h>

DB_SavePersonPublic::DB_SavePersonPublic()
{
	m_result = 0;
	m_senderID = 0;
	m_message = NULL;
}

DB_SavePersonPublic::~DB_SavePersonPublic()
{

}

void DB_SavePersonPublic::setMessage(KChatMessage* message)
{
	m_message = message;
	if(m_message)
	{
		m_senderID = m_message->m_sender->m_acctID;
	}
}

BOOL DB_SavePersonPublic::checkDepends()
{
	return TRUE;
}

BOOL DB_SavePersonPublic::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_SavePersonPublicMessage(?,?,?,?,?)";
	KSQLFieldDescribe params[5];

	params[0].m_name = "mid";
	params[0].m_cdt = KSQLTypes::sql_c_uint64;
	params[1].m_name = "senderID";
	params[1].m_cdt = KSQLTypes::sql_c_uint64;
	params[2].m_name = "contentType";
	params[2].m_cdt = KSQLTypes::sql_c_int32;
	params[3].m_name = "msgFile";
	params[3].m_cdt = KSQLTypes::sql_c_string;
	params[3].m_length = 32;
	params[4].m_name = "recvTime";
	params[4].m_cdt = KSQLTypes::sql_c_uint64;

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
	int namelen = sprintf_k(fileName, sizeof(fileName), "public/%08d/person/%llu/%u",
		m_message->m_messageID.yyyy_mm_dd,
		m_message->m_target.targetID,
		m_message->m_messageID.sequence);

	m_messageFile.assign(fileName, namelen);

	if(    !cmd.SetInt64  (0, MessageID2Int64(m_message->m_messageID))
		|| !cmd.SetInt64  (1, m_senderID)
		|| !cmd.SetInteger(2, m_message->m_contentType)
		|| !cmd.SetString (3, fileName, namelen)
		|| !cmd.SetInt64  (4, time(NULL)))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_SavePersonPublic, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	return TRUE;
}

BOOL DB_SavePersonPublic::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_SavePersonPublic::onFinished acct:%llu result:%d", m_senderID, m_result);
	if(!m_result)
	{
		
	}
	return TRUE;
}

void DB_SavePersonPublic::reset()
{
	KSqlTask::reset();
	m_result = 0;
	m_senderID = 0;
	m_messageFile.clear();
	m_message = NULL;
}

void DB_SavePersonPublic::destroy()
{
	this->reset();
	DB_SavePersonPublic::Free(this);
}
