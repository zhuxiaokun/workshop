#ifndef __KTPKERNEL_H__
#define __KTPKERNEL_H__
#include "KTPWorkload.h"
namespace System { namespace Thread {
	
	class KITPKernel
	{
	public:
		KITPKernel(){}
		virtual ~KITPKernel(){}
		virtual void Execute(KTPWorkload& Workload) = 0;
	};

} /*Thread*/ } /* System */

#endif