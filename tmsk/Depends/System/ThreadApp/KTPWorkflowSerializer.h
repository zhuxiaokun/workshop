#ifndef __KTPWORKFLOWSERIALIZER_H__
#define __KTPWORKFLOWSERIALIZER_H__
#include "KThreadProcessorUnit.h"
namespace System { namespace Thread {

	class KTPWorkflow;
	class KTPWorkflowSerializer : public KIThreadProcessorUnit
	{
	public:
		KTPWorkflowSerializer(KTPWorkflow* pWorkflow);
		virtual ~KTPWorkflowSerializer();
		virtual void ThreadExecute();
		virtual void CompleteInThread();
	protected:
		KTPWorkflow* m_pWorkflow;
	};

} /*Thread*/ } /* System */


#endif