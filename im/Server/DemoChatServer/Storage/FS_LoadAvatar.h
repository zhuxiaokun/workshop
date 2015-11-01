#pragma once
#include "KStorageTask.h"
#include <System/Memory/KStepObjectPool.h>
#include <JgMessageDefine.h>
#include "../PacketGate/KChatSession.h"

class FS_LoadAvatar
	: public KStorageTask
	, public JG_M::KPortableStepPool<FS_LoadAvatar,128>
{
public:
	FS_LoadAvatar();
	~FS_LoadAvatar();

public:
	virtual void execute();
	virtual void onFinish();
	virtual void reset();
	virtual void destroy();

public:
	int m_result;
	DWORD m_reqSeq;
	KChatAccount* m_source;
	KChatAccount* m_target;
	typedef piece_buffer_stream::alloc_type< piece_buffer_stream::piece_type<1024>,1024,JG_S::KMTLock > piece_alloc;
	KPieceBufferStream<1024,piece_alloc> m_avatarData;
};
