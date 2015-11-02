

#include <math.h>
#include "PathFinder.h"

#if !defined(max)
#	define max(a, b) ((a) > (b) ? (a) : (b))
#	define min(a, b) ((a) < (b) ? (a) : (b))
#endif

namespace DataStructureAbout
{
	CAStar::CAStar() 
	{
		m_pClosed = NULL;
		m_pStack = NULL;
		m_pBest = NULL;

		udCost = NULL;
		udValid = NULL;
		udNotifyChild = NULL;
		udNotifyList = NULL;
		m_maxLoop = 0x7fffffff;
		udCreateChild = NULL;
		udHeuristici = NULL;

		m_pOpen = NULL;

#ifdef USING_OWN_MEMORY_POOL
		m_nodePool.Create(10000);
#endif

		m_openTab.SetCapacity(10000);
		m_closeTab.SetCapacity(10000);

#ifdef _DEBUG_DETAIL
		testSearchInOpenSuccCount = 0;
		testSearchInOpenFailCount = 0;
		testAddInOpenSearchCount = 0;
		testAddToOpenCallCount = 0;
		testSearchInCloseSuccCount = 0;
		testSearchInCloseFailCount = 0;
		testUpdateParentCallCount = 0;
#endif
	}

	CAStar::~CAStar() 
	{
		ClearNodes();
	}

	void CAStar::ClearNodes() 
	{
		_asNode *temp = NULL, *temp2 = NULL;

#if defined(USING_OWN_MEMORY_POOL)
		m_nodePool.Clear();
		m_stackNodePool.Clear();
		m_openTab.Clear();
		if (m_pOpen){
			m_pOpen = NULL;
		}
#else
		if (m_pOpen) 
		{
			while (m_pOpen) 

			{
				temp = m_pOpen->next;

				delete m_pOpen;

				m_pOpen = temp;
			}
			m_openTab.Clear();
		}
#endif // USING_OWN_MEMORY_POOL

#if defined(USING_OWN_MEMORY_POOL)
		m_closeTab.Clear();
		if (m_pClosed){
			m_pClosed = NULL;
		}
#else
		if (m_pClosed) 
		{
			while (m_pClosed) 
			{
				temp = m_pClosed->next;

				delete m_pClosed;

				m_pClosed = temp;
			}
			m_closeTab.Clear();
		}
#endif
	}

	int	CAStar::GetBestNodeStepNum()
	{
		int nRet = 0;

		int nX = 0;
		int nY = 0;

		if (m_pBest)
		{
			nX = abs(m_pBest->x - m_nodeStart.x);
			nY = abs(m_pBest->y - m_nodeStart.y);

			nRet = nX + nY;
		}

		return nRet;
	}



	int CAStar::GeneratePath(int sx, int sy, int dx, int dy, int nMaxStep, int sid /*= 0*/, int did /*= 0*/)
	{
		if (sx == dx && sy == dy)
		{
			return 0;
		}
		StepInitialize(sx, sy, dx, dy, sid, did);

		// Add step count to stop searching

		m_pBest = NULL;

		int nStepCount = 0;
		int retval = 0;

		int nBestNodeStepNum =0;
		int nRealBestNodeStepNum =0;
		while (retval == 0) 
		{
	
			nStepCount++;

			if ( m_pBest )
			{
				//nBestNodeStepNum = GetBestNodeStepNum();
				nBestNodeStepNum = m_pBest->nStepNum;
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
		//if (retval == -1 || !m_pBest) 
		{
			m_pBest = NULL;
			return -1;
		}

		return retval;
	}

	void CAStar::StepInitialize(int sx, int sy, int dx, int dy, int sid, int did)
	{
		ClearNodes();

		m_nodeStart.x = m_iSX = sx;
		m_nodeStart.y = m_iSY = sy;

		m_nodeEnd.x = m_iDX = dx;
		m_nodeEnd.y = m_iDY = dy;

		if (sid == 0)
		{
			sid = Coord2ID(sx, sy);
		}
		if (did == 0)
		{
			did = Coord2ID(dx, dy);
		}


		m_nodeEnd.number =  m_iDNum = did;


#if defined(USING_OWN_MEMORY_POOL)
		_asNode *temp = m_nodePool.New();
#else
		_asNode *temp = new _asNode(sx, sy);
#endif
		if (temp == NULL)
		{
			// 内存满
			assert(FALSE);
			return;
		}
		temp->Init();
		temp->x = sx;
		temp->y = sy;
		temp->next = NULL;
		temp->numchildren = 0;


		temp->g = 0;
		//temp->h = abs(dx-sx) + abs(dy-sy);
		temp->h = udFunc(udHeuristici, &m_nodeStart, &m_nodeEnd, 0, m_pCBData);

		temp->f = temp->g + temp->h;

		m_nodeStart.number =  temp->number = sid;

		AddToOpen(temp);

		udFunc(udNotifyList, NULL, m_pOpen, ASNL_STARTOPEN, m_pNCData);
		udFunc(udNotifyChild, NULL, temp, 0, m_pNCData);
	}

	int CAStar::Step()
	{
		if (!(m_pBest = GetBest()))
			return -1;

		if (m_pBest->number == m_iDNum) 
			return 1;

		CreateChildren(m_pBest);

		return 0;
	}

	_asNode *CAStar::GetBest() 
	{
		_asNode* temp;
		_asNode* temp2;
		if (!m_pOpen) return NULL;
		temp = m_pOpen;
		m_pOpen = temp->next;

		m_openTab.Delete(temp->number);

		udFunc(udNotifyList, NULL, temp, ASNL_DELETEOPEN, m_pNCData);

		temp2 = m_pClosed;
		m_pClosed = temp;
		m_pClosed->next = temp2;
		m_closeTab.Insert(temp->number, temp);

		udFunc(udNotifyList, NULL, m_pClosed, ASNL_ADDCLOSED, m_pNCData);

		return temp;
	}

	void CAStar::CreateChildren(_asNode *node) 
	{
		_asNode temp;
		int x = node->x, y = node->y;
		// 每个下级子结点的步长加一
		temp.nStepNum = node->nStepNum+1;

		if (!udCreateChild)
		{
			static const int dx[] = {-1, 0, 1, 0, 1, 1, -1, -1};
			static const int dy[] = {0, 1, 0, -1, 1, -1, 1, -1};
			// 使用8向可达
			for (int i = 0; i < 8; i++){
				temp.x = x+dx[i];
				temp.y = y+dy[i];
				temp.number = Coord2ID(temp.x, temp.y);
				if (0 == udFunc(udValid, node, &temp, 0, m_pCBData)) continue;
				LinkChild(node, &temp);
			}
		}
		else
		{
			_asNode tempNode[ASTAR_MAX_CHILDREN];
			int n = min(udCreateChild(node, tempNode, m_pCBData), ASTAR_MAX_CHILDREN);
			for (int i = 0; i < n; i++)
			{
				if (0 == udFunc(udValid, node, &tempNode[i], 0, m_pCBData)) continue;
				LinkChild(node, &tempNode[i]);
			}
		}
	}

	void CAStar::LinkChild(_asNode *node, _asNode *temp) 
	{
		int x = temp->x;
		int y = temp->y;
		VALUE_TYPE g = node->g + udFunc(udCost, node, temp, 0, m_pCBData);
		int num = temp->number;

		_asNode *check = NULL;

		if (check = CheckList(true, num)) 
		{
			check->nStepNum = temp->nStepNum;

			node->children[node->numchildren++] = check;


			if (g < check->g) 
			{
				check->parent = node;
				check->g = g;
				check->f = g + check->h;
				udFunc(udNotifyChild, node, check, 1, m_pNCData);
			} 
			else 
			{
				udFunc(udNotifyChild, node, check, 2, m_pNCData);
			}
		} 
		else if (check = CheckList(false, num)) 
		{
			//check->nStepNum = temp->nStepNum;
			node->children[node->numchildren++] = check;

			if (g < check->g) 
			{
				check->parent = node;
				check->g = g;
				check->f = g + check->h;

				udFunc(udNotifyChild, node, check, 3, m_pNCData);


				UpdateParents(check);
			} 
			else 
			{
				udFunc(udNotifyChild, node, check, 4, m_pNCData);
			}
		} 
		else 
		{
#if defined(USING_OWN_MEMORY_POOL)
			_asNode *newnode = m_nodePool.New();
#else
			_asNode *newnode = new _asNode(x,y);
#endif
			if (newnode == NULL)
			{
				//内存满
				return;
			}
			newnode->Init();
			newnode->x = x;
			newnode->y = y;
			newnode->next = NULL;
			newnode->numchildren = 0;

			newnode->parent = node;
			newnode->g = g;
			newnode->nStepNum = temp->nStepNum;
			//newnode->h = abs(x-m_iDX) + abs(y-m_iDY);
			newnode->h = udFunc(udHeuristici, newnode, &m_nodeEnd, 0, m_pCBData);

			newnode->f = newnode->g + newnode->h;
			newnode->number = num;

			AddToOpen(newnode);

			node->children[node->numchildren++] = newnode;

			udFunc(udNotifyChild, node, newnode, 5, m_pNCData);
		}
	}

	_asNode *CAStar::CheckList(bool bOpen, int num) 
	{
		if (bOpen){
			_asNode* pResult = m_openTab.Find(num);

#ifdef _DEBUG_DETAIL
			if (pResult){
				testSearchInOpenSuccCount++;
			}else{
				testSearchInOpenFailCount++;
			}
#endif
			return pResult;
		}else{
			_asNode* pResult = m_closeTab.Find(num);
#ifdef _DEBUG_DETAIL
			if (pResult){
				testSearchInCloseSuccCount++;
			}else{
				testSearchInCloseFailCount++;
			}
#endif
			return pResult;
		}


#ifdef _DEBUG_DETAIL
		if (bOpen){
			testSearchInOpenFailCount++;
		}else{
			testSearchInCloseFailCount++;
		}
#endif
		return NULL;
	}

	void CAStar::AddToOpen(_asNode *addnode) 
	{
#ifdef _DEBUG_DETAIL
		testAddToOpenCallCount++;
#endif

		m_openTab.Insert(addnode->number, addnode);

		_asNode *node = m_pOpen;
		_asNode *prev = NULL;

		if (!m_pOpen) 
		{
			m_pOpen = addnode;
			m_pOpen->next = NULL;

			udFunc(udNotifyList, NULL, addnode, ASNL_STARTOPEN, m_pNCData);

			return;
		}

		while(node) 
		{
#ifdef _DEBUG_DETAIL
			testAddInOpenSearchCount++;
#endif
			if (addnode->f > node->f) 
			{
				prev = node;
				node = node->next;
			} 
			else 
			{
				if (prev) 
				{
					prev->next = addnode;
					addnode->next = node;
					udFunc(udNotifyList, prev, addnode, ASNL_ADDOPEN, m_pNCData);
				} 
				else 
				{
					_asNode *temp = m_pOpen;

					m_pOpen = addnode;
					m_pOpen->next = temp;
					udFunc(udNotifyList, temp, addnode, ASNL_STARTOPEN, m_pNCData);
				}

				return;
			}
		}

		prev->next = addnode;
		udFunc(udNotifyList, prev, addnode, ASNL_ADDOPEN, m_pNCData);
	}

	void CAStar::UpdateParents(_asNode *node) 
	{
#ifdef _DEBUG_DETAIL
		testUpdateParentCallCount++;
#endif // _DEBUG_DETAIL
		VALUE_TYPE g = node->g;
		int c = node->numchildren;

		_asNode *kid = NULL;
		for (int i=0;i<c;i++) 
		{
			kid = node->children[i];
			VALUE_TYPE cost = udFunc(udCost, node, kid, 0, m_pCBData);
			if (g + cost < kid->g)
			{
				kid->g = g + cost;
				kid->f = kid->g + kid->h;
				kid->parent = node;

				Push(kid);
			}
		}

		_asNode *parent;
		while (m_pStack) 
		{
			parent = Pop();
			c = parent->numchildren;
			for (int i=0;i<c;i++) 
			{
				kid = parent->children[i];
				VALUE_TYPE cost = udFunc(udCost, parent, kid, 0, m_pCBData);

				if (parent->g + cost < kid->g)
				{
					kid->g = parent->g + udFunc(udCost, parent, kid, 0, m_pCBData);
					kid->f = kid->g + kid->h;
					kid->parent = parent;
					Push(kid);
				}
			}
		}
	}

	void CAStar::Push(_asNode *node)
	{

		if (!m_pStack) 
		{
#ifdef USING_OWN_MEMORY_POOL
			m_pStack = m_stackNodePool.New();
#else
			m_pStack = new _asStack;
#endif
			m_pStack->data = node;
			m_pStack->next = NULL;
		} else {
#ifdef USING_OWN_MEMORY_POOL
			_asStack *temp = m_stackNodePool.New();
#else
			_asStack *temp = new _asStack;
#endif
			temp->data = node;
			temp->next = m_pStack;
			m_pStack = temp;
		}
	}

	_asNode *CAStar::Pop() 
	{
		_asNode *data = m_pStack->data;
		_asStack *temp = m_pStack;

		m_pStack = temp->next;

#ifdef USING_OWN_MEMORY_POOL
		m_stackNodePool.Delete(temp);
#else
		delete temp;
#endif

		return data;
	}

	VALUE_TYPE CAStar::udFunc(_asFunc func, _asNode *param1, _asNode *param2, int data, void *cb)
	{
		if (func) return func(param1, param2, data, cb);

		return 1;
	}
} // end namespace DataStructureAbout
