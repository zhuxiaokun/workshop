#ifndef _K_EVENT_SYSTEM_H_
#define _K_EVENT_SYSTEM_H_

#include "KEventRegistry.h"
#include <lua/KLuaWrap.h>

struct EventWrapper
{
	int evt;
	const void* data;
	int len;
	//
	EventWrapper(int evtId,const void* p, int l):evt(evtId),data(p),len(l)
	{
	}
};

class KEventHandler : public KBaseLuaClass<KEventHandler>
{
public:
	virtual ~KEventHandler() { }
	virtual void handleEvent(int evt, const void* data, int len) { }

public:
	BeginDefLuaClassNoCon(KEventHandler);
		DefMemberFunc(handleEvent);
	EndDef;
}; __luaClassPtr2Lua(KEventHandler);

/**
 * 将一个Event转换为另一个Event (ID和数据都可能发生变化)
 *
 */
class KEventAdapter
{
public:
	virtual ~KEventAdapter() { }
	virtual bool transform(const EventWrapper& input, EventWrapper& output) = 0;
};

class KEventAdapterChain : public KEventAdapter
{
public:
	struct Node
	{
		KEventAdapter* adapter;
		Node* m_prevNode;
		Node* m_nextNode;
	};
	typedef System::Collections::KSelfList<Node> AdapterList;

public:
	KEventAdapterChain();
	~KEventAdapterChain();

public:
	bool transform(const EventWrapper& input, EventWrapper& output);

public:
	bool append(KEventAdapter* adapter);
	bool erase(KEventAdapter* adapter);
	void clear();

private:
	AdapterList m_list;
};

class KEventSource
{
public:
	virtual ~KEventSource() { }

protected:
	virtual KEvtRegEntry* getEvtRegEntry(int evt) { ASSERT(FALSE); return NULL; }
	virtual void clearEvtRegistry() { ASSERT(FALSE); }

public:
	bool regEvent(int evt, KEventHandler* handler);
	bool unregEvent(int evt, KEventHandler* handler);
	void fireEvent(int evt, const void* data, int len);

public:
	BeginDefLuaClass(KEventSource);
		DefMemberFunc(regEvent);
		DefMemberFunc(unregEvent);
		DefMemberFunc(fireEvent);
	EndDef;
};

template <int startEvt,int endEvt>
class KEventSrcTmpl : public KEventSource
{
private:
	KEvtRegistry<startEvt,endEvt> m_evtRegistry;

public:
	KEventSrcTmpl()
	{
	}
	~KEventSrcTmpl()
	{
	}

protected:
	KEvtRegEntry* getEvtRegEntry(int evt)
	{
		return m_evtRegistry.get(evt);
	}
	void clearEvtRegistry()
	{
		m_evtRegistry.clear();
	}
};

#endif