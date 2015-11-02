#ifndef _K_VECTOR_H_
#define _K_VECTOR_H_

#include "../KType.h"
#include "../KMacro.h"
#include "../Memory/KAlloc.h"
#include "../Memory/KMemory.h"
#include "string.h"

#pragma warning(disable:6308)
#pragma warning(disable:6011)

namespace System { namespace Collections {

/**
 * 模拟std::vector的一个简单实现
 * 使用拷贝构造函数实现元素的拷贝
 * 对于用户自定义类型，需要考虑缺省的位拷贝的构造函数是否符合需要
 * 如果缺省的位拷贝不符合要求，那么就需要提供拷贝构造函数
 * 可容纳的元素数量是可变的，但是会受到分配器的容量限制
 * 使用KAlloc.h中的Allocator来分配内存
 */
template< typename T, typename A=::System::Memory::HeapAllocator> class KVector
{
private:
	typedef T value_type;
	typedef A alloc_type;

public:
	KVector(int capacity=64, int step=16)
	{
		m_size = 0;
		m_step = step;
		m_capacity = capacity;

		int length = m_capacity*sizeof(value_type*);
		m_vals = (value_type**)malloc_k(length);

		m_ownAlloc = true;
		m_al = new alloc_type();

		if(m_al->unitSize())
		{
			ASSERT(m_al->unitSize() >= sizeof(T));
		}
	}
	KVector(alloc_type* al, int capacity=64, int step=16)
	{
		m_size = 0;
		m_step = step;
		m_capacity = capacity;

		int length = m_capacity*sizeof(value_type*);
		m_vals = (value_type**)malloc_k(length);

		m_ownAlloc = false;
		m_al = al;

		if(m_al->unitSize())
		{
			ASSERT(m_al->unitSize() >= sizeof(T));
		}
	}
	KVector(const KVector& o)
	{
		m_size = 0;
		m_step = o.m_step;
		m_capacity = o.m_capacity;

		int length = m_capacity*sizeof(value_type*);
		m_vals = (value_type**)malloc_k(length);

		if(o.m_ownAlloc)
		{
			m_ownAlloc = true;
			m_al = new alloc_type();
		}
		else
		{
			m_ownAlloc = false;
			m_al = o.m_al;
		}
		for(int i=0; i<o.m_size; i++)
		{
			this->push_back(o.at(i));
		}
	}
	~KVector()
	{
		this->Destroy();
	}
	KVector& operator = (const KVector& o)
	{
		if(this == &o) return *this;
		
		this->clear();

		if(m_capacity < o.m_capacity)
		{
			m_vals = (value_type**)realloc_k(m_vals, o.m_capacity*sizeof(value_type*));
			m_capacity = o.m_capacity;
		}

		for(int i=0; i<o.m_size; i++)
		{
			this->push_back(o.at(i));
		}
		
		return *this;
	}

public:
	int size() const
	{
		return m_size;
	}
	int getCapacity() const
	{
		return m_capacity;
	}
	KVector& clear()
	{
		while(m_size)
		{
			this->erase(m_size-1);
		}
		return *this;
	}
	value_type& at(int index)
	{
		ASSERT(index >= 0 && index<m_size);
		return *m_vals[index];
	}
	const value_type& at(int index) const
	{
		ASSERT(index >= 0 && index<m_size);
		return *m_vals[index];
	}
	KVector& erase(int index)
	{
		ASSERT(index >= 0 && index<m_size);

		m_vals[index]->~value_type();
		m_al->free(m_vals[index]);

		int length = (m_size-index-1)*sizeof(value_type*);
		m_size -= 1;

		if(length > 0)
		{
			void* pSrc = (void*)&m_vals[index+1];
			void* pDest = (void*)&m_vals[index];
			::memmove(pDest, pSrc, length);
		}

		return *this;
	}
	KVector& push_back(const value_type& val)
	{
		if(m_size >= m_capacity)
		{
			int capacity = m_capacity + m_step;
			m_vals = (value_type**)realloc_k(m_vals, capacity*sizeof(value_type*));
			m_capacity = capacity;
		}

		void* p = m_al->alloc(sizeof(value_type));
		ASSERT(p);

		new (p) value_type(val);
		m_vals[m_size++] = (value_type*)p;

		return *this;
	}
	//在指定位置插入
	KVector& insert(int index, const value_type& val)
	{
		ASSERT(index>=0 && index <= m_size);
		
		if(index == m_size)
		{
			return this->push_back(val);
		}

		if(m_size >= m_capacity)
		{
			int capacity = m_capacity + m_step;
			m_vals = (value_type**)realloc_k(m_vals, capacity*sizeof(value_type*));
			m_capacity = capacity;
		}

		void* p = m_al->alloc(sizeof(value_type));
		ASSERT(p);

		new (p) value_type(val);
		
		void* pSrc = &m_vals[index];
		void* pDst = &m_vals[index+1];
		int length = (m_size-index)*sizeof(value_type*);
		memmove(pDst, pSrc, length);

		m_vals[index] = (value_type*)p;
		m_size += 1;

		return *this;
	}
	value_type& operator[](int index)
	{
		return this->at(index);
	}
	const value_type& operator [] (int index) const
	{
		return this->at(index);
	}

public:
	const void* _first_ptr_() const
	{
		return (void*)m_vals;
	}

private:
	void Destroy()
	{
		this->clear();

		if(m_ownAlloc && m_al)
		{
			delete m_al;
			m_al = NULL;
		}
		else
		{
			m_al = NULL;
		}

		if(m_vals)
		{
			free_k(m_vals);
			m_vals = NULL;
		}

		m_capacity = 0;
	}

public:
	int m_size;
	int m_step;
	int m_capacity;
	value_type** m_vals;
	bool m_ownAlloc;
	alloc_type* m_al;
};

/**
 * 按顺序存储的数组
 */
 template <typename T,typename C=KLessCompare<T,KLess<T> >,typename A=::System::Memory::HeapAllocator>
 class KSortedVector2
 {
 private:
	 typedef T value_type;
	 typedef C cmp_type;
	 typedef A alloc_type;

 public:
	 KSortedVector2(int capacity=64, int step=16):m_vector(capacity,step)
	 {
	 }
	 KSortedVector2(alloc_type* al, int capacity=64, int step=16):m_vector(al,capacity,step)
	 {
	 }
	 KSortedVector2(const KSortedVector2& o):m_vector(o.m_vector),m_cmp(o.m_cmp)
	 {
	 }
	 KSortedVector2& operator = (const KSortedVector2& o)
	 {
		 if(this == &o) return *this;
		 m_cmp = o.m_cmp;
		 m_vector = o.m_vector;
		 return *this;
	 }
	 BOOL empty() const
	 {
		 return m_vector.size() < 1;
	 }
	 int size() const
	 {
		 return m_vector.size();
	 }
	 int getCapacity() const
	 {
		 return m_vector.getCapacity();
	 }
	 KSortedVector2& clear()
	 {
		 m_vector.clear();
		 return *this;
	 }
	 // 插入一个元素，不检查唯一性，返回在数组中的位置
	 int insert(const value_type& val)
	 {
		 BOOL found;
		 int index = this->bsearch(val, found);
		 if(found) index += 1;
		 m_vector.insert(index, val);
		 return index;
	 }
	 // 插入一个元素，检查唯一性，返回在数组中的位置
	 // return -1 表示有重复
	 int insert_unique(const value_type& val)
	 {
		 BOOL found;
		 int index = this->bsearch(val, found);
		 if(found) return -1;
		 m_vector.insert(index, val);
		 return index;
	 }
	 KSortedVector2& erase(int index)
	 {
		 m_vector.erase(index);
		 return *this;
	 }
	 int find(const value_type& val) const
	 {
		 BOOL found;
		 int index = this->bsearch(val, found);
		 if(!found) return -1;
		 return index;
	 }
	 int mfind(const value_type& val, int* pCount) const
	 {
		 BOOL found; *pCount = 0;
		 int index = this->bsearch(val, found);
		 if(!found) return -1;

		 *pCount = 1;

		 int first_index = index;
		 while(first_index > 0)
		 {
			 int prev_index = first_index - 1;
			 if(m_cmp(at(prev_index), val)) break;
			 *pCount += 1; first_index -= 1;
		 }

		 int n = this->size();
		 while(++index < n)
		 {
			 if(m_cmp(at(index), val)) break;
			 *pCount += 1;
		 }

		 return first_index;
	 }
	 value_type& at(int index)
	 {
		 return m_vector.at(index);
	 }
	 const value_type& at(int index) const
	 {
		 return m_vector.at(index);
	 }
	 value_type& operator [] (int index)
	 {
		 return m_vector[index];
	 }
	 const value_type& operator [] (int index) const
	 {
		 return m_vector[index];
	 }

 protected:
	 /**
	 * 实现对半查找，返回值是元素应当插入的位置
	 */
	 int bsearch(const value_type& val, BOOL& found) const
	 {
		 found = FALSE;
		 if(m_vector.size() < 1) return 0;

		 char* orig = (char*)m_vector._first_ptr_();
		 register char* lo = orig;
		 register char* hi = lo + (m_vector.size()-1) * sizeof(value_type*);

		 register int num = m_vector.size();
		 register int unitlen = sizeof(value_type*);

		 char* mid;
		 int half;
		 int result;

		 while (lo <= hi)
		 {
			 if (half = num / 2)
			 {	// num >= 2
				 mid = lo + (num & 1 ? half : (half - 1)) * unitlen;
				 if (!(result = m_cmp(val, **(const value_type* const *)mid)))
				 {
					 found = TRUE;
					 return (int)((mid-orig) / unitlen);
				 }
				 else if (result < 0)
				 {	// val < mid
					 hi = mid - unitlen;
					 num = num & 1 ? half : half-1;

					 if(lo > hi)
					 {
						 return (int)((mid-orig) / unitlen);
					 }
				 }
				 else
				 {	// val > mid
					 lo = mid + unitlen;
					 num = half;

					 if(lo > hi)
					 {
						 return (int)((mid-orig) / unitlen + 1);
					 }
				 }
			 }
			 else if (num)
			 {	//只有一个
				 int n = m_cmp(val, **(const value_type* const *)lo);
				 if(!n)
				 {
					 found = TRUE;
					 return (int)((lo-orig)/unitlen);
				 }
				 else if(n < 0)
				 {
					 return (int)((lo-orig)/unitlen);
				 }
				 else
				 {
					 return (int)((lo-orig)/unitlen + 1);
				 }
			 }
			 else
			 {	//没有
				 return (int)((lo-orig)/unitlen);
			 }
		 }
		 //不可达
		 return -1;
	 }

 public:
	 cmp_type m_cmp;
	 KVector<T,A> m_vector;
 };

template <typename T,typename C=KLess<T>,typename A=::System::Memory::HeapAllocator>
class KSortedVector : public KSortedVector2<T,KLessCompare<T,C>,A>
{
public:
	typedef KSortedVector2<T,KLessCompare<T,C>,A> base_type;
	typedef A alloc_type;

public:
	KSortedVector(int capacity=64, int step=16):base_type(capacity,step)
	{
	}
	KSortedVector(alloc_type* al, int capacity=64, int step=16):base_type(al,capacity,step)
	{
	}
	KSortedVector(const KSortedVector& o):base_type(o)
	{
	}
	KSortedVector& operator = (const KSortedVector& o)
	{
		if(this == &o) return *this;
		base_type::operator = (o);
		return *this;
	}
};

}}

#endif