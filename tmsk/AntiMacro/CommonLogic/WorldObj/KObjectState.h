#pragma once

#include <System/KType.h>
#include <System/KMacro.h>

enum
{
	cs_none,		// 无效状态
	cs_stand,		// 站立状态
	cs_move,		// 移动状态（走、跑）
	cs_death,		// 死亡状态
	cs_count,
};

class KWorldObj;

/**
 * onCommand 主动发起的命令，如玩家的操作，NPC的AI等
 * onEvent   系统产生的对状态有影响的事件
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
