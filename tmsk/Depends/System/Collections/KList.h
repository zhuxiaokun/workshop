/********************************************************************
	created:	2007/10/22
	created:	22:10:2007   11:15
	filename: 	KList.h
	file ext:	h
	author:		xuqin
	
	purpose:	
*********************************************************************/
#pragma once
#include "../KType.h"
#include "../KMacro.h"
#include "DynArray.h"
#include "../Memory/KAlloc.h"
#include "../Memory/KStepObjectPool.h"
#include"../Sync/KSync.h"

#define DECLARE_NODE_GETTER(NodeClass,nodeMember,clsName) \
class clsName \
	{ \
	public: \
	NodeClass*& prevNode(NodeClass* pNode) \
		{ \
		return pNode->nodeMember.m_prevNode; \
		} \
		NodeClass*& nextNode(NodeClass* pNode) \
		{ \
		return pNode->nodeMember.m_nextNode; \
		} \
		void clearNode(NodeClass* pNode) \
		{ \
		memset(&pNode->nodeMember, 0, sizeof(pNode->nodeMember)); \
		} \
	};

namespace System { namespace Collections {

template <typename T> struct SelfListNode
{
	T* m_prevNode;
	T* m_nextNode;
	///
	SelfListNode():m_prevNode(NULL),m_nextNode(NULL)
	{

	}
	bool empty() const
	{
		return m_prevNode == NULL && m_nextNode == NULL;
	}
	void reset()
	{
		m_prevNode = m_nextNode = NULL;
	}
};

template <typename T> struct ListNode
{
	//----------------------------------------------

	T val;
	ListNode* m_prevNode;
	ListNode* m_nextNode;

	// ---------------------------------------------

	ListNode() : val(_type_default_value_<T>::get())
		, m_prevNode(NULL)
		, m_nextNode(NULL)
	{
	}
	ListNode(const T& v):val(v)
		, m_prevNode(NULL)
		, m_nextNode(NULL)
	{
	}
	ListNode& operator = (const T& v)
	{
		if(v == &val) return *this;
		val = v;
		return *this;
	}
};

// add by Augustine 3/26/2012 for Function
// 缺省的链表结点前后指针的默认获取器

template <typename T>
class KListNodePointerGetter
{
private:
	typedef T value_type;
public:
	inline value_type* &prevNode(value_type* pNode){return pNode->m_prevNode; }
	inline value_type* &nextNode(value_type* pNode){return pNode->m_nextNode; }
	inline void clearNode(value_type* pNode){pNode->m_prevNode = NULL ; pNode->m_nextNode = NULL;}
};


template <typename T> class KHeapListNodeAllocator
{
private:
	typedef T value_type;
	typedef ListNode<T> node_type;
	typedef ::System::Memory::HeapAllocator alloc_type;
	alloc_type m_alloc;

public:
	node_type* Alloc()
	{
		node_type* node = (node_type*)m_alloc.alloc(sizeof(node_type));
		new (node) node_type;
		node->m_prevNode = node->m_nextNode = NULL;
		return node;
	}
	void Free(node_type* node)
	{
		(node)->~node_type();
		m_alloc.free(node);
	}
};

template <typename T,size_t step,typename L=JG_S::KNoneLock> class KStaticListNodePool
{
private:
	typedef T value_type;
	typedef L lock_type;
	typedef ListNode<T> node_type;
	typedef ::System::Memory::KStepObjectPool<node_type,step,L> pool_type;

private:
	static pool_type& _getpool()
	{
		static lock_type mx;
		static pool_type* pool = NULL;
		if(pool) return *pool;
		mx.Lock();
		if(!pool) pool = new pool_type();
		mx.Unlock();
		return *pool;
	}
public:
	node_type* Alloc()
	{
		pool_type& pool = _getpool();
		node_type* node = pool.Alloc();
		node->m_prevNode = node->m_nextNode = NULL;
		return node;
	}
	void Free(node_type* node)
	{
		pool_type& pool = _getpool();
		pool.Free(node);
	}
};

// change by Augustine 3/26/2012
// 保证一个结点可以插入多个链表
/**
 * 自带前后指针构成的双向链表
 * m_prevNode, m_nextNode
 */
template <typename T,typename G = KListNodePointerGetter<T> > 
class KSelfList
{
public:
	typedef T node_type;

public:
	node_type* m_head;
	node_type* m_tail;

protected:
	G m_NodePointerGetter;
public:
	KSelfList():m_head(NULL),m_tail(NULL)
	{

	}
	~KSelfList()
	{

	}

public:
	bool empty() const
	{
		return m_head == NULL;
	}
	size_t size() const
	{
		size_t n = 0;
		node_type* p = m_head;
		while(p) 
		{ 
			n++; 
			p = ((G*)(&m_NodePointerGetter))->nextNode(p);
			//p = p->m_nextNode; 
		}
		return n;
	}
	void clear()
	{
		m_head = m_tail = NULL;
	}

	void clearAllNode()
	{
		node_type* p = m_head;
		node_type* pOld = NULL;
		while(NULL != p)
		{
			pOld = p;
			p = m_NodePointerGetter.nextNode(p);
			m_NodePointerGetter.clearNode(pOld);
		}
		m_head = m_tail = NULL;
	}

public:
	KSelfList& push_front(node_type* node)
	{
		ASSERT(NULL == m_NodePointerGetter.prevNode(node));
		ASSERT(NULL == m_NodePointerGetter.nextNode(node));
		if(m_head)
		{
			//node->m_nextNode = m_head;
			//m_head->m_prevNode = node;
			

			m_NodePointerGetter.nextNode(node) = m_head;
			m_NodePointerGetter.prevNode(m_head) = node;
			m_head = node;
		}
		else
		{
			m_head = m_tail = node;
		}
		return *this;
	}
	KSelfList& push_back(node_type* node)
	{
		ASSERT(NULL == m_NodePointerGetter.prevNode(node));
		ASSERT(NULL == m_NodePointerGetter.nextNode(node));
		if(m_tail)
		{
			//m_tail->m_nextNode = node;
			//node->m_prevNode = m_tail;
			m_NodePointerGetter.nextNode(m_tail) = node;
			m_NodePointerGetter.prevNode(node) = m_tail;
			m_tail = node;
		}
		else
		{
			m_head = m_tail = node;
		}
		return *this;
	}

public:
	node_type* pop_front()
	{
		if(!m_head) 
			return NULL;
		node_type* node = m_head;
		//m_head = m_head->m_nextNode;
		//node->m_prevNode = NULL;
		//node->m_nextNode = NULL;
		m_head = m_NodePointerGetter.nextNode(m_head);
		m_NodePointerGetter.clearNode(node);
		
		if(m_head) 
		{
			//m_head->m_prevNode = NULL;
			m_NodePointerGetter.prevNode(m_head) = NULL;
		}
		else 
			m_tail = NULL;
		return node;
	}
	node_type* pop_back()
	{
		if(!m_tail) return NULL;
		node_type* node = m_tail;
		//m_tail = m_tail->m_prevNode;
		//node->m_prevNode = NULL;
		//node->m_nextNode = NULL;
		m_tail = m_NodePointerGetter.prevNode(m_tail);
		m_NodePointerGetter.clearNode(node);

		if(m_tail) 
		{
			//m_tail->m_nextNode = NULL;
			m_NodePointerGetter.nextNode(m_tail) = NULL;
		}
		else 
			m_head = NULL;
		return node;
	}

public:
	bool contain(const node_type* node) const
	{
		const node_type* p = m_head;
		while(p)
		{
			if(p == node) 
				return true;
			//p = p->m_nextNode;
			p = ((KSelfList*)this)->m_NodePointerGetter.nextNode((node_type*)p);
		}
		return false;
	}
	bool erase(node_type* node)
	{
		/*node_type* prev = node->m_prevNode;
		node_type* next = node->m_nextNode;
		if(prev) prev->m_nextNode = next;
		if(next) next->m_prevNode = prev;
		*/
		
		node_type* prev = m_NodePointerGetter.prevNode(node); 
		node_type* next = m_NodePointerGetter.nextNode(node);
		if(prev)
		{
			m_NodePointerGetter.nextNode(prev)= next;
		}
		if(next)
		{
			m_NodePointerGetter.prevNode(next) = prev;
		}
		if(!prev)
		{
			ASSERT(node == m_head);
			m_head = next;
		}
		if(!next)
		{
			ASSERT(node == m_tail);
			m_tail = prev;
		}
		
		//node->m_prevNode = NULL;
		//node->m_prevNode = NULL;
		m_NodePointerGetter.clearNode(node);
		return true;
	}
	bool insert(node_type* loc, node_type* node)
	{
		ASSERT(loc);
		/*ASSERT(!node->m_prevNode);
		ASSERT(!node->m_nextNode);*/
		ASSERT(NULL == m_NodePointerGetter.prevNode(node));
		ASSERT(NULL == m_NodePointerGetter.nextNode(node));

		//node_type* prev = loc->m_prevNode;
		node_type* prev = m_NodePointerGetter.prevNode(loc);
		node_type* next = loc;

		/*node->m_prevNode = prev;
		if(prev) prev->m_nextNode = node;

		node->m_nextNode = next;
		next->m_prevNode = node;*/
		
		m_NodePointerGetter.prevNode(node) = prev;
		if(NULL != prev)
		{
			m_NodePointerGetter.nextNode(prev) = node;
		}
		
		m_NodePointerGetter.nextNode(node) = next;
		m_NodePointerGetter.prevNode(next) = node;

		if(!prev) 
			m_head = node;
		return true;
	}

public:
	node_type* begin()
	{
		return m_head;
	}
	node_type* next(node_type* node)
	{
		//return node->m_nextNode;
		return m_NodePointerGetter.nextNode(node);
	}
	node_type* prev(node_type* node)
	{
		//return node->m_prevNode;
		return m_NodePointerGetter.prevNode(node);
	}

public:
	const node_type* begin() const
	{
		return m_head;
	}
	const node_type* next(node_type* node) const
	{
		//return node->m_nextNode;
		return m_NodePointerGetter.nextNode(node);
	}
	const node_type* prev(node_type* node) const
	{
		//return node->m_prevNode;
		return m_NodePointerGetter.prevNode(node);
	}

public:
	node_type* rbegin()
	{
		return m_tail;
	}
	const node_type* rbegin() const
	{
		return m_tail;
	}

};

template <typename T, typename A=KHeapListNodeAllocator<T> > class KList
	: private KSelfList< ListNode<T> >
{
protected:
	typedef T value_type;
	typedef A alloc_type;
	typedef ListNode<T> node_type;
	typedef KSelfList<node_type> base_type;

public:
	class reverse_iterator;
	class iterator
	{
	public:
		node_type* m_node;

	public:
		iterator():m_node(NULL)
		{

		}
		iterator(const iterator& o):m_node(o.m_node)
		{

		}
		iterator(const reverse_iterator& o):m_node(o.m_node)
		{

		}
		iterator(node_type* node):m_node(node)
		{
		}
		iterator& operator = (const iterator& o)
		{
			m_node = o.m_node;
			return *this;
		}
		value_type& operator * ()
		{
			return m_node->val;
		}
		value_type* operator ->()
		{
			return &m_node->val;
		}
		iterator& operator ++ ()
		{
			if(!m_node) return *this;
			m_node = m_node->m_nextNode;
			return *this;
		}
		iterator& operator ++ (int)
		{
			if(!m_node) return *this;
			m_node = m_node->m_nextNode;
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
	};	
	class reverse_iterator
	{
	public:
		node_type* m_node;

	public:
		reverse_iterator():m_node(NULL)
		{

		}
		reverse_iterator(const iterator& o):m_node(o.m_node)
		{

		}
		reverse_iterator(const reverse_iterator& o):m_node(o.m_node)
		{

		}
		reverse_iterator(node_type* node):m_node(node)
		{
		}
		reverse_iterator& operator = (const reverse_iterator& o)
		{
			m_node = o.m_node;
			return *this;
		}
		value_type& operator * ()
		{
			return m_node->val;
		}
		value_type* operator ->()
		{
			return &m_node->val;
		}
		reverse_iterator& operator ++ ()
		{
			if(!m_node) return *this;
			m_node = m_node->m_prevNode;
			return *this;
		}
		reverse_iterator& operator ++ (int)
		{
			if(!m_node) return *this;
			m_node = m_node->m_prevNode;
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
	};	
	class const_iterator
	{
	public:
		const node_type* m_node;

	public:
		const_iterator():m_node(NULL)
		{

		}
		const_iterator(const const_iterator& o):m_node(o.m_node)
		{

		}
		const_iterator(const iterator& o):m_node(o.m_node)
		{

		}
		const_iterator(const node_type* node):m_node(node)
		{
		}
		const_iterator& operator = (const iterator& o)
		{
			m_node = o.m_node;
			return *this;
		}
		const_iterator& operator = (const const_iterator& o)
		{
			m_node = o.m_node;
			return *this;
		}
		const value_type& operator * ()
		{
			return m_node->val;
		}
		const value_type* operator ->()
		{
			return &m_node->val;
		}
		const_iterator& operator ++ ()
		{
			if(!m_node) return *this;
			m_node = m_node->m_nextNode;
			return *this;
		}
		const_iterator& operator ++ (int)
		{
			if(!m_node) return *this;
			m_node = m_node->m_nextNode;
			return *this;
		}
		bool operator == (const iterator& o) const
		{
			return m_node == o.m_node;
		}
		bool operator == (const const_iterator& o) const
		{
			return m_node == o.m_node;
		}
		bool operator != (const iterator& o) const
		{
			return m_node != o.m_node;
		}
		bool operator != (const const_iterator& o) const
		{
			return m_node != o.m_node;
		}
	};	
	class const_reverse_iterator
	{
	public:
		const node_type* m_node;

	public:
		const_reverse_iterator():m_node(NULL)
		{

		}
		const_reverse_iterator(const const_reverse_iterator& o):m_node(o.m_node)
		{

		}
		const_reverse_iterator(const reverse_iterator& o):m_node(o.m_node)
		{

		}
		const_reverse_iterator(const node_type* node):m_node(node)
		{
		}
		const_reverse_iterator& operator = (const reverse_iterator& o)
		{
			m_node = o.m_node;
			return *this;
		}
		const_reverse_iterator& operator = (const const_reverse_iterator& o)
		{
			m_node = o.m_node;
			return *this;
		}
		const value_type& operator * ()
		{
			return m_node->val;
		}
		const value_type* operator ->()
		{
			return &m_node->val;
		}
		const_reverse_iterator& operator ++ ()
		{
			if(!m_node) return *this;
			m_node = m_node->m_prevNode;
			return *this;
		}
		const_reverse_iterator& operator ++ (int)
		{
			if(!m_node) return *this;
			m_node = m_node->m_prevNode;
			return *this;
		}
		bool operator == (const reverse_iterator& o) const
		{
			return m_node == o.m_node;
		}
		bool operator == (const const_reverse_iterator& o) const
		{
			return m_node == o.m_node;
		}
		bool operator != (const reverse_iterator& o) const
		{
			return m_node != o.m_node;
		}
		bool operator != (const const_reverse_iterator& o) const
		{
			return m_node != o.m_node;
		}
	};	

protected:
	alloc_type m_alloc;
	int m_iSize;

public:
	KList():m_iSize(0)  { }
	~KList() { this->clear(); }

public:
	bool empty() const
	{
		return base_type::empty();
	}
	size_t size() const
	{
		return m_iSize;
		//return base_type::size();
	}
	void clear()
	{
		node_type* node = base_type::begin();
		while(node)
		{
			node_type* cur_node = node;
			node = base_type::next(node);
			m_alloc.Free(cur_node);
		}
		base_type::clear();
		m_iSize = 0;
	}

public:
	KList& push_front(const value_type& val)
	{
		node_type* node = m_alloc.Alloc();
		node->val = val;
		base_type::push_front(node);
		++m_iSize;
		return *this;
	}
	KList& push_back(const value_type& val)
	{
		node_type* node = m_alloc.Alloc();
		node->val = val;
		base_type::push_back(node);
		++m_iSize;
		return *this;
	}

public:
	void pop_front()
	{
		node_type* node = base_type::pop_front();
		if(!node) return;
		m_alloc.Free(node);
		--m_iSize;
	}
	void pop_back()
	{
		node_type* node = base_type::pop_back();
		if(!node) return ;
		m_alloc.Free(node);
		--m_iSize;
	}

	value_type& back()
	{
		ASSERT( !empty() );
		return base_type::m_tail->val;
	}
	const value_type& back() const
	{
		ASSERT( !empty() );
		return base_type::m_tail->val;
	}

	value_type& front()
	{
		ASSERT( !empty() );
		node_type* node = base_type::begin();
		return node->val;
	}
	const value_type& front() const
	{
		ASSERT( !empty() );
		node_type* node = base_type::begin();
		return node->val;
	}

public:
	bool contain(const value_type& val) const
	{
		const node_type* n = base_type::begin();
		while(n)
		{
			if(n->val == val) return true;
			n = n->m_nextNode;
		}
		return false;
	}
	iterator erase(iterator it)
	{
		node_type* node = it.m_node;
		if(!node) return NULL;
		node_type* ret = node->m_nextNode;
		base_type::erase(node);
		m_alloc.Free(node);
		--m_iSize;
		return iterator(ret);
	}
	iterator insert( iterator itPos, const value_type& val )
	{
		node_type* node = m_alloc.Alloc();
		node->val = val;
		base_type::insert( itPos.m_node, node );
		++m_iSize;
		return iterator(node);
	}

public:
	iterator begin()
	{
		return iterator(base_type::begin());
	}
	const_iterator begin() const
	{
		return const_iterator(base_type::begin());
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
		return reverse_iterator(base_type::rbegin());
	}
	const_reverse_iterator rbegin() const
	{
		return const_reverse_iterator(base_type::rbegin());
	}
	reverse_iterator rend()
	{
		return reverse_iterator();
	}
	const_reverse_iterator rend() const
	{
		return const_reverse_iterator();
	}
};

template <typename T,size_t step> class KList_pooled
: public KList< T,KStaticListNodePool<T,step,::System::Sync::KNoneLock> >
{
};

template <typename T,size_t step> class KList_MT_pooled
	: public KList< T,KStaticListNodePool<T,step,::System::Sync::KMTLock> >
{
};

/**
 * 通用双向循环链表 by Allen, 2012.8.30
 */
template <typename T>
class KBiCycleListNode
{
public:
	T m_data;
private:
	KBiCycleListNode<T>* m_prev;
	KBiCycleListNode<T>* m_next;

public:
	explicit KBiCycleListNode(T data) : m_data(data)
	{
		m_prev = this;
		m_next = this;
	}
	/**
	 * 从当前列表中断开
	 */
	inline void Detach()
	{
		m_prev->m_next = m_next;
		m_next->m_prev = m_prev;
		m_prev = this;
		m_next = this;
	}
	/**
	 * 将节点从原列表移除，并且移入本节点的前面
	 */
	inline void MoveToFront(KBiCycleListNode<T>* other)
	{
		// 保护一下
		ASSERT_RETURN_VOID(other && this != other);
		other->m_next->m_prev = other->m_prev;
		other->m_prev->m_next = other->m_next;
		other->m_next = this;
		other->m_prev = m_prev;
		m_prev->m_next = other;
		m_prev = other;
	}
	/**
	 * 将节点从原列表移除，并且移入本节点的后面
	 */
	inline void MoveToBack(KBiCycleListNode<T>* other)
	{
		// 保护一下
		ASSERT_RETURN_VOID(other && this != other);
		other->m_next->m_prev = other->m_prev;
		other->m_prev->m_next = other->m_next;
		other->m_next = m_next;
		other->m_prev = this;
		m_next->m_prev = other;
		m_next = other;
	}
	/**
	 * 得到前面的节点
	 */
	inline KBiCycleListNode<T>* GetPrev()
	{
		return m_prev;
	}
	/**
	 * 得到后面的节点
	 */
	inline KBiCycleListNode<T>* GetNext()
	{
		return m_next;
	}
};


} /* Collections */ } /* System */