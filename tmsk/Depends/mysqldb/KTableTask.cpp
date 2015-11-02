#include "KTableTask.h"
#include <System/Log/log.h>
using namespace System::Sync;

/////

void KTableTaskOwner::on_tableTask_finish(KTableTask* task)
{
	task->on_finish();
}

void KTableTaskOwner::on_tableTask_timeout(KTableTask* task)
{
	task->on_timeout();
}

/////

KTableTask::KTableTask()
	: m_seq(0)
	, m_errCode(0)
	, m_refCount(0)
	, m_wantCount(0)
	, m_msRemain(0)
	, m_pTaskOwner(NULL)
{

}

KTableTask::~KTableTask()
{

}

void KTableTask::notifyFinish()
{
	//Log(LOG_WARN, "finish: %s seq:%u err:%u", this->toString().c_str(), m_seq, m_errCode);
	if(m_pTaskOwner) m_pTaskOwner->on_tableTask_finish(this);
	else this->on_finish();
}

void KTableTask::notifyTimeout()
{
	//Log(LOG_WARN, "timeout: %s seq:%u err:%u", this->toString().c_str(), m_seq, m_errCode);
	if(m_pTaskOwner) m_pTaskOwner->on_tableTask_timeout(this);
	else this->on_timeout();
}

int_r KTableTask::addRef()
{
	KAutoThreadMutex mx(m_mx);
	m_refCount++;
	return m_refCount;
}

int_r KTableTask::releaseRef()
{
	KAutoThreadMutex mx(m_mx);
	m_refCount--;
	return m_refCount;
}

int_r KTableTask::addWant()
{
	KAutoThreadMutex mx(m_mx);
	m_wantCount++;
	return m_wantCount;
}

int_r KTableTask::decWant()
{
	KAutoThreadMutex mx(m_mx);
	m_wantCount--;
	return m_wantCount;
}