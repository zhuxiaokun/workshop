#include "FS_LoadUnreadMessages.h"
#include "../KDemoChatServerApp.h"
#include <JgErrorCode.h>
#include <System/File/KFile.h>

FS_LoadUnreadMessages::FS_LoadUnreadMessages()
{
	m_result = 0;
	m_chatAcct = NULL;
}

FS_LoadUnreadMessages::~FS_LoadUnreadMessages()
{

}

void FS_LoadUnreadMessages::setChatAcct(KChatAccount* chatAcct)
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
}

void FS_LoadUnreadMessages::execute()
{
	MessageInformation* minfo = m_messageList.begin(); ASSERT(minfo);

	char filepath[512];
	const char* dataDir = g_pApp->m_storageAgent.m_dataDir.c_str();
	sprintf_k(filepath, sizeof(filepath), "%s/%s", dataDir, minfo->filePath);

	KInputFileStream fi;
	if(!fi.Open(filepath, "rb"))
	{
		m_result = GetLastError();
		Log(LOG_ERROR, "error: open '%s', %s", filepath, strerror_r2(m_result).c_str());
		m_result = JgErrorCode::err_OpenFile;
		return;
	}

	int n; char buf[1024];
	while(n = fi.ReadData(buf, sizeof(buf)), n > 0)
		m_messageData.WriteData(buf, n);

	return;
}

void FS_LoadUnreadMessages::onFinish()
{
	if(!m_chatAcct->isSocketReady())
	{
		m_chatAcct->clearFlag(KChatAccount::loading_unread_messages);
		return;
	}

	MessageInformation* minfo = m_messageList.pop_front(); ASSERT(minfo);
	{
		ChtC_Message_Head mhead;
		mhead.sequence = g_pApp->m_nextSequence++;
		mhead.messageID = minfo->messageID;
		mhead.contentType = minfo->contentType;
		mhead.sender = minfo->source;
		mhead.mesageTarget = minfo->target;
		mhead.length = m_messageData.size();
		m_chatAcct->send(ChtC_Message_Head::s_nCode, &mhead, sizeof(mhead));

		ChtC_Message_Body_Tmpl<1000> mbody; mbody.sequence = mhead.sequence;
		while(m_messageData.size())
		{
			int bytes = m_messageData.read(&mbody.body, 1000);
			if(bytes < 1) break;
			m_chatAcct->send(ChtC_Message_Body::s_nCode, &mbody, sizeof(ChtC_Message_Body)+bytes);
		}

		ChtC_Message_End mend;
		mend.sequence = mhead.sequence;
		m_chatAcct->send(ChtC_Message_End::s_nCode, &mend, sizeof(mend));

		switch(minfo->target.targetType)
		{
		case JgMessageTargetType::PERSON:
			m_chatAcct->setLastPersonMessage(minfo->messageID);
			break;
		case JgMessageTargetType::GROUP:
			m_chatAcct->setLastGroupMessage(minfo->messageID);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
	}
	MessageInformation::Free(minfo);

	if(m_messageList.empty())
	{
		m_chatAcct->clearFlag(KChatAccount::loading_unread_messages);
		return;
	}

	FS_LoadUnreadMessages* task = FS_LoadUnreadMessages::Alloc(); task->reset();
	task->setChatAcct(m_chatAcct);
	while(!m_messageList.empty()) task->m_messageList.push_back(m_messageList.pop_front());
	g_pApp->m_storageAgent.Push(task);
}

void FS_LoadUnreadMessages::reset()
{
	m_result = 0;
	this->setChatAcct(NULL);
	while(!m_messageList.empty()) MessageInformation::Free(m_messageList.pop_front());
	m_messageData.reset();
	KStorageTask::reset();
}

void FS_LoadUnreadMessages::destroy()
{
	this->reset();
	FS_LoadUnreadMessages::Free(this);
}
