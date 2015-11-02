#include "KTPTask.h"
#include "KThreadUnit.h"
#include "KTPJobList.h"
#include "KTPWorkflow.h"
#include "KThreadProcessor.h"
#include "KTPObjPool.h"
namespace System { namespace Thread {

	const UINT32 KTPTask::AUTO_BLOCK_COUNT = 0xFFFFFFFF;

	KTPTask::KTPTask()
		:m_pWorkflow(0)
		,m_usStage(0)
		,m_pKernel(NULL)
		,m_uiOptimalBlockCount(AUTO_BLOCK_COUNT)
		,m_pThreadProcessor(NULL)
		,m_bComplete(false)
		,m_bNeedReset(true)
	{
	}

	KTPTask::~KTPTask()
	{
		Clear();
		RemoveAllInput();
		RemoveAllOutput();
	}

	void KTPTask::Prepare()
	{
		if(!m_bNeedReset)
		{
			return;
		}
		
		ASSERT( NULL != m_pWorkflow);
		KThreadProcessor* pThreadProcessor = m_pWorkflow->GetThreadProcessor();
		KTPObjPool& ObjPool = pThreadProcessor->GetObjPool();
		
		m_StreamPartitioner.Initialize(this);
		UINT32 iTotalBlockCount = m_StreamPartitioner.GetTotalBlockCount();
		UINT32 iJobCount = m_StreamPartitioner.GetTotalJobCount();

		AllocJobList(iJobCount);

		if( 0 == iTotalBlockCount )
		{
			KTPJobList* pJobList = m_JobListSet[0];
			KTPWorkload& Workload = pJobList->GetWorkload();
			m_StreamPartitioner.Partition(&Workload);
			ASSERT( NULL != m_pKernel );
			Workload.SetKernel(m_pKernel);
		}
		else
		{
			bool bHasMoreData = true;
			int iIndex = 0;
			do
			{
				// Get a job list and add to the job
				KTPJobList* pJobList = m_JobListSet[iIndex];
				
				KTPWorkload& Workload = pJobList->GetWorkload();
				Workload.Reset(m_InputStreams.size(),m_OutputStreams.size());
				m_StreamPartitioner.Partition(&Workload);
				ASSERT( NULL != m_pKernel );
				Workload.SetKernel(m_pKernel);
				UINT32 iBlockCount = m_StreamPartitioner.AdvanceBlockIndex();
				bHasMoreData = m_StreamPartitioner.HasMoreBlocks();
				++iIndex;
			}
			while (bHasMoreData);
		}
		
		m_bNeedReset = false;
	}

	void KTPTask::Clear()
	{
		if(NULL != m_pThreadProcessor)
		{
			KTPObjPool& ObjPool = m_pThreadProcessor->GetObjPool();

			for(int i = 0;i<m_JobListSet.size();++i)
			{
				m_JobListSet[i]->SetTask(NULL);
				ObjPool.FreeJobList(m_JobListSet[i]);
			}
			m_JobListSet.clear();
		}
	}

	void KTPTask::NotifyJobListComplete()
	{
		bool bComplete = true;
		int iJobListSize = m_JobListSet.size();
		for(int j = 0;j<iJobListSize;++j)
		{
			if(KIThreadUnit::THREADSTATE_COMPLETED != m_JobListSet.m_vals[j]->GetThreadState())
			{
				bComplete = false;
				break;
			}
		}

		m_bComplete = bComplete;
		if(m_bComplete)
		{
			m_pWorkflow->NotifyTaskComplete();
		}
			
		//OutputDebugString("KTPTask::NotifyJobListComplete\n");
	}

	void KTPTask::AllocJobList(UINT32 iCount)
	{
		KThreadProcessor* pThreadProcessor = m_pWorkflow->GetThreadProcessor();
		KTPObjPool& ObjPool = pThreadProcessor->GetObjPool();


		UINT32 iSize = (UINT32)(m_JobListSet.size()); 
		if( iSize>iCount )
		{
			for(UINT32 i = iCount;i<iSize;++i)
			{
				KTPJobList* pJobList = m_JobListSet.pop_back();
				pJobList->SetTask(NULL);
				ObjPool.FreeJobList(pJobList);
			}
		}
		else if(iSize<iCount )
		{
			for(UINT32 i = iSize;i<iCount;++i)
			{
				KTPJobList* pJobList = ObjPool.AllocJobList();
				pJobList->SetTask(this);
				m_JobListSet.push_back(pJobList);
			}
		}
	}

	void KTPTask::DeleteThis()
	{
		if(NULL != m_pThreadProcessor)
		{
			Clear();
			RemoveAllInput();
			RemoveAllOutput();
			m_pThreadProcessor->GetObjPool().FreeTask(this);
		}
		else
		{
			delete this;
		}
	}

	void KTPTask::Reset()
	{
		m_bComplete = false;
	}

} /*Thread*/ } /* System */