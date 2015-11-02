#include "KTPStream.h"
#include "KTPTask.h"
#include "KThreadProcessor.h"
namespace System { namespace Thread {

	KTPStream::KTPStream(UINT16 uiStride, bool bIsFixedInput)
		:m_pvData(NULL)
		,m_pThreadProcessor(NULL)
		,m_uiBlockCount(0)
		,m_uiElementOffset(0)
		,m_uiStride(uiStride)
		,m_uiDataSize(0)
		,m_bIsFixedInput(bIsFixedInput)
	{
		
	}

	KTPStream::~KTPStream()
	{
		ClearTaskArrays();
	}

	/*void KTPStream::Initialize(UINT16 uiStride, bool bIsFixedInput)
	{
		m_uiStride = uiStride;
		m_bIsFixedInput = bIsFixedInput;
	}*/

	void KTPStream::RemoveFromDependentTasks()
	{
		UINT32 uiInputCount = GetInputSize();
		for (UINT32 i = 0; i < uiInputCount; i++)
		{
			KTPTask* pkTask = GetInputAt(i);
			if (NULL != pkTask)
			{
				pkTask->RemoveInputImp(this);
			}
		}

		UINT32 uiOutputCount = GetOutputSize();
		for (UINT32 i = 0; i < uiOutputCount; i++)
		{
			KTPTask* pkTask = GetOutputAt(i);
			if (NULL != pkTask)
			{
				pkTask->RemoveOutputImp(this);
			}
		}
	}

	void KTPStream::NotifyDependentTasks()
	{
		UINT32 uiInputCount = GetInputSize();
		for (UINT32 i = 0; i < uiInputCount; i++)
		{
			KTPTask* pTask = GetInputAt(i);
			if (pTask)
			{
				pTask->m_bNeedReset = true;
			}
		}
		UINT32 uiOutputCount = GetOutputSize();
		for (UINT32 i = 0; i < uiOutputCount; i++)
		{
			KTPTask* pTask = GetOutputAt(i);
			if (pTask)
			{	
				pTask->m_bNeedReset = true;
			}
		}
	}

	/*void KTPStream::DeleteThis()
	{
		if(NULL != m_pThreadProcessor)
		{
			m_uiDataSize = 0;
			m_pThreadProcessor->GetObjPool().FreeStream(this);
		}
		else
		{
			delete this;
		}
	}*/

	void KTPStream::Prepare()
	{
		m_uiDataSize = m_uiBlockCount*m_uiStride;
	}

	UINT32 KTPStream::GetDataOffsetForBlockIndex(UINT32 uiBlockIndex)
	{
		if (uiBlockIndex >= m_uiBlockCount)
			return 0;

		if (GetData() == NULL)
			return 0;

		return uiBlockIndex * m_uiStride;
	}

} /*Thread*/ } /* System */