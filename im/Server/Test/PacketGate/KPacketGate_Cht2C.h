#pragma once
#include "../Socket/KSocketDefine.h"
#include <KPacketGateTmpl.h>
#include <PacketGate/chat_client_packet.h>

class KPacketGate_Cht2C	: public KPacketGateTmpl<KPacketGate_Cht2C,KSocket_Chat,s_nChtC_PacketStart,s_nChtC_PacketEnd>
{
public:
	KPacketGate_Cht2C(void);
	~KPacketGate_Cht2C(void);

public:
	void process_Connected(KSocket_Chat* pSock, const void* data, int len);
	void process_ConnClosed(KSocket_Chat* pSock, const void* data, int len);
	void process_ConnError(KSocket_Chat* pSock, const void* data, int len);
	void process_LoginRsp(KSocket_Chat* pSock, const void* data, int len);
	void process_AcctInfo(KSocket_Chat* pSock, const void* data, int len);
	void process_FriendInfo(KSocket_Chat* pSock, const void* data, int len);
	void process_GroupInfo(KSocket_Chat* pSock, const void* data, int len);
	void process_GroupDetail(KSocket_Chat* pSock, const void* data, int len);
	void process_NotifyEnter(KSocket_Chat* pSock, const void* data, int len);
	void process_NotifyLeave(KSocket_Chat* pSock, const void* data, int len);
	void process_AddFriend_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_AddFriend_Invite(KSocket_Chat* pSock, const void* data, int len);
	void process_JoinGroup_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_CreateGroup_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_ModifyAccount_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_Notify_AccountModification(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_Notify_GroupMemberModification(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_RemoveFriend_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_Notify_RemoveFriend(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_LeaveGroup_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_Notify_LeaveGroup(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_Message_Head(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_Message_Body(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_Message_End(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_JoinGroup_Invite(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_AddGroupMember_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_Notify_JoinGroup(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_Message_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_SetAvatar_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_QueryAvatar_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_Message_Ack(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_QueryAccount_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_QueryGroupByID_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_QueryGroupByNick_Rsp(KSocket_Chat* pSock, const void* data, int len);
	void process_ChtC_QueryAccountByNick_Rsp(KSocket_Chat* pSock, const void* data, int len);
};
