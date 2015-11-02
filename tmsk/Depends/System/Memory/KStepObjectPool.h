#ifndef _K_STEP_OBJECT_POOL_H_
#define _K_STEP_OBJECT_POOL_H_

#include "../Sync/KSync.h"
#include "KMemoryRecordset.h"
#include "../Collections/KSDList.h"
#include "KMemoryCleaner.h"
#include "../Collections/DynArray.h"
#include "../Log/log.h"
#include <typeinfo>
#include "_memory_pool.h"

namespace System { namespace Memory {

template <typename T, int capacity> class KPieceObjectPool
{
public:
	typedef T obj_type;
	typedef JG_C::SDListNode<T> node_type;
	typedef _memory_piece<sizeof(node_type),capacity> piece_type;
	typedef JG_C::KSDSelfList<node_type> node_list;
	piece_type m_piece;
	node_list m_lstFreeObj;

public:
	KPieceObjectPool()
	{
	}
	~KPieceObjectPool()
	{
		node_type* n;
		while(n=m_lstFreeObj.pop_front(), n)
			((obj_type*)n)->~obj_type();
	}

public:
	obj_type* Alloc()
	{
		node_type* pNode = m_lstFreeObj.pop_front();
		if(pNode)
		{
			pNode->m_pNextNode = (node_type*)INVALID_PTR;
			return &pNode->m_val;
		}

		pNode = (node_type*)m_piece.allocUnit();
		if(!pNode) return NULL;

		pNode->m_pNextNode = (node_type*)INVALID_PTR;
		obj_type* pObj = &pNode->m_val;
		new (pObj) obj_type;
		return pObj;
	}
	BOOL Free(obj_type* pObj)
	{
		if(!m_piece.has(pObj)) return FALSE;
		node_type* pNode = (node_type*)pObj;

		#if defined(_DEBUG)
		if(pNode->m_pNextNode != INVALID_PTR)
		{
			char buf[1024];
			const char* bt = backtrace(buf, sizeof(buf));
			Log(LOG_ERROR, "error: PieceObjectPool:Free, at:%s", bt);
		}
		#endif

		ASSERT_RETURN(pNode->m_pNextNode == INVALID_PTR, FALSE);
		m_lstFreeObj.push_back(pNode);
		return TRUE;
	}
};

template <typename T, int capacity> class KPieceObjectPool2
{
public:
public:
	typedef T obj_type;
	typedef T node_type;
	typedef _memory_piece<sizeof(node_type),capacity> piece_type;
	piece_type m_piece;

public:
	KPieceObjectPool2()
	{
	}
	~KPieceObjectPool2()
	{
	}

public:
	obj_type* Alloc()
	{
		obj_type* pObj = (obj_type*)m_piece.allocUnit();
		if(!pObj) return NULL;
		new (pObj) obj_type;
		return pObj;
	}
	BOOL Free(obj_type* pObj)
	{
		if(!m_piece.has(pObj)) return FALSE;
		(pObj)->~obj_type();
		m_piece.freeUnit(pObj);
		return TRUE;
	}
};

template <typename T, int stepCapcity,typename L=::System::Sync::KNoneLock,bool shrinkable=false> 
class KStepObjectPool : public _object_pool<T,stepCapcity,shrinkable,L>
{
public:
	typedef _object_pool<T,stepCapcity,shrinkable,L> base_type;
	void Destroy()
	{
		base_type::reset();
	}
};

template <typename T,int stepCapcity,typename L=::System::Sync::KNoneLock,bool shrinkable=false> 
class KStepObjectPool2 : public _autoreset_object_pool<T,stepCapcity,shrinkable,L>
{
public:
	typedef _autoreset_object_pool<T,stepCapcity,shrinkable,L> base_type;
	void Destroy()
	{
		base_type::reset();
	}
};

template <typename T,int stepCapacity=64,typename L=::System::Sync::KNoneLock,bool shrinkable=false>
class KPortableStepPool : public _portable_object_pool<T,stepCapacity,shrinkable,L>
{
//
};

template <typename T,int stepCapacity=64,typename L=::System::Sync::KNoneLock,bool shrinkable=false>
class KPortableMemoryPool : public _portable_autoreset_object_pool<T,stepCapacity,shrinkable,L>
{
//
};

}}
#endif
