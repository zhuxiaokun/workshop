#include "DB_DeleteMessage.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include <JgErrorCode.h>
#include <KGlobalFunction.h>
#include <System/File/KFile.h>

DB_DeleteMessage::DB_DeleteMessage():m_result(0)
{
	memset(&m_messageID, 0, sizeof(m_messageID));
	memset(&m_messageTarget, 0, sizeof(m_messageTarget));
}

DB_DeleteMessage::~DB_DeleteMessage()
{

}

void DB_DeleteMessage::setMessage(JgMessageTarget target, JgMessageID messageID)
{
	m_messageTarget = target;
	m_messageID = messageID;
}

BOOL DB_DeleteMessage::checkDepends()
{
	return TRUE;
}

BOOL DB_DeleteMessage::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = NULL;
	switch(m_messageTarget.targetType)
	{
	case JgMessageTargetType::PERSON:
		sql = "call proc_Delete_Person_PrivateMessage(?)";
		break;
	case JgMessageTargetType::GROUP:
		sql = "call proc_Delete_Group_PrivateMessage(?)";
		break;
	case JgMessageTargetType::AVATAR:
	default: return TRUE;
	}

	KSQLFieldDescribe params[1];
	params[0].m_name = "id";  params[0].m_cdt = KSQLTypes::sql_c_uint64;
	
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
	if(!cmd.SetInt64(0, MessageID2Int64(m_messageID)))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_DeleteMessage, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	string_512 fileName = this->_buildMessageFile();
	if(!fileName.empty())
		JG_F::KFileUtil::deleteFile(fileName.c_str());

	return TRUE;
}

BOOL DB_DeleteMessage::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_DeleteMessage::onFinished messageID:%llu result:%d",
		MessageID2Int64(m_messageID), m_result);
	return TRUE;
}

void DB_DeleteMessage::reset()
{
	KSqlTask::reset();
	m_result = 0;
	memset(&m_messageID, 0, sizeof(m_messageID));
	memset(&m_messageTarget, 0, sizeof(m_messageTarget));	
}

void DB_DeleteMessage::destroy()
{
	this->reset();
	DB_DeleteMessage::Free(this);
}

string_512 DB_DeleteMessage::_buildMessageFile()
{
	char fileName[256];
	switch(m_messageTarget.targetType)
	{
	case JgMessageTargetType::PERSON:
		{
			int namelen = sprintf_k(fileName, sizeof(fileName), "%s/private/%08d/person/%llu/%u",
				g_pApp->m_storageAgent.m_dataDir.c_str(),
				m_messageID.yyyy_mm_dd,
				m_messageTarget.targetID,
				m_messageID.sequence);
			return string_512(fileName, namelen);
		}
	case JgMessageTargetType::GROUP:
		{
			int namelen = sprintf_k(fileName, sizeof(fileName), "%s/private/%08d/group/%llu/%u",
				g_pApp->m_storageAgent.m_dataDir.c_str(),
				m_messageID.yyyy_mm_dd,
				m_messageTarget.targetID,
				m_messageID.sequence);
			return string_512(fileName, namelen);
		}
	case JgMessageTargetType::AVATAR:
		{
			int namelen = sprintf_k(fileName, sizeof(fileName), "%s/private/%08d/person/%llu/%u",
				g_pApp->m_storageAgent.m_dataDir.c_str(),
				m_messageID.yyyy_mm_dd,
				m_messageTarget.targetID,
				m_messageID.sequence);
			return string_512(fileName, namelen);
		}
	}
	ASSERT(FALSE);
	return string_512();
}
