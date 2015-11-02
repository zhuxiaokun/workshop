#pragma once
#include "../Memory/KAlloc.h"
#include "../Collections/KMapByVector.h"

/// 带内存池的KMapByVector，可以优化内存的使用

namespace System { namespace Collections {

	template <class K,class V,size_t step>
	class MapNodePoolAllocator
	{
	public:
		typedef KMapNode<K,V> node_type;
		typedef ::System::Memory::KStepBlockAlloc<node_type,step> node_pool;

	public:
		node_pool m_pool;

	public:
		void* alloc(int)
		{
			return m_pool.alloc(sizeof(node_type));
		}
		void free(void* node)
		{
			m_pool.free(node);
		}
		int unitSize() const
		{
			return sizeof(node_type);
		}
	};
	
	template <class K,class V,size_t step>
	class MapNodePoolAllocator_static
	{
	public:
		typedef KMapNode<K,V> node_type;
		typedef ::System::Memory::KStepBlockAlloc<node_type,step> node_pool;

	public:
		node_pool& _getPool()
		{
			static node_pool* m_pool = new node_pool();
			return *m_pool;
		}

	public:
		void* alloc(int)
		{
			return this->_getPool().alloc(sizeof(node_type));
		}
		void free(void* node)
		{
			this->_getPool().free(node);
		}
		int unitSize() const
		{
			return sizeof(node_type);
		}
	};

	/***
	使用内存池的map对象,
	map节点的内存池。
	*/
	template <
		typename K,
		typename V,
		size_t step = 1024,
		typename C = KLess<K>
	>
	class KMapByVector_pooled
		: public KMapByVector<K,V,C,MapNodePoolAllocator<K,V,step> >
	{
	public:
		typedef KMapByVector<K,V,C,MapNodePoolAllocator<K,V,step> > base_type;
		KMapByVector_pooled()
		{
		}
		KMapByVector_pooled(const KMapByVector_pooled& o):base_type(o)
		{
		}
		KMapByVector_pooled& operator = (const KMapByVector_pooled& o)
		{
			base_type::operator = (o);
			return *this;
		}
	};

	/***
	使用静态内存池的map对象,
	当K,V,step相同时，会使用同一个pool
	*/
	template <
		typename K,
		typename V,
		size_t step = 1024,
		typename C = KLessCompare<K> >
	class KMapByVector_staticPooled
		: public KMapByVector<K,V,C,MapNodePoolAllocator_static<K,V,step> >
	{
	public:
		typedef KMapByVector<K,V,C,MapNodePoolAllocator_static<K,V,step> > base_type;
		KMapByVector_staticPooled()
		{
		}
		KMapByVector_staticPooled(const KMapByVector_staticPooled& o):base_type(o)
		{
		}
		KMapByVector_staticPooled& operator = (const KMapByVector_staticPooled& o)
		{
			base_type::operator = (o);
			return *this;
		}
	};

}}