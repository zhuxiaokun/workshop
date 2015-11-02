#pragma once
#include <System/Collections/KHashTable.h>
#include "../interface/PathFinderDefine.h"
#include "../interface/IFinderPlugin.h"
#include "AlgorithmHash.h"
#include "AlgorithmDynArray.h"

namespace KPathFinderAbout
{
	class _Node_AStar : public _Node
	{
	public:
		_Node_AStar() {memset(this, 0, sizeof(_Node_AStar));}
		_Node_AStar(_Node &o):_Node(o),numchildren(0),nStepNum(0),next(NULL) {memset(children, 0, sizeof(children));}
		_Node_AStar(_Node_AStar &o) {memcpy(this, &o, sizeof(_Node_AStar));}

		int_r		numchildren;
		int_r		nStepNum;//增加步深的概念,默认是0就是第一个根结点
		_Node_AStar *next;
		_Node		*children[ASTAR_MAX_CHILDREN];

	public:
		typedef ::System::Memory::KStepBlockAlloc<_Node_AStar, 2048> Pool;
		static Pool& _pool()
		{
			static Pool pool;
			return pool;
		}
		static _Node_AStar* Alloc()
		{
			Pool& pool = _pool();
			void* p = pool.alloc(sizeof(_Node_AStar));
			return (_Node_AStar*)p;
		}
		static void Free(_Node_AStar* p)
		{
			Pool& pool = _pool();
			pool.free(p);
		}
		static void ResetPool()
		{
			Pool& pool = _pool();
			pool.clear();
		}
	};

	class KAStar : public IAlgorithmHash
	{
	public:
		KAStar();
		virtual ~KAStar();
		
		virtual int_r GeneratePath(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did);
		virtual void ReleaseNodes();

	private:
		int_r Step();
		void StepInitialize(int_r sx, int_r sy, int_r dx, int_r dy, int_r &sid, int_r &did);
		int_r GetBestNodeStepNum();
		int_r GetBestNodeFromStartNum();

	protected:
		void * m_pCBData;
		void * m_pNCData;
		int_r m_iSX, m_iSY, m_iDX, m_iDY, m_iDNum;
		_Node m_nodeStart;
		_Node m_nodeEnd;
		_Node * m_pOpen;
		_Node * m_pClosed;
		_asStack * m_pStack;

		// Functions.
		void	AddToOpen(_Node *);
		void	CreateChildren(_Node *);
		void	LinkChild(_Node *, _Node *);
		void	UpdateParents(_Node *);

		// Stack Functions.
		void	Push(_Node *);
		_Node * Pop();
		_Node * GetBest();
	};

	class __f_Cmp
	{
	public:
		int operator () (const _Node * a, const _Node * b) const
		{
			if( a->f < b->f )
				return -1;
			else if(a->f > b->f)
				return 1;
			else
			{
				if( a->id < b->id )
					return -1;
				else if(a->id > b->id)
					return 1; 
				else
					return 0;
			}
		}
	};

	class KAStar2 : public IAlgorithmDynArray<__f_Cmp>
	{
	public:
		KAStar2();
		virtual ~KAStar2();

		virtual int_r GeneratePath(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did);
		virtual void ReleaseNodes();

	private:
		void StepInitialize(int_r sx, int_r sy, int_r dx, int_r dy, int_r &sid, int_r &did);

	protected:
		int_r m_iSX, m_iSY, m_iDX, m_iDY;
		_Node m_nodeStart;
		_Node m_nodeEnd;
	};
}