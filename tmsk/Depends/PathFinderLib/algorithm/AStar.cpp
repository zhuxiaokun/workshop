#include "AStar.h"
#include <math.h>
#include "../PerfHitch/perf_PathFinderLib.h"

namespace KPathFinderAbout
{
	KAStar::KAStar():IAlgorithmHash()
	{	PERF_COUNTER(KAStar_KAStar);

		m_pClosed = NULL;
		m_pStack = NULL;
		m_pOpen = NULL;
	}

	KAStar::~KAStar() 
	{	PERF_COUNTER(KAStar_D_KAStar);

		ReleaseNodes();
	}

	void KAStar::ReleaseNodes()
	{	PERF_COUNTER(KAStar_ReleaseNodes);

		//_Node_AStar::ResetPool();
		_Node_AStar* pLoopNode = (_Node_AStar*)m_pOpen;
		while( pLoopNode )
		{
			_Node_AStar::Free(pLoopNode);
			pLoopNode = pLoopNode->next;
		}
		pLoopNode = (_Node_AStar*)m_pClosed;
		while( pLoopNode )
		{
			_Node_AStar::Free(pLoopNode);
			pLoopNode = pLoopNode->next;
		}
		m_pClosed = m_pOpen = NULL;
		IAlgorithmHash::ReleaseNodes();
	}

	int_r KAStar::GetBestNodeStepNum()
	{	PERF_COUNTER(KAStar_GetBestNodeStepNum);

		int_r nRet = 0;
		int_r nX = 0;
		int_r nY = 0;
		if (m_pBest)
		{
			nX = m_pBest->x - m_nodeStart.x;
			nX = nX < 0 ? -nX : nX;
			nY = m_pBest->y - m_nodeStart.y;
			nY = nY < 0 ? -nY : nY;
			nRet = nX + nY;
		}
		return nRet;
	}

	int_r KAStar::GeneratePath(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did)
	{	PERF_COUNTER(KAStar_GeneratePath);

		ReleaseNodes();
		m_pBest = NULL;
		if (sx == dx && sy == dy)
		{
			return 0;
		}
		StepInitialize(sx, sy, dx, dy, sid, did);

		// Add step count to stop searching
		int_r nStepCount = 0;
		int_r retval = 0;

		int_r nBestNodeStepNum =0;
		int_r nRealBestNodeStepNum =0;
		while (retval == 0) 
		{
			nStepCount++;
			if ( m_pBest )
			{
				_Node_AStar * pBest = (_Node_AStar *)m_pBest;
				nBestNodeStepNum = pBest->nStepNum;
				if (nStepCount >= m_maxLoop)
				{
					nRealBestNodeStepNum = GetBestNodeFromStartNum();
					retval = -2;
					break;

				}
				else if ( nBestNodeStepNum >= nMaxStep)
				{
					nRealBestNodeStepNum = GetBestNodeFromStartNum();
					if ( nRealBestNodeStepNum >= nMaxStep)
					{
						retval = 2;
						break;
					}
				}
			}
			retval = Step();
		};

		if (!m_pBest) 
		{
			m_pBest = NULL;
			return -1;
		}

		return retval;
	}

	void KAStar::StepInitialize(int_r sx, int_r sy, int_r dx, int_r dy, int_r &sid, int_r &did)
	{	PERF_COUNTER(KAStar_StepInitialize);

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
		m_nodeEnd.id =  m_iDNum = did;

		_Node_AStar *temp = _Node_AStar::Alloc();
		if (temp == NULL)
		{
			ASSERT(FALSE);
			return;
		}
		new (temp) _Node_AStar();
		temp->x = sx;
		temp->y = sy;
		temp->g = 0;
		temp->h = GetPlugin()->AS_Heuristici(&m_nodeStart, &m_nodeEnd, 0, m_pCBData);
		temp->f = temp->g + temp->h;
		m_nodeStart.id = temp->id = sid;

		AddToOpen(temp);
		GetPlugin()->AS_Notify_ByList(NULL, m_pOpen, ASNL_STARTOPEN, m_pNCData);
		GetPlugin()->AS_Notify_ByChild(NULL, temp, 0, m_pNCData);
	}

	int_r KAStar::GetBestNodeFromStartNum()
	{	PERF_COUNTER(KAStar_GetBestNodeFromStartNum);

		int_r nNum = 0;
		if (m_pBest)
		{
			_Node * pNode = m_pBest;
			nNum++;
			pNode = pNode->parent;

			while(pNode != NULL)
			{
				nNum++;
				pNode = pNode->parent;
			}
		}
		return nNum;
	}

	int_r KAStar::Step()
	{	PERF_COUNTER(KAStar_Step);

		if (!(m_pBest = GetBest()))
			return -1;
		if (m_pBest->id == m_iDNum) 
			return 1;
		CreateChildren(m_pBest);
		return 0;
	}

	_Node * KAStar::GetBest() 
	{
		if (!m_pOpen)
		{
			return NULL;
		}

		_Node_AStar * temp = (_Node_AStar *)m_pOpen;
		m_pOpen = temp->next;
		erase_from_open(temp);
		GetPlugin()->AS_Notify_ByList(NULL, temp, ASNL_DELETEOPEN, m_pNCData);

		_Node_AStar* temp2 = (_Node_AStar *)m_pClosed;
		m_pClosed = temp;
		_Node_AStar* pCloseNode = (_Node_AStar *)m_pClosed;
		pCloseNode->next = temp2;
		add_to_close(temp);
		GetPlugin()->AS_Notify_ByList(NULL, m_pClosed, ASNL_ADDCLOSED, m_pNCData);
		return temp;
	}

	void KAStar::CreateChildren(_Node *node) 
	{	PERF_COUNTER(KAStar_CreateChildren);

		_Node_AStar * pNode = (_Node_AStar *)node;
		_Node tempNode[ASTAR_MAX_CHILDREN];
		memset(tempNode, 0, sizeof(tempNode));
		int_r n = GetPlugin()->AS_CreateChild(node, tempNode, ASTAR_MAX_CHILDREN, m_pCBData);
		n = n > ASTAR_MAX_CHILDREN ? ASTAR_MAX_CHILDREN : n;
		for (int_r i = 0; i < n; i++)
		{
			if (0 == GetPlugin()->AS_Valid(node, &tempNode[i], 0, m_pCBData)) continue;
			LinkChild(node, &tempNode[i]);
		}
		/*
		memset(tempNode, 0, sizeof(tempNode));
		n = GetPlugin()->AS_CreateDiagonalChild(node, tempNode, ASTAR_MAX_CHILDREN, m_pCBData);
		n = n > ASTAR_MAX_CHILDREN ? ASTAR_MAX_CHILDREN : n;
		for (int_r i = 0; i < n; i++)
		{
			if (0 == GetPlugin()->AS_Valid(node, &tempNode[i], 0, m_pCBData)) continue;
			LinkChild(node, &tempNode[i]);
		}
		*/
	}

	void KAStar::LinkChild(_Node * pNode, _Node * pTempNode) 
	{	PERF_COUNTER(KAStar_LinkChild);

		_Node_AStar * node = (_Node_AStar *)pNode;
		int_r x = pTempNode->x;
		int_r y = pTempNode->y;
		KPathFinderAbout::VALUE_T g = node->g + GetPlugin()->AS_Cost(node, pTempNode, 0, m_pCBData);
		int_r num = pTempNode->id;

		_Node_AStar * check = (_Node_AStar *)find_from_open(pTempNode->id);
		if (check) 
		{
			check->nStepNum = node->nStepNum + 1;
			node->children[node->numchildren++] = check;

			if (g < check->g) 
			{
				check->parent = node;
				check->g = g;
				check->f = g + check->h;
				GetPlugin()->AS_Notify_ByChild(node, check, 1, m_pNCData);
			} 
			else 
			{
				GetPlugin()->AS_Notify_ByChild(node, check, 2, m_pNCData);
			}
		} 
		else 
		{
			check = (_Node_AStar *)find_from_close(pTempNode->id);
			if (check)
			{
				node->children[node->numchildren++] = check;

				if (g < check->g) 
				{
					check->parent = node;
					check->g = g;
					check->f = g + check->h;
					GetPlugin()->AS_Notify_ByChild(node, check, 3, m_pNCData);
					UpdateParents(check);
				} 
				else 
				{
					GetPlugin()->AS_Notify_ByChild(node, check, 4, m_pNCData);
				}
			}
			else 
			{
				_Node_AStar *newnode = _Node_AStar::Alloc();
				if (newnode == NULL)
				{
					ASSERT(FALSE);
					return;
				}
				new (newnode) _Node_AStar();
				newnode->x = x;
				newnode->y = y;
				newnode->next = NULL;
				newnode->numchildren = 0;
				newnode->parent = node;
				newnode->g = g;
				newnode->nStepNum = node->nStepNum;
				newnode->id = num;
				newnode->h = GetPlugin()->AS_Heuristici(newnode, &m_nodeEnd, 0, m_pCBData);
				newnode->f = newnode->g + newnode->h;
				
				AddToOpen(newnode);
				node->children[node->numchildren++] = newnode;
				GetPlugin()->AS_Notify_ByChild(node, newnode, 5, m_pNCData);
			}
		}
	}

	void KAStar::AddToOpen(_Node * pNode) 
	{	PERF_COUNTER(KAStar_AddToOpen);

		_Node_AStar * addnode = (_Node_AStar *)pNode;
		add_to_open(addnode);
		_Node_AStar * node = (_Node_AStar *)m_pOpen;
		_Node_AStar * prev = NULL;
		if (!m_pOpen) 
		{
			m_pOpen = addnode;
			_Node_AStar * pOpen = (_Node_AStar *)m_pOpen;
			pOpen->next = NULL;
			GetPlugin()->AS_Notify_ByList(NULL, addnode, ASNL_STARTOPEN, m_pNCData);
			return;
		}

		while(node) 
		{
			if (addnode->f > node->f) 
			{
				prev = node;
				node = (_Node_AStar *)node->next;
			} 
			else 
			{
				if (prev) 
				{
					prev->next = addnode;
					addnode->next = node;
					GetPlugin()->AS_Notify_ByList(prev, addnode, ASNL_ADDOPEN, m_pNCData);
				} 
				else 
				{
					_Node_AStar * temp = (_Node_AStar *)m_pOpen;
					m_pOpen = addnode;
					_Node_AStar * pOpen = (_Node_AStar *)m_pOpen;
					pOpen->next = temp;
					GetPlugin()->AS_Notify_ByList(temp, addnode, ASNL_STARTOPEN, m_pNCData);
				}

				return;
			}
		}

		prev->next = addnode;
		GetPlugin()->AS_Notify_ByList(prev, addnode, ASNL_ADDOPEN, m_pNCData);
	}

	void KAStar::UpdateParents(_Node *pNode) 
	{	PERF_COUNTER(KAStar_UpdateParents);
		_Node_AStar * node = (_Node_AStar *)pNode;
		int_r g = node->g;
		int_r c = node->numchildren;

		_Node_AStar *kid = NULL;
		for (int_r i=0;i<c;i++) 
		{
			kid = (_Node_AStar*)node->children[i];
			VALUE_T cost = GetPlugin()->AS_Cost(node, kid, 0, m_pCBData);
			if (g + cost < kid->g)
			{
				kid->g = g + cost;
				kid->f = kid->g + kid->h;
				kid->parent = node;
				Push(kid);
			}
		}

		_Node_AStar *parent;
		while (m_pStack) 
		{
			parent = (_Node_AStar *)Pop();
			c = parent->numchildren;
			for (int_r i=0;i<c;i++) 
			{
				kid = (_Node_AStar *)parent->children[i];
				VALUE_T cost = GetPlugin()->AS_Cost(parent, kid, 0, m_pCBData);
				if (parent->g + cost < kid->g)
				{
					kid->g = parent->g + GetPlugin()->AS_Cost(parent, kid, 0, m_pCBData);
					kid->f = kid->g + kid->h;
					kid->parent = parent;
					Push(kid);
				}
			}
		}
	}

	void KAStar::Push(_Node *node)
	{	PERF_COUNTER(KAStar_Push);

		if (!m_pStack) 
		{
			m_pStack = _asStack::Alloc();
			m_pStack->data = node;
			m_pStack->next = NULL;
		}
		else
		{
			_asStack *temp = _asStack::Alloc();
			temp->data = node;
			temp->next = m_pStack;
			m_pStack = temp;
		}
	}

	_Node * KAStar::Pop()
	{
		_Node * data = m_pStack->data;
		_asStack * temp = m_pStack;
		m_pStack = temp->next;
		temp->data = NULL;
		temp->next = NULL;
		_asStack::Free(temp);
		return data;
	}

	////////////////////////////////////////

	KAStar2::KAStar2():IAlgorithmDynArray<__f_Cmp>()
	{	PERF_COUNTER(KAStar2_KAStar2);

	}

	KAStar2::~KAStar2() 
	{	PERF_COUNTER(KAStar2_D_KAStar2);

		ReleaseNodes();
	}

	void KAStar2::ReleaseNodes()
	{	PERF_COUNTER(KAStar2_ReleaseNodes);

		_Node_AStar::ResetPool();
		IAlgorithmDynArray<__f_Cmp>::ReleaseNodes();
	}

	int_r KAStar2::GeneratePath(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did)
	{	PERF_COUNTER(KAStar2_GeneratePath);

		ReleaseNodes();
		if (sx == dx && sy == dy)
		{
			return 0;
		}
		StepInitialize(sx, sy, dx, dy, sid, did);

		_Node_AStar * pFirst = _Node_AStar::Alloc();
		if (pFirst == NULL)
		{
			ASSERT(FALSE);
		}
		new (pFirst) _Node_AStar();
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

				_Node_AStar * pChild = _Node_AStar::Alloc();
				new (pChild) _Node_AStar(tempNode[i]);
				if (pChild == NULL)
				{
					ASSERT(FALSE);
				}
				pChild->parent = m_pBest;
				pChild->g = GetPlugin()->AS_Cost(m_pBest, pChild, 0, 0);
				pChild->h = GetPlugin()->AS_Heuristici(pChild, &m_nodeEnd, 0, 0);
				pChild->f = pChild->g + pChild->h;
				add_to_open(pChild);
			}

			// create children
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

				_Node_AStar * pChild = _Node_AStar::Alloc();
				new (pChild) _Node_AStar(tempNode[i]);
				if (pChild == NULL)
				{
					ASSERT(FALSE);
				}
				pChild->parent = m_pBest;
				pChild->g = GetPlugin()->AS_Cost(m_pBest, pChild, 0, 0);
				pChild->h = GetPlugin()->AS_Heuristici(pChild, &m_nodeEnd, 0, 0);
				pChild->f = pChild->g + pChild->h;
				add_to_open(pChild);
			}
		}

		return 0;
	}

	void KAStar2::StepInitialize(int_r sx, int_r sy, int_r dx, int_r dy, int_r &sid, int_r &did)
	{	PERF_COUNTER(KAStar2_StepInitialize);

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