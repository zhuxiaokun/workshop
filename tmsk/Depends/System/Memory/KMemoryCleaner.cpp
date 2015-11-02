#include "KMemory.h"
#include "KMemoryCleaner.h"
#include "../Sync/KSync.h"

//using namespace ::System::Sync;

namespace System { namespace Memory {

KMemoryCleaner* KMemoryCleaner::m_pInstance = NULL;

KMemoryCleaner::KMemoryCleaner()
{
}

KMemoryCleaner* KMemoryCleaner::GetInstance()
{
	static JG_S::KMTLock m_mx;
	static volatile BOOL m_inited = FALSE;
	if(m_inited) return m_pInstance;

	m_mx.Lock();
	if(m_inited)
	{
		m_mx.Unlock();
		return m_pInstance;
	}

	m_pInstance = new KMemoryCleaner();
	m_inited = TRUE;

	m_mx.Unlock();
	return m_pInstance;
}

void KMemoryCleaner::Destroy()
{
	if(m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

KMemoryCleaner::~KMemoryCleaner()
{
	this->Clean();
}

void KMemoryCleaner::AddDestroyer(IObjectDestroyer* p)
{
	JG_S::KAutoThreadMutex mx(m_x);
	m_vec.push_back(p);
}

void KMemoryCleaner::Clean()
{
	JG_S::KAutoThreadMutex mx(m_x);
	for(int i=0; i<m_vec.size(); i++)
	{
		IObjectDestroyer* p = m_vec[i];
		p->Destroy();
	}
	for(int i=0; i<m_vec.size(); i++) delete m_vec[i];
	m_vec.clear();
}

}}