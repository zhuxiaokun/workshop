#include "BStar.h"
#include "CommonUtils.h"
#include "../PerfHitch/perf_PathFinderLib.h"

namespace KPathFinderAbout
{
	KBStarIndependence::KBStarIndependence():IAlgorithmDynArray<__B_Cmp>()
	{	PERF_COUNTER(KBStarIndependence_KBStarIndependence);

	}

	KBStarIndependence::~KBStarIndependence() 
	{	PERF_COUNTER(KBStarIndependence_D_KBStarIndependence);

		ReleaseNodes();
	}

	void KBStarIndependence::ReleaseNodes()
	{	PERF_COUNTER(KBStarIndependence_ReleaseNodes);

		mBackTable.clear();
		mOpenTable.clear();
		_Node_BStar::ResetPool();
		IAlgorithmDynArray<__B_Cmp>::ReleaseNodes();
	}

	int_r KBStarIndependence::GeneratePath(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did)
	{	PERF_COUNTER(KBStarIndependence_GeneratePath);

		ReleaseNodes();
		return GeneratePath_NoRelease(sx, sy, dx, dy, nMaxStep, sid, did);
	}

	bool KBStarIndependence::_GoStraight(_Node_BStar * pCurrentNode, _Node_BStar &nextNode)
	{	PERF_COUNTER(KBStarIndependence__GoStraight);

		// 找到目标方向 create children
		int nextX = 0;
		int nextY = 0;
		int_r nTempNextX = 0;
		int_r nTempNextY = 0;
		int_r nNodeID = 0;
		int_r nTempDir = cn_angle64;
		int_r minDir = cn_angle64;
		for (int loop_dir = 0; loop_dir < MAX_DIRECTION; loop_dir++)
		{
			nTempNextX = pCurrentNode->x + sgcAround[loop_dir].x;
			nTempNextY = pCurrentNode->y + sgcAround[loop_dir].y;
			nNodeID = GetPlugin()->Coord2ID(nTempNextX, nTempNextY);

			if(find_from_close(nNodeID))
				continue;
			if(find_from_open(nNodeID))
				continue;

			nTempDir = g_GetDirDiff(pCurrentNode->x, pCurrentNode->y, m_nodeEnd.x, m_nodeEnd.y, nTempNextX, nTempNextY);
			if( nTempDir > cn_angle8 )
				continue;

			if (nTempDir < minDir)
			{
				minDir = nTempDir;

				nextNode.x = nTempNextX;
				nextNode.y = nTempNextY;
				nextNode.id = nNodeID;
				nextNode.dir = (_BStar_Direct)loop_dir;

				if(0 == minDir)
					break;
			}
		}
		ASSERT(nextNode.id >= 0);

		if(0 != GetPlugin()->AS_Valid(pCurrentNode, &nextNode, 0, 0))
		{
			_Node_BStar * pChild = _Node_BStar::Alloc();
			new (pChild) _Node_BStar(nextNode);
			if (pChild == NULL)
			{
				ASSERT(FALSE);
			}
			pChild->parent = pCurrentNode;
			pChild->g = pCurrentNode->g + GetPlugin()->AS_Cost(pCurrentNode, pChild, 0, 0);
			pChild->f = pChild->g + pChild->h;
			mBackTable.push_back(pChild);
			//Log(LOG_FATAL, "freenode %d %d create child %d %d dir %d %s", pCurrentNode->x, pCurrentNode->y, pChild->x, pChild->y, pChild->dir, DirToString(pChild->dir));
			return true;
		}
		else
		{
			//Log(LOG_FATAL, "block! %d %d dir %d %s", nextNode.x, nextNode.y, nextNode.dir, DirToString(nextNode.dir));
			return false;
		}
	}

	void KBStarIndependence::_TurnLeft(_Node_BStar * pCurrentNode, _Node_BStar blockNode)
	{	PERF_COUNTER(KBStarIndependence__TurnLeft);

		// 碰到阻挡，分出两个叉
		_Node_BStar left;
		_Node_BStar * pNextNode = NULL;

		// 左分支
		pNextNode = &left;
		bool cancel = true;
		int testdir = 0;
		for (testdir = 0; testdir < MAX_DIRECTION; testdir++)
		{
			if (sgcBranchAround[_Node_BStar::branch_dir_left][testdir] == blockNode.dir)
			{
				break;
			}
		}
		int count = 0;
		for (testdir = testdir + 1, count = 0; count < MAX_DIRECTION - 1; testdir++, count++)
		{
			if (testdir >= MAX_DIRECTION)
				testdir = 0;
			_BStar_Direct nTempDirect = sgcBranchAround[_Node_BStar::branch_dir_left][testdir];
			pNextNode->x = pCurrentNode->x + sgcAround[nTempDirect].x;
			pNextNode->y = pCurrentNode->y + sgcAround[nTempDirect].y;
			pNextNode->id = GetPlugin()->Coord2ID(pNextNode->x, pNextNode->y);
			if (0 == GetPlugin()->AS_Valid(pCurrentNode, pNextNode, 0, 0))
			{
				continue;
			}
			else
			{
				cancel = false;
				break;
			}
		}
		if (!cancel)
		{
			bool bNeedAlloc = true;
			pNextNode->branch = _Node_BStar::branch_dir_left;
			pNextNode->dir = sgcBranchAround[_Node_BStar::branch_dir_left][testdir];
			pNextNode->g = pCurrentNode->g + GetPlugin()->AS_Cost(pCurrentNode, pNextNode, 0, 0);

			if(_Node_BStar * pClose = (_Node_BStar*)find_from_close(pNextNode->id))
			{
				ASSERT((pClose->id == m_nodeStart.id) == (NULL == pClose->parent));
				if (pClose->branch == _Node_BStar::branch_dir_right && (pCurrentNode->parent && pClose->id == pCurrentNode->parent->id))
				{
					cancel = true;
				}
				else if (pClose->g > pNextNode->g)
				{
					pNextNode->parent = pClose->parent;
					new (pClose) _Node_BStar(*pNextNode);
					pNextNode = pClose;
					erase_from_close(pClose);
					pNextNode->parent = pCurrentNode;
					bNeedAlloc = false;
					ASSERT((pClose->id == m_nodeStart.id) == (NULL == pClose->parent));
				}
				else
				{
					pNextNode->parent = pClose->parent;
					new (pClose) _Node_BStar(*pNextNode);
					pNextNode = pClose;
					erase_from_close(pClose);
					bNeedAlloc = false;
					ASSERT((pClose->id == m_nodeStart.id) == (NULL == pClose->parent));
				}
			}
			else if(_Node_BStar * pOpen = (_Node_BStar*)find_from_open(pNextNode->id))
			{
				ASSERT((pOpen->id == m_nodeStart.id) == (NULL == pOpen->parent));
				if (pOpen->g > pNextNode->g)
				{
					new (pOpen) _Node_BStar(pNextNode);
					pNextNode = pOpen;
					pOpen->parent = pCurrentNode;
					bNeedAlloc = false;
				}
				else
				{
					pNextNode->parent = pOpen->parent;
					new (pOpen) _Node_BStar(pNextNode);
					pNextNode = pOpen;
					bNeedAlloc = false;
				}
			}
			else
			{
				pNextNode->parent = pCurrentNode;
			}

			if (!cancel)
			{
				pNextNode->real = count + 1;
				int angle = g_GetDirAngle(m_nodeStart.x, m_nodeStart.y, pNextNode->x, pNextNode->y, blockNode.x, blockNode.y);
				angle = angle > cn_angle64_2 ? (cn_angle64 - angle) : -angle;
				pNextNode->angle = angle;

				_Node_BStar * pTemp = NULL;
				if(bNeedAlloc)
				{
					pTemp = _Node_BStar::Alloc();
					new (pTemp) _Node_BStar(*pNextNode);
				}
				else
				{
					pTemp = pNextNode;
				}
				ASSERT(pTemp && (pTemp->id == m_nodeStart.id) == (NULL == pTemp->parent));
				mBackTable.push_back(pTemp);
				//Log(LOG_FATAL, "the left %d %d dir %d %s", pTemp->x, pTemp->y, pTemp->dir, DirToString(pTemp->dir));
			}
		}
	}

	void KBStarIndependence::_TurnRight(_Node_BStar * pCurrentNode, _Node_BStar blockNode)
	{	PERF_COUNTER(KBStarIndependence__TurnRight);

		_Node_BStar right;
		_Node_BStar * pNextNode = NULL;

		// 右分支
		pNextNode = &right;
		bool cancel = true;
		int_r testdir = 0;
		for (testdir = 0; testdir < MAX_DIRECTION; testdir++)
		{
			if (sgcBranchAround[_Node_BStar::branch_dir_right][testdir] == blockNode.dir)
			{
				break;
			}
		}
		int_r count = 0;
		for (testdir = testdir + 1, count = 0; count < MAX_DIRECTION - 1; testdir++, count++)
		{
			if (testdir >= MAX_DIRECTION)
				testdir = 0;
			_BStar_Direct nTempDirect = sgcBranchAround[_Node_BStar::branch_dir_right][testdir];
			pNextNode->x = pCurrentNode->x + sgcAround[nTempDirect].x;
			pNextNode->y = pCurrentNode->y + sgcAround[nTempDirect].y;
			pNextNode->id = GetPlugin()->Coord2ID(pNextNode->x, pNextNode->y);
			if (0 == GetPlugin()->AS_Valid(pCurrentNode, pNextNode, 0, 0))
			{
				continue;
			}
			else
			{
				cancel = false;
				break;
			}
		}
		if (!cancel)
		{
			bool bNeedAlloc = true;
			pNextNode->branch = _Node_BStar::branch_dir_right;
			pNextNode->dir = sgcBranchAround[_Node_BStar::branch_dir_right][testdir];
			pNextNode->g = pCurrentNode->g + GetPlugin()->AS_Cost(pCurrentNode, pNextNode, 0, 0);

			if(_Node_BStar * pClose = (_Node_BStar*)find_from_close(pNextNode->id))
			{
				ASSERT((pClose->id == m_nodeStart.id) == (NULL == pClose->parent));
				if (pClose->branch == _Node_BStar::branch_dir_left && (pCurrentNode->parent && pClose->id == pCurrentNode->parent->id))
				{
					cancel = true;
				}
				else if (pClose->g > pNextNode->g)
				{
					pNextNode->parent = pClose->parent;
					new (pClose) _Node_BStar(*pNextNode);
					pNextNode = pClose;
					erase_from_close(pClose);
					pNextNode->parent = pCurrentNode;
					bNeedAlloc = false;
					ASSERT((pClose->id == m_nodeStart.id) == (NULL == pClose->parent));
				}
				else
				{
					pNextNode->parent = pClose->parent;
					new (pClose) _Node_BStar(*pNextNode);
					pNextNode = pClose;
					erase_from_close(pClose);
					bNeedAlloc = false;
					ASSERT((pClose->id == m_nodeStart.id) == (NULL == pClose->parent));
				}
			}
			else if(_Node_BStar * pOpen = (_Node_BStar*)find_from_open(pNextNode->id))
			{
				ASSERT((pOpen->id == m_nodeStart.id) == (NULL == pOpen->parent));
				if (pOpen->g > pNextNode->g)
				{
					new (pOpen) _Node_BStar(*pNextNode);
					pNextNode = pOpen;
					pOpen->parent = pCurrentNode;
					bNeedAlloc = false;
				}
				else
				{
					pNextNode->parent = pOpen->parent;
					new (pOpen) _Node_BStar(*pNextNode);
					pNextNode = pOpen;
					bNeedAlloc = false;
				}
			}
			else
			{
				pNextNode->parent = pCurrentNode;
			}

			if (!cancel)
			{
				pNextNode->real = count + 1;
				int angle = g_GetDirAngle(m_nodeStart.x, m_nodeStart.y, pNextNode->x, pNextNode->y, blockNode.x, blockNode.y);
				angle = angle > cn_angle64_2 ? (angle - cn_angle64) : angle;
				pNextNode->angle = angle;

				_Node_BStar * pTemp = NULL;
				if(bNeedAlloc)
				{
					pTemp = _Node_BStar::Alloc();
					new (pTemp) _Node_BStar(*pNextNode);
				}
				else
				{
					pTemp = pNextNode;
				}
				ASSERT(pTemp && (pTemp->id == m_nodeStart.id) == (NULL == pTemp->parent));
				mBackTable.push_back(pTemp);
				//Log(LOG_FATAL, "the right %d %d dir %d %s", pTemp->x, pTemp->y, pTemp->dir, DirToString(pTemp->dir));
			}
		}
	}

	void KBStarIndependence::_ClimbRight(_Node_BStar * pCurrentNode)
	{	PERF_COUNTER(KBStarIndependence__ClimbRight);

		ASSERT(pCurrentNode && _Node_BStar::branch_dir_right == pCurrentNode->branch);
		_Node_BStar nextCell;
		_Node_BStar * pNextCell = NULL;

		pNextCell = &nextCell;
		int dir = eBD_Unknow;
		for (dir = eBD_Bottom; dir < MAX_DIRECTION; dir++)
		{
			if (sgcBranchAround[_Node_BStar::branch_dir_right][dir] == pCurrentNode->dir)
			{
				break;
			}
		}
		dir += (MAX_DIRECTION - ONE_DIRECTION);
		if (dir >= MAX_DIRECTION) dir -= MAX_DIRECTION;
		for (int count = 0; count < MAX_DIRECTION; count++,dir++)
		{
			if (dir >= MAX_DIRECTION) dir -= MAX_DIRECTION;
			_BStar_Direct nextdir = sgcBranchAround[_Node_BStar::branch_dir_right][dir];
			pNextCell->x = pCurrentNode->x + sgcAround[nextdir].x;
			pNextCell->y = pCurrentNode->y + sgcAround[nextdir].y;
			pNextCell->id = GetPlugin()->Coord2ID(pNextCell->x, pNextCell->y);
			pNextCell->g = pCurrentNode->g + GetPlugin()->AS_Cost(pCurrentNode, pNextCell, 0, 0);

			if (0 == GetPlugin()->AS_Valid(pCurrentNode, pNextCell, 0, 0))
			{
				continue;
			}
			else
			{
				// is target
				bool bNeedAlloc = true;
				if(pNextCell->id == m_nodeEnd.id)
				{
					m_pBest = _Node_BStar::Alloc();
					new (m_pBest) _Node_BStar(nextCell);
					if (m_pBest == NULL)
					{
						ASSERT(FALSE);
					}
					m_pBest->parent = pCurrentNode;
					m_pBest->f = m_pBest->g + m_pBest->h;
					return;
				}
				else if(_Node_BStar * pClose = (_Node_BStar*)find_from_close(pNextCell->id))
				{
					ASSERT((pClose->id == m_nodeStart.id) == (NULL == pClose->parent));
					if (pClose->branch == _Node_BStar::branch_dir_left && (pCurrentNode->parent && pClose->id == pCurrentNode->parent->id)
						&& pClose->dir != nextdir)
					{
						break;
					}
					else if (pClose->branch == _Node_BStar::branch_dir_right && pClose->dir == nextdir)
					{
						break;
					}
					else if (pClose->branch == _Node_BStar::branch_dir_none && pClose->g > pCurrentNode->g)
					{
						break;
					}
					else if (pClose->g > pNextCell->g)
					{
						pNextCell->parent = pClose->parent;
						new (pClose) _Node_BStar(*pNextCell);
						pNextCell = pClose;
						erase_from_close(pClose);
						pNextCell->parent = pCurrentNode;
						bNeedAlloc = false;
						ASSERT((pClose->id == m_nodeStart.id) == (NULL == pClose->parent));
					}
					else
					{
						pNextCell->parent = pClose->parent;
						new (pClose) _Node_BStar(*pNextCell);
						pNextCell = pClose;
						erase_from_close(pClose);
						bNeedAlloc = false;
						ASSERT((pClose->id == m_nodeStart.id) == (NULL == pClose->parent));
					}
				}
				else if(_Node_BStar * pOpen = (_Node_BStar*)find_from_open(pNextCell->id))
				{
					ASSERT((pOpen->id == m_nodeStart.id) == (NULL == pOpen->parent));
					if (pOpen->branch == _Node_BStar::branch_dir_right && pOpen->dir == nextdir)
					{
						break;
					}
					else if (pOpen->g > pNextCell->g)
					{
						new (pOpen) _Node_BStar(*pNextCell);
						pNextCell = pOpen;
						pNextCell->parent = pCurrentNode;
						bNeedAlloc = false;
					}
					else
					{
						pNextCell->parent = pOpen->parent;
						new (pOpen) _Node_BStar(*pNextCell);
						pNextCell = pOpen;
						bNeedAlloc = false;
					}
				}
				else
				{
					pNextCell->parent = pCurrentNode;
				}

				int reelAdd = count - ONE_DIRECTION;
				pNextCell->real = kmax(0, pCurrentNode->real + reelAdd);

				int angle = g_GetDirAngle(m_nodeStart.x, m_nodeStart.y, pNextCell->x, pNextCell->y, pCurrentNode->x, pCurrentNode->y);
				angle = angle > cn_angle64_2 ? (angle - cn_angle64) : angle;
				pNextCell->angle = pCurrentNode->angle + angle;
				if (pNextCell->angle >= cn_angle64)
				{
					pNextCell->angle -= cn_angle64;
					pNextCell->real -= MAX_DIRECTION;
				}

				if (pNextCell->real > 0)
				{
					pNextCell->branch = _Node_BStar::branch_dir_right;
					pNextCell->dir = nextdir;
				}
				else
				{
					pNextCell->branch = _Node_BStar::branch_dir_none;
					pNextCell->dir = eBD_Unknow;
					pNextCell->angle = 0;
				}

				_Node_BStar * pTemp = NULL;
				if(bNeedAlloc)
				{
					pTemp = _Node_BStar::Alloc();
					new (pTemp) _Node_BStar(*pNextCell);
				}
				else
				{
					pTemp = pNextCell;
				}
				ASSERT(pTemp && (pTemp->id == m_nodeStart.id) == (NULL == pTemp->parent));
				mBackTable.push_back(pTemp);
				//Log(LOG_FATAL, "the right node %d %d create child %d %d dir %d %s 卷曲%d", pCurrentNode->x, pCurrentNode->y, pNextCell->x, pNextCell->y, pTemp->dir, DirToString(pTemp->dir), pTemp->real);
				break;
			}
		}
	}

	void KBStarIndependence::_ClimbLeft(_Node_BStar * pCurrentNode)
	{	PERF_COUNTER(KBStarIndependence__ClimbLeft);

		ASSERT(pCurrentNode && _Node_BStar::branch_dir_left == pCurrentNode->branch);
		_Node_BStar nextCell;
		_Node_BStar * pNextCell = NULL;

		//左分支，靠右攀爬
		pNextCell = &nextCell;
		int dir = eBD_Unknow;
		for (dir = eBD_Bottom; dir < MAX_DIRECTION; dir++)
		{
			if (sgcBranchAround[_Node_BStar::branch_dir_left][dir] == pCurrentNode->dir)
			{
				break;
			}
		}
		dir += (MAX_DIRECTION - ONE_DIRECTION);
		if (dir >= MAX_DIRECTION) dir -= MAX_DIRECTION;
		for (int count = 0; count < MAX_DIRECTION; count++,dir++)
		{
			if (dir >= MAX_DIRECTION) dir -= MAX_DIRECTION;
			_BStar_Direct nextdir = sgcBranchAround[_Node_BStar::branch_dir_left][dir];
			pNextCell->x = pCurrentNode->x + sgcAround[nextdir].x;
			pNextCell->y = pCurrentNode->y + sgcAround[nextdir].y;
			pNextCell->id = GetPlugin()->Coord2ID(pNextCell->x, pNextCell->y);
			pNextCell->g = pCurrentNode->g + GetPlugin()->AS_Cost(pCurrentNode, pNextCell, 0, 0);

			if (0 == GetPlugin()->AS_Valid(pCurrentNode, pNextCell, 0, 0))
			{
				continue;
			}
			else
			{
				// is target
				bool bNeedAlloc = true;
				if(pNextCell->id == m_nodeEnd.id)
				{
					m_pBest = _Node_BStar::Alloc();
					new (m_pBest) _Node_BStar(nextCell);
					if (m_pBest == NULL)
					{
						ASSERT(FALSE);
					}
					m_pBest->parent = pCurrentNode;
					m_pBest->f = m_pBest->g + m_pBest->h;
					return;
				}
				else if(_Node_BStar * pClose = (_Node_BStar*)find_from_close(pNextCell->id))
				{
					ASSERT((pClose->id == m_nodeStart.id) == (NULL == pClose->parent));
					if (pClose->branch == _Node_BStar::branch_dir_right && (pCurrentNode->parent && pClose->id == pCurrentNode->parent->id)
						&& pClose->dir != nextdir)
					{
						break;
					}
					else if (pClose->branch == _Node_BStar::branch_dir_left && pClose->dir == nextdir)
					{
						break;
					}
					else if (pClose->branch == _Node_BStar::branch_dir_none && pClose->g > pCurrentNode->g)
					{
						break;
					}
					else if (pClose->g > pNextCell->g)
					{
						pNextCell->parent = pClose->parent;
						new (pClose) _Node_BStar(*pNextCell);
						pNextCell = pClose;
						erase_from_close(pClose);
						pNextCell->parent = pCurrentNode;
						bNeedAlloc = false;
						ASSERT((pClose->id == m_nodeStart.id) == (NULL == pClose->parent));
					}
					else
					{
						pNextCell->parent = pClose->parent;
						new (pClose) _Node_BStar(*pNextCell);
						pNextCell = pClose;
						erase_from_close(pClose);
						bNeedAlloc = false;
						ASSERT((pClose->id == m_nodeStart.id) == (NULL == pClose->parent));
					}
				}
				else if(_Node_BStar * pOpen = (_Node_BStar*)find_from_open(pNextCell->id))
				{
					ASSERT((pOpen->id == m_nodeStart.id) == (NULL == pOpen->parent));
					if (pOpen->branch == _Node_BStar::branch_dir_left && pOpen->dir == nextdir)
					{
						break;
					}
					else if (pOpen->g > pNextCell->g)
					{
						new (pOpen) _Node_BStar(*pNextCell);
						pNextCell = pOpen;
						pNextCell->parent = pCurrentNode;
						bNeedAlloc = false;
					}
					else
					{
						pNextCell->parent = pOpen->parent;
						new (pOpen) _Node_BStar(*pNextCell);
						pNextCell = pOpen;
						bNeedAlloc = false;
					}
				}
				else
				{
					pNextCell->parent = pCurrentNode;
				}

				int reelAdd = count - ONE_DIRECTION;
				pNextCell->real = kmax(0, pCurrentNode->real + reelAdd);
				int angle = g_GetDirAngle(m_nodeStart.x, m_nodeStart.y, pNextCell->x, pNextCell->y, pCurrentNode->x, pCurrentNode->y);
				angle = angle > cn_angle64_2 ? (cn_angle64 - angle) : -angle;
				pNextCell->angle = pCurrentNode->angle + angle;
				if (pNextCell->angle >= cn_angle64)
				{
					pNextCell->angle -= cn_angle64;
					pNextCell->real -= MAX_DIRECTION;
				}

				if (pNextCell->real > 0)
				{
					pNextCell->branch = _Node_BStar::branch_dir_left;
					pNextCell->dir = nextdir;
				}
				else
				{
					pNextCell->branch = _Node_BStar::branch_dir_none;
					pNextCell->dir = eBD_Unknow;
					pNextCell->angle = 0;
				}

				_Node_BStar * pTemp = NULL;
				if(bNeedAlloc)
				{
					pTemp = _Node_BStar::Alloc();
					new (pTemp) _Node_BStar(*pNextCell);
				}
				else
				{
					pTemp = pNextCell;
				}
				ASSERT(pTemp && (pTemp->id == m_nodeStart.id) == (NULL == pTemp->parent));
				mBackTable.push_back(pTemp);
				//Log(LOG_FATAL, "the left node %d %d create child %d %d dir %d %s 卷曲%d", pCurrentNode->x, pCurrentNode->y, pNextCell->x, pNextCell->y, pTemp->dir, DirToString(pTemp->dir), pTemp->real);
				break;
			}
		}
	}

	int_r KBStarIndependence::GeneratePath_NoRelease(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did)
	{	PERF_COUNTER(KBStarIndependence_GeneratePath_NoRelease);

		//Log(LOG_FATAL, "");
		//Log(LOG_FATAL, "start B* find %d %d to %d %d", sx, sy, dx, dy);
		//Log(LOG_FATAL, "");
		unsigned long ulT1 = GetTickCount();

		if (sx == dx && sy == dy)
		{
			return 0;
		}
		StepInitialize(sx, sy, dx, dy, sid, did);

		_Node_BStar * pFirst = _Node_BStar::Alloc();
		new (pFirst) _Node_BStar();
		if (pFirst == NULL)
		{
			ASSERT(FALSE);
		}
		pFirst->x = sx;
		pFirst->y = sy;
		pFirst->f = pFirst->g + pFirst->h;
		pFirst->id = sid;
		pFirst->parent = NULL;
		add_to_open(pFirst);

		nStepCount = 0;
		while(m_openTab.size() > 0)
		{
			nStepCount++;
			//Log(LOG_FATAL, "step %d start", nStepCount);

			if(nStepCount >= GetMaxLoop())
			{
				//Log(LOG_FATAL, "faild. too slow step %d costtime %d", nStepCount, GetTickCount() - ulT1);
				return -1;
			}

			for(int i = 0; i < m_openTab.size(); ++i)
			{
				_Node_BStar * pCurrentNode = (_Node_BStar *)m_openTab[i];
				_Node_BStar lNextNode;
				ASSERT(pCurrentNode);

				// is target
				if(pCurrentNode->id == m_nodeEnd.id)
				{
					m_pBest = pCurrentNode;
					//Log(LOG_FATAL, "succeed costtime %d", GetTickCount() - ulT1);
					return nStepCount;
				}

				if(pCurrentNode->branch == _Node_BStar::branch_dir_none)
				{
					//Log(LOG_FATAL, "freenode %d %d", pCurrentNode->x, pCurrentNode->y);
					if(!_GoStraight(pCurrentNode, lNextNode))
					{
						_TurnLeft(pCurrentNode, lNextNode);
						_TurnRight(pCurrentNode, lNextNode);
					}
				}
				else
				{
					// 非自由节点，沿着障碍爬
					if (pCurrentNode->branch == _Node_BStar::branch_dir_left)
					{
						_ClimbLeft(pCurrentNode);
					}
					else
					{
						_ClimbRight(pCurrentNode);
					}
				}

				//if(pCurrentNode->id != m_nodeStart.id)
				//{
				add_to_close(pCurrentNode);
				//}
			}

			clear_open();
			for(int ii = 0; ii < mBackTable.size(); ++ii)
			{
				add_to_open(mBackTable[ii]);
			}
			mBackTable.clear();
			//Log(LOG_FATAL, "");
			ASSERT(NULL == pFirst->parent);
		}

		//Log(LOG_FATAL, "error costtime %d", GetTickCount() - ulT1);
		return 0;
	}

	void KBStarIndependence::StepInitialize(int_r sx, int_r sy, int_r dx, int_r dy, int_r &sid, int_r &did)
	{	PERF_COUNTER(KBStarIndependence_StepInitialize);

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

	void KBStarIndependence::clear_open()
	{	PERF_COUNTER(KBStarIndependence_clear_open);

		IAlgorithmDynArray<__B_Cmp>::clear_open();
		mOpenTable.clear();
	}
	
	bool KBStarIndependence::add_to_open(const _Node * pNode)
	{	PERF_COUNTER(KBStarIndependence_add_to_open);

		IAlgorithmDynArray<__B_Cmp>::add_to_open(pNode);
		mOpenTable.push_back(pNode);
		return true;
	}

	_Node * KBStarIndependence::find_from_open(int_r nID)
	{
		_Node * pNode = IAlgorithmDynArray<__B_Cmp>::find_from_open(nID);
		if(!pNode)
		{
			for(int i = 0; i < mBackTable.size(); ++i)
			{
				if(mBackTable[i]->id == nID)
				{
					return const_cast<_Node *>(mBackTable[i]);
				}
			}
		}
		return NULL;
	}

	///////////////

	KBStar::KBStar():KBStarIndependence()
	{	PERF_COUNTER(KBStar_KBStar);

	}

	KBStar::~KBStar()
	{	PERF_COUNTER(KBStar_D_KBStar);

		ReleaseNodes();
	}

	int_r KBStar::GeneratePath(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did)
	{	PERF_COUNTER(KBStar_GeneratePath);

		return GeneratePath_NoRelease(sx, sy, dx, dy, nMaxStep, sid, did);
	}

	///////////////

	KBStarComplex::KBStarComplex():IAlgorithmBase(),nStepCount(0),nRStepCount(0)
	{	PERF_COUNTER(KBStarComplex_KBStarComplex);

		//
	}

	KBStarComplex::~KBStarComplex()
	{	PERF_COUNTER(KBStarComplex_D_KBStarComplex);

		nStepCount = 0;
		nRStepCount = 0;
	}

	void KBStarComplex::SetMaxLoop(int_r maxLoop)
	{	PERF_COUNTER(KBStarComplex_SetMaxLoop);

		IAlgorithmBase::SetMaxLoop(maxLoop);
		mForward.SetMaxLoop(maxLoop);
		mReverse.SetMaxLoop(maxLoop);
	}

	void KBStarComplex::SetMaxLineLength(int_r nLength)
	{	PERF_COUNTER(KBStarComplex_SetMaxLineLength);

		IAlgorithmBase::SetMaxLineLength(nLength);
		mForward.SetMaxLineLength(nLength);
		mReverse.SetMaxLineLength(nLength);
	}

	void KBStarComplex::SetPlugin(IFinderPlugin * pPlugin)
	{	PERF_COUNTER(KBStarComplex_SetPlugin);

		IAlgorithmBase::SetPlugin(pPlugin);
		mForward.SetPlugin(pPlugin);
		mReverse.SetPlugin(pPlugin);
	}

	int_r KBStarComplex::GeneratePath(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did)
	{	PERF_COUNTER(KBStarComplex_GeneratePath);

		ReleaseNodes();
		nStepCount = mForward.GeneratePath(sx, sy, dx, dy, nMaxStep, sid, did);
		nRStepCount = 0;//mReverse.GeneratePath(dx, dy, sx, sy, nMaxStep, did, sid);
		if( nRStepCount > 0 && nStepCount > 0 )
			return nRStepCount < nStepCount ? nRStepCount : nStepCount;
		else if( nRStepCount <= 0 && nStepCount <= 0 )
			return 0;
		return nRStepCount > 0 ? nRStepCount : nStepCount;
	}

	void KBStarComplex::ReleaseNodes()
	{	PERF_COUNTER(KBStarComplex_ReleaseNodes);

		mForward.ReleaseNodes();
		mReverse.ReleaseNodes();
		_Node_BStar::ResetPool();
	}

	_Node * KBStarComplex::GetBestNodeFromStart(bool bIsDis/*=true*/)
	{
		if( nRStepCount > 0 && nStepCount > 0 )
		{
			if(nRStepCount < nStepCount)
			{
				return mReverse.GetBestNode();
			}
			else
			{
				return _Reverse(mForward.GetBestNode());
			}
		}
		else if( nRStepCount <= 0 && nStepCount <= 0 )
		{
			return NULL;
		}
		return nRStepCount > 0 ? mReverse.GetBestNode() : _Reverse(mForward.GetBestNode());
	}

	_Node * KBStarComplex::_Reverse(_Node * pHead)
	{
		if(pHead == NULL)
			return NULL;
		_Node *pre(NULL), *cur(NULL), *ne(NULL);
		pre = pHead;
		cur = pHead->parent;
		while(cur)
		{
			ne = cur->parent;
			cur->parent = pre;
			pre = cur;
			cur = ne;
		}
		pHead->parent = NULL;
		pHead = pre;
		return pHead;
	}
};