#pragma once
#include <System/Collections/KHashTable.h>
#include "../interface/PathFinderDefine.h"
#include "../interface/IFinderPlugin.h"
#include "AlgorithmDynArray.h"
#include "CommonUtils.h"

namespace KPathFinderAbout
{
	class _Node_BStar : public _Node
	{
	public:
		enum enBranchDirect
		{
			branch_dir_left = 0,
			branch_dir_right,
			branch_dir_none,
			branch_dir_count = branch_dir_none,
		};

		_Node_BStar():_Node(),branch(branch_dir_none),dir(eBD_Unknow),angle(0),real(0) {}
		_Node_BStar(const _Node &o):_Node(o),branch(branch_dir_none),dir(eBD_Unknow),angle(0),real(0) {}
		_Node_BStar(const _Node_BStar &o) {memcpy(this, &o, sizeof(_Node_BStar));}
		enBranchDirect branch;
		_BStar_Direct dir;
		int angle;
		int real;

	public:
		typedef ::System::Memory::KStepBlockAlloc<_Node_BStar, 2048> Pool;
		static Pool& _pool()
		{
			static Pool pool;
			return pool;
		}
		static _Node_BStar* Alloc()
		{
			Pool& pool = _pool();
			void* p = pool.alloc(sizeof(_Node_BStar));
			return (_Node_BStar*)p;
		}
		static void Free(_Node_BStar* p)
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

	class __B_Cmp
	{
	public:
		int operator () (const _Node * a1, const _Node * b1) const
		{
			_Node_BStar * a = (_Node_BStar *)a1;
			_Node_BStar * b = (_Node_BStar *)b1;

			if( a->f < b->f )
				return -1;
			else if(a->f > b->f)
				return 1;
			else
			{
				if( a->dir < b->dir )
					return -1;
				else if(a->dir > b->dir)
					return 1; 
				//return 0;
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
		}
	};

	struct tagAround
	{
		int_r x, y;
	};

	static const tagAround sgcAround[MAX_DIRECTION] =
	{
		{0, 1},		//eBD_Bottom
		//{-1, 1},	//eBD_LBottom
		{-1, 0},	//eBD_Left
		//{-1, -1},	//eBD_LTop
		{0, -1},	//eBD_Top
		//{1, -1},	//eBD_RTop
		{1, 0},		//eBD_Right
		//{1, 1},		//eBD_RBottom
	};

	static const _BStar_Direct sgcBranchAround[_Node_BStar::branch_dir_count][MAX_DIRECTION] =
	{
		{ eBD_Bottom, /*eBD_RBottom,*/ eBD_Right, /*eBD_RTop,*/ eBD_Top, /*eBD_LTop,*/ eBD_Left, /*eBD_LBottom,*/ },	// branch_dir_left
		{ eBD_Bottom, /*eBD_LBottom,*/ eBD_Left, /*eBD_LTop,*/ eBD_Top, /*eBD_RTop,*/ eBD_Right, /*eBD_RBottom,*/ },	// branch_dir_right
	};

	class KBStarIndependence : public IAlgorithmDynArray<__B_Cmp>
	{
	public:
		static const int_r cn_angle8 = 8;
		static const int_r cn_angle64 = 64;
		static const int_r cn_angle64_2 = 32;
		KBStarIndependence();
		virtual ~KBStarIndependence();

		virtual int_r GeneratePath(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did);
		virtual void ReleaseNodes();

		virtual void clear_open();
		virtual bool add_to_open(const _Node * pNode);
		virtual _Node * find_from_open(int_r nID);

		int_r GeneratePath_NoRelease(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did);

	private:
		void StepInitialize(int_r sx, int_r sy, int_r dx, int_r dy, int_r &sid, int_r &did);

		bool _GoStraight(_Node_BStar * pCurrentNode, _Node_BStar &nextNode);
		void _TurnRight(_Node_BStar * pCurrentNode, _Node_BStar blockNode);
		void _TurnLeft(_Node_BStar * pCurrentNode, _Node_BStar blockNode);
		void _ClimbRight(_Node_BStar * pCurrentNode);
		void _ClimbLeft(_Node_BStar * pCurrentNode);

	protected:
		int_r m_iSX, m_iSY, m_iDX, m_iDY;
		_Node m_nodeStart;
		_Node m_nodeEnd;

		int_r nStepCount;
		JG_C::DynArray<const _Node *> mOpenTable;
		JG_C::DynArray<const _Node *> mBackTable;
	};

	class KBStar : public KBStarIndependence
	{
	public:
		KBStar();
		virtual ~KBStar();
		virtual int_r GeneratePath(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did);
	};

	class KBStarComplex : public IAlgorithmBase
	{
	public:
		KBStarComplex();
		virtual ~KBStarComplex();
		void SetMaxLoop(int_r maxLoop);
		void SetMaxLineLength(int_r nLength);
		void SetPlugin(IFinderPlugin * pPlugin);
		virtual int_r GeneratePath(int_r sx, int_r sy, int_r dx, int_r dy, int_r nMaxStep, int_r sid, int_r did);
		virtual void ReleaseNodes();
		virtual _Node * GetBestNodeFromStart(bool bIsDis=true);

	protected:
		_Node * _Reverse(_Node * pHead);

	private:
		int_r nStepCount;
		KBStar mForward;
		int_r nRStepCount;
		KBStar mReverse;
	};
};