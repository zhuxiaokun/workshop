#pragma once

enum
{
	cse_beHit,		// ������
	cse_count,
};

class KCharacter;
class KSkill;

struct cse_beHit_d
{
	KCharacter* who;
	KSkill* skill;
};
