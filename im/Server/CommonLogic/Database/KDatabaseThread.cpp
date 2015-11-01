#include "KDatabaseThread.h"
#include <System/Log/log.h>

/// IDatabaseTask ///

KSqlTask::KSqlTask() : m_errCode(0)
{

}


KSqlTask::~KSqlTask()
{

}

void KSqlTask::reset()
{
	m_errCode = 0;
	//m_taskKind = 0;
}

// 用于内存池对象的回收和复用
void KSqlTask::destroy()
{
	this->reset();
	delete this;
}

DWORD KSqlTask::getLastError() const
{
	return m_errCode;
}

void KSqlTask::setLastError(DWORD errCode)
{
	m_errCode = errCode;
}

///

KDatabaseThread::KDatabaseThread():m_stopDesired(FALSE),m_syncEvent(NULL)
{

}

KDatabaseThread::~KDatabaseThread()
{

}

BOOL KDatabaseThread::initialize(const KSQLConnParam& cnnParam, JG_S::KSync_PulseEvent* syncEvent)
{
	if(!m_ctx.cnn.Connect(cnnParam))
	{
		Log(LOG_ERROR, "error: DatabaseAgent::Initialize, connect to database, %s",
			m_ctx.cnn.GetLastErrorMsg());
		return FALSE;
	}
	m_syncEvent = syncEvent;
	m_ctx.cmd.SetConnection(m_ctx.cnn);
	::System::Thread::KThread::Run();
	return TRUE;
}

void KDatabaseThread::finalize()
{

}

void KDatabaseThread::push(KSqlTask& task)
{
	m_taskQueue.push(&task);
}

KSqlTask* KDatabaseThread::pop()
{
	KSqlTask* task;
	return m_resultQueue.pop(task) ? task : NULL;
}

void KDatabaseThread::Execute()
{
	DWORD errCode;
	KSqlTask* task;
	while(!m_stopDesired)
	{
		if(!m_taskQueue.pop(task))
		{
			m_taskQueue.wait(50);
			continue;
		}
		
		if(!task->checkDepends())
		{
			task->setLastError(9999);
			m_resultQueue.push(task);
			if(m_syncEvent) m_syncEvent->Set();
			continue;
		}

		task->setLastError(0);
		task->execute(m_ctx);
		errCode = task->getLastError();
	
		if(errCode)
		{	// 出错了，检查数据库连接，准备重连
			while(!m_stopDesired && !this->isConnActive())
			{
				if(this->reconnect())
				{	// 因为数据库不可用而导致的操作失败，重试一次
					task->setLastError(0);
					task->execute(m_ctx);
					break;
				}
				Log(LOG_ERROR, "error: KDatabaseThread, database not avail, reconnect after 5 seconds");
				msleep(5000);
			}
		}
		
		m_resultQueue.push(task);
		if(m_syncEvent) m_syncEvent->Set();
	}
}

void KDatabaseThread::Stop()
{
	m_stopDesired = TRUE;
	System::Thread::KThread::WaitFor();
}

void KDatabaseThread::Clear()
{
	this->Stop();
	m_taskQueue.clear();
	m_resultQueue.clear();
}

BOOL KDatabaseThread::isConnActive()
{
	m_ctx.cmd.Reset();
	return m_ctx.cnn.IsActive();
}

BOOL KDatabaseThread::reconnect()
{
	m_ctx.cmd.Reset();
	return m_ctx.cnn.Reconnect();
}
