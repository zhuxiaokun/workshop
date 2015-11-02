#include "KObjectState.h"

KObjectState::KObjectState():m_sid(cs_none),m_object(NULL)
{

}

KObjectState::~KObjectState()
{

}

void KObjectState::onEnterState()
{

}

void KObjectState::onLeaveState()
{

}

void KObjectState::onCommand(int cmd, const void* data, int len)
{

}

void KObjectState::onEvent(int evt, const void* data, int len)
{

}

void KObjectState::breathe(int interval)
{

}

void KObjectState::reset()
{
	//m_sid = cs_none;
	m_object = NULL;
}

void KObjectState::destroy()
{
	this->reset();
	delete this;
}
