#include "DirectAStar.h"
#include "CommonUtils.h"
#include "../PerfHitch/perf_PathFinderLib.h"

namespace KPathFinderAbout
{
	KDirectAStar::KDirectAStar():IAlgorithmDynArray<__d_Cmp>()
	{	PERF_COUNTER(KDirectAStar_KDirectAStar);

	}

	KDirectAStar::~KDirectAStar() 
	{	PERF_COUNTER(KDirectAStar_D_KDirectAStar);

		ReleaseNodes();
	}

	void KDirectAStar::ReleaseNodes()
	{	PERF_COUNTER(KDirectAStar_ReleaseNodes);

		_Node_DirectAStar::ResetPool();
		IAlgorithmDynArray<__d_Cmp>::ReleaseNodes();
	}

	int_r KDirectAStar::GeneratePath(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did)
	{	PERF_COUNTER(KDirectAStar_GeneratePath);

		ReleaseNodes();
		if (sx == dx && sy == dy)
		{
			return 0;
		}
		StepInitialize(sx, sy, dx, dy, sid, did);

		_Node_DirectAStar * pFirst = _Node_DirectAStar::Alloc();
		if (pFirst == NULL)
		{
			ASSERT(FALSE);
		}
		new (pFirst) _Node_DirectAStar();
		pFirst->x = sx;
		pFirst->y = sy;
		pFirst->g = 0;
		pFirst->h = GetPlugin()->AS_Heuristici(&m_nodeStart, &m_nodeEnd, 0, 0);
		pFirst->f = pFirst->g + pFirst->h;
		pFirst->id = sid;
		add_to_open(pFirst);

		int_r nStepCount = 0;
		_Node tempNode[ASTAR_MAX_CHILDREN];
		while(m_pBest = get_from_open(0))
		{
			add_to_close(m_pBest);
			erase_from_open(m_pBest);

			// is target
			if(m_pBest->id == m_nodeEnd.id)
			{
				return nStepCount;
			}

			nStepCount++;
			if(nStepCount >= GetMaxLoop())
			{
				return -1;
			}

			// create children
			memset(tempNode, 0, sizeof(tempNode));
			int_r n = GetPlugin()->AS_CreateChild(m_pBest, tempNode, ASTAR_MAX_CHILDREN, 0);
			n = n > ASTAR_MAX_CHILDREN ? ASTAR_MAX_CHILDREN : n;
			for (int_r i = 0; i < n; i++)
			{
				if (0 == GetPlugin()->AS_Valid(m_pBest, &tempNode[i], 0, 0)) continue;
				if(find_from_close(tempNode[i].id))
					continue;
				if(find_from_open(tempNode[i].id))
					continue;

				_Node_DirectAStar * pChild = _Node_DirectAStar::Alloc();
				new (pChild) _Node(tempNode[i]);
				if (pChild == NULL)
				{
					ASSERT(FALSE);
				}
				pChild->parent = m_pBest;
				pChild->g = GetPlugin()->AS_Cost(m_pBest, pChild, 0, 0);
				pChild->h = GetPlugin()->AS_Heuristici(pChild, &m_nodeEnd, 0, 0);
				pChild->f = pChild->g + pChild->h;
				pChild->dir = g_GetDirDiff(m_pBest->x, m_pBest->y, m_nodeEnd.x, m_nodeEnd.y, pChild->x, pChild->y);
				add_to_open(pChild);
			}

			// create d children
			memset(tempNode, 0, sizeof(tempNode));
			n = GetPlugin()->AS_CreateDiagonalChild(m_pBest, tempNode, ASTAR_MAX_CHILDREN, 0);
			n = n > ASTAR_MAX_CHILDREN ? ASTAR_MAX_CHILDREN : n;
			for (int_r i = 0; i < n; i++)
			{
				if (0 == GetPlugin()->AS_Valid(m_pBest, &tempNode[i], 0, 0)) continue;
				if(find_from_close(tempNode[i].id))
					continue;
				if(find_from_open(tempNode[i].id))
					continue;

				_Node_DirectAStar * pChild = _Node_DirectAStar::Alloc();
				new (pChild) _Node(tempNode[i]);
				if (pChild == NULL)
				{
					ASSERT(FALSE);
				}
				pChild->parent = m_pBest;
				pChild->g = GetPlugin()->AS_Cost(m_pBest, pChild, 0, 0);
				pChild->h = GetPlugin()->AS_Heuristici(pChild, &m_nodeEnd, 0, 0);
				pChild->f = pChild->g + pChild->h;
				pChild->dir = g_GetDirDiff(m_pBest->x, m_pBest->y, m_nodeEnd.x, m_nodeEnd.y, pChild->x, pChild->y);
				add_to_open(pChild);
			}
		}

		return 0;
	}

	void KDirectAStar::StepInitialize(int_r sx, int_r sy, int_r dx, int_r dy, int_r &sid, int_r &did)
	{	PERF_COUNTER(KDirectAStar_StepInitialize);

		m_nodeStart.x = m_iSX = sx;
		m_nodeStart.y = m_iSY = sy;
		m_nodeEnd.x = m_iDX = dx;
		m_nodeEnd.y = m_iDY = dy;

		if (sid == 0)
		{
			sid = GetPlugin()->Coord2ID(sx, sy);
		}
		if (did == 0)
		{
			did = GetPlugin()->Coord2ID(dx, dy);
		}
		m_nodeStart.id = sid;
		m_nodeEnd.id = did;
	}
}