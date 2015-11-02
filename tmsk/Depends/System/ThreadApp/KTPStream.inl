inline void* KTPStream::GetData()
{
	return m_pvData;
}

inline void KTPStream::SetData(void* pvData)
{
	if(m_pvData != pvData)
	{
		m_pvData = pvData;
		NotifyDependentTasks();
	}
}

inline UINT32 KTPStream::GetElementOffset() const
{
	return m_uiElementOffset;
}

inline void KTPStream::SetElementOffset(UINT32 uiElementOffset)
{
	m_uiElementOffset = uiElementOffset;

}


inline UINT16 KTPStream::GetStride()
{
	return m_uiStride;
}

inline void KTPStream::SetStride(UINT16 uiStride)
{
	if(m_uiStride != uiStride)
	{
		m_uiStride = uiStride;
		NotifyDependentTasks();
	}
}


inline UINT32 KTPStream::GetBlockCount()
{
	return m_uiBlockCount;
}


inline UINT32 KTPStream::GetDataSize()
{
	return m_uiDataSize;
}

inline void KTPStream::SetBlockCount(UINT32 uiBlockCount)
{
	if( m_uiBlockCount != uiBlockCount)
	{
		m_uiBlockCount = uiBlockCount;
		NotifyDependentTasks();
	}
}

inline bool KTPStream::IsFixedInput()
{
	return m_bIsFixedInput;
}

//---------------------------------------------------------------------------
inline void KTPStream::AddInput(KTPTask* pkTask)
{
	m_Inputs.insert(pkTask);
}
//---------------------------------------------------------------------------
inline void KTPStream::RemoveInput(KTPTask* pkTask)
{
	m_Inputs.erase(pkTask);	
}
//---------------------------------------------------------------------------
inline void KTPStream::AddOutput(KTPTask* pkTask)
{
	m_Outputs.insert(pkTask);
}
//---------------------------------------------------------------------------
inline void KTPStream::RemoveOutput(KTPTask* pkTask)
{
	m_Outputs.erase(pkTask);	
}

inline void KTPStream::ClearTaskArrays()
{
	RemoveFromDependentTasks();
	m_Inputs.clear();
	m_Outputs.clear();
}

inline void KTPStream::SetThreadProcessor(KThreadProcessor* pThreadProcessor)
{ 
	m_pThreadProcessor = pThreadProcessor;
}