#pragma once
#include "../KType.h"
#include "../KMacro.h"
#include "../Thread/KThread.h"
#include "../Log/log.h"
#include "../Collections/KString.h"
#include "../Collections/DynArray.h"
#include "../Collections/KList.h"
#include "../Memory/KStepObjectPool.h"
#include "../Memory/KThreadLocalStorage.h"

#if defined(_MONIT_PERFORMANCE_)

class KPerfCounter
{
public:
	KPerfCounter(int id);
	~KPerfCounter();

public:
	int m_id;
	UINT64 m_begCycle;
	UINT64 m_endCycle;
};

class KPerfStat : public ::System::Thread::KThread
{
public:
	enum { max_counter = 4096*4 };

public:
	struct StatRec
	{
		const char* name;
		UINT64 runTimes;
		UINT64 spendCycles;
		UINT64 lastTimes,lastCycles;
		void markDump();
		::System::Collections::KString<512> dumpString() const;
	};
	class cmp_t
	{
	public:
		typedef StatRec* ptr;
		int operator () (const ptr& a, const ptr& b) const
		{
			INT64 x = a->spendCycles - a->lastCycles;
			INT64 y = b->spendCycles - b->lastCycles;
			if(y > x) return 1;
			else if(y == x) return 0;
			else return -1;
		}
	};
	typedef ::System::Collections::KDynSortedArray<StatRec*,cmp_t> RecordArray;

public:
	KPerfStat();
	~KPerfStat();

public:
	static KPerfStat& GetInstance();
	static int SetDumpInterval(int secs);

public:
	bool Register(int id, const char* name);

public:
	void Execute();

private:
	void stop();

public:
	TIME_T m_lastDumpTime;
	int m_dumpInterval;			// second/s
	
	StatRec m_recs[max_counter];
	StatRec m_buffer[max_counter];
	StatRec* m_buffer2[max_counter];

	volatile BOOL m_stopDesired;
	DirectFileLogger m_logger;
};
DECLARE_SIMPLE_TYPE(KPerfStat::StatRec);

#else

class KPerfStat
{
public:
	static KPerfStat& GetInstance() { static KPerfStat x; return x; }
	static int SetDumpInterval(int secs) { return 60; }
	bool Register(int id, const char* name) { return true; }
};

#endif

class KThreadPerfStat;
class KFunctionStat
{
public:
	class ptrcmp
	{
	public: typedef KFunctionStat* ptr;
		int operator () (const ptr& a, const ptr& b) const { return a->m_id - b->m_id; }
	};
	class ptrcmpByCycles
	{
	public: typedef KFunctionStat* ptr;
			int operator () (const ptr& a, const ptr& b) const
			{
				INT64 n = a->m_spendCycles - b->m_spendCycles;
				return n > 0 ? -1 : (n == 0 ? 0 : 1);
			}
	};
	typedef JG_C::KDynSortedArray<KFunctionStat*,ptrcmp,32,32> Children;
	typedef JG_C::KDynSortedArray<KFunctionStat*,ptrcmpByCycles,1024,1024> ArrayByCycles;

public:
	int m_id;
	const char* m_name;
	INT64 m_begCycle;
	INT64 m_runTimes;
	INT64 m_spendCycles;
	INT64 m_lastTimes;
	INT64 m_lastCycles;
	KFunctionStat* m_parent;
	Children m_children;

public:
	inline KFunctionStat(int id):m_id(id)
	{
	}
	inline KFunctionStat()
		: m_id(0)
		, m_name(NULL)
		, m_begCycle(0)
		, m_runTimes(0)
		, m_spendCycles(0)
		, m_lastTimes(0)
		, m_lastCycles(0)
		, m_parent(NULL)
	{
	}
	inline ~KFunctionStat()
	{
	}

public:
	inline KFunctionStat* getChild(int id)
	{
		KFunctionStat tmp(id);
		int pos = m_children.find(&tmp);
		if(pos < 0) return NULL;
		return m_children.at(pos);
	}
	
	void clear(KThreadPerfStat& owner);
	void reset();

	void dump(int level, ILogger* logger);
	void dumpStatistic(int level, ILogger* logger);
	void dumpToArray(ArrayByCycles& arr, int count);
};

class KThreadPerfStat
{
public:
	enum { max_counter = 4096*4 };
	typedef JG_M::KStepObjectPool<KFunctionStat,256> FunctionStatPool;

public:
	INT64 m_frame;
	KFunctionStat* m_root;
	KFunctionStat* m_current;
	KFunctionStat* m_funcs[max_counter];
	KFunctionStat* m_funcs_buf[max_counter];
	FunctionStatPool m_pool;
	time_t m_nextDumpTime;
	
public:
	enum WORK_MODE { STATISTIC_MODE = 0, PULSE_MODE = 1 };
	static int_r m_enabled;
	static WORK_MODE m_mode;
	static INT64 m_modeParam;
	static int m_tlsSlot;

public:
	inline static KThreadPerfStat* _getThreadPerfStat()
	{
		KThreadPerfStat* perfStat;
		if(m_tlsSlot == -1) return NULL;
		KThreadLocalStorage::Get(m_tlsSlot, (void*&)perfStat);
		return perfStat;
	}
	inline static KThreadPerfStat* getThreadPerfStat()
	{
		KThreadPerfStat* perfStat;
		if(m_tlsSlot == -1) KThreadPerfStat::Init();
		KThreadLocalStorage::Get(m_tlsSlot, (void*&)perfStat);
		if(!perfStat)
		{
			perfStat = new KThreadPerfStat();
			KThreadLocalStorage::Set(m_tlsSlot, perfStat);
		}
		return perfStat;
	}

public:
	KThreadPerfStat();
	~KThreadPerfStat();

public:
	static void EnablePerformanceMonitor(BOOL enable, int mode, int param);
	
public:
	static void Init();
	void clear();

public:
	void enter_current(int id, const char* name);
	void leave_current();

public:
	void _dumpCurrentFrame(ILogger* logger);
	void _dumpStatistic(ILogger* logger);
	void _dump();
};

class KThreadPerfCounter
{
public:
	KThreadPerfStat* m_perfStat;
	inline KThreadPerfCounter(int id, const char* funcName):m_perfStat(NULL)
	{
		if(KThreadPerfStat::m_enabled)
		{
			m_perfStat = KThreadPerfStat::getThreadPerfStat();
			if(m_perfStat) m_perfStat->enter_current(id, funcName);
		}
	}
	inline ~KThreadPerfCounter()
	{
		if(KThreadPerfStat::m_enabled)
		{
			if(m_perfStat) m_perfStat->leave_current();
		}
	}
};

//////////////////////////////////////////////////////////
#if defined(DEF_PERF_HITCH_COUNTER)
	#define PERF_COUNTER(id) KPerfHitchMark PerfHitchMarker_##id(id,&KPerfHitchIns::GetInstance())
	#define PERF_COUNTER_2(id,name)
	#define ENABLE_PERF_MONITOR(enable,mode,param)
#elif defined(_MONIT_PERFORMANCE_2_)
	#if defined(WIN32)
		#define PERF_COUNTER(id) KThreadPerfCounter _perf_counter_##id(id,__FUNCTION__);
	#else
		#define PERF_COUNTER(id) KThreadPerfCounter _perf_counter_##id(id,__PRETTY_FUNCTION__);
	#endif
	#define PERF_COUNTER_2(id,name) KThreadPerfCounter _perf_counter_##id(id,name);
	#define ENABLE_PERF_MONITOR(enable,mode,param) KThreadPerfStat::EnablePerformanceMonitor(enable,mode,param);
#elif defined(_MONIT_PERFORMANCE_)
	#define PERF_COUNTER(id) KPerfCounter _perf_counter_##id(id);
	#define PERF_COUNTER_2(id,name)
	#define ENABLE_PERF_MONITOR(enable,mode,param)
#else
	#define PERF_COUNTER(id)
	#define PERF_COUNTER_2(id,name)
	#define ENABLE_PERF_MONITOR(enable,mode,param)
#endif
