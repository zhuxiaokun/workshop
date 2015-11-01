#include "KAccountCache.h"
#include <time.h>
#include "../KDemoChatServerApp.h"
#include "../Database/DB_Authenticate.h"
#include "../Database/DB_LoadFriends.h"
#include "../Database/DB_LoadGroups.h"
#include "../Database/DB_AddFriend.h"
#include "../Database/DB_JoinGroup.h"
#include "../Database/DB_ModifyAccount.h"
#include "../Database/DB_RemoveFriend.h"
#include "../Database/DB_LeaveGroup.h"
#include "../Database/DB_LoadUnreadMessages.h"
#include "KGroupCache.h"
#include "JgErrorCode.h"
#include <KGlobalFunction.h>

/// KChatAccount

KChatAccount::KChatAccount():m_state(empty)
{
	ChatAccount::clear();
	m_flags = 0;
	m_refCount = 0;
	m_clnSocket = NULL;
	m_lastPersonMessage = 0;
	m_lastGroupMessage = 0;
	//m_nextSequence = 0;
	m_chatSession.m_chatAcct = this;
}

KChatAccount::~KChatAccount()
{

}

string_128 KChatAccount::toString() const
{
	char buf[128];
	int len = sprintf_k(buf, sizeof(buf), "acct:%llu %s", m_acctID, m_acctName.c_str());
	return string_128(buf, len);
}

string_32 KChatAccount::buildAvatarName(const JgMessageID& messageID) const
{
	char buf[32];
	int n = sprintf_k(buf, sizeof(buf), "%08d-%04d", messageID.yyyy_mm_dd, messageID.sequence);
	return string_32(buf, n);
}

string_512 KChatAccount::buildAvatarFilePath(const JgMessageID& messageID) const
{
	char buf[512];
	int n = sprintf_k(buf, sizeof(buf), "%s/avatar/%llu/%s",
		g_pApp->m_storageAgent.m_dataDir.c_str(),
		m_acctID, this->buildAvatarName(messageID).c_str());
	return string_512(buf, n);
}

string_512 KChatAccount::buildAvatarFilePath(const char* avatarName) const
{
	char buf[512];
	int n = sprintf_k(buf, sizeof(buf), "%s/avatar/%llu/%s",
		g_pApp->m_storageAgent.m_dataDir.c_str(),
		m_acctID, avatarName);
	return string_512(buf, n);
}

JgMessageTarget KChatAccount::toMessageTarget() const
{
	JgMessageTarget target;
	target.targetType = JgMessageTargetType::PERSON;
	target.targetID = m_acctID;
	return target;
}

JgMessageSource KChatAccount::toMessageSource() const
{
	JgMessageSource source;
	source.sourceType = JgMessageTargetType::PERSON;
	source.sourceID = m_acctID;
	return source;
}

void KChatAccount::reset()
{
	ChatAccount::clear();
	m_flags = 0;
	m_refCount = 0;
	m_state = empty;
	this->setSocket(NULL);
	m_friends.clear();
	m_groups.clear();
	m_lastPersonMessage = 0;
	m_lastGroupMessage = 0;
	//m_nextSequence = 0;
	m_chatSession.clear();
}

void KChatAccount::copy(const ChatAccount& acct)
{
	memcpy(this, &acct, sizeof(acct));
}

int KChatAccount::addRef()
{
	return ++m_refCount;
}

int KChatAccount::releaseRef()
{
	return --m_refCount;
}

void KChatAccount::setSocket(KSocket_Client* clnSocket)
{
	if(m_clnSocket == clnSocket) return;
	if(m_clnSocket)
	{
		m_clnSocket->m_chatAcct = NULL;
		m_clnSocket->ReleaseRef();
		m_clnSocket = NULL;
	}
	if(clnSocket)
	{
		clnSocket->AddRef();
		m_clnSocket = clnSocket;
		clnSocket->m_chatAcct = this;
	}
}

BOOL KChatAccount::send(int cmd, const void* data, int len)
{
	if(!m_clnSocket || !m_clnSocket->IsReady())
		return FALSE;
	return m_clnSocket->Send(cmd, data, len);
}

BOOL KChatAccount::broadcastFriends(int cmd, const void* data, int len)
{
	int n = m_friends.size();
	for(int i=0; i<n; i++)
	{
		KChatAccount* frd = (KChatAccount*)m_friends.at(i);
		frd->send(cmd, data, len);
	}
	return TRUE;
}

BOOL KChatAccount::isSocketReady() const
{
	return m_clnSocket && m_clnSocket->IsReady();
}

void KChatAccount::closeSocket(int delaySec)
{
	if(m_clnSocket)
	{
		if(!delaySec)
			m_clnSocket->Close();
		else
			m_clnSocket->DelayClose(abs(delaySec));
	}
}

BOOL KChatAccount::sendFriendsInfo()
{
	if(!this->hasFlag(friends_loaded))
	{
		this->loadFriends();	
		return FALSE;
	}
	
	KBufferOutputStream<1024*32> so;

	int n = m_friends.size();
	so.WriteShort(n);
	for(int i=0; i<n; i++)
	{
		KChatAccount* acct = (KChatAccount*)m_friends.at(i);
		ChtC_FriendInfo::Friend frd;
		frd.acctID = acct->m_acctID;
		strcpy_k(frd.nick, sizeof(frd.nick), acct->m_nick.c_str());
		strcpy_k(frd.signature, sizeof(frd.signature), acct->m_signature.c_str());
		strcpy_k(frd.avatar, sizeof(frd.avatar), acct->m_avatar.c_str());
		so.WriteByteArray(&frd, sizeof(frd));
	}
	this->send(s_nChtC_FriendInfo, so.data(), so.size());

	this->notifyEnter();
	return TRUE;
}

BOOL KChatAccount::sendGroupsInfo()
{
	if(!this->hasFlag(groups_loaded))
	{
		this->loadGroups();
		return FALSE;
	}

	KBufferOutputStream<1024*32> so;

	int n = m_groups.size();
	so.WriteShort(n);
	for(int i=0; i<n; i++)
	{
		KChatGroup* pGrp = (KChatGroup*)m_groups.at(i);
		ChtC_GroupInfo::Group grp;
		grp.groupID = pGrp->m_groupID;
		strcpy_k(grp.nick, sizeof(grp.nick), pGrp->m_nick.c_str());
		strcpy_k(grp.signature, sizeof(grp.signature), pGrp->m_signature.c_str());
		so.WriteByteArray(&grp, sizeof(grp));
	}
	return this->send(s_nChtC_GroupInfo, so.data(), so.size());

	return TRUE;
}

BOOL KChatAccount::sendGroupDetail(KChatGroup* chatGrp)
{
	if(!chatGrp->hasFlag(KChatGroup::detail_loaded))
	{
		chatGrp->loadDetail(this);
		return FALSE;
	}

	KBufferOutputStream<32*1024> so;

	int n = chatGrp->m_members.size();
	so.WriteUint64(chatGrp->m_groupID);
	so.WriteShort((short)n);
	for(int i=0; i<n; i++)
	{
		KChatAccount* chatAcct = (KChatAccount*)chatGrp->m_members.at(i);
		ChtC_GroupDetail::Member m;
		m.passport = chatAcct->m_acctID;
		strcpy_k(m.nick, sizeof(m.nick), chatAcct->m_nick.c_str());
		strcpy_k(m.signature, sizeof(m.signature), chatAcct->m_signature.c_str());
		strcpy_k(m.avatar, sizeof(m.avatar), chatAcct->m_avatar.c_str());
		so.WriteByteArray(&m, sizeof(m));
	}
	return this->send(ChtC_GroupDetail::s_nCode, so.data(), so.size());
}

BOOL KChatAccount::notifyEnter()
{
	int n = m_friends.size();
	for(int i=0; i<n; i++)
	{
		KChatAccount* acct = (KChatAccount*)m_friends.at(i);
		ChtC_NotifyEnter req; req.acctID = m_acctID;
		acct->send(s_nChtC_NotifyEnter, &req, sizeof(req));
	}
	return TRUE;
}

BOOL KChatAccount::notifyLeave()
{
	int n = m_friends.size();
	for(int i=0; i<n; i++)
	{
		KChatAccount* acct = (KChatAccount*)m_friends.at(i);
		ChtC_NotifyLeave req; 
		req.acctID = m_acctID;
		acct->send(s_nChtC_NotifyLeave, &req, sizeof(req));
	}
	return TRUE;
}

BOOL KChatAccount::notifyModification()
{
	int n = m_friends.size();
	for(int i=0; i<n; i++)
	{
		KChatAccount* acct = (KChatAccount*)m_friends.at(i);
		ChtC_Notify_AccountModification req;
		req.acctID = m_acctID;
		strcpy_k(req.nick, sizeof(req.nick), m_nick.c_str());
		strcpy_k(req.signature, sizeof(req.signature), m_signature.c_str());
		strcpy_k(req.avatar, sizeof(req.avatar), m_avatar.c_str());
		acct->send(s_nChtC_Notify_AccountModification, &req, sizeof(req));
	}
	return TRUE;
}

void KChatAccount::loadFriends()
{
	if(this->hasFlag(loading_friends)) return;
	if(this->hasFlag(friends_loaded)) return;
	m_state = KChatAccount::load_friends;
	DB_LoadFriends* task = DB_LoadFriends::Alloc(); task->reset();
	task->setAcct(this);
	this->setFlag(loading_friends);
	g_pApp->m_databaseAgent.push(*task);
}

void KChatAccount::loadGroups()
{
	if(this->hasFlag(loading_groups)) return;
	if(this->hasFlag(groups_loaded)) return;
	m_state = KChatAccount::load_groups;
	DB_LoadGroups* task = DB_LoadGroups::Alloc(); task->reset();
	task->setAcct(this);
	this->setFlag(loading_groups);
	g_pApp->m_databaseAgent.push(*task);
}

void KChatAccount::loadUnreadMessages(INT64 lastPersonMessage, INT64 lastGroupMessage)
{
	if(this->hasFlag(loading_unread_messages)) return;
	this->setFlag(loading_unread_messages);//标志正在读取未读消息
	DB_LoadUnreadMessages* task = DB_LoadUnreadMessages::Alloc();
	task->reset(); task->setAcct(this);
	task->m_lastPersonMessageID = Int642MessageID(lastPersonMessage);
	task->m_lastGroupMessageID = Int642MessageID(lastGroupMessage);
	g_pApp->m_databaseAgent.push(*task);
}

void KChatAccount::setLastPersonMessage(const JgMessageID& messageID)
{
	m_lastPersonMessage = MessageID2Int64(messageID);
}

void KChatAccount::setLastGroupMessage(const JgMessageID& messageID)
{
	m_lastGroupMessage = MessageID2Int64(messageID);
}

BOOL KChatAccount::hasFriend(UINT64 acctID) const
{
	ChatAccount acct;
	acct.m_acctID = acctID;
	return m_friends.find(&acct) >= 0;
}

BOOL KChatAccount::isGroupMember(UINT64 grpID) const
{
	ChatGroup grp; grp.m_groupID = grpID;
	return m_groups.find(&grp) >= 0;
}

BOOL KChatAccount::addFriend(KChatAccount* frd)
{
	if(this->hasFriend(frd->m_acctID)) return TRUE;

	m_friends.insert_unique(frd);
	frd->m_friends.insert_unique(this);

	DB_AddFriend* task = DB_AddFriend::Alloc(); task->reset();
	task->setAcct(this, frd);
	g_pApp->m_databaseAgent.push(*task);

	return TRUE;
}

BOOL KChatAccount::joinGroup(KChatGroup* chatGrp)
{
	if(this->isGroupMember(chatGrp->m_groupID))
	{
		ChtC_JoinGroup_Rsp rsp;
		rsp.result =JgErrorCode::err_AlreadyGroupMember;//已经是群成员
		this->send(s_nChtC_JoinGroup_Rsp, &rsp, sizeof(rsp));
		return FALSE;
	}

	m_groups.insert_unique(chatGrp);
	chatGrp->m_members.insert_unique(this);

	DB_JoinGroup* task = DB_JoinGroup::Alloc(); task->reset();
	task->setAcct(this, chatGrp);
	g_pApp->m_databaseAgent.push(*task);

	{
		ChtC_JoinGroup_Rsp rsp;
		rsp.result =JgErrorCode::err_Success;
		this->send(s_nChtC_JoinGroup_Rsp, &rsp, sizeof(rsp));
	}
	{
		KBufferOutputStream<1024*32> so;
		so.WriteShort(-1);
		KChatGroup* pGrp = chatGrp;
		ChtC_GroupInfo::Group grp;
		grp.groupID = pGrp->m_groupID;
		strcpy_k(grp.nick, sizeof(grp.nick), pGrp->m_nick.c_str());
		strcpy_k(grp.signature, sizeof(grp.signature), pGrp->m_signature.c_str());
		so.WriteByteArray(&grp, sizeof(grp));
		this->send(s_nChtC_GroupInfo, so.data(), so.size());
	}
	
	return TRUE;
}

BOOL KChatAccount::modify(const char* nick, const char* signature)
{
	m_nick = nick;
	m_signature = signature;

	{
		ChtC_ModifyAccount_Rsp rsp;
		rsp.result =JgErrorCode::err_Success;
		this->send(s_nChtC_ModifyAccount_Rsp, &rsp, sizeof(rsp));
	}
	{
		ChtC_Notify_AccountModification rsp;
		rsp.acctID = m_acctID;
		strcpy_k(rsp.nick, sizeof(rsp.nick), m_nick.c_str());
		strcpy_k(rsp.signature, sizeof(rsp.signature), m_signature.c_str());
		strcpy_k(rsp.avatar, sizeof(rsp.avatar), m_avatar.c_str());
		this->broadcastFriends(s_nChtC_Notify_AccountModification, &rsp, sizeof(rsp));
	}

	int n = m_groups.size();
	for(int i=0; i<n; i++)
	{
		KChatGroup* grp = (KChatGroup*)m_groups.at(i);
		ChtC_Notify_GroupMemberModification rsp;
		rsp.grpID = grp->m_groupID;
		rsp.member.acctID = m_acctID;
		strcpy_k(rsp.member.nick, sizeof(rsp.member.nick), m_nick.c_str());
		strcpy_k(rsp.member.signature, sizeof(rsp.member.signature), m_signature.c_str());
		strcpy_k(rsp.member.avatar, sizeof(rsp.member.avatar), m_avatar.c_str());
		grp->broadcastMembers(ChtC_Notify_GroupMemberModification::s_nCode, &rsp, sizeof(rsp), this);
	}
	{
		DB_ModifyAccount* task = DB_ModifyAccount::Alloc(); task->reset();
		task->setAcct(this, this);
		g_pApp->m_databaseAgent.push(*task);
	}
	
	return TRUE;
}

BOOL KChatAccount::removeFriend(KChatAccount* frd)
{
	if(!this->hasFriend(frd->m_acctID))
	{
		ChtC_RemoveFriend_Rsp rsp;
		rsp.result =JgErrorCode::err_Peer_NotFound;
		this->send(ChtC_RemoveFriend_Rsp::s_nCode, &rsp, sizeof(rsp));
		return FALSE;
	}

	m_friends.erase(frd);
	frd->m_friends.erase(this);
	{
		ChtC_RemoveFriend_Rsp rsp;
		rsp.result =JgErrorCode::err_Success;
		rsp.frdID = frd->m_acctID;
		this->send(ChtC_RemoveFriend_Rsp::s_nCode, &rsp, sizeof(rsp));
	}
	{
		ChtC_Notify_RemoveFriend rsp;
		rsp.frdID = m_acctID;
		frd->send(ChtC_Notify_RemoveFriend::s_nCode, &rsp, sizeof(rsp));
	}
	{
		DB_RemoveFriend* task = DB_RemoveFriend::Alloc(); task->reset();
		task->setAcct(this, frd);
		g_pApp->m_databaseAgent.push(*task);
	}

	return TRUE;
}

BOOL KChatAccount::leaveGroup(KChatGroup* grp)
{
	if(!this->isGroupMember(grp->m_groupID))
	{
		ChtC_LeaveGroup_Rsp rsp;
		rsp.result =JgErrorCode::err_AccessDeny; 
		rsp.grpID = grp->m_groupID;
		this->send(ChtC_LeaveGroup_Rsp::s_nCode, &rsp, sizeof(rsp));
		return FALSE;
	}
	grp->m_members.erase(this);
	m_groups.erase(grp);
	{
		ChtC_LeaveGroup_Rsp rsp;
		rsp.result =JgErrorCode::err_Success; 
		rsp.grpID = grp->m_groupID;
		this->send(ChtC_LeaveGroup_Rsp::s_nCode, &rsp, sizeof(rsp));
	}
	{
		ChtC_Notify_LeaveGroup rsp;
		rsp.grpID = grp->m_groupID;
		rsp.acctID = m_acctID;
		grp->broadcastMembers(ChtC_Notify_LeaveGroup::s_nCode, &rsp, sizeof(rsp), this);
	}
	{
		DB_LeaveGroup* task = DB_LeaveGroup::Alloc(); task->reset();
		task->setAcct(grp->m_groupID, this);
		g_pApp->m_databaseAgent.push(*task);
	}

	return TRUE;
}

BOOL KChatAccount::sendMessage(const JgMessageTarget& target, JgMessageContentType::Type contentType, StreamInterface& si)
{
	KChatAccount* receiver = NULL;
	KChatGroup* chatGrp = NULL;
	switch(target.targetType)
	{
	case JgMessageTargetType::GROUP:
		chatGrp = g_pApp->m_grpCache.find(target.targetID);
		if(!chatGrp)
		{
			Log(LOG_ERROR, "error: %s send message, target group:%llu not found", this->toString().c_str(), target.targetID);
			return FALSE;
		} break;
	case JgMessageTargetType::PERSON:
		receiver = g_pApp->m_acctCache.find(target.targetID);
		if(!receiver)
		{
			Log(LOG_ERROR, "error: %s send message, target acct:%llu not found", this->toString().c_str(), target.targetID);
			return FALSE;
		} break;
	case JgMessageTargetType::AVATAR:
		{
			receiver = this;
			break;
		}
	default:
		ASSERT(FALSE); break;
	}

	DWORD seq = g_pApp->m_nextSequence++;

	ChtC_Message_Head messageHead;
	messageHead.sequence = seq;

	messageHead.messageID = g_pApp->nextMessageID();
	messageHead.contentType = contentType;
	messageHead.sender = this->toMessageSource();
	messageHead.mesageTarget = target;
	messageHead.length = 0;

	if(receiver)
		receiver->send(ChtC_Message_Head::s_nCode, &messageHead, sizeof(messageHead));
	else if(chatGrp)
		chatGrp->broadcastMembers(ChtC_Message_Head::s_nCode, &messageHead, sizeof(messageHead), this);

	KBufferOutputStream<2048> so;

	char buf[JgPacketConst::MaxNetDataSize];
	while(TRUE)
	{
		int n = si.ReadData(buf, sizeof(buf));
		so.Seek(0); so.WriteDword(seq); so.WriteByteArray(buf, n);
		if(receiver)
			receiver->send(ChtC_Message_Body::s_nCode, so.data(), so.size());
		else if(chatGrp)
			chatGrp->broadcastMembers(ChtC_Message_Body::s_nCode, so.data(), so.size(), this);
		if(n < sizeof(buf)) break;
	}

	ChtC_Message_End messageEnd; messageEnd.sequence = seq;

	if(receiver)
		receiver->send(ChtC_Message_End::s_nCode, &messageEnd, sizeof(messageEnd));
	else if(chatGrp)
		chatGrp->broadcastMembers(ChtC_Message_End::s_nCode, &messageEnd, sizeof(messageEnd), this);

	return TRUE;
}

BOOL KChatAccount::sendMessage(const JgMessageTarget& target, JgMessageContentType::Type contentType, const void* data, int len)
{
	KChatAccount* receiver = NULL;
	KChatGroup* chatGrp = NULL;
	switch(target.targetType)
	{
	case JgMessageTargetType::GROUP:
		chatGrp = g_pApp->m_grpCache.find(target.targetID);
		if(!chatGrp)
		{
			Log(LOG_ERROR, "error: %s send message, target group:%llu not found", this->toString().c_str(), target.targetID);
			return FALSE;
		} break;
	case JgMessageTargetType::PERSON:
		receiver = g_pApp->m_acctCache.find(target.targetID);
		if(!receiver)
		{
			Log(LOG_ERROR, "error: %s send message, target acct:%llu not found", this->toString().c_str(), target.targetID);
			return FALSE;
		} break;
	case JgMessageTargetType::AVATAR:
		{
			receiver = this;
			break;
		}
	default:
		ASSERT(FALSE); break;
	}

	DWORD seq = g_pApp->m_nextSequence++;

	ChtC_Message_Head messageHead;
	messageHead.sequence = seq;

	messageHead.messageID = g_pApp->nextMessageID();
	messageHead.contentType = contentType;
	messageHead.sender = this->toMessageSource();
	messageHead.mesageTarget = target;
	messageHead.length = len;

	if(receiver)
		receiver->send(ChtC_Message_Head::s_nCode, &messageHead, sizeof(messageHead));
	else if(chatGrp)
		chatGrp->broadcastMembers(ChtC_Message_Head::s_nCode, &messageHead, sizeof(messageHead), this);

	KBufferOutputStream<JgPacketConst::MaxAppDataSize> so;

	const char* bptr = (const char*)data;
	int max_datalen = JgPacketConst::MaxAppDataSize-sizeof(ChtC_Message_Body);
	while(len > 0)
	{
		int datalen = len >  max_datalen ? max_datalen : len;
		so.Seek(0);
		so.WriteDword(seq);
		so.WriteByteArray(bptr, datalen);
		len -= datalen; bptr += datalen;

		if(receiver)
			receiver->send(ChtC_Message_Body::s_nCode, so.data(), so.size());
		else if(chatGrp)
			chatGrp->broadcastMembers(ChtC_Message_Body::s_nCode, so.data(), so.size(), this);
	}

	ChtC_Message_End messageEnd; messageEnd.sequence = seq;

	if(receiver)
		receiver->send(ChtC_Message_End::s_nCode, &messageEnd, sizeof(messageEnd));
	else if(chatGrp)
		chatGrp->broadcastMembers(ChtC_Message_End::s_nCode, &messageEnd, sizeof(messageEnd), this);

	return TRUE;
}

BOOL KChatAccount::sendMessageToFriends(JgMessageContentType::Type messageType, const void* data, int len)
{
	int n = m_friends.size();
	for(int i=0; i<n; i++)
	{
		KChatAccount* frd = (KChatAccount*)m_friends.at(i);
		this->sendMessage(frd->toMessageTarget(), messageType, data, len);
	}
	return TRUE;
}

BOOL KChatAccount::sendMessageToGroups(JgMessageContentType::Type messageType, const void* data, int len)
{
	int n = m_groups.size();
	for(int i=0; i<n; i++)
	{
		KChatGroup* grp = (KChatGroup*)m_groups.at(i);
		this->sendMessage(grp->toMessageTarget(), messageType, data, len);
	}
	return TRUE;
}

/// KChatAccountCache

KChatAccountCache::KChatAccountCache()
{

}

KChatAccountCache::~KChatAccountCache()
{

}

KChatAccount* KChatAccountCache::find(UINT64 acctID)
{
	ChatAcctMap::iterator it = m_acctMap.find(acctID);
	if(it == m_acctMap.end()) return NULL;
	return it->second;
}

KChatAccount* KChatAccountCache::find(const char* acctName)
{
	AcctNameMap::iterator it = m_acctNameMap.find(acctName);
	if(it == m_acctNameMap.end()) return NULL;
	return it->second;
}

BOOL KChatAccountCache::add(KChatAccount* chatAccount)
{
	UINT64 acctID = chatAccount->m_acctID;
	if(this->find(chatAccount->m_acctID))
		return FALSE;
	m_acctMap[acctID] = chatAccount;
	m_acctNameMap[chatAccount->m_acctName.c_str()] = chatAccount;
	return TRUE;
}

BOOL KChatAccountCache::remove(UINT64 acctID)
{
	KChatAccount* chatAcct = NULL;
	ChatAcctMap::iterator it = m_acctMap.find(acctID);
	
	if(it != m_acctMap.end())
	{
		chatAcct = it->second;
		m_acctMap.erase(it);
	}

	if(chatAcct)
	{
		AcctNameMap::iterator it2 = m_acctNameMap.find(chatAcct->m_acctName.c_str());
		if(it2 != m_acctNameMap.end()) m_acctNameMap.erase(it2);
	}
	
	return TRUE;
}

/// 

KChatCandidateManager::KChatCandidateManager()
{

}

KChatCandidateManager::~KChatCandidateManager()
{

}

KChatAccount* KChatCandidateManager::find(const char* acctName)
{
	CandidateMap::iterator it = m_candidateMap.find(acctName);
	if(it == m_candidateMap.end()) return NULL;
	Candidate& candidate = it->second;
	return candidate.chatAcct;
}

BOOL KChatCandidateManager::add(KChatAccount* chatAcct)
{
	const char* acctName = chatAcct->m_acctName.c_str();
	if(this->find(acctName)) return FALSE;
	Candidate candidate;
	candidate.chatAcct = chatAcct;
	candidate.loginTime = time(NULL);
	m_candidateMap[acctName] = candidate;
	return TRUE;
}

BOOL KChatCandidateManager::remove(KChatAccount* chatAcct)
{
	const char* acctName = chatAcct->m_acctName.c_str();
	CandidateMap::iterator it = m_candidateMap.find(acctName);
	if(it == m_candidateMap.end()) return FALSE;
	m_candidateMap.erase(it);
	return TRUE;
}

void KChatCandidateManager::process_CCht_Login(KSocket_Client* clnSocket, CCht_Login* req)
{
	const char* acctName = req->acctName;

	KChatAccount* chatAcct = KChatAccount::Alloc(); chatAcct->reset();
	chatAcct->m_state = KChatAccount::authenticate;
	chatAcct->m_acctName = acctName;
	memcpy(&chatAcct->m_password, &req->pwdMd5, sizeof(chatAcct->m_password));
	chatAcct->setSocket(clnSocket);

	ASSERT(this->add(chatAcct));

	DB_Authenticate* task = DB_Authenticate::Alloc(); task->reset();
	task->setAcct(chatAcct);
	g_pApp->m_databaseAgent.push(*task);
	return;
}

void KChatCandidateManager::process_CCht_ConnClosed(KSocket_Client* clnSocket)
{
	KChatAccount* chatAcct = clnSocket->m_chatAcct; ASSERT(chatAcct);
	chatAcct->setSocket(NULL);
	this->remove(chatAcct);
	chatAcct->reset(); KChatAccount::Free(chatAcct);
	return;
}
