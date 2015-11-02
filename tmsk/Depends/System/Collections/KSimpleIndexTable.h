#ifndef _KSIMPLEINDEXTABLE_H_
#define _KSIMPLEINDEXTABLE_H_

#include "../KType.h"
#include "../KMacro.h"
#include "../Memory/KMemory.h"
#include "./DynArray.h"
#include <stdlib.h>

namespace System { namespace Collections {

	template<typename BaseObjType,typename IndexType = int>
	class KSimpleIndexTable
	{
	public:
		typedef BaseObjType* BaseObjTypePtr;
		typedef KPair<IndexType,BaseObjTypePtr> BaseObjNode;

		class BaseObjNodeCompare
		{
		public:
			BOOL operator () (const BaseObjNode &a1, const BaseObjNode &a2) const
			{
				return (a1.first <  a2.first);
			}
		};

	public:
		KSimpleIndexTable()
			:mpObjBuf(NULL)
			,miIndexMax(0)
		{

		}
		virtual ~KSimpleIndexTable()
		{
			if(NULL != mpObjBuf)
			{
				delete[]mpObjBuf;
				mpObjBuf=  NULL;
			}
			miIndexMax = 0;
		}
		virtual void init(size_t  iIndexMax)
		{
			mpObjBuf = new BaseObjTypePtr[iIndexMax];
			memset(mpObjBuf,0,sizeof(BaseObjTypePtr)*iIndexMax);
			miIndexMax = iIndexMax;
		}

		// 
		virtual bool addObj(IndexType iIndex,BaseObjTypePtr pObj)
		{
			if(iIndex>=miIndexMax)
			{
				return false;
			}

			if(NULL != mpObjBuf[(size_t)iIndex])
			{
				removeObjByIndex(iIndex);
			}

			mpObjBuf[iIndex] = pObj;
			BaseObjNode TempNode;
			TempNode.first = iIndex;
			TempNode.second = pObj;
			mTraverseList.insert_unique(TempNode);
			return true;
		}

		void removeObj(BaseObjTypePtr pObj)
		{
			size_t i = 0;
			IndexType iIndex = miIndexMax;
			for(i = 0;i<miIndexMax;++i)
			{
				if(mpObjBuf[i] == pObj)
				{
					mpObjBuf[i] = NULL;
					iIndex = (IndexType)i;
					break;
				}
			}

			if(iIndex<(IndexType)miIndexMax)
			{
				BaseObjNode TempNode;
				TempNode.first = iIndex;
				TempNode.second = NULL;
				mTraverseList.erase(TempNode);
			}

		}

		void removeObjByIndex(IndexType iIndex)
		{
			if(iIndex>=0 && iIndex<(IndexType)miIndexMax)
			{
				mpObjBuf[iIndex] = NULL;

				BaseObjNode TempNode;
				TempNode.first = iIndex;
				TempNode.second = NULL;

				mTraverseList.erase(TempNode);
			}
		}


		BaseObjTypePtr findObjByIndex(IndexType iIndex)
		{
			return 	mpObjBuf[iIndex];
		}

		BaseObjTypePtr getObjByTraverseIndex(size_t iIndex) 
		{
			return mTraverseList[iIndex].second;
		}


		void clear()
		{
			if(NULL != mpObjBuf)
			{
				delete[]mpObjBuf;
				mpObjBuf=  NULL;
			}
			miIndexMax = 0;
			mTraverseList.clear();
		}
		size_t getIndexMax(){return miIndexMax;}
		size_t getTraverseCount(){return mTraverseList.size();}

	
	protected:
		// 为遍历用
		DynSortedArray<BaseObjNode,BaseObjNodeCompare>  mTraverseList;
		// 为查找用
		BaseObjTypePtr* mpObjBuf;
		size_t		    miIndexMax;
	
	};
	
	
};	};

#endif