#include "RWMutex.h"

RWMutex::RWMutex():m_num(0x7FFFFFFF),m_bWriteWaiting(FALSE)
{
}

RWMutex::~RWMutex()
{
}

BOOL RWMutex::WaitRead()
{
	while(!this->WaitRead(10));
	return TRUE;
}

BOOL RWMutex::WaitWrite()
{
	while(!this->WaitWrite(10));
	return TRUE;
}

BOOL RWMutex::WaitRead(int ms)
{
	m_mx.Lock();
	if(!m_bWriteWaiting && m_num > 0)
	{
		m_num -= 1;
		m_mx.Unlock();
		return TRUE;
	}
	m_mx.Unlock();

	m_event.Block(ms);

	m_mx.Lock();
	if(!m_bWriteWaiting && m_num > 0)
	{
		m_num -= 1;
		m_mx.Unlock();
		return TRUE;
	}
	m_mx.Unlock();

	return FALSE;
}

BOOL RWMutex::WaitWrite(int ms)
{
	m_mx.Lock();
	if(m_num >= 0x7FFFFFFF)
	{
		m_num = 0;
		m_bWriteWaiting = FALSE;
		m_mx.Unlock();
		return TRUE;
	}
	m_mx.Unlock();

	m_event.Block(ms);

	m_mx.Lock();
	if(m_num >= 0x7FFFFFFF)
	{
		m_num = 0;
		m_bWriteWaiting = FALSE;
		m_mx.Unlock();
		return TRUE;
	}
	else
	{
		int nRand = m_random.GetRandom(0,100);
		if(nRand < 20)
			m_bWriteWaiting = TRUE;
	}
	m_mx.Unlock();

	return FALSE;
}

void RWMutex::ReadPost()
{
	m_mx.Lock();
	if(m_num >= 0x7FFFFFFF) { ASSERT(FALSE); }
	m_num += 1;
	//通知等待于写的线程，假定读线程是不可能被其它的读线程阻塞的
	if(m_num == 0x7FFFFFFF) m_event.Set();
	m_mx.Unlock();
}

void RWMutex::WritePost()
{
	m_mx.Lock();
	if(m_num != 0) { ASSERT(FALSE); }
	m_num = 0x7FFFFFFF;
	m_event.Set();
	m_mx.Unlock();
}

BOOL RWMutex::WaitWriteInRead(int ms)
{
	m_mx.Lock();
	if(m_num >= 0x7FFFFFFE)
	{
		m_num = 0;
		m_bWriteWaiting = FALSE;
		m_mx.Unlock();
		return TRUE;
	}
	m_mx.Unlock();

	this->ReadPost();

	m_event.Block(ms);

	m_mx.Lock();
	if(m_num >= 0x7FFFFFFF)
	{
		m_num = 0;
		m_bWriteWaiting = FALSE;
		m_mx.Unlock();
		return TRUE;
	}
	else
	{
		int nRand = m_random.GetRandom(0,100);
		if(nRand < 20)
			m_bWriteWaiting = TRUE;
	}
	m_mx.Unlock();

	return FALSE;
}

void RWMutex::WritePost2Read()
{
	m_mx.Lock();
	if(m_num != 0) { ASSERT(FALSE); }
	m_num = 0x7FFFFFFE;
	m_event.Set();
	m_mx.Unlock();
}


AutoRMutex::AutoRMutex(RWMutex& rwmx):m_bInWrite(FALSE),m_rwmx(rwmx)
{
	while(!rwmx.WaitRead(10)) {}
}

AutoRMutex::~AutoRMutex()
{
	if(m_bInWrite)
		m_rwmx.WritePost();
	else
		m_rwmx.ReadPost();
}

BOOL AutoRMutex::WaitWrite()
{
	while(!m_rwmx.WaitWriteInRead(10)) {}
	m_bInWrite = TRUE;
	return TRUE;
}

void AutoRMutex::WritePost()
{
	m_rwmx.WritePost2Read();
	m_bInWrite = FALSE;
}

AutoWMutex::AutoWMutex(RWMutex& rwmx):m_rwmx(rwmx)
{
	while(!m_rwmx.WaitWrite(10)) {}
}

AutoWMutex::~AutoWMutex()
{
	m_rwmx.WritePost();
}