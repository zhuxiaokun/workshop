#pragma once

/// 主动发起的命令

enum
{
	csc_move,			// 移动
	csc_changeSpeed,	// 改变移动速度
	csc_stop,			// 停止
	csc_useSkill,		// 使用技能
	csc_count,
};

class KSkill;

struct csc_move_d // csc_move data
{
	float fx, fy;
	float tx, ty;
	int mode;
};

struct csc_changeSpeed_d
{
	float speed;
};

struct csc_stop_d
{
	float distance;
};

struct csc_useSkill_d
{
	KSkill* skill;
};
