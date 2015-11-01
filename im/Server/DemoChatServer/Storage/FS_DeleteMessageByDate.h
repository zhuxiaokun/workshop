#pragma once
#include "KStorageTask.h"
#include <System/Memory/KStepObjectPool.h>
#include <JgMessageDefine.h>
#include "../PacketGate/KChatSession.h"

class FS_DeleteMessageByDate
	: public KStorageTask
	, public JG_M::KPortableStepPool<FS_DeleteMessageByDate,128>
{
public:
	FS_DeleteMessageByDate();
	~FS_DeleteMessageByDate();

public:
	virtual void execute();
	virtual void onFinish();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	int m_yyyymmdd;
};
