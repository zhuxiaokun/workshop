#ifndef __KTPKERNELMACROS_H__
#define __KTPKERNELMACROS_H__

//---------------------------------------------------------------------------
#include "KTPKernel.h"
#include "KTPWorkload.h"

//---------------------------------------------------------------------------
// Macro to declare an TP Kernel
//---------------------------------------------------------------------------
typedef JG_T::KITPKernel KITPKernelBase;
#define KTPDeclareKernel(KernelName) \
class KernelName : public KITPKernelBase { \
public: \
	KernelName() : KITPKernelBase(){} \
	virtual ~KernelName(){} \
	virtual void Execute(JG_T::KTPWorkload& Workload); \
}; 
#define KTPDeclareKernelLib(KernelName, entrytag) \
class entrytag KernelName : public KITPKernelBase { \
public: \
	KernelName() : KITPKernelBase(){} \
	virtual ~KernelName(){} \
	virtual void Execute(JG_T::KTPWorkload& Workload); \
}; 


#define KTPBeginKernelImpl(KernelName) void KernelName::Execute( \
	JG_T::KTPWorkload& Workload)


#define KTPEndKernelImpl(KernelName)

#endif