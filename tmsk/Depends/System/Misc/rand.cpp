#include "rand.h"
#include <time.h>
#include "../KPlatform.h"

#define K_RAND_MAX	0x3fffffff
::System::Misc::CRandom g_Random;

namespace System { namespace Misc {

CRandom::CRandom():m_seed((UINT)(time(NULL) * GetTickCount()))
{
}

CRandom::CRandom(unsigned int seed):m_seed(seed)
{
}

CRandom::~CRandom()
{
}

unsigned int CRandom::GetRandom()
{
	return ( ((m_seed = m_seed * 214013L + 2531011L) >> 16) & 0x7fff ) | ( ((m_seed = m_seed * 214013L + 2531011L) >> 1) & 0x3fff8000 );
}
// [beg,end)
unsigned int CRandom::GetRandom(unsigned int beg, unsigned int end)
{
	if (beg == end) return beg;
	double n = (this->GetRandom() / (double)(K_RAND_MAX+1));
	return (int)(n * (end - beg)) + beg;
}

// 0.0f - 1.0f
float CRandom::GetFRandom()
{
	unsigned int n = this->GetRandom(0, 1001);
	return n/1000.0f;
}

}}
