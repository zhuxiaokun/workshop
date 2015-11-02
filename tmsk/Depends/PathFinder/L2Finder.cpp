/********************************************************************
created:	2009/3/23
filename: 	L2Finder.cpp
file path:	
file base:	L2Finder
file ext:	cpp
author:		Allen Wang
copyright:	AuroraGame

purpose:	Implementation part of L2Finder.
*********************************************************************/
#include "L2Finder.h"
#include <fstream>
#include "System/Log/log.h"
using namespace std;

bool KL2Finder::LoadL2PathInfo(const char* filename /*= L2MAP_INFO_FILENAME*/)
{
	std::ifstream ifs(filename, ios_base::binary);
	if (!ifs.is_open())
	{
		Log(LOG_FATAL, "�޷��򿪶�����ͼ�ļ�\"%s\"��", filename);
		ASSERT(false);
		return false;
	}
	ifs >> m_mapSet;
	return true;
}

bool KL2Finder::LoadL2PathInfo(std::istream& fs)
{
	fs >> m_mapSet;
	return true;
}


bool KL2Finder::FindPathByAStar( int fromx, int fromy, int tox, int toy, KLevel2Map::STEP_TYPE& st, int maxLoop /*= 0*/ )
{
	/*
	if (IsObstacle(fromx, fromy))
	{
// 		sprintf(m_strLastFindResult, "���������ϰ���");
		SetLastResult(enumLFRC_ORG_IN_OBSTACLE);
		return false;
	}
	*/
	if (IsObstacle(tox, toy))
	{
// 		sprintf(m_strLastFindResult, "Ŀ������ϰ���");
		SetLastResult(enumLFRC_TARGET_IN_OBSTACLE);
		return false;
	}
	/*
	KLevel2MapSet::BLOCK_POS bp = m_mapSet.GetBlockFromPos(fromx, fromy);
/*
	if (bp != m_mapSet.GetBlockFromPos(tox, toy))
	{
		//TODO: ��֧�ֿ糡��Ѱ·
// 		sprintf(m_strLastFindResult, "��֧�ֿ糡��AStarѰ·");
		SetLastResult(enumLFRC_DO_NOT_SUPPORT_MULTISCENE_ASTAR);
		return false;
	}
*/
	st.clear();
	if (fromx == tox && fromy == toy)
	{
		POINT pt;
		pt.x = fromx;
		pt.y = fromy;
		st.push_back(pt);
// 		sprintf(m_strLastFindResult, "�Ѿ���Ŀ���");
		SetLastResult(enumLFRC_ALREADY_REACHED);
		return true;
	}
	else
	{
		if (maxLoop != 0)
		{
			m_pAStar->SetMaxLoop(maxLoop);
		}

		KLevel2Map::ONE_ROAD rd;
		SetAStarPathCompact();
		bool bFound = _FindPathByAStar(fromx, fromy, tox, toy, rd);
		SetAStarPathCompact(false);
		if (maxLoop != 0)
		{
			m_pAStar->SetMaxLoop(0x7fffffff);
		}
		if (bFound)
		{
			KLevel2Map::AppendPath(st, rd);
// 			sprintf(m_strLastFindResult, "С��Χ·�������ɹ�");
			SetLastResult(enumLFRC_SMALL_AREA_SEARCH_SUCCESS);
			return true;
		}
	}
// 	sprintf(m_strLastFindResult, "С��Χ·������ʧ��");
	SetLastResult(enumLFRC_SMALL_AREA_SEARCH_FAIL);
	return false;
}


bool KL2Finder::FindPath( int fromx, int fromy, int tox, int toy, KLevel2Map::STEP_TYPE& st, int maxLoop /*= 0*/ )
{
	if (maxLoop <= 0) maxLoop = 5000;
	/*
	if (IsObstacle(fromx, fromy))
	{
// 		sprintf(m_strLastFindResult, "���������ϰ���");
		SetLastResult(enumLFRC_ORG_IN_OBSTACLE);
		return false;
	}
	*/
	if (IsObstacle(tox, toy))
	{
// 		sprintf(m_strLastFindResult, "Ŀ������ϰ���");
		SetLastResult(enumLFRC_TARGET_IN_OBSTACLE);
		return false;
	}
	KLevel2MapSet::BLOCK_POS bp = m_mapSet.GetBlockFromPos(fromx, fromy);
// 	if (bp != m_mapSet.GetBlockFromPos(tox, toy))
// 	{
// 		//TODO: ��֧�ֿ糡��Ѱ·
// // 		sprintf(m_strLastFindResult, "�糡��Ѱ·���ݲ�֧��");
// 		SetLastResult(enumLFRC_DO_NOT_SUPPORT_MULTISCENE_ASTAR);
// 		return false;
// 	}
	st.clear();
	if (fromx == tox && fromy == toy)
	{
		POINT pt;
		pt.x = fromx;
		pt.y = fromy;
		st.push_back(pt);
// 		sprintf(m_strLastFindResult, "�Ѿ���Ŀ���");
		SetLastResult(enumLFRC_ALREADY_REACHED);
		return true;
	}
	else if (LineTest(fromx, fromy, tox, toy))
	{
		POINT pt;
		pt.x = fromx;
		pt.y = fromy;
		st.push_back(pt);
		pt.x = tox;
		pt.y = toy;
		st.push_back(pt);
// 		sprintf(m_strLastFindResult, "ֱ�߿ɴ�");
		SetLastResult(enumLFRC_REACH_DIRECTLY);
		return true;
	}
	else	//����Ŀ�ĵ�Ѱ·�ʹ�ΧѰ·�ıȽ�
	{
		KLevel2Map* pL2M = m_mapSet.GetL2Map(bp);
		pL2M->SetAStar(m_pAStar);

		KLevel2Map::STEP_TYPE tempst1;
		KLevel2Map::STEP_TYPE tempst3;

		const static int MAX_COST = 60000;
		m_pAStar->SetMaxLoop(1000);
		KLevel2Map::ONE_ROAD rdAStar;
		SetAStarPathCompact();
		//TAG: �õ�AStar·��
		if (!_FindPath(fromx, fromy, tox, toy, rdAStar))
		{
			rdAStar.cost = MAX_COST;
		}
/*
		else
		{
			//TAG: ���AStar�ɴ��ֱ����
			KLevel2Map::AppendPath(st, rdAStar);
// 			sprintf(m_strLastFindResult, "С��ΧѰ·�ɹ�");
			SetLastResult(enumLFRC_SMALL_AREA_SEARCH_SUCCESS);
			return true;
		}
*/

		//TAG: �õ�Զ����Ѱ··��
		int cost = 0;
		KLevel2Map::PT_LIST_WITH_DISTANCE_TYPE ptList;
		pL2M->ComputePtListWithDistance(fromx, fromy, ptList);
		KLevel2Map::ONE_ROAD rdToJoint;
		rdToJoint.cost = MAX_COST;
		DWORD id1 = 0;
		DWORD id2 = 0;
		KLevel2Map::ONE_ROAD rdTemp;
		for (int i = 0; i < min(ptList.size(), 5); i++)
		{
			if (ptList[i].distance > (DWORD)rdAStar.cost * (DWORD)rdAStar.cost) break;
			if (ptList[i].distance > (DWORD)rdToJoint.cost * (DWORD)rdToJoint.cost) break;
			if (_FindPath(fromx, fromy, ptList[i].pJoint->x, ptList[i].pJoint->y, rdTemp))
			{
				if (rdTemp.cost < rdToJoint.cost)
				{
					id1 = ptList[i].pJoint->id;
					rdToJoint = rdTemp;
				}
			}
		}
		SetAStarPathCompact(false);
		m_pAStar->SetMaxLoop(0x7fffffff);
		if (id1 == 0)
		{
// 			sprintf(m_strLastFindResult, "�����㸽��û��·��");
			rdToJoint.cost = MAX_COST;
			SetLastResult(enumLFRC_CAN_NOT_FIND_ROAD_POINT_AROUND_ORG);
		}
		else// if (rdAStar.cost != MAX_COST && rdToJoint.cost < rdAStar.cost * 1.1)	//����Ƚ�
		{
			pL2M->ComputePtListWithDistance(tox, toy, ptList);
			if (ptList.size() == 0)
			{
				//Ŀ��㲻�ɴ�
// 				sprintf(m_strLastFindResult, "Զ����Ѱ·��Ŀ���(%d, %d)����û��·��", tox, toy);
				SetLastResult(enumLFRC_CAN_NOT_FIND_ROAD_POINT_AROUND_TARGET, tox, toy);
				rdToJoint.cost = MAX_COST;
// 				return false;
			}
			else
			{
				id2 = ptList[0].pJoint->id;
				//ʹ�ô��ͼѰ·
				if (!pL2M->FindPath(id1, id2, tempst3, cost))
				{
					const KLevel2Map::JOINT* pj = pL2M->FindJoint(id1);
// 					sprintf(m_strLastFindResult, "Զ��Ѱ·Ŀ�긽��·�㲻��ֱ�ӵ���[·��(%d, %d)��·��(%d, %d)]", pj->x, pj->y, ptList[0].pJoint->x, ptList[0].pJoint->y);
					SetLastResult(enumLFRC_CAN_NOT_REACH_FROM_RDPT_TO_RDPT, pj->x, pj->y, ptList[0].pJoint->x, ptList[0].pJoint->y);
					rdToJoint.cost = MAX_COST;
// 					return false;	//���ͼ���ɴ�
				}
				else
				{
					if (tempst3.size() >= 2)
					{
						const KLevel2Map::JOINT* pj1 = pL2M->FindJoint(tempst3[0].x, tempst3[0].y);
						const KLevel2Map::JOINT* pj2 = pL2M->FindJoint(tempst3[1].x, tempst3[1].y);

						if (pj1 && pj2)
						{
							const KLevel2Map::ONE_ROAD* prd = pL2M->GetRoad(pj1->id, pj2->id);
							SetAStarPathCompact(true);
							if (prd && _FindPath(fromx, fromy, tempst3[1].x, tempst3[1].y, rdTemp))
							{
								if (rdTemp.cost < rdToJoint.cost + prd->cost)
								{
									rdToJoint = rdTemp;
									tempst3.erase(0);
								}
							}
							SetAStarPathCompact(false);
						}
					}
				}
			}
		}
		//�Ƚ�
		if (rdToJoint.cost == MAX_COST && rdAStar.cost == MAX_COST)
		{
			//��·(����ΧAStar����)
			m_pAStar->SetMaxLoop(maxLoop);
			SetAStarPathCompact();
			bool b = _FindPath(fromx, fromy, tox, toy, rdTemp);
			SetAStarPathCompact(false);
			m_pAStar->SetMaxLoop(0x7fffffff);
			if (b)
			{
				KLevel2Map::AppendPath(st, rdTemp);
// 				sprintf(m_strLastFindResult, "�з�ΧѰ·�ɹ�");
				CompactPath(st);
				SetLastResult(enumLFRC_MIDDLE_AREA_SEARCH_SUCCESS);
				return true;
			}
// 			sprintf(m_strLastFindResult, "�޷��ҵ�·��");
			SetLastResult(enumLFRC_FAIL);
			return false;
		}
		else
		{
			bool bUseAStar = true;
			if (rdToJoint.cost == MAX_COST)	//ʹ��AStar���
			{
				bUseAStar = true;
			}
			else if (rdAStar.cost == MAX_COST)	//ʹ��Զ������
			{
				bUseAStar = false;
			}
			else	//���о�����Ҫ�Ƚ�
			{
				if (rdToJoint.cost + cost < rdAStar.cost * 1.3)
				{
					bUseAStar = false;
				}
				else
				{
					bUseAStar = true;
				}
			}

			if (bUseAStar)
			{
				KLevel2Map::AppendPath(st, rdAStar);
// 				sprintf(m_strLastFindResult, "��ΧѰ·ʧ�ܵ�С��ΧѰ·�ɹ�");
				CompactPath(st);
				SetLastResult(enumLFRC_LARGE_FAIL_BUT_SMALL_SUCCESS);
				return true;
			}
			else	//ʹ��Զ������
			{
				KLevel2Map::AppendPath(st, rdToJoint);
				KLevel2Map::AppendPath(st, tempst3);
//				sprintf(m_strLastFindResult, "��ΧѰ·�ɹ�");
//				if (st.size() > 0)
//				{
//					if (st[st.size() - 1].x == tox && st[st.size() - 1].y == toy)
//					{
//						SetLastResult(enumLFRC_LARGE_AREA_SEARCH_SUCCESS);
//						return true;
//					}
//				}
//				SetLastResult(enumLFRC_CAN_NOT_FIND_ROAD_POINT_AROUND_TARGET);
//				return false;

				CompactPath(st);
				SetLastResult(enumLFRC_LARGE_AREA_SEARCH_SUCCESS);
				return true;
			}
		}
	}

	return false;
}

DWORD KL2Finder::PtStepToL2Map( KLevel2Map* pL2M, int x, int y, KLevel2Map::STEP_TYPE& st )
{
	if (!pL2M)
	{
		ASSERT(false);
		return 0;
	}
	st.clear();
	const KLevel2Map::JOINT* p = NULL;
	p = pL2M->FindJoint(x, y);
	if (p)
	{
		POINT pt;
		pt.x = x;
		pt.y = y;
		st.push_back(pt);
		return p->id;
	}
	else
	{
		//AHEAD: ��L2�������㷨
		KLevel2Map::PT_LIST_WITH_DISTANCE_TYPE ptList;
		pL2M->ComputePtListWithDistance(x, y, ptList);
		ASSERT(ptList.size() > 0);
		KLevel2Map::ONE_ROAD road;
		if (!_FindPath(x, y, ptList[0].pJoint->x, ptList[0].pJoint->y, road))
		{
			return 0;
		}
		for (int i = 0; i < road.road.size(); i++)
		{
			POINT pt;
			pt.x = road.road[i].x;
			pt.y = road.road[i].y;
			st.push_back(pt);
		}
		return ptList[0].pJoint->id;
	}
}

DWORD KL2Finder::L2MapStepToPt( KLevel2Map* pL2M, int x, int y, KLevel2Map::STEP_TYPE& st )
{
	if (!pL2M)
	{
		ASSERT(false);
		return 0;
	}
	st.clear();
	const KLevel2Map::JOINT* p = NULL;
	p = pL2M->FindJoint(x, y);
	if (p)
	{
		POINT pt;
		pt.x = x;
		pt.y = y;
		st.push_back(pt);
		return p->id;
	}
	else
	{
		//AHEAD: ��L2�������㷨
		KLevel2Map::PT_LIST_WITH_DISTANCE_TYPE ptList;
		pL2M->ComputePtListWithDistance(x, y, ptList);
		ASSERT(ptList.size() > 0);
		//����п���û�б����أ���ֻ�ڱ༭�����ء�
// 		KLevel2Map::ONE_ROAD road;
// 
// 		if (!_FindPath(ptList[0].pJoint->x, ptList[0].pJoint->y, x, y, road))
// 		{
// 			return 0;
// 		}
// 		for (int i = 0; i < road.road.size(); i++)
// 		{
// 			POINT pt;
// 			pt.x = road.road[i].x;
// 			pt.y = road.road[i].y;
// 			st.push_back(pt);
// 		}
		POINT pt;
		pt.x = x;
		pt.y = y;
		st.push_back(pt);
		return ptList[0].pJoint->id;
	}
}

bool KL2Finder::_FindPath( int fromx, int fromy, int tox, int toy, KLevel2Map::ONE_ROAD& road )
{
	if (fromx == tox && fromy == toy)
	{
		road.road.clear();
		POINT pt;
		pt.x = fromx;
		pt.y = fromy;
		road.road.push_back(pt);
		road.cost = 0;
		return true;
	}
	else if (LineTest(fromx, fromy, tox, toy))
	{
		road.road.clear();
		POINT pt;
		pt.x = fromx;
		pt.y = fromy;
		road.road.push_back(pt);
		pt.x = tox;
		pt.y = toy;
		road.road.push_back(pt);
		//ȡ���������ֱ�߾���
		road.cost = (int)(sqrt(pow((double)fromx - tox, 2) + pow((double)fromy - toy, 2)) * 100 + 0.5);
		return true;
	}
	else
	{
		return _FindPathByAStar(fromx, fromy, tox, toy, road);
	}
}

bool KL2Finder::LineTest( int nSrcX, int nSrcY, int nDestX, int nDestY ) const
{
	return m_plugin->LineTest(nSrcX, nSrcY, nDestX, nDestY);
}


bool KL2Finder::IsObstacle( int fromx, int fromy, int x, int y ) const
{
	return !m_plugin->LineTest(fromx, fromy, x, y);
}

bool KL2Finder::IsObstacle( int x, int y ) const
{
	return !m_plugin->LineTest(x, y, x, y);
}

bool KL2Finder::_FindPathByAStar( int fromx, int fromy, int tox, int toy, KLevel2Map::ONE_ROAD& road )
{
	road.Reset();
	if (IsObstacle(fromx, fromy)
		|| IsObstacle(tox, toy))
	{
		return false;
	}
	if (fromx == tox && fromy == toy)
	{
		POINT pt;
		pt.x = fromx;
		pt.y = fromy;
		road.road.push_back(pt);
		road.cost = 0;
		return true;
	}
	else
	{
		m_plugin->SetupAStar(m_pAStar);
//		g_DynamicWorld.GetMainPlayer()->SetupAStar(true, false);
		if (m_pAStar->GeneratePath(fromx, fromy, tox, toy, 1000) == 1)
		{
			_asNode* pNode = m_pAStar->GetBestNodeFromStart();
			if (!pNode)
			{
				return false;
			}
			road.cost = pNode->f;

			//////////////////////////////////////////////////////////////////////////
			/**
			 * ·��ѹ����ѹ������ֱ�����·
			 * ����˼·���ӵ�һ�����������ֱ�߿ɴ��·���������еĵ㶼��������������޷����㣬�Ӳ������ĵط���������
			 * ��������1.2.3.4.5.6.7��7���㣬1��3��1��4��5��7ֱ�߿ɴ������������£�
			 * ����1.3ֱ�߿ɴɾ��2					(1)->3->4->5->6->7
			 * ����1.4ֱ�߿ɴɾ��3					(1)->4->5->6->7
			 * ����1.5ֱ�߲��ɴ���4��ʼ��������		1->(4)->5->6->7
			 * ����4.6ֱ�߲��ɴ��5��ʼ����			1->4->(5)->6->7
			 * ����5.7ֱ�߿ɴɾ��6					1->4->(5)->7
			 * ����7֮���Ԫ�أ�����Ϊ�գ�ѹ������
			 */
			if (m_bAStarCompact)
			{
				_asNode* pNodeAhead = pNode;
				_asNode* pNodeToRemove = NULL;

				pNodeToRemove = pNodeAhead->parent;
				while (pNodeToRemove && pNodeToRemove->parent)
				{
					if (LineTest(pNodeAhead->x, pNodeAhead->y, pNodeToRemove->parent->x, pNodeToRemove->parent->y))
					{
						pNodeAhead->parent = pNodeToRemove->parent;
						pNodeToRemove = pNodeToRemove->parent;
					}
					else
					{
						pNodeAhead = pNodeAhead->parent;
						if (pNodeAhead)
						{
							pNodeToRemove = pNodeAhead->parent;
						}
						else
						{
							pNodeToRemove = NULL;
						}
					}
				}
			}
			// ·��ѹ������
			//////////////////////////////////////////////////////////////////////////

			while (pNode)
			{
				POINT pt;
				pt.x = pNode->x;
				pt.y = pNode->y;
				road.road.push_back(pt);
				pNode = pNode->parent;
			}
			return true;
		}
	}
	return false;
}

bool KL2Finder::ClearL2PathInfo()
{
	m_mapSet.Clear();
	return true;
}

void KL2Finder::CompactPath( KLevel2Map::STEP_TYPE& rd )
{
	while(rd.size() >= 3)
	{
		if (LineTest(rd[0].x, rd[0].y, rd[2].x, rd[2].y))
		{
			rd.erase(1);
		}
		else
		{
			break;
		}
	}
}
