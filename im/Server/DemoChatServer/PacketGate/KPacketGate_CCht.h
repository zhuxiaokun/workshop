#pragma once
#include <PacketGate/chat_client_packet.h>
#include "KSocketDefine.h"
#include <System/Collections/KTree.h>

class KPacketGate_CCht
{
public:
	typedef void (KPacketGate_CCht::*ProcMethod)(KSocket_Client*,const void*,int);
	enum
	{
		CommandBegin = s_nCCht_PacketStart,
		CommandEnd = s_nCCht_PacketEnd,
		CommandCount = CommandEnd-CommandBegin,
	};

public:
	KPacketGate_CCht();
	~KPacketGate_CCht();

public:
	BOOL Register(int cmd, ProcMethod method);
	void Process(KSocket_Client* pSock, int cmd, const void* data, int len);

public:
	ProcMethod m_methods[CommandCount];

private:
	void Process_CCht_Connected(KSocket_Client* pSock, const void* pData, int len);//处理客户端连接
	void Process_CCht_ConnClosed(KSocket_Client* pSock, const void* pData, int len);//处理客户端关闭连接
	void Process_CCht_Login(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_QueryGroupDetail(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_AddFriend(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_AddFriend_ByName(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_AddFriend_Invite_Rsp(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_JoinGroup(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_CreateGroup(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_ModifyAccount(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_RemoveFriend(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_LeaveGroup(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_Message_Old(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_Message_Head(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_Message_Body(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_Message_End(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_AddGroupMember(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_JoinGroup_Invite_Rsp(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_QueryAvatar(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_QueryMessage(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_QueryAccount(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_QueryGroupByID(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_QueryGroupByNick(KSocket_Client* pSock, const void* pData, int len);
	void Process_CCht_QueryAccountByNick(KSocket_Client* pSock, const void* pData, int len);
};
