#ifndef _K_TREE_H_
#define _K_TREE_H_

#include "../KType.h"
#include "../KMacro.h"
#include "../Memory/KAlloc.h"

/**
 * 红黑树实现的map容器，tree的实现是从网上找的代码，做了一下C++的封装
 * zxk 2011.9.2
 */

namespace System { namespace Collections {

	struct rb_node_t
	{
		#define	RB_RED		0
		#define	RB_BLACK	1
		uint_r  rb_parent_color;
		struct rb_node_t* rb_right;
		struct rb_node_t* rb_left;
	};

	struct rb_root_t
	{
		struct rb_node_t *rb_node;
		rb_root_t():rb_node(NULL)
		{
		}
	};

	static inline void rb_set_parent(struct rb_node_t *rb, struct rb_node_t *p)
	{
		rb->rb_parent_color = (uint_r)((rb->rb_parent_color & 3) | (uint_r)p);
	}

	#define RB_ROOT	(struct rb_root_t) { NULL, }
	#define	rb_entry(ptr, type, member) ((type*)((char*)ptr - (char*)&((type*)0)->member))
	#define RB_CLEAR_NODE(node)	(rb_set_parent(node, node))
	#define RB_EMPTY_ROOT(root)	((root)->rb_node == NULL)
	#define RB_EMPTY_NODE(node)	(rb_parent(node) == node)

	static inline void rb_init_node(struct rb_node_t *rb)
	{
		rb->rb_parent_color = 0;
		rb->rb_right = NULL;
		rb->rb_left = NULL;
		RB_CLEAR_NODE(rb);
	}

	extern void rb_insert_color(struct rb_node_t*, struct rb_root_t*);
	extern void rb_erase(struct rb_node_t*, struct rb_root_t*);
	extern struct rb_node_t* rb_next(const struct rb_node_t*);
	extern struct rb_node_t* rb_prev(const struct rb_node_t*);
	extern struct rb_node_t* rb_first(const struct rb_root_t*);
	extern struct rb_node_t* rb_last(const struct rb_root_t*);
	extern void rb_replace_node(struct rb_node_t*, struct rb_node_t*, struct rb_root_t*);
	
    static inline void rb_link_node(struct rb_node_t * node, struct rb_node_t * parent, struct rb_node_t ** rb_link)
    {
        node->rb_parent_color = (uint_r)parent;
        node->rb_left = node->rb_right = NULL;
                
        *rb_link = node;
    }

	struct rb_node_val
	{
		struct rb_node_t rb_node;
	};

	#define rb_parent(node) \
		((rb_node_t*)((node)->rb_parent_color & ~3))

	template <typename T> class _Tree
	{
	public:
		typedef T value_type;
		typedef _Tree tree_type;

	public:
		static value_type* from_rb_node(rb_node_t* n)
		{
			return (value_type*)((char*)n - (char*)&((value_type*)0)->rb_node);
		}
		static const value_type* from_rb_node(const rb_node_t* n)
		{
			return (const value_type*)((char*)n - (char*)&((value_type*)0)->rb_node);
		}
		static rb_node_t* from_type_val(value_type* v)
		{
			return &v->rb_node;
		}
		static const rb_node_t* from_type_val(const value_type* v)
		{
			return &v->rb_node;
		}

	public:
		class iterator
		{
		public:
			iterator():m_node(NULL)
			{
			}
			iterator(rb_node_t* node):m_node(node)
			{
			}
			iterator(const iterator& o):m_node(o.m_node)
			{
			}
			~iterator()
			{
			}
			iterator& operator = (const iterator& o)
			{
				if(this == &o) return *this;
				m_node = o.m_node;
				return *this;
			}
			iterator& operator++(int)
			{
				m_node = rb_next(m_node);
				return *this;
			}
			iterator& operator++()
			{
				m_node = rb_next(m_node);
				return *this;
			}
			bool operator == (const iterator& o) const
			{
				return m_node == o.m_node;
			}
			bool operator != (const iterator& o) const
			{
				return m_node != o.m_node;
			}
			value_type& operator * ()
			{
				ASSERT(m_node);
				return *(tree_type::from_rb_node(m_node));
			}
			value_type* operator -> ()
			{
				ASSERT(m_node);
				return tree_type::from_rb_node(m_node);
			}

		public:
			rb_node_t* m_node;
		};
		class reverse_iterator
		{
		public:
			reverse_iterator():m_node(NULL)
			{
			}
			reverse_iterator(rb_node_t* node):m_node(node)
			{
			}
			reverse_iterator(const iterator& o):m_node(o.m_node)
			{
			}
			~reverse_iterator()
			{
			}
			reverse_iterator& operator = (const reverse_iterator& o)
			{
				if(this == &o) return *this;
				m_node = o.m_node;
				return *this;
			}
			reverse_iterator& operator++(int)
			{
				m_node = rb_prev(m_node);
				return *this;
			}
			reverse_iterator& operator++()
			{
				m_node = rb_prev(m_node);
				return *this;
			}
			bool operator == (const reverse_iterator& o) const
			{
				return m_node == o.m_node;
			}
			bool operator != (const reverse_iterator& o) const
			{
				return m_node != o.m_node;
			}
			value_type& operator * ()
			{
				ASSERT(m_node);
				return *(tree_type::from_rb_node(m_node));
			}
			value_type* operator -> ()
			{
				ASSERT(m_node);
				return tree_type::from_rb_node(m_node);
			}

		public:
			rb_node_t* m_node;
		};
		class const_iterator
		{
		public:
			const_iterator():m_node(NULL)
			{
			}
			const_iterator(const rb_node_t* node):m_node(node)
			{
			}
			const_iterator(const const_iterator& o):m_node(o.m_node)
			{
			}
			const_iterator(const iterator& o):m_node(o.m_node)
			{
			}
			const_iterator& operator = (const const_iterator& o)
			{
				m_node = o.m_node;
				return *this;
			}
			const_iterator& operator = (const iterator& o)
			{
				m_node = o.m_node;
				return *this;
			}
			const_iterator& operator ++ (int)
			{
				if(!m_node) return *this;
				m_node = rb_next(m_node);
				return *this;
			}
			const_iterator& operator ++ ()
			{
				if(!m_node) return *this;
				m_node = rb_next(m_node);
				return *this;
			}
			bool operator == (const const_iterator& o) const
			{
				return m_node == o.m_node;
			}
			bool operator != (const const_iterator& o) const
			{
				return m_node != o.m_node;
			}
			bool operator == (const iterator& o) const
			{
				return m_node == o.m_node;
			}
			bool operator != (const iterator& o) const
			{
				return m_node != o.m_node;
			}
			const value_type& operator * ()
			{
				ASSERT(m_node);
				return *(tree_type::from_rb_node(m_node));
			}
			const value_type* operator -> () const
			{
				ASSERT(m_node);
				return tree_type::from_rb_node(m_node);
			}
		public:
			const rb_node_t* m_node;
		};
		class const_reverse_iterator
		{
		public:
			const_reverse_iterator():m_node(NULL)
			{
			}
			const_reverse_iterator(rb_node_t* node):m_node(node)
			{
			}
			const_reverse_iterator(const const_reverse_iterator& o):m_node(o.m_node)
			{
			}
			const_reverse_iterator(const reverse_iterator& o):m_node(o.m_node)
			{
			}
			~const_reverse_iterator()
			{
			}
			const_reverse_iterator& operator = (const const_reverse_iterator& o)
			{
				m_node = o.m_node;
				return *this;
			}
			const_reverse_iterator& operator = (const reverse_iterator& o)
			{
				m_node = o.m_node;
				return *this;
			}
			const_reverse_iterator& operator++(int)
			{
				m_node = rb_prev(m_node);
				return *this;
			}
			const_reverse_iterator& operator++()
			{
				m_node = rb_prev(m_node);
				return *this;
			}
			bool operator == (const const_reverse_iterator& o) const
			{
				return m_node == o.m_node;
			}
			bool operator != (const const_reverse_iterator& o) const
			{
				return m_node != o.m_node;
			}
			bool operator == (const reverse_iterator& o) const
			{
				return m_node == o.m_node;
			}
			bool operator != (const reverse_iterator& o) const
			{
				return m_node != o.m_node;
			}
			const value_type& operator * ()
			{
				ASSERT(m_node);
				return *(tree_type::from_rb_node(m_node));
			}
			const value_type* operator -> ()
			{
				ASSERT(m_node);
				return tree_type::from_rb_node(m_node);
			}

		public:
			const rb_node_t* m_node;
		};

	public:
		_Tree()
		{

		}
		_Tree(_Tree& o):m_root(o.m_root)
		{

		}
		~_Tree()
		{

		}
		_Tree& operator = (_Tree& o)
		{
			m_root = o.m_root;
			return *this;
		}
		iterator begin()
		{
			return iterator(rb_first(&m_root));
		}
		const_iterator begin() const
		{
			return const_iterator(rb_first(&m_root));
		}
		iterator end()
		{
			return iterator();
		}
		const_iterator end() const
		{
			return const_iterator();
		}
		reverse_iterator rbegin()
		{
			return reverse_iterator(rb_last(&m_root));
		}
		const_reverse_iterator rbegin() const
		{
			return const_reverse_iterator(rb_last(&m_root));
		}
		reverse_iterator rend()
		{
			return reverse_iterator();
		}
		const_reverse_iterator rend() const
		{
			return const_reverse_iterator();
		}
		// re-balance the red-black tree starting from node
		void balance(struct rb_node_t* node)
		{
			rb_insert_color(node, &m_root);
		}
		void erase(iterator it)
		{
			if(!it.m_node) return;
			rb_erase(it.m_node, &m_root);
			return;
		}
		bool empty() const
		{
			return RB_EMPTY_ROOT(&m_root);
		}
		void clear()
		{
			m_root.rb_node = NULL;
		}

	public:
		rb_root_t m_root;
	};

	template <typename K,typename V>
	struct TreeMapNode : public rb_node_val, public KPair<K,V>
	{
		typedef KPair<K,V> pair_type;
		TreeMapNode() : pair_type(_type_default_value_<K>::get(),_type_default_value_<V>::get())
		{
			rb_init_node(&this->rb_node);
		}
		TreeMapNode(const K& k, const V& v):pair_type(k,v)
		{
			rb_init_node(&this->rb_node);
		}
	};

	template <class K,class V>
	class TreeMapNodeHeapAllocator
	{
	public:
		typedef TreeMapNode<K,V> node_type;
		typedef ::System::Memory::HeapAllocator node_pool;

	public:
		node_pool m_pool;

	public:
		void* alloc()
		{
			return m_pool.alloc(sizeof(node_type));
		}
		void free(void* node)
		{
			m_pool.free(node);
		}
	};

	template <class K,class V>
	class TreeMapNodeAllocator : private ::System::Memory::BlockAllocatorEx<TreeMapNode<K,V>,128>
	{
	public:
		typedef TreeMapNode<K,V> node_type;
		typedef ::System::Memory::BlockAllocatorEx<TreeMapNode<K,V>,128> base_type;
		void* alloc()
		{
			return base_type::alloc(sizeof(node_type));
		}
		void free(void* node)
		{
			base_type::free(node);
		}
	};

	template <class K,class V,size_t step>
	class TreeMapNodePoolAllocator
	{
	public:
		typedef TreeMapNode<K,V> node_type;
		typedef ::System::Memory::KStepBlockAlloc<node_type,step> node_pool;

	public:
		node_pool m_pool;

	public:
		void* alloc()
		{
			return m_pool.alloc(sizeof(node_type));
		}
		void free(void* node)
		{
			m_pool.free(node);
		}
	};

	template <class K,class V,size_t step>
	class TreeMapNodePoolAllocator_static
	{
	public:
		typedef TreeMapNode<K,V> node_type;
		typedef ::System::Memory::KStepBlockAlloc<node_type,step> node_pool;

	public:
		node_pool& _getPool()
		{
			static node_pool* m_pool = new node_pool();
			return *m_pool;
		}

	public:
		void* alloc()
		{
			return this->_getPool().alloc(sizeof(node_type));
		}
		void free(void* node)
		{
			this->_getPool().free(node);
		}
	};

	/**
		使用红黑树作为存储结构的map对象，
		缺省使用heap来管理节点的分配和释放，
		可以指定自己的内存分配器。
	 */
	template <
		class K,						// key type
		class V,						// value type
		class C = KLessCompare<K>,		// key compare type
		class A = TreeMapNodeAllocator<K,V> // node allocator
										// default is a BlockAllocatorEx allocator
										// or specify 'TreeMapNodeHeapAllocator<K,V,step>' use a pool allocator 
	>
	class KMapByTree
	{
	public:
		typedef K key_type;
		typedef V value_type;
		typedef C compare_type;
		typedef A alloc_type;
		typedef typename alloc_type::node_type node_type;
		typedef _Tree<node_type> tree_type;

	public:
		typedef typename tree_type::iterator iterator;
		typedef typename tree_type::const_iterator const_iterator;
		typedef typename tree_type::reverse_iterator reverse_iterator;
		typedef typename tree_type::const_reverse_iterator const_reverse_iterator;

	public:
		size_t m_size;
		alloc_type m_alloc;
		tree_type m_tree;
		compare_type m_compare;

	public:
		KMapByTree():m_size(0)
		{

		}
		KMapByTree(const KMapByTree& o):m_size(0)
		{
			const_iterator it = o.begin();
			const_iterator eit = o.end();
			for(; it != eit; it++)
			{
				this->insert(it->first, it->second);
			}
		}
		~KMapByTree()
		{
			this->clear();
		}
		KMapByTree& operator = (const KMapByTree& o)
		{
			if(this == &o) return *this;
			this->clear();
			const_iterator it = o.begin();
			const_iterator eit = o.end();
			for(; it != eit; it++)
			{
				this->insert(it->first, it->second);
			}
			return *this;
		}
		iterator begin()
		{
			return m_tree.begin();
		}
		iterator end()
		{
			return m_tree.end();
		}
		iterator find(const key_type& k)
		{
			node_type* node;
			struct rb_node_t* n = m_tree.m_root.rb_node;
			while (n)
			{
				node = tree_type::from_rb_node(n);
				int cmp_rst = m_compare(k, node->first);

				if (cmp_rst < 0)
					n = n->rb_left;
				else if (cmp_rst > 0)
					n = n->rb_right;
				else
					return iterator(n);
			}
			return iterator();
		}
		const_iterator begin() const
		{
			return m_tree.begin();
		}
		const_iterator end() const
		{
			return m_tree.end();
		}
		const_iterator find(const key_type& k) const
		{
			node_type* node;
			struct rb_node_t* n = m_tree.m_root.rb_node;
			while (n)
			{
				node = tree_type::from_rb_node(n);
				int cmp_rst = m_compare(k, node->first);

				if (cmp_rst < 0)
					n = n->rb_left;
				else if (cmp_rst > 0)
					n = n->rb_right;
				else
					return const_iterator(n);
			}
			return const_iterator();
		}
		reverse_iterator rbegin()
		{
			return m_tree.rbegin();
		}
		reverse_iterator rend()
		{
			return m_tree.rend();
		}
		reverse_iterator rfind(const key_type& k)
		{
			node_type* node;
			struct rb_node_t* n = m_tree.m_root.rb_node;
			while (n)
			{
				node = tree_type::from_rb_node(n);
				int cmp_rst = m_compare(k, node->first);

				if (cmp_rst < 0)
					n = n->rb_left;
				else if (cmp_rst > 0)
					n = n->rb_right;
				else
					return reverse_iterator(n);
			}
			return reverse_iterator();
		}
		const_reverse_iterator rbegin() const
		{
			return m_tree.rbegin();
		}
		const_reverse_iterator rend() const
		{
			return m_tree.rend();
		}
		const_reverse_iterator rfind(const key_type& k) const
		{
			node_type* node;
			struct rb_node_t* n = m_tree.m_root.rb_node;
			while (n)
			{
				node = tree_type::from_rb_node(n);
				int cmp_rst = m_compare(k, node->first);

				if (cmp_rst < 0)
					n = n->rb_left;
				else if (cmp_rst > 0)
					n = n->rb_right;
				else
					return const_reverse_iterator(n);
			}
			return const_reverse_iterator();
		}
		iterator insert(const key_type& k, const value_type& v)
		{
			node_type* node = (node_type*)m_alloc.alloc();
			new (node) node_type(k, v);

			rb_node_t** p = &m_tree.m_root.rb_node;
			rb_node_t* parent = NULL;
			node_type* pVal = NULL;

			while(*p)
			{
				parent = *p;
				pVal = tree_type::from_rb_node(parent);

				int n = m_compare(k, pVal->first);
				if(n < 0)
					p = &(*p)->rb_left;
				else if(n > 0)
					p = &(*p)->rb_right;
				else
					return iterator();
			}

			rb_node_t* rb_node = &node->rb_node;
			rb_link_node(rb_node, parent, p);
			m_tree.balance(rb_node);

			m_size += 1;

			return iterator(rb_node);
		}
		iterator insert_unique(const key_type& k, const value_type& v)
		{
			return insert(k, v);
		}
		iterator insert_multi(const key_type& k, const value_type& v)
		{
			node_type* node = (node_type*)m_alloc.alloc();
			new (node) node_type(k, v);

			::System::Collections::rb_node_t** p = &m_tree.m_root.rb_node;
			::System::Collections::rb_node_t* parent = NULL;
			node_type* pVal = NULL;

			while(*p)
			{
				parent = *p;
				pVal = tree_type::from_rb_node(parent);

				int n = m_compare(k, pVal->first);
				if(n < 0)
					p = &(*p)->rb_left;
				else
					p = &(*p)->rb_right;
			}

			::System::Collections::rb_node_t* rb_node = &node->rb_node;
			rb_link_node(rb_node, parent, p);
			m_tree.balance(rb_node);

			m_size += 1;

			return iterator(rb_node);
		}
		bool erase(iterator it)
		{
			if(it == this->end()) return false;
			node_type& node = *it;
			m_tree.erase(it);
			node.~node_type();
			m_alloc.free(&node);
			m_size -= 1;
			return true;
		}
		bool erase(const key_type& k)
		{
			iterator it = this->find(k);
			return this->erase(it);
		}
		bool empty() const
		{
			return m_tree.empty();
		}
		void clear()
		{
			while(!this->empty())
			{
				iterator it = this->begin();
				this->erase(it);
			}
		}
		size_t size() const
		{
			return m_size;
		}
		value_type& operator [] (const key_type& k)
		{
			iterator it = this->find(k);
			if(it != this->end())
				return it->second;

			const value_type& v = _type_default_value_<value_type>::get();
			it = this->insert(k, v);
			ASSERT(it != this->end());

			return it->second;
		}
		const value_type& operator [] (const key_type& k) const
		{
			iterator it = this->find(k);
			if(it != this->end())
				return it->second;
			ASSERT(FALSE);
			return value_type();
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
		typename C = KLessCompare<K> >
	class KMapByTree_pooled
		: public KMapByTree<K,V,C,TreeMapNodePoolAllocator<K,V,step> >
	{
	public:
		typedef KMapByTree<K,V,C,TreeMapNodePoolAllocator<K,V,step> > base_type;
		KMapByTree_pooled()
		{
		}
		KMapByTree_pooled(const KMapByTree_pooled& o):base_type(o)
		{
		}
		KMapByTree_pooled& operator = (const KMapByTree_pooled& o)
		{
			base_type::operator = (o);
			return *this;
		}
	};

	/***
		使用静态内存池的map对象,
		当K,V,step相同时，回使用同一个pool
	 */
	template <
		typename K,
		typename V,
		size_t step = 1024,
		typename C = KLessCompare<K> >
	class KMapByTree_staticPooled
		: public KMapByTree<K,V,C,TreeMapNodePoolAllocator_static<K,V,step> >
	{
	public:
		typedef KMapByTree<K,V,C,TreeMapNodePoolAllocator_static<K,V,step> > base_type;
		KMapByTree_staticPooled()
		{
		}
		KMapByTree_staticPooled(const KMapByTree_staticPooled& o):base_type(o)
		{
		}
		KMapByTree_staticPooled& operator = (const KMapByTree_staticPooled& o)
		{
			base_type::operator = (o);
			return *this;
		}
	};

}} // end namespace ::System::Collections

#endif