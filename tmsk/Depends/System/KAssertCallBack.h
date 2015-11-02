#pragma once
#include "KType.h"
struct KAssertError
{
	const char* pInfor;	// 给临时变量就行，底层不管删除
	UINT64 WParam;
	UINT64 LParam;

	KAssertError()
	{
		memset( this, 0, sizeof(KAssertError) );
	}
};

class IAssertCallBackUnit
{
public:
	IAssertCallBackUnit();
	virtual ~IAssertCallBackUnit();
	virtual void AssertInfor(const char* pAssertLine,KAssertError* pError) = 0;
};

class KAssertCallBack
{
public:
	KAssertCallBack();
	virtual ~KAssertCallBack();
	virtual bool InitUnitMaxCount(size_t iUnitMaxCount);
	bool	RegistratCallBackUnit(size_t iIndex, IAssertCallBackUnit* pAssertCallBackUnit);
	void	AssertInfor(const char* pAssertLine,size_t iIndex = 0xffffffff,KAssertError* pError = NULL);
	void	SetDefaultIndex(size_t	iDefaultIndex);
protected:
	IAssertCallBackUnit**   m_pCallBackUnitArray;
	size_t				    m_iUnitMaxCount;
	size_t				    m_iDefaultIndex;
};