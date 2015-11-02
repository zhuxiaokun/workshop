/********************************************************************
created:	2009/3/17
filename: 	Level2Map.cpp
file path:	
file base:	Level2Map
file ext:	cpp
author:		Allen Wang
copyright:	AuroraGame

purpose:	Implementation part of one Level2Map.
*********************************************************************/
#include "Level2Map.h"
#include "SPathByAStar.h"
#include "System/Log/log.h"
using namespace std;

KLevel2Map::KLevel2Map()
{
	usedRoad.SetAll();
	usedJointID.SetAll();
	KSPathByAStar* p = new KSPathByAStar;
	p->SetL2Map(this);
	m_pSPath = p;
	m_pAStar = NULL;
	offsetX = 0;
	offsetY = 0;
}

KLevel2Map::~KLevel2Map()
{
	if (m_pSPath)
	{
		delete m_pSPath;
		m_pSPath = NULL;
	}
}

const KLevel2Map::JOINT* KLevel2Map::FindJoint( int x, int y ) const
{
	JOINT joint;
	joint.x = x;
	joint.y = y;
	int pos = m_pts.find(joint);
	if (pos >= 0)
	{
		return &(m_pts[pos]);
	}
	return NULL;
}

const KLevel2Map::JOINT* KLevel2Map::FindJoint( DWORD id ) const
{
	for (int i = 0; i < m_pts.size(); i++)
	{
		if (m_pts[i].id == id)
		{
			return &(m_pts[i]);
		}
	}
	return NULL;
}

void KLevel2Map::Reset()
{
	m_l2map.clear();
	usedRoad.SetAll();
	usedJointID.SetAll();
	m_pts.clear();
	for (int i = 0; i < MAX_ROAD; i++)
	{
		m_roads[i].Reset();
	}
	m_ptsByBlock.clear();
	if (m_pSPath)
	{
		delete m_pSPath;
		m_pSPath = NULL;
	}
}

bool KLevel2Map::FindPath( DWORD from, DWORD to, KLevel2Map::STEP_TYPE& st, int& cost )
{
	cost = 0;
	st.clear();
	if (from == to)
	{
		const JOINT* p = FindJoint(from);
		if (p)
		{
			POINT pt;
			pt.x = p->x;
			pt.y = p->y;
			st.push_back(pt);
		}
		return st.size() != 0;
	}
	DWORD buff[1000];
	DWORD size = 1000;
	if (m_pSPath)
	{
		if (m_pSPath->GetKindID() == 1)
		{
			DWORD start = 0;
			DWORD end = 0;
			for (int i = 0; i < m_pts.size(); i++)
			{
				if (m_pts[i].id == from)
				{
					start = i;
				}
				if (m_pts[i].id == to)
				{
					end = i;
				}
			}
			if (m_pSPath->GetPath(start, end, buff, size, cost) > 0)
			{
				DWORD d1 = buff[0];
				DWORD d2 = 0;
				for (DWORD i = 1; i < size; i++)
				{
					d2 = buff[i];
					const ONE_ROAD* pr = GetRoad(m_pts[d1].id, m_pts[d2].id);
					if (m_pts[d1].id > m_pts[d2].id)
					{
						//反向填充路径
						for (int j = pr->road.size() - 1; j >= 0; j--)
						{
							POINT pt;
							pt.x = pr->road[j].x;
							pt.y = pr->road[j].y;
							if (st.size() > 0 && st[st.size() - 1].x == pt.x && st[st.size() - 1].y == pt.y)
							{
								continue;
							}
							st.push_back(pt);
						}
					}
					else
					{
						//正向填充
						for (int j = 0; j < pr->road.size(); j++)
						{
							POINT pt;
							pt.x = pr->road[j].x;
							pt.y = pr->road[j].y;
							if (st.size() > 0 && st[st.size() - 1].x == pt.x && st[st.size() - 1].y == pt.y)
							{
								continue;
							}
							st.push_back(pt);
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
		else
		{
			if (m_pSPath->GetPath(from, to, buff, size, cost) > 0)
			{
				DWORD d1 = buff[0];
				DWORD d2 = 0;
				for (DWORD i = 1; i < size; i++)
				{
					d2 = buff[i];
					const ONE_ROAD* pr = GetRoad(d1, d2);
					if (d1 > d2)
					{
						//反向填充路径
						for (int j = pr->road.size() - 1; j >= 0; j--)
						{
							POINT pt;
							pt.x = pr->road[j].x;
							pt.y = pr->road[j].y;
							if (st.size() > 0 && st[st.size() - 1].x == pt.x && st[st.size() - 1].y == pt.y)
							{
								continue;
							}
							st.push_back(pt);
						}
					}
					else
					{
						//正向填充
						for (int j = 0; j < pr->road.size(); j++)
						{
							POINT pt;
							pt.x = pr->road[j].x;
							pt.y = pr->road[j].y;
							if (st.size() > 0 && st[st.size() - 1].x == pt.x && st[st.size() - 1].y == pt.y)
							{
								continue;
							}
							st.push_back(pt);
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
	}
	return false;
}

void KLevel2Map::AppendPath( STEP_TYPE& st, const STEP_TYPE& toAppend )
{
	for (int i = 0; i < toAppend.size(); i++)
	{
		POINT pt;
		pt.x = toAppend[i].x;
		pt.y = toAppend[i].y;
		if (st.size() > 0 && st[st.size() - 1].x == pt.x && st[st.size() - 1].y == pt.y)
		{
			continue;
		}
		st.push_back(pt);
	}
}

void KLevel2Map::AppendPath( STEP_TYPE& st, const ONE_ROAD& toAppend )
{
	for (int i = 0; i < toAppend.road.size(); i++)
	{
		POINT pt;
		pt.x = toAppend.road[i].x;
		pt.y = toAppend.road[i].y;
		if (st.size() > 0 && st[st.size() - 1].x == pt.x && st[st.size() - 1].y == pt.y)
		{
			continue;
		}
		st.push_back(pt);
	}
}

void KLevel2Map::ComputePtListWithDistance( int fromx, int fromy, PT_LIST_WITH_DISTANCE_TYPE& ptList )
{
	ASSERT(512 >= POINT_BLOCK_SIZE);
	ptList.clear();
	int blx, bhx, bly, bhy, bx, by;
	GetJointBlockPos(fromx, fromy, bx, by);
	//窗口使用附近3x3个区域
	blx = bx - 1;
	bhx = bx + 1;
	bly = by - 1;
	bhy = by + 1;

	//如果窗口延伸超出当前512将被剪裁。使用大地图的时候可以考虑去掉这个限制。
	{
		bx /= 512 / POINT_BLOCK_SIZE;
		by /= 512 / POINT_BLOCK_SIZE;
		if (blx / (512 / POINT_BLOCK_SIZE) != bx)
		{
			blx++;
		}
		if (bhx / (512 / POINT_BLOCK_SIZE) != bx)
		{
			bhx--;
		}
		if (bly / (512 / POINT_BLOCK_SIZE) != by)
		{
			bly++;
		}
		if (bhy / (512 / POINT_BLOCK_SIZE) != by)
		{
			bhy--;
		}
	}

	for (bx = blx; bx <= bhx; bx++)
	{
		if (m_ptsByBlock.find(bx) == m_ptsByBlock.end())
		{
			continue;
		}
		for (by = bly; by <= bhy; by++)
		{
			if (m_ptsByBlock[bx].find(by) == m_ptsByBlock[bx].end())
			{
				continue;
			}
			for (int i = 0; i < m_ptsByBlock[bx][by].size(); i++)
			{
				DISTANCE_INFO di;
				int x = m_ptsByBlock[bx][by][i].x;
				int y = m_ptsByBlock[bx][by][i].y;
				di.pJoint = FindJoint(x, y);
				di.distance = (DWORD)((pow((double)(fromx - x), 2) + pow((double)(fromy - y), 2)) * 10000);
				ptList.insert(di);
			}
		}
	}
}

const KLevel2Map::ONE_ROAD* KLevel2Map::GetRoad( DWORD from, DWORD to ) const
{
	if (from >= MAX_PTS || to >= MAX_PTS)
	{
		return NULL;
	}
	if (m_l2map.find(from) != m_l2map.end())
	{
		if (m_l2map[from].find(to) != m_l2map[from].end())
		{
			return &m_roads[m_l2map[from][to]];
		}
	}
	return NULL;
}

istream& operator >>(istream& is, KLevel2Map& map)
{
	map.Reset();
	DWORD count;
	//读点总数
	is.read((char*)&count, sizeof(DWORD));
	//读每个点
	DWORD i;
	for (i = 0; i < count; i++)
	{
		KLevel2Map::JOINT joint;
		is.read((char*)&joint.id, sizeof(DWORD));
		is.read((char*)&joint.x, sizeof(int));
		is.read((char*)&joint.y, sizeof(int));
		ASSERT(joint.id > 0 && joint.id < KLevel2Map::MAX_PTS);
		map.m_pts.insert_unique(joint);
		map.usedJointID.Clear(joint.id);
	}

	//读路总数
	is.read((char*)&count, sizeof(DWORD));
	for (i = 0; i < count; i++)
	{
		//读每条路
		int from;
		int to;
		DWORD idx;
		int cost;
		int size;
		is.read((char*)&from, sizeof(int));
		is.read((char*)&to, sizeof(int));
		is.read((char*)&idx, sizeof(DWORD));
		is.read((char*)&cost, sizeof(int));
		is.read((char*)&size, sizeof(int));
		ASSERT(from > 0 && from < KLevel2Map::MAX_PTS);
		ASSERT(to > 0 && to < KLevel2Map::MAX_PTS);
		ASSERT(idx > 0 && idx < KLevel2Map::MAX_ROAD);
		map.usedRoad.Clear(idx);
		map.m_roads[idx].cost = cost;
		for (int j = 0; j < size; j++)
		{
			int x;
			int y;
			is.read((char*)&x, sizeof(int));
			is.read((char*)&y, sizeof(int));
			POINT pt;
			pt.x = x;
			pt.y = y;
			map.m_roads[idx].road.push_back(pt);
		}
		map.m_l2map[from][to] = idx;
		map.m_l2map[to][from] = idx;	//双向路径，写两边
	}
	//TAG: 读二级的map
	char ch;	//是否计算路径标志
	is.read((char*)&ch, sizeof(char));
	if (ch == 0)
	{
		ASSERT(FALSE);
	}
	else if (ch == 1)
	{
		Log(LOG_FATAL, "这种格式的二级地图文件已经被废弃");
		ASSERT(FALSE);
		if (map.m_pSPath && map.m_pSPath->GetKindID() != ch)
		{
			delete map.m_pSPath;
			map.m_pSPath = NULL;
		}
		if (!map.m_pSPath)
		{
			map.m_pSPath = new KSPath;
		}
		is >> *(KSPath*)map.m_pSPath;
	}
	else if (ch == 2)
	{
		if (map.m_pSPath && map.m_pSPath->GetKindID() != ch)
		{
			delete map.m_pSPath;
			map.m_pSPath = NULL;
		}
		if (!map.m_pSPath)
		{
			KSPathByAStar* p = new KSPathByAStar;
			p->SetL2Map(&map);
			map.m_pSPath = p;
		}
		is >> *(KSPathByAStar*)map.m_pSPath;
	}
	map.RecomputeJointBlock();
	if (map.m_pSPath)
	{
		map.m_pSPath->Compute();
	}

	return is;
}

void KLevel2Map::OffsetMap(int x, int y)
{
	if (x == offsetX && y == offsetY)
	{
		return;
	}
	int ox = x - offsetX;
	int oy = y - offsetY;
	offsetX = x;
	offsetY = y;
	x = ox;
	y = oy;
	int i;
	for (i = 0; i < m_pts.size(); i++)
	{
		m_pts[i].x += x;
		m_pts[i].y += y;
	}
	::System::Collections::KMapByVector<DWORD, ::System::Collections::KMapByVector<DWORD, DWORD> >::iterator itx;
	::System::Collections::KMapByVector<DWORD, DWORD>::iterator ity;
	for (itx = m_l2map.begin(); itx != m_l2map.end(); itx++)
	{
		for (ity = itx->second.begin(); ity != itx->second.end(); ity++)
		{
			if (itx->first <= ity->first)	//TAG: 由于使用双向路径，需要跳过一半
			{
				break;
			}
			for (i = 0; i < m_roads[ity->second].road.size(); i++)
			{
				m_roads[ity->second].road[i].x += x;
				m_roads[ity->second].road[i].y += y;
			}
		}
	}
	RecomputeJointBlock();
}
