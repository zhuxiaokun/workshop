/********************************************************************
created:	2009/3/25
filename: 	KSPathByAStar.h
file path:	
file base:	KSPathByAStar
file ext:	h
author:		Allen Wang
copyright:	AuroraGame

purpose:	It contains an algorithm that to compute global shortest path.
*********************************************************************/

#pragma once

#include <System/KType.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "Level2Map.h"
#include "SPath.h"
#include "PathFinder.h"
#include <System/Collections/KMapByVector.h>
// #include <hash_map>


class KSPathByAStar : public ISPath
{
public:
	KSPathByAStar()
	{
		m_pAStar = NULL;
		m_pL2M = NULL;
	}
	virtual void SetMapSize(DWORD howManyNode)
	{

	}
	virtual bool SetCost(DWORD from, DWORD to, int cost)
	{
		return TRUE;
	}
	virtual void Compute()
	{
		CreateAdjacencyList();
	}
	virtual int GetPath(DWORD from, DWORD to, DWORD* nodeIDBuffer, DWORD& size, int& cost)
	{
		size = 0;
		if (!m_pL2M)
		{
			return 0;
		}
		if (from == to)
		{
			return 0;
		}
		SetAStar(m_pL2M->GetAStar());
		if (!m_pAStar) return false;
		if (!SetupAStar())
		{
			return 0;
		}
		const KLevel2Map::JOINT* p1 = m_pL2M->FindJoint(from);
		const KLevel2Map::JOINT* p2 = m_pL2M->FindJoint(to);
		if (!p1 || !p2) return false;

		cost = 0;
		if (m_pAStar->GeneratePath(p1->x, p1->y, p2->x, p2->y, 200, p1->id, p2->id) == 1)
		{
			_asNode* p = m_pAStar->GetBestNode();
			while(p)
			{
				nodeIDBuffer[size++] = p->number;
				p = p->parent;
				if (size >= 2)
				{
					const KLevel2Map::ONE_ROAD* pRd = m_pL2M->GetRoad(nodeIDBuffer[size - 1], nodeIDBuffer[size - 2]);
					if (pRd)
					{
						cost += pRd->cost;
					}
				}
			}

			//反向
			for (DWORD i = 0; i < size / 2; i++)
			{
				DWORD temp = nodeIDBuffer[i];
				nodeIDBuffer[i] = nodeIDBuffer[size - i - 1];
				nodeIDBuffer[size - i - 1] = temp;
			}
			return 1;
		}
		return 0;
	}
	virtual int GetKindID() const
	{
		return 2;
	}

	void SetAStar(CAStar* pAStar)
	{
		m_pAStar = pAStar;
	}

	void SetL2Map(KLevel2Map* pL2M)
	{
		this->m_pL2M = pL2M;
// 		CreateAdjacencyList();
	}

	void CreateAdjacencyList()
	{
		m_AdjacencyList.clear();
		if (m_pL2M)
		{
			const KLevel2Map::JOINT_LIST_TYPE& jlt = m_pL2M->GetJointList();
			for (int i = 0; i < jlt.size(); i++)
			{
				for (int j = 0; j < jlt.size(); j++)
				{
					if (i == j) continue;
					const KLevel2Map::ONE_ROAD* pRd = m_pL2M->GetRoad(jlt[i].id, jlt[j].id);
					if (pRd)
					{
						m_AdjacencyList[jlt[i].id][jlt[j].id] = &jlt[j];
#ifdef _DEBUG
						ASSERT(m_AdjacencyList[jlt[i].id].size() < ASTAR_MAX_CHILDREN);
#endif
					}
				}
			}
		}
		
	}

protected:
	CAStar* m_pAStar;
	KLevel2Map* m_pL2M;
// 	typedef stdext::hash_map<DWORD, const KLevel2Map::JOINT*> ADJACENCY_LIST_SUB_TYPE;
// 	typedef stdext::hash_map<DWORD, ADJACENCY_LIST_SUB_TYPE> ADJACENCY_LIST_TYPE;
	typedef ::System::Collections::KMapByVector<DWORD, const KLevel2Map::JOINT*> ADJACENCY_LIST_SUB_TYPE;
	typedef ::System::Collections::KMapByVector<DWORD, ADJACENCY_LIST_SUB_TYPE> ADJACENCY_LIST_TYPE;
	ADJACENCY_LIST_TYPE m_AdjacencyList;
	static int AS_CreateChild(const _asNode* node, _asNode* buff, void* pointer)
	{
		KSPathByAStar *me = reinterpret_cast<KSPathByAStar*>(pointer);

		if (me->m_AdjacencyList[node->number].size() > 0)
		{
			int count = 0;
			ADJACENCY_LIST_SUB_TYPE::iterator it;
			for (it = me->m_AdjacencyList[node->number].begin(); it != me->m_AdjacencyList[node->number].end(); it++)
			{
				buff[count].x = it->second->x;
				buff[count].y = it->second->y;
				buff[count].number = (int)it->second->id;
				count++;
			}
			return count;
		}
		else
		{
			return 0;
		}
	}

	bool SetupAStar()
	{
		if (!m_pAStar) return false;

		m_pAStar->udValid = KSPathByAStar::AS_Valid;
		m_pAStar->udCost = KSPathByAStar::AS_Cost;
		m_pAStar->udHeuristici = KSPathByAStar::AS_DistanceHeuristici;
		m_pAStar->udCreateChild = KSPathByAStar::AS_CreateChild;

		m_pAStar->udNotifyList = NULL;
		m_pAStar->udNotifyChild = NULL;

		m_pAStar->m_pCBData = reinterpret_cast<void *>(this);
		m_pAStar->m_pNCData = NULL;

		return true;
	}

	static VALUE_TYPE AS_Valid(_asNode *parent, _asNode *node, int data, void *pointer)
	{
		//TAG: 一定有效，扩展出来的节点都可达
		if (!pointer) return FALSE;
		return TRUE;
	}

	static VALUE_TYPE AS_Cost(_asNode *parent, _asNode *node, int data, void *pointer)
	{
		KSPathByAStar *me = reinterpret_cast<KSPathByAStar*>(pointer);
		int cost = 10000;
		//假定一定能拿到
// 		if (me->m_pL2M)
		{
			cost = me->m_pL2M->GetRoad(parent->number, node->number)->cost;
		}

		return cost;
	}

	static VALUE_TYPE AS_DistanceHeuristici(_asNode *start, _asNode *end, int data, void *pointer)
	{
		VALUE_TYPE Heuristici = (VALUE_TYPE)(sqrt(pow((double)(start->x - end->x), 2) + pow((double)(start->y - end->y), 2)) * 100);

		return Heuristici;
	}
};

std::ostream& operator <<(std::ostream& os, const KSPathByAStar& spath);
std::istream& operator >>(std::istream& is, KSPathByAStar& spath);
