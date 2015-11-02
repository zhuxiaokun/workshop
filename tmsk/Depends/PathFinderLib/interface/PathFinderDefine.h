#pragma once
#include <System/KType.h>
#include <System/Collections/DynArray.h>
#include <System/Collections/KMapByVector.h>
#include <System/Memory/KStepObjectPool.h>
#pragma   warning(disable:4244)

namespace KPathFinderAbout
{
	typedef float VALUE_T;

	#define ASNL_ADDOPEN		0
	#define ASNL_STARTOPEN		1
	#define ASNL_DELETEOPEN		2
	#define ASNL_ADDCLOSED		3

	#define ASNC_INITIALADD		0
	#define ASNC_OPENADD_UP		1
	#define ASNC_OPENADD		2
	#define ASNC_CLOSEDADD_UP	3
	#define ASNC_CLOSEDADD		4
	#define ASNC_NEWADD			5

	#define  ASTAR_MAX_CHILDREN	(1024)

	#define Distance2D(x1, y1, x2, y2) ((x1) - (x2)) * ((x1) - (x2)) + ((y1) - (y2)) * ((y1) - (y2))
	#define Distance3D(x1, y1, z1, x2, y2, z2) ((x1) - (x2)) * ((x1) - (x2)) + ((y1) - (y2)) * ((y1) - (y2)) + ((z1) - (z2)) * ((z1) - (z2))

	const static int_r MAX_COST = 0xffffff;
	const static int_r MAX_LOOP = 0x00ffff;
	const static int_r MAX_LENGTH = 0x000064;

	struct KL2MapSettings
	{
		int_r mBlockSize;
		int_r mMapSize;
	};

	struct _Node_Lite
	{
		_Node_Lite() {reset();}
		_Node_Lite(int_r nID, int_r a, int_r b):id(nID),x(a),y(b) {}
		void reset() {memset(this, 0, sizeof(_Node_Lite));}
		int_r id;
		int_r x;
		int_r y;

		class PtIDCmp
		{
		public:
			int_r operator()(const _Node_Lite * a, const _Node_Lite * b) const
			{
				if(a->id < b->id) return -1;
				else if(a->id > b->id) return 1;
				return 0;
			}
		};
		class PtCmp
		{
		public:
			int_r operator()(const _Node_Lite * a, const _Node_Lite * b) const
			{
				if(a->x < b->x) return -1;
				else if(a->x > b->x) return 1;
				else
				{
					if(a->y < b->y) return -1;
					else if(a->y > b->y) return 1;
				}
				return 0;
			}
		};

		bool operator == (const _Node_Lite& o)
		{
			if( this == &o )
				return true;
			return x == o.x && y == o.y && id == o.id;
		}
	};

	struct KPathPoint
	{
		KPathPoint() {memset(this, 0, sizeof(KPathPoint));}
		KPathPoint(int a, int b):x(a),y(b) {}
		int x;
		int y;
	};

	struct KPoint : public KPathPoint
	{
		KPoint() {memset(this, 0, sizeof(KPoint));}
		KPoint(float a, float b, const _Node_Lite * pNode):mNode(pNode) {x=a;y=b;}
		bool operator == (const KPoint &o) const {return floatEqual(o);}
		bool intEqual(const KPoint &o) const {return this==&o?true:(int_r)x==(int_r)o.x&&(int_r)y==(int_r)o.y;}
		bool floatEqual(const KPoint &o) const {return this==&o?true:(!(x>o.x))&&(!(x<o.x))&&(!(y>o.y))&&(!(y<o.y));}
		const _Node_Lite * mNode;
	};
	typedef JG_C::DynArray<KPoint> __Nodes;
	static __Nodes& push_point(__Nodes &aNodes, const _Node_Lite * pNode)
	{
		KPoint point(pNode->x, pNode->y, pNode);
		aNodes.push_back(point);
		return aNodes;
	}
	static __Nodes& push_point(__Nodes &aNodes, int_r x, int_r y)
	{
		KPoint point(x, y, NULL);
		aNodes.push_back(point);
		return aNodes;
	}
	struct __OneRoad
	{
		__OneRoad() {reset();}
		void reset() {m_Road.clear();m_Cost = MAX_COST;}
		__OneRoad& push_back(const _Node_Lite * pNode)
		{
			push_point(m_Road, pNode);
			return *this;
		}
		__Nodes m_Road;
		VALUE_T m_Cost;
	};
	typedef JG_C::KMapByVector<int_r, __OneRoad> __RoadMap_ByID;

	class _Node : public _Node_Lite //, public JG_M::KPortableStepPool<_Node, 2048>
	{
	public:
		_Node()	{reset();}
		_Node(int_r a, int_r b) {reset();x=a;y=b;};
		_Node(const _Node &o) {memcpy(this, &o, sizeof(_Node));}
		_Node(const _Node *o) {if(o) memcpy(this, o, sizeof(_Node));}
		void reset() {memset(this, 0, sizeof(_Node));}

		VALUE_T f,g,h;
		_Node * parent;

		//typedef ::System::Memory::KStepBlockAlloc<_Node,2048> Pool;
		//static Pool& _pool()
		//{
		//	static Pool pool;
		//	return pool;
		//}
		//static _Node* Alloc()
		//{
		//	Pool& pool = _pool();
		//	void* p = pool.alloc(sizeof(_Node));
		//	return (_Node*)p;
		//}
		//static void Free(_Node* p)
		//{
		//	Pool& pool = _pool();
		//	pool.free(p);
		//}
		//static void ResetPool()
		//{
		//	Pool& pool = _pool();
		//	pool.clear();
		//}
	};

	struct _asStack : public JG_M::KPortableStepPool<_asStack, 1024>
	{
		_asStack() {reset();}
		void reset() {memset(this, 0, sizeof(_asStack));}
		_Node	 *data;
		_asStack *next;
	};

	typedef int_r (*_asFunc)(_Node *, _Node *, int_r, void *);
	typedef int_r (*_createChildFunc)(const _Node *, _Node *, void *);
}
DECLARE_SIMPLE_TYPE(KPathFinderAbout::KPathPoint);
DECLARE_SIMPLE_TYPE(KPathFinderAbout::KPoint);
DECLARE_SIMPLE_TYPE(KPathFinderAbout::_Node);
DECLARE_SIMPLE_TYPE(KPathFinderAbout::_Node_Lite);