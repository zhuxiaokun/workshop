﻿#ifndef _K_LOGIC_TIMER_H_
#define _K_LOGIC_TIMER_H_

#include <System/Collections/KString.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Misc/KDatetime.h>

class KLogicRealTimer : public System::Memory::KPortableStepPool<KLogicRealTimer,256>
{
public:
	KLogicRealTimer();
	~KLogicRealTimer();

	enum enumLogicRealTimerState
	{
		eStop,
		eRun,
	};
public:
	typedef KLogicRealTimer timer;
	typedef KLogicRealTimer* timerp;

	class cmp
	{
	public:
		int operator () (const timer& a, const timer& b) const
		{
			int_r x = (int)a.m_name - (int)b.m_name;
			if(x < 0) return -1;
			else if(x > 0) return 1;
			if(a.m_cookie.value < b.m_cookie.value) return -1;
			else if(a.m_cookie.value > b.m_cookie.value) return 1;
			return 0;
		}
	};
	class ptrcmp
	{
	public:
		int operator () (const timerp& a, const timerp& b) const
		{
			int_r x = (int)a->m_name - (int)b->m_name;
			if(x < 0) return -1;
			else if(x > 0) return 1;
			if(a->m_cookie.value < b->m_cookie.value) return -1;
			else if(a->m_cookie.value > b->m_cookie.value) return 1;
			return 0;
		}
	};

public:
	bool startRealTimer(DWORD name, Identity64 cookie,int mode, int val, int hour, int minute, int second, void* data);
	bool isTimeout() const;
	void Breathe(int nInterval);

public:
	DWORD m_name;
	Identity64 m_cookie;
	void* m_data;	
	int m_mode;
	int m_value;
	int m_hour;
	int m_minute;
	int m_second;
	int m_state;
	int m_oldstate;
};

template <typename Receiver,size_t capacity=64,size_t step=32>
class KRealTimerpArray
	: public System::Collections::KDynSortedArray<KLogicRealTimer*,KLogicRealTimer::ptrcmp,capacity,step>
{
protected:
	typedef System::Collections::KDynSortedArray<KLogicRealTimer*,KLogicRealTimer::ptrcmp,capacity,step> base_type;

public:
	// 有onRealTimer(DWORD name,Identity64 cookie,void* data)函数
	Receiver* m_receiver;

protected:
	int _find(DWORD name, Identity64 cookie)
	{
		KLogicRealTimer tm;
		tm.m_name = name;
		tm.m_cookie = cookie;
		return this->find(&tm);
	}

public:
	KRealTimerpArray():m_receiver(NULL)
	{

	}

public:
	void breathe(int ms)
	{
		{
			int i;
			int c = this->size();
			for(i=0; i<c; i++)
			{
				this->at(i)->Breathe(ms);
			}
			i = c - 1;
			while(i >= 0)
			{
				KLogicRealTimer* tm = this->at(i--);
				if(!tm->isTimeout())continue;
				int name = tm->m_name; 
				Identity64 cookie = tm->m_cookie;
				void* data = tm->m_data;
				m_receiver->onRealTimer(name,cookie,data);
			}
		}
	}

public:
	KLogicRealTimer* get(DWORD name, Identity64 cookie)
	{
		int pos = this->_find(name, cookie);
		if(pos < 0) return NULL;
		return this->at(pos);
	}
	KLogicRealTimer* start(DWORD name, Identity64 cookie,int mode, int val, int hour, int minute, int second, void* data)
	{
		KLogicRealTimer* tm = this->get(name, cookie);
		if(!tm)
		{
			tm = KLogicRealTimer::Alloc();
			tm->startRealTimer(name, cookie, mode, val, hour, minute, second, data);
			this->insert(tm);
		}
		else
		{
			tm->startRealTimer(name, cookie, mode, val, hour, minute, second, data);
		}
		return tm;
	}
	void stop(DWORD name, Identity64 cookie)
	{
		int pos = this->_find(name, cookie);
		if(pos < 0) return;
		KLogicRealTimer* tm = this->at(pos);
		KLogicRealTimer::Free(tm);
		this->erase_by_index(pos);
	}
	void clear()
	{
		int c = this->size();
		for(int i=c-1; i>=0; i--)
		{
			KLogicRealTimer* timer = this->at(i);
			KLogicRealTimer::Free(timer);
		}
		base_type::clear();
	}
};

class KLogicTimer : public System::Memory::KPortableStepPool<KLogicTimer,256>
{
public:
	KLogicTimer();
	~KLogicTimer();

public:
	typedef KLogicTimer& ref_timer_type;
	typedef KLogicTimer* ptr_timer_type;

	struct refLess
	{
		bool operator () (ref_timer_type a, ref_timer_type b) const
		{
			if(a.m_name==b.m_name)	return a.m_cookie<b.m_cookie;
			else	return a.m_name<b.m_name;
		}
	};

	struct ptrLess
	{
		bool operator () (ptr_timer_type a, ptr_timer_type b) const
		{
			if(a->m_name==b->m_name)	return a->m_cookie<b->m_cookie;
			else	return a->m_name<b->m_name;
		}
	};

public:
	bool start(DWORD name, Identity64 cookie, int secs, void* data);
	bool start_m(DWORD name, Identity64 cookie, int msecs, void* data);
	bool restart();

	inline bool isTimeout()  const 
	{ 
		DWORD now = GetTickCount(); 
		return m_focusMilliSecs <= now;
	}
	inline bool isSuspend()  const { return m_suspendMilliSecs>0; }
	inline bool isAutoStop() const { return m_isAutoStop; }
	inline void setAutoStop(bool b){ m_isAutoStop=b; }
public:
	void*		m_data;
	Identity64	m_cookie;	// key of this timer
	DWORD		m_name;	// key of this timer
	 
	DWORD		m_focusMilliSecs;	// focus point time
	DWORD		m_milliSecs;	// milli secs	

	DWORD		m_suspendMilliSecs;	// suspend point time
	bool		m_isAutoStop;	// is auto stop
};

template <size_t capacity=64,size_t step=32>
class KTimerArray
	: public System::Collections::DynSortedArray<KLogicTimer,KLogicTimer::refLess,capacity,step>
{

};

template <typename Receiver,size_t capacity=64,size_t step=32>
class KTimerpArray
	: public System::Collections::DynSortedArray<KLogicTimer*,KLogicTimer::ptrLess,capacity,step>
{
protected:
	typedef System::Collections::DynSortedArray<KLogicTimer*,KLogicTimer::ptrLess,capacity,step> base_type;

public:
	Receiver* m_receiver;

protected:
	int _find(DWORD name, Identity64 cookie)
	{
		KLogicTimer tm;
		tm.m_name = name;
		tm.m_cookie = cookie;
		return this->find(&tm);
	}

public:
	KTimerpArray():m_receiver(NULL)
	{
	}

public:
	void breathe(int)
	{
		for(int i=this->size()-1; i>=0; --i)
		{
			KLogicTimer* tm = this->at(i);
			if(tm->isSuspend()==true)
				continue;
			if(tm->isTimeout()==false)
				continue;

			DWORD name = tm->m_name; 
			Identity64 cookie = tm->m_cookie;
			void* data = tm->m_data;
			if(tm->isAutoStop())
				this->stop(tm->m_name, tm->m_cookie);
			else
				this->restart(tm->m_name, tm->m_cookie);
			m_receiver->onTimer(name,cookie,data);

			if( i>=this->size() ) 
				i = this->size()-1;
		}
	}

public:
	void suspend( DWORD name, Identity64 cookie )
	{
		KLogicTimer* tm = this->get(name, cookie);
		if( tm!=NULL && tm->isSuspend()==false && tm->isTimeout()==false )
		{
			tm->m_suspendMilliSecs = GetTickCount();
			tm->m_suspendMilliSecs = tm->m_suspendMilliSecs>=tm->m_focusMilliSecs? tm->m_focusMilliSecs-1:tm->m_suspendMilliSecs; 
		}
	}

	void resume( DWORD name, Identity64 cookie )
	{
		KLogicTimer* tm = this->get(name, cookie);
		if( tm!=NULL && tm->isSuspend()==true )
		{
			int diff = tm->m_focusMilliSecs-tm->m_suspendMilliSecs;	ASSERT(diff>0);
			tm->m_focusMilliSecs = GetTickCount() + diff;
			tm->m_suspendMilliSecs = 0;
		}
	}

	int getSurplusTime( DWORD name, Identity64 cookie )
	{
		int iRet = 0;
		KLogicTimer* tm = this->get(name, cookie);
		if( tm!=NULL )
		{	
			iRet = (tm->m_focusMilliSecs - GetTickCount())/1000;
			iRet = iRet>0? iRet:0;
		}
		return iRet;
	}

	int getSurplusTime_m(DWORD name, Identity64 cookie)
	{
		int iRet = 0;
		KLogicTimer* tm = this->get(name, cookie);
		if( tm!=NULL )
		{
			iRet = tm->m_focusMilliSecs - GetTickCount();
			iRet = iRet>0? iRet:0;
		}
		return iRet;
	}

	KLogicTimer* get(DWORD name, Identity64 cookie)
	{
		int pos = this->_find(name, cookie);
		if(pos < 0) return NULL;
		return this->at(pos);
	}

	KLogicTimer* start(DWORD name, Identity64 cookie, int secs, void* data)
	{
		KLogicTimer* tm = this->get(name, cookie);
		if(tm==NULL)
		{
			tm = KLogicTimer::Alloc();
			tm->start(name, cookie, secs, data);
			this->insert(tm);
		}else tm->start(name, cookie, secs, data);
		return tm;
	}

	KLogicTimer* start_m(DWORD name, Identity64 cookie, int secs, void* data)
	{
		KLogicTimer* tm = this->get(name, cookie);
		if(tm==NULL)
		{
			tm = KLogicTimer::Alloc();
			tm->start_m(name, cookie, secs, data);
			this->insert(tm);
		}else	tm->start_m(name, cookie, secs, data);
		return tm;
	}

	KLogicTimer* startAt(DWORD name, Identity64 cookie, TIME_T when, void* data)
	{
		TIME_T nowTime = (TIME_T)time(NULL);
		return this->start(name, cookie, when-nowTime, data);
	}

	void setTimerAutoStop(DWORD name, Identity64 cookie, bool bIsAutoStop)
	{
		KLogicTimer* tm = this->get(name, cookie);
		if(tm!=NULL)
			tm->m_isAutoStop = bIsAutoStop;
	}

	void restart(DWORD name, Identity64 cookie)
	{
		KLogicTimer* tm = this->get(name, cookie);
		if(tm!=NULL)
			tm->restart();
	}

	void stop(DWORD name, Identity64 cookie)
	{
		KLogicTimer* tm = this->get(name, cookie);
		if(tm==NULL)
			return;
		this->erase(tm);
		KLogicTimer::Free(tm);
	}

	void clear()
	{
		for(int i=this->size()-1; i>=0; --i)
		{
			KLogicTimer* tm = this->at(i);
			KLogicTimer::Free(tm);
		}
		base_type::clear();
	}
};

class KLogicTimerManager : private KTimerpArray<KLogicTimerManager>
{
public:
	typedef KTimerpArray<KLogicTimerManager> base_type;

public:
	KLogicTimerManager();
	~KLogicTimerManager();

protected:
	void breathe(int ms);

public:
	KLogicTimer* getTimer(DWORD name, Identity64 cookie);
	bool startTimer(DWORD name, Identity64 cookie, float secs, void* data);
	bool startmTimer(DWORD name, Identity64 cookie, int secs, void* data);
	bool startTimerAt(DWORD name, Identity64 cookie, TIME_T when, void* data);
	void stopTimer(DWORD name, Identity64 cookie);
	void clear();

public:
	virtual bool onTimer(DWORD name, Identity64 cookie, void* data) { return true; }
};

#endif