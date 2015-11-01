#pragma once
#pragma pack(push, 1)

struct JgMessageEntityType
{
	enum Type
	{
		Person,
		Group,
		System,
	};
};

struct JgMessageType
{
	enum Type
	{
		Request,
		Response,
		Notification,
	};
};

struct JgMessageContentType
{
	enum Type
	{
		Text,
		Image,
		Audio,
		Vedio,

		// for inner usage
		SystemCommand = 1024,
	};
};

struct JgSystemMessageCommand
{
	enum
	{
		Error,
		Connect,
		Login,
		ModifyAccount,
		CreateGroup,
		JoinGroup,
		LeaveGroup,
		AddFriend,
		RemoveFriend,
		AddFriendAck,
		QueryGroupDetail,
		Close,
	};
};

struct JgMessageHead
{
	unsigned int sequence;
	JgMessageType::Type messageType;
	JgMessageContentType::Type contentType;
};

struct JgMessageEntity
{
	JgMessageEntityType::Type entityType;
	unsigned long long entityID; // if entityType is System, this field indicate the message command
};

#pragma pack(pop)

// 客户端能收到的请求类型

struct JgRequestType
{
	enum Type
	{
		AddFriendInvite,
		Count,
	};
};

// 客户端能收到的响应类型

struct JgResponseType
{
	enum Type
	{
		Connect,
		Login,
		ModifyAccount,
		CreateGroup,
		JoinGroup,
		LeaveGroup,
		QueryGroupDetail,
		AddFriend,
		RemoveFriend,
		Close,
		Count,
	};
};

struct JgNotifyType
{
	enum Type
	{
		AccountInfo,
		AccountEnter,
		AccountLeave,
		AccountModification,
		GroupMemberModification,
		RemoveFriend,
		FriendInfo,
		GroupInfo,
		GroupDetail,
		GroupMemberLeave,
		Message,
		Count,
	};
};

typedef void (*JgRequestCallback)(JgRequestType::Type requestType, void* requestData);
typedef void (*JgResponseCallback)(JgResponseType::Type responseType, void* responseData);
typedef void (*JgNotifyCallback)(JgNotifyType::Type notifyType, void* notifyData);
