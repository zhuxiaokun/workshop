#pragma once
#include <System/KType.h>
#include "../interface/PathFinderDefine.h"
#include "../interface/IFinderPlugin.h"
#include "AlgorithmDynArray.h"

namespace KPathFinderAbout
{
	class _Node_DirectAStar : public _Node
	{
	public:
		_Node_DirectAStar() {memset(this, 0, sizeof(_Node_DirectAStar));}
		_Node_DirectAStar(const _Node_DirectAStar &o) {memcpy(this, &o, sizeof(_Node_DirectAStar));}

		int_r dir;

	public:
		typedef ::System::Memory::KStepBlockAlloc<_Node_DirectAStar, 2048> Pool;
		static Pool& _pool()
		{
			static Pool pool;
			return pool;
		}
		static _Node_DirectAStar* Alloc()
		{
			Pool& pool = _pool();
			void* p = pool.alloc(sizeof(_Node_DirectAStar));
			return (_Node_DirectAStar*)p;
		}
		static void Free(_Node_DirectAStar* p)
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

	class __d_Cmp
	{
	public:
		int operator () (const _Node * a1, const _Node * b1) const
		{
			_Node_DirectAStar * a = (_Node_DirectAStar *)a1;
			_Node_DirectAStar * b = (_Node_DirectAStar *)b1;
			if( a->f < b->f )
				return -1;
			else if(a->f > b->f)
				return 1;
			else
			{
				if(a->dir < b->dir )
					return -1;
				else if(a->dir > b->dir)
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
		}
	};

	class KDirectAStar : public IAlgorithmDynArray<__d_Cmp>
	{
	public:
		KDirectAStar();
		virtual ~KDirectAStar();

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