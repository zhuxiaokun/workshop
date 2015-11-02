#ifndef __BASE_RAND_H__
#define __BASE_RAND_H__

#include "../KType.h"
#include "../KMacro.h"

namespace System { namespace Misc {

class CRandom
{
public:
	CRandom();
	CRandom(unsigned int seed);
	~CRandom();

	unsigned int GetRandom();
	unsigned int GetRandom(unsigned int beg, unsigned int end);
	float GetFRandom(); // 0.0f - 1.0f

private:
	unsigned int m_seed;
};

}}
extern ::System::Misc::CRandom g_Random;
#endif