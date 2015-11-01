#include "KEventEntity.h"
#include "../KAttrParser.h"
#include "../ObjectFactory/KObjectFactory.h"

KEventEntity::KEventEntity()
{
	TimerEntity::m_receiver = this;
}

KEventEntity::~KEventEntity()
{

}

bool KEventEntity::initialize(const char* initAttrs, KStreamSession* ss)
{
	return this->onInitialize(initAttrs, ss);
}

void KEventEntity::finaize()
{
	this->onFinalize();
}

void KEventEntity::destroy()
{
	delete this;
}

bool KEventEntity::reload()
{
	return false;
}

bool KEventEntity::onInitialize(const char* initAttrs, KStreamSession* ss)
{
	return true;
}

void KEventEntity::onFinalize()
{
	return;
}

void KEventEntity::breathe(int ms)
{
	TimerEntity::breathe(ms);
}

bool KEventEntity::careEvent(KEventSource* src, int evtId)
{
	return src->regEvent(evtId, this);
}

bool KEventEntity::uncareEvent(KEventSource* src, int evtId)
{
	return src->unregEvent(evtId, this);
}

bool KEventEntity::startTimer(DWORD name, Identity64 cookie, float secs, void* data)
{
	return TimerEntity::start(name, cookie, (int)secs, data) != NULL;
}

bool KEventEntity::startTimerAt(DWORD name, Identity64 cookie, TIME_T when, void* data)
{
	return TimerEntity::startAt(name, cookie, when, data) != NULL;
}

void KEventEntity::stopTimer(DWORD name, Identity64 cookie)
{
	TimerEntity::stop(name, cookie);
}

void KEventEntity::clearTimer()
{
	TimerEntity::clear();
}

bool KEventEntity::onEvent(int evtId, const void* data, size_t len)
{
	return true;
}

bool KEventEntity::onTimer(DWORD name, Identity64 cookie, const void* data)
{
	return true;
}

bool KEventEntity::postEvent(int evtId, const void* data, int len)
{
	return this->onEvent(evtId, data, len);
}

void KEventEntity::handleEvent(int evtId, const void* data, int len)
{
	this->onEvent(evtId, data, len);
}

//////////////////////////////////////////////////////////////////////////

KEventEntity_luaMaster::KEventEntity_luaMaster():m_pAssist(NULL)
{

}

KEventEntity_luaMaster::~KEventEntity_luaMaster()
{

}

bool KEventEntity_luaMaster::initialize(const char* initAttrs, KStreamSession* ss)
{
	KAttrParser parser;
	if(!parser.Parse(initAttrs))
	{
		Log(LOG_ERROR, "error: KEventEntity_luaMaster::initialize(%s)", initAttrs);
		return false;
	}

	const char* cppClass = parser.GetString("c++", NULL);
	if(cppClass)
	{
		m_pAssist = (KEventEntityAssist*)KObjectFactory::GetInstance().Create(cppClass);
		if(!m_pAssist)
		{
			Log(LOG_ERROR, "error: KEventEntityAssist c++ class %s not found", cppClass);
			return false;
		}
		m_pAssist->finaize();
	}

	if(m_pAssist)
	{
		m_pAssist->m_pMaster = this;
		if(!m_pAssist->initialize(initAttrs, ss))
		{
			m_pAssist->destroy();
			m_pAssist = NULL;
			return false;
		}
	}

	lua_State* L = LuaWraper.m_l;
	KLuaStackRecover sr(L);

	char tbname[128];
	if(!this->getLuaClass(tbname, sizeof(tbname))) return false;
	if(!this->newTableInstance(tbname)) return false;
	this->initLuaHandlers();

	//if(m_pAssist) this->setTableField<KEventEntityAssist>("assist", m_pAssist);
	//this->setTableField<KEventEntity_luaMaster>("this", this);

	this->setLuaRuntime();
	this->onInitialize(initAttrs, ss);
	return true;
}

void KEventEntity_luaMaster::finalize()
{
	if(!m_luaRef) return;
	this->onFinalize();
	if(m_pAssist)
	{
		m_pAssist->destroy();
		m_pAssist = NULL;
	}
	this->clearLuaHandlers();
	this->releaseTableInstance();
}

bool KEventEntity_luaMaster::onInitialize(const char* initAttrs, KStreamSession* ss)
{
	if(m_pAssist) m_pAssist->onInitialize(initAttrs, ss);
	this->tableCall("onInitialize", initAttrs, ss);
	return true;
}

void KEventEntity_luaMaster::onFinalize()
{
	if(m_pAssist) m_pAssist->onFinalize();
	this->tableCall("onFinalize");
}

bool KEventEntity_luaMaster::onEvent(int evtId, const void* data, size_t len)
{
	if(this->luaHandleEvent(evtId, data, (int)len))
		return true;
	if(m_pAssist)
		return m_pAssist->onEvent(evtId, data, len);
	return false;
}

bool KEventEntity_luaMaster::onTimer(DWORD name, Identity64 cookie, const void* data)
{
	if(this->luaHandleTimer(name, cookie, data))
		return true;
	if(m_pAssist)
		return m_pAssist->onTimer(name, cookie, data);
	return false;
}