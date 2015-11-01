#include "KChatSession.h"
#include <time.h>
#include "../KTestClient.h"
#include <System/Misc/KDatetime.h>
#include <System/File/KFile.h>

////////////////////////////////////

const char* getUserDirectory();
bool saveMessageToDB(KOutgoingMessage* msg);

/// KOutGoingMessage ///////////////

KOutgoingMessage::KOutgoingMessage()
{
	this->reset();
}

KOutgoingMessage::~KOutgoingMessage()
{

}

void KOutgoingMessage::reset()
{
	m_sequence = 0;
	memset(&m_messageID, 0, sizeof(m_messageID));
	memset(&m_messageSource, 0, sizeof(m_messageSource));
	m_contentType = JgMessageContentType::TEXT;
	memset(&m_messageTarget, 0, sizeof(m_messageTarget));
	if(m_isTmpFile && !m_messageFile.empty())
		System::File::KFileUtil::deleteFile(m_messageFile.c_str());
	m_messageFile.clear();
	m_messageTime = 0;
	m_isTmpFile = false;
}

void KOutgoingMessage::destroy()
{
	this->reset();
	KOutgoingMessage::Free(this);
}

string_512 KOutgoingMessage::buildTmpFileName() const
{
	char buf[512];
	KDatetime nowDate;
	KLocalDatetime ldt = nowDate.GetLocalTime();
	int n = sprintf_k(buf, sizeof(buf), "%s/tmp/%04d%02d%02d/%d",
		getUserDirectory(),
		ldt.year, ldt.month, ldt.day, m_sequence);
	return string_512(buf, n);
}

string_512 KOutgoingMessage::buildMessageFilePath() const
{
	JgMessageTarget target;
	target.targetType = JgMessageTargetType::PERSON;
	target.targetID = g_client->m_acctID;
	return KChatMessage::buildMessageFilePath(m_messageID, target);
}

string_512 KOutgoingMessage::buildMessageFullPath() const
{
	char buf[512];
	int n = sprintf_k(buf, sizeof(buf), "%s/%s",
		getUserDirectory(),
		this->buildMessageFilePath().c_str());
	return string_512(buf, n);
}

bool KOutgoingMessage::saveTmpFile(const void* data, int len)
{
	string_512 filepath = this->buildTmpFileName();
	if(filepath.empty()) return true;

	KOutputFileStream fo;
	if(!fo.OpenForce(filepath.c_str(), "w+"))
	{
		Log(LOG_ERROR, "error: open '%s'", filepath.c_str());
		return false;
	}

	char* p = (char*)data;
	while(len > 0)
	{
		int n = kmin(len, 512);
		fo.WriteData(p, n); p += n; len -= n;
	}	fo.Close();

	return true;
}

bool KOutgoingMessage::save()
{
	string_512 fullpath = this->buildMessageFullPath();
	if(fullpath.empty()) return true;

	KOutputFileStream fo;
	if(!fo.OpenForce(fullpath.c_str(), "w+"))
	{
		Log(LOG_ERROR, "error: open '%s'", fullpath.c_str());
		return false;
	}
	
	KInputFileStream fi;
	if(!fi.Open(m_messageFile.c_str(), "rb"))
	{
		Log(LOG_ERROR, "error: open '%s'", m_messageFile.c_str());
		return false;
	}

	char buf[1024]; int n;
	while(n = fi.ReadData(buf, sizeof(buf)), n > 0) fo.WriteData(buf, n);
	fi.Close(); fo.Close();

	if(!m_messageFile.empty() && m_isTmpFile)
		System::File::KFileUtil::deleteFile(m_messageFile.c_str());
	
	return saveMessageToDB(this);
}

/// KChatMessage ///////////////////////////////////////////////

string_512 KChatMessage::buildMessageFilePath(const JgMessageID& mid, const JgMessageTarget& target)
{
	char buf[512];
	switch(target.targetType)
	{
	case JgMessageTargetType::PERSON:
		{
			int n = sprintf_k(buf, sizeof(buf), "received/%08d/person/%llu/%u",
				mid.yyyy_mm_dd,
				target.targetID,
				mid.sequence);
			return string_512(buf, n);
		}
	case JgMessageTargetType::GROUP:
		{
			int n = sprintf_k(buf, sizeof(buf), "received/%08d/group/%llu/%u",
				mid.yyyy_mm_dd,
				target.targetID,
				mid.sequence);
			return string_512(buf, n);
		}
	case JgMessageTargetType::AVATAR:
		{
			int n = sprintf_k(buf, sizeof(buf), "avatar/%llu/%08d-%u",
				target.targetID,
				mid.yyyy_mm_dd,
				mid.sequence);
			return string_512(buf, n);
		}
	case JgMessageTargetType::PUBLIC:
		break;
	}
	return string_512();
}

/// KChatSession ///////////////////////////////////////////////

KChatSession::KChatSession()
{

}

KChatSession::~KChatSession()
{

}

KChatMessage* KChatSession::getMessage(DWORD seq)
{
	KChatMessage msg; msg.m_sequence = seq;
	int pos = m_messages.find(&msg);
	if(pos < 0) return NULL;
	return m_messages.at(pos);
}

KOutgoingMessage* KChatSession::getOutgoingMessage(DWORD seq)
{
	KOutgoingMessage msg; msg.m_sequence = seq;
	int pos = m_outMessages.find(&msg);
	if(pos < 0) return NULL;
	return m_outMessages.at(pos);
}

bool KChatSession::removeMessage(DWORD seq)
{
	KChatMessage msg; msg.m_sequence = seq;
	int pos = m_messages.find(&msg);
	if(pos < 0) return false;
	KChatMessage* ptr = m_messages.at(pos);
	m_messages.erase_by_index(pos);
	KChatMessage::Free(ptr);
	return true;
}

bool KChatSession::removeOutGoingMessage(DWORD seq)
{
	KOutgoingMessage msg; msg.m_sequence = seq;
	int pos = m_outMessages.find(&msg);
	if(pos < 0) return false;
	KOutgoingMessage* ptr = m_outMessages.at(pos);
	m_outMessages.erase_by_index(pos);
	ptr->destroy();
	return true;
}

void KChatSession::clear()
{
	while(!m_messages.empty())
	{
		KChatMessage* msg = m_messages.pop_back();
		KChatMessage::Free(msg);
	}
	KOutgoingMessage* outMsg = NULL;
	while(outMsg = m_outMessages.pop_back(), outMsg)
		outMsg->destroy();
}

bool KChatSession::on_Message_Head(ChtC_Message_Head* messageHead, int len)
{
	DWORD seq = messageHead->sequence;
	KChatMessage* msg = this->getMessage(seq);
	if(msg)
	{
		Log(LOG_CONSOLE, "error: message seq:%u already exist", seq);
		return false;
	}

	//ChatAccount* sender = g_client->getFriend(messageHead->sender.sourceID);
	//KChatGroup* chatGrp = NULL;

	//switch(messageHead->mesageTarget.targetType)
	//{
	//case JgMessageTargetType::PERSON:
	//	{
	//		if(!sender)
	//		{
	//			Log(LOG_CONSOLE, "error: seq:%u sender acct:%llu not found", seq, messageHead->sender.sourceID);
	//			return false;
	//		}
	//	} break;
	//case JgMessageTargetType::GROUP:
	//	{
	//		chatGrp = g_client->getGroup(messageHead->mesageTarget.targetID);
	//		if(!chatGrp)
	//		{
	//			Log(LOG_CONSOLE, "error: seq %u, sender grp:%llu not found", seq, messageHead->mesageTarget.targetID);
	//			return false;
	//		}
	//	} break;
	//case JgMessageTargetType::AVATAR:
	//	{
	//		break;
	//	}
	//default:
	//	return false;
	//}

	msg = KChatMessage::Alloc();
	msg->m_sequence = seq;
	msg->m_messageID = messageHead->messageID;
	msg->m_messageSource = messageHead->sender;
	msg->m_messageTarget = messageHead->mesageTarget;
	msg->m_contentType = messageHead->contentType;
	msg->m_lengthSum = 0;
	msg->m_startTime = time(NULL);
	msg->m_messageData.Seek(0);

	m_messages.insert(msg);
	return true;
}

bool KChatSession::on_Message_Body(ChtC_Message_Body* messageBody, int len)
{
	DWORD seq = messageBody->sequence;
	KChatMessage* msg = this->getMessage(seq);
	if(!msg)
	{
		Log(LOG_ERROR, "error: message body, seq:%u not found", seq);
		return false;
	}
	msg->m_messageData.WriteByteArray(messageBody->body, len-sizeof(ChtC_Message_Body));
	return true;
}

bool KChatSession::on_Message_End(ChtC_Message_End* messageEnd, int len)
{
	DWORD seq = messageEnd->sequence;
	KChatMessage* msg = this->getMessage(seq);
	if(!msg)
	{
		Log(LOG_ERROR, "error: message end, seq:%u not found", seq);
		return false;
	}
	msg->m_finished = TRUE;
	return true;
}
