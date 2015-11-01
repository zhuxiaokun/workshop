#pragma once
#include <System/KType.h>
#include <KCommonStruct.h>
#pragma pack(push,1)

struct JgMessageTargetType
{
	enum Type
	{
		PERSON = 1,		// ¸öÈËÏûÏ¢
		GROUP  = 2,		// ÈºÏûÏ¢
		AVATAR = 3,		// Í·Ïñ
		PUBLIC = 4,		// ·¢²¼µÄ¹«ÖÚÏûÏ¢
	};
};

struct JgMessageContentType
{
	enum Type
	{
		TEXT  = 1,
		IMAGE = 2,
		AUDIO = 3,
		VIDEO = 4,
		SYS_COMMAND = 1000, // int: command, byte array: command data
	};
};

struct JgMessageRequestType
{
	enum Type
	{
		AddFriend,
		RemoveFriend,
		AddMember,
		RemoveMember,
		AddFriendAck,
		JoinGroup,
		QueryGroup,
		QueryAccount,
		QueryGroupByID,
		QueryGroupByNick,
	};
};

struct JgMessageID
{
	int yyyy_mm_dd;
	int sequence;
};

struct JgMessageSource
{
	JgMessageTargetType::Type sourceType;
	UINT64 sourceID;
};

struct JgMessageTarget
{
	JgMessageTargetType::Type targetType;
	UINT64 targetID;
};

template <size_t n> struct ChtC_Message_Body_Tmpl
{
	DWORD sequence;
	char body[n];
};

#pragma pack(pop)

class JgMessageRequest
{
public:
	JgMessageRequestType::Type m_requestType;
	DWORD m_requestID;
	time_t m_requestTime;
};

class JgMessageRequest_AddFriend : public JgMessageRequest
{
public:
	UINT64 m_initialAcctID;
	UINT64 m_friendAcctID;
	string_64 m_friendAcctName;
	char m_note[256];

	/////////////////////////////////////
	JgMessageRequest_AddFriend()
	{
		m_requestType = JgMessageRequestType::AddFriend;
	}
};

class  JgMessageRequest_JoinGroup: public JgMessageRequest
{
public :
	UINT64 m_initialAcctID;
	string_64  m_group_nick;

	/////////////////////////////////////
	JgMessageRequest_JoinGroup()
	{
		m_requestType = JgMessageRequestType::JoinGroup;
	}
};


class  JgMessageRequest_QueryGroupByID: public JgMessageRequest
{
public :
	UINT64 m_initialAcctID;
	UINT64  m_group_ID;

	/////////////////////////////////////
	JgMessageRequest_QueryGroupByID()
	{
		m_requestType = JgMessageRequestType::QueryGroupByID;
	}
};

class  JgMessageRequest_QueryGroupByNick: public JgMessageRequest
{
public :
	UINT64 m_initialAcctID;
	char grpNick[64];

	/////////////////////////////////////
	JgMessageRequest_QueryGroupByNick()
	{
		m_requestType = JgMessageRequestType::QueryGroupByNick;
	}
};

class  JgMessageRequest_QueryGroup: public JgMessageRequest
{
public :
	UINT64 m_initialAcctID;
	UINT64 grpID;
	/////////////////////////////////////
	JgMessageRequest_QueryGroup()
	{
		m_requestType = JgMessageRequestType::QueryGroup;
	}
};


class  JgMessageRequest_QueryAccount: public JgMessageRequest
{
public :
	UINT64 m_initialAcctID;
	UINT64 acctID;
	/////////////////////////////////////
	JgMessageRequest_QueryAccount()
	{
		m_requestType = JgMessageRequestType::QueryAccount;
	}
};


class JgMessageRequest_RemoveFriend : public JgMessageRequest
{
public:
	UINT64 m_initialAcctID;
	UINT64 m_friendAcctID;

	/////////////////////////////////////
	JgMessageRequest_RemoveFriend()
	{
		m_requestType = JgMessageRequestType::RemoveFriend;
	}
};

class JgMessageRequest_AddMember : public JgMessageRequest
{
public:
	UINT64 m_groupID;
	UINT64 m_memberAcctID;
	UINT64 m_initialAcctID;
   char m_note[256];
public:
	JgMessageRequest_AddMember()
	{
		m_requestType = JgMessageRequestType::AddMember;
	}
};

class JgMessageRequest_RemoveMember : public JgMessageRequest
{
public:
	UINT64 m_groupID;
	UINT64 m_initialAcctID;
	UINT64 m_memberAcctID;

	/////////////////////////////////////
	JgMessageRequest_RemoveMember()
	{
		m_requestType = JgMessageRequestType::RemoveMember;
	}
};

class JgMessageRequest_AddFriendAck : public JgMessageRequest
{
public:
	UINT64 m_initialAcctID;
	UINT64 m_friendID;

	/////////////////////////////////////
	JgMessageRequest_AddFriendAck()
	{
		m_requestType = JgMessageRequestType::AddFriendAck;
	}
};
