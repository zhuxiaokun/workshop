#include "KEventRegistry.h"
#include "KEventSystem.h"

/// node pool ///

static System::Memory::KStepObjectPool<KEvtRegEntry::Node,1024> g_evtNodePool;
static KEvtRegEntry::Node* allocEvtNode()
{
	KEvtRegEntry::Node* node = g_evtNodePool.Alloc();
	node->m_prevNode = NULL;
	node->m_nextNode = NULL;
	return node;
}
static void freeEvtNode(KEvtRegEntry::Node* node)
{
	g_evtNodePool.Free(node);
}

// KEventRegistryEntry ///

KEvtRegEntry::KEvtRegEntry()
	: m_needClean(0)
	, m_stackDeep(0)
{

}

KEvtRegEntry::~KEvtRegEntry()
{
	this->clear();
}

void KEvtRegEntry::clear()
{
	m_needClean = m_stackDeep = 0;
	m_hmap.clear();
	while(!m_list.empty())
	{
		Node* node = m_list.pop_front();
		freeEvtNode(node);
	}
}

bool KEvtRegEntry::regHandler(KEventHandler* handler)
{
	MapHandler::iterator it = m_hmap.find(handler);
	if(it != m_hmap.end())
	{
		Node* node = it->second;
		node->refCount++;
	}
	else
	{
		Node* node = allocEvtNode();
		node->handler = handler;
		node->refCount = 1;
		m_list.push_back(node);
		m_hmap[handler] = node;
	}
	return true;
}

bool KEvtRegEntry::unregHandler(KEventHandler* handler)
{
	MapHandler::iterator it = m_hmap.find(handler);
	if(it == m_hmap.end()) return false;

	Node* node = it->second;
	if(node->refCount) node->refCount--;
	if(node->refCount < 1)
	{
		m_hmap.erase(it);
		if(m_stackDeep)
		{
			m_needClean += 1;		// 在事件处理的时候取消事件注册
		}
		else
		{
			m_list.erase(node);
			freeEvtNode(node);
		}
	}
	return true;
}

void KEvtRegEntry::onEvent(int evt, const void* data, int len)
{
	if(++m_stackDeep > max_stack_deep)
	{
		char buf[1024];
		Log(LOG_ERROR, "error: event process stack overflow, callstack:%s", backtrace(buf,sizeof(buf)));
		m_stackDeep--;
		return;
	}

	{//
		Node* node = m_list.begin();
		while(node)
		{
			if(node->refCount)
			{
				node->handler->handleEvent(evt, data, len);
			}
			node = m_list.next(node);
		}
	}

	m_stackDeep--;
	if(!m_stackDeep && m_needClean)
	{
		Node* node = m_list.begin();
		while(node)
		{
			if(!node->refCount)
			{
				Node* node2 = node; node = m_list.next(node);
				m_list.erase(node2); freeEvtNode(node2);
			}
			else
			{
				node = m_list.next(node);
			}
		}
		m_needClean = 0;
	}
}