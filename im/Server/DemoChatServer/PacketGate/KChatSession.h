#pragma once
#include "KSocketDefine.h"
#include <System/Collections/DynArray.h>
#include <PacketGate/chat_client_packet.h>
#include <System/Misc/KStream.h>
#include <System/Memory/KPieceBufferStream.h>

class KChatAccount;
class KChatGroup;

class KChatMessage : public System::Memory::KPortableMemoryPool<KChatMessage,2048>
{
public:
	typedef piece_buffer_stream::alloc_type< piece_buffer_stream::piece_type<1024>,1024,JG_S::KMTLock > piece_alloc;
	typedef KPieceBufferStream<1024,piece_alloc> piece_stream;

public:
	DWORD m_inSequence;
	DWORD m_outSequence;

	JgMessageID m_messageID;
	JgMessageTarget m_target;

	KChatAccount* m_sender;
	KChatAccount* m_receiver;
	KChatGroup* m_grp;
	KPieceBufferStream<1024,piece_alloc> m_messageData;

	JgMessageContentType::Type m_contentType;
	int m_lengthSum;		// 累计已发送的消息长度
	time_t m_startTime;

public:
	KChatMessage();
	~KChatMessage();

public:
	void Reset();
	string_256 BuildMessageFileName() const;

public:
	class ptrcmp
	{
	public:
		typedef KChatMessage* ptr;
		int operator () (const ptr& a, const ptr& b) const
		{
			return (int)(a->m_inSequence - b->m_inSequence);
		}
	};
};

class KChatSession
{
public:
	typedef System::Collections::KDynSortedArray<KChatMessage*,KChatMessage::ptrcmp> MessageArray;

public:
	KChatSession();
	~KChatSession();

public:
	KChatMessage* getMessage(DWORD seq);
	bool removeMessage(DWORD seq);
	void clear();

public:
	bool on_Message_Head(CCht_Message_Head* messageHead, int len);
	bool on_Message_Body(CCht_Message_Body* messageBody, int len);
	bool on_Message_End(CCht_Message_End* messageEnd, int len);

public:
	KChatAccount* m_chatAcct;
	MessageArray m_messages;
};
