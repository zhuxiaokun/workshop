#ifndef __KTHREADINSTANCE_H__
#define __KTHREADINSTANCE_H__
#include "../Thread/KThread.h"
namespace System { namespace Thread {

	class KIThreadFunctor;
	class KThreadInstance : public  KThread
	{
	public:
		KThreadInstance(KIThreadFunctor* pThreadFunctor);
		virtual ~KThreadInstance();
	protected:
		virtual void			Execute();
	protected:
		KIThreadFunctor* m_pThreadFunctor;

	};


} /*Thread*/ } /* System */
#endif