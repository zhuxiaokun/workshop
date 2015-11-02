/********************************************************************
created:	2011/3/14
manager:	Allen Wang
copyright:	Aurora Interactive, 2011
*********************************************************************/

#pragma once
#include "../KType.h"
#include "../Collections/KVector.h"

namespace System
{
	namespace Random
	{
		/**
		 * 随机组
		 * 按概率权重和数据塞入该组，再从该组中取出。每个组的对象可能是一个数字，或者是一组连续（等概率）的数字。
		 * 所填概率为命中某对象的概率。如果是连续数字对象，则认为对象内的连续数字等概率。
		 */
		class KRandomGroup
		{
		public:
			// 概率分布的最大权重（所有权重的和的极限）
			const static DWORD MAX_RANGE;
		private:
			struct ELEM_DATA
			{
				//在其之前插入的所有元素的概率和
				DWORD m_dwRate;
				//元素类型
				enum TYPE
				{
					enumT_Number,	// 一个数字
					enumT_Range,	// 连续数字
				} m_type;
				//元素数据
				union
				{
					int m_num;
					struct
					{
						int m_min;
						int m_max;
					} m_range;
				};
				ELEM_DATA();
				bool operator < (const ELEM_DATA& other) const;
				int GetValue() const;
			};

		private:
			class KSortedVectorWithKey : public ::System::Collections::KSortedVector<ELEM_DATA>
			{
			public:
				/**
				 * 根据Key查找元素。
				 * param:
				 *		key:	主键
				 *		bFound:	是否找到
				 * return:
				 *		返回要插入的位置。
				 */
				int find(const DWORD& key, BOOL& bFound) const
				{
					ELEM_DATA ed;
					ed.m_dwRate = key;
					return this->bsearch(ed, bFound);
				}
			};

			//用以存储元素数据
			//Warning: 使用删除操作，会产生概率合并！
			KSortedVectorWithKey m_vecData;

			/**
			 * 随机选定一个元素出来
			 * return:
			 *		选中的元素。如不存在选定元素则返回NULL（一般是组为空）。
			 */
			const ELEM_DATA* GetRandomElement() const;

		public:
			/**
			 * 插入一个组中元素
			 * param:
			 *		rate:		选中这个元素的概率
			 *		data:		插入的元素的数据
			 *		type:		插入的元素的类型
			 */
			void InsertData(DWORD rate, int data);

			/**
			 * 插入一个组中元素
			 * param:
			 *		rate:		选中这个元素的概率
			 *		dwRangeMin:	插入的元素的下限
			 *		dwRangeMax:	插入的元素的上限
			 */
			void InsertRange(DWORD rate, int rangeMin, int rangeMax);

			/**
			 * 清除Group
			 */
			void Clear();

			/**
			 * 判定是否有数据
			 * return:
			 *		已经有数据返回false，否则true
			 */
			bool IsEmpty() const;

			/**
			 * 随机选定一个元素，将其值返回
			 * return:
			 *		选中的元素的值。如不存在选定元素则返回0。
			 */
			int GetRandomVal() const;
			/**
			 * 随机选定一个元素，将其值返回，并且在组中删除该元素
			 * return:
			 *		选中的元素的值。如不存在选定元素则返回0。
			 */
			int TakeRandomVal();
		};
	}
}