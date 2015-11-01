#include "JgRequestManager.h"
#include<time.h>
#include "../KDemoChatServerApp.h"
#include "../Storage/FS_SaveMessage_2.h"
#include<JgerrorCode.h>
JgRequestManager::JgRequestManager():m_nextRequestID(0)
{

}

JgRequestManager::~JgRequestManager()
{

}

void JgRequestManager::processRequest(JgMessageRequest* request)
{
	switch(request->m_requestType)
	{
	case JgMessageRequestType::AddFriend:
		process_AddFriend((JgMessageRequest_AddFriend*)request);
		break;
	case JgMessageRequestType::RemoveFriend:
		process_RemoveFriend((JgMessageRequest_RemoveFriend*)request);
		break;
	case JgMessageRequestType::AddMember:
		process_AddMember((JgMessageRequest_AddMember*)request);
		break;
	case JgMessageRequestType::RemoveMember:
		process_RemoveMember((JgMessageRequest_RemoveMember*)request);
		break;
	case JgMessageRequestType::AddFriendAck:
		process_AddFriendAck((JgMessageRequest_AddFriendAck*)request);
		break;
	case  JgMessageRequestType::JoinGroup:
		process_JoinGroup((JgMessageRequest_JoinGroup*)request);
		break;
	case  JgMessageRequestType::QueryGroup:
		process_QueryGroup((JgMessageRequest_QueryGroup*)request);
		break;
	case  JgMessageRequestType::QueryAccount:
		process_QueryAccount((JgMessageRequest_QueryAccount*)request);
		break;
	case JgMessageRequestType::QueryGroupByID:
		process_QueryGroupByID((JgMessageRequest_QueryGroupByID*)request);
		break;
	case JgMessageRequestType::QueryGroupByNick:
		process_QueryGroupByNick((JgMessageRequest_QueryGroupByNick*)request);
		break;
	}
}

DWORD JgRequestManager::AddRequest_AddMember(KChatAccount* initial, KChatGroup* grp, UINT64 acctID)
{
	JgMessageRequest_AddMember* request = new JgMessageRequest_AddMember();
	
	request->m_requestID = m_nextRequestID++;
	request->m_requestType = JgMessageRequestType::AddMember;
	request->m_requestTime = time(NULL);
	request->m_groupID = grp->m_groupID;
	request->m_initialAcctID = initial->m_acctID;
	request->m_memberAcctID = acctID;
	m_requestMap[request->m_requestID] = request;
	return request->m_requestID;
}

JgMessageRequest_AddMember* JgRequestManager::FindRequest_AddMember(DWORD requestID)
{
	RequestMap::iterator it = m_requestMap.find(requestID);
	if(it == m_requestMap.end()) return NULL;
	JgMessageRequest* request = it->second;
	if(request->m_requestType != JgMessageRequestType::AddMember) return NULL;
	return (JgMessageRequest_AddMember*)request;
}

bool JgRequestManager::RemoveRequest(DWORD requestID)
{
	RequestMap::iterator it = m_requestMap.find(requestID);
	if(it == m_requestMap.end()) return false;
	JgMessageRequest* request = it->second;
	delete request;
	m_requestMap.erase(it);
	return true;
}

void JgRequestManager::process_AddFriend(JgMessageRequest_AddFriend* request)//添加好友
{
	KChatAccount* chatAcct = NULL;
	UINT64 friendID = request->m_friendAcctID;
	const char* friendAcctName = request->m_friendAcctName.c_str();
	
	if(friendID) chatAcct = g_pApp->m_acctCache.find(friendID);
	else chatAcct = g_pApp->m_acctCache.find(friendAcctName);

	if(!chatAcct) return;

	friendID = chatAcct->m_acctID;
	request->m_friendAcctID = friendID;

	if(chatAcct->isSocketReady())
	{
		ChtC_AddFriend_Invite rsp;
		rsp.acctID = request->m_initialAcctID;
		strcpy_k(rsp.note, sizeof(rsp.note), request->m_note);
		chatAcct->send(ChtC_AddFriend_Invite::s_nCode, &rsp, sizeof(rsp));
	}
	else
	{
		FS_SaveMessage_2* task = FS_SaveMessage_2::Alloc(); task->reset();
		task->m_messageID = g_pApp->nextMessageID();
		task->m_messageSource.sourceType = JgMessageTargetType::PERSON;
		task->m_messageSource.sourceID = request->m_initialAcctID;
		task->m_messageTarget = chatAcct->toMessageTarget();
		task->m_contentType = JgMessageContentType::SYS_COMMAND;
		task->m_messageData.write(request, sizeof(*request));
		task->m_messageTime = time(NULL);
		g_pApp->m_storageAgent.Push(task);
	}
}

void JgRequestManager::process_JoinGroup(JgMessageRequest_JoinGroup* request)//加群操作
{
	   KChatGroup* chatGrp = g_pApp->m_grpCache.find(request->m_group_nick.c_str());
	   UINT64 m_AccID = request->m_initialAcctID;
	   KChatAccount* chatAcct = g_pApp->m_acctCache.find(m_AccID);
	   if(chatAcct->isSocketReady())
	   {
			if(chatGrp)
			{
						chatAcct->joinGroup(chatGrp);
			}
			else
			{
				ChtC_JoinGroup_Rsp rsp;
				rsp.result =JgErrorCode::err_Group_NotFound;
				chatAcct->send(s_nChtC_JoinGroup_Rsp, &rsp, sizeof(rsp));			
			}
	   }
	   else
	   {
		   if(chatGrp)
		   {
			   chatAcct->joinGroup(chatGrp);
		   }
	   }
	  
}
void JgRequestManager::process_RemoveFriend(JgMessageRequest_RemoveFriend* request)//删除好友
{
		
}

void JgRequestManager::process_AddMember(JgMessageRequest_AddMember* request)
{

}

void JgRequestManager::process_RemoveMember(JgMessageRequest_RemoveMember* request)
{

}


void JgRequestManager::process_QueryGroup(JgMessageRequest_QueryGroup* request)
{
	KChatGroup* chatGrp = g_pApp->m_grpCache.find(request->grpID);
	UINT64 m_AccID = request->m_initialAcctID;
	KChatAccount* chatAcct = g_pApp->m_acctCache.find(m_AccID);
	if(chatAcct->isSocketReady())
	{
		if(chatGrp)
		{
			chatAcct->sendGroupDetail(chatGrp);
		}
		else
		{
			ChtC_JoinGroup_Rsp rsp;
			rsp.result =JgErrorCode::err_Group_NotFound;
			chatAcct->send(s_nChtC_JoinGroup_Rsp, &rsp, sizeof(rsp));			
		}
	}
}


void JgRequestManager::process_QueryAccount(JgMessageRequest_QueryAccount* request)//
{
		UINT64 acctID =request->acctID;
		UINT64 chatAcctID =request->m_initialAcctID;
		KChatAccount* m_chatAcct=g_pApp->m_acctCache.find(chatAcctID);
		KChatAccount* Acct=g_pApp->m_acctCache.find(acctID);
		if(m_chatAcct)
		{
				if(Acct)
				{
					ChtC_QueryAccount_Rsp rsp;
					rsp.result=JgErrorCode::err_Success;
					rsp.passport = Acct->m_acctID;
					strcpy_k(rsp.nick, sizeof(rsp.nick),Acct->m_nick.c_str());
					strcpy_k(rsp.signature, sizeof(rsp.signature), Acct->m_signature.c_str());
					strcpy_k(rsp.avatar, sizeof(rsp.avatar),Acct->m_avatar.c_str());
					m_chatAcct->send(s_nChtC_QueryAccount_Rsp, &rsp, sizeof(rsp));
					return;
				}
				else
				{
					ChtC_QueryAccount_Rsp rsp;
					rsp.result =JgErrorCode::err_AccountNotFound;
					rsp.passport=request->acctID;
					m_chatAcct->send(s_nChtC_QueryAccount_Rsp, &rsp, sizeof(rsp));
					return;
				}
		}
}

void JgRequestManager::process_AddFriendAck(JgMessageRequest_AddFriendAck* request)
{
	UINT64 acctID =request->m_initialAcctID;
	UINT64 friendID =request->m_friendID;

	KChatAccount* chatAcct =g_pApp->m_acctCache.find(acctID);
	if(!chatAcct) return;

	KChatAccount* frd =g_pApp->m_acctCache.find(friendID);
	if(!frd) return;

	chatAcct->addFriend(frd);
	{
		ChtC_FriendInfo_Tmpl<1> rsp;
		rsp.friendCount = -1;
		rsp.friends[0].acctID = frd->m_acctID;
		strcpy_k(rsp.friends[0].nick, sizeof(rsp.friends[0].nick), frd->m_nick.c_str());
		strcpy_k(rsp.friends[0].signature, sizeof(rsp.friends[0].signature), frd->m_signature.c_str());
		strcpy_k(rsp.friends[0].avatar, sizeof(rsp.friends[0].avatar), frd->m_avatar.c_str());
		chatAcct->send(s_nChtC_FriendInfo, &rsp, sizeof(rsp));
	}
   {
		ChtC_FriendInfo_Tmpl<1> rsp;
		rsp.friendCount = -1;
		rsp.friends[0].acctID = chatAcct->m_acctID;
		strcpy_k(rsp.friends[0].nick, sizeof(rsp.friends[0].nick), chatAcct->m_nick.c_str());
		strcpy_k(rsp.friends[0].signature, sizeof(rsp.friends[0].signature), chatAcct->m_signature.c_str());
		strcpy_k(rsp.friends[0].avatar, sizeof(rsp.friends[0].avatar), chatAcct->m_avatar.c_str());
		frd->send(s_nChtC_FriendInfo, &rsp, sizeof(rsp));
	}
}

void JgRequestManager::process_QueryGroupByID(JgMessageRequest_QueryGroupByID* request)
{
	UINT64 acctID =request->m_initialAcctID;
	UINT64  grpID=request->m_group_ID;
	KChatGroup * target=g_pApp->m_grpCache.find(grpID);
	KChatAccount * chatAcct=g_pApp->m_acctCache.find(acctID);

	if(chatAcct)
	{
		KBufferOutputStream<1024*32> so;
			if(target)
			{
				so.WriteShort(-1);
				ChtC_GroupInfo::Group grp;
				grp.groupID=target->m_groupID;
				strcpy_k(grp.nick, sizeof(grp.nick),target->m_nick.c_str());
				strcpy_k(grp.signature, sizeof(grp.signature), target->m_signature.c_str());
				so.WriteByteArray(&grp, sizeof(grp));
				chatAcct->send(s_nChC_QueryGroupByID_Rsp, so.data(), so.size());
				return;
			}
			else
			{
					so.WriteShort(0);
					chatAcct->send(s_nChC_QueryGroupByID_Rsp, so.data(), so.size());
					return ;
			}
	}
}

void JgRequestManager::process_QueryGroupByNick(JgMessageRequest_QueryGroupByNick* request)
{
	UINT64 acctID =request->m_initialAcctID;
	const char * nick=request->grpNick;
	KChatGroup * target=g_pApp->m_grpCache.find(nick);
	KChatAccount * chatAcct=g_pApp->m_acctCache.find(acctID);

	if(chatAcct)
	{
		KBufferOutputStream<1024*32> so;
		if(target)
		{
			so.WriteShort(-1);
			ChtC_GroupInfo::Group grp;
			grp.groupID=target->m_groupID;
			strcpy_k(grp.nick, sizeof(grp.nick),target->m_nick.c_str());
			strcpy_k(grp.signature, sizeof(grp.signature), target->m_signature.c_str());
			so.WriteByteArray(&grp, sizeof(grp));
			chatAcct->send(s_nChC_QueryGroupByNick_Rsp, so.data(), so.size());
			return;
		}
		else
		{
			so.WriteShort(0);
			chatAcct->send(s_nChC_QueryGroupByNick_Rsp, so.data(), so.size());
			return ;
		}
	}
}

