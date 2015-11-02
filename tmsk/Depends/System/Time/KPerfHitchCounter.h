#ifndef _K_PERFHITCH_COUNTER_H_
#define _K_PERFHITCH_COUNTER_H_
#include "../KType.h"
#include "../KMacro.h"
#include "../Thread/KThread.h"
#include "../Log/log.h"
#include "../Log/AsyncLogger.h"
#include "../Collections/DynArray.h"
#include "../Collections/KVector.h"
#include "../Collections/KString.h"
#include "../Memory/KAlloc.h"
#include "KPerfCounter.h"

#define PARAM_INIT_CAPACITY		128

class KPerfHitchCounter;
class KPerfHitchMark
{
public:
	KPerfHitchMark(int id,KPerfHitchCounter* pPerfHitchCounter,const char* pszParam = NULL);
	~KPerfHitchMark();

protected:
	int m_id;
	KPerfHitchCounter* m_pPerfHitchCounter;

	UINT64 m_begCycle;
	UINT64 m_endCycle;
	bool   m_Enable;
};
// 不从KPerfHitchMark继承的目的是不调基类的析构,少一次函数调用
class KPerfHitchMarkCheckThread
{
public:
	KPerfHitchMarkCheckThread(int id,KPerfHitchCounter* pPerfHitchCounter,const char* pszParam = NULL);
	~KPerfHitchMarkCheckThread();

protected:
	int m_id;
	KPerfHitchCounter* m_pPerfHitchCounter;

	UINT64 m_begCycle;
	UINT64 m_endCycle;

	bool   m_Enable;
};


struct PerfHitchRec
{
	PerfHitchRec()
		:id(-1)
		,lastrunCount(0)
		,lastCycles(0)
		,sumCycles(0)
		,sumCount(0)
		,used(false)
	{
		name[0] = 0;
	}
	
	void Update()
	{
		sumCycles += lastCycles;
		sumCount += lastrunCount;

		lastCycles = 0;	
		lastrunCount = 0;
	}

	void RefreshSum()
	{
		sumCycles = 0;
		sumCount = 0;
	}
	int        id;
	::System::Collections::KString<256>  name;
	UINT64  lastrunCount;
	UINT64  lastCycles;	
	
	UINT64  sumCount;
	UINT64  sumCycles;

	bool    used;
};

struct PerfHitchRecUnit;
class KPerfHitchRecUnitLess
{
public:
	BOOL operator () (PerfHitchRecUnit* const &a, PerfHitchRecUnit* const &b) const;
};
class KPerfHitchStackRec;
struct PerfHitchRecUnit
{
	PerfHitchRecUnit()
		:pPerfHitchRec(NULL)
		,pParent(NULL)
		,SumCount(0)
		,SumCycles(0)
		,MaxCycles(0)
		,iParamIndex(0)
	{
		ChildArray.clear();
	}
	void Reset()
	{
		pPerfHitchRec = NULL;
		pParent = NULL;
		SumCount = 0;
		SumCycles = 0;
		MaxCycles = 0;
		iParamIndex = -1;
		ChildArray.clear();
	}
	
	void WriteInfor(AsyncFileLogger& logger,unsigned short level,KPerfHitchStackRec* pStack);

	PerfHitchRec* pPerfHitchRec;
	UINT64  SumCount;
	UINT64  SumCycles;
	UINT64  MaxCycles;
	int     iParamIndex;
	PerfHitchRecUnit*                  pParent;
	::System::Collections::DynSortedArray<PerfHitchRecUnit*,KPerfHitchRecUnitLess> ChildArray;
};

class KPerfHitchStackRec
{
public:
	friend struct PerfHitchRecUnit;
	typedef ::System::Memory::KStepBlockAlloc<PerfHitchRecUnit,64> alloc_type;
	KPerfHitchStackRec();
	virtual ~KPerfHitchStackRec();
	void BeginFun(PerfHitchRec* pPerfHitchRec,const char* pszParam = NULL);
	void EndFun(PerfHitchRec* pPerfHitchRec,UINT64 Cycles);
	void Reset();
	void WriteInfor(AsyncFileLogger& logger);
protected:
	PerfHitchRecUnit* AllocNode();
	void              ClearPool();

	::System::Collections::KDString<PARAM_INIT_CAPACITY>* AllocParam();
	void						   ClearParamPool();

protected:
	alloc_type		  m_alloc;
	PerfHitchRecUnit* m_pRoot;
	PerfHitchRecUnit* m_pCurIter;
	
	::System::Collections::KVector<PerfHitchRecUnit> m_Pool;
	int                                            m_PoolFreeHead;

	::System::Collections::DynArray< ::System::Collections::KDString<PARAM_INIT_CAPACITY>* > m_ParamPool;
	int                                           m_ParamFreeHead;
};

class KPerfHitchCounter 
{
public:
	friend class KPerfHitchMark;
	friend class KPerfHitchMarkCheckThread;
	KPerfHitchCounter();
	virtual ~KPerfHitchCounter();
	void Init(const char* pLogFileName,size_t iMaxCount = 2048);
	bool Register(int id, const char* pName);
	void BeginFrame();
	void EndFrame();

	void RefreshSum();
	void WriteInfor();
	void Open(){m_bOpen = true;}
	void Close(){m_bOpen = false;}
	bool IsOpen(){return m_bOpen;}
	
	void EnableWriteSumRec(bool bWriteSumRec){m_bWriteSumRec = bWriteSumRec;}
	void EnableWriteStackRec(bool bWriteStackRec){m_bWriteStackRec = bWriteStackRec;}
	bool IsWriteSumRec(){return m_bWriteSumRec;}
	bool IsWriteStackRec(){return m_bWriteStackRec;}
protected:
	void WriteInforImp();
	void RefreshSumImp();
protected:
	PerfHitchRec* m_recs;
	size_t		  m_iMaxCount;
	KPerfHitchStackRec m_StackRec;
	bool m_bOpen;
	// 数据统计开关每一祯要么全开，要么全不开
	bool m_bRec;
	bool m_bNeedWrite;
	bool m_bNeedRefreshSum;
	//
	bool m_bWriteSumRec;
	bool m_bWriteStackRec;
	// 主线程id
	DWORD m_MainThreadId;
	AsyncFileLogger m_logger;
};

class KPerfHitchIns : public KPerfHitchCounter 
{
public:
	KPerfHitchIns() {}
	virtual ~KPerfHitchIns() {}

public:
	static KPerfHitchIns& GetInstance();
};

#endif