#include "FS_SaveAvatar.h"
#include "../KDemoChatServerApp.h"
#include <JgErrorCode.h>
#include <System/File/KFile.h>
#include "../Database/DB_SetAccountAvatar.h"

FS_SaveAvatar::FS_SaveAvatar()
{
	m_result = 0;
	memset(&m_messageID, 0, sizeof(m_messageID));
	m_chatAcct = NULL;
	m_message = NULL;
}

FS_SaveAvatar::~FS_SaveAvatar()
{

}

void FS_SaveAvatar::execute()
{
	string_32 avatarName = m_chatAcct->buildAvatarName(m_messageID);
	string_512 filepath = m_chatAcct->buildAvatarFilePath(m_messageID);

	KOutputFileStream fo;
	if(!fo.OpenForce(filepath.c_str(), "w+b"))
	{
		m_result = GetLastError();
		Log(LOG_ERROR, "error: open '%s', %s", filepath.c_str(), strerror_r2(m_result).c_str());
		m_result = JgErrorCode::err_OpenFile;
		return;
	}

	char buf[1024]; int n;
	while(n = m_message->m_messageData.read(buf, sizeof(buf)), n > 0)
	{
		if(fo.WriteData(buf, n) != n)
		{
			m_result = JgErrorCode::err_WriteFile;
			return;
		}
	}

	if(!m_chatAcct->m_avatar.empty())
	{
		filepath = m_chatAcct->buildAvatarFilePath(m_chatAcct->m_avatar.c_str());
		JG_F::KFileUtil::deleteFile(filepath.c_str());
	}

	return;
}

void FS_SaveAvatar::onFinish()
{
	ChtC_SetAvatar_Rsp rsp;
	rsp.result = m_result;
	if(m_result)
	{
		rsp.avatarName[0] = '\0';
	}
	else
	{
		m_chatAcct->m_avatar = m_chatAcct->buildAvatarName(m_messageID);
		strcpy_k(rsp.avatarName, sizeof(rsp.avatarName), m_chatAcct->m_avatar.c_str());
	}

	m_chatAcct->send(ChtC_SetAvatar_Rsp::s_nCode, &rsp, sizeof(rsp));
	m_chatAcct->m_chatSession.removeMessage(m_message->m_inSequence);

	{
		ChtC_Notify_AccountModification notification;
		notification.acctID = m_chatAcct->m_acctID;
		strcpy_k(notification.nick, sizeof(notification.nick), m_chatAcct->m_nick.c_str());
		strcpy_k(notification.signature, sizeof(notification.signature), m_chatAcct->m_signature.c_str());
		strcpy_k(notification.avatar, sizeof(notification.avatar), m_chatAcct->m_avatar.c_str());
		m_chatAcct->broadcastFriends(s_nChtC_Notify_AccountModification, &notification, sizeof(notification));
	}
	{
		ChtC_Notify_GroupMemberModification notification;
		notification.member.acctID = m_chatAcct->m_acctID;
		strcpy_k(notification.member.nick, sizeof(notification.member.nick), m_chatAcct->m_nick.c_str());
		strcpy_k(notification.member.signature, sizeof(notification.member.signature), m_chatAcct->m_signature.c_str());
		strcpy_k(notification.member.avatar, sizeof(notification.member.avatar), m_chatAcct->m_avatar.c_str());
		int n = m_chatAcct->m_groups.size();
		for(int i=0; i<n; i++)
		{
			KChatGroup* grp = (KChatGroup*)m_chatAcct->m_groups.at(i);
			notification.grpID = grp->m_groupID;
			grp->broadcastMembers(ChtC_Notify_GroupMemberModification::s_nCode, &notification, sizeof(notification), m_chatAcct);
		}
	}

	if(!m_result)
	{
		DB_SetAccountAvatar* task = DB_SetAccountAvatar::Alloc(); task->reset();
		task->setAcct(m_chatAcct);
		g_pApp->m_databaseAgent.push(*task);
	}
}

void FS_SaveAvatar::reset()
{
	m_result = 0;
	memset(&m_messageID, 0, sizeof(m_messageID));
	m_chatAcct = NULL;
	m_message = NULL;
}

void FS_SaveAvatar::destroy()
{
	this->reset();
	FS_SaveAvatar::Free(this);
}
