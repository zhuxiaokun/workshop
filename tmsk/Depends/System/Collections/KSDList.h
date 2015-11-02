#ifndef _K_SD_LIST_H_
#define _K_SD_LIST_H_

#include "../KType.h"
#include "../KMacro.h"
#include "../Memory/_heap_alloc.h"
#include "../Sync/KSync.h"

/**
* 单向链表(Single Direction)
*/

namespace System { namespace Collections {

	template <typename T> struct SDListNode
	{
		typedef T value_type;
		value_type m_val;
		SDListNode* m_pNextNode;
	};

	/**
	* 由元素自身构成的单向链表，链表内的元素是T*
	* 类型T必须满足, 有一个public的数据成员m_pNextNode
	*
	* 如:
	* struct TheType {
	*     int x;
	*     TheType* m_pNextNode;
	* };
	*
	* 局限:     从头部删除效率高，任意删除效率很低
	* 适用范围: 不希望由链表自身管理内存，而是由调用者管理元素内存
	*           删除只是从头部, 可用于先进先出，后进先出队列
	*
	*/
	template<typename T, typename lock=::System::Sync::KNoneLock> class KSDSelfList
	{
	private:
		typedef T value_type;
		typedef lock lock_type;

	private: // 不允许拷贝
		KSDSelfList(const KSDSelfList&);
		KSDSelfList& operator = (const KSDSelfList&);

	public:
		KSDSelfList() : m_size(0),m_pHeadNode(NULL),m_pTailNode(NULL)
		{
		}

		// 不管内存，直接清空
		void clear()
		{
			m_mx.Lock();
			m_size = 0;
			m_pHeadNode = m_pTailNode = NULL;
			m_mx.Unlock();
		}

		value_type* pop_front()
		{
			if(!m_pHeadNode) return NULL;

			m_mx.Lock();
			if(!m_pHeadNode)
			{
				m_mx.Unlock();
				return NULL;
			}

			value_type* pNode = (value_type*)m_pHeadNode;
			m_pHeadNode = (volatile value_type*)pNode->m_pNextNode;
			if(!m_pHeadNode) m_pTailNode = NULL;
			pNode->m_pNextNode = NULL;

			m_size -= 1;

			m_mx.Unlock();
			return pNode;
		}

		void push_front(value_type* pNode)
		{
			m_mx.Lock();
			if(!m_pHeadNode)
			{
				pNode->m_pNextNode = NULL;
				m_pHeadNode = m_pTailNode = pNode;
			}
			else
			{
				pNode->m_pNextNode = (value_type*)m_pHeadNode;
				m_pHeadNode = pNode;
			}

			m_size += 1;

			m_mx.Unlock();
		}

		void push_back(value_type* pNode)
		{
			m_mx.Lock();
			pNode->m_pNextNode = NULL;
			if(m_pHeadNode)
			{
				m_pTailNode->m_pNextNode = pNode;
				m_pTailNode = pNode;
			}
			else
			{
				m_pHeadNode = m_pTailNode = pNode;
			}

			m_size += 1;

			m_mx.Unlock();
		}

		// 在提供前节点的情况下，删除下一个节点
		BOOL eraseNext(value_type* prev)
		{
			if(!prev) // 要删除的是第一个节点
			{
				value_type* pHead = this->pop_front();
				return TRUE;
			}
			else
			{
				m_mx.Lock();
				value_type* pNext = prev->m_pNextNode;
				if(!pNext)	// 后面没有节点了
				{
					m_mx.Unlock();
					ASSERT_I(FALSE);
					return TRUE;
				}

				value_type* pNextNext = pNext->m_pNextNode;

				pNext->m_pNextNode = NULL;
				prev->m_pNextNode = pNextNext;

				if(!pNextNext)
				{	// prev变成了最后一个元素
					m_pTailNode = prev;
				}

				m_size -= 1;
				ASSERT_I(m_size >= 0);

				m_mx.Unlock();
				return TRUE;
			}
			return TRUE;
		}

		// 需要遍历整个链表，如果链表过长，效率就会很低
		BOOL erase(value_type* pNode)
		{
			if(!m_pHeadNode) return FALSE;

			m_mx.Lock();
			if(!m_pHeadNode)
			{
				m_mx.Unlock();
				return FALSE;
			}

			value_type* pPrev = NULL;
			value_type* p = (value_type*)m_pHeadNode;
			do
			{
				if(p == pNode)
				{
					if(pPrev)
					{
						//不是第一个元素
						pPrev->m_pNextNode = p->m_pNextNode;
						//删除的是最后一个元素
						if(!pNode->m_pNextNode)
						{
							m_pTailNode = pPrev;
						}
					}
					else
					{
						//是第一个元素
						m_pHeadNode = p->m_pNextNode;
						//只有一个元素，现在空了
						if(!m_pHeadNode) m_pTailNode = NULL;
					}

					m_size -= 1;

					m_mx.Unlock();
					return TRUE;
				}
				pPrev = p;
			}
			while(p = p->m_pNextNode, p);
			m_mx.Unlock();

			pNode->m_pNextNode = NULL;
			return FALSE;
		}

		// 需要遍历整个链表，如果链表过长，效率就会很低
		BOOL exists(value_type* pNode) const
		{
			if(!m_pHeadNode) return FALSE;

			m_mx.Lock();
			if(!m_pHeadNode)
			{
				m_mx.Unlock();
				return FALSE;
			}

			value_type* p = (value_type*)m_pHeadNode;
			do
			{
				if(p == pNode)
				{
					m_mx.Unlock();
					return TRUE;
				}
			}
			while(p = p->m_pNextNode, p);
			m_mx.Unlock();

			return FALSE;
		}

		BOOL empty() const
		{
			return m_pHeadNode == NULL;
		}

		int size() const
		{
			return m_size;
		}

		// 提供遍历的方法
		// 不管锁定，由调用者管理锁定和解锁
		value_type* begin()
		{
			return (value_type*)m_pHeadNode;
		}

		value_type* rbegin()
		{
			return (value_type*)m_pTailNode;
		}

		value_type* next(value_type* pNode)
		{
			return pNode->m_pNextNode;
		}

		void Lock()
		{
			m_mx.Lock();
		}

		void Unlock()
		{
			m_mx.Unlock();
		}

		const value_type* begin() const
		{
			return (const value_type*)m_pHeadNode;
		}

		const value_type* rbegin() const
		{
			return (const value_type*)m_pTailNode;
		}

		const value_type* next(const value_type* pNode) const
		{
			return pNode->m_pNextNode;
		}

		void Lock() const
		{
			lock_type* pMx = (lock_type*)&m_mx;
			pMx->Lock();
		}

		void Unlock() const
		{
			lock_type* pMx = (lock_type*)&m_mx;
			pMx->Unlock();
		}

	protected:
		int m_size;
		volatile value_type* m_pHeadNode;
		value_type* m_pTailNode;
		lock_type m_mx;
	};

	/**
	* 单向链表，由alloc管理内存
	* 存放的元素是T，而非它的指针
	* 需要注意的是分配器的类型参数应该是SDListNode<T>
	* 因为每次分配的内存单元是SDListNode<T>
	*/
	template < typename T, typename L=::System::Sync::KNoneLock, typename A=::System::Memory::HeapAllocator>
	class KSDList
	{
	protected:
		typedef T value_type;
		typedef SDListNode<T> node_type;
		typedef KSDSelfList<node_type,::System::Sync::KNoneLock> list_type;
		typedef L lock_type;
		typedef A alloc_type;

	public:
		KSDList():m_ownAlloc(TRUE),m_al(new alloc_type())
		{
			if(m_al->unitSize())
			{
				ASSERT(m_al->unitSize() >= sizeof(node_type));
			}
		}
		KSDList(alloc_type* al):m_ownAlloc(FALSE),m_al(al)
		{
			if(m_al->unitSize())
			{
				ASSERT(m_al->unitSize() >= sizeof(node_type));
			}
		}
		KSDList(const KSDList& o):m_ownAlloc(o.m_ownAlloc)
		{
			if(m_ownAlloc) m_al = new alloc_type();
			else m_al = o.m_al;

			o.Lock();

			const value_type* p = o.begin();
			while(p)
			{
				this->push_back(*p);
				p = o.next(p);
			}

			o.Unlock();
		}
		~KSDList()
		{
			this->clear();
			if(m_ownAlloc)
			{
				delete m_al;
				m_al = NULL;
			}
		}
		KSDList& operator = (const KSDList& o)
		{
			if(this == &o) return *this;

			this->clear();

			o.Lock();

			const value_type* p = o.begin();
			while(p)
			{
				this->push_back(*p);
				p = o.next(p);
			}

			o.Unlock();

			return *this;
		}
		int size() const
		{
			return m_list.size();
		}
		BOOL empty() const
		{
			return m_list.empty();
		}
		KSDList& clear()
		{
			m_mx.Lock();
			while(!this->empty())
			{
				this->erase_front();
			}
			m_mx.Unlock();
			return *this;
		}
		void Lock() const
		{
			lock_type* pMx = (lock_type*)&m_mx;
			pMx->Lock();
		}
		void Unlock() const
		{
			lock_type* pMx = (lock_type*)&m_mx;
			pMx->Unlock();
		}
		// 不考虑锁定，由调用者保证线程安全
		value_type* begin()
		{
			if(m_list.empty()) return NULL;
			node_type* pNode = m_list.begin();
			return this->ValueFromNode(pNode);
		}
		// 不考虑锁定，由调用者保证线程安全
		value_type* next(value_type* val)
		{
			node_type* pNode = this->NodeFromValue(val);
			pNode = pNode->m_pNextNode;
			if(!pNode) return NULL;
			return this->ValueFromNode(pNode);
		}
		// 不考虑锁定，由调用者保证线程安全
		const value_type* begin() const
		{
			if(m_list.empty()) return NULL;
			const node_type* pNode = m_list.begin();
			return this->ValueFromNode(pNode);
		}
		// 不考虑锁定，由调用者保证线程安全
		const value_type* next(const value_type* val) const
		{
			const node_type* pNode = this->NodeFromValue(val);
			pNode = pNode->m_pNextNode;
			if(!pNode) return NULL;
			return this->ValueFromNode(pNode);
		}

		// 不保护，由调用者保证线程安全
		// 删除链表中的元素，需要提供前面的节点
		void eraseNext(value_type* prev)
		{
			node_type* pNextNode = NULL;
			node_type* pPrevNode = this->NodeFromValue(prev);

			if(pPrevNode)
			{
				pNextNode = pPrevNode->m_pNextNode;
			}

			m_list.eraseNext(pPrevNode);
			if(pNextNode)
			{
				pNextNode->m_val.~value_type();
				m_al->free(pNextNode);
			}

			return;
		}

		/**
		* 不考虑锁定，由调用者保证线程安全
		*/
		void erase_front()
		{
			if(m_list.empty()) return;
			node_type* pNode = m_list.pop_front();
			value_type* pVal = this->ValueFromNode(pNode);
			pVal->~value_type();
			m_al->free(pNode);
		}
		/**
		* 取出链表的第一个元素，并且把第一个元素从链表头部删除
		* 注: 这是一个低效的操作，仅仅是为了提供一个简单类型方便的操作
		*     它需要元素有空构造函数，拷贝构造函数，赋值操作符
		*     如果链表空，空构造函数将被调用
		*     如果链表不空，拷贝构造函数和赋值操作符将被调用
		*     对于复杂的类型，建议不使用此方法，而是直接使用begin()
		*/
		value_type pop_front(BOOL* pbSuccess=NULL)
		{
			if(m_list.empty())
			{
				if(pbSuccess) *pbSuccess = FALSE;
				return value_type();
			}

			m_mx.Lock();

			if(m_list.empty())
			{
				m_mx.Unlock();
				if(pbSuccess) *pbSuccess = FALSE;
				return value_type();
			}

			node_type* pNode = m_list.pop_front();
			value_type* pVal = this->ValueFromNode(pNode);

			value_type val(*pVal);
			pVal->~value_type();

			m_al->free(pNode);
			m_mx.Unlock();

			if(pbSuccess) *pbSuccess = TRUE;
			return val;
		}
		void safe_earse_front()
		{
			m_mx.Lock();
			this->erase_front();
			m_mx.Unlock();
		}
		KSDList& push_back(const value_type& val)
		{
			m_mx.Lock();

			node_type* pNode = (node_type*)m_al->alloc(sizeof(node_type));
			ASSERT(pNode);
			new (&pNode->m_val) value_type(val);

			m_list.push_back(pNode);

			m_mx.Unlock();
			return *this;
		}
		KSDList& push_front(const value_type& val)
		{
			m_mx.Lock();

			node_type* pNode = (node_type*)m_al->alloc(sizeof(node_type));
			ASSERT(pNode);
			new (&pNode->m_val) value_type(val);
			m_list.push_front(pNode);

			m_mx.Unlock();
			return *this;
		}

	protected:
		value_type* ValueFromNode(node_type* node)
		{
			return &node->m_val;
		}
		node_type* NodeFromValue(value_type* val)
		{
			return (node_type*)val;
		}

		const value_type* ValueFromNode(const node_type* node) const
		{
			return &node->m_val;
		}
		const node_type* NodeFromValue(const value_type* val) const
		{
			return (const node_type*)val;
		}

	protected:
		lock_type m_mx;
		list_type m_list;

		BOOL m_ownAlloc;
		alloc_type* m_al;
	};

}}

#endif
