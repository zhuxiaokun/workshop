#ifndef __KTPWORKFLOW_H__
#define __KTPWORKFLOW_H__
#include "KThreadUnit.h"
#include "../Collections/DynComplexArray.h"
#include "KTPTask.h"
#include "KTPWorkflowSerializer.h"
namespace System { namespace Thread {

	class KTPWorkflow : public KIThreadUnit
	{
	public:
		friend class KThreadProcessor;
		friend class KTPWorkflowSerializer;
		KTPWorkflow();
		virtual ~KTPWorkflow();
		void Prepare();
		void AddTask(KTPTask* pTask);
		KTPTask* CreateNewTask();
		KTPTask* FindTaskByIdx(int iTaskIdx);
		int		 FindTaskIdx(KTPTask* pTask);
		void	 RemoveTask(int iTaskIdx);
		void	 RemoveAllTask();
		int		 GetTaskSize(){return m_TaskList.size();}

		virtual void ThreadExecute();
		virtual void CompleteInThread();
		virtual void ExecuteSerial();

		virtual void DealComplete();
		void NotifyTaskComplete();

		void   Reset();

		inline void SetThreadProcessor(KThreadProcessor* pThreadProcessor){m_pThreadProcessor = pThreadProcessor;}
		inline KThreadProcessor* GetThreadProcessor(){ return m_pThreadProcessor;}
		unsigned int GetPriority(){return m_iPriority;}
		UINT32		 GetID()	  {return m_uiID;}
	protected:	 
		void SetPriority(unsigned int iPriority){ m_iPriority = iPriority;}
		void SortTask();
		void SetSerial(bool	bSerialExecute){m_bSerialExecute = bSerialExecute;}
	protected:
		Collections::DynComplexArray<KTPTaskPtr,4,4> m_TaskList;
		
		KTPWorkflowSerializer						m_Serializer;

		unsigned int					  m_iPriority;
		KThreadProcessor*				  m_pThreadProcessor;
		UINT32							  m_uiID;
		Sync::KRecursiveMutex			  m_CompleteMutex;
		bool							  m_bComplete;
		bool							  m_bSerialExecute;
	};
} /*Thread*/ } /* System */

#endif