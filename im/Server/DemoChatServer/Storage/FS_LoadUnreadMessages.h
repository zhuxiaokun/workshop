#pragma once
#include "KStorageTask.h"
#include <System/Memory/KStepObjectPool.h>
#include <JgMessageDefine.h>
#include "../PacketGate/KChatSession.h"
#include <JgMessageDefine.h>
#include "../Cache/KStructDefine.h"

class FS_LoadUnreadMessages
	: public KStorageTask
	, public JG_M::KPortableStepPool<FS_LoadUnreadMessages,128>
{
public:
	typedef System::Collections::KSDSelfList<MessageInformation> MessageList;
	typedef piece_buffer_stream::alloc_type< piece_buffer_stream::piece_type<1024>,1024,JG_S::KMTLock > piece_alloc;

public:
	FS_LoadUnreadMessages();
	~FS_LoadUnreadMessages();

public:
	void setChatAcct(KChatAccount* chatAcct);

public:
	virtual void execute();
	virtual void onFinish();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	KChatAccount* m_chatAcct;
	MessageList m_messageList;
	KPieceBufferStream<1024,piece_alloc> m_messageData;
};
