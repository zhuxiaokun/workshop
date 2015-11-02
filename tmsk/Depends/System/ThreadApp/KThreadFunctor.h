#ifndef __KTHREADFUNCTOR_H__
#define __KTHREADFUNCTOR_H__
#include "../Thread/KThread.h"
namespace System { namespace Thread {

	class KThreadInstance;
	class KIThreadFunctor
	{
	public:
		KIThreadFunctor();
		virtual ~KIThreadFunctor();
		// 线程的执行函数，子类必须重写。
		virtual void  Execute(KThreadInstance* pThreadInstance) = 0;

	};


} /*Thread*/ } /* System */
#endif