//---------------------------------------------------------------------------
inline void KTPTask::SetKernel(KITPKernel* pkKernel)
{
	if(m_pKernel != pkKernel)
	{
		m_pKernel = pkKernel;
		m_bNeedReset = true;
	}
}
//---------------------------------------------------------------------------
inline KITPKernel* KTPTask::GetKernel() const
{
	return m_pKernel;
}

inline void KTPTask::SetOptimalBlockCount(UINT32 uiBlockCount)
{
	if(m_uiOptimalBlockCount != uiBlockCount)
	{
		m_uiOptimalBlockCount = uiBlockCount;
		m_bNeedReset = true;
	}
}
//---------------------------------------------------------------------------
inline UINT32 KTPTask::GetOptimalBlockCount()
{
	return m_uiOptimalBlockCount;
}

inline UINT32 KTPTask::GetInputCount() const
{
	return ((UINT32)m_InputStreams.size());
}

inline UINT32 KTPTask::GetOutputCount() const
{
	return ((UINT32)m_OutputStreams.size());	
}

inline KTPStream* KTPTask::GetInputAt(UINT32 uiIndex) const
{
	ASSERT(uiIndex<(UINT32)(m_InputStreams.size()));
	return m_InputStreams[uiIndex];
}

inline KTPStream* KTPTask::GetOutputAt(UINT32 uiIndex) const
{
	ASSERT(uiIndex<(UINT32)(m_OutputStreams.size()));
	return m_OutputStreams[uiIndex];
}

inline void KTPTask::AddInput(KTPStream* pInputStream)
{
	int iFindIndex = -1;
	int iSize = m_InputStreams.size();
	for( int i = 0;i<iSize;++i )
	{
		if( pInputStream == m_InputStreams[i] )
		{
			iFindIndex = i;
			break;
		}
	}

	ASSERT(NULL != pInputStream && -1 == iFindIndex);
	m_InputStreams.push_back(pInputStream);
	pInputStream->AddInput(this);
	
	m_bNeedReset = true;
}


inline void KTPTask::AddOutput(KTPStream* pOutputStream)
{
	int iFindIndex = -1;
	int iSize = m_OutputStreams.size();
	for( int i = 0;i<iSize;++i )
	{
		if( pOutputStream == m_OutputStreams[i] )
		{
			iFindIndex = i;
			break;
		}
	}

	ASSERT(NULL != pOutputStream && -1 == iFindIndex);

	m_OutputStreams.push_back(pOutputStream);
	pOutputStream->AddOutput(this);
	
	m_bNeedReset = true;
}


inline void KTPTask::RemoveInput(KTPStream* pInputStream)
{
	if(RemoveInputImp(pInputStream))
	{
		pInputStream->RemoveInput(this);
		m_bNeedReset = true;
	}
}

inline void KTPTask::RemoveOutput(KTPStream* pOutputStream)
{
	if(RemoveOutputImp(pOutputStream))
	{
		pOutputStream->RemoveOutput(this);
		m_bNeedReset = true;
	}
}

inline void KTPTask::RemoveAllInput()
{
	int iSize = m_InputStreams.size();
	for( int i = 0;i<iSize;++i )
	{
		if( NULL != m_InputStreams[i] )
		{
			m_InputStreams[i]->RemoveInput(this);
		}
	}
	m_InputStreams.clear();
}

inline void KTPTask::RemoveAllOutput()
{
	int iSize = m_OutputStreams.size();
	for( int i = 0;i<iSize;++i )
	{
		if( NULL != m_OutputStreams[i] )
		{
			m_OutputStreams[i]->RemoveOutput(this);
		}
	}
	m_OutputStreams.clear();
}

bool KTPTask::RemoveInputImp(KTPStream* pInputStream)
{
	bool bRet = false;
	int iSize = m_InputStreams.size();
	for( int i = 0;i<iSize;++i )
	{
		if( pInputStream == m_InputStreams[i] )
		{
			m_InputStreams.erase(i);
			bRet = true;
			break;
		}
	}

	return bRet;
}

bool KTPTask::RemoveOutputImp(KTPStream* pOutputStream)
{
	bool bRet = false;
	int iSize = m_OutputStreams.size();
	for( int i = 0;i<iSize;++i )
	{
		if( pOutputStream == m_OutputStreams[i] )
		{
			m_OutputStreams.erase(i);
			bRet = true;
			break;
		}
	}
	return bRet;
}