#pragma once
#include "KStorageTask.h"
#include <System/Memory/KStepObjectPool.h>
#include <JgMessageDefine.h>
#include "../PacketGate/KChatSession.h"

class FS_SaveAvatar
	: public KStorageTask
	, public JG_M::KPortableStepPool<FS_SaveAvatar,128>
{
public:
	FS_SaveAvatar();
	~FS_SaveAvatar();

public:
	virtual void execute();
	virtual void onFinish();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	JgMessageID m_messageID;
	KChatAccount* m_chatAcct;
	KChatMessage* m_message;
};
