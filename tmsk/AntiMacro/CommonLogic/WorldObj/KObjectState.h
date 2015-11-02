#pragma once

#include <System/KType.h>
#include <System/KMacro.h>

enum
{
	cs_none,		// ��Ч״̬
	cs_stand,		// վ��״̬
	cs_move,		// �ƶ�״̬���ߡ��ܣ�
	cs_death,		// ����״̬
	cs_count,
};

class KWorldObj;

/**
 * onCommand ����������������ҵĲ�����NPC��AI��
 * onEvent   ϵͳ�����Ķ�״̬��Ӱ����¼�
 */
class KObjectState
{
public:
	int m_sid; // state id
	KWorldObj* m_object; // owner object

public:
	KObjectState();
	virtual ~KObjectState();

public:
	virtual void onEnterState();
	virtual void onLeaveState();
	virtual void onCommand(int cmd, const void* data, int len);
	virtual void onEvent(int evt, const void* data, int len);
	virtual void breathe(int interval);
	virtual void reset();
	virtual void destroy();
};
