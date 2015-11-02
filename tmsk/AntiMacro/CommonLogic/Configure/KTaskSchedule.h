#ifndef _K_TASK_SCHEDULE_H_
#define _K_TASK_SCHEDULE_H_

#include <System/Collections/DynArray.h>

// 实现一个全局任务调度的功能
// 在指定的时间点上执行注册的工作


// 可以被调度的任务基类
class KTaskObject
{
public:
	virtual ~KTaskObject();

	// 下次任务激活的时间(mill-sec : GetTickCount)
	// 返回 0 表示任务结束了
	virtual DWORD GetNextActivateTick() const = 0;
	
	// 激活时，需要的工作在这里执行
	virtual void  OnActivate() = 0;

	// 释放本对象，这个函数可以和对象池配合
	virtual void  Destroy() = 0;

protected:
	void Destrot_Delete();
};

class KTaskSchedule
{
public:
	typedef KTaskObject* KTaskPtr;
	class KTaskLess
	{
	public:	// 从大到小排序
		BOOL operator () (const KTaskPtr& a, const KTaskPtr& b) const
		{
			DWORD tick1 = a->GetNextActivateTick();
			DWORD tick2 = b->GetNextActivateTick();
			return tick1 > tick2;
		}
	};

	typedef System::Collections::DynArray<KTaskPtr> KTaskArray;
	typedef System::Collections::DynSortedArray<KTaskPtr,KTaskLess,256,256> KTaskQueue;

public:
	KTaskSchedule();
	~KTaskSchedule();

public:
	BOOL Register(KTaskObject* pTask);
	void Breathe(int interval);

public:
	KTaskQueue m_taskQueue;
	KTaskArray m_taskArray;
};

#endif
