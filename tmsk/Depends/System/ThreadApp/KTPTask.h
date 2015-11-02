#ifndef __KTPTASK_H__
#define __KTPTASK_H__
#include "../Collections/DynArray.h"
#include "../Collections/DynComplexArray.h"
#include "KTPStreamPartitioner.h"
#include "KTPStream.h"
#include "../Misc/KSmartPointer.h"
#include "../Misc/KRefObject.h"
namespace System { namespace Thread {

	class KTPWorkflow;
	class KTPJobList;
	class KITPKernel;
	class KTPStream;
	class KThreadProcessor;
	KSmartPointer(KTPTask);

	class KTPTask : public KRefObject
	{
	public:
		friend class KTPStream;
		typedef Collections::DynArray<KTPJobList*,8,4> KTPJobListSet;
		typedef Collections::/*DynComplexArray*/DynArray<KTPStream*,8,4>  KTPStreamSet;

		KTPTask();
		virtual ~KTPTask();
		void Prepare();
		void Clear();

		void SetWorkflow(KTPWorkflow* pWorkflow){ m_pWorkflow = pWorkflow; }
		KTPWorkflow* GetWorkflow() const{ return m_pWorkflow; }
		KTPJobListSet& GetJobListSet(){return m_JobListSet;}
		
		void SetStage(unsigned short usStage){m_usStage = usStage;}
		unsigned short GetStage() const{return m_usStage;}

		void NotifyJobListComplete();
		bool IsComplete(){return m_bComplete;}
		
		void SetThreadProcessor(KThreadProcessor* pThreadProcessor){ m_pThreadProcessor = pThreadProcessor;}

		void Reset();

		//---------------------------------------------------------------------------
		inline void SetKernel(KITPKernel* pkKernel);
		inline KITPKernel* GetKernel() const;

		inline void SetOptimalBlockCount(UINT32 uiBlockCount);
		inline UINT32 GetOptimalBlockCount();

		inline UINT32	  GetInputCount() const;
		inline UINT32     GetOutputCount() const;
		inline KTPStream* GetInputAt(UINT32 uiIndex) const;
		inline KTPStream* GetOutputAt(UINT32 uiIndex) const;

		/// Adds an input stream to the task.
		inline void AddInput(KTPStream* pInputStream);

		/// Adds an output stream to the task.
		inline void AddOutput(KTPStream* pOutputStream);

		/// Removes an input stream from the task.
		inline void RemoveInput(KTPStream* pInputStream);

		/// Removes an output stream from the task.
		inline void RemoveOutput(KTPStream* pOutputStream);
	
		inline void RemoveAllInput();

		inline void RemoveAllOutput();
	protected:
		void AllocJobList(UINT32 iCount);
		void DeleteThis();

		inline bool RemoveInputImp(KTPStream* pInputStream);
		inline bool RemoveOutputImp(KTPStream* pOutputStream);
		
	public:
		 static const UINT32 AUTO_BLOCK_COUNT;
	protected:
		KTPWorkflow*   m_pWorkflow;
		KTPStreamSet   m_InputStreams;
		KTPStreamSet   m_OutputStreams;
		unsigned short m_usStage;
		KITPKernel*	   m_pKernel; 
		UINT32		   m_uiOptimalBlockCount;

		KThreadProcessor* m_pThreadProcessor;

		// 内部的
		KTPJobListSet  m_JobListSet;	
		KTPStreamPartitioner	m_StreamPartitioner;
		
		bool           m_bComplete;
		bool           m_bNeedReset;
	};

	#include "KTPTask.inl"
} /*Thread*/ } /* System */
#endif