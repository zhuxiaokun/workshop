#pragma once
//#include <System/KType.h>
#include "packet_segment.h"
//#include <KCommonDefine.h>
//#include <KCommonStruct.h>
//#include "common_packet.h"
typedef long long INT64;
typedef unsigned long long UINT64;

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

	s_nCCht_PacketEnd,
};

enum ChtC_Protocol
{
	s_nChtC_Connected = s_nChtC_PacketStart,
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
	} friends[0];
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
	unsigned short memberCount;
	struct Member
	{
		UINT64 passport;
		char nick[64];
		char signature[256];
	} members[0];
};

struct ChtC_NotifyEnter
{
	enum { s_nCode = s_nChtC_NotifyEnter };
	UINT64 acctID;
	char nick[64];
	char signature[256];
};

struct ChtC_NotifyLeave
{
	enum { s_nCode = s_nChtC_NotifyLeave };
	UINT64 acctID;
};

struct CCht_AddFriend
{
	enum { s_nCode = s_nCCht_AddFriend };
	char acctName[32];
	char note[256];
};

struct ChtC_AddFriend_Rsp
{
	enum { s_nCode = s_nChtC_AddFriend_Rsp };
	int result;
	UINT64 acctID;
	char nick[64];
	char signature[256];
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

#pragma warning(default:4200)
#pragma pack(pop)//-------------------------------------
