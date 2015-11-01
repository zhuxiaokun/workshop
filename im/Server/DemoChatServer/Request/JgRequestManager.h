#pragma once
#include "JgMessageRequest.h"
#include <System/Collections/KHashTable.h>
#include "../Cache/KAccountCache.h"
#include "../Cache/KGroupCache.h"

// È«¾ÖµÄ

class JgRequestManager
{
public:
	typedef System::Collections::KHashTable<DWORD,JgMessageRequest*> RequestMap;

public:
	JgRequestManager();
	~JgRequestManager();

public:
	void processRequest(JgMessageRequest* request);

public:
	DWORD AddRequest_AddMember(KChatAccount* initial, KChatGroup* grp, UINT64 acctID);
	JgMessageRequest_AddMember* FindRequest_AddMember(DWORD requestID);
	bool RemoveRequest(DWORD requestID);

private:
	void process_AddFriend(JgMessageRequest_AddFriend* request);
	void process_RemoveFriend(JgMessageRequest_RemoveFriend* request);
	void process_AddMember(JgMessageRequest_AddMember* request);
	void process_RemoveMember(JgMessageRequest_RemoveMember* request);
	void process_AddFriendAck(JgMessageRequest_AddFriendAck* request);
	void process_JoinGroup(JgMessageRequest_JoinGroup* request);
	void process_QueryGroup(JgMessageRequest_QueryGroup* request);
    void process_QueryAccount(JgMessageRequest_QueryAccount* request);
	void process_QueryGroupByID(JgMessageRequest_QueryGroupByID* request);
	void process_QueryGroupByNick(JgMessageRequest_QueryGroupByNick* request);

public:
	DWORD m_nextRequestID;
	RequestMap m_requestMap;
};
