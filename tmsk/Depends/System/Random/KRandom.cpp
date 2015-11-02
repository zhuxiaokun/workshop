/********************************************************************
created:	2011/2/28
filename: 	CommonLogic\StaticTable\KRandom.h
file path:	CommonLogic\StaticTable
file base:	KRandom
file ext:	cpp
author:		zaki
copyright:	AuroraGame

purpose:	
*********************************************************************/

#include "KRandom.h"
#include <time.h>
#define K_RAND_MAX	0x3fffffff
namespace System
{
	namespace Random
	{
		KRandom::KRandom():m_seed((UINT)(time(NULL) * GetTickCount()))
		{
		}

		KRandom::KRandom(unsigned int seed):m_seed(seed)
		{
		}

		KRandom::~KRandom()
		{
		}

		unsigned int KRandom::GetRandom()
		{
			return ( ((m_seed = m_seed * 214013L + 2531011L) >> 16) & 0x7fff ) | ( ((m_seed = m_seed * 214013L + 2531011L) >> 1) & 0x3fff8000 );
		}

		unsigned int KRandom::GetRandom(unsigned int beg, unsigned int end)	// 值域[beg,end)
		{
			if (beg == end) return beg;
			double n = (this->GetRandom() / (double)(K_RAND_MAX+1));	// 值域[0,n)
			return (int)(n * (end - beg)) + beg;
		}

		float KRandom::GetFRandom(unsigned int lessthan)
		{
			unsigned int n = this->GetRandom(0, 1001);
			return n/1000.0f/(0 == lessthan ? 1.0f : lessthan);
		}

		double KRandom::GetDRandom(unsigned int lessthan)
		{
			unsigned int n = this->GetRandom(0, 1001);
			return n/(double)1000/(0 == lessthan ? 1.0f : lessthan);
		}

		void Random_DrawCard( int n, int m, ::System::Collections::DynArray<int>& res )
		{
			res.clear();	// 清空结果
			// 需要保证m,n的合理性,n>=m
			for (int i=0;i<n;i++)
			{
				if (rand()%(n-i) < m)
				{
					res.push_back(i);
					--m;
				}
				if(!m) break;
			}
		}
	};
};