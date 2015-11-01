#pragma once
#include <System/KType.h>
#include "packet_segment.h"
#include <KCommonDefine.h>
#include <KCommonStruct.h>
#include "common_packet.h"
#include "../JgMessageDefine.h"


enum CCht_Protocol
{
	s_nCCht_Connected = s_nCCht_PacketStart,
	s_nCCht_ConnClosed,

	s_nCCht_Login,
	s_nCCht_QueryGroupDetail,
	s_nCCht_AddFriend,
	s_nCCht_AddFriend_Invite_Rsp,
	s_nCCht_JoinGroup,
	s_nCCht_CreateGroup,

	s_nCCht_ModifyAccount,
	s_nCCht_RemoveFriend,
	s_nCCht_LeaveGroup,

	s_nCCht_Message_Head,
	s_nCCht_Message_Body,
	s_nCCht_Message_End,

	s_nCCht_Message_Old,

	s_nCCht_AddGroupMember,
	s_nCCht_JoinGroup_Invite_Rsp,

	s_nCCht_QueryAvatar,
	s_nCCht_QueryMessage,

	s_nCCht_AddFriend_ByName,
	s_nCCht_QueryAccount,
	s_nCCht_QueryGroupByID,
	s_nCCht_QueryGroupByNick,
	s_nCCht_QueryAccountByNick,

	s_nCCht_PacketEnd,
	
};

enum ChtC_Protocol   //返回值类型
{
	s_nChtC_Connected = s_nChtC_PacketStart,
	s_nChtC_ConnError,
	s_nChtC_ConnClosed,

	s_nChtC_LoginRsp,
	s_nChtC_AcctInfo,
	s_nChtC_FriendInfo,
	s_nChtC_GroupInfo,
	s_nChtC_GroupDetail,

	s_nChtC_NotifyEnter,
	s_nChtC_NotifyLeave,

	s_nChtC_AddFriend_Rsp,
	s_nChtC_AddFriend_Invite,

	s_nChtC_JoinGroup_Rsp,
	s_nChtC_CreateGroup_Rsp,

	s_nChtC_ModifyAccount_Rsp,
	s_nChtC_Notify_AccountModification,
	s_nChtC_Notify_GroupMemberModification,

	s_nChtC_RemoveFriend_Rsp,
	s_nChtC_Notify_RemoveFriend,

	s_nChtC_LeaveGroup_Rsp,
	s_nChtC_Notify_LeaveGroup,

	s_nChtC_Message_Head,
	s_nChtC_Message_Body,
	s_nChtC_Message_End,

	s_nChtC_Message_Old,

	s_nChtC_JoinGroup_Invite,
	s_nChtC_Notify_JoinGroup,
	s_nChtC_AddGroupMember_Rsp,

	s_nChtC_Message_Rsp,

	s_nChtC_SetAvatar_Rsp,
	s_nChtC_QueryAvatar_Rsp,
	
	s_nChtC_Message_Ack, // 确认客户端发送的消息

	s_nChtC_AcctListInfo,
    s_nChtC_QueryAccount_Rsp,
	s_nChC_QueryGroupByID_Rsp,
	s_nChC_QueryGroupByNick_Rsp,
	s_nChC_QueryAccountByNick_Rsp,

	s_nChtC_PacketEnd,
	
};

#pragma pack(push,1)//----------------------------------
#pragma warning(disable:4200)

struct CCht_Login
{
	enum { s_nCode = s_nCCht_Login };
	char acctName[64];
	unsigned char pwdMd5[16];
};

struct CCht_QueryGroupDetail
{
	enum { s_nCode = s_nCCht_QueryGroupDetail };
	UINT64 groupID;
};

struct CCht_QueryAccount  //按照passport查找Account
{
	enum { s_nCode = s_nCCht_QueryAccount };
	UINT64 acctID;
};
struct CCht_QueryAccountByNick //按昵称查找Account
{
	enum { s_nCode = s_nCCht_QueryAccountByNick };
	char nick[64];
};
struct CCht_QueryGroupByID //按群ID查找群信息
{
	enum { s_nCode = s_nCCht_QueryGroupByID };
	UINT64 groupID;
};
struct CCht_QueryGroupByNick //按群NICK查找群信息
{
	enum { s_nCode = s_nCCht_QueryGroupByID };
	char grpNick[64];
};
struct ChtC_LoginRsp
{
	enum { s_nCode = s_nChtC_LoginRsp };
	int result;
};

struct ChtC_AcctInfo
{
	enum { s_nCode = s_nChtC_AcctInfo };
	UINT64 passport;
	char nick[64];
	char signature[256];
	char avatar[32];
};

//2014-05-05
struct ChtC_AcctListInfo  //账户集,如果用nick作为关键字查找时,结果会有多个,用它来存储
{
	enum{s_nCode=s_nChtC_AcctListInfo};
	short  AcctCount;//如果小于零,表示增加
	struct AcctMember
	{
		 UINT64 passport;
		 char nick[64];
		 char signature[256];
		 char avatar[32];
	 } members[0];
};
struct ChtC_FriendInfo
{
	enum { s_nCode = s_nChtC_FriendInfo };
	short friendCount; // 如果小于0，表示增加
	struct Friend
	{
		UINT64 acctID;
        
		char nick[64];
		char signature[256];
		char avatar[32];
	} friends[0];
};

template <size_t n> struct ChtC_FriendInfo_Tmpl
{
	enum { s_nCode = s_nChtC_FriendInfo };
	short friendCount; // 如果小于0，表示增加
	struct Friend
	{
		UINT64 acctID;
		char nick[64];
		char signature[256];
		char avatar[32];
	} friends[n];
};

struct ChtC_GroupInfo
{
	enum { s_nCode = s_nChtC_GroupInfo };
	short groupCount; // 如果小于0，表示增加
	struct Group
	{
		UINT64 groupID;
		char nick[64];
		char signature[256];
	} groups[0];
};

struct ChtC_GroupDetail
{
	enum { s_nCode = s_nChtC_GroupDetail };
	UINT64 grpID;
	unsigned short memberCount;
	struct Member
	{
		UINT64 passport;
		char nick[64];
		char signature[256];
		char avatar[32];
	} members[0];
};

struct ChtC_NotifyEnter
{
	enum { s_nCode = s_nChtC_NotifyEnter };
	UINT64 acctID;
};

struct ChtC_NotifyLeave
{
	enum { s_nCode = s_nChtC_NotifyLeave };
	UINT64 acctID;
};

struct CCht_AddFriend
{
	enum { s_nCode = s_nCCht_AddFriend };
	UINT64 friendID;
	char note[256];
};

struct CCht_AddFriend_ByName
{
	enum { s_nCode = s_nCCht_AddFriend_ByName };
	char friendAcctName[64];
	char note[256];
};

struct ChtC_AddFriend_Rsp
{
	enum { s_nCode = s_nChtC_AddFriend_Rsp };
	 int result;
	UINT64 acctID;
	char nick[64];
	char signature[256];
	char avatar[32];
};

struct ChtC_AddFriend_Invite
{
	enum { s_nCode = s_nChtC_AddFriend_Invite };
	UINT64 acctID;
	char note[256];
};

struct CCht_AddFriend_Invite_Rsp
{
	enum { s_nCode = s_nCCht_AddFriend_Invite_Rsp };
	UINT64 acctID;
	int result;
};

struct CCht_JoinGroup
{
	enum { s_nCode = s_nCCht_JoinGroup };
	char grpNick[64];
};

struct ChtC_JoinGroup_Rsp
{
	enum { s_nCode = s_nChtC_JoinGroup_Rsp };
	int result;
};

struct CCht_CreateGroup
{
	enum { s_nCode = s_nCCht_CreateGroup };
	char nick[64];
	char signature[256];
};

struct ChtC_CreateGroup_Rsp
{
	enum { s_nCode = s_nChtC_CreateGroup_Rsp };
	int result;
};

struct CCht_ModifyAccount
{
	enum { s_nCode = s_nCCht_ModifyAccount };
	char nick[64];
	char signature[256];
};

struct ChtC_ModifyAccount_Rsp
{
	enum { s_nCode = s_nChtC_ModifyAccount_Rsp };
	int result;
};

struct ChtC_Notify_AccountModification
{
	enum { s_nCode = s_nChtC_Notify_AccountModification };
	UINT64 acctID;
	char nick[64];
	char signature[256];
	char avatar[32];
};

struct ChtC_Notify_GroupMemberModification
{
	enum { s_nCode = s_nChtC_Notify_GroupMemberModification };
	UINT64 grpID;
	struct Member
	{
		UINT64 acctID;
		char nick[64];
		char signature[256];
		char avatar[32];
	} member;
};

struct CCht_RemoveFriend
{
	enum { s_nCode = s_nCCht_RemoveFriend };
	UINT64 frdID;
};

struct CCht_LeaveGroup
{
	enum { s_nCode = s_nCCht_LeaveGroup };
	UINT64 grpID;
};

struct ChtC_RemoveFriend_Rsp
{
	enum { s_nCode = s_nChtC_RemoveFriend_Rsp };
	int result;
	UINT64 frdID;
};

struct ChtC_Notify_RemoveFriend
{
	enum { s_nCode = s_nChtC_Notify_RemoveFriend };
	UINT64 frdID;
};

struct ChtC_LeaveGroup_Rsp
{
	enum { s_nCode = s_nChtC_LeaveGroup_Rsp };
	int result;
	UINT64 grpID;
};

struct ChtC_Notify_LeaveGroup
{
	enum { s_nCode = s_nChtC_Notify_LeaveGroup };
	UINT64 grpID;
	UINT64 acctID;
};

struct CCht_Message_Old
{
	enum { s_nCode = s_nCCht_Message_Old };
	WORD msgcmd;
	WORD group;			// 群
	INT64 playerID;		// 接收方
	char playerNm[62];	// 接收方名字
	BYTE type  ;
	int strlen;
	char strmsg[800];
	int datalength;
	int dataCount;
};

struct ChtC_Message_Old
{
	enum { s_nCode = s_nChtC_Message_Old };
	WORD msgcmd;
	WORD group; // 群号
	INT64 playerID; // 发送方
	char playerNm[62]; // 发送方
	BYTE type  ;
	int strlen;
	char strmsg[800];
	int datalength;
	int dataCount;
};

struct CCht_Message_Head
{
	enum { s_nCode = s_nCCht_Message_Head };
	
	DWORD sequence;

	JgMessageContentType::Type contentType;
	JgMessageTarget messageTarget;

	int length; // 0 indicate unknown
};

struct CCht_Message_Body
{
	enum { s_nCode = s_nCCht_Message_Body };
	DWORD sequence;
	char body[0];
};

struct CCht_Message_End
{
	enum { s_nCode = s_nCCht_Message_End };
	DWORD sequence;
};

struct ChtC_Message_Head
{
	enum { s_nCode = s_nChtC_Message_Head };

	DWORD sequence; // 消息序号 sequence

	JgMessageID messageID;
	JgMessageContentType::Type contentType;
	JgMessageSource sender;
	JgMessageTarget mesageTarget;

	int length; // 消息长度，0 表示未知
};

struct ChtC_Message_Body
{
	enum { s_nCode = s_nChtC_Message_Body };
	DWORD sequence;
	char body[0];
};

struct ChtC_Message_End
{
	enum { s_nCode = s_nChtC_Message_End };
	DWORD sequence;
};

struct CCht_AddGroupMember
{
	enum { s_nCode = s_nCCht_AddGroupMember };
	UINT64 memberAcctID;
	UINT64 groupID;
	char note[256];
};

struct CCht_JoinGroup_Invite_Rsp
{
	enum { s_nCode = s_nCCht_JoinGroup_Invite_Rsp };
	DWORD requestID;
	int result;
};

struct ChtC_JoinGroup_Invite
{
	enum { s_nCode = s_nChtC_JoinGroup_Invite };
	DWORD requestID;
	UINT64 initialAcctID;
	UINT64 groupID;
	char initialAcctNick[64];
	char groupNick[64];
	char note[256];
};

struct ChtC_Notify_JoinGroup
{
	enum { s_nCode = s_nChtC_Notify_JoinGroup };
	struct Member
	{
		UINT64 acctID;
		char acctName[32];
		char nick[64];
		char signature[256];
		char avatar[32];
	};
	UINT64 grpID;
	Member member;
};

struct ChtC_AddGroupMember_Rsp
{
	enum { s_nCode = s_nChtC_AddGroupMember_Rsp };
	int result;
};

struct ChtC_QueryAccount_Rsp
{
	enum { s_nCode = s_nChtC_QueryAccount_Rsp};
	int result;
	UINT64 passport;
	char nick[64];
	char signature[256];
	char avatar[32];
};

struct ChtC_Message_Rsp
{
	enum { s_nCode = s_nChtC_Message_Rsp };
	DWORD sequence;
	JgMessageID messageID;
	JgMessageTarget messageTarget;
	JgMessageContentType::Type contentType;
	int result;
};

struct CCht_QueryAvatar
{
	enum { s_nCode = s_nCCht_QueryAvatar };
	DWORD sequence;
	UINT64 acctID;
};

struct ChtC_SetAvatar_Rsp
{
	enum { s_nCode = s_nChtC_SetAvatar_Rsp };
	int result;
	char avatarName[32];
};

struct ChtC_QueryAvatar_Rsp
{
	enum { s_nCode = s_nChtC_QueryAvatar_Rsp };
	DWORD sequence;
	int result;
	UINT64 acctID;
	char avatarName[32];
};

struct CCht_QueryMessage
{
	enum { s_nCode = s_nCCht_QueryMessage };
	JgMessageID lastPersonMessage;
	JgMessageID lastGroupMessage;
};

struct ChtC_Message_Ack
{
	enum { s_nCode = s_nChtC_Message_Ack };
	int result;
	DWORD sequence;
	JgMessageID messageID;
	INT64 messageTime;
};

#pragma warning(default:4200)
#pragma pack(pop)//-------------------------------------
