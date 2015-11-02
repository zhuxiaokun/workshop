#pragma once

#include "DynArray.h"
#include "KMapByVector.h"
#include "../Memory/KAlloc.h"
#include "../Sync/KSync.h"

namespace System { namespace Collections {

	template <
		typename K,
		typename V,
		typename C=KLessCompare<K,KLess<K> >,
		typename H=KHasher<K>,
		typename L=::System::Sync::KNoneLock,
		int buckets=1024,
		int initCapacity=1024*8>
	class KHashTable
	{
	public:
		enum { init_bucket_capacity = initCapacity/buckets };
		enum { bucket_inflate_step = init_bucket_capacity  };
		enum { alloc_inflate_step = initCapacity/2 };

	public:	
		typedef K key_type;
		typedef V value_type;
		typedef C cmp_type;
		typedef H hash_type;
		typedef L lock_type;

	public:
		typedef KMapNode<K,V> node_type;
		typedef ::System::Memory::BlockAllocatorEx<node_type,alloc_inflate_step> alloc_type;
		typedef KMapByVector2<K,V,C,alloc_type> map_type;

	public:
		int m_size;
		lock_type m_mx;
		alloc_type m_alloc;
		hash_type m_hash;
		map_type** m_ppMap;

	public:
		class iterator
		{
		public:
			int m_bucket;
			typename map_type::iterator m_it;
			KHashTable* m_pOwner;

		public:
			iterator():m_bucket(0),m_pOwner(NULL)
			{

			}
			iterator(const iterator& o):m_bucket(o.m_bucket),m_it(o.m_it),m_pOwner(o.m_pOwner)
			{

			}
			iterator(KHashTable* pOwner, int bucket, typename map_type::iterator& mit):m_pOwner(pOwner),m_bucket(bucket),m_it(mit)
			{

			}
			~iterator()
			{

			}
			iterator& operator = (const iterator& o)
			{
				if(this == &o) return *this;
				m_bucket = o.m_bucket;
				m_it = o.m_it;
				m_pOwner = o.m_pOwner;
				return *this;
			}
			node_type& operator * ()
			{
				ASSERT(m_pOwner);
				map_type* pMap = m_pOwner->m_ppMap[m_bucket];
				return *m_it;
			}
			node_type* operator -> ()
			{
				if(!m_pOwner) return NULL;
				map_type* pMap = m_pOwner->m_ppMap[m_bucket];
				if(m_it == pMap->end()) return NULL;
				return m_it.operator->();
			}
			iterator& operator ++ ()
			{
				if(!m_pOwner) return *this;

				map_type* pMap = m_pOwner->m_ppMap[m_bucket];

				m_it++;
				if(m_it == pMap->end())
				{
					while(TRUE)
					{
						m_bucket += 1;

						if(m_bucket >= buckets)
						{
							m_bucket = 0;
							m_pOwner = NULL;
							return *this;
						}

						pMap = m_pOwner->m_ppMap[m_bucket];
						m_it = pMap->begin();
						if(m_it == pMap->end()) continue;

						return *this;
					}
				}

				return *this;
			}
			iterator& operator ++ (int)
			{
				if(!m_pOwner) return *this;

				map_type* pMap = m_pOwner->m_ppMap[m_bucket];

				m_it++;
				if(m_it == pMap->end())
				{
					while(TRUE)
					{
						m_bucket += 1;

						if(m_bucket >= buckets)
						{
							m_bucket = 0;
							m_pOwner = NULL;
							return *this;
						}

						pMap = m_pOwner->m_ppMap[m_bucket];

						m_it = pMap->begin();
						if(m_it == pMap->end()) continue;

						return *this;
					}
				}
				return *this;
			}
			BOOL operator == (const iterator& o) const
			{
				if(!m_pOwner) return o.m_pOwner == NULL;
				if(m_bucket != o.m_bucket) return FALSE;
				return m_it == o.m_it;
			}
			BOOL operator != (const iterator& o) const
			{
				return !this->operator==(o);
			}
		};
		class const_iterator
		{
		public:
			int m_bucket;
			typename map_type::const_iterator m_it;
			const KHashTable* m_pOwner;

		public:
			const_iterator():m_bucket(0),m_pOwner(NULL)
			{

			}
			const_iterator(const const_iterator& o):m_bucket(o.m_bucket),m_it(o.m_it),m_pOwner(o.m_pOwner)
			{

			}
			const_iterator(const iterator& o):m_bucket(o.m_bucket),m_it(o.m_it),m_pOwner(o.m_pOwner)
			{

			}
			const_iterator(const KHashTable* pOwner, int bucket, typename map_type::const_iterator& mit):m_pOwner(pOwner),m_bucket(bucket),m_it(mit)
			{

			}
			~const_iterator()
			{

			}
			const_iterator& operator = (const const_iterator& o)
			{
				if(this == &o) return *this;
				m_bucket = o.m_bucket;
				m_it = o.m_it;
				m_pOwner = o.m_pOwner;
				return *this;
			}
			const_iterator& operator = (const iterator& o)
			{
				if(this == &o) return *this;
				m_bucket = o.m_bucket;
				m_it = o.m_it;
				m_pOwner = o.m_pOwner;
				return *this;
			}
			const node_type& operator * ()
			{
				ASSERT(m_pOwner);
				const map_type* pMap = m_pOwner->m_ppMap[m_bucket];
				return *m_it;
			}
			const node_type* operator -> ()
			{
				if(!m_pOwner) return NULL;
				const map_type* pMap = m_pOwner->m_ppMap[m_bucket];
				if(m_it == pMap->end()) return NULL;
				return m_it.operator->();
			}
			const_iterator& operator ++ ()
			{
				if(!m_pOwner) return *this;

				const map_type* pMap = m_pOwner->m_ppMap[m_bucket];

				m_it++;
				if(m_it == pMap->end())
				{
					while(TRUE)
					{
						m_bucket += 1;

						if(m_bucket >= buckets)
						{
							m_bucket = 0;
							m_pOwner = NULL;
							return *this;
						}

						pMap = m_pOwner->m_ppMap[m_bucket];
						if(pMap->empty()) continue;

						m_it = pMap->begin();
						return *this;
					}
				}

				return *this;
			}
			const_iterator& operator ++ (int)
			{
				if(!m_pOwner) return *this;

				const map_type* pMap = m_pOwner->m_ppMap[m_bucket];

				m_it++;
				if(m_it == pMap->end())
				{
					while(TRUE)
					{
						m_bucket += 1;

						if(m_bucket >= buckets)
						{
							m_bucket = 0;
							m_pOwner = NULL;
							return *this;
						}

						pMap = m_pOwner->m_ppMap[m_bucket];
						if(pMap->empty()) continue;

						m_it = pMap->begin();
						return *this;
					}
				}
				return *this;
			}
			BOOL operator == (const const_iterator& o) const
			{
				if(!m_pOwner) return o.m_pOwner == NULL;
				if(m_bucket != o.m_bucket) return FALSE;
				return m_it == o.m_it;
			}
			BOOL operator != (const const_iterator& o) const
			{
				return !this->operator==(o);
			}
		};

	public:
		KHashTable():m_size(0)
		{
			m_ppMap = new map_type*[buckets];
			memset(m_ppMap, 0, buckets*sizeof(map_type*));

			int cap = init_bucket_capacity;
			int capStep = bucket_inflate_step;
			for(int i=0; i<buckets; i++)
			{	
				m_ppMap[i] = new map_type(&m_alloc, cap, capStep);
			}
		}
		KHashTable(const KHashTable& o):m_size(0)
		{
			m_ppMap = new map_type*[buckets];
			memset(m_ppMap, 0, buckets*sizeof(map_type*));
			for(int i=0; i<buckets; i++)
			{
				const map_type* pMap = o.get_bucket(i);

				int cap = pMap->getCapacity();
				int capStep = bucket_inflate_step;

				map_type*& pMyMap = m_ppMap[i];
				pMyMap = new map_type(&m_alloc, cap, capStep);
				*pMyMap = *pMap;
			}
			m_size = o.m_size;
		}
		~KHashTable()
		{
			if(!m_ppMap) return;
			for(int i=0; i<buckets; i++)
			{
				map_type* pMap = m_ppMap[i];
				m_size -= pMap->size();
				delete pMap;
				m_ppMap[i] = NULL;
			}
			delete [] m_ppMap;
			m_ppMap = NULL;
		}
		KHashTable& operator = (const KHashTable& o)
		{
			if(this == &o) return *this;
			this->clear();
			for(int i=0; i<buckets; i++)
			{
				const map_type* pMap = o.get_bucket(i);
				map_type* pMyMap = m_ppMap[i];
				*pMyMap = *pMap;
				m_size += pMap->size();
			}
			return *this;
		}
		BOOL empty() const
		{
			return m_size < 1;
		}
		int size() const
		{
			return m_size;
		}
		int getCapacity() const
		{
			int cap = 0;
			for(int i=0; i<buckets; i++)
			{
				const map_type* pMap = m_ppMap[i];
				cap += pMap->getCapacity();
			}
			return cap;
		}
		void clear()
		{
			::System::Sync::KAutoLock<lock_type> mx(m_mx);

			for(int i=0; i<buckets; i++)
			{
				map_type* pMap = m_ppMap[i];
				pMap->clear();
			}
			m_size = 0;
		}
		iterator begin()
		{
			if(m_size < 1) return iterator();
			int bucket = 0;
			while(TRUE)
			{
				map_type* pMap = m_ppMap[bucket];
				if(pMap->empty())
				{
					bucket += 1;
					continue;
				}
				typename map_type::iterator mit = pMap->begin();
				return iterator(this, bucket, mit);
			}
			ASSERT_I(FALSE);
			return iterator();
		}
		iterator end()
		{
			return iterator();
		}
		iterator find(const key_type& k)
		{
			::System::Sync::KAutoLock<lock_type> mx(m_mx);

			int hashCode = m_hash(k);
			int bucket = hashCode % buckets;
			map_type* pMap = m_ppMap[bucket];
			if(pMap->empty()) return this->end();
			typename map_type::iterator mit = pMap->find(k);
			if(mit == pMap->end()) return this->end();
			return iterator(this, bucket, mit);
		}
		iterator mfind(const key_type& k, int* pCount)
		{
			::System::Sync::KAutoLock<lock_type> mx(m_mx);

			int hashCode = m_hash(k);
			int bucket = hashCode % buckets;
			map_type* pMap = m_ppMap[bucket];
			if(pMap->empty()) return this->end();
			typename map_type::iterator mit = pMap->mfind(k, pCount);
			if(mit == pMap->end()) return this->end();
			return iterator(this, bucket, mit);
		}
		value_type& operator [] (const key_type& k)
		{
			::System::Sync::KAutoLock<lock_type> mx(m_mx);

			int hashCode = m_hash(k);
			int bucket = hashCode % buckets;
			map_type* pMap = m_ppMap[bucket];

			int sz = m_size - pMap->size();
			value_type& retVal = pMap->operator[](k);
			m_size = sz + pMap->size();
			
			return retVal;
		}
		const_iterator begin() const
		{
			if(m_size < 1) return const_iterator();
			int bucket = 0;
			while(TRUE)
			{
				const map_type* pMap = m_ppMap[bucket];
				if(pMap->empty())
				{
					bucket += 1;
					continue;
				}
				typename map_type::const_iterator mit = pMap->begin();
				return const_iterator(this, bucket, mit);
			}
			ASSERT_I(FALSE);
			return const_iterator();
		}
		const_iterator end() const
		{
			return const_iterator();
		}
		const_iterator find(const key_type& k) const
		{
			::System::Sync::KAutoLock<lock_type> mx(m_mx);

			int hashCode = m_hash(k);
			int bucket = hashCode % buckets;
			const map_type* pMap = m_ppMap[bucket];
			if(pMap->empty()) return this->end();
			typename map_type::const_iterator mit = pMap->find(k);
			if(mit == pMap->end()) return this->end();
			return const_iterator(this, bucket, mit);
		}
		const_iterator mfind(const key_type& k, int* pCount) const
		{
			::System::Sync::KAutoLock<lock_type> mx(m_mx);

			int hashCode = m_hash(k);
			int bucket = hashCode % buckets;
			const map_type* pMap = m_ppMap[bucket];
			if(pMap->empty()) return this->end();
			typename map_type::const_iterator mit = pMap->mfind(k, pCount);
			if(mit == pMap->end()) return this->end();
			return const_iterator(this, bucket, mit);
		}
		const value_type& operator [] (const key_type& k) const
		{
			::System::Sync::KAutoLock<lock_type> mx(m_mx);

			int hashCode = m_hash(k);
			int bucket = hashCode % buckets;
			const map_type* pMap = m_ppMap[bucket];
			return pMap->operator[](k);
		}
		iterator insert(const key_type& k, const value_type& v)
		{
			::System::Sync::KAutoLock<lock_type> mx(m_mx);

			int hashCode = m_hash(k);
			int bucket = hashCode % buckets;
			map_type* pMap = m_ppMap[bucket];

			int sz = m_size - pMap->size();
			typename map_type::iterator mit = pMap->insert(k, v);
			m_size = sz + pMap->size();

			return iterator(this, bucket, mit);
		}
		iterator insert_unique(const key_type& k, const value_type& v)
		{
			::System::Sync::KAutoLock<lock_type> mx(m_mx);

			int hashCode = m_hash(k);
			int bucket = hashCode % buckets;
			map_type* pMap = m_ppMap[bucket];

			int sz = m_size - pMap->size();
			typename map_type::iterator mit = pMap->insert_unique(k, v);
			m_size = sz + pMap->size();

			return iterator(this, bucket, mit);
		}
		iterator erase(const key_type& k)
		{
			::System::Sync::KAutoLock<lock_type> mx(m_mx);

			iterator it = this->find(k);
			if(it == this->end()) return this->end();
			return this->erase(it);
		}
		iterator erase(const iterator& it)
		{
			::System::Sync::KAutoLock<lock_type> mx(m_mx);

			if(it == this->end())
			{
				return this->end();
			}

			int bucket = it.m_bucket;
			if(bucket < 0 || bucket >= buckets)
			{
				return this->end();
			}

			map_type* pMap = m_ppMap[bucket];

			if(pMap->empty())
			{
				return this->end();
			}

			int sz = m_size - pMap->size();
			typename map_type::iterator mit = pMap->erase(it.m_it);
			m_size = sz + pMap->size();

			if(mit != pMap->end())
			{
				return iterator(this, bucket, mit);
			}

			while(TRUE)
			{
				bucket += 1;
				if(bucket >= buckets)
				{
					return this->end();
				}

				pMap = m_ppMap[bucket];
				if(pMap->empty()) continue;

				typename map_type::iterator mit = pMap->begin();
				return iterator(this, bucket, mit);
			}
			ASSERT_I(FALSE);
			return this->end();
		}
		void print() const
		{
			for(int i=0; i<buckets; i++)
			{
				const map_type* pMap = m_ppMap[i];
				printf_k("%4d    %4d\n", i, pMap->size());
			}
		}
		void Lock() const
		{
			m_mx.Lock();
		}
		void Unlock() const
		{
			m_mx.Unlock;
		}
		/**
		* 得到在指定序号上的迭代器
		*/
		iterator at(int idx)
		{
			for(int i=0; i<buckets && idx>=0; i++)
			{
				map_type* m = m_ppMap[i];
				if(m)
				{
					int c = m->size();
					if(idx < c)
					{
						typename map_type::iterator mit = m->at(idx);
						return iterator(this,i,mit);
					}
					else
					{
						idx -= c;
					}
				}
			}
			return iterator();
		}
		const_iterator at(int idx) const
		{
			for(int i=0; i<buckets && idx>=0; i++)
			{
				const map_type* m = m_ppMap[i];
				if(m)
				{
					int c = m->size();
					if(idx < c)
					{
						typename map_type::const_iterator mit = m->at(idx);
						return const_iterator(this,i,mit);
					}
					else
					{
						idx -= c;
					}
				}
			}
			return const_iterator();
		}

	private:
		map_type* get_bucket(int i)
		{
			return m_ppMap[i];
		}
		const map_type* get_bucket(int i) const
		{
			return m_ppMap[i];
		}
	};

}}