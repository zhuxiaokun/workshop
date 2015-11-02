inline KThreadUnitNode* KTPObjPool::AllocUnitNode()
{
	return m_pThreadUnitNodePool->Alloc();
}
inline void KTPObjPool::FreeUnitNode(KThreadUnitNode* pNode)
{ 
	m_pThreadUnitNodePool->Free(pNode);
}

inline KTPWorkflow* KTPObjPool::AllocWorkflow()
{
	KTPWorkflow* pWorkflow = m_pWorkflowPool->Alloc(); 
	pWorkflow->SetThreadProcessor(m_pThreadProcessor);
	return pWorkflow;
}

inline void  KTPObjPool::FreeWorkflow(KTPWorkflow* pWorkflow)
{ 
	pWorkflow->RemoveAllTask();
	pWorkflow->SetThreadProcessor(NULL);
	m_pWorkflowPool->Free(pWorkflow);
}

inline KTPTask*	 KTPObjPool::AllocTask()
{
	KTPTask* pTask = m_pTaskPool->Alloc();
	pTask->SetWorkflow(NULL);
	pTask->SetThreadProcessor(m_pThreadProcessor);
	return pTask;
}

inline void  KTPObjPool::FreeTask(KTPTask* pTask)
{ 
	pTask->SetThreadProcessor(NULL);
	m_pTaskPool->Free(pTask);
}

inline KTPStream* KTPObjPool::AllocStream()
{
	KTPStream* pStream = m_pStreamPool->Alloc();
	pStream->SetThreadProcessor(m_pThreadProcessor);
	return pStream;
}
inline void KTPObjPool::FreeStream(KTPStream* pStream)
{
	pStream->SetThreadProcessor(NULL);
	m_pStreamPool->Free(pStream);
}

inline KTPJobList*	KTPObjPool::AllocJobList()
{
	KTPJobList* pJobList = m_pJobListPool->Alloc();
	pJobList->SetTask(NULL);
	return pJobList;
}

inline void  KTPObjPool::FreeJobList(KTPJobList* pJobList)
{ 
	m_pJobListPool->Free(pJobList);
}

inline KThreadUnitNodePool* KTPObjPool::GetUnitNodePool()
{
	return m_pThreadUnitNodePool;
}

inline KTPWorkflowPool*	KTPObjPool::GetWorkflowPool()
{
	return m_pWorkflowPool;
}

inline KTPTaskPool*	  KTPObjPool::GetTaskPool()	 
{
	return m_pTaskPool;
}

inline KTPJobListPool*	KTPObjPool::GetJobListPool() 
{
	return m_pJobListPool;
}

inline KTPStreamPool*	KTPObjPool::GetStreamPool()
{
	return m_pStreamPool;
}

inline bool	KTPObjPool::IsInitialize()	 
{
	return NULL != m_pThreadUnitNodePool && NULL != m_pWorkflowPool && NULL != m_pTaskPool && NULL != m_pJobListPool && NULL != m_pStreamPool;
}