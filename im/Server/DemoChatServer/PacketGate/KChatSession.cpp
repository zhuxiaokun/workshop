#include "KChatSession.h"
#include "../Cache/KAccountCache.h"
#include "../Cache/KGroupCache.h"
#include <time.h>
#include "../KDemoChatServerApp.h"
#include "../Storage/FS_SaveAvatar.h"
#include "../Storage/FS_SaveMessage.h"
#include <JgErrorCode.h>

/// KChatMessage

KChatMessage::KChatMessage()
{
	m_inSequence = 0;
	m_outSequence = 0;
	memset(&m_messageID, 0, sizeof(m_messageID));
	memset(&m_target, 0, sizeof(m_target));
	m_sender = NULL;
	m_receiver = NULL;
	m_grp = 0;
	m_contentType = JgMessageContentType::TEXT;
	m_lengthSum = 0;
	m_startTime = 0;
}

KChatMessage::~KChatMessage()
{

}

void KChatMessage::Reset()
{
	m_inSequence = 0;
	m_outSequence = 0;
	memset(&m_messageID, 0, sizeof(m_messageID));
	memset(&m_target, 0, sizeof(m_target));
	m_sender = NULL;
	m_receiver = NULL;
	m_grp = NULL;
	m_messageData.reset();
	m_contentType = JgMessageContentType::TEXT;
	m_lengthSum = 0;
	m_startTime = 0;

}

string_256 KChatMessage::BuildMessageFileName() const
{
	char fileName[256];
	switch(m_target.targetType)
	{
	case JgMessageTargetType::PERSON:
		{
			int namelen = sprintf_k(fileName, sizeof(fileName), "private/%08d/person/%llu/%u",
				m_messageID.yyyy_mm_dd,
				m_target.targetID,
				m_messageID.sequence);
			return string_256(fileName, namelen);
		}
	case JgMessageTargetType::GROUP:
		{
			int namelen = sprintf_k(fileName, sizeof(fileName), "private/%08d/group/%llu/%u",
				m_messageID.yyyy_mm_dd,
				m_target.targetID,
				m_messageID.sequence);
			return string_256(fileName, namelen);
		}
	case JgMessageTargetType::AVATAR:
		{
			int namelen = sprintf_k(fileName, sizeof(fileName), "private/%08d/person/%llu/%u",
				m_messageID.yyyy_mm_dd,
				m_target.targetID,
				m_messageID.sequence);
			return string_256(fileName, namelen);
		}
	case JgMessageTargetType::PUBLIC:
		{
			int namelen = sprintf_k(fileName, sizeof(fileName), "public/%08d/person/%llu/%u",
				m_messageID.yyyy_mm_dd,
				m_sender->m_acctID,
				m_messageID.sequence);
			return string_256(fileName, namelen);
		}
	}
	ASSERT(FALSE);
	return string_256();
}

/// KChatSession

KChatSession::KChatSession():m_chatAcct(NULL)
{

}

KChatSession::~KChatSession()
{

}

KChatMessage* KChatSession::getMessage(DWORD seq)
{
	KChatMessage msg; msg.m_inSequence = seq;
	int pos = m_messages.find(&msg);
	if(pos < 0) return NULL;
	return m_messages.at(pos);
}

bool KChatSession::removeMessage(DWORD seq)
{
	KChatMessage msg; msg.m_inSequence = seq;
	int pos = m_messages.find(&msg);
	if(pos < 0) return false;
	KChatMessage* ptr = m_messages.at(pos);
	m_messages.erase_by_index(pos);
	KChatMessage::Free(ptr);
	return true;
}

void KChatSession::clear()
{
	while(!m_messages.empty())
	{
		KChatMessage* msg = m_messages.pop_back();
		KChatMessage::Free(msg);
	}
}

bool KChatSession::on_Message_Head(CCht_Message_Head* messageHead, int len)
{
	DWORD seq = messageHead->sequence;

	ChtC_Message_Ack ack; memset(&ack, 0, sizeof(ack));
	ack.sequence = seq;

	KChatMessage* msg = this->getMessage(seq);
	if(msg)
	{
		ack.result = JgErrorCode::err_InnerError;
		m_chatAcct->send(ChtC_Message_Ack::s_nCode, &ack, sizeof(ack));
		Log(LOG_ERROR, "error: %s message seq:%u already exist", m_chatAcct->toString().c_str(), seq);
		return false;
	}

	KChatAccount* receiver = NULL;
	KChatGroup* chatGrp = NULL;
	switch(messageHead->messageTarget.targetType)
	{
	case JgMessageTargetType::PERSON:
		{
			receiver = g_pApp->m_acctCache.find(messageHead->messageTarget.targetID);
			if(!receiver)
			{
				ack.result = JgErrorCode::err_Peer_NotFound;
				m_chatAcct->send(ChtC_Message_Ack::s_nCode, &ack, sizeof(ack));
				Log(LOG_ERROR, "error: %s message seq:%u, target acct:%llu not found", m_chatAcct->toString().c_str(), seq, messageHead->messageTarget.targetID);
				return false;
			}
		} break;
	case JgMessageTargetType::GROUP:
		{
			chatGrp = g_pApp->m_grpCache.find(messageHead->messageTarget.targetID);
			if(!chatGrp)
			{
				ack.result = JgErrorCode::err_Group_NotFound;
				m_chatAcct->send(ChtC_Message_Ack::s_nCode, &ack, sizeof(ack));
				Log(LOG_ERROR, "error: %s message %u, target grp:%llu not found", m_chatAcct->toString().c_str(), seq, messageHead->messageTarget.targetID);
				return false;
			}
		} break;
	case JgMessageTargetType::AVATAR:
		break;
	default:
		ack.result = JgErrorCode::err_InnerError;
		m_chatAcct->send(ChtC_Message_Ack::s_nCode, &ack, sizeof(ack));
		return false;
	}

	msg = KChatMessage::Alloc(); msg->Reset();
	msg->m_inSequence = seq;
	msg->m_outSequence = g_pApp->m_nextSequence++;
	msg->m_messageID = g_pApp->nextMessageID();
	msg->m_target = messageHead->messageTarget;
	msg->m_sender = m_chatAcct;
	msg->m_receiver = receiver;
	msg->m_grp = chatGrp;
	msg->m_contentType = messageHead->contentType;
	msg->m_lengthSum = 0;
	msg->m_startTime = time(NULL);

	ack.messageID = msg->m_messageID;
	ack.messageTime = msg->m_startTime;
	m_chatAcct->send(ChtC_Message_Ack::s_nCode, &ack, sizeof(ack));

	m_messages.insert(msg);

	switch(msg->m_target.targetType)
	{
	case JgMessageTargetType::PERSON:
		{
			ChtC_Message_Head rsp;
			rsp.sequence = msg->m_outSequence;
			rsp.messageID = msg->m_messageID;
			rsp.contentType = messageHead->contentType;
			rsp.sender = m_chatAcct->toMessageSource();
			rsp.mesageTarget = messageHead->messageTarget;
			rsp.length = messageHead->length;
			receiver->send(ChtC_Message_Head::s_nCode, &rsp, sizeof(rsp));
			break;
		}
		
	case JgMessageTargetType::GROUP:
		{
			ChtC_Message_Head rsp;
			rsp.sequence = msg->m_outSequence;
			rsp.messageID = msg->m_messageID;
			rsp.contentType = messageHead->contentType;
			rsp.sender = m_chatAcct->toMessageSource();
			rsp.mesageTarget = messageHead->messageTarget;
			rsp.length = messageHead->length;
			chatGrp->broadcastMembers(ChtC_Message_Head::s_nCode, &rsp, sizeof(rsp), m_chatAcct);
			break;
		}
	}

	return true;
}

bool KChatSession::on_Message_Body(CCht_Message_Body* messageBody, int len)
{
	DWORD seq = messageBody->sequence;
	KChatMessage* msg = this->getMessage(seq);
	if(!msg)
	{
		Log(LOG_ERROR, "error: %s message body, seq:%u not found", m_chatAcct->toString().c_str(), seq);
		return false;
	}

	KChatAccount* receiver = msg->m_receiver;
	KChatGroup* chatGrp = msg->m_grp;

	msg->m_messageData.WriteData(&messageBody->body, len-sizeof(DWORD));
	
	switch(msg->m_target.targetType)
	{
	case JgMessageTargetType::PERSON:
		{
			KBufferOutputStream<32*1024> so;
			so.WriteDword(msg->m_outSequence);
			so.WriteByteArray(&messageBody->body, len-sizeof(DWORD));
			receiver->send(ChtC_Message_Body::s_nCode, so.data(), so.size());
		} break;
	case JgMessageTargetType::GROUP:
		{
			KBufferOutputStream<32*1024> so;
			so.WriteDword(msg->m_outSequence);
			so.WriteByteArray(&messageBody->body, len-sizeof(DWORD));
			chatGrp->broadcastMembers(ChtC_Message_Body::s_nCode, so.data(), so.size(), m_chatAcct);
		} break;
	//case JgMessageTargetType::AVATAR:
	//	{
	//		msg->m_messageData.WriteData(&messageBody->body, len-sizeof(DWORD));
	//	} break;
	}

	return true;
}

bool KChatSession::on_Message_End(CCht_Message_End* messageEnd, int len)
{
	DWORD seq = messageEnd->sequence;
	KChatMessage* msg = this->getMessage(seq);
	if(!msg)
	{
		Log(LOG_ERROR, "error: %s message end, seq:%u not found", m_chatAcct->toString().c_str(), seq);
		return false;
	}

	KChatAccount* receiver = msg->m_receiver;
	KChatGroup* chatGrp = msg->m_grp;

	ChtC_Message_End rsp;
	rsp.sequence = msg->m_outSequence;

	switch(msg->m_target.targetType)
	{
	case JgMessageTargetType::PERSON:
		{
			receiver->setLastPersonMessage(msg->m_messageID);
			if(receiver->isSocketReady())
			{
				receiver->send(ChtC_Message_End::s_nCode, &rsp, sizeof(rsp));
				this->removeMessage(seq);
			}
			else
			{
				FS_SaveMessage* task = FS_SaveMessage::Alloc(); task->reset();
				task->setMessage(msg);
				g_pApp->m_storageAgent.Push(task);
			}
			break;
		}
	case JgMessageTargetType::GROUP:
		{
			chatGrp->setLastMessage(msg->m_messageID);
			chatGrp->broadcastMembers(ChtC_Message_End::s_nCode, &rsp, sizeof(rsp), m_chatAcct);
			FS_SaveMessage* task = FS_SaveMessage::Alloc(); task->reset();
			task->setMessage(msg);
			g_pApp->m_storageAgent.Push(task);
			break;
		}
	case JgMessageTargetType::AVATAR:
		{
			FS_SaveAvatar* task = FS_SaveAvatar::Alloc(); task->reset();
			task->m_messageID = msg->m_messageID;
			task->m_chatAcct = m_chatAcct;
			task->m_message = msg;
			g_pApp->m_storageAgent.Push(task);
			break;
		}
	}
	
	return true;
}
