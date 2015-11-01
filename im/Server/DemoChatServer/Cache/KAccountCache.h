#pragma once
#include <System/Collections/KHashTable.h>
#include <System/Memory/KStepObjectPool.h>
#include "KStructDefine.h"
#include "../PacketGate/KSocketDefine.h"
#include <PacketGate/chat_client_packet.h>
#include "../PacketGate/KChatSession.h"

class KChatGroup;

class KChatAccount
	: public ChatAccount
	, public JG_M::KPortableStepPool<KChatAccount,1024>
{
public:
	enum EnumFlag
	{
		loading_friends,
		loading_groups,
		loading_unread_messages, // 正在加载未读消息
		friends_loaded,
		groups_loaded,
		unread_messages_loaded,  // 未读消息加载完毕
	};
	enum EnumState
	{
		empty,
		with_info,
		authenticate,
		load_friends,
		load_groups,
		ready,
		offline,
	};
	typedef JG_C::DynSortedArray<ChatAccount*,ChatAccount::less,32> FriendArray;
	typedef JG_C::DynSortedArray<ChatGroup*,ChatGroup::less,8> GroupArray;

public:
	KChatAccount();
	~KChatAccount();

public:
	string_128 toString() const;
	string_32 buildAvatarName(const JgMessageID& messageID) const;
	string_512 buildAvatarFilePath(const JgMessageID& messageID) const;
	string_512 buildAvatarFilePath(const char* avatarName) const;
	JgMessageTarget toMessageTarget() const;
	JgMessageSource toMessageSource() const;

public:
	static KChatAccount* Alloc() { return JG_M::KPortableStepPool<KChatAccount,1024>::Alloc(); }
	static void Free(KChatAccount* p) { JG_M::KPortableStepPool<KChatAccount,1024>::Free(p); }

public:
	void setFlag(EnumFlag flag) { m_flags |= 1<<flag; }
	BOOL hasFlag(EnumFlag flag) const { return m_flags & (1<<flag); }
	void clearFlag(EnumFlag flag) { m_flags &= ~(1<<flag); }

public:
	void reset();
	void copy(const ChatAccount& acct);
	int  addRef();
	int  releaseRef();
	void setSocket(KSocket_Client* clnSocket);
	BOOL isSocketReady() const;
	void closeSocket(int delaySec=0);

public:
	BOOL send(int cmd, const void* data, int len);
	BOOL broadcastFriends(int cmd, const void* data, int len);

	BOOL sendFriendsInfo();
	BOOL sendGroupsInfo();
	BOOL sendGroupDetail(KChatGroup* chatGrp);
	BOOL notifyEnter();
	BOOL notifyLeave();
	BOOL notifyModification();
	
	void loadFriends();
	void loadGroups();
	void loadUnreadMessages(INT64 lastPersonMessage, INT64 lastGroupMessage);

public:
	void setLastPersonMessage(const JgMessageID& messageID);
	void setLastGroupMessage(const JgMessageID& messageID);

public:
	BOOL hasFriend(UINT64 acctID) const;
	BOOL isGroupMember(UINT64 grpID) const;
	BOOL addFriend(KChatAccount* frd);
	BOOL joinGroup(KChatGroup* grp);
	BOOL modify(const char* nick, const char* signature);
	BOOL removeFriend(KChatAccount* frd);
	BOOL leaveGroup(KChatGroup* grp);

public:
	BOOL sendMessage(const JgMessageTarget& target, JgMessageContentType::Type contentType, StreamInterface& si);
	BOOL sendMessage(const JgMessageTarget& target, JgMessageContentType::Type contentType, const void* data, int len);
	BOOL sendMessageToFriends(JgMessageContentType::Type messageType, const void* data, int len);
	BOOL sendMessageToGroups(JgMessageContentType::Type messageType, const void* data, int len);

public:
	DWORD m_flags;
	int m_refCount;
	EnumState m_state;
	unsigned char m_pwdMd5[16];
	KSocket_Client* m_clnSocket;
	FriendArray m_friends;
	GroupArray m_groups;

public:
	INT64 m_lastPersonMessage;
	INT64 m_lastGroupMessage;

public:
	DWORD m_nextSequence; // 下一个发出包的序列号
	KChatSession m_chatSession;
};

class KChatAccountCache
{
public:
	typedef JG_C::KHashTable<UINT64,KChatAccount*> ChatAcctMap;
	typedef JG_C::KHashTable< ccstr,KChatAccount*,KccstriCmp,KStringHasher<ccstr> > AcctNameMap;

public:
	KChatAccountCache();
	~KChatAccountCache();

public:
	KChatAccount* find(UINT64 acctID);
	KChatAccount* find(const char* acctName);
	BOOL add(KChatAccount* chatAccount);
	BOOL remove(UINT64 passport);

public:
	ChatAcctMap m_acctMap;
	AcctNameMap m_acctNameMap;
};

class KChatCandidateManager
{
public:
	struct Candidate
	{
		KChatAccount* chatAcct;
		time_t loginTime;
	};
	typedef JG_C::KHashTable< ccstr,Candidate,KccstriCmp,KStringHasher<ccstr> > CandidateMap;

public:
	KChatCandidateManager();
	~KChatCandidateManager();

public:
	KChatAccount* find(const char* acctName);
	BOOL add(KChatAccount* chatAcct);
	BOOL remove(KChatAccount* chatAcct);

public:
	void process_CCht_Login(KSocket_Client* clnSocket, CCht_Login* req);
	void process_CCht_ConnClosed(KSocket_Client* clnSocket);

public:
	CandidateMap m_candidateMap;
};
