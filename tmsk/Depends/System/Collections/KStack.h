#ifndef _K_STACK_H_
#define _K_STACK_H_

#include "KVector.h"

namespace System { namespace Collections {

template< typename T, typename A=::System::Memory::HeapAllocator> class KStack
{
public:
	typedef T value_type;
	typedef A alloc_type;

public:
	KStack()
	{

	}
	~KStack()
	{

	}
	KStack(const KStack& o):m_vec(o.m_vec)
	{
	}
	KStack& operator = (const KStack& o)
	{
		if(this == &o) return *this;
		m_vec = o.m_vec;
		return *this;
	}
	void push(const value_type& val)
	{
		m_vec.push_back(val);
	}
	void pop()
	{
		int idx = m_vec.size() - 1;
		m_vec.erase(idx);
	}
	value_type& top()
	{
		int idx = m_vec.size() - 1;
		return m_vec.at(idx);
	}
	const value_type& top() const
	{
		int idx = m_vec.size() - 1;
		return m_vec.at(idx);
	}
	value_type& operator [] (int index)
	{
		int idx = m_vec.size() - index - 1;
		return m_vec[idx];
	}
	const value_type& operator [] (int index) const
	{
		int idx = m_vec.size() - index - 1;
		return m_vec[idx];
	}
	int size() const
	{
		return m_vec.size();
	}
	BOOL empty() const
	{
		return m_vec.size() < 1;
	}
	void clear()
	{
		m_vec.clear();
	}

public:
	KVector<T,A> m_vec;
};

}}

#endif