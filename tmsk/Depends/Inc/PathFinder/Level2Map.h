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
	//��������������Ϣ����
	typedef ::System::Collections::KVector<POINT> STEP_TYPE;

public:
	KLevel2Map();
	virtual ~KLevel2Map();
public:
	//һ���ڽ�·����Ϣ
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
	//һ���ڵ���Ϣ
	struct JOINT
	{
		DWORD id;
		int x;
		int y;

		/*
		 * ������Ƚϴ�С��Ϊ����������ٶ�λ�á�
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
			//��Σ�գ�ָ��Ϊ����ô�죿
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

	//����ڵ��б�����
	typedef ::System::Collections::KSortedVector<JOINT, JOINT::PtLess> JOINT_LIST_TYPE;

	/*
	 * ��Ŀ����id�;�����Ϣ
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
	//���嵽�ڵ�ľ����б�
	typedef ::System::Collections::KSortedVector<DISTANCE_INFO, DISTANCE_INFO::DistanceLess> PT_LIST_WITH_DISTANCE_TYPE;

public:
	/**
	 * ��������õ�һ���ڵ�
	 * param:
	 *		x, y:		����
	 * return:
	 *		����ڵ���ڣ����ؽڵ�ָ�룬����NULL
	 */
	const JOINT* FindJoint(int x, int y) const;

	/**
	 * ��������õ�һ���ڵ�
	 * param:
	 *		id:			�ڵ�id
	 * return:
	 *		����ڵ���ڣ����ؽڵ�ָ�룬����NULL
	 */
	const JOINT* FindJoint(DWORD id) const;

	/**
	 * Serialize input
	 */
	friend std::istream& operator >>(std::istream& is, KLevel2Map& map);

	/**
	 * ��ͼ��λ
	 */
	void Reset();

	/**
	 * ���Ѿ���õ�·����������·�����������GetLastRoad()�õ���
	 * param:
	 *		from:		�������
	 *		to:			�յ�����
	 * return:
	 *		�����
	 */
	bool FindPath(DWORD from, DWORD to, STEP_TYPE& st, int& cost);

	/**
	 * ���ݵ�Ŀ���ľ�����������б�
	 * param:
	 *		fromx, fromy:		��ʼ������
	 *		ptList:				���������������
	 */
	void ComputePtListWithDistance(int fromx, int fromy, PT_LIST_WITH_DISTANCE_TYPE& ptList);

	/**
	 * ����·��toAppend��st��β��
	 * param:
	 *		st:			��ƴ�ӵ�·��
	 *		toAppend:	ƴ�ӵ�·��
	 */
	static void AppendPath(STEP_TYPE& st, const STEP_TYPE& toAppend);

	/**
	 * ����·��toAppend��st��β��
	 * param:
	 *		st:			��ƴ�ӵ�·��
	 *		toAppend:	ƴ�ӵ�·��
	 */
	static void AppendPath(STEP_TYPE& st, const ONE_ROAD& toAppend);


	/**
	 * �õ��ڵ��б�
	 */
	const JOINT_LIST_TYPE& GetJointList() const
	{
		return m_pts;
	}

	/**
	 * ���������ڽӽڵ�������õ�һ��ֱ������·
	 * param:
	 *		from:		�������
	 *		to:			Ŀ������
	 * return:
	 *		�����ͨ������Ӧ·��ָ�룬����NULL
	 */
	const ONE_ROAD* GetRoad(DWORD from, DWORD to) const;

	/**
	 * ƽ�ƶ�����ͼ(x, y)����λ
	 * param:
	 *		x, y:		ƽ�Ƶ�λ�á�
	 */
	void OffsetMap(int x, int y);

	/**
	 * ����AStar�㷨
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
	//����·�ĳ�
	ONE_ROAD m_roads[MAX_ROAD];

	//��Щ·��ʹ�õı�־λ
	System::Collections::KBitset<MAX_ROAD> usedRoad;
// 	int usedRoad[MAX_ROAD];

	//��Щ�ڵ㱻ʹ�õı�־λ
	System::Collections::KBitset<MAX_PTS> usedJointID;
// 	int usedJointID[MAX_PTS];

	//ȫ������·���㷨
	ISPath* m_pSPath;


	//·����ͼ
	//ʹ�ýڵ�id��Ϊ�±꣬[from idx][to idx]
	::System::Collections::KMapByVector<DWORD, ::System::Collections::KMapByVector<DWORD, DWORD> > m_l2map;

	//ע�⣡�ǰ���������ģ�
	//���д����Ľڵ�
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
	//��λ�ñ���ڵ����굽������
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

	//�ڲ�ʹ�õ�A*�㷨
	CAStar* m_pAStar;

	int offsetX;
	int offsetY;
};

std::istream& operator >>(std::istream& is, KLevel2Map& map);
