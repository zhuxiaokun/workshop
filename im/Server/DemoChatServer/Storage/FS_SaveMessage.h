#pragma once
#include "KStorageTask.h"
#include <System/Memory/KStepObjectPool.h>
#include <JgMessageDefine.h>
#include "../PacketGate/KChatSession.h"

class FS_SaveMessage
	: public KStorageTask
	, public JG_M::KPortableStepPool<FS_SaveMessage,128>
{
public:
	FS_SaveMessage();
	~FS_SaveMessage();

public:
	void setMessage(KChatMessage* message);

public:
	virtual void execute();
	virtual void onFinish();
	virtual void reset();
	virtual void destroy();

public:
	int m_errCode;
	KChatMessage* m_message;
};
