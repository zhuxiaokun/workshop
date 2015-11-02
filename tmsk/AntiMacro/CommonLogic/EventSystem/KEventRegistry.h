#ifndef _K_EVENT_REGISTRY_H_
#define _K_EVENT_REGISTRY_H_

#include <System/Collections/KList.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KTree.h>

class KEventHandler;
class KEvtRegEntry
{
public:
	enum { max_stack_deep = 3 };
	struct Node
	{
		KEventHandler* handler;		// 事件处理器
		int refCount;				// 引用计数
		Node* m_prevNode;			// 构造双向链表用
		Node* m_nextNode;
	};
	typedef System::Collections::KSelfList<Node> NodeList;
	typedef System::Collections::KMapByTree_staticPooled<KEventHandler*,Node*,1024> MapHandler;

public:
	KEvtRegEntry();
	~KEvtRegEntry();

public:
	void clear();
	bool regHandler(KEventHandler* handler);
	bool unregHandler(KEventHandler* handler);
	void onEvent(int evt, const void* data, int len);

public:
	NodeList m_list;
	MapHandler m_hmap;
	short m_needClean, m_stackDeep;
};

template <int startEvt,int endEvt> class KEvtRegistry
{
private:
	enum { evt_count = endEvt-startEvt };
	KEvtRegEntry m_regEntries[evt_count];

public:
	KEvtRegistry()
	{
	}
	~KEvtRegistry()
	{
		this->clear();
	}
	void clear()
	{
		for(int i=0; i<evt_count; i++) m_regEntries[i].clear();
	}
	KEvtRegEntry* get(int evt)
	{
		if(evt<startEvt || evt>=endEvt) return NULL;
		return &m_regEntries[evt-startEvt];
	}
};

#endif