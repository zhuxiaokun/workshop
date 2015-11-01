#include "DB_SaveMessage_2.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include "../Cache/KGroupCache.h"
#include "DB_LoadGroups.h"
#include <JgErrorCode.h>
#include <KGlobalFunction.h>

DB_SaveMessage_2::DB_SaveMessage_2()
{
	m_result = 0;
	memset(&m_messageID, 0, sizeof(m_messageID));
	memset(&m_messageSource, 0, sizeof(m_messageSource));
	memset(&m_messageTarget, 0, sizeof(m_messageTarget));
	m_contentType = JgMessageContentType::TEXT;
	m_messageTime = 0;
}

DB_SaveMessage_2::~DB_SaveMessage_2()
{

}

BOOL DB_SaveMessage_2::checkDepends()
{
	return TRUE;
}

BOOL DB_SaveMessage_2::execute(KSqlContext& ctx)
{
	switch(m_messageTarget.targetType)
	{
	case JgMessageTargetType::PERSON:
		return this->_savePersonPrivate(ctx);
	case JgMessageTargetType::GROUP:
		return this->_saveGroupPrivate(ctx);
	default: ASSERT(FALSE);
	}
	return TRUE;
}

BOOL DB_SaveMessage_2::onFinished()
{
	string_512 filepath = g_pApp->buildMessageShortFilePath(m_messageTarget, m_messageID);
	Log(LOG_DEBUG, "debug: DB_SaveMessage_2 finished, result:%d filepath:'%s'", m_result, filepath.c_str());
	return TRUE;
}

void DB_SaveMessage_2::reset()
{
	KSqlTask::reset();
	m_result = 0;
	memset(&m_messageID, 0, sizeof(m_messageID));
	memset(&m_messageSource, 0, sizeof(m_messageSource));
	memset(&m_messageTarget, 0, sizeof(m_messageTarget));
	m_contentType = JgMessageContentType::TEXT;
	m_messageTime = 0;
}

void DB_SaveMessage_2::destroy()
{
	this->reset();
	DB_SaveMessage_2::Free(this);
}

BOOL DB_SaveMessage_2::_savePersonPrivate(KSqlContext& ctx)
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

	string_512 messageFile = g_pApp->buildMessageShortFilePath(m_messageTarget, m_messageID);

	if(    !cmd.SetInt64  (0, MessageID2Int64(m_messageID))
		|| !cmd.SetInt64  (1, m_messageTarget.targetID)
		|| !cmd.SetInt64  (2, m_messageSource.sourceID)
		|| !cmd.SetInteger(3, m_contentType)
		|| !cmd.SetString (4, messageFile.c_str(), messageFile.size())
		|| !cmd.SetInt64  (5, m_messageTime))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: _savePersonPrivate, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	return TRUE;
}

BOOL DB_SaveMessage_2::_saveGroupPrivate(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_SaveGroupPrivateMessage(?,?,?,?,?,?)";
	KSQLFieldDescribe params[6];

	params[0].m_name = "mid";		// message ID
	params[0].m_cdt = KSQLTypes::sql_c_uint64;
	params[1].m_name = "grpID";	// group ID
	params[1].m_cdt = KSQLTypes::sql_c_uint64;
	params[2].m_name = "senderID";	// sender IDs
	params[2].m_cdt = KSQLTypes::sql_c_uint64;
	params[3].m_name = "contentType";
	params[3].m_cdt = KSQLTypes::sql_c_int32;
	params[4].m_name = "msgFile";	// message file path
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

	string_512 messageFile = g_pApp->buildMessageShortFilePath(m_messageTarget, m_messageID);

	if(    !cmd.SetInt64  (0, MessageID2Int64(m_messageID))
		|| !cmd.SetInt64  (1, m_messageTarget.targetID)
		|| !cmd.SetInt64  (2, m_messageSource.sourceID)
		|| !cmd.SetInteger(3, m_contentType)
		|| !cmd.SetString (4, messageFile.c_str(), messageFile.size())
		|| !cmd.SetInt64  (5, m_messageTime))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: _saveGroupPrivate, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	return TRUE;
}
