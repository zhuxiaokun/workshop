#pragma once

enum
{
	cse_beHit,		// ±»¹¥»÷
	cse_count,
};

class KCharacter;
class KSkill;

struct cse_beHit_d
{
	KCharacter* who;
	KSkill* skill;
};
