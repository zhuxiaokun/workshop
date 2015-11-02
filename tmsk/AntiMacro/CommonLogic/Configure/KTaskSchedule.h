#ifndef _K_TASK_SCHEDULE_H_
#define _K_TASK_SCHEDULE_H_

#include <System/Collections/DynArray.h>

// ʵ��һ��ȫ��������ȵĹ���
// ��ָ����ʱ�����ִ��ע��Ĺ���


// ���Ա����ȵ��������
class KTaskObject
{
public:
	virtual ~KTaskObject();

	// �´����񼤻��ʱ��(mill-sec : GetTickCount)
	// ���� 0 ��ʾ���������
	virtual DWORD GetNextActivateTick() const = 0;
	
	// ����ʱ����Ҫ�Ĺ���������ִ��
	virtual void  OnActivate() = 0;

	// �ͷű���������������ԺͶ�������
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
	public:	// �Ӵ�С����
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
