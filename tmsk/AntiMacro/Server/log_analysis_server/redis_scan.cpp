#include "redis_scan.h"
#include <System/Log/log.h>
#include <System/Misc/StrUtil.h>

//// KAccountNameArray

KNameArray::KNameArray()
	: m_nextPos(0)
	, m_current(NULL)
{

}

KNameArray::~KNameArray()
{
	this->reset();
}

bool KNameArray::avail() const
{
	return m_nextPos < CAPACITY;
}

int KNameArray::size() const
{
	return m_nextPos;
}

const char* KNameArray::append(const char* name)
{
	if(!this->avail())
		return NULL;

	size_t len = strlen(name) + 1;
	const char* nm = this->_alloc(name, len);
	m_buffer[m_nextPos++] = nm;

	return nm;
}

const char* KNameArray::at(int idx) const
{
	ASSERT(idx >= 0 && idx < m_nextPos);
	return m_buffer[idx];
}

void KNameArray::reset()
{
	list_node::reset();
	m_nextPos = 0;
	while(!m_bufferList.empty())
		KNameBuffer::Free(m_bufferList.pop_front());
}

const char* KNameArray::_alloc(const char* str, size_t len)
{
	if(!m_current)
	{
		m_current = KNameBuffer::Alloc();
	}
	if((size_t)m_current->WriteAvail() < len)
	{
		m_bufferList.push_back(m_current);
		m_current = KNameBuffer::Alloc();
	}
	const char* p = (const char*)m_current->GetWriteBuffer();
	m_current->Write(str, len);
	return p;
}

//// KLargeNameList::iterator::

KLargeNameList::iterator::iterator():m_index(0),m_arr(NULL)
{

}

KLargeNameList::iterator::iterator(const iterator& o):m_index(o.m_index),m_arr(o.m_arr)
{

}

KLargeNameList::iterator::~iterator()
{

}

KLargeNameList::iterator& KLargeNameList::iterator::operator = (const iterator& o)
{
	if(this == &o) return *this;
	m_index = o.m_index;
	m_arr = o.m_arr;
	return *this;
}

KLargeNameList::iterator& KLargeNameList::iterator::operator ++ ()
{
	if(!m_arr) return *this;
	if(m_index+1 < m_arr->size())
	{
		m_index += 1;
		return *this;
	}
	m_index = 0;
	m_arr = m_arr->m_nextNode;
	return *this;
}

KLargeNameList::iterator& KLargeNameList::iterator::operator ++ (int)
{
	if(!m_arr) return *this;
	if(m_index+1 < m_arr->size())
	{
		m_index += 1;
		return *this;
	}
	m_index = 0;
	m_arr = m_arr->m_nextNode;
	return *this;
}

bool KLargeNameList::iterator::operator == (const iterator& o) const
{
	return m_index == o.m_index && m_arr == o.m_arr;
}

bool KLargeNameList::iterator::operator != (const iterator& o) const
{
	return m_index != o.m_index || m_arr != o.m_arr;
}

const char* KLargeNameList::iterator::operator * () const
{
	if(!m_arr) return NULL;
	return m_arr->m_buffer[m_index];
}

//// KLargeNameList

KLargeNameList::KLargeNameList()
{

}

KLargeNameList::~KLargeNameList()
{
	this->reset();
}

int KLargeNameList::size() const
{
	int n = (int)m_list.size();
	if(n < 1)
	{
		return 0;
	}
	else if(n == 1)
	{
		return m_list.rbegin()->size();
	}
	else
	{
		return (n - 1)*KNameArray::CAPACITY + m_list.rbegin()->size();
	}
}

const char* KLargeNameList::append(const char* name)
{
	KNameArray* arr = m_list.rbegin();
	if(!arr)
	{
		arr = KNameArray::Alloc();
		m_list.push_back(arr);
	}
	if(!arr->avail())
	{
		arr = KNameArray::Alloc();
		m_list.push_back(arr);
	}
	return arr->append(name);
}

void KLargeNameList::reset()
{
	while(!m_list.empty())
	{
		KNameArray* p = m_list.pop_front();
		p->reset();
		KNameArray::Free(p);
	}
}

bool KLargeNameList::foreach(lua_func func, l_stackval ctx)
{
	iterator it = this->begin();
	for(; it != this->end(); it++)
	{
		const char* name = *it;
		if(!func.r_invoke<bool,const char*,l_stackval>(name, ctx))
			return false;
	}
	return true;
}
