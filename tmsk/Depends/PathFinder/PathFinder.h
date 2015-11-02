/********************************************************************
	created:	2008/01/03
	created:	3:1:2008   15:38
	filename: 	Athena\Main\Src\Common\LogicWorld\Src\DataStructure\PathFinder.h
	file path:	Athena\Main\Src\Common\LogicWorld\Src\DataStructure
	file base:	PathFinder
	file ext:	h
	author:		xueyan
	
	purpose:	通用的A＊寻路算法
*********************************************************************/



#ifndef _ASTAR_H_
#define _ASTAR_H_

//Using KMemoryPool
//#define USING_MEMORY_POOL

//Using own written memory pool
#define USING_OWN_MEMORY_POOL

#include <memory.h>
#include "AsIncludes.h"

#include "HashTable.h"
//#include "assert.h"


#ifdef USING_OWN_MEMORY_POOL
#	include "MemoryPool.h"
#endif

namespace DataStructureAbout
{

	class CAStar {
	public:
		CAStar();
		~CAStar();


		_asFunc  udValid;
		_asFunc	 udCost;
		_asFunc  udHeuristici;
		_createChildFunc	udCreateChild;

		_asFunc  udNotifyChild;
		_asFunc	 udNotifyList;

		void	*m_pCBData;
		void	*m_pNCData;

		int		GetBestNodeStepNum();

		int		GeneratePath(int sx, int sy, int dx, int dy, int nMaxStep, int sid = 0, int did = 0);
		int		Step();
		void	StepInitialize(int sx, int sy, int dx, int dy, int sid = 0, int did = 0);
		void	SetRows(int r)		 { m_iRows = r;    }
		void	Reset() { m_pBest = NULL; }
	inline void SetMaxLoop(int maxLoop){
		ASSERT_I(maxLoop > 0);
		m_maxLoop = maxLoop;
	}

		_asNode*	GetBestNode() { return m_pBest; }
	// make parent to link next node and return start node
	// a link list will be made by this function
	// using "parent" pointer instead "next" pointer because
	// "next" may be used by open table and close table.
	/*_asNode*	GetBestNodeFromStart() {
		if (m_pBest){
			_asNode* pNode = m_pBest;
			_asNode* p1 = NULL;
			_asNode* p2 = pNode;
			if (pNode){
				pNode = pNode->parent;
			}
			for(; pNode != NULL;)
			{
				p2->parent = p1;
				p1 = p2;
				p2 = pNode;
				pNode = pNode->parent;
			}
			p2->parent = p1;
			m_pBest = p2;
		}
		return m_pBest;
	}*/

		// 这个只保留了各个拐角的点
		_asNode*	GetBestNodeFromStart()
		{
			if (m_pBest)
			{

				_asNode* pNodeStart = m_pBest;
				_asNode* pNodeNext = NULL;

				INT nXDis = -2;
				INT nYDis = -2;

				// 这是一个反转程序
				_asNode* pNode = m_pBest;
				_asNode* p1 = NULL;
				_asNode* p2 = pNode;
				if (pNode)
				{
					pNode = pNode->parent;
					pNodeNext = pNode;

					if ( pNodeNext )
					{
						nXDis = pNodeStart->x - pNodeNext->x;
						nYDis = pNodeStart->y - pNodeNext->y;
					}

				}
				for(; pNode != NULL;)
				{
					if ( nXDis == pNodeStart->x - pNodeNext->x && 
						nYDis == pNodeStart->y - pNodeNext->y )
					{
						// 中间直线上的点不用记了
					}
					else
					{
						// 产生转角了。
						p2->parent = p1;
						p1 = p2;
						p2 = pNodeStart;

						nXDis = pNodeStart->x - pNodeNext->x;
						nYDis = pNodeStart->y - pNodeNext->y;
					}

					pNodeStart = pNode;
					pNode = pNode->parent;
					pNodeNext = pNode;

				}
				p2->parent = p1;
				p1 = p2;
				p2 = pNodeStart;

				p2->parent = p1;
				m_pBest = p2;
			}
			return m_pBest;
		}

	int	GetBestNodeFromStartNum()
	{
		int nNum = 0;
		if (m_pBest)
		{
			_asNode* pNode = m_pBest;
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

	protected:
		int		m_iRows;
		int		m_iSX, m_iSY, m_iDX, m_iDY, m_iDNum;
		_asNode m_nodeStart;
		_asNode m_nodeEnd;
		int		m_maxLoop;

		_asNode	*m_pOpen;
		_asNode	*m_pClosed;
		_asNode *m_pBest;
		_asStack*m_pStack;

#if defined(USING_OWN_MEMORY_POOL)
		MemoryPool<_asNode> m_nodePool;
		MemoryPool<_asStack, 100> m_stackNodePool;
#endif

		hash_table<_asNode*> m_openTab;
		hash_table<_asNode*> m_closeTab;


		// Functions.
		void	AddToOpen(_asNode *);
		void	ClearNodes();
		void	CreateChildren(_asNode *);
		void	LinkChild(_asNode *, _asNode *);
		void	UpdateParents(_asNode *);

		// Stack Functions.
		void	Push(_asNode *);
		_asNode *Pop();

		_asNode *CheckList(bool, int);
		_asNode	*GetBest();

		// Inline functions.
		inline int Coord2ID(int x, int y) { return x * m_iRows + y; }
		inline VALUE_TYPE udFunc(_asFunc, _asNode *, _asNode *, int, void *);
#ifdef _DEBUG_DETAIL
	public:
		int testSearchInOpenSuccCount;
		int testSearchInOpenFailCount;
		int testAddInOpenSearchCount;
		int testAddToOpenCallCount;
		int testSearchInCloseSuccCount;
		int testSearchInCloseFailCount;
		int testUpdateParentCallCount;
#endif
	};

} // end namespace DataStructureAbout

using namespace DataStructureAbout;

#endif

