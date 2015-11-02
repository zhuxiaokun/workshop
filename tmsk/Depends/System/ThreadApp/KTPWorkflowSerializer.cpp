#include "KTPWorkflowSerializer.h"
#include "KTPWorkflow.h"
#include "KThreadProcessor.h"
namespace System { namespace Thread {

	KTPWorkflowSerializer::KTPWorkflowSerializer(KTPWorkflow* pWorkflow)
		:m_pWorkflow(pWorkflow)
	{
		
	}

	KTPWorkflowSerializer::~KTPWorkflowSerializer()
	{

	}

	void KTPWorkflowSerializer::ThreadExecute()
	{
		m_pWorkflow->ExecuteSerial();
	}

	void KTPWorkflowSerializer::CompleteInThread()
	{
		SetThreadState(THREADSTATE_COMPLETED);
		m_pWorkflow->m_pThreadProcessor->SubmitForComplete(m_pWorkflow);
	}
	
} /*Thread*/ } /* System */