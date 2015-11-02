/********************************************************************
created:	2011/2/28
filename: 	CommonLogic\StaticTable\KRandom.h
file path:	CommonLogic\StaticTable
file base:	KRandom
file ext:	h
author:		zaki
copyright:	AuroraGame

purpose:	
*********************************************************************/

#ifndef _KRANDOM_H_
#define _KRANDOM_H_

#include "../KType.h"
#include "../KMacro.h"
#include <algorithm>
#include "../Memory/KStepObjectPool.h"

namespace System
{
	namespace Random
	{
		class KRandom_2 : public ::System::Memory::KPortableStepPool<KRandom_2, 16>
		{
		public:
			KRandom_2() {}
			virtual ~KRandom_2() {}

			virtual BOOL Initialize(DWORD aValidCount, DWORD aMaxCount)
			{
				if( aValidCount > aMaxCount ) aValidCount = aMaxCount;
				while( aMaxCount-- > 0 ) { mRandomPool.push_back(0); }
				INT idx = 0;
				while( aValidCount > 0 )
				{
					idx = rand() % mRandomPool.size();
					if( !mRandomPool.at( idx ) )
					{
						mRandomPool.at( idx ) = 1;
						aValidCount--;
					}
				}
				return TRUE;
			}

			virtual DWORD GetRandom()
			{
				DWORD lVal = mRandomPool.at(0);
				mRandomPool.erase(0);
				return lVal;
			}

		public:
			::System::Collections::DynArray< DWORD > mRandomPool;
		};

		class KRandom
		{
		public:
			KRandom();
			KRandom(unsigned int seed);
			~KRandom();

			unsigned int GetRandom();
			unsigned int GetRandom(unsigned int beg, unsigned int end);
			float GetFRandom(unsigned int lessthan);
			double GetDRandom(unsigned int lessthan);

		private:
			unsigned int m_seed;
		};

		/** 
		* 抽牌算法
		* 原理：从n张牌中抽m张牌(前提n>=m)，每张牌被抽中的概率为left_m/left_n(即剩余抽牌数/剩余牌数)
		* 参数：n为总牌数，m为抽牌数，res为抽出牌的结果集
		*/
		void Random_DrawCard(int n, int m, ::System::Collections::DynArray<int>& res);


		/*
		关于随机排列算法
		请直接使用 random_shuffle
		random_shuffle 2种使用规则
		1. random_shuffle(first, last)
			1.1 例如 一个静态数组 INT num[10] = {1,2,3,4,5,6,7,8,9,10}
				随机排列可以std::random_shuffle(num, num + 10)

			1.2 例如 一个动态数组 DynArray<INT> num
				随机排列可以std::random_shuffle(numD.m_vals, numD.m_vals + 10)

			1.3 例如 一个有迭代器的任意容器 std::vector<INT> num KVector没有迭代器 不能使用
				随机排列可以std::random_shuffle(num.begin(), num.end())

		2. random_shuffle(first, last, randomfun)
			random_shuffle的算法说明 有助于编写自己的randomfun
			randomfun是一个函数对象
			这个函数对象的参数是算法遍历序列时的index
			返回值是0-X之间的一个随机数 X可由用户来决定
			默认的random_shuffle中, 被操作序列的index 与 rand() % index 两个位置的值交换

			randomfun 可以是这样的

			int myrandom (int i) { return rand() % i;}

			也可以这样

			class MyRand   
			{       
			public:   
				int operator()(int index)   
				{   
					return rand() % 52;   
				}
			};
		*/
	};
};

#endif // _KRANDOM_H_