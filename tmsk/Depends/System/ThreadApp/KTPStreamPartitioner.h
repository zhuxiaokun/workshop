#ifndef __KTPSTREAMPARTITIONER_H__
#define __KTPSTREAMPARTITIONER_H__
#include "../KType.h"
#include "KTPWorkload.h"
namespace System { namespace Thread {

	class KTPTask;
	class KTPStream;
	class KTPStreamPartitioner
	{
	public:
		KTPStreamPartitioner();
		virtual ~KTPStreamPartitioner();
	
		static void   AssertTask(KTPTask* pkTask);

		static UINT32 ComputeTotalBlockCount(KTPTask* pkTask);
		
		virtual void Initialize(KTPTask* pkTask);
	
		virtual void Partition(KTPWorkload* pWorkload);

		/// Gets the optimal number of blocks to send at one time.
		UINT32 GetOptimalBlockCount();

		/// Indicates if the set has more chunks to transfer.
		bool HasMoreBlocks();

		/// Resets the set back to the start of its streams.
		void Reset();

		/// Gets the current chunk index.
		inline UINT32 GetBlockIndex();

		/// Gets the number of blocks to transfer.
		inline  UINT32 GetTransferBlockCount();

		/// Increases the chunk index by transfer chunk count.
		inline UINT32 AdvanceBlockIndex();

		inline UINT32 GetTotalBlockCount();
		
		inline UINT32 GetTotalJobCount();

	protected:
		virtual UINT32 PartitionInput(KTPWorkload* pWorkload,KTPStream* pStream,KTPWorkload::Header& WorkloadHeader);
		virtual UINT32 PartitionOutput(KTPWorkload* pWorkload,KTPStream* pStream,KTPWorkload::Header& WorkloadHeader);
	protected:
		KTPTask*  m_pkTask;
		UINT32	  m_uiTotalBlockCount;
		UINT32	  m_uiCurrentBlockIndex;
		UINT32	  m_uiOptimalBlockCount;
	};

	#include "KTPStreamPartitioner.inl"
} /*Thread*/ } /* System */

#endif