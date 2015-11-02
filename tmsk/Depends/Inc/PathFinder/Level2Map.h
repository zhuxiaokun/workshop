/********************************************************************
created:	2009/3/17
filename: 	Level2Map.h
file path:	
file base:	Level2Map
file ext:	h
author:		Allen Wang
copyright:	AuroraGame

purpose:	It contains a class named KLevel2Map that used to create
			higher level map for finding path.
*********************************************************************/
#pragma once

#include <System/KType.h>
#include <System/Collections/KVector.h>
#include <System/Collections/KMapByVector.h>
#include <System/Collections/KBitset.h>
#include "PathFinder.h"
#include <iostream>
#include "SPath.h"
#include <math.h>

class KLevel2Map
{
#ifdef _DEBUG
public:
	void DebugTest()
	{
		if (m_pSPath && m_pSPath->GetKindID() == 1)
		{
			((KSPath*)m_pSPath)->DebugTest();
		}
	}
#endif // _DEBUG

	static const int MAX_PTS = 10000;
	static const int MAX_ROAD = 15000;
	static const int POINT_BLOCK_SIZE = 64;

public:
	//定义对外的行走信息类型
	typedef ::System::Collections::KVector<POINT> STEP_TYPE;

public:
	KLevel2Map();
	virtual ~KLevel2Map();
public:
	//一条邻接路的信息
	struct ONE_ROAD
	{
		STEP_TYPE road;
		int cost;
		void Reset()
		{
			road.clear();
			cost = 0xffffff;
		}
	};
	//一个节点信息
	struct JOINT
	{
		DWORD id;
		int x;
		int y;

		/*
		 * 按坐标比较大小，为根据坐标快速定位用。
		 */
		class PtLess
		{
		public:
			bool operator()(const JOINT& a, const JOINT& b) const
			{
				if (a.x < b.x)
				{
					return true;
				}
				else if (a.x == b.x)
				{
					return a.y < b.y;
				}
				else
				{
					return false;
				}
			}
		};
		class IDPointerLess
		{
		public:
			//有危险，指针为空怎么办？
			bool operator()(const JOINT*& a, const JOINT*& b) const
			{
				if (!a || !b)
				{
					throw "Null pointer error!";
					return true;
				}
				return a->id < b->id;
			}
		};
		JOINT(const POINT& pt)
		{
			x = pt.x;
			y = pt.y;
			id = 0;
		}
		JOINT(){};
		JOINT(int x, int y) : x(x), y(y){}
	};

	//定义节点列表类型
	typedef ::System::Collections::KSortedVector<JOINT, JOINT::PtLess> JOINT_LIST_TYPE;

	/*
	 * 到目标点的id和距离信息
	 */
	struct DISTANCE_INFO
	{
		const JOINT* pJoint;
		DWORD distance;
		class DistanceLess
		{
		public:
			bool operator()(const DISTANCE_INFO& a, const DISTANCE_INFO& b) const
			{
				return a.distance < b.distance;
			}
		};
	};
	//定义到节点的距离列表
	typedef ::System::Collections::KSortedVector<DISTANCE_INFO, DISTANCE_INFO::DistanceLess> PT_LIST_WITH_DISTANCE_TYPE;

public:
	/**
	 * 根据坐标得到一个节点
	 * param:
	 *		x, y:		坐标
	 * return:
	 *		如果节点存在，返回节点指针，否则NULL
	 */
	const JOINT* FindJoint(int x, int y) const;

	/**
	 * 根据坐标得到一个节点
	 * param:
	 *		id:			节点id
	 * return:
	 *		如果节点存在，返回节点指针，否则NULL
	 */
	const JOINT* FindJoint(DWORD id) const;

	/**
	 * Serialize input
	 */
	friend std::istream& operator >>(std::istream& is, KLevel2Map& map);

	/**
	 * 地图复位
	 */
	void Reset();

	/**
	 * 从已经算好的路中搜索二级路径，结果可由GetLastRoad()得到。
	 * param:
	 *		from:		起点索引
	 *		to:			终点索引
	 * return:
	 *		结果。
	 */
	bool FindPath(DWORD from, DWORD to, STEP_TYPE& st, int& cost);

	/**
	 * 根据到目标点的距离计算排序列表
	 * param:
	 *		fromx, fromy:		起始点坐标
	 *		ptList:				用以填充排序序列
	 */
	void ComputePtListWithDistance(int fromx, int fromy, PT_LIST_WITH_DISTANCE_TYPE& ptList);

	/**
	 * 连接路径toAppend到st的尾部
	 * param:
	 *		st:			被拼接的路径
	 *		toAppend:	拼接的路径
	 */
	static void AppendPath(STEP_TYPE& st, const STEP_TYPE& toAppend);

	/**
	 * 连接路径toAppend到st的尾部
	 * param:
	 *		st:			被拼接的路径
	 *		toAppend:	拼接的路径
	 */
	static void AppendPath(STEP_TYPE& st, const ONE_ROAD& toAppend);


	/**
	 * 得到节点列表。
	 */
	const JOINT_LIST_TYPE& GetJointList() const
	{
		return m_pts;
	}

	/**
	 * 根据两个邻接节点的索引得到一条直接连接路
	 * param:
	 *		from:		起点索引
	 *		to:			目的索引
	 * return:
	 *		如果连通返回相应路径指针，否则NULL
	 */
	const ONE_ROAD* GetRoad(DWORD from, DWORD to) const;

	/**
	 * 平移二级地图(x, y)个单位
	 * param:
	 *		x, y:		平移的位置。
	 */
	void OffsetMap(int x, int y);

	/**
	 * 设置AStar算法
	 */
	void SetAStar(CAStar* pAStar)
	{
		m_pAStar = pAStar;
	}
	CAStar* GetAStar()
	{
		return m_pAStar;
	}

protected:
	//所有路的池
	ONE_ROAD m_roads[MAX_ROAD];

	//哪些路被使用的标志位
	System::Collections::KBitset<MAX_ROAD> usedRoad;
// 	int usedRoad[MAX_ROAD];

	//哪些节点被使用的标志位
	System::Collections::KBitset<MAX_PTS> usedJointID;
// 	int usedJointID[MAX_PTS];

	//全局最优路径算法
	ISPath* m_pSPath;


	//路径地图
	//使用节点id作为下标，[from idx][to idx]
	::System::Collections::KMapByVector<DWORD, ::System::Collections::KMapByVector<DWORD, DWORD> > m_l2map;

	//注意！是按坐标排序的！
	//所有创建的节点
	JOINT_LIST_TYPE m_pts;

	class PointLess
	{
	public:
		bool operator()(const POINT& a, const POINT& b) const
		{
			if (a.x < b.x) return true;
			if (a.x == b.x) return a.y < b.y;
			return false;
		}
	};
	//按位置保存节点坐标到区块中
	::System::Collections::KMapByVector<
		int, ::System::Collections::KMapByVector<
		int, ::System::Collections::KSortedVector<POINT, KLevel2Map::PointLess> > >		m_ptsByBlock;
	void InsertJointToBlock(int x, int y)
	{
		POINT pt;
		pt.x = x;
		pt.y = y;
		m_ptsByBlock[pt.x / POINT_BLOCK_SIZE][pt.y / POINT_BLOCK_SIZE].insert_unique(pt);
	}
	void EraseJointFromBlock(int x, int y)
	{
		POINT pt;
		pt.x = x;
		pt.y = y;
		int blockx = pt.x / POINT_BLOCK_SIZE;
		int blocky = pt.y / POINT_BLOCK_SIZE;
		if (m_ptsByBlock.find(blockx) != m_ptsByBlock.end()
			&& m_ptsByBlock[blockx].find(blocky) != m_ptsByBlock[blockx].end())
		{
			int pos = m_ptsByBlock[blockx][blocky].find(pt);
			if (pos >= 0)
			{
				m_ptsByBlock[blockx][blocky].erase(pos);
			}
			if (m_ptsByBlock[blockx][blocky].size() == 0)
			{
				m_ptsByBlock[blockx].erase(blocky);
				if (m_ptsByBlock[blockx].size() == 0)
				{
					m_ptsByBlock.erase(blockx);
				}
			}
		}
	}
	void RecomputeJointBlock()
	{
		m_ptsByBlock.clear();
		for (int i = 0; i < m_pts.size(); i++)
		{
			InsertJointToBlock(m_pts[i].x, m_pts[i].y);
		}
	}

	inline void GetJointBlockPos(int x, int y, int& bx, int& by) const
	{
		bx = x / POINT_BLOCK_SIZE;
		by = y / POINT_BLOCK_SIZE;
	}

	//内部使用的A*算法
	CAStar* m_pAStar;

	int offsetX;
	int offsetY;
};

std::istream& operator >>(std::istream& is, KLevel2Map& map);
