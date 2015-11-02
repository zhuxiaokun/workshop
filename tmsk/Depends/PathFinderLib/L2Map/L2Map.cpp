#include "L2Map.h"
#include <math.h>
#include "../PerfHitch/perf_PathFinderLib.h"

namespace KPathFinderAbout
{
	KL2Map_Base::KL2Map_Base()
	{	PERF_COUNTER(KL2Map_Base_KL2Map_Base);

		m_pFinder = NULL;
		offsetX = 0;
		offsetY = 0;
		mPosArray.clear();
		mIDArray.clear();
		mPosMap.clear();
		mIDMap.clear();
		mRoads.clear();
		m_L2MapByBlock.clear();
	}

	KL2Map_Base::~KL2Map_Base()
	{	PERF_COUNTER(KL2Map_Base_D_KL2Map_Base);

		m_pFinder = NULL;
		offsetX = 0;
		offsetY = 0;
		mPosArray.clear();
		mIDArray.clear();
		mPosMap.clear();
		mIDMap.clear();
		mRoads.clear();
		m_L2MapByBlock.clear();
	}

	const _Node_Lite * KL2Map_Base::Find(int_r x, int_r y) const
	{
		_Node_Lite temp(0,x,y);
		int pos = mPosArray.find(&temp);
		return -1 == pos ? NULL : mPosArray[pos];
	}

	const _Node_Lite * KL2Map_Base::Find(int_r id) const
	{
		_Node_Lite temp(id,0,0);
		int pos = mIDArray.find(&temp);
		return -1 == pos ? NULL : mIDArray[pos];
	}

	bool KL2Map_Base::FindPath(int_r from, int_r to, __Nodes& st, VALUE_T& cost)
	{	PERF_COUNTER(KL2Map_Base_FindPath);

		cost = 0;
		st.clear();
		if (from == to)
		{
			const _Node_Lite * p = Find(from);
			if (p)
			{
				push_point(st, p);
			}
			return st.size() != 0;
		}

		JG_C::DynArray<int_r> nodeIDs;
		if (m_pFinder)
		{
			if (m_pFinder->GetPath(from, to, nodeIDs, cost) > 0)
			{
				int_r d1 = nodeIDs[0];
				int_r d2 = 0;
				for (int_r i = 1; i < nodeIDs.size(); i++)
				{
					d2 = nodeIDs[i];
					const __OneRoad * pr = GetRoad(d1, d2);
					if ( pr != NULL )
					{
						//正向填充
						for (int_r j = 0; j < pr->m_Road.size(); j++)
						{
							if (st.size() > 0 && st[st.size() - 1] == pr->m_Road[j])
							{
								continue;
							}
							st.push_back(pr->m_Road[j]);
						}
					}
					d1 = d2;
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		return false;
	}

	void KL2Map_Base::ComputePtListWithDistance(int_r fromx, int_r fromy, __NodeList_ByDistance& ptList)
	{	PERF_COUNTER(KL2Map_Base_ComputePtListWithDistance);

		ASSERT(mpSettings && mpSettings->mMapSize >= mpSettings->mBlockSize);
		ptList.clear();
		int_r blx, bhx, bly, bhy, bx, by;
		GetJointBlockPos(fromx, fromy, bx, by);
		// 窗口使用附近3x3个区域
		blx = bx - 1;
		bhx = bx + 1;
		bly = by - 1;
		bhy = by + 1;

		//如果窗口延伸超出当前512将被剪裁。使用大地图的时候可以考虑去掉这个限制。
		int_r nRate = mpSettings->mMapSize / mpSettings->mBlockSize;
		ASSERT(0 != nRate);
		bx /= nRate;
		by /= nRate;
		if (blx / nRate != bx)
		{
			blx++;
		}
		if (bhx / nRate != bx)
		{
			bhx--;
		}
		if (bly / nRate != by)
		{
			bly++;
		}
		if (bhy / nRate != by)
		{
			bhy--;
		}

		__DistanceInfo di;
		for (bx = blx; bx <= bhx; ++bx)
		{
			for (by = bly; by <= bhy; ++by)
			{
				int_r nSize = m_L2MapByBlock[bx][by].size();
				for (int_r i = 0; i < nSize; ++i)
				{
					int_r x = m_L2MapByBlock[bx][by][i]->x;
					int_r y = m_L2MapByBlock[bx][by][i]->y;
					di.first = Find(x, y);
					ASSERT(di.first);
					di.second = sqrtf(Distance2D(fromx,fromy,x,y));
					ptList.insert(di);
				}
			}
		}
	}

	const __OneRoad * KL2Map_Base::GetRoad(int_r from, int_r to) const
	{
		__NodeL2Map_ByID::const_iterator itX = mIDMap.find(from);
		if ( mIDMap.end() != itX )
		{
			__NodeMap_ByID::const_iterator itY = itX->second.find(to);
			if ( itX->second.end() != itY )
			{
				return &mRoads[itY->second];
			}
		}
		return NULL;
	}

	void KL2Map_Base::OffsetMap(int_r x, int_r y)
	{	PERF_COUNTER(KL2Map_Base_OffsetMap);

		if (x == offsetX && y == offsetY)
		{
			return;
		}
		int_r ox = x - offsetX;
		int_r oy = y - offsetY;
		offsetX = x;
		offsetY = y;
		x = ox;
		y = oy;
		int_r i;
		for (i = 0; i < mPosArray.size(); i++)
		{
			(const_cast<_Node_Lite *>(mPosArray[i]))->x += x;
			(const_cast<_Node_Lite *>(mPosArray[i]))->y += y;
		}
		
		//int_r nSize1 = mIDMap.size();
		//int_r nSize2 = 0;
		//int_r nKey = 0;
		//for (int_r i = 0; i < nSize1; ++i)
		//{
		//	nSize2 = mIDMap[i].size();
		//	for (int_r j = 0; j < nSize2; ++j)
		//	{
		//		nKey = mIDMap[i][j];
		//		for (i = 0; i < mRoads[nKey].m_Road.size(); i++)
		//		{
		//			(const_cast<_Node_Lite *>(mRoads[nKey].m_Road[i]))->x += x;
		//			(const_cast<_Node_Lite *>(mRoads[nKey].m_Road[i]))->y += y;
		//		}
		//	}
		//}
		RecomputeJointBlock();
	}

	void KL2Map_Base::GetJointBlockPos(int_r x, int_r y, int_r& bx, int_r& by) const
	{	PERF_COUNTER(KL2Map_Base_GetJointBlockPos);

		if(mpSettings)
		{
			bx = x / mpSettings->mBlockSize;
			by = y / mpSettings->mBlockSize;
		}
		else
		{
			bx = x;
			by = y;
		}
	}

	void KL2Map_Base::RecomputeJointBlock()
	{	PERF_COUNTER(KL2Map_Base_RecomputeJointBlock);

		m_L2MapByBlock.clear();
		int_r nSize = mPosArray.size();
		bool bRet = false;
		for(int_r i = 0; i < nSize; ++i)
		{
			bRet = _InsertJointToBlock( mPosArray[i] );
			ASSERT(bRet);
		}
	}

	bool KL2Map_Base::AddOneJoint(const _Node_Lite * pNode)
	{	PERF_COUNTER(KL2Map_Base_AddOneJoint);

		ASSERT(pNode->id > 0);
		if( Find(pNode->x, pNode->y) )
			return false;
		if( Find(pNode->id) )
			return false;
		int_r pos = mPosArray.insert_unique(pNode);
		ASSERT(pos > -1);
		pos = mIDArray.insert_unique(pNode);
		ASSERT(pos > -1);
		bool bRet = _InsertJointToBlock(pNode);
		ASSERT(bRet);
		return bRet;
	}

	bool KL2Map_Base::CreateOneRoad(int_r nID, const _Node_Lite * pBegin, const _Node_Lite * pEnd, bool bUnidirectional)
	{	PERF_COUNTER(KL2Map_Base_CreateOneRoad);

		mRoads[nID].m_Cost = sqrtf(Distance2D(pBegin->x, pBegin->y, pEnd->x, pEnd->y));
		mRoads[nID].push_back(pBegin);
		mRoads[nID].push_back(pEnd);
		mIDMap[pBegin->id][pEnd->id] = nID;
		if(!bUnidirectional)
		{
			mIDMap[pEnd->id][pBegin->id] = nID;
		}
		return true;
	}

	bool KL2Map_Base::_InsertJointToBlock(const _Node_Lite * pNode)
	{	PERF_COUNTER(KL2Map_Base__InsertJointToBlock);

		ASSERT_RETURN(pNode, false);
		int_r bx(-1), by(-1);
		GetJointBlockPos(pNode->x, pNode->y, bx, by);
		ASSERT_RETURN(bx >= 0, false);
		ASSERT_RETURN(by >= 0, false);
		return m_L2MapByBlock[bx][by].insert_unique(pNode) > -1;
	}
}