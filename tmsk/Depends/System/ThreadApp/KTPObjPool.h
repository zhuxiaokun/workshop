#ifndef __KTPOBJPOOL_H__
#define __KTPOBJPOOL_H__
#include "KThreadPool.h"
#include "KTPWorkflow.h"
#include "KTPTask.h"
#include "KTPJobList.h"
#include "KTPStream.h"

namespace System { namespace Thread {
	
	class KThreadProcessor;
	typedef  Memory::KStepObjectPool<KTPWorkflow,64,Sync::KThreadMutex> KTPWorkflowPool;
	typedef  Memory::KStepObjectPool<KTPTask,64,Sync::KThreadMutex> KTPTaskPool;
	typedef  Memory::KStepObjectPool<KTPJobList,64,Sync::KThreadMutex> KTPJobListPool;
	typedef  Memory::KStepObjectPool<KTPStream,64,Sync::KThreadMutex> KTPStreamPool;

	class KTPObjPool
	{
	public:
		KTPObjPool(KThreadProcessor* pThreadProcessor);
		virtual ~KTPObjPool();
		virtual void Initialize(KThreadUnitNodePool* pThreadUnitNodePool = NULL,KTPWorkflowPool* pWorkflowPool = NULL,
			KTPTaskPool* pTaskPool = NULL,KTPStreamPool* pStreamPool = NULL, KTPJobListPool* pJobListPool= NULL);
	
		virtual void Clear();

		inline KThreadUnitNode* AllocUnitNode();
		inline void             FreeUnitNode(KThreadUnitNode* pNode);

		inline KTPWorkflow* AllocWorkflow();
		inline void         FreeWorkflow(KTPWorkflow* pWorkflow);
		
		inline KTPTask*		AllocTask();
		inline void         FreeTask(KTPTask* pTask);

		inline KTPJobList*	AllocJobList();
		inline void         FreeJobList(KTPJobList* pJobList);

		inline KTPStream*	AllocStream();
		inline void			FreeStream(KTPStream* pStream);

		inline KThreadUnitNodePool* GetUnitNodePool();
		inline KTPWorkflowPool*		GetWorkflowPool();
		inline KTPTaskPool*			GetTaskPool();
		inline KTPStreamPool*		GetStreamPool();
		inline KTPJobListPool*		GetJobListPool();
		inline bool					IsInitialize();
		

	protected:
		void SetBit(bool bVal, UINT8 uMask) 
		{ 
			if (bVal)
			{ 
				m_uFlags |= uMask; 
			} 
			else 
			{ 
				m_uFlags &= ~uMask; 
			} 
		}
		bool GetBit(UINT8 uMask) const
		{ 
			return (m_uFlags & uMask) != 0;
		}

	protected:
		enum Flags 
		{
			ATTACH_UNITNODEPOOL = 0x01,
			ATTACH_WORKFLOWPOOL = 0x02,
			ATTACH_TASKPOOL		= 0x04,
			ATTACH_JOBLISTPOOL	= 0x08,
			ATTACH_STREAMPOOL	= 0x10,
		};
		KThreadUnitNodePool*	 m_pThreadUnitNodePool;
		KTPWorkflowPool*		 m_pWorkflowPool;
		KTPTaskPool*			 m_pTaskPool;
		KTPJobListPool*			 m_pJobListPool;
		KTPStreamPool*			 m_pStreamPool;
		KThreadProcessor*		 m_pThreadProcessor;
		UINT8					 m_uFlags;
	};

	#include "KTPObjPool.inl"
} /*Thread*/ } /* System */


#endif