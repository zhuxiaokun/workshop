#include "KTPWorkflow.h"
#include "KThreadProcessor.h"
#include "KTPJobList.h"
namespace System { namespace Thread {

	KTPWorkflow::KTPWorkflow()
		:m_iPriority(0)
		,m_Serializer(this)
		,m_pThreadProcessor(NULL)
		,m_bComplete(false)
		,m_bSerialExecute(false)
	{
		m_uiID = (static_cast<UINT32>(
			reinterpret_cast<UINT64>(this) & 0xFFFFFF));
	}
	
	KTPWorkflow::~KTPWorkflow()
	{
		ASSERT(THREADSTATE_IDLE == m_ThreadState || THREADSTATE_COMPLETED == m_ThreadState);
		RemoveAllTask();
	}

	void KTPWorkflow::Prepare()
	{
		int iTaskSize = m_TaskList.size();
		for(int i = 0;i<iTaskSize;++i)
		{
			m_TaskList.at(i)->Prepare();
		}
	}

	KTPTask* KTPWorkflow::CreateNewTask()
	{
		KTPTask* pTask = NULL;
		if(NULL != m_pThreadProcessor)
		{
			pTask = m_pThreadProcessor->GetObjPool().AllocTask();
		}
		else
		{
			pTask = new KTPTask;
		}
		AddTask(pTask);
		return pTask;
	}

	void KTPWorkflow::AddTask(KTPTask* pTask)
	{
		ASSERT( NULL != pTask && NULL == pTask->GetWorkflow());
		ASSERT(THREADSTATE_IDLE == m_ThreadState || THREADSTATE_COMPLETED == m_ThreadState);
		pTask->SetWorkflow(this);
		m_TaskList.push_back(pTask);
	}

	KTPTask* KTPWorkflow::FindTaskByIdx(int iTaskIdx)
	{
		int iTaskSize = m_TaskList.size();
		ASSERT(iTaskIdx>=0 && iTaskIdx<iTaskSize );
		return m_TaskList[iTaskIdx];
	}

	int KTPWorkflow::FindTaskIdx(KTPTask* pTask)
	{
		int iRet = 0;
		int iTaskSize = m_TaskList.size();
		for(int i = 0;i<iTaskSize;++i)
		{
			if( m_TaskList.at(i) == pTask )
			{
				iRet = i;
				break;
			}
		}

		return iTaskSize;
	}

	void KTPWorkflow::RemoveTask(int iTaskIdx)
	{
		int iTaskSize = m_TaskList.size();
		ASSERT(THREADSTATE_IDLE == m_ThreadState || THREADSTATE_COMPLETED == m_ThreadState);
		ASSERT(iTaskIdx>=0 && iTaskIdx<iTaskSize );
		m_TaskList[iTaskIdx]->Clear();
		m_TaskList.erase(iTaskIdx);
	}

	void KTPWorkflow::RemoveAllTask()
	{
		ASSERT(THREADSTATE_IDLE == m_ThreadState || THREADSTATE_COMPLETED == m_ThreadState);
		int iTaskSize = m_TaskList.size();
		for(int i = 0;i<iTaskSize;++i)
		{
			m_TaskList.at(i)->Clear();
		}
		m_TaskList.clear();
	}

	void KTPWorkflow::ThreadExecute()
	{
		//
		SortTask();
		Prepare();
		
		if(!m_bSerialExecute)
		{
			bool bYield = false;
			int iTaskSize = m_TaskList.size();
			
			for(int i = 0;i<iTaskSize;++i)
			{
				for(int j = i;j<iTaskSize;++j)
				{
					if( m_TaskList[i]->GetStage() != m_TaskList[j]->GetStage())
					{
						bYield = true;
						break;
					}
				}
			}

			SetThreadState(THREADSTATE_RUNNING);

			for(int i = 0;i<iTaskSize;++i)
			{
				KTPTask::KTPJobListSet& JobListSet = m_TaskList[i]->GetJobListSet();
				
				int iJobListSize = JobListSet.size();
				
				
				for(int j = 0;j<iJobListSize;++j)
				{
					if(!bYield)
					{
						JobListSet[j]->SetWorkflowID(KIThreadProcessorUnit::NO_YIELD_ID);
					}
					else
					{
						JobListSet[j]->SetWorkflowID(GetID());
					}
					JobListSet[j]->SetStage(m_TaskList[i]->GetStage());
					m_pThreadProcessor->ExecuteProcessor(JobListSet[j]);
				}
				
			}
		}
		else
		{
			m_Serializer.SetWorkflowID(KIThreadProcessorUnit::NO_YIELD_ID);
			m_Serializer.SetStage(0);
			m_pThreadProcessor->ExecuteProcessor(&m_Serializer);
		}
		//
		//OutputDebugString("Workflow Execute");
	}

	void KTPWorkflow::ExecuteSerial()
	{
		int iTaskSize = m_TaskList.size();
		for(int i = 0;i<iTaskSize;++i)
		{
			KTPTask::KTPJobListSet& JobListSet = m_TaskList[i]->GetJobListSet();
			int iJobListSize = JobListSet.size();
			for(int j = 0;j<iJobListSize;++j)
			{
				JobListSet[j]->ThreadExecute();
				JobListSet[j]->SetThreadState(THREADSTATE_COMPLETED);
			}
		}
		m_bComplete = true;
	}

	void KTPWorkflow::CompleteInThread()
	{
		//OutputDebugString("Workflow Running\n");
		// do nothing
	}

	void KTPWorkflow::DealComplete()
	{
		//OutputDebugString("KTPWorkflow::DealComplete\n");
	}

	void KTPWorkflow::NotifyTaskComplete()
	{
		//OutputDebugString("Workflow NotifyTaskComplete\n");
		m_CompleteMutex.Lock();
		if(!m_bComplete)
		{
			bool bComplete = true;
			int iTaskSize = m_TaskList.size();
			for(int i = 0;i<iTaskSize;++i)
			{
				if(!m_TaskList[i]->IsComplete())
				{
					bComplete = false;
					break;
				}
			}

			m_bComplete = bComplete;
			if(m_bComplete)
			{
				m_pThreadProcessor->SubmitForComplete(this);
			}
			else
			{
				int h = 0;
			}
		}
		else
		{
			int h = 0;
		}
		m_CompleteMutex.Unlock();
	}

	void KTPWorkflow::SortTask()
	{
		int iTaskSize = m_TaskList.size();

		for(int i = 0;i<iTaskSize;++i)
		{
			for(int j = i;j<iTaskSize;++j)
			{
				if( m_TaskList[i]->GetStage() > m_TaskList[j]->GetStage())
				{
					KTPTaskPtr pTempTask = m_TaskList[i];
					m_TaskList[i] = m_TaskList[j];
					m_TaskList[j] = pTempTask;
				}
			}
		}
	}

	void KTPWorkflow::Reset()
	{
		m_bComplete = false;
		int iTaskSize = m_TaskList.size();
		for(int i = 0;i<iTaskSize;++i)
		{
			m_TaskList[i]->Reset();
		}
	}

} /*Thread*/ } /* System */