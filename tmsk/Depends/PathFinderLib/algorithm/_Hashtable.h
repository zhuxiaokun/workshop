#pragma once
#include <System/Collections/KHashTable.h>

template <typename K,
		typename V,
		typename C=KLessCompare<K,KLess<K> >,
		typename H=KHasher<K>,
		typename L=::System::Sync::KNoneLock,
		int buckets=1024,
		int initCapacity=1024*8>
class _Hashtable : public ::System::Collections::KHashTable<K,V,C,H,L,buckets,initCapacity>
{
public:
	typedef ::System::Collections::KHashTable<K,V,C,H,L,buckets,initCapacity> base_type;
	void clear()
	{
		System::Sync::KAutoLock<typename base_type::lock_type> mx(base_type::m_mx);
		for(int i=0; i<buckets; i++)
		{
			typename base_type::map_type* pMap = base_type::m_ppMap[i];
			pMap->m_vec.m_vector.m_size = 0;
		}
		base_type::m_alloc.clear();
		base_type::m_size = 0;
	}
};