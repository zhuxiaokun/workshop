#pragma once
#include <System/Collections/DynArray.h>
#include <PacketGate/chat_client_packet.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Misc/KStream.h>

struct ChatAccount;
class KChatGroup;

/// KOutgoingMessage
class KOutgoingMessage : public System::Memory::KPortableMemoryPool<KOutgoingMessage,8>
{
public:
	DWORD m_sequence;
	JgMessageID m_messageID;
	JgMessageSource m_messageSource;
	JgMessageContentType::Type m_contentType;
	JgMessageTarget m_messageTarget;
	string_512 m_messageFile;
	time_t m_messageTime;
	bool m_isTmpFile;

public:
	KOutgoingMessage();
	~KOutgoingMessage();
	void reset();
	void destroy();
	string_512 buildTmpFileName() const;
	string_512 buildMessageFilePath() const;
	string_512 buildMessageFullPath() const;
	bool saveTmpFile(const void* data, int len);
	bool save();

public: class ptr_cmp
	{
	public: typedef KOutgoingMessage* ptr;
		int operator() (const ptr& a, const ptr& b) const
		{ return (int)(a->m_sequence - b->m_sequence); }
	};
};

/// KChatMessage 
class KChatMessage : public System::Memory::KPortableMemoryPool<KChatMessage,32>
{
public:
	DWORD m_sequence;
	JgMessageContentType::Type m_contentType;
	JgMessageID m_messageID;
	JgMessageSource m_messageSource;
	JgMessageTarget m_messageTarget;
	int m_lengthSum;		// 累计已发送的消息长度
	time_t m_startTime;
	KMsgOutputStream m_messageData;
	
	BOOL m_finished;

public:
	KChatMessage()
	{
		m_sequence = 0;
		m_contentType = JgMessageContentType::TEXT;
		memset(&m_messageID, 0, sizeof(m_messageID));
		memset(&m_messageSource, 0, sizeof(m_messageSource));
		memset(&m_messageTarget, 0, sizeof(m_messageTarget));
		int m_lengthSum = 0;
		m_startTime = 0;
		m_finished = FALSE;
	}
	static string_512 buildMessageFilePath(const JgMessageID& mid, const JgMessageTarget& target);

public:
	class ptrcmp
	{
	public:
		typedef KChatMessage* ptr;
		int operator () (const ptr& a, const ptr& b) const
		{
			return (int)(a->m_sequence - b->m_sequence);
		}
	};
};

class KChatSession
{
public:
	typedef System::Collections::KDynSortedArray<KChatMessage*,KChatMessage::ptrcmp> MessageArray;
	typedef System::Collections::KDynSortedArray<KOutgoingMessage*,KOutgoingMessage::ptr_cmp> OutGoingMessageArray;

public:
	KChatSession();
	~KChatSession();

public:
	KChatMessage* getMessage(DWORD seq);
	KOutgoingMessage* getOutgoingMessage(DWORD seq);
	bool removeMessage(DWORD seq);
	bool removeOutGoingMessage(DWORD seq);
	void clear();

public:
	bool on_Message_Head(ChtC_Message_Head* messageHead, int len);
	bool on_Message_Body(ChtC_Message_Body* messageBody, int len);
	bool on_Message_End(ChtC_Message_End* messageEnd, int len);

public:
	MessageArray m_messages;
	OutGoingMessageArray m_outMessages;
};
