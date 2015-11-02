#include "KPerfHitchCounter.h"
KPerfHitchMark::KPerfHitchMark(int id,KPerfHitchCounter* pPerfHitchCounter,const char* pszParam)
:m_Enable(false)
{
	if(pPerfHitchCounter->m_bRec)
	{
		m_pPerfHitchCounter = pPerfHitchCounter;
		m_id = id;
		m_begCycle = rdtsc();
		m_endCycle = m_begCycle; 
		m_Enable = true;
		m_pPerfHitchCounter->m_StackRec.BeginFun(&(m_pPerfHitchCounter->m_recs[id]),pszParam);
	}
}

KPerfHitchMark::~KPerfHitchMark()
{
	if(m_Enable)
	{
		m_endCycle = rdtsc();
		UINT64 Cycles = (m_endCycle > m_begCycle) ? (m_endCycle - m_begCycle) : 0;
		PerfHitchRec* pPerfHitchRec = &(m_pPerfHitchCounter->m_recs[m_id]);

		pPerfHitchRec->lastrunCount += 1;
		pPerfHitchRec->lastCycles += Cycles;
		pPerfHitchRec->used = true;

		m_pPerfHitchCounter->m_StackRec.EndFun(pPerfHitchRec,Cycles);
	}
}

KPerfHitchMarkCheckThread::KPerfHitchMarkCheckThread(int id,KPerfHitchCounter* pPerfHitchCounter,const char* pszParam)
:m_Enable(false)
{
	if(pPerfHitchCounter->m_bRec)
	{
		if(GetCurrentThreadId() ==  pPerfHitchCounter->m_MainThreadId)
		{
			m_pPerfHitchCounter = pPerfHitchCounter;
			m_id = id;
			m_begCycle = rdtsc();
			m_endCycle = m_begCycle; 
			m_Enable = true;
			m_pPerfHitchCounter->m_StackRec.BeginFun(&(m_pPerfHitchCounter->m_recs[id]),pszParam);
		}
	}
}


KPerfHitchMarkCheckThread::~KPerfHitchMarkCheckThread()
{
	if(m_Enable)
	{
		m_endCycle = rdtsc();
		UINT64 Cycles = m_endCycle - m_begCycle;
		PerfHitchRec* pPerfHitchRec = &(m_pPerfHitchCounter->m_recs[m_id]);

		pPerfHitchRec->lastrunCount += 1;
		pPerfHitchRec->lastCycles += Cycles;
		pPerfHitchRec->used = true;

		m_pPerfHitchCounter->m_StackRec.EndFun(pPerfHitchRec,Cycles);
	}
}


BOOL KPerfHitchRecUnitLess::operator () (PerfHitchRecUnit* const &a, PerfHitchRecUnit* const &b) const
{
	return (a->pPerfHitchRec->id) < (b->pPerfHitchRec->id);
}

void PerfHitchRecUnit::WriteInfor(AsyncFileLogger& logger,unsigned short iFormatLevel,KPerfHitchStackRec* pStack)
{
	static DWORD cpuSecCycles = GetCpuCyclesPerSecond();
	/*double fAvg = 0.0f;
	if(SumCount>0)
	{
		fAvg = (double)(SumCycles/SumCount);
	}*/


	UINT64 sumCycles = (UINT64) (((double)SumCycles/cpuSecCycles)*1000000);
	if(sumCycles == 0)
		return ;

	UINT64 maxCycles = (UINT64) (((double)MaxCycles/cpuSecCycles)*1000000);
	if(NULL != pPerfHitchRec)
	{
		::System::Collections::KString<64>FormatBuf;
		for(int i = 0;i<iFormatLevel;++i)
		{
			FormatBuf.append(" ");
		}

		char buf[1024];
		size_t n = sprintf_k(buf, sizeof(buf), "%s    %llu    %llu    %llu",
		pPerfHitchRec->name.c_str(),
		SumCount,
		sumCycles,
		maxCycles);
		logger.WriteLog("    %s%s", FormatBuf.c_str(),buf);
		
		if(-1 != iParamIndex)
		{
			logger.WriteLog("--Param: %s", pStack->m_ParamPool[iParamIndex]->c_str());
		}
	}
	for(int i = 0;i<ChildArray.size();++i)
	{
		ChildArray[i]->WriteInfor(logger,iFormatLevel+1,pStack);
	}
}

KPerfHitchStackRec::KPerfHitchStackRec()
{
	m_pRoot = new PerfHitchRecUnit;
	m_pCurIter = m_pRoot;
	m_PoolFreeHead = 0;
	m_ParamFreeHead = 0;
}

KPerfHitchStackRec::~KPerfHitchStackRec()
{
	delete m_pRoot;
	m_pRoot = NULL;
	ClearParamPool();
}

void KPerfHitchStackRec::BeginFun(PerfHitchRec* pPerfHitchRec,const char* pszParam)
{
	PerfHitchRecUnit FindRecUnit;
	FindRecUnit.pPerfHitchRec = pPerfHitchRec;
	int iPos = m_pCurIter->ChildArray.find(&FindRecUnit);
	if(-1 == iPos)
	{
		PerfHitchRecUnit* pPerfHitchRecUnit = AllocNode();
		pPerfHitchRecUnit->Reset();
		pPerfHitchRecUnit->pParent = m_pCurIter;
		pPerfHitchRecUnit->pPerfHitchRec = pPerfHitchRec;
		m_pCurIter->ChildArray.insert(pPerfHitchRecUnit);
		m_pCurIter = pPerfHitchRecUnit;
	}
	else
	{
		m_pCurIter = m_pCurIter->ChildArray[iPos];
	}
	if(NULL != pszParam)
	{
		::System::Collections::KDString<PARAM_INIT_CAPACITY>* pParam = NULL;
		if(-1 == m_pCurIter->iParamIndex)
		{
			m_pCurIter->iParamIndex = m_ParamFreeHead;
			pParam = AllocParam();
			
		}
		else
		{
			pParam = m_ParamPool.at(m_pCurIter->iParamIndex);
		}
		pParam->append("||");
		pParam->append(pszParam);
		
	}
}

void KPerfHitchStackRec::EndFun(PerfHitchRec* pPerfHitchRec,UINT64 Cycles)
{
	ASSERT(m_pCurIter->pPerfHitchRec == pPerfHitchRec);
	m_pCurIter->SumCycles += Cycles;
	m_pCurIter->SumCount ++;
	if(m_pCurIter->MaxCycles<Cycles)
	{
		m_pCurIter->MaxCycles = Cycles;
	}
	m_pCurIter = m_pCurIter->pParent;
}

void KPerfHitchStackRec::Reset()
{
	m_pRoot->Reset();
	m_pCurIter = m_pRoot;

	m_PoolFreeHead = 0;
	m_ParamFreeHead = 0;
}

void KPerfHitchStackRec::WriteInfor(AsyncFileLogger& logger)
{
	for(int i = 0;i<m_pRoot->ChildArray.size();++i)
	{
		m_pRoot->ChildArray[i]->WriteInfor(logger,0,this);
	}
}

PerfHitchRecUnit* KPerfHitchStackRec::AllocNode()
{
	PerfHitchRecUnit* pRetPerfHitchRecUnit = NULL; 
	if(m_PoolFreeHead>=m_Pool.size())
	{
		PerfHitchRecUnit RecUnit;
		m_Pool.push_back(RecUnit);
	}
	
	pRetPerfHitchRecUnit = &(m_Pool.at(m_PoolFreeHead));
	++m_PoolFreeHead;
	return pRetPerfHitchRecUnit;
}

void KPerfHitchStackRec::ClearPool()
{
	m_Pool.clear();
	m_PoolFreeHead = 0;
}

::System::Collections::KDString<PARAM_INIT_CAPACITY>* KPerfHitchStackRec::AllocParam()
{
	::System::Collections::KDString<PARAM_INIT_CAPACITY>* pParam = NULL;
	if(m_ParamFreeHead>=m_ParamPool.size())
	{
		pParam = new ::System::Collections::KDString<PARAM_INIT_CAPACITY>;
		m_ParamPool.insert(m_ParamFreeHead,pParam);
	}
	else
	{
		pParam = m_ParamPool.at(m_ParamFreeHead);
		pParam->clear();
	}
	++m_ParamFreeHead;
	return pParam;
}

void KPerfHitchStackRec::ClearParamPool()
{
	int iSize = m_ParamPool.size();
	for(int i = 0;i<iSize;++i)
	{
		delete m_ParamPool[i];
	}
	m_ParamPool.clear();
	m_ParamFreeHead = 0;
}

KPerfHitchCounter::KPerfHitchCounter()
:m_bOpen(false)
,m_bRec(m_bOpen)
,m_bNeedWrite(false)
,m_bNeedRefreshSum(false)
,m_bWriteSumRec(true)
,m_bWriteStackRec(true)
,m_MainThreadId(0)
,m_recs(NULL)
,m_iMaxCount(0)
{
	
}

KPerfHitchCounter::~KPerfHitchCounter()
{
	delete[] m_recs;
}

void KPerfHitchCounter::Init(const char* pLogFileName,size_t iMaxCount)
{
	m_MainThreadId = GetCurrentThreadId();
	m_logger.Open(pLogFileName);
	m_iMaxCount = iMaxCount;
	m_recs = new PerfHitchRec[m_iMaxCount];
	char szName[128];

	for(size_t i = 0;i<m_iMaxCount;++i)
	{
		m_recs[i].id = (int)i;
		char szName[128];
		sprintf_k(szName,sizeof(szName),"UnNamed_FuncID_%d",m_recs[i].id);
		m_recs[i].name = szName;
	}
}

bool KPerfHitchCounter::Register(int id, const char* pName)
{
	if(id < 0 || id >= (int)m_iMaxCount) 
		return false;
	
	PerfHitchRec& rec = m_recs[id];
	rec.id = id;
	rec.name = pName;

	return true;	
}

void KPerfHitchCounter::WriteInfor()
{
	if(m_bRec)
	{
		m_bNeedWrite = true;
		//WriteInforImp();
	}
}

void KPerfHitchCounter::WriteInforImp()
{
	if(m_bWriteSumRec)
	{
		m_logger.WriteLog(">>>>>>>>>>>>>>> perf Hitch sum Infor >>>>>>>>>>>>>>>>>>>>>>>>>");
		m_logger.WriteLog("    Name    LastCount    avg    Last");
		char buf[512];
		for(size_t i=0; i<m_iMaxCount; i++)
		{
			if(m_recs[i].used)
			{
				static DWORD cpuSecCycles = GetCpuCyclesPerSecond();
				double fAvg = 0.0f;
				if(m_recs[i].sumCount>0)
				{
					fAvg = (double)(m_recs[i].sumCycles/m_recs[i].sumCount);
				}
				size_t n = sprintf_k(buf, sizeof(buf), "%s    %llu    %llu    %llu",
					m_recs[i].name.c_str(),
					m_recs[i].lastrunCount,
					(UINT64) (((double)fAvg/cpuSecCycles)*1000000),
					(UINT64) (((double)m_recs[i].lastCycles/cpuSecCycles)*1000000));
				m_logger.WriteLog("    %s", buf);
			}
			
		}
		m_logger.WriteLog(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
	}
	if(m_bWriteStackRec)
	{
		m_logger.WriteLog(">>>>>>>>>>>>>>> perf Hitch stack Infor >>>>>>>>>>>>>>>>>>>>>>>>>");
		m_logger.WriteLog("    Name    Count    Sum    Max");
		m_StackRec.WriteInfor(m_logger);
		m_logger.WriteLog(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
	}
}

void KPerfHitchCounter::RefreshSum()
{
	m_bNeedRefreshSum = true;
}

void KPerfHitchCounter::RefreshSumImp()
{
	for(size_t i = 0;i<m_iMaxCount;++i)
	{
		m_recs[i].RefreshSum();
	}
}

void KPerfHitchCounter::BeginFrame()
{
	for(size_t i = 0;i<m_iMaxCount;++i)
	{
		m_recs[i].Update();
	}
	m_StackRec.Reset();
	m_bRec = m_bOpen;
}

void KPerfHitchCounter::EndFrame()
{
	if(m_bNeedWrite)
	{
		WriteInforImp();
	}

	if(m_bNeedRefreshSum)
	{
		RefreshSumImp();
	}
	m_bNeedWrite = false;
	m_bNeedRefreshSum = false;
}




KPerfHitchIns& KPerfHitchIns::GetInstance()
{
	static KPerfHitchIns* p = new KPerfHitchIns();
	return *p;
}
