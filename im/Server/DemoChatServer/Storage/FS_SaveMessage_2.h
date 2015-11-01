#pragma once
#include "KStorageTask.h"
#include <System/Memory/KStepObjectPool.h>
#include <JgMessageDefine.h>
#include "../PacketGate/KChatSession.h"

class FS_SaveMessage_2
	: public KStorageTask
	, public JG_M::KPortableStepPool<FS_SaveMessage_2,128>
{
public:
	FS_SaveMessage_2();
	~FS_SaveMessage_2();

public:
	virtual void execute();
	virtual void onFinish();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	JgMessageID m_messageID;
	JgMessageSource m_messageSource;
	JgMessageTarget m_messageTarget;
	JgMessageContentType::Type m_contentType;
	KChatMessage::piece_stream m_messageData;
	time_t m_messageTime;
};
