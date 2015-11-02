#pragma once

#include "../Collections/KVector.h"

namespace System { namespace Collections {

	/**
	* 实际在Map中存放的数据结构
	*/
	template <typename K,typename V> class KMapNode
	{
	public:
		typedef K key_type;
		typedef V value_type;

		key_type first;
		value_type second;

		KMapNode()
		{
		}
		KMapNode(const KMapNode& o):first(o.first),second(o.second)
		{
		}
		KMapNode(const key_type& key, const value_type& val):first(key),second(val)
		{
		}
	};

	/**
	* 使用KSortedVector来实现Map的功能，如果元素个数在5000以内，应该没有什么效率问题
	* 注意: 传递给分配器的类型参数应该是KMapNode<K,V>，因为是以KMapNode为分配单元
	*/
	template <
		typename K,
		typename V,
		typename C=KLessCompare<K,KLess<K> >,
		typename A=::System::Memory::BlockAllocatorEx<KMapNode<K,V>,32> >
	class KMapByVector2
	{
	public:
		typedef K key_type;
		typedef V value_type;
		typedef A alloc_type;
		typedef KMapNode<K,V> node_type;
		enum {max_idx=0x7ffffffe};

	private:
		class KCompare
		{
		private:
			C m_keyCmp;
		public:
			int operator () (const node_type& a, const node_type& b) const
			{
				return m_keyCmp(a.first, b.first);
			}
		};

	public:
		typedef KCompare cmp_type;

	public:
		class iterator
		{	
		public:
			int m_index;
			KMapByVector2* m_pOwner;
			friend class KMapByVector2;

		private:
			iterator(int index, KMapByVector2* pOwner):m_index(index),m_pOwner(pOwner)
			{
			}

		public:
			iterator():m_index(max_idx),m_pOwner(NULL)
			{
			}
			iterator(const iterator& o):m_index(o.m_index),m_pOwner(o.m_pOwner)
			{
			}
			iterator& operator = (const iterator& o)
			{
				if(this == &o) return *this;
				m_index = o.m_index;
				m_pOwner = o.m_pOwner;
				return *this;
			}
			iterator& operator ++ ()
			{
				m_index += 1;
				if(m_index >= m_pOwner->size())
				{
					m_index = max_idx;
				}
				return *this;
			}
			iterator& operator ++ (int)
			{
				m_index += 1;
				if(m_index >= m_pOwner->size())
				{
					m_index = max_idx;
				}
				return *this;
			}
			node_type& operator * ()
			{
				ASSERT(m_index < m_pOwner->size());
				return m_pOwner->m_vec[m_index];
			}
			node_type* operator -> ()
			{
				ASSERT(m_index < m_pOwner->size());
				return &m_pOwner->m_vec[m_index];
			}
			BOOL operator == (const iterator& o) const
			{
				return m_index == o.m_index;
			}
			BOOL operator != (const iterator& o) const
			{
				return m_index != o.m_index;
			}
		};

		class const_iterator
		{
		public:
			int m_index;
			const KMapByVector2* m_pOwner;
			friend class KMapByVector2;

		private:
			const_iterator(int index, const KMapByVector2* pOwner):m_index(index),m_pOwner(pOwner)
			{
			}
		public:
			const_iterator():m_index(max_idx),m_pOwner(NULL)
			{
			}
			const_iterator(const const_iterator& o):m_index(o.m_index),m_pOwner(o.m_pOwner)
			{
			}
			const_iterator(const iterator& o):m_index(o.m_index),m_pOwner(o.m_pOwner)
			{
			}
			const_iterator& operator = (const const_iterator& o)
			{
				if(this == &o) return *this;
				m_index = o.m_index;
				m_pOwner = o.m_pOwner;
				return *this;
			}
			const_iterator& operator = (const iterator& o)
			{
				if(this == &o) return *this;
				m_index = o.m_index;
				m_pOwner = o.m_pOwner;
				return *this;
			}
			const_iterator& operator ++ ()
			{
				m_index += 1;
				if(m_index >= m_pOwner->size())
				{
					m_index = max_idx;
				}
				return *this;
			}
			const_iterator& operator ++ (int)
			{
				m_index += 1;
				if(m_index >= m_pOwner->size())
				{
					m_index = max_idx;
				}
				return *this;
			}
			const node_type& operator * ()
			{
				ASSERT(m_index < m_pOwner->size());
				return m_pOwner->m_vec[m_index];
			}
			const node_type* operator -> ()
			{
				ASSERT(m_index < m_pOwner->size());
				return &m_pOwner->m_vec[m_index];
			}
			BOOL operator == (const const_iterator& o) const
			{
				return m_index == o.m_index;
			}
			BOOL operator != (const const_iterator& o) const
			{
				return m_index != o.m_index;
			}
		};

	public:
		KSortedVector2<node_type,cmp_type,alloc_type> m_vec;

	public:
		KMapByVector2(int capacity=64,int step=16):m_vec(capacity,step)
		{
		}
		KMapByVector2(alloc_type* al,int capacity=64,int step=16):m_vec(al,capacity,step)
		{
		}
		KMapByVector2& operator = (const KMapByVector2& o)
		{
			if(this == &o) return *this;
			m_vec = o.m_vec;
			return *this;
		}
		BOOL empty() const
		{
			return m_vec.empty();
		}
		int size() const
		{
			return m_vec.size();
		}
		int getCapacity() const
		{
			return m_vec.getCapacity();
		}
		KMapByVector2& clear()
		{
			m_vec.clear();
			return *this;
		}
		iterator insert(const key_type& key, const value_type& val)
		{
			node_type node(key,val);
			int index = m_vec.insert(node);
			if(index < 0) return iterator();
			else return iterator(index, this);
		}
		iterator insert_unique(const key_type& key, const value_type& val)
		{
			node_type node(key,val);
			int index = m_vec.insert_unique(node);
			if(index < 0) return iterator();
			else return iterator(index, this);
		}
		iterator begin()
		{
			if(m_vec.empty()) return iterator();
			else return iterator(0, this);
		}
		iterator end()
		{
			return iterator();
		}
		iterator find(const key_type& key)
		{
			const node_type& node = *(const node_type*)&key;
			int index = m_vec.find(node);
			if(index < 0) return iterator();
			return iterator(index, this);
		}
		iterator mfind(const key_type& key, int* pCount)
		{
			const node_type& node = *(const node_type*)&key;
			int index = m_vec.mfind(node, pCount);
			if(index < 0) return iterator();
			return iterator(index, this);
		}
		const_iterator begin() const
		{
			if(m_vec.empty()) return const_iterator();
			else return const_iterator(0, this);
		}
		const_iterator end() const
		{
			return const_iterator();
		}
		const_iterator find(const key_type& key) const
		{
			const node_type& node = *(const node_type*)&key;
			int index = m_vec.find(node);
			if(index < 0) return const_iterator();
			return const_iterator(index, this);
		}
		const_iterator mfind(const key_type& key, int* pCount) const
		{
			const node_type& node = *(const node_type*)&key;
			int index = m_vec.mfind(node, pCount);
			if(index < 0) return const_iterator();
			return const_iterator(index, this);
		}
		KMapByVector2& erase(const key_type& key)
		{
			iterator it = this->find(key);
			if(it == this->end()) return *this;
			m_vec.erase(it.m_index);
			return *this;
		}
		/**
		* 返回下一个有效的节点
		*
		* 如：删除值为val的所有节点
		*
		*   iterator it = this->begin();
		*   while(it != this->end())
		*    {
		*		if(it->second == val) it = this->erase(it);
		*		else it++;
		*	 }
		*/
		iterator erase(const iterator& it)
		{
			if(it == this->end())
				return this->end();

			m_vec.erase(it.m_index);

			int idx = it.m_index;
			if(idx >= m_vec.size())
			{
				return this->end();
			}
			else
			{
				return it;
			}
		}
		BOOL exists(const key_type& key) const
		{
			return this->find(key) != this->end();
		}
		/**
		* 可以作为左值
		* 如果与键对应的值不存在，那么创建一个新的节点
		* 此时value_type的空构造函数将被调用
		* 注:
		*    这样的操作是低效的，但是可以带来使用上的方便，如
		*    map[key] = value; // 赋值操作符被调用
		*/
		value_type& operator [] (const key_type& key)
		{
			iterator it = this->find(key);
			if(it == this->end())
			{
				node_type node(key, _type_default_value_<value_type>::get());
				int index = m_vec.insert(node);
				ASSERT(index >= 0);
				it = iterator(index, this);
			}
			return it->second;
		}
		const value_type& operator [] (const key_type& key) const
		{
			const_iterator it = this->find(key);
			ASSERT(it != this->end());
			return it->second;
		}
		/**
		* 得到在指定序号上的迭代器
		*/
		iterator at(int idx)
		{
			if(idx < m_vec.size())
				return iterator(idx, this);
			else
				return iterator();
		}
		const_iterator at(int idx) const
		{
			if(idx < m_vec.size())
				return const_iterator(idx, this);
			else
				return const_iterator();
		}
	};

	template <
		typename K,
		typename V,
		typename L=KLess<K>,
		typename A=::System::Memory::BlockAllocatorEx<KMapNode<K,V>,32> >
	class KMapByVector : public KMapByVector2<K,V,KLessCompare<K,L>,A>
	{
	public:
		typedef A alloc_type;
		typedef KMapByVector2<K,V,KLessCompare<K,L>,A> base_type;

	public:
		KMapByVector(int capacity=64,int step=16):base_type(capacity,step)
		{
		}
		KMapByVector(alloc_type* al,int capacity=64,int step=16):base_type(al,capacity,step)
		{
		}
		KMapByVector& operator = (const KMapByVector& o)
		{
			if(this == &o) return *this;
			base_type::operator = (o);
			return *this;
		}
	};

}}