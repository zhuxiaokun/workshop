#pragma once
#include "KDataBuffer.h"
#include "../Sync/KSync.h"
#include "../Collections/KSDList.h"
#include "../Thread/KThread.h"
#include "../Collections/KTree.h"
#include "../Collections/DynArray.h"

#ifdef _CHK_MEM_LEAK_

typedef uint_r CallStack[16];
namespace System { namespace Memory {

class KMemoryRecorder
{
public:
	enum
	{
		enum_free_rec,
		enum_alloc_rec,
		enum_realloc_rec,
	};
	struct AllocRec
	{
		uint_r type : 2; // 最高位的2个bit作为记录类型
		uint_r size : (sizeof(uint_r)*8-2);
		CallStack callStack;
		const void* p;
	};
	struct FreeRec
	{
		uint_r type : 2; // 最高位的2个bit作为记录类型
		uint_r size : (sizeof(uint_r)*8-2);
		CallStack callStack;
		const void* p;
	};
	struct ReallocRec
	{
		uint_r type : 2; // 最高位的2个bit作为记录类型
		uint_r size : (sizeof(uint_r)*8-2);
		CallStack callStack;
		const void* p;
		const void* pn;
	};

public:
	KMemoryRecorder();
	~KMemoryRecorder();

public: // 在内存操作所在的线程内调用
	void push_alloc(const CallStack& callStack, const void* p, size_t size);
	void push_free(const CallStack& callStack, const void* p);
	void push_realloc(const CallStack& callStack, const void* p, const void* np, size_t size);

public: // 处理线程调用
	BOOL pop(uint_r& pos, AllocRec*& allocRec, FreeRec*& freeRec, ReallocRec*& reallocRec);
	BOOL _pop(KDataBuffer* buff, uint_r& pos, AllocRec*& allocRec, FreeRec*& freeRec, ReallocRec*& reallocRec);

public:
	BOOL swap(); // 切换两个缓存
	void reset();

public:
	KDataBufferTmpl<512*1024> m_buff1;
	KDataBufferTmpl<512*1024> m_buff2;
	KDataBuffer* m_current; // 当前线程记录用
	KDataBuffer* m_proc;    // 处理线程正在处理的记录
	::System::Sync::KSync_PulseEvent m_event; // 等待处理缓存的处理完事件
	KMemoryRecorder* m_pNextNode;
	volatile BOOL m_inqueue;
};

class KMemoryMonitor : public ::System::Thread::KThread
{
public:
	typedef const void* ptr;
	struct mu { CallStack callStack; size_t size; };
	class cmp_p
	{
	public:
		int operator() (const ptr& a, const ptr& b) const
		{
			uint_r x = (uint_r)a;
			uint_r y = (uint_r)b;
			if(x < y) return -1;
			else if(x == y) return 0;
			else return 1;
		}
	};
	typedef ::System::Sync::KMTLock KMTLock;
	typedef ::System::Collections::KSDSelfList<KMemoryRecorder,KMTLock> Queue;
	typedef ::System::Collections::KMapByTree_pooled<ptr,mu,10240,cmp_p> ResultMap;
	typedef ::System::Collections::DynArray<KMemoryRecorder*> RecorderArray;

public:
	KMemoryMonitor();
	~KMemoryMonitor();
	static KMemoryMonitor* getInstance();

public:
	void Execute();
	void start();
	void stop();
	void push(KMemoryRecorder* recorder);
	void handle_allocRec(const CallStack& callStack, const void* p, size_t size);
	void handle_freeRec(const CallStack& callStack, const void* p);
	void handle_reallocRec(const CallStack& callStack, const void* p, const void* pn, size_t size);
	void dumpResult();
	KMemoryRecorder* getThreadRecorder();
	void addRecorder(KMemoryRecorder* recorder);
	void handle_recorder_buff(KMemoryRecorder* recorder, KDataBuffer* buff);

public:
	int m_tls;
	uint_r m_threadID;
	Queue m_queue;
	volatile BOOL m_stopDesired;
	::System::Sync::KSync_PulseEvent m_event;
	ResultMap m_allocMap;
	ResultMap m_freeMap;
	KMemoryRecorder* m_buff[1024];
	::System::Sync::KThreadMutex m_mx;
	RecorderArray m_recorders;
};

}};
#endif