#pragma once
#include <System/KPlatform.h>
#include <System/Collections/KQueue.h>
#include <System/Sync/KSync.h>

class KStorageTask
{
public:
	typedef System::Collections::KQueueFifo<KStorageTask*,JG_S::KMTLock> TaskQueue;

public:
	virtual ~KStorageTask() { }
	virtual void execute() = 0;
	virtual void onFinish() { }
	virtual void reset()    { }
	virtual void destroy()  { delete this; }
};
