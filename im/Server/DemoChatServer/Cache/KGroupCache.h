#pragma once
#include <System/Collections/KHashTable.h>
#include <System/Collections/DynArray.h>
#include <System/Memory/KStepObjectPool.h>
#include "KAccountCache.h"

class KChatAccount;

class KChatGroup
	: public ChatGroup
	, JG_M::KPortableStepPool<KChatGroup,1024>
{
public:
	enum EnumFlag
	{
		detail_loading,
		detail_loaded,
	};
	typedef JG_C::DynSortedArray<ChatAccount*,ChatAccount::less,32> MemberArray;
	typedef MemberArray WaitDetailQueue;

public:
	KChatGroup();
	~KChatGroup();

public:
	JgMessageTarget toMessageTarget() const;
	JgMessageSource toMessageSource() const;

public:
	static KChatGroup* Alloc() { return JG_M::KPortableStepPool<KChatGroup,1024>::Alloc(); }
	static void Free(KChatGroup* p) { return JG_M::KPortableStepPool<KChatGroup,1024>::Free(p); }

public:
	KChatAccount* findMember(UINT64 acctID);
	bool addMember(KChatAccount* chatAcct);
	void setLastMessage(const JgMessageID& messageID);

public:
	int addRef() { return ++m_refCount; }
	int releaseRef() { return --m_refCount; }

public:
	void setFlag(EnumFlag flag) { m_flags |= 1<<flag; }
	BOOL hasFlag(EnumFlag flag) const { return m_flags & (1<<flag); }
	void clearFlag(EnumFlag flag) { m_flags &= ~(1<<flag); }

public:
	void reset();
	void copy(const ChatGroup& grp);
	void loadDetail(KChatAccount* chatAcct);
	void answerWaitDetailQueue();
	void broadcastMembers(int cmd, const void* data, int len, KChatAccount* exceptAcct);

public:
	int m_refCount;
	DWORD m_flags;
	MemberArray m_members;
	WaitDetailQueue m_waitDetailQueue;
};

class KChatGroupCache
{
public:
	typedef JG_C::KHashTable<UINT64,KChatGroup*> ChatGroupMap;
	typedef JG_C::KHashTable< ccstr,KChatGroup*,KccstriCmp,KStringHasher<ccstr> > NickMap;
	
public:
	KChatGroupCache();
	~KChatGroupCache();

public:
	KChatGroup* find(UINT64 grpID);
	KChatGroup* find(const char* nick);

	BOOL add(KChatGroup* grp);
	BOOL erase(UINT64 grpID);

public:
	ChatGroupMap m_grpMap;
	NickMap m_nickMap;
};
