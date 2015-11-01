#pragma once
#include <System/Collections/DynArray.h>
#include "KCommonStruct.h"
#include <System/Memory/KStepObjectPool.h>
#include <JgMessageDefine.h>

struct ChatAccount
	: public JG_M::KPortableMemoryPool<ChatAccount,1024,JG_S::KMTLock>
{
	UINT64 m_acctID;
	string_64 m_acctName;
	unsigned char m_password[16];
	string_64 m_nick;
	string_256 m_signature;
	string_32 m_avatar;
	ChatAccount* m_pNextNode;

	///
	class less
	{
	public:
		typedef ChatAccount* ptr;
		bool operator () (const ptr& a, const ptr& b) const
		{
			return a->m_acctID < b->m_acctID;
		}
	};
	void clear()
	{
		m_acctID = 0;
		m_acctName.clear();
		memset(&m_password, 0, sizeof(m_password));
		m_nick.clear();
		m_signature.clear();
		m_avatar.clear();
		m_pNextNode = NULL;
	}
};

struct ChatGroup
	: public JG_M::KPortableMemoryPool<ChatGroup,1024,JG_S::KMTLock>
{
	UINT64 m_groupID;
	string_64 m_nick;
	string_256 m_signature;
	ChatGroup* m_pNextNode;

	///
	class less
	{
	public:
		typedef ChatGroup* ptr;
		bool operator () (const ptr& a, const ptr& b) const
		{
			return a->m_groupID < b->m_groupID;
		}
	};
	void clear()
	{
		m_groupID = 0;
		m_nick.clear();
		m_signature.clear();
		m_pNextNode = NULL;
	}
};

struct MessageInformation : public System::Memory::KPortableMemoryPool<MessageInformation,1024,JG_S::KMTLock>
{
	JgMessageID messageID;
	JgMessageSource source;
	JgMessageTarget target;
	JgMessageContentType::Type contentType;
	char filePath[128];
	time_t messageTime;
	MessageInformation* m_pNextNode;
};

