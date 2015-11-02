#pragma once
#include <System/KType.h>
#include <System/Collections/DynArray.h>
#include "PathFinderDefine.h"

namespace KPathFinderAbout
{
	class IAlgorithmBase;
	class _Node;
	struct IFinderPlugin
	{
	public:
		virtual ~IFinderPlugin() {}
		virtual bool SetupAStar(KPathFinderAbout::IAlgorithmBase * pAStar, void * pPointer) = 0;
		virtual bool IsObstacle(int_r x, int_r y) const = 0;
		virtual bool IsExist(int_r x, int_r y) const = 0;
		virtual bool CanBressenhamTest(int_r x, int_r y, int_r tx, int_r ty) const = 0;
		virtual bool PathCompact(KPathFinderAbout::_Node * pNode) = 0;
		virtual int_r GetPath(int_r from, int_r to, JG_C::DynArray<int_r> &nodeIDs, VALUE_T& cost) = 0;
		virtual int_r Coord2ID(int_r x, int_r y) const = 0;

		virtual int_r AS_Valid(KPathFinderAbout::_Node * pParent, KPathFinderAbout::_Node * pNode, int_r nData, void * pPointer) = 0;
		virtual VALUE_T AS_Cost(KPathFinderAbout::_Node * pParent, KPathFinderAbout::_Node * pNode, int_r nData, void * pPointer) = 0;
		virtual VALUE_T AS_Heuristici(KPathFinderAbout::_Node * pStart, KPathFinderAbout::_Node * pEnd, int_r nData, void * pPointer) = 0;
		virtual int_r AS_Notify_ByList(KPathFinderAbout::_Node * pParent, KPathFinderAbout::_Node * pNode, int_r nData, void * pPointer) = 0;
		virtual int_r AS_Notify_ByChild(KPathFinderAbout::_Node * pParent, KPathFinderAbout::_Node * pNode, int_r nData, void * pPointer) = 0;
		virtual int_r AS_CreateChild(KPathFinderAbout::_Node * pNode, KPathFinderAbout::_Node Nodes_out[], int_r nSize, void * pPointer) = 0;
		virtual int_r AS_CreateDiagonalChild(KPathFinderAbout::_Node * pNode, KPathFinderAbout::_Node Nodes_out[], int_r nSize, void * pPointer) = 0;
	};
}