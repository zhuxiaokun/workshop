#include "KSync.h"
#include "../KPlatform.h"

namespace System { namespace Sync {

//KRecursiveMutex::KRecursiveMutex()
//	: m_tid(0)
//	, m_refCount(0)
//{
//
//}
//
//KRecursiveMutex::~KRecursiveMutex()
//{
//
//}
//
//void KRecursiveMutex::Lock() const
//{
//	uint_r tid = getCurrentThreadId();
//	if(m_refCount && m_tid == tid)
//	{
//		(((KRecursiveMutex*)this)->m_refCount)++;
//		return;
//	}
//	m_mx.Lock();
//	(((KRecursiveMutex*)this)->m_refCount) = 1;
//	((KRecursiveMutex*)this)->m_tid = tid;
//}
//
//void KRecursiveMutex::Unlock() const 
//{
//	uint_r tid = getCurrentThreadId();
//	if(m_refCount && m_tid == tid)
//	{
//		(((KRecursiveMutex*)this)->m_refCount)--;
//		if(!m_refCount)
//		{
//			((KRecursiveMutex*)this)->m_tid = 0;
//			m_mx.Unlock();
//		}
//		return;
//	}
//}

}}