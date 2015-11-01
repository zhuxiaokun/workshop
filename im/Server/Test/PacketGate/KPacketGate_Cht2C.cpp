#include "KPacketGate_Cht2C.h"
#include "../KTestClient.h"
#include <System/Misc/KStream.h>
#include <JgMessageDefine.h>
#include <JgerrMessage.h>
#include <System/Misc/KDatetime.h>

/////////////////////////////////////////////////////////////////
const char* getUserDirectory()
{
	static char* cstr = "./data";
	return cstr;
}

bool savePersonMessage(KChatMessage* msg)
{
	return true;
}

bool saveGroupMessage(KChatMessage* msg)
{
	return true;
}

bool saveMessageToDB(KChatMessage* msg)
{
	switch(msg->m_messageTarget.targetType)
	{
	case JgMessageTargetType::PERSON:
		return savePersonMessage(msg);
	case JgMessageTargetType::GROUP:
		return saveGroupMessage(msg);
	default: return true;
	}
}

bool saveMessageToDB(KOutgoingMessage* msg)
{
	return true;
}

/////////////////////////////////////////////////////////////////

KPacketGate_Cht2C::KPacketGate_Cht2C(void)
{
	this->Register(s_nChtC_Connected, &KPacketGate_Cht2C::process_Connected);
	this->Register(s_nChtC_ConnClosed, &KPacketGate_Cht2C::process_ConnClosed);
	this->Register(s_nChtC_ConnError, &KPacketGate_Cht2C::process_ConnError);
	this->Register(s_nChtC_LoginRsp, &KPacketGate_Cht2C::process_LoginRsp);
	this->Register(s_nChtC_AcctInfo, &KPacketGate_Cht2C::process_AcctInfo);
	this->Register(s_nChtC_FriendInfo, &KPacketGate_Cht2C::process_FriendInfo);
	this->Register(s_nChtC_GroupInfo, &KPacketGate_Cht2C::process_GroupInfo);
	this->Register(s_nChtC_GroupDetail, &KPacketGate_Cht2C::process_GroupDetail);
	this->Register(s_nChtC_NotifyEnter, &KPacketGate_Cht2C::process_NotifyEnter);
	this->Register(s_nChtC_NotifyLeave, &KPacketGate_Cht2C::process_NotifyLeave);
	this->Register(s_nChtC_AddFriend_Rsp, &KPacketGate_Cht2C::process_AddFriend_Rsp);
	this->Register(s_nChtC_AddFriend_Invite, &KPacketGate_Cht2C::process_AddFriend_Invite);
	this->Register(s_nChtC_JoinGroup_Rsp, &KPacketGate_Cht2C::process_JoinGroup_Rsp);
	this->Register(s_nChtC_CreateGroup_Rsp, &KPacketGate_Cht2C::process_CreateGroup_Rsp);
	this->Register(s_nChtC_ModifyAccount_Rsp, &KPacketGate_Cht2C::process_ChtC_ModifyAccount_Rsp);
	this->Register(s_nChtC_Notify_AccountModification, &KPacketGate_Cht2C::process_ChtC_Notify_AccountModification);
	this->Register(s_nChtC_Notify_GroupMemberModification, &KPacketGate_Cht2C::process_ChtC_Notify_GroupMemberModification);
	this->Register(s_nChtC_RemoveFriend_Rsp, &KPacketGate_Cht2C::process_ChtC_RemoveFriend_Rsp);
	this->Register(s_nChtC_Notify_RemoveFriend, &KPacketGate_Cht2C::process_ChtC_Notify_RemoveFriend);
	this->Register(s_nChtC_LeaveGroup_Rsp, &KPacketGate_Cht2C::process_ChtC_LeaveGroup_Rsp);
	this->Register(s_nChtC_Notify_LeaveGroup, &KPacketGate_Cht2C::process_ChtC_Notify_LeaveGroup);
	this->Register(s_nChtC_Message_Head, &KPacketGate_Cht2C::process_ChtC_Message_Head);
	this->Register(s_nChtC_Message_Body, &KPacketGate_Cht2C::process_ChtC_Message_Body);
	this->Register(s_nChtC_Message_End, &KPacketGate_Cht2C::process_ChtC_Message_End);
	this->Register(s_nChtC_JoinGroup_Invite, &KPacketGate_Cht2C::process_ChtC_JoinGroup_Invite);
	this->Register(s_nChtC_AddGroupMember_Rsp, &KPacketGate_Cht2C::process_ChtC_AddGroupMember_Rsp);
	this->Register(s_nChtC_Notify_JoinGroup, &KPacketGate_Cht2C::process_ChtC_Notify_JoinGroup);
	this->Register(s_nChtC_Message_Rsp, &KPacketGate_Cht2C::process_ChtC_Message_Rsp);
	this->Register(s_nChtC_SetAvatar_Rsp, &KPacketGate_Cht2C::process_ChtC_SetAvatar_Rsp);
	this->Register(s_nChtC_QueryAvatar_Rsp, &KPacketGate_Cht2C::process_ChtC_QueryAvatar_Rsp);
	this->Register(s_nChtC_Message_Ack, &KPacketGate_Cht2C::process_ChtC_Message_Ack);
	this->Register(s_nChtC_QueryAccount_Rsp, &KPacketGate_Cht2C::process_ChtC_QueryAccount_Rsp);
	this->Register(s_nChC_QueryGroupByID_Rsp, &KPacketGate_Cht2C::process_ChtC_QueryGroupByID_Rsp);
	this->Register(s_nChC_QueryGroupByNick_Rsp, &KPacketGate_Cht2C::process_ChtC_QueryGroupByNick_Rsp);
	this->Register(s_nChC_QueryAccountByNick_Rsp, &KPacketGate_Cht2C::process_ChtC_QueryAccountByNick_Rsp);
}

KPacketGate_Cht2C::~KPacketGate_Cht2C(void)
{

}

void KPacketGate_Cht2C::process_Connected(KSocket_Chat* pSock, const void* data, int len)
{
	Log(LOG_CONSOLE, "connected to sever %s", pSock->ToString().c_str());
}

void KPacketGate_Cht2C::process_ConnClosed(KSocket_Chat* pSock, const void* data, int len)
{
	Log(LOG_CONSOLE, "disconnected from %s", pSock->ToString().c_str());
	g_client->m_chatSocket->ReleaseRef();
	g_client->m_chatSocket = NULL;
	g_client->clear();
}

void KPacketGate_Cht2C::process_ConnError(KSocket_Chat* pSock, const void* data, int len)
{
	Log(LOG_CONSOLE, "ConnError");
	g_client->m_chatSocket->ReleaseRef();
	g_client->m_chatSocket = NULL;
}

void KPacketGate_Cht2C::process_LoginRsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_LoginRsp* rsp = (ChtC_LoginRsp*)data;
	Log(LOG_CONSOLE, "LoginRsp: result %d----%s", rsp->result,  JgErrorMessage::getErrorMeaage(rsp->result));
}

void KPacketGate_Cht2C::process_AcctInfo(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_AcctInfo* rsp = (ChtC_AcctInfo*)data;
	Log(LOG_CONSOLE, "AcctInfo: passport:%llu nick:%s signature:%s", rsp->passport, rsp->nick, rsp->signature);
	g_client->m_acctID = rsp->passport;
	g_client->m_nick = rsp->nick;
	g_client->m_signature = rsp->signature;
	g_client->m_avatar = rsp->avatar;

	// 获取今天得离线消息
	{
		KDatetime ndt;
		KLocalDatetime ldt = ndt.GetLocalTime();
		int yyyymmdd = ldt.year * 10000 + ldt.month * 100 + ldt.day;
		JgMessageID lastMid = {yyyymmdd, 0};

		CCht_QueryMessage req;
		req.lastGroupMessage = lastMid;
		req.lastPersonMessage = lastMid;
		pSock->Send(CCht_QueryMessage::s_nCode, &req, sizeof(req));
	}
}

void KPacketGate_Cht2C::process_FriendInfo(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_FriendInfo* rsp = (ChtC_FriendInfo*)data;
	Log(LOG_CONSOLE, "FriendInfo: count %d", rsp->friendCount);
	int n = abs(rsp->friendCount);
	for(int i=0; i<n; i++)
	{
		Log(LOG_CONSOLE, "\tAcctID:%llu nick:%s signature:%s avatar:%s",
			rsp->friends[i].acctID,
			rsp->friends[i].nick,
			rsp->friends[i].signature,
			rsp->friends[i].avatar);
		ChatAccount acct;
		acct.m_acctID = rsp->friends[i].acctID;
		acct.m_nick = rsp->friends[i].nick;
		acct.m_signature = rsp->friends[i].signature;
		acct.m_avatar = rsp->friends[i].avatar;
		g_client->_addFriend(acct);
	}
}

void KPacketGate_Cht2C::process_GroupInfo(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_GroupInfo* rsp = (ChtC_GroupInfo*)data;
	Log(LOG_CONSOLE, "GroupInfo: count %d", rsp->groupCount);

	int n = abs(rsp->groupCount);
	for(int i=0; i<n; i++)
	{
		Log(LOG_CONSOLE, "\tGrpID:%llu nick:%s signature:%s",
			rsp->groups[i].groupID,
			rsp->groups[i].nick,
			rsp->groups[i].signature);

		ChatGroup grp;
		grp.m_groupID = rsp->groups[i].groupID;
		grp.m_nick = rsp->groups[i].nick;
		grp.m_signature = rsp->groups[i].signature;
		g_client->_joinGroup(grp);
	}
}

void KPacketGate_Cht2C::process_GroupDetail(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_GroupDetail* rsp = (ChtC_GroupDetail*)data;
	Log(LOG_CONSOLE, "GroupDetail: grp:%llu member:%d", rsp->grpID, rsp->memberCount);

	KChatGroup* chatGrp = g_client->getGroup(rsp->grpID);
	if(!chatGrp) return;

	//chatGrp->clearMembers();
	int n = abs(rsp->memberCount);
	for(int i=0; i<n; i++)
	{
		Log(LOG_CONSOLE, "\tAcctID:%llu nick:%s signature:%s avatar:%s",
			rsp->members[i].passport,
			rsp->members[i].nick,
			rsp->members[i].signature,
			rsp->members[i].avatar);

		ChatAccount acct;
		acct.m_acctID = rsp->members[i].passport;
		acct.m_nick = rsp->members[i].nick;
		acct.m_signature = rsp->members[i].signature;
		acct.m_avatar = rsp->members[i].avatar;
		chatGrp->addMember(acct);
	}
}

void KPacketGate_Cht2C::process_NotifyEnter(KSocket_Chat* pSock, const void* data, int len)//上线通知
{
	ChtC_NotifyEnter* rsp = (ChtC_NotifyEnter*)data;
	Log(LOG_CONSOLE, "NotifyEnter: %llu", rsp->acctID);
}

void KPacketGate_Cht2C::process_NotifyLeave(KSocket_Chat* pSock, const void* data, int len)//下线通知
{
	ChtC_NotifyLeave* rsp = (ChtC_NotifyLeave*)data;
	Log(LOG_CONSOLE, "NotifyLeave: %llu", rsp->acctID);
}

void KPacketGate_Cht2C::process_AddFriend_Rsp(KSocket_Chat* pSock, const void* data, int len)//添加好友通知
{
	ChtC_AddFriend_Rsp* rsp = (ChtC_AddFriend_Rsp*)data;
	Log(LOG_CONSOLE, "AddFriend: result:%d-----%s", rsp->result,JgErrorMessage::getErrorMeaage(rsp->result));
	if(!rsp->result)
	{
		ChatAccount acct;
		acct.m_acctID = rsp->acctID;
		acct.m_nick = rsp->nick;
		acct.m_signature = rsp->signature;
		g_client->_addFriend(acct);
	}
}

void KPacketGate_Cht2C::process_AddFriend_Invite(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_AddFriend_Invite* rsp = (ChtC_AddFriend_Invite*)data;
	Log(LOG_CONSOLE, "AddFriendInvite: from %llu note:%s", rsp->acctID, rsp->note);
	CCht_AddFriend_Invite_Rsp rsp2;
	rsp2.acctID = rsp->acctID;
	rsp2.result = 0;
	pSock->Send(s_nCCht_AddFriend_Invite_Rsp, &rsp2, sizeof(rsp2));
}

void KPacketGate_Cht2C::process_JoinGroup_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_JoinGroup_Rsp* rsp = (ChtC_JoinGroup_Rsp*)data;
	Log(LOG_CONSOLE, "JoinGroupRsp: result %d-----%s", rsp->result,JgErrorMessage::getErrorMeaage(rsp->result));
}

void KPacketGate_Cht2C::process_CreateGroup_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_CreateGroup_Rsp* rsp = (ChtC_CreateGroup_Rsp*)data;
	Log(LOG_CONSOLE, "CreateGroupRsp:  result %d-----%s", rsp->result,JgErrorMessage::getErrorMeaage(rsp->result));
}

void KPacketGate_Cht2C::process_ChtC_ModifyAccount_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_ModifyAccount_Rsp* rsp = (ChtC_ModifyAccount_Rsp*)data;
	Log(LOG_CONSOLE, "ModifyAccount_Rsp:  result %d-----%s", rsp->result,JgErrorMessage::getErrorMeaage(rsp->result));
	if(!rsp->result)
	{
		g_client->m_nick = (const char*)g_client->m_tmpTab["ModifyAccount"]["nick"];
		g_client->m_signature = (const char*)g_client->m_tmpTab["ModifyAccount"]["signature"];
	}
}

void KPacketGate_Cht2C::process_ChtC_Notify_AccountModification(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_Notify_AccountModification* rsp = (ChtC_Notify_AccountModification*)data;
	Log(LOG_CONSOLE, "AccountModification: acct:%llu nick:%s signature:%s",
		rsp->acctID, rsp->nick, rsp->signature);

	ChatAccount* acct = g_client->getFriend(rsp->acctID);
	if(acct)
	{
		acct->m_nick = rsp->nick;
		acct->m_signature = rsp->signature;
	}
}

void KPacketGate_Cht2C::process_ChtC_Notify_GroupMemberModification(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_Notify_GroupMemberModification* rsp = (ChtC_Notify_GroupMemberModification*)data;
	Log(LOG_CONSOLE, "debug: Notify_GroupMemberModification grp:%llu acct:%llu nick:%s", rsp->grpID, rsp->member.acctID, rsp->member.nick);

	KChatGroup* grp = g_client->getGroup(rsp->grpID);
	if(grp)
	{
		ChatAccount* acct = grp->getMember(rsp->member.acctID);
		if(acct)
		{
			acct->m_nick = rsp->member.nick;
			acct->m_signature = rsp->member.signature;
		}
	}
}

void KPacketGate_Cht2C::process_ChtC_RemoveFriend_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_RemoveFriend_Rsp* rsp = (ChtC_RemoveFriend_Rsp*)data;
	Log(LOG_CONSOLE, "debug: RemoveFriend_Rsp result:%d-----%s  friend:%llu", rsp->result,JgErrorMessage::getErrorMeaage(rsp->result), rsp->frdID);
	if(!rsp->result)
		g_client->_removeFriend(rsp->frdID);
}

void KPacketGate_Cht2C::process_ChtC_QueryAccount_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_QueryAccount_Rsp* rsp=(ChtC_QueryAccount_Rsp*)data;
	if(rsp->result)
	{
		Log(LOG_CONSOLE, "debug: QueryAccount_Rsp AccountID:%llu      result:%d-----%s", rsp->passport,rsp->result,JgErrorMessage::getErrorMeaage(rsp->result));
	}
	else
	{
		Log(LOG_CONSOLE, "AcctInfo: passport:%llu nick:%s signature:%s  avatar:%s ", rsp->passport, rsp->nick, rsp->signature,rsp->avatar);
	}
	return;
}
void KPacketGate_Cht2C::process_ChtC_Notify_RemoveFriend(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_Notify_RemoveFriend* rsp = (ChtC_Notify_RemoveFriend*)data;
	Log(LOG_CONSOLE, "debug: Notify_RemoveFriend %llu", rsp->frdID);
	g_client->_removeFriend(rsp->frdID);
}

void KPacketGate_Cht2C::process_ChtC_LeaveGroup_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_LeaveGroup_Rsp* rsp = (ChtC_LeaveGroup_Rsp*)data;
	Log(LOG_CONSOLE, "debug: LeaveGroup_Rsp grp:%llu result:%d-----%s", rsp->grpID, rsp->result,JgErrorMessage::getErrorMeaage(rsp->result));
	if(!rsp->result)
		g_client->_leaveGroup(rsp->grpID);
}

void KPacketGate_Cht2C::process_ChtC_Notify_LeaveGroup(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_Notify_LeaveGroup* rsp = (ChtC_Notify_LeaveGroup*)data;
	Log(LOG_CONSOLE, "debug: Notify_LeaveGroup grp:%llu acct:%llu", rsp->grpID, rsp->acctID);
	
	KChatGroup* grp = g_client->getGroup(rsp->grpID);
	if(grp) grp->removeMember(rsp->acctID);
}

void KPacketGate_Cht2C::process_ChtC_Message_Head(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_Message_Head* messageHead = (ChtC_Message_Head*)data;
	g_client->m_chatSession.on_Message_Head(messageHead, len);
}

void KPacketGate_Cht2C::process_ChtC_Message_Body(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_Message_Body* messageBody = (ChtC_Message_Body*)data;
	g_client->m_chatSession.on_Message_Body(messageBody, len);
}

void KPacketGate_Cht2C::process_ChtC_Message_End(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_Message_End* messageEnd = (ChtC_Message_End*)data;
	g_client->m_chatSession.on_Message_End(messageEnd, len);
	KChatMessage* chatMessage = g_client->m_chatSession.getMessage(messageEnd->sequence);
	if(chatMessage->m_contentType == JgMessageContentType::SYS_COMMAND)
	{
		JgMessageRequest* request = (JgMessageRequest*)chatMessage->m_messageData.data();
		switch(request->m_requestType)
		{
		case JgMessageRequestType::AddFriend:
			{
				JgMessageRequest_AddFriend* addfrd = (JgMessageRequest_AddFriend*)request;
				CCht_AddFriend_Invite_Rsp rsp;
				rsp.acctID = addfrd->m_initialAcctID;
				rsp.result = 0;
				pSock->Send(CCht_AddFriend_Invite_Rsp::s_nCode, &rsp, sizeof(rsp));
				break;
			}
		case JgMessageRequestType::AddMember:
			{
				JgMessageRequest_AddMember * addMem= (JgMessageRequest_AddMember*)request;
				CCht_JoinGroup_Invite_Rsp rsp;
				rsp.requestID=addMem->m_requestID;
				rsp.result=0;
				pSock->Send(CCht_JoinGroup_Invite_Rsp::s_nCode, &rsp, sizeof(rsp));
				break;
			}
		}
	}
	g_client->_saveMessage(chatMessage);
	g_client->m_chatSession.removeMessage(messageEnd->sequence);
}
void KPacketGate_Cht2C::process_ChtC_JoinGroup_Invite(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_JoinGroup_Invite* req = (ChtC_JoinGroup_Invite*)data;
	Log(LOG_CONSOLE, "JoinGroupInvite: from member %llu  InGroup %llu note:%s", req->initialAcctID,req->groupID,req->note);
	CCht_JoinGroup_Invite_Rsp rsp;
	rsp.requestID = req->requestID;
	rsp.result = 0;
	pSock->Send(CCht_JoinGroup_Invite_Rsp::s_nCode, &rsp, sizeof(rsp));
}

void KPacketGate_Cht2C::process_ChtC_AddGroupMember_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_AddGroupMember_Rsp* rsp = (ChtC_AddGroupMember_Rsp*)data;
	Log(LOG_WARN, "AddGroupMember_Rsp: result %d-----%s", rsp->result,JgErrorMessage::getErrorMeaage(rsp->result));
}

void KPacketGate_Cht2C::process_ChtC_Notify_JoinGroup(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_Notify_JoinGroup* notify = (ChtC_Notify_JoinGroup*)data;
	Log(LOG_WARN, "notify: %llu %s join group %llu",
		notify->member.acctID, notify->member.nick, notify->grpID);
}

void KPacketGate_Cht2C::process_ChtC_Message_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_Message_Rsp* rsp = (ChtC_Message_Rsp*)data;
	Log(LOG_CONSOLE, "MessageRsp: seq:%u messageID:%08d-%04d result:%d------%s",
		rsp->sequence, rsp->messageID.yyyy_mm_dd,
		rsp->messageID.sequence, rsp->result, JgErrorMessage::getErrorMeaage(rsp->result));
}

void KPacketGate_Cht2C::process_ChtC_SetAvatar_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_SetAvatar_Rsp* rsp = (ChtC_SetAvatar_Rsp*)data;
	Log(LOG_CONSOLE, "SetAvatar: result:%d(%s) name:%s", rsp->result,  JgErrorMessage::getErrorMeaage( rsp->result)  ,rsp->avatarName);
	if(!rsp->result) g_client->m_avatar = rsp->avatarName;
}

void KPacketGate_Cht2C::process_ChtC_QueryAvatar_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_QueryAvatar_Rsp* rsp = (ChtC_QueryAvatar_Rsp*)data;
	Log(LOG_CONSOLE, "QueryAvatar: seq:%d acct:%llu result:%d(%s) avatar:%s", rsp->sequence, rsp->acctID, rsp->result, JgErrorMessage::getErrorMeaage(rsp->result),rsp->avatarName);
	if(!rsp->result)
	{
		ChatAccount* acct = g_client->_findAccount(rsp->acctID);
		acct->m_avatar = rsp->avatarName;
	}
}

void KPacketGate_Cht2C::process_ChtC_Message_Ack(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_Message_Ack* ack = (ChtC_Message_Ack*)data;
	KOutgoingMessage* msg = g_client->m_chatSession.getOutgoingMessage(ack->sequence);

	if(!msg)
	{
		Log(LOG_ERROR, "ChtC_Message_Ack: sequence:%u not found", ack->sequence);//%llu话输出有问题
		return;
	}

	if(ack->result)
	{
		Log(LOG_ERROR, "error: ChtC_Message_Ack result:%d", ack->result);
		g_client->m_chatSession.removeOutGoingMessage(ack->sequence);
		return;
	}

	msg->m_messageID = ack->messageID;
	msg->m_messageTime = ack->messageTime;
	msg->save();
	g_client->m_chatSession.removeOutGoingMessage(ack->sequence);
}


void KPacketGate_Cht2C::process_ChtC_QueryGroupByID_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_GroupInfo* rsp = (ChtC_GroupInfo*)data;
	int n = abs(rsp->groupCount);
	if(n==0)
	{
		Log(LOG_CONSOLE, "Error found Group");
		return;
	}
	for(int i=0; i<n; i++)
	{
		Log(LOG_CONSOLE, "\tGrpID:%llu nick:%s signature:%s",
			rsp->groups[i].groupID,
			rsp->groups[i].nick,
			rsp->groups[i].signature);
   }
}

void KPacketGate_Cht2C::process_ChtC_QueryGroupByNick_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
	ChtC_GroupInfo* rsp = (ChtC_GroupInfo*)data;
	int n = abs(rsp->groupCount);
	if(n==0)
	{
		Log(LOG_CONSOLE, "Error found Group");
		return;
	}
	for(int i=0; i<n; i++)
	{
		Log(LOG_CONSOLE, "\tGrpID:%llu nick:%s signature:%s",
			rsp->groups[i].groupID,
			rsp->groups[i].nick,
			rsp->groups[i].signature);
	}
	return;
}

void KPacketGate_Cht2C:: process_ChtC_QueryAccountByNick_Rsp(KSocket_Chat* pSock, const void* data, int len)
{
		ChtC_AcctListInfo *rsp=(ChtC_AcctListInfo *)data;
		int n=abs(rsp->AcctCount);
			 if(n==0)
			{
				Log(LOG_CONSOLE, "^_^为查找到相应昵称的账户,请确认输入的nick是否准确^_^");
				return;
			}

			 for(int i=0;i<n; i++)
			 {
					Log(LOG_CONSOLE, "AcctInfo: passport:%llu nick:%s signature:%s  avatar:%s ", 
						rsp->members[i].passport, 
						rsp->members[i].nick, 
						rsp->members[i].signature,
						rsp->members[i].avatar);
			 }
			
			return;
}
