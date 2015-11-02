inline KTPWorkload::KTPWorkload()
:m_pKernel(NULL)
,m_pkInputsOutputs(NULL)
,m_uiInOutCount(0)
,m_uiInputCount(0)
,m_uiOutputCount(0)
,m_uiTotalBlockCount(0)
{
}

inline KTPWorkload::~KTPWorkload()
{
	Clear();
}

//---------------------------------------------------------------------------
template <class T>
inline T* KTPWorkload::GetInput(UINT16 uiInputIndex)
{
	KTPWorkload::Header& kHeader = m_pkInputsOutputs[uiInputIndex];

	return (T*)((char*)kHeader.m_pvBaseAddress + kHeader.m_uiDataOffset);
}
//---------------------------------------------------------------------------
template <class T> 
inline T* KTPWorkload::GetInputBase(UINT16 uiInputIndex)
{
	KTPWorkload::Header& kHeader = m_pkInputsOutputs[uiInputIndex];

	return (T*)kHeader.m_pvBaseAddress;
}
//---------------------------------------------------------------------------
inline UINT32 KTPWorkload::GetInputStride(UINT16 uiInputIndex)
{
	KTPWorkload::Header& kHeader = m_pkInputsOutputs[uiInputIndex];
	return kHeader.m_uiStride;
}

inline UINT32 KTPWorkload::GetInputBlockCount(UINT16 uiInputIndex)
{
	KTPWorkload::Header& kHeader = m_pkInputsOutputs[uiInputIndex];
	return kHeader.m_uiBlockCount;
};

//---------------------------------------------------------------------------
template <class T>
inline T* KTPWorkload::GetOutput(UINT16 uiOutputIndex)
{
	UINT32 uiIndex = uiOutputIndex + m_uiInputCount;
	KTPWorkload::Header& kHeader = m_pkInputsOutputs[uiIndex];

	return (T*)((char*)kHeader.m_pvBaseAddress + kHeader.m_uiDataOffset);
}
//---------------------------------------------------------------------------
template <class T> 
inline T* KTPWorkload::GetOutputBase(UINT16 uiOutputIndex)
{
	UINT32 uiIndex = uiOutputIndex + m_uiInputCount;
	KTPWorkload::Header& kHeader = m_pkInputsOutputs[uiIndex];

	return (T*)kHeader.m_pvBaseAddress;
}
//---------------------------------------------------------------------------
inline UINT32 KTPWorkload::GetOutputStride(UINT16 uiOutputIndex)
{
	UINT32 uiIndex = uiOutputIndex + m_uiInputCount;
	KTPWorkload::Header& kHeader = m_pkInputsOutputs[uiIndex];
	return kHeader.m_uiStride;
}

inline UINT32 KTPWorkload::GetOutputBlockCount(UINT16 uiOutputIndex)
{
	UINT32 uiIndex = uiOutputIndex + m_uiInputCount;
	KTPWorkload::Header& kHeader = m_pkInputsOutputs[uiIndex];
	return kHeader.m_uiBlockCount;
};


//---------------------------------------------------------------------------
inline UINT32 KTPWorkload::GetTotalBlockCount()
{
	return m_uiTotalBlockCount;
}
//---------------------------------------------------------------------------
inline UINT32 KTPWorkload::GetInputCount()
{
	return m_uiInputCount;
}
//---------------------------------------------------------------------------
inline UINT32 KTPWorkload::GetOutputCount()
{
	return m_uiOutputCount;
}
//---------------------------------------------------------------------------
inline KITPKernel* KTPWorkload::GetKernel()
{
	return m_pKernel;
}
//---------------------------------------------------------------------------
inline void KTPWorkload::SetKernel(KITPKernel* pkKernel)
{
	m_pKernel = pkKernel;
}

inline void KTPWorkload::Reset(UINT32 uiInputCount, UINT32 uiOutputCount)
{
	UINT32 uiInOutCount = uiInputCount + uiOutputCount;
	if (m_uiInOutCount < uiInOutCount )
	{
		if(NULL != m_pkInputsOutputs)
		{
			delete m_pkInputsOutputs;
			m_pkInputsOutputs = NULL;
		}

		m_pkInputsOutputs = (Header*) (new Header[uiInOutCount]);
		m_uiInOutCount = uiInOutCount;
	}
	m_uiInputCount = (UINT16) uiInputCount;
	m_uiOutputCount = (UINT16) uiOutputCount;
}

inline void KTPWorkload::Clear()
{
	if( NULL != m_pkInputsOutputs )
	{
		delete []m_pkInputsOutputs;
		m_pkInputsOutputs = NULL;
	}
	m_pkInputsOutputs = 0;
	m_uiInOutCount = 0;
	m_uiInputCount = 0;
	m_uiOutputCount = 0;
	m_uiTotalBlockCount = 0;
}