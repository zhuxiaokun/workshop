#pragma once
#include "KEventSystem.h"
#include "KLuaEventHandler.h"
#include "../Timer/KLogicTimer.h"
#include "../CommSession/KStreamSession.h"

class KEventEntity
	: public KEventHandler
	, private KTimerpArray<KEventEntity>
	, private KRealTimerpArray<KEventEntity>
{
public:
	typedef KTimerpArray<KEventEntity> TimerEntity;
	typedef KRealTimerpArray<KEventEntity> RealTimerEntity;

public:
	KEventEntity();
	virtual ~KEventEntity();

public:
	virtual bool initialize(const char* initAttrs, KStreamSession* ss);
	virtual void finaize();
	virtual void destroy();
	virtual bool reload();

public:
	virtual bool onInitialize(const char* initAttrs, KStreamSession* ss);
	virtual void onFinalize();
	void breathe(int ms);

public:
	virtual bool careEvent(KEventSource* src, int evtId);
	virtual bool uncareEvent(KEventSource* src, int evtId);

public:
	virtual bool startTimer(DWORD name, Identity64 cookie, float secs, void* data);
	virtual bool startTimerAt(DWORD name, Identity64 cookie, TIME_T when, void* data);
	virtual void stopTimer(DWORD name, Identity64 cookie);
	virtual void clearTimer();

public:
	virtual bool startRealTimer(DWORD name, Identity64 cookie,int mode, int val, int hour, int minute, int second, void* data);	
	virtual void stopRealTimer(DWORD name, Identity64 cookie);
	virtual void clearRealTimer();

public:
	virtual bool onEvent(int evtId, const void* data, size_t len);
	virtual bool onTimer(DWORD name, Identity64 cookie, const void* data);
	virtual bool onRealTimer(DWORD name, Identity64 cookie, const void* data);
	bool postEvent(int evtId, const void* data, int len);
	KEventHandler* toEventHandler() { return this; }
	void handleEvent(int evt, const void* data, int len);

public:
	BeginDefLuaClass(KEventEntity);
		DefMemberFunc(careEvent);
		DefMemberFunc(uncareEvent);
		DefMemberFunc(startTimer);
		DefMemberFunc(startTimerAt);
		DefMemberFunc(stopTimer);
		DefMemberFunc(clearTimer);
		DefMemberFunc(startRealTimer);
		DefMemberFunc(stopRealTimer);
		DefMemberFunc(clearRealTimer);
		DefMemberFunc(toEventHandler);
		DefMemberFunc(postEvent);
	EndDef;
};

class KEventEntityAssist : public KEventEntity
{
public:
	KEventEntity* m_pMaster;

public:
	KEventEntityAssist():m_pMaster(NULL)
	{

	}

public:
	bool initialize(const char* initAttrs, KStreamSession* ss)
	{
		return true;
	}
	void breathe(int ms)
	{
	}
	bool careEvent(KEventSource* src, int evtId)
	{
		return src->regEvent(evtId, m_pMaster);
	}
	bool uncareEvent(KEventSource* src, int evtId)
	{
		return src->unregEvent(evtId, m_pMaster);
	}

public:
	bool startTimer(DWORD name, Identity64 cookie, float secs, void* data)
	{
		return m_pMaster->startTimer(name, cookie, secs, data);
	}
	bool startTimerAt(DWORD name, Identity64 cookie, TIME_T when, void* data)
	{
		return m_pMaster->startTimerAt(name, cookie, when, data);
	}
	void stopTimer(DWORD name, Identity64 cookie)
	{
		m_pMaster->stopTimer(name, cookie);
	}
	void clearTimer()
	{
		m_pMaster->clearTimer();
	}

public:
	bool startRealTimer(DWORD name, Identity64 cookie,int mode, int val, int hour, int minute, int second, void* data)
	{
		return m_pMaster->startRealTimer(name, cookie, mode, val, hour, minute, second, data);
	}
	void stopRealTimer(DWORD name, Identity64 cookie)
	{
		m_pMaster->stopRealTimer(name, cookie);
	}
	void clearRealTimer()
	{
		m_pMaster->clearRealTimer();
	}

public:
	BeginDefLuaClass(KEventEntityAssist);
		InheritFrom(KEventEntity);
	EndDef;
};

class KEventEntity_luaMaster
	: public KEventEntity
	, public KLuaEventHandler<KEventEntity_luaMaster>
{
public:
	KEventEntityAssist* m_pAssist;

public:
	KEventEntity_luaMaster();
	~KEventEntity_luaMaster();

public:
	virtual bool getLuaClass(char* name, size_t len) { ASSERT(FALSE); return false; }
	// export this, assist into lua table instance (self)
	virtual bool setLuaRuntime() { ASSERT(FALSE); return false; }

public:
	bool initialize(const char* initAttrs, KStreamSession* ss);
	void finalize();

public:
	bool onInitialize(const char* initAttrs, KStreamSession* ss);
	void onFinalize();

public:
	bool onEvent(int evtId, const void* data, size_t len);
	bool onTimer(DWORD name, Identity64 cookie, const void* data);
	bool onRealTimer(DWORD name, Identity64 cookie, const void* data);

public:
	BeginDefLuaClass(KEventEntity_luaMaster);
		InheritFrom(KEventEntity);
	EndDef;
};