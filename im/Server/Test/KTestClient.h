#pragma once
#include <System/Collections/KQueue.h>
#include "Socket/KSocketDefine.h"
#include <lua/KLuaWrap.h>
#include <lua/KLuaTableHolder.h>
#include "PacketGate/KPacketGate_Cht2C.h"
#include "../DemoChatServer/Cache/KStructDefine.h"
#include <System/Collections/DynArray.h>
#include "PacketGate/KChatSession.h"
#include "Socket/JgSocketManager.h"

extern DWORD g_nowTicks;
extern KTestClient* g_client;

class KChatGroup;
class KChatAccount;

class KChatGroup
	: public ChatGroup
	, public JG_M::KPortableStepPool<KChatGroup>
{
public:
	typedef JG_C::DynSortedArray<ChatAccount*,ChatAccount::less> MemberArray;
	MemberArray m_members;
	static KChatGroup* Alloc() { return JG_M::KPortableStepPool<KChatGroup>::Alloc(); }
	static void Free(KChatGroup* p) { JG_M::KPortableStepPool<KChatGroup>::Free(p); }
	ChatAccount* getMember(UINT64 acctID);
	void removeMember(UINT64 acctID);
	void addMember(const ChatAccount& acct);
	void clearMembers();
	void clear();
};

class KTestClient : public KBaseLuaClass<KTestClient>, public ChatAccount
{
public:
	typedef JG_C::DynSortedArray<ChatAccount*,ChatAccount::less> FriendArray;
	typedef JG_C::DynSortedArray<ChatGroup*,ChatGroup::less>     GroupArray;

public:
	KTestClient();
	~KTestClient();

public:
	bool initialize();
	void breathe(int interval);
	void processPacket(KSocket_Chat* pSock, JgPacketHead* pHead);
	void finalize();
	bool checkConn();

public:
	bool connect(const char* ip, int port);
	void close();

	bool login(const char* account, const char* pwd);//��¼ϵͳ
	bool setAvatar(const char* filename); // ����ͷ��
	bool say(int acctID, const char* text);//˽������
	bool sayInGroup(int grpID, const char* text);//Ⱥ����
	bool shareFile(int acctID, int messageType, const char* filename);//�����ļ�
	bool shareFileInGroup(int grpID, int messageType, const char* filename);//����Ⱥ�ļ�
	bool addFriend(int acctID, const char* note);//��Ӻ���
	bool joinGroup(const char* grpNick);//��Ⱥ
    bool addFriendByName(const char* friendAcctName, const char* note);
	bool createGroup(const char* grpNick, const char* grpSignature);//����Ⱥ
	bool modify(const char* nick, const char* signature);//�޸ĸ�����Ϣ
	bool removeFriend(int frdID);//ɾ������
	bool leaveGroup(int grpID);//��Ⱥ
	bool queryGroupDetail(int grpID);
	bool inviteJoinGroup(int acctID, int grpID, const char* note);//������Ѽ�Ⱥ
	bool queryAvatar(int acctID);
	bool queryMessage(INT64 lastPersonMessage, INT64 lastGroupMessage);//�鿴��Ϣ
	bool queryGroupByID(int grpID);//��Ⱥ�˺Ų���Ⱥ
	bool queryAccount(int acctID);//���˺Ų��Һ���
	bool queryAccountByNick(const char * acctNick);
	bool queryGroupByNick(const char *  grpNick);//���˺Ų��Һ���

public:
	bool print_friends();
	bool print_groups();
	string_64 toString() const;

public:
	ChatAccount* getFriend(UINT64 acctID);
	KChatGroup* getGroup(UINT64 grpID);
	void clear();

public:
	bool _addFriend(const ChatAccount& acct);
	bool _joinGroup(const ChatGroup& grp);
	bool _removeFriend(UINT64 frdID);
	bool _leaveGroup(UINT64 grpID);
	bool _sendMessage(int targetType, UINT64 targetID, int contentType, const void* data, int len);
	bool _sendMessageFile(int targetType, UINT64 targetID, int contentType, const char* filename);

	string_512 _buildMessageFileName(KChatMessage* msg);
	bool _saveMessage(KChatMessage* msg);
	ChatAccount* _findAccount(UINT64 acctID);

public:
	DWORD m_nextSequence;
	KSocket_Chat* m_chatSocket;
	jg::net::JgSocketManager m_sockServer;
	JgPacketReceiver::PacketPtrArray m_packets;
	KPacketGate_Cht2C m_gateChat;

public:
	FriendArray m_friends;
	GroupArray m_groups;
	LMREG::CppTable m_tmpTab;
	KChatSession m_chatSession;

public:
	BeginDefLuaClassNoCon(KTestClient);
	DefMemberFunc(connect);
	DefMemberFunc(login);
	DefMemberFunc(close);
	DefMemberFunc(say);
	DefMemberFunc(setAvatar);
	DefMemberFunc(sayInGroup);
	DefMemberFunc(shareFile);
	DefMemberFunc(shareFileInGroup);
	DefMemberFunc(addFriend);
	DefMemberFunc(joinGroup);
    DefMemberFunc(addFriendByName);
	DefMemberFunc(createGroup);
	DefMemberFunc(modify);
	DefMemberFunc(removeFriend);
	DefMemberFunc(leaveGroup);
	DefMemberFunc(print_friends);
	DefMemberFunc(print_groups);
	DefMemberFunc(queryGroupDetail);
	DefMemberFunc(inviteJoinGroup);
	DefMemberFunc(queryAvatar);
	DefMemberFunc(queryMessage);
	DefMemberFunc(queryGroupByID);
	DefMemberFunc(queryAccount);
	DefMemberFunc(queryGroupByNick);
	DefMemberFunc(queryAccountByNick);
	EndDef;
};	luaClassPtr2Lua(KTestClient);
