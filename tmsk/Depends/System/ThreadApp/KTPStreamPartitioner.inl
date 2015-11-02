inline UINT32 KTPStreamPartitioner::GetOptimalBlockCount()
{
	return m_uiOptimalBlockCount;
}

inline bool KTPStreamPartitioner::HasMoreBlocks()
{
	return (m_uiCurrentBlockIndex < m_uiTotalBlockCount);
}

inline void KTPStreamPartitioner::Reset()
{
	m_uiCurrentBlockIndex = 0;
}

inline UINT32 KTPStreamPartitioner::GetBlockIndex()
{
	return m_uiCurrentBlockIndex;
}

inline UINT32 KTPStreamPartitioner::GetTransferBlockCount()
{
	UINT32 iRet = m_uiTotalBlockCount - m_uiCurrentBlockIndex;
	if(iRet>m_uiOptimalBlockCount)
		iRet = m_uiOptimalBlockCount;

	return iRet;
}

inline UINT32 KTPStreamPartitioner::GetTotalBlockCount()
{
	return m_uiTotalBlockCount;
}


UINT32 KTPStreamPartitioner::AdvanceBlockIndex()
{
	UINT32 iTransferBlockCount = GetTransferBlockCount();
	m_uiCurrentBlockIndex += iTransferBlockCount;
	return iTransferBlockCount;
}

UINT32 KTPStreamPartitioner::GetTotalJobCount()
{
	UINT32 uiOldCurrentBlockIndex = m_uiCurrentBlockIndex;
	m_uiCurrentBlockIndex = 0;
	
	UINT32 uiRet = 0;
	bool bHasMoreData = true;
	do
	{
		// Get a job list and add to the job
		++uiRet;
		AdvanceBlockIndex();
		bHasMoreData = HasMoreBlocks();
	} 
	while (bHasMoreData);
	m_uiCurrentBlockIndex = uiOldCurrentBlockIndex;
	return uiRet;
}