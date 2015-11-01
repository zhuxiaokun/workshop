#include "KEventSystem.h"
#include <System/Log/log.h>

/// KEventAdapterChain ///
static System::Memory::KStepObjectPool<KEventAdapterChain::Node,512> g_chainNodePool;
KEventAdapterChain::KEventAdapterChain()
{

}

KEventAdapterChain::~KEventAdapterChain()
{
	this->clear();
}

bool KEventAdapterChain::transform(const EventWrapper& input, EventWrapper& output)
{
	char buf1[512];
	char buf2[512];
	
	bool done = false;
	const EventWrapper* pi = &input;
	EventWrapper my1(0, &buf1, sizeof(buf1));
	EventWrapper my2(0, &buf2, sizeof(buf2));
	EventWrapper* po = &my1;

	Node* node = m_list.begin();
	while(node)
	{
		if(node->adapter->transform(*pi, *po))
		{
			pi = po;
			po = (po == &my1) ? &my2 : &my1;
			done = TRUE;
		}
		node = m_list.next(node);
	}
	
	if(done)
	{
		output.evt = pi->evt;
		output.len = pi->len;
		memcpy((void*)output.data, pi->data, pi->len);
	}

	return done;
}

bool KEventAdapterChain::append(KEventAdapter* adapter)
{
	Node* node = g_chainNodePool.Alloc();
	node->adapter = adapter;
	node->m_prevNode = NULL;
	node->m_nextNode = NULL;
	m_list.push_back(node);
	return true;
}

bool KEventAdapterChain::erase(KEventAdapter* adapter)
{
	Node* node = m_list.begin();
	while(!node)
	{
		if(node->adapter == adapter)
		{
			m_list.erase(node);
			return true;
		}
		node = m_list.next(node);
	}
	return false;
}

void KEventAdapterChain::clear()
{
	while(!m_list.empty())
	{
		Node* node = m_list.pop_front();
		g_chainNodePool.Free(node);
	}
}

// KEventSource //

bool KEventSource::regEvent(int evt, KEventHandler* handler)
{
	KEvtRegEntry* regEntry = this->getEvtRegEntry(evt);
	if(!regEntry) return false;
	return regEntry->regHandler(handler);
}

bool KEventSource::unregEvent(int evt, KEventHandler* handler)
{
	KEvtRegEntry* regEntry = this->getEvtRegEntry(evt);
	if(!regEntry) return false;
	return regEntry->unregHandler(handler);
}

void KEventSource::fireEvent(int evt, const void* data, int len)
{
	KEvtRegEntry* regEntry = this->getEvtRegEntry(evt);
	if(!regEntry) return;
	return regEntry->onEvent(evt, data, len);
}