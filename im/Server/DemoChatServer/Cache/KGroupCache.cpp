#include "KGroupCache.h"
#include "../KDemoChatServerApp.h"
#include "../Database/DB_LoadGroupDetail.h"
#include "../Database/DB_JoinGroup.h"

/// KChatGroup

KChatGroup::KChatGroup():m_refCount(0),m_flags(0)
{
	ChatGroup::clear();
}

KChatGroup::~KChatGroup()
{

}

JgMessageTarget KChatGroup::toMessageTarget() const
{
	JgMessageTarget target;
	target.targetType = JgMessageTargetType::GROUP;
	target.targetID = m_groupID;
	return target;
}

JgMessageSource KChatGroup::toMessageSource() const
{
	JgMessageSource source;
	source.sourceType = JgMessageTargetType::GROUP;
	source.sourceID = m_groupID;
	return source;
}

KChatAccount* KChatGroup::findMember(UINT64 acctID)
{
	ChatAccount acct; acct.m_acctID = acctID;
	int pos = m_members.find(&acct);
	if(pos < 0) return NULL;
	return (KChatAccount*)m_members.at(pos);
}

bool KChatGroup::addMember(KChatAccount* chatAcct)
{
	if(this->findMember(chatAcct->m_acctID)) return false;

	chatAcct->m_groups.insert_unique(this);
	m_members.insert_unique(chatAcct);

	DB_JoinGroup* task = DB_JoinGroup::Alloc(); task->reset();
	task->setAcct(chatAcct, this);
	g_pApp->m_databaseAgent.push(*task);
	{
		KBufferOutputStream<1024*32> so;
		so.WriteShort(-1);
		KChatGroup* pGrp = this;
		ChtC_GroupInfo::Group grp;
		grp.groupID = m_groupID;
		strcpy_k(grp.nick, sizeof(grp.nick), m_nick.c_str());
		strcpy_k(grp.signature, sizeof(grp.signature), m_signature.c_str());
		so.WriteByteArray(&grp, sizeof(grp));
		chatAcct->send(s_nChtC_GroupInfo, so.data(), so.size());
	}
	{
		ChtC_Notify_JoinGroup rsp;
		rsp.grpID = m_groupID;
		rsp.member.acctID = chatAcct->m_acctID;
		strcpy_k(rsp.member.nick, sizeof(rsp.member.nick), chatAcct->m_nick.c_str());
		strcpy_k(rsp.member.signature, sizeof(rsp.member.signature), chatAcct->m_signature.c_str());
		strcpy_k(rsp.member.avatar, sizeof(rsp.member.avatar), chatAcct->m_avatar.c_str());
		this->broadcastMembers(ChtC_Notify_JoinGroup::s_nCode, &rsp, sizeof(rsp), chatAcct);
	}

	return true;
}

void KChatGroup::setLastMessage(const JgMessageID& messageID)
{
	int n = m_members.size();
	for(int i=0; i<n; i++)
	{
		KChatAccount* acct = (KChatAccount*)m_members.at(i);
		acct->setLastGroupMessage(messageID);
	}
}

void KChatGroup::reset()
{
	ChatGroup::clear();
	m_flags = 0;
	m_refCount = 0;
	m_members.clear();
	m_waitDetailQueue.clear();
}

void KChatGroup::copy(const ChatGroup& grp)
{
	memcpy(this, &grp, sizeof(grp));
}

void KChatGroup::loadDetail(KChatAccount* chatAcct)
{
	if(chatAcct) m_waitDetailQueue.insert_unique(chatAcct);
	if(this->hasFlag(detail_loaded) || this->hasFlag(detail_loading)) return;
	DB_LoadGroupDetail* task = DB_LoadGroupDetail::Alloc(); task->reset();
	task->setAcct(m_groupID, chatAcct);
	this->setFlag(detail_loading);
	g_pApp->m_databaseAgent.push(*task);
}

void KChatGroup::answerWaitDetailQueue()
{
	ASSERT(this->hasFlag(KChatGroup::detail_loaded));
	while(!m_waitDetailQueue.empty())
	{
		KChatAccount* chatAcct = (KChatAccount*)m_waitDetailQueue.pop_back();
		chatAcct->sendGroupDetail(this);
	}
}

void KChatGroup::broadcastMembers(int cmd, const void* data, int len, KChatAccount* exceptAcct)
{
	int n = m_members.size();
	for(int i=0; i<n; i++)
	{
		KChatAccount* acct = (KChatAccount*)m_members.at(i);
		if(acct != exceptAcct)
    	acct->send(cmd, data, len);
	}
}

/// KChatGroupCache

KChatGroupCache::KChatGroupCache()
{

}

KChatGroupCache::~KChatGroupCache()
{

}

KChatGroup* KChatGroupCache::find(UINT64 grpID)
{
	ChatGroupMap::iterator it = m_grpMap.find(grpID);
	if(it == m_grpMap.end()) return NULL;
	return it->second;
}

KChatGroup* KChatGroupCache::find(const char* nick)
{
	NickMap::iterator it = m_nickMap.find(nick);
	if(it == m_nickMap.end()) return NULL;
	return it->second;
}

BOOL KChatGroupCache::add(KChatGroup* grp)
{
	UINT64 grpID = grp->m_groupID;
	if(this->find(grpID)) return FALSE;
	m_grpMap[grpID] = grp;
	m_nickMap[grp->m_nick.c_str()] = grp;
	return TRUE;
}

BOOL KChatGroupCache::erase(UINT64 grpID)
{
	KChatGroup* grp = NULL;
	{
		ChatGroupMap::iterator it = m_grpMap.find(grpID);
		if(it == m_grpMap.end()) return FALSE;
		grp = it->second;
		m_grpMap.erase(it);
	}
	if(grp)
	{
		m_nickMap.erase(grp->m_nick.c_str());
	}
	
	return TRUE;
}
