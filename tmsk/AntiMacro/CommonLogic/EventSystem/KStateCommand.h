#pragma once

/// �������������

enum
{
	csc_move,			// �ƶ�
	csc_changeSpeed,	// �ı��ƶ��ٶ�
	csc_stop,			// ֹͣ
	csc_useSkill,		// ʹ�ü���
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
