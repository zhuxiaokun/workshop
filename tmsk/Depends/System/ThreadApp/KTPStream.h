#ifndef __KTPSTREAM_H__
#define __KTPSTREAM_H__
#include "../KType.h"

#include "../Collections/DynArray.h"
//#include "../Collections/DynComplexArray.h"

#include "../Misc/KSmartPointer.h"
#include "../Misc/KRefObject.h"
namespace System { namespace Thread {

	class KTPTask;
	class KThreadProcessor;
	//KSmartPointer(KTPStream);
	class KTPStream //: public KRefObject
	{
	public:
		KTPStream(UINT16 uiStride = 0,bool bIsFixedInput = false);
		virtual ~KTPStream();
		
		//void Initialize();

		void Prepare();

		void* GetData();
		void SetData(void* pvData);
		
		UINT32 GetElementOffset() const;
		void SetElementOffset(UINT32 uiElementOffset);
		
		UINT16 GetStride();
		void SetStride(UINT16 uiStride);

		UINT32 GetBlockCount();
		void SetBlockCount(UINT32 uiBlockCount);

		UINT32 GetDataSize();
		UINT32 GetDataOffsetForBlockIndex(UINT32 uiBlockIndex);

		bool IsFixedInput();
	
		//---------------------------------------------------------------------------
		inline void AddInput(KTPTask* pkTask);
		//---------------------------------------------------------------------------
		inline void RemoveInput(KTPTask* pkTask);
		//---------------------------------------------------------------------------
		inline KTPTask* GetInputAt(UINT32 uiIndex){return m_Inputs.at(uiIndex);}
		//---------------------------------------------------------------------------
		inline UINT32 GetInputSize() const{return m_Inputs.size();}

		//---------------------------------------------------------------------------
		inline void AddOutput(KTPTask* pkTask);
		//---------------------------------------------------------------------------
		inline void RemoveOutput(KTPTask* pkTask);
		//---------------------------------------------------------------------------
		inline KTPTask* GetOutputAt(UINT32 uiIndex){return m_Outputs.at(uiIndex);}
		//---------------------------------------------------------------------------
		inline UINT32 GetOutputSize() const{return m_Outputs.size();}

		inline void  ClearTaskArrays();
	
		inline void SetThreadProcessor(KThreadProcessor* pThreadProcessor);

	protected:
		void RemoveFromDependentTasks();
		void NotifyDependentTasks();
		//void DeleteThis();
	protected:
		
		typedef Collections::DynSortedArray<KTPTask*,KLess<KTPTask*>,8,4> KTPTaskArray;
		/// Pointer to data buffer.
		void*		m_pvData;     
		UINT32		m_uiBlockCount;
		UINT32		m_uiElementOffset;
		UINT16		m_uiStride;
		UINT32		m_uiDataSize;

		KThreadProcessor* m_pThreadProcessor;

		KTPTaskArray m_Inputs; 
		KTPTaskArray m_Outputs;

		bool		m_bIsFixedInput;


	};
	#include "KTPStream.inl"

	template <class T>
	class KTTPStream : public KTPStream
	{
	public:
		/// Constructor: sets the internal data to NULL.
		KTTPStream() : KTPStream(sizeof(T), false){}

		/// Constructor: takes a pointer to the data and the number of elements.
		KTTPStream(T* pData, UINT32 uiBlockCount) : 
		KTPStream(pData, sizeof(T), uiBlockCount){}

		/// Virtual destructor.
		virtual ~KTTPStream(){}
	};

	///    Helper class for constructing fixed streams of a known data types.
	template <class T>
	class KTTPFixedInputStream: public KTPStream
	{
	public:
		/// Constructor: sets the internal data to NULL.
		KTTPFixedInputStream() : KTPStream( sizeof(T), true){}

		/// Constructor: takes a pointer to the data and the number of elements.
		KTTPFixedInputStream(T* pData, UINT32 uiBlockCount) : 
		KTPStream(pData, sizeof(T), uiBlockCount, true){}

		/// Virtual destructor.
		virtual ~KTTPFixedInputStream(){}
	};

} /*Thread*/ } /* System */
#endif