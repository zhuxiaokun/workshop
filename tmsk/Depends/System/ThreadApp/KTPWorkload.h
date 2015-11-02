#ifndef __KTPWORKLOAD_H__
#define __KTPWORKLOAD_H__

namespace System { namespace Thread {
	class KITPKernel;
	class KTPWorkload
	{
	public:
		friend class KTPStreamPartitioner;
		struct Header                       
		{
			void* m_pvBaseAddress;   
			UINT32 m_uiDataOffset;  
			UINT32 m_uiBlockCount;
			UINT32 m_uiStride;
		};    

		KTPWorkload();
		~KTPWorkload();
		inline void Reset(UINT32 uiInputCount, UINT32 uiOutputCount);
		inline void Clear();

		
		template <class T> T* GetInput(UINT16 uiInputIndex);
		template <class T> T* GetInputBase(UINT16 uiInputIndex);
		inline UINT32 GetInputStride(UINT16 uiInputIndex);
		inline UINT32 GetInputBlockCount(UINT16 uiInputIndex);

		template <class T> inline T* GetOutput(UINT16 uiOutputIndex);
		template <class T> inline T* GetOutputBase(UINT16 uiOutputIndex);
		inline UINT32 GetOutputStride(UINT16 uiOutputIndex);
		inline UINT32 GetOutputBlockCount(UINT16 uiInputIndex);
		
		inline UINT32 GetInputCount();
		inline UINT32 GetOutputCount();
		inline UINT32 GetTotalBlockCount();
		inline KITPKernel* GetKernel();
		
		//---------------------------------------------------------------------------
		inline void SetKernel(KITPKernel* pkKernel);

	protected:
		KITPKernel*	   m_pKernel; 
		
		Header* m_pkInputsOutputs;

		/// The number of available inputs and outputs.
		UINT32 m_uiInOutCount;

		/// The number of input streams.
		UINT16 m_uiInputCount;

		/// The number of output streams.
		UINT16 m_uiOutputCount;

		/// The number of blocks of data.
		UINT32 m_uiTotalBlockCount;
	};

	#include "KTPWorkload.inl"

} /*Thread*/ } /* System */
#endif