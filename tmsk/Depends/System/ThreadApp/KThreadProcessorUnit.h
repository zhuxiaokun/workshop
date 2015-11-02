#ifndef __KTHREADPROCESSORUNIT_H__
#define __KTHREADPROCESSORUNIT_H__
#include "KThreadUnit.h"
namespace System { namespace Thread {

	class KIThreadProcessorUnit : public KIThreadUnit
	{
	public:
		static const UINT32 NO_YIELD_ID = 0xFFFFFF;

		KIThreadProcessorUnit();
		virtual ~KIThreadProcessorUnit();
	
		void SetWorkflowID(UINT32 uiID){m_uiID = uiID;}
		UINT32		 GetWorkflowID()	  {return m_uiID;}

		void SetStage(unsigned short usStage){m_usStage = usStage;}
		unsigned short GetStage() const{return m_usStage;}
		
		bool IsNeedYield(){return m_uiID != NO_YIELD_ID;}

	protected:
		UINT32			m_uiID;
		unsigned short  m_usStage;
	};
	
} /*Thread*/ } /* System */


#endif