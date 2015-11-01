#include "KPacketGate_CCht.h"
#include "../KDemoChatServerApp.h"
#include <System/Misc/StrUtil.h>
#include "../Database/DB_Authenticate.h"
#include "../Database/DB_CreateGroup.h"
#include <JgErrorCode.h>
#include "../Storage/FS_LoadAvatar.h"
#include "../Storage/FS_SaveMessage_2.h"
#include <KGlobalFunction.h>
#include "../Database/DB_LoadAccount.h"
#include "../Database/DB_LoadGroups.h"
#include "../Database/DB_QueryGroupByNick.h"
#include "../Database/DB_QueryAccountByNick.h"
#include "../Database/DB_QueryGroupByID.h"
#include "../Database/DB_LoadAccountByName.h"

KPacketGate_CCht::KPacketGate_CCht()
{
	memset(&m_methods, 0, sizeof(m_methods));
	this->Register(s_nCCht_Connected, &KPacketGate_CCht::Process_CCht_Connected);
	this->Register(s_nCCht_ConnClosed, &KPacketGate_CCht::Process_CCht_ConnClosed);
	this->Register(s_nCCht_Login, &KPacketGate_CCht::Process_CCht_Login);
	this->Register(s_nCCht_QueryGroupDetail, &KPacketGate_CCht::Process_CCht_QueryGroupDetail);
	this->Register(s_nCCht_AddFriend, &KPacketGate_CCht::Process_CCht_AddFriend);
	this->Register(s_nCCht_AddFriend_ByName, &KPacketGate_CCht::Process_CCht_AddFriend_ByName);
	this->Register(s_nCCht_AddFriend_Invite_Rsp, &KPacketGate_CCht::Process_CCht_AddFriend_Invite_Rsp);
	this->Register(s_nCCht_JoinGroup, &KPacketGate_CCht::Process_CCht_JoinGroup);
	this->Register(s_nCCht_CreateGroup, &KPacketGate_CCht::Process_CCht_CreateGroup);
	this->Register(s_nCCht_ModifyAccount, &KPacketGate_CCht::Process_CCht_ModifyAccount);
	this->Register(s_nCCht_RemoveFriend, &KPacketGate_CCht::Process_CCht_RemoveFriend);
	this->Register(s_nCCht_LeaveGroup, &KPacketGate_CCht::Process_CCht_LeaveGroup);
	this->Register(s_nCCht_Message_Old, &KPacketGate_CCht::Process_CCht_Message_Old);
	this->Register(s_nCCht_Message_Head, &KPacketGate_CCht::Process_CCht_Message_Head);
	this->Register(s_nCCht_Message_Body, &KPacketGate_CCht::Process_CCht_Message_Body);
	this->Register(s_nCCht_Message_End, &KPacketGate_CCht::Process_CCht_Message_End);
	this->Register(s_nCCht_AddGroupMember, &KPacketGate_CCht::Process_CCht_AddGroupMember);
	this->Register(s_nCCht_JoinGroup_Invite_Rsp, &KPacketGate_CCht::Process_CCht_JoinGroup_Invite_Rsp);
	this->Register(s_nCCht_QueryAvatar, &KPacketGate_CCht::Process_CCht_QueryAvatar);
	this->Register(s_nCCht_QueryMessage, &KPacketGate_CCht::Process_CCht_QueryMessage);
	this->Register(s_nCCht_QueryAccount, &KPacketGate_CCht::Process_CCht_QueryAccount);
	this->Register(s_nCCht_QueryGroupByID, &KPacketGate_CCht::Process_CCht_QueryGroupByID);
	this->Register(s_nCCht_QueryGroupByNick, &KPacketGate_CCht::Process_CCht_QueryGroupByNick);
	this->Register(s_nCCht_QueryAccountByNick, &KPacketGate_CCht::Process_CCht_QueryAccountByNick);
}

KPacketGate_CCht::~KPacketGate_CCht()
{

}

BOOL KPacketGate_CCht::Register(int cmd, ProcMethod method)
{
	if(cmd < CommandBegin) return FALSE;
	if(cmd >= CommandEnd)  return FALSE;
	m_methods[cmd-CommandBegin] = method;
	return TRUE;
}

void KPacketGate_CCht::Process(KSocket_Client* pSock, int cmd, const void* data, int len)
{
	if(cmd < CommandBegin || cmd >= CommandEnd)
	{
		Log(LOG_DEBUG, "error: unexpected Client-Chat packet cmd:%u length:%u", cmd, len);
		return;
	}
	ProcMethod method = m_methods[cmd - CommandBegin];
	if(!method) return;
	(this->*method)(pSock, data, len);
}

void KPacketGate_CCht::Process_CCht_Connected(KSocket_Client* pSock, const void* pData, int len)//客户端连接服务器端
{
	Log(LOG_FATAL, "Client connected-------, %s", pSock->ToString().c_str());
}

void KPacketGate_CCht::Process_CCht_ConnClosed(KSocket_Client* pSock, const void* pData, int len)//客户端关闭连接
{
	Log(LOG_FATAL, "Client closed----------, %s", pSock->ToString().c_str());
	if(pSock->m_chatAcct)
	{
		KChatAccount* chatAcct = pSock->m_chatAcct;
		chatAcct->setSocket(NULL);
		chatAcct->m_chatSession.clear();
		
		chatAcct->notifyLeave();//通知好友离开

		chatAcct = g_pApp->m_candidateManager.find(chatAcct->m_acctName.c_str());
		if(chatAcct)
		{
			g_pApp->m_candidateManager.remove(chatAcct);
			chatAcct->reset(); 
			KChatAccount::Free(chatAcct);
		}
	}
}


void KPacketGate_CCht::Process_CCht_Login(KSocket_Client* pSock, const void* pData, int len)//客户端登录
{
	CCht_Login* req = (CCht_Login*)pData;
	const char* acctName = req->acctName;
	if(pSock->m_chatAcct)
	{
		pSock->Close();
		Log(LOG_ERROR, "error: login, %s already has an account %s",
			pSock->ToString().c_str(),
			pSock->m_chatAcct->toString().c_str());
		return;
	}

	KChatAccount* chatAcct = g_pApp->m_candidateManager.find(acctName);
	if(chatAcct)
	{
		pSock->Close();
		Log(LOG_ERROR, "error: CCht_Login '%s' already in login process", acctName);
		return;
	}

	chatAcct = g_pApp->m_acctCache.find(req->acctName);
	if(!chatAcct)
	{
		g_pApp->m_candidateManager.process_CCht_Login(pSock, req);
		return;
	}

	if(memcmp(&chatAcct->m_password, &req->pwdMd5, sizeof(chatAcct->m_password)))
	{
		ChtC_LoginRsp rsp;
		rsp.result = JgErrorCode::err_PasswordMismatch;//用户名和密码不匹配
		pSock->Send(ChtC_LoginRsp::s_nCode, &rsp, sizeof(rsp));
		pSock->DelayClose(3);
		Log(LOG_DEBUG, "debug: login '%s' password mismatch", req->acctName);
		return;
	}
	
	if(chatAcct->m_clnSocket)
	{
		chatAcct->closeSocket(5);
		pSock->Close();
		Log(LOG_WARN, "warn: login '%s' already online", acctName);
		return;
	}
	
	chatAcct->setSocket(pSock);
	{
		ChtC_LoginRsp rsp;
		rsp.result = JgErrorCode::err_Success;//登陆成功
		pSock->Send(ChtC_LoginRsp::s_nCode, &rsp, sizeof(rsp));
	}
	
	{
		ChtC_AcctInfo rsp;
		rsp.passport = chatAcct->m_acctID;
		strcpy_k(rsp.nick, sizeof(rsp.nick), chatAcct->m_nick.c_str());
		strcpy_k(rsp.signature, sizeof(rsp.signature), chatAcct->m_signature.c_str());
		strcpy_k(rsp.avatar, sizeof(rsp.avatar), chatAcct->m_avatar.c_str());
		pSock->Send(ChtC_AcctInfo::s_nCode, &rsp, sizeof(rsp));
	}

	if(chatAcct->sendFriendsInfo())
		chatAcct->sendGroupsInfo();

	return;
}

void KPacketGate_CCht::Process_CCht_QueryGroupDetail(KSocket_Client* pSock, const void* pData, int len)
{
	CCht_QueryGroupDetail* req = (CCht_QueryGroupDetail*)pData;
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct) return;
	if(chatAcct->m_state == KChatAccount::authenticate) return;
	KChatGroup* chatGrp = g_pApp->m_grpCache.find(req->groupID);
	if(!chatGrp)
	{
		ChtC_GroupDetail rsp;
		rsp.grpID = req->groupID;
		rsp.memberCount =0;
		chatAcct->send(s_nChtC_GroupDetail, &rsp, sizeof(rsp));
		return;
	}

	chatAcct->sendGroupDetail(chatGrp);
}

void KPacketGate_CCht::Process_CCht_AddFriend(KSocket_Client* pSock, const void* pData, int len)//客户端添加好友
{
	CCht_AddFriend* req = (CCht_AddFriend*)pData;
	const char* note = req->note;
	if(!pSock->m_chatAcct) return;

	KChatAccount* chatAcct = g_pApp->m_acctCache.find(req->friendID);
	if(!chatAcct)
	{
		ChtC_AddFriend_Rsp rsp;
		rsp.result = JgErrorCode::err_Peer_NotFound;
		pSock->Send(s_nChtC_AddFriend_Rsp, &rsp, sizeof(rsp));
		JgMessageRequest_AddFriend* request = new JgMessageRequest_AddFriend();
		request->m_requestID = g_pApp->m_requestManager.m_nextRequestID++;
		request->m_requestTime = time(NULL);
		request->m_initialAcctID = pSock->m_chatAcct->m_acctID;
		request->m_friendAcctID = req->friendID;
		strcpy_k(request->m_note, sizeof(request->m_note), req->note);

		DB_LoadAccount* task = DB_LoadAccount::Alloc(); task->reset();
		task->m_acctID = req->friendID;
		task->setNextRequest(request);
		g_pApp->m_databaseAgent.push(*task);
		return;
	}

	if(chatAcct == pSock->m_chatAcct)
	{
		ChtC_AddFriend_Rsp rsp;
		rsp.result = JgErrorCode::err_AddFriend_Self;
		pSock->Send(s_nChtC_AddFriend_Rsp, &rsp, sizeof(rsp));
		return;
	}

	if(!chatAcct->isSocketReady())
	{
		JgMessageRequest_AddFriend request;
		request.m_requestID = g_pApp->m_requestManager.m_nextRequestID++;
		request.m_requestTime = time(NULL);
		request.m_initialAcctID = pSock->m_chatAcct->m_acctID;
		request.m_friendAcctID = req->friendID;
		strcpy_k(request.m_note, sizeof(request.m_note), req->note);

		FS_SaveMessage_2* task = FS_SaveMessage_2::Alloc(); task->reset();
		task->m_messageID = g_pApp->nextMessageID();
		task->m_messageSource = chatAcct->toMessageSource();
		task->m_messageTarget = chatAcct->toMessageTarget();
		task->m_contentType = JgMessageContentType::SYS_COMMAND;
		task->m_messageData.write(&request, sizeof(request));
		task->m_messageTime = time(NULL);
		g_pApp->m_storageAgent.Push(task);

		return;
	}
	{
		ChtC_AddFriend_Invite rsp;
		rsp.acctID = pSock->m_chatAcct->m_acctID;
		strcpy_k(rsp.note, sizeof(rsp.note), req->note);
		chatAcct->send(s_nChtC_AddFriend_Invite, &rsp, sizeof(rsp));
	}
}

void KPacketGate_CCht::Process_CCht_AddFriend_ByName(KSocket_Client* pSock, const void* pData, int len)
{
	CCht_AddFriend_ByName* req = (CCht_AddFriend_ByName*)pData;

	const char* friendAcctName = req->friendAcctName;
	const char* note = req->note;

	if(!pSock->m_chatAcct) return;

	KChatAccount* chatAcct = g_pApp->m_acctCache.find(friendAcctName);
	if(!chatAcct)
	{
		ChtC_AddFriend_Rsp rsp;
		rsp.result = JgErrorCode::err_Peer_NotFound;
		pSock->Send(s_nChtC_AddFriend_Rsp, &rsp, sizeof(rsp));

		JgMessageRequest_AddFriend* request = new JgMessageRequest_AddFriend();
		request->m_requestID = g_pApp->m_requestManager.m_nextRequestID++;
		request->m_requestTime = time(NULL);
		request->m_initialAcctID = pSock->m_chatAcct->m_acctID;
		request->m_friendAcctID = 0;
		request->m_friendAcctName = friendAcctName;
		strcpy_k(request->m_note, sizeof(request->m_note), req->note);

		DB_LoadAccountByName* task = DB_LoadAccountByName::Alloc(); task->reset();
		task->m_acctName = friendAcctName;
		task->setNextRequest(request);
		g_pApp->m_databaseAgent.push(*task);
		return;
	}

	if(chatAcct == pSock->m_chatAcct)
	{
		ChtC_AddFriend_Rsp rsp;
		rsp.result = JgErrorCode::err_AddFriend_Self;
		pSock->Send(s_nChtC_AddFriend_Rsp, &rsp, sizeof(rsp));
		return;
	}

	if(!chatAcct->isSocketReady())
	{
		JgMessageRequest_AddFriend request;
		request.m_requestID = g_pApp->m_requestManager.m_nextRequestID++;
		request.m_requestTime = time(NULL);
		request.m_initialAcctID = pSock->m_chatAcct->m_acctID;
		request.m_friendAcctID = chatAcct->m_acctID;
		request.m_friendAcctName = friendAcctName;
		strcpy_k(request.m_note, sizeof(request.m_note), req->note);

		FS_SaveMessage_2* task = FS_SaveMessage_2::Alloc(); task->reset();
		task->m_messageID = g_pApp->nextMessageID();
		task->m_messageSource = chatAcct->toMessageSource();
		task->m_messageTarget = chatAcct->toMessageTarget();
		task->m_contentType = JgMessageContentType::SYS_COMMAND;
		task->m_messageData.write(&request, sizeof(request));
		task->m_messageTime = time(NULL);
		g_pApp->m_storageAgent.Push(task);
		return;
	}
	{
		ChtC_AddFriend_Invite rsp;
		rsp.acctID = pSock->m_chatAcct->m_acctID;
		strcpy_k(rsp.note, sizeof(rsp.note), req->note);
		chatAcct->send(s_nChtC_AddFriend_Invite, &rsp, sizeof(rsp));
	}
}

void KPacketGate_CCht::Process_CCht_AddFriend_Invite_Rsp(KSocket_Client* pSock, const void* pData, int len)
{
	CCht_AddFriend_Invite_Rsp* req = (CCht_AddFriend_Invite_Rsp*)pData;
	if(!pSock->m_chatAcct) return;
	
	KChatAccount* chatAcct = g_pApp->m_acctCache.find(req->acctID);
	if(!chatAcct)
	{
		if(!req->result)//
		{
			JgMessageRequest_AddFriendAck* request = new JgMessageRequest_AddFriendAck();
			request->m_requestID = g_pApp->m_requestManager.m_nextRequestID++;
			request->m_requestTime = time(NULL);
			request->m_initialAcctID = req->acctID;
			request->m_friendID = pSock->m_chatAcct->m_acctID;

			DB_LoadAccount* task = DB_LoadAccount::Alloc(); task->reset();
			task->m_acctID = req->acctID;

			task->setNextRequest(request);
			g_pApp->m_databaseAgent.push(*task);
		}
		return;
	}

	if(req->result)
	{
		ChtC_AddFriend_Rsp rsp;
		memset(&rsp, 0, sizeof(rsp));
		rsp.result = req->result;
		chatAcct->send(s_nChtC_AddFriend_Rsp, &rsp, sizeof(rsp));
		return;
	}

	KChatAccount* frd = pSock->m_chatAcct;
	if(!frd)
	{
		ChtC_AddFriend_Rsp rsp;
		memset(&rsp, 0, sizeof(rsp));
		rsp.result = JgErrorCode::err_InvalidState;
		chatAcct->send(s_nChtC_AddFriend_Rsp, &rsp, sizeof(rsp));
		return;
	}

	chatAcct->addFriend(frd);

	{
		ChtC_AddFriend_Rsp rsp;
		rsp.result =JgErrorCode::err_Success;
		rsp.acctID = frd->m_acctID;
		strcpy_k(rsp.nick, sizeof(rsp.nick), frd->m_nick.c_str());
		strcpy_k(rsp.signature, sizeof(rsp.signature), frd->m_signature.c_str());
		strcpy_k(rsp.avatar, sizeof(rsp.avatar), frd->m_avatar.c_str());
		chatAcct->send(s_nChtC_AddFriend_Rsp, &rsp, sizeof(rsp));
	}
	{
		KBufferOutputStream<2048> so;
		so.WriteShort(-1);
		KChatAccount* acct = chatAcct;
		ChtC_FriendInfo::Friend theFrd;
		theFrd.acctID = acct->m_acctID;
		strcpy_k(theFrd.nick, sizeof(theFrd.nick), acct->m_nick.c_str());
		strcpy_k(theFrd.signature, sizeof(theFrd.signature), acct->m_signature.c_str());
		strcpy_k(theFrd.avatar, sizeof(theFrd.avatar), acct->m_avatar.c_str());
		so.WriteByteArray(&theFrd, sizeof(theFrd));
		frd->send(s_nChtC_FriendInfo, so.data(), so.size());
	}
}

void KPacketGate_CCht::Process_CCht_JoinGroup(KSocket_Client* pSock, const void* pData, int len)
{
	CCht_JoinGroup* req = (CCht_JoinGroup*)pData;
	const  char * nick=req->grpNick;
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(chatAcct->m_state == KChatAccount::authenticate)
		return;
	KChatGroup* chatGrp = g_pApp->m_grpCache.find(req->grpNick);
	if(!chatGrp)
	{
		ChtC_JoinGroup_Rsp rsp;
		rsp.result =JgErrorCode::err_Group_NotFound;
		chatAcct->send(s_nChtC_JoinGroup_Rsp, &rsp, sizeof(rsp));
	    JgMessageRequest_JoinGroup* request=new JgMessageRequest_JoinGroup();
		request->m_requestID = g_pApp->m_requestManager.m_nextRequestID++;
		request->m_requestTime = time(NULL);
		request->m_initialAcctID = pSock->m_chatAcct->m_acctID;
		request->m_group_nick=nick;

		ChatGroup grp;
		grp.m_groupID = 0;
		grp.m_nick =nick;
		grp.m_signature = NULL;
       DB_QueryGroupByNick * task = DB_QueryGroupByNick::Alloc();
        task->reset();
	    task->setAcct(chatAcct, nick);
        g_pApp->m_databaseAgent.push(*task);
       return;
	}
	chatAcct->joinGroup(chatGrp);
}

void KPacketGate_CCht::Process_CCht_CreateGroup(KSocket_Client* pSock, const void* pData, int len)//创建群
{
	CCht_CreateGroup* req = (CCht_CreateGroup*)pData;
	
	const char* nick = req->nick;
	const char* signature = req->signature;

	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate)
		return;

	KChatGroup* chatGrp = g_pApp->m_grpCache.find(nick);
	if(chatGrp)
	{
		ChtC_CreateGroup_Rsp rsp;
		rsp.result =JgErrorCode::err_Group_AlreadyExist;
		pSock->Send(s_nChtC_CreateGroup_Rsp, &rsp, sizeof(rsp));
		return;
	}

	ChatGroup grp;
	grp.m_groupID = 0;
	grp.m_nick = nick;
	grp.m_signature = signature;
	DB_CreateGroup* task = DB_CreateGroup::Alloc(); task->reset();
	task->setAcct(chatAcct, &grp);
	g_pApp->m_databaseAgent.push(*task);
}
//修改个人信息
void KPacketGate_CCht::Process_CCht_ModifyAccount(KSocket_Client* pSock, const void* pData, int len)
{
	CCht_ModifyAccount* req = (CCht_ModifyAccount*)pData;
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct) return;
	if(chatAcct->m_state == KChatAccount::authenticate) return;
	chatAcct->modify(req->nick, req->signature);
}

void KPacketGate_CCht::Process_CCht_RemoveFriend(KSocket_Client* pSock, const void* pData, int len)
{
	CCht_RemoveFriend* req = (CCht_RemoveFriend*)pData;
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;
	KChatAccount* frd = g_pApp->m_acctCache.find(req->frdID);
	if(!frd) return;
	chatAcct->removeFriend(frd);
}

void KPacketGate_CCht::Process_CCht_LeaveGroup(KSocket_Client* pSock, const void* pData, int len)
{
	CCht_LeaveGroup* req = (CCht_LeaveGroup*)pData;
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;
	KChatGroup* grp = g_pApp->m_grpCache.find(req->grpID);
	if(!grp) return;
	chatAcct->leaveGroup(grp);
}

void KPacketGate_CCht::Process_CCht_Message_Old(KSocket_Client* pSock, const void* pData, int len)
{
	KChatAccount* sender = pSock->m_chatAcct;
	if(!sender || sender->m_state == KChatAccount::authenticate)
		return;

	CCht_Message_Old* req = (CCht_Message_Old*)pData;
	UINT64 grpID = req->group;
	UINT64 acctID = req->playerID;
	if(grpID)
	{
		KChatGroup* chatGrp = g_pApp->m_grpCache.find(grpID);
		if(!chatGrp)
		{
			Log(LOG_ERROR, "error: group %llu not found", grpID);
			return;
		}
		ChtC_Message_Old rsp; memcpy(&rsp, req, sizeof(rsp));
		rsp.playerID = sender->m_acctID;
		strcpy_k(rsp.playerNm, sizeof(rsp.playerNm), sender->m_acctName.c_str());
		chatGrp->broadcastMembers(ChtC_Message_Old::s_nCode, &rsp, sizeof(rsp), sender);
	}
	else
	{
		KChatAccount* recver = g_pApp->m_acctCache.find(acctID);
		if(!recver)
		{
			Log(LOG_ERROR, "error: account %llu not found", acctID);
			return;
		}
		ChtC_Message_Old rsp; memcpy(&rsp, req, sizeof(rsp));
		rsp.playerID = sender->m_acctID;
		strcpy_k(rsp.playerNm, sizeof(rsp.playerNm), sender->m_acctName.c_str());
		recver->send(ChtC_Message_Old::s_nCode, &rsp, sizeof(rsp));
	}
}

void KPacketGate_CCht::Process_CCht_Message_Head(KSocket_Client* pSock, const void* pData, int len)
{
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;

	CCht_Message_Head* req = (CCht_Message_Head*)pData;
	chatAcct->m_chatSession.on_Message_Head(req, len);
}

void KPacketGate_CCht::Process_CCht_Message_Body(KSocket_Client* pSock, const void* pData, int len)
{
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;

	CCht_Message_Body* req = (CCht_Message_Body*)pData;
	chatAcct->m_chatSession.on_Message_Body(req, len);
}

void KPacketGate_CCht::Process_CCht_Message_End(KSocket_Client* pSock, const void* pData, int len)
{
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;

	CCht_Message_End* req = (CCht_Message_End*)pData;
	chatAcct->m_chatSession.on_Message_End(req, len);
}

void KPacketGate_CCht::Process_CCht_AddGroupMember(KSocket_Client* pSock, const void* pData, int len)//添加群成员
{

	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;
	CCht_AddGroupMember* req = (CCht_AddGroupMember*)pData;
	KChatAccount* member = g_pApp->m_acctCache.find(req->memberAcctID);
	if(!member)
	{
		ChtC_AddGroupMember_Rsp rsp;
		rsp.result = JgErrorCode::err_Peer_NotFound;
		pSock->Send(ChtC_AddGroupMember_Rsp::s_nCode, &rsp, sizeof(rsp));
		return;
	}


	KChatGroup* grp = g_pApp->m_grpCache.find(req->groupID);
	if(!grp)//未找到该帐号群
	{
		ChtC_AddGroupMember_Rsp rsp;
		rsp.result = JgErrorCode::err_Group_NotFound;
		pSock->Send(ChtC_AddGroupMember_Rsp::s_nCode, &rsp, sizeof(rsp));
		return;
	}
	
	if(!grp->findMember(chatAcct->m_acctID))//如果邀请方不是群成员,则没有邀请权限
	{
		ChtC_AddGroupMember_Rsp rsp;
		rsp.result = JgErrorCode::err_AccessDeny;
		pSock->Send(ChtC_AddGroupMember_Rsp::s_nCode, &rsp, sizeof(rsp));
		return;
	}

	if(grp->findMember(req->memberAcctID))//已经是群成员,就不需要处理了
	{
		ChtC_AddGroupMember_Rsp rsp;
		rsp.result = JgErrorCode::err_AlreadyGroupMember;
		pSock->Send(ChtC_AddGroupMember_Rsp::s_nCode, &rsp, sizeof(rsp));
		return;
	}
	//如果以上各情况满足,判断一下对方是否在线,如果不在线,则将邀请信息存下来
	if(!member->m_clnSocket)
	{
		//处理好友不在线时的情况
		JgMessageRequest_AddMember request;
		DWORD requestID = g_pApp->m_requestManager.AddRequest_AddMember(chatAcct, grp, req->memberAcctID);
		request.m_requestID = requestID;
		request.m_requestType = JgMessageRequestType::AddMember;
		request.m_requestTime=time(NULL);
		request.m_groupID=grp->m_groupID;
		request.m_initialAcctID = chatAcct->m_acctID;
		request.m_memberAcctID =member->m_acctID;
       strcpy_k(request.m_note, sizeof(request.m_note), req->note);
		g_pApp->m_requestManager.AddRequest_AddMember(chatAcct, grp, req->memberAcctID);
		FS_SaveMessage_2* task = FS_SaveMessage_2::Alloc();
		task->reset();
		task->m_messageID =g_pApp->nextMessageID();
		task->m_messageSource =chatAcct->toMessageSource();
		task->m_messageTarget = member->toMessageTarget();
		task->m_contentType = JgMessageContentType::SYS_COMMAND;
		task->m_messageData.write(&request, sizeof(request));
		task->m_messageTime = time(NULL);
		g_pApp->m_storageAgent.Push(task);
		return;
	}

	DWORD requestID = g_pApp->m_requestManager.AddRequest_AddMember(chatAcct, grp, req->memberAcctID);
	{
		ChtC_JoinGroup_Invite rsp;
		rsp.requestID = requestID;
		rsp.initialAcctID = chatAcct->m_acctID;
		rsp.groupID = grp->m_groupID;
		strcpy_k(rsp.initialAcctNick, sizeof(rsp.initialAcctNick), chatAcct->m_nick.c_str());
		strcpy_k(rsp.groupNick, sizeof(rsp.groupNick), grp->m_nick.c_str());
		strcpy_k(rsp.note, sizeof(rsp.note), req->note);
		member->send(ChtC_JoinGroup_Invite::s_nCode, &rsp, sizeof(rsp));
	}


}

void KPacketGate_CCht::Process_CCht_JoinGroup_Invite_Rsp(KSocket_Client* pSock, const void* pData, int len)
{
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;

	CCht_JoinGroup_Invite_Rsp* req = (CCht_JoinGroup_Invite_Rsp*)pData;
	JgMessageRequest_AddMember* request = g_pApp->m_requestManager.FindRequest_AddMember(req->requestID);
	if(!request)
	{
		Log(LOG_WARN, "warn: request:%u not found", req->requestID);
		return;
	}

	KChatAccount* initial = g_pApp->m_acctCache.find(request->m_initialAcctID);
	if(!initial || !initial->m_clnSocket)
	{
		Log(LOG_WARN, "warn: request:%u initial &llu not found", req->requestID, request->m_initialAcctID);
		return;
	}

	if(req->result)
	{
		ChtC_AddGroupMember_Rsp rsp;
		rsp.result = JgErrorCode::err_PeerRefuse;
		initial->send(ChtC_AddGroupMember_Rsp::s_nCode, &rsp, sizeof(rsp));
		return;
	}
	{
		ChtC_AddGroupMember_Rsp rsp;
		rsp.result =JgErrorCode::err_Success;
		initial->send(ChtC_AddGroupMember_Rsp::s_nCode, &rsp, sizeof(rsp));
	}
	KChatGroup* grp = g_pApp->m_grpCache.find(request->m_groupID);
	ASSERT(grp);
	grp->addMember(chatAcct);
	
	g_pApp->m_requestManager.RemoveRequest(req->requestID);
}

void KPacketGate_CCht::Process_CCht_QueryAvatar(KSocket_Client* pSock, const void* pData, int len)
{
	CCht_QueryAvatar* req = (CCht_QueryAvatar*)pData;
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;

	DWORD seq = req->sequence;
	UINT64 acctID = req->acctID;
	KChatAccount* target = g_pApp->m_acctCache.find(acctID);
	if(!target)
	{
		Log(LOG_WARN, "warn: %s QueryAvatar acct:%llu not found", chatAcct->toString().c_str(), acctID);
		return;
	}

	if(target->m_avatar.empty())
	{
		ChtC_QueryAvatar_Rsp rsp;
		memset(&rsp, 0, sizeof(rsp));
		memset(&rsp, 0, sizeof(rsp));
		rsp.sequence = seq;
		rsp.result = JgErrorCode::err_AvatarNotSet;
		rsp.acctID = acctID;
		chatAcct->send(ChtC_QueryAvatar_Rsp::s_nCode, &rsp, sizeof(rsp));
		return;
	}

	FS_LoadAvatar* task = FS_LoadAvatar::Alloc(); task->reset();
	task->m_reqSeq = seq;
	task->m_source = chatAcct;
	task->m_target = target;
	g_pApp->m_storageAgent.Push(task);
}

void KPacketGate_CCht::Process_CCht_QueryMessage(KSocket_Client* pSock, const void* pData, int len)
{
	CCht_QueryMessage* req = (CCht_QueryMessage*)pData;
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;
	INT64 lastPersonMessage = MessageID2Int64(req->lastPersonMessage);
	INT64 lastGroupMessage = MessageID2Int64(req->lastGroupMessage);

	if(!chatAcct->m_lastPersonMessage||
		!chatAcct->m_lastGroupMessage|| 
		lastPersonMessage < chatAcct->m_lastPersonMessage|| 
		lastGroupMessage < chatAcct->m_lastGroupMessage)
	{
		if(!chatAcct->hasFlag(KChatAccount::friends_loaded) || !chatAcct->hasFlag(KChatAccount::groups_loaded))
		{
			KDemoChatServerApp::Data_LoadUnreadMessages* timerData = KDemoChatServerApp::Data_LoadUnreadMessages::Alloc();
			timerData->chatAcct = chatAcct;
			timerData->lastPersonMessage = lastPersonMessage;
			timerData->lastGroupMessage = lastGroupMessage;
			g_pApp->startTimer(KDemoChatServerApp::Timer_LoadUnreadMessages, chatAcct->m_acctID, 5.0f, timerData);
			return;
		}
		else
		{
		
			chatAcct->loadUnreadMessages(lastPersonMessage, lastGroupMessage);
		}
	}
}

void KPacketGate_CCht::Process_CCht_QueryAccount(KSocket_Client* pSock, const void* pData, int len)
{
	CCht_QueryAccount* req = (CCht_QueryAccount*)pData;
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;

	UINT64 acctID = req->acctID;
	KChatAccount* target = g_pApp->m_acctCache.find(acctID);
	if(target)
	{
		ChtC_QueryAccount_Rsp rsp;
		rsp.result = JgErrorCode::err_Success;
		rsp.passport = target->m_acctID;
		strcpy_k(rsp.nick, sizeof(rsp.nick), target->m_nick.c_str());
		strcpy_k(rsp.signature, sizeof(rsp.signature), target->m_signature.c_str());
		strcpy_k(rsp.avatar, sizeof(rsp.avatar), target->m_avatar.c_str());
		chatAcct->send(s_nChtC_QueryAccount_Rsp, &rsp, sizeof(rsp));
		return;
	}

	//从数据库中加载账户信息
	JgMessageRequest_QueryAccount* request = new JgMessageRequest_QueryAccount();
	request->m_requestID = g_pApp->m_requestManager.m_nextRequestID++;
	request->m_requestTime = time(NULL);
	request->m_initialAcctID =chatAcct->m_acctID;
	request->acctID= acctID;

	DB_LoadAccount* task = DB_LoadAccount::Alloc(); task->reset();
	task->m_acctID =acctID;
	task->setNextRequest(request);
	g_pApp->m_databaseAgent.push(*task);
	return ;
}

void KPacketGate_CCht::Process_CCht_QueryGroupByID(KSocket_Client* pSock, const void* pData, int len)
{
	CCht_QueryGroupByID* req=(CCht_QueryGroupByID*)pData;
	KChatAccount* chatAcct = pSock->m_chatAcct;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;
	UINT64 grpID = req->groupID;
	KChatGroup* target = g_pApp->m_grpCache.find(grpID);
	if(target)
	{
			KBufferOutputStream<1024*32> so;
			 so.WriteShort(-1);
			 ChtC_GroupInfo::Group grp;
			 grp.groupID=target->m_groupID;
			 strcpy_k(grp.nick, sizeof(grp.nick),target->m_nick.c_str());
			 strcpy_k(grp.signature, sizeof(grp.signature), target->m_signature.c_str());
			 so.WriteByteArray(&grp, sizeof(grp));
			chatAcct->send(s_nChC_QueryGroupByID_Rsp, so.data(), so.size());
			return;
	}
     //从数据库中查找群
	JgMessageRequest_QueryGroupByID* request=new JgMessageRequest_QueryGroupByID();
	request->m_requestID = g_pApp->m_requestManager.m_nextRequestID++;
	request->m_requestTime = time(NULL);
	request->m_initialAcctID = chatAcct ->m_acctID;
	request->m_group_ID=grpID;

	DB_QueryGroupByID * task = DB_QueryGroupByID::Alloc();
	task->reset();
	task->setAcct(chatAcct, grpID);
	task->setNextRequest(request);
	g_pApp->m_databaseAgent.push(*task);
	return;
}


void KPacketGate_CCht::Process_CCht_QueryGroupByNick(KSocket_Client* pSock, const void* pData, int len)
{
	CCht_QueryGroupByNick* req=(CCht_QueryGroupByNick*)pData;
	KChatAccount* chatAcct = pSock->m_chatAcct;
	const  char * nick=req->grpNick;
	if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;
	ChatGroup grp;
	grp.m_groupID = 0;
	grp.m_nick =nick;
	grp.m_signature = NULL;


	DB_QueryGroupByNick * task = DB_QueryGroupByNick::Alloc();
	task->reset();
	task->setAcct(chatAcct, nick);
	g_pApp->m_databaseAgent.push(*task);
	return;
}

void KPacketGate_CCht::Process_CCht_QueryAccountByNick(KSocket_Client* pSock, const void* pData, int len)
{
		CCht_QueryAccountByNick* req=(CCht_QueryAccountByNick*)pData;
		const char * nick=req->nick;
		KChatAccount* chatAcct = pSock->m_chatAcct;
		if(!chatAcct || chatAcct->m_state == KChatAccount::authenticate) return;

		DB_QueryAccountByNick * task=DB_QueryAccountByNick::Alloc();
		task->reset();
		task->setAcct(chatAcct,nick);
		g_pApp->m_databaseAgent.push(*task);
		return;
}
