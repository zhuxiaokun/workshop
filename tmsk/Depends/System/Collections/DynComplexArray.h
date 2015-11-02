/********************************************************************
created:	2010/5/31
filename: 	System\Collections\DynComplexArray.h
file path:	System\Collections
file base:	DynComplexArray
file ext:	h
author:		Allen Wang
copyright:	AuroraGame

purpose:	基于DynArray，重写了一些方法用以处理复杂的对象，没有提供
			iterator，从原来KTreasureCaseManager.h分离出来
*********************************************************************/
#pragma once
#include "../KType.h"
#include "../KMacro.h"
#include "../Memory/KMemory.h"
namespace System { namespace Collections {

	template <typename T, int initCapacity=32, int inflateStep=16>
	class DynComplexArray
	{
	public:
		typedef T value_type;

	protected:
		BOOL m_ownMem;
		int m_size;
		int m_capacity;
		value_type* m_vals;

	public:
		DynComplexArray()
			: m_ownMem(TRUE)
			, m_size(0)
			, m_capacity(0)
			, m_vals(NULL)
		{
		}
		DynComplexArray(value_type* buffer, int capacity, int size)
			: m_ownMem(FALSE)
			, m_vals(buffer)
			, m_capacity(capacity)
			, m_size(size)
		{
		}
		DynComplexArray(const DynComplexArray& o)
			:m_ownMem(TRUE)
			,m_size(0)
			,m_capacity(0)
			,m_vals(NULL)
		{
			if(!o.m_size) return;
			this->inflate(o.m_size);
			for (int i = 0; i < o.m_size; ++i)
			{
				m_vals[i] = o.m_vals[i];
			}
			m_size = o.m_size;
		}
		~DynComplexArray()
		{
			this->destroy();
		}
		DynComplexArray& operator = (const DynComplexArray& o)
		{
			if(this == &o) return *this;

			this->clear();
			if(!o.m_size) return *this;

			if(m_capacity < o.m_size) this->inflate(o.m_size);
			for (int i = 0; i < o.m_size; ++i)
			{
				m_vals[i] = o.m_vals[i];
			}
			m_size = o.m_size;

			return *this;
		}
		int size() const
		{
			return m_size;
		}
		void clear()
		{
			for (int i = 0; i < m_size; i++)
			{
				((value_type*)&m_vals[i])->~value_type();
			}
			m_size = 0;
		}
		BOOL empty() const
		{
			return m_size < 1;
		}
		DynComplexArray& attach(value_type* vals, int capacity, int size)
		{
			this->destroy();
			m_ownMem = FALSE;
			m_size = size;
			m_capacity = capacity;
			m_vals = vals;
			return *this;
		}
		void detach()
		{
			if(m_ownMem) return;
			m_ownMem = TRUE;
			m_size = 0;
			m_capacity = 0;
			m_vals = NULL;
		}
		void resize( int newSize )
		{
			ASSERT( newSize >= 0 );
			if(m_capacity < newSize) this->inflate(newSize);
			if(m_size < newSize)
			{
				for (int i = m_size; i < newSize; i++)
				{
					new(&m_vals[i]) value_type;
				}
			}
			else
			{
				for (int i = m_size-1; i >= newSize; i--)
				{
					((value_type*)&m_vals[i])->~value_type();
				}
			}
			m_size = newSize;
		}
		value_type& at(int idx)
		{
			ASSERT(idx >= 0);
			int size = idx + 1;
			if(m_capacity < size) this->inflate(size);
			if(m_size < size)
			{
				for (int i = m_size; i < size; i++)
				{
					new(&m_vals[i]) value_type;
				}
				m_size = size;
			}
			return m_vals[idx];
		}
		const value_type& at(int idx) const
		{
			ASSERT(idx >= 0 && idx < m_size);
			return m_vals[idx];
		}
		value_type& get(int idx)
		{
			ASSERT(idx >= 0 && idx < m_size);
			return m_vals[idx];
		}
		value_type& operator [] (int idx)
		{
			return this->at(idx);
		}
		const value_type& operator [] (int idx) const
		{
			return this->at(idx);
		}
		DynComplexArray& push_back(const value_type& val)
		{
			this->at(m_size) = val;
			return *this;
		}
		DynComplexArray& erase(int idx)
		{
			if(idx < 0 || idx >= m_size) return *this;
			int len = (m_size-idx-1) * sizeof(value_type);
			if(len > 0)
			{
				void* pSrc = &m_vals[idx+1];
				void* pDst = &m_vals[idx];
				// 析构一下
				((value_type*)pDst)->~value_type();
				::memmove(pDst, pSrc, len);
			}
			else
			{
				((value_type*)&m_vals[idx])->~value_type();
			}
			m_size -= 1;
			return *this;
		}
		DynComplexArray& insert(int idx, const value_type& val)
		{
			if(idx >= m_size)
			{
				this->at(idx) = val;
				return *this;
			}

			int size = m_size + 1;
			if(m_capacity < size) this->inflate(size);

			int len = (m_size-idx)*sizeof(value_type);
			if(len > 0)
			{
				void* pSrc = &m_vals[idx];
				void* pDst = &m_vals[idx+1];
				memmove(pDst, pSrc, len);
			}
			m_vals[idx] = val;
			m_size += 1;
			return *this;
		}

	protected:
		// 在当前的capacity小于size时被调用
		BOOL inflate(int size)
		{
			ASSERT(m_ownMem);
			if(!m_vals)
			{
				int capacity = initCapacity;
				while(capacity < size) capacity += inflateStep;
				m_vals = (value_type*)malloc(capacity*sizeof(value_type));
				for (int i = 0; i < capacity; ++i)
				{
					new(&m_vals[i]) value_type;
				}
				m_capacity = capacity;
			}
			else
			{
				int capacity = m_capacity;
				while(capacity < size) capacity += inflateStep;
				m_vals = (value_type*)realloc(m_vals, capacity*sizeof(value_type));
				for (int i = m_capacity; i < capacity; ++i)
				{
					new(&m_vals[i]) value_type;
				}
				m_capacity = capacity;
			}
			return TRUE;
		}
		void destroy()
		{
			this->clear();
			if(m_ownMem && m_vals)
			{
				free(m_vals);
				m_vals = NULL;
				m_capacity = 0;
			}
		}
	};

};	};