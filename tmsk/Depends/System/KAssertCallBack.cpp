#include "KAssertCallBack.h"
#include "Memory/KMemory.h"

IAssertCallBackUnit::IAssertCallBackUnit()
{
	
}

IAssertCallBackUnit::~IAssertCallBackUnit()
{

}


KAssertCallBack::KAssertCallBack()
:m_pCallBackUnitArray(0)
,m_iUnitMaxCount(0)
,m_iDefaultIndex(0)
{
	
}

KAssertCallBack::~KAssertCallBack()
{
	if(0 != m_pCallBackUnitArray)
	{
		free_k(m_pCallBackUnitArray);
		m_pCallBackUnitArray = 0;
	}

	m_iUnitMaxCount = 0;
	
}

bool KAssertCallBack::InitUnitMaxCount(size_t iUnitMaxCount)
{
	if(0 != m_pCallBackUnitArray)
	{
		return false;
	}
	m_pCallBackUnitArray = (IAssertCallBackUnit**)malloc_k(iUnitMaxCount*sizeof(IAssertCallBackUnit*));
	for(int i = 0;i<iUnitMaxCount;++i)
	{
		m_pCallBackUnitArray[i] = 0;
	}

	m_iUnitMaxCount = iUnitMaxCount;
	return true;
}

bool KAssertCallBack::RegistratCallBackUnit(size_t iIndex, IAssertCallBackUnit* pAssertCallBackUnit)
{
	if(iIndex>=m_iUnitMaxCount)
	{
		return false;
	}
	m_pCallBackUnitArray[iIndex] = pAssertCallBackUnit;
	return true;
}

void KAssertCallBack::SetDefaultIndex(size_t iDefaultIndex)
{
	if(iDefaultIndex<m_iUnitMaxCount)
	{
		m_iDefaultIndex = iDefaultIndex;
	}
}

void KAssertCallBack::AssertInfor(const char* pAssertLine,size_t iIndex,KAssertError* pError)
{
	if ( iIndex == 0xffffffff )
	{
		iIndex = m_iDefaultIndex;
	}
	if( iIndex >=0 
		&& iIndex < m_iUnitMaxCount 
		&& 0 != m_pCallBackUnitArray[iIndex]  )
	{
		m_pCallBackUnitArray[iIndex]->AssertInfor(pAssertLine,pError);
	}
}