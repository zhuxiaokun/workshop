#include "FS_SaveMessage_2.h"
#include "../KDemoChatServerApp.h"
#include <JgErrorCode.h>
#include <System/File/KFile.h>
#include "../Database/DB_SetAccountAvatar.h"
#include "../Database/DB_SavePersionPrivate.h"
#include "../Database/DB_SaveGroupPrivate.h"
#include "../Database/DB_SavePersionPublic.h"
#include "../Database/DB_SaveMessage_2.h"

FS_SaveMessage_2::FS_SaveMessage_2()
{
	m_result = 0;
	memset(&m_messageID, 0, sizeof(m_messageID));
	memset(&m_messageSource, 0, sizeof(m_messageSource));
	memset(&m_messageTarget, 0, sizeof(m_messageTarget));
	m_contentType = JgMessageContentType::TEXT;
	m_messageTime = 0;
}

FS_SaveMessage_2::~FS_SaveMessage_2()
{

}

void FS_SaveMessage_2::execute()
{
	string_512 filepath = g_pApp->buildMessageFullFilePath(m_messageTarget, m_messageID);
	KOutputFileStream fo;
	if(!fo.OpenForce(filepath.c_str(), "w+b"))
	{
		m_result = GetLastError();
		Log(LOG_ERROR, "error: open '%s', %s", filepath.c_str(), strerror_r2(m_result).c_str());
		m_result = JgErrorCode::err_OpenFile;
		return;
	}
	char buf[1024]; int n;
	while(n = m_messageData.read(buf, sizeof(buf)), n > 0)
	{
		if(fo.WriteData(buf, n) != n)
		{
			m_result = JgErrorCode::err_WriteFile;
			return;
		}
	}
	
	return;
}

void FS_SaveMessage_2::onFinish()
{
	string_512 filepath = g_pApp->buildMessageFullFilePath(m_messageTarget, m_messageID);
	Log(LOG_WARN, "FS_SaveMessage_2 finish, result:%d filepath:'%s'", m_result, filepath.c_str());

	if(m_result) return;
	DB_SaveMessage_2* task = DB_SaveMessage_2::Alloc(); task->reset();
	task->m_messageID = m_messageID;
	task->m_messageTarget = m_messageTarget;
	task->m_messageSource = m_messageSource;
	task->m_contentType = m_contentType;
	task->m_messageTime = m_messageTime;
	g_pApp->m_databaseAgent.push(*task);
}

void FS_SaveMessage_2::reset()
{
	m_result = 0;
	memset(&m_messageID, 0, sizeof(m_messageID));
	memset(&m_messageSource, 0, sizeof(m_messageSource));
	memset(&m_messageTarget, 0, sizeof(m_messageTarget));
	m_contentType = JgMessageContentType::TEXT;
	m_messageData.reset();
	m_messageTime = 0;
	KStorageTask::reset();
}

void FS_SaveMessage_2::destroy()
{
	this->reset();
	FS_SaveMessage_2::Free(this);
}
