#ifndef _K_QUEUE_H_
#define _K_QUEUE_H_

#include "KSDList.h"
#include "../Memory/KAlloc.h"

namespace System { namespace Collections {

template <typename T, typename L=::System::Sync::KNoneLock, int initSize=64> class KQueueFifo
{
public:
	typedef T value_type;
	typedef L lock_type;
	typedef SDListNode<T> node_type;
	typedef ::System::Memory::KStepBlockAlloc<node_type,initSize> alloc_type;
	typedef KSDList<T,L,alloc_type> list_type;

private:
	list_type m_list;
	::System::Sync::KSync_PulseEvent m_event;

public:
	KQueueFifo()
	{

	}

	~KQueueFifo()
	{

	}

public:
	BOOL empty() const
	{
		return m_list.empty();
	}
	int size() const
	{
		return m_list.size();
	}
	void clear()
	{
		m_list.clear();
	}
	BOOL push(const value_type& val)
	{
		m_list.push_back(val);
		m_event.Set();
		return TRUE;
	}
	BOOL pop(value_type& val)
	{
		value_type* pVal;
		
		m_list.Lock();
		
		pVal = m_list.begin();
		if(!pVal)
		{
			m_list.Unlock();
			return FALSE;
		}

		val = *pVal;
		m_list.erase_front();

		m_list.Unlock();

		return TRUE;
	}
	BOOL wait(int ms)
	{
		return m_event.Block(ms);
	}
};

}}

#endif