#include "KTPStreamPartitioner.h"
#include "../KMacro.h"
#include "KTPTask.h"
#include "KTPStream.h"
namespace System { namespace Thread {

	KTPStreamPartitioner::KTPStreamPartitioner()
		:m_pkTask(NULL)
		,m_uiTotalBlockCount(0)
		,m_uiCurrentBlockIndex(0)
		,m_uiOptimalBlockCount(0)
	{
		
	}

	KTPStreamPartitioner::~KTPStreamPartitioner()
	{
		
	}

	void KTPStreamPartitioner::AssertTask(KTPTask* pkTask)
	{
		UINT32 uiStreamCount = pkTask->GetInputCount();
		for (UINT32 uiIndex = 0; uiIndex < uiStreamCount; ++uiIndex)
		{
			KTPStream* pkStream = pkTask->GetInputAt(uiIndex);
			ASSERT( NULL != pkStream->GetData() && 0 != pkStream->GetBlockCount());
		}

		uiStreamCount = pkTask->GetOutputCount();
		for (UINT32 uiIndex = 0; uiIndex < uiStreamCount; ++uiIndex)
		{
			KTPStream* pkStream = pkTask->GetOutputAt(uiIndex);
			ASSERT( NULL != pkStream->GetData() && 0 != pkStream->GetBlockCount());
			ASSERT( !pkStream->IsFixedInput());
		}

	}

	UINT32 KTPStreamPartitioner::ComputeTotalBlockCount(KTPTask* pkTask)
	{
		ASSERT(NULL != pkTask);
	
		UINT32 uiTotalBlockCount = 0;

		// Iterate through the input streams to determine the total block count.  
		// Also verify that each non-fixed input stream has the same block count. 
		UINT32 uiStreamCount = pkTask->GetInputCount();
		for (UINT32 uiIndex = 0; uiIndex < uiStreamCount; ++uiIndex)
		{
			KTPStream* pkStream = pkTask->GetInputAt(uiIndex);
			if (!pkStream->IsFixedInput())
			{
				if (uiTotalBlockCount == 0)
				{
					uiTotalBlockCount = pkStream->GetBlockCount();
				}
				else if(pkStream->GetBlockCount() != uiTotalBlockCount)
				{
					uiTotalBlockCount = 0;
					break;					
				}
			}
		}

		uiStreamCount = pkTask->GetOutputCount();
		if(0 != uiTotalBlockCount)
		{
			for (UINT32 uiIndex = 0; uiIndex < uiStreamCount; ++uiIndex)
			{
				KTPStream* pkStream = pkTask->GetOutputAt(uiIndex);

				if(pkStream->GetBlockCount() != uiTotalBlockCount)
				{
					uiTotalBlockCount = 0;
					break;					
				}
			}
		}

		return uiTotalBlockCount;
	}

	void KTPStreamPartitioner::Initialize(KTPTask* pkTask)
	{
		ASSERT(NULL != pkTask);
		m_pkTask = pkTask;
		m_uiOptimalBlockCount = 0; 
		m_uiCurrentBlockIndex = 0; 
		m_uiTotalBlockCount = 0;

		AssertTask(pkTask);
		m_uiTotalBlockCount = KTPStreamPartitioner::ComputeTotalBlockCount(pkTask);
		m_uiOptimalBlockCount = pkTask->GetOptimalBlockCount();
	}

	void KTPStreamPartitioner::Partition(KTPWorkload* pWorkload)
	{
		ASSERT(NULL != pWorkload);
		pWorkload->m_uiTotalBlockCount = GetTransferBlockCount();
		
		// Partition input streams
		UINT32 uiInputCount = m_pkTask->GetInputCount();
		for (UINT32 uiIndex = 0; uiIndex < uiInputCount; ++uiIndex)
		{
			KTPStream* pkStream = m_pkTask->GetInputAt(uiIndex);
			ASSERT( NULL != pkStream->GetData() );
			
			PartitionInput(pWorkload, pkStream, pWorkload->m_pkInputsOutputs[uiIndex]);
		}


		UINT32 uiOutputCount = m_pkTask->GetOutputCount();
		for (UINT32 uiIndex = 0; uiIndex < uiOutputCount; ++uiIndex)
		{
			KTPStream* pkStream = m_pkTask->GetOutputAt(uiIndex);
			ASSERT( NULL != pkStream->GetData() );
			PartitionOutput(pWorkload, pkStream,pWorkload->m_pkInputsOutputs[uiIndex + uiInputCount]);
		}
	}

	UINT32 KTPStreamPartitioner::PartitionInput(KTPWorkload* pWorkload,KTPStream* pStream,KTPWorkload::Header& WorkloadHeader)
	{
		UINT32 uiBytes = 0;
		if (pStream->IsFixedInput())
		{
			WorkloadHeader.m_uiBlockCount = pStream->GetBlockCount();
			WorkloadHeader.m_pvBaseAddress = ((BYTE*)pStream->GetData() + pStream->GetElementOffset());
			WorkloadHeader.m_uiDataOffset = 0;
			WorkloadHeader.m_uiStride = pStream->GetStride();
			uiBytes = pStream->GetDataSize();
		}
		else
		{
			WorkloadHeader.m_uiBlockCount = GetTransferBlockCount();
			WorkloadHeader.m_pvBaseAddress = ((BYTE*)pStream->GetData() + pStream->GetElementOffset());
			WorkloadHeader.m_uiDataOffset = pStream->GetDataOffsetForBlockIndex(m_uiCurrentBlockIndex);
			WorkloadHeader.m_uiStride = pStream->GetStride();
			uiBytes = WorkloadHeader.m_uiStride * WorkloadHeader.m_uiBlockCount;
		}
		return uiBytes;
	}

	UINT32 KTPStreamPartitioner::PartitionOutput(KTPWorkload* pWorkload,KTPStream* pStream,KTPWorkload::Header& WorkloadHeader)
	{
		WorkloadHeader.m_pvBaseAddress = ((BYTE*)pStream->GetData() + pStream->GetElementOffset());
		WorkloadHeader.m_uiDataOffset = pStream->GetDataOffsetForBlockIndex(m_uiCurrentBlockIndex);
		WorkloadHeader.m_uiBlockCount = GetTransferBlockCount();
		WorkloadHeader.m_uiStride = pStream->GetStride();
		return WorkloadHeader.m_uiStride * WorkloadHeader.m_uiBlockCount;
	}

} /*Thread*/ } /* System */