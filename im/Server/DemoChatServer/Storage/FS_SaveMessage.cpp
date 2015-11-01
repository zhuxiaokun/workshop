#include "FS_SaveMessage.h"
#include "../KDemoChatServerApp.h"
#include <JgErrorCode.h>
#include <System/File/KFile.h>
#include "../Database/DB_SetAccountAvatar.h"
#include "../Database/DB_SavePersionPrivate.h"
#include "../Database/DB_SaveGroupPrivate.h"
#include "../Database/DB_SavePersionPublic.h"

FS_SaveMessage::FS_SaveMessage()
{
	m_errCode = 0;
	m_message = NULL;
}

FS_SaveMessage::~FS_SaveMessage()
{

}

void FS_SaveMessage::setMessage(KChatMessage* message)
{
	m_message = message;
}

void FS_SaveMessage::execute()
{
	char filepath[512];
	const char* dataDir = g_pApp->m_storageAgent.m_dataDir.c_str();

	string_256 messageFile = m_message->BuildMessageFileName();
	sprintf_k(filepath, sizeof(filepath), "%s/%s", dataDir, messageFile.c_str());

	KOutputFileStream fo;
	if(!fo.OpenForce(filepath, "w+b"))
	{
		m_errCode = GetLastError();
		Log(LOG_ERROR, "error: open '%s', %s", filepath, strerror_r2(m_errCode).c_str());
		m_errCode = JgErrorCode::err_OpenFile;
		return;
	}

	char buf[1024]; int n;
	while(n = m_message->m_messageData.read(buf, sizeof(buf)), n > 0)
	{
		if(fo.WriteData(buf, n) != n)
		{
			m_errCode = JgErrorCode::err_WriteFile;
			return;
		}
	}
	
	return;
}

void FS_SaveMessage::onFinish()
{
	if(m_errCode)
	{
		Log(LOG_ERROR, "error: FS_SaveMessage '%s' err:%d", m_message->BuildMessageFileName().c_str(), m_errCode);
		m_message->m_sender->m_chatSession.removeMessage(m_message->m_inSequence);
		return;
	}

	switch(m_message->m_target.targetType)
	{
	case JgMessageTargetType::PERSON:
		{
			DB_SavePersonPrivate* task = DB_SavePersonPrivate::Alloc(); task->reset();
			task->setMessage(m_message);
			g_pApp->m_databaseAgent.push(*task);
			break;
		}
	case JgMessageTargetType::GROUP:
		{
			DB_SaveGroupPrivate* task = DB_SaveGroupPrivate::Alloc(); task->reset();
			task->setMessage(m_message);
			g_pApp->m_databaseAgent.push(*task);
			break;
		}
	case JgMessageTargetType::AVATAR:
		{
			DB_SetAccountAvatar* task = DB_SetAccountAvatar::Alloc(); task->reset();
			task->setAcct(m_message->m_sender);
			g_pApp->m_databaseAgent.push(*task);
			break;
		}
	case JgMessageTargetType::PUBLIC:
		{
			DB_SavePersonPublic* task = DB_SavePersonPublic::Alloc(); task->reset();
			task->setMessage(m_message);
			g_pApp->m_databaseAgent.push(*task);
			break;
		}
	default:
		{
			ASSERT(!"FS_SaveMessage:un-excepted message target type");
			break;
		}
	}
}

void FS_SaveMessage::reset()
{
	m_errCode = 0;
	m_message = NULL;
}

void FS_SaveMessage::destroy()
{
	this->reset();
	FS_SaveMessage::Free(this);
}
