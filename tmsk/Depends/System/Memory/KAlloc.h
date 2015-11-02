#ifndef _K_ALLOC_H_
#define _K_ALLOC_H_

#include "../KMacro.h"
#include "../KType.h"
#include "KMemoryRecordset.h"
#include "KNew.h"
#include "../Collections/DynArray.h"
#include "KMemory.h"
#include "_memory_pool.h"
#include "_heap_alloc.h"

namespace System { namespace Memory {

#define _max_size(a,b) ((a)>(b)?(a):(b))

/**
* 从预分配的大块内存中分配
*/
template <typename T, int capacity> class BlockAllocator
{
public:
	typedef _memory_piece<sizeof(T),capacity> piece_type;
	typedef typename piece_type::unit_type unit_type;
	piece_type m_piece;

public:
	BlockAllocator()
	{
		m_piece.init();
	}
	~BlockAllocator()
	{
	}
	void* alloc(int /*size*/)
	{
		return m_piece.allocUnit();
	}
	BOOL free(void* p)
	{
		return m_piece.freeUnit(p);
	}
	int unitSize() const
	{
		return (int)sizeof(unit_type);
	}
	void clear()
	{
		m_piece.clear();
	}
};

// 仿造上面的做一个动态初始化的block by xueyan
template <typename T> class BlockAllocator_D
{
public:
	typedef _dyn_memory_piece<sizeof(T)> piece_type;
	typedef typename piece_type::unit_type unit_type;
	piece_type m_piece;

public:
	BlockAllocator_D()
	{

	}
	~BlockAllocator_D()
	{
	}
	// 使用前要调初始化函数
	void init(int capacity)
	{
		m_piece.init(capacity);
	}
	void* alloc(int /*size*/)
	{
		return m_piece.allocUnit();
	}
	BOOL free(void* p)
	{
		return m_piece.freeUnit(p);
	}
	int unitSize() const
	{
		return (int)sizeof(unit_type);
	}
	void clear()
	{
		m_piece.clear();
	}
};

// 内存池，分片扩展，不会释放
template <typename T,int steps> class KStepBlockAlloc
{
public:
	typedef T value_type;
	typedef _memory_pool<sizeof(T),steps> memory_pool_type;
	typedef typename memory_pool_type::piece_type::unit_type unit_type;
	memory_pool_type m_memory_pool;

public:
	KStepBlockAlloc()
	{

	}
	~KStepBlockAlloc()
	{
		
	}

public:
	void* alloc(int /*size*/)
	{
		return m_memory_pool.Alloc();
	}
	BOOL free(void* p)
	{
		m_memory_pool.Free(p); return TRUE;
	}
	int unitSize() const
	{
		return (int)sizeof(unit_type);
	}
	void clear()
	{
		m_memory_pool.clear();
	}
};

/**
 * 扩展的分配器，如果大块内存耗尽，则使用HeapAllocator
 * 所以它的总容量是没有限制的
 * 
 * 改成: _shrinkable_memory_pool<T,capacity/4>来实现了
 * by: zxk 
 * at: 2013-09-04
 *
 */
template <typename T,int capacity> class BlockAllocatorEx
	: public _shrinkable_memory_pool<sizeof(T),_max_size(capacity/4,16)>
{
public:
	typedef T value_type;
	typedef _shrinkable_memory_pool<sizeof(T),_max_size(capacity/4,16)> memory_pool_type;
	typedef typename memory_pool_type::piece_type::unit_type unit_type;
	memory_pool_type m_memory_pool;

public:
	BlockAllocatorEx()
	{

	}
	~BlockAllocatorEx()
	{

	}

public:
	void* alloc(int /*size*/)
	{
		return m_memory_pool.Alloc();
	}
	BOOL free(void* p)
	{
		m_memory_pool.Free(p); return TRUE;
	}
	int unitSize() const
	{
		return (int)sizeof(unit_type);
	}
	void clear()
	{
		m_memory_pool.clear();
	}
};
	
#undef _max_size

}}

#endif
