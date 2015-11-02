#include "KThreadInstance.h"
#include "KThreadFunctor.h"

namespace System { namespace Thread {

	KThreadInstance::KThreadInstance(KIThreadFunctor* pThreadFunctor)
		:m_pThreadFunctor(pThreadFunctor)
	{
		ASSERT(NULL != m_pThreadFunctor)
	}
	
	KThreadInstance::~KThreadInstance()
	{
	}

	void  KThreadInstance::Execute()
	{
		m_pThreadFunctor->Execute(this);
	}


} /*Thread*/ } /* System */