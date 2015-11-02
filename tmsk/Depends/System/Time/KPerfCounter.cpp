#include "KPerfCounter.h"
#include "../Memory/KBuffer64k.h"
#include "../File/KFile.h"
#include <time.h>
#include "../Memory/KThreadLocalStorage.h"

#if defined(_MONIT_PERFORMANCE_)

#define g_perfStat KPerfStat::GetInstance()

KPerfCounter::KPerfCounter(int id)
	: m_id(id)
	, m_begCycle(rdtsc())
	, m_endCycle(m_begCycle)
{

}

KPerfCounter::~KPerfCounter()
{
	m_endCycle = rdtsc();
	INT64 n = m_endCycle - m_begCycle;
	g_perfStat.m_recs[m_id].runTimes += 1;
	g_perfStat.m_recs[m_id].spendCycles += n > 0 ? n:0;
}

void KPerfStat::StatRec::markDump()
{
	lastTimes = runTimes;
	lastCycles = spendCycles;
}

::System::Collections::KString<512> KPerfStat::StatRec::dumpString() const
{
	char buf[512];
	static DWORD cpuSecCycles = GetCpuCyclesPerSecond();
	size_t n = sprintf_k(buf, sizeof(buf), "%s    %lld(%lld)    %04f(%04f)",
		name ? name : "no-name",
		runTimes,
		runTimes-lastTimes,
		(double)spendCycles/cpuSecCycles,
		(double)(spendCycles-lastCycles)/cpuSecCycles);
	return ::System::Collections::KString<512>(buf, n);
}

KPerfStat::KPerfStat()
	: m_lastDumpTime(0)
	, m_stopDesired(FALSE)
	, m_dumpInterval(60)
{
	memset(&m_recs, 0, sizeof(m_recs));
	// open log file
	char modfile[512];
	char cleanname[128];
	GetModuleFileName_k(NULL, modfile, sizeof(modfile));
	::System::File::KFileUtil::cleanname_r(modfile, cleanname);
	::System::Collections::KString<256> logfile("./Log/");
	logfile.append(cleanname);
	logfile.append("_perf.log");
	m_logger.Open(logfile.c_str());

	// start thread
	this->Run();
}

KPerfStat::~KPerfStat()
{
	this->stop();
}

KPerfStat& KPerfStat::GetInstance()
{
	static KPerfStat* p = new KPerfStat();
	return *p;
}

int KPerfStat::SetDumpInterval(int secs)
{
	int old = g_perfStat.m_dumpInterval;
	g_perfStat.m_dumpInterval = secs;
	return old;
}

bool KPerfStat::Register(int id, const char* name)
{
	if(id < 0 || id >= max_counter) return false;
	//ASSERT(m_recs[id].name == NULL);

	StatRec& rec = m_recs[id];
	rec.name = KBuffer64k::WriteData(name, (int)strlen(name)+1);
	rec.runTimes = 0;
	rec.spendCycles = 0;
	rec.lastTimes = 0;
	rec.lastCycles = 0;
	
	return true;
}

void KPerfStat::Execute()
{
	RecordArray arr;
	arr.attach(m_buffer2, max_counter, 0);

	if(!m_lastDumpTime)
	{
		m_lastDumpTime = (TIME_T)time(NULL);
	}

	while(!m_stopDesired)
	{
		TIME_T now = (TIME_T)time(NULL);
		if(now < m_lastDumpTime + m_dumpInterval) { Sleep(100); continue; }
		m_lastDumpTime = (TIME_T)time(NULL);

		arr.clear();
		memcpy(&m_buffer, &m_recs, sizeof(m_recs));
		
		for(int i=0; i<max_counter; i++)
		{
			StatRec& rec = m_buffer[i]; m_recs[i].markDump();
			if(rec.runTimes > rec.lastTimes)
				arr.insert(&rec);
		}

		int n = arr.size();
		if(n)
		{
			m_logger.WriteLog("start dump perf-stat >>>>>>>>>>>>>>>>>>>>>>>>>");
			for(int i=0; i<n; i++)
			{
				StatRec& rec = *arr[i];
				m_logger.WriteLog("no.%d	%s", i, rec.dumpString().c_str());
			}
			m_logger.WriteLog("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
		}
	}
}

void KPerfStat::stop()
{
	m_stopDesired = TRUE;
	this->WaitFor();
}

#endif

////

static const char* _cleanName(const char* funcName, char* buf)
{
#if defined(WIN32)
	return funcName;
#else
	const char* smark = strrchr(funcName, '(');
	if(!smark)
		return funcName;

	int pos2 = (int)((uint_r)smark - (uint_r)funcName);
	int i = pos2 - 1;
	while(i >= 0)
	{
		if(funcName[i] == ' ')
		{
			int k =i + 1;
			for(; k<pos2; k++)
				buf[k-(i+1)] = funcName[k];
			buf[k-(i+1)] = '\0';
			return buf;
		}
		else if(!i)
		{
			int k = 0;
			for(; k<pos2; k++) buf[k] = funcName[k];
			buf[k] = '\0';
			return buf;
		}
		i -= 1;
	}
	Log(LOG_WARN, "warn: perf: %s", funcName);
	return "function-name-parse-error";
#endif
}

////
void KFunctionStat::clear(KThreadPerfStat& owner)
{
	while(!m_children.empty())
	{
		KFunctionStat* p = m_children.pop_back();
		p->clear(owner);
		owner.m_pool.Free(p);
	}
}

void KFunctionStat::reset()
{
	m_id = 0;
	m_name = NULL;
	m_begCycle = 0;
	m_runTimes = 0;
	m_spendCycles = 0;
	m_lastTimes = 0;
	m_lastCycles = 0;
	m_parent = NULL;
	m_children.clear();
}

void KFunctionStat::dump(int level, ILogger* logger)
{
	static INT64 cpuSecCycles = GetCpuCyclesPerSecond();
	if(!m_runTimes) return;

	int n = 0;
	char buf[2048];

	if(logger) 
	{
		char cTmp[256];
		for(int i=0; i<level; i++) buf[n++] = '\t';
		sprintf(buf+n, "%s(%d) %llu %.2f(ms)", _cleanName(m_name,cTmp), m_id, m_runTimes, (double)m_spendCycles/cpuSecCycles*1000.f);
		logger->WriteLog("perf: %s", buf);
	}
	
	int c = m_children.size();
	for(int i=0; i<c; i++)
	{
		KFunctionStat* child = m_children.at(i);
		child->dump(level+1, logger);
	}
	m_lastTimes += m_runTimes; m_runTimes = 0;
	m_lastCycles += m_spendCycles; m_spendCycles = 0;
}

void KFunctionStat::dumpStatistic(int level, ILogger* logger)
{
	static INT64 cpuSecCycles = GetCpuCyclesPerSecond();
	if(!m_runTimes) return;
	
	m_lastTimes += m_runTimes;
	m_lastCycles += m_spendCycles;
	
	if(logger)
	{
		int n = 0;
		char buf[2048];
		char cTmp[256];
		for(int i=0; i<level; i++) buf[n++] = '\t';
		sprintf(buf+n, "%s(%d) %llu/%llu %.2f/%.2f(ms)",
			_cleanName(m_name,cTmp), m_id,
			m_runTimes, m_lastTimes,
			(double)m_spendCycles/cpuSecCycles*1000.f, (double)m_lastCycles/cpuSecCycles*1000.f);
		logger->WriteLog("perf: %s", buf);
	}

	int c = m_children.size();
	for(int i=0; i<c; i++)
	{
		KFunctionStat* child = m_children.at(i);
		child->dumpStatistic(level+1, logger);
	}
	
	m_runTimes = 0;
	m_spendCycles = 0;
}

void KFunctionStat::dumpToArray(ArrayByCycles& arr, int count)
{
	if(arr.size() < count)
	{
		arr.insert(this);
		if(arr.size() >= count)
			return;
	}
	int c = m_children.size();
	for(int i=0; i<c; i++)
	{
		KFunctionStat* child = m_children.at(i);
		child->dumpToArray(arr, count);
	}
}

/// KThreadPerfStat

int KThreadPerfStat::m_tlsSlot = -1;
int_r KThreadPerfStat::m_enabled = 0;
KThreadPerfStat::WORK_MODE KThreadPerfStat::m_mode = KThreadPerfStat::STATISTIC_MODE;
INT64 KThreadPerfStat::m_modeParam = 60*1000;

KThreadPerfStat::KThreadPerfStat()
	: m_frame(0)
	, m_root(NULL)
	, m_current(NULL)
	, m_nextDumpTime(0)
{
	memset(&m_funcs, 0, sizeof(m_funcs));
}

KThreadPerfStat::~KThreadPerfStat()
{

}

void KThreadPerfStat::EnablePerformanceMonitor(BOOL enable, int mode, int param)
{
	static INT64 cpuSecCycles = GetCpuCyclesPerSecond();
	
	if(enable)
	{
		switch(mode)
		{
		case PULSE_MODE:
			{
				m_mode = PULSE_MODE;
				m_modeParam = (INT64)((param/1000.f) * cpuSecCycles);
				m_enabled = TRUE;
				KThreadPerfStat* perfStat = getThreadPerfStat();
				perfStat->_dumpCurrentFrame(NULL);
			}
			break;
		case STATISTIC_MODE:
			{
				m_mode = STATISTIC_MODE;
				m_modeParam = param;
				m_enabled = TRUE;
				KThreadPerfStat* perfStat = getThreadPerfStat();
				perfStat->m_nextDumpTime = 1;
				perfStat->_dumpStatistic(NULL);
			}
			break;
		default:
			break;
		}
	}
	else
	{
		KThreadPerfStat* perfStat = _getThreadPerfStat();
		if(perfStat) perfStat->clear();
		m_enabled = FALSE;
		return;
	}
}

void KThreadPerfStat::_dump()
{
	switch(m_mode)
	{
	case PULSE_MODE:
		this->_dumpCurrentFrame(GetGlobalLogger().GetLogger());
		return;
	case STATISTIC_MODE:
		this->_dumpStatistic(GetGlobalLogger().GetLogger());
		return;
	default: return;
	}
}

void KThreadPerfStat::Init()
{
	static JG_S::KThreadMutex m_mx;

	if(m_tlsSlot != -1) return;
	JG_S::KAutoThreadMutex mx(m_mx);
	if(m_tlsSlot != -1) return;
	
	m_tlsSlot = KThreadLocalStorage::Alloc();
	ASSERT(m_tlsSlot != -1);
	
	return;
}

void KThreadPerfStat::clear()
{
	if(m_root)
	{
		m_root->clear(*this);
		m_pool.Free(m_root);
		m_root = NULL;
	}
	m_current = NULL;
	memset(&m_funcs, 0, sizeof(m_funcs));
}

void KThreadPerfStat::enter_current(int id, const char* name)
{
	KFunctionStat* c = m_funcs[id];
	if(c && c->m_parent == m_current)
	{
		m_current = c;
		c->m_begCycle = rdtsc();
	}
	else
	{
		if(m_current)
		{
			c = m_current->getChild(id);
			if(!c)
			{
				c = m_pool.Alloc(); c->reset();
			
				c->m_id = id;
				c->m_name = name;
				c->m_parent = m_current;

				m_current->m_children.insert(c);
				if(!m_funcs[id])
					m_funcs[id] = c;
			}
			m_current = c;
			c->m_begCycle = rdtsc();
		}
		else
		{
			if(!id) //root
			{
				c = m_pool.Alloc(); c->reset();
				c->m_id = id;
				c->m_name = name;
				c->m_parent = NULL;
				m_root = c;
				if(!m_funcs[id])
					m_funcs[id] = c;

				m_current = c;
				c->m_begCycle = rdtsc();
			}
		}
	}
}

void KThreadPerfStat::leave_current()
{
	if(m_current)
	{
		INT64 endCycle = rdtsc();
		m_current->m_runTimes += 1;
		m_current->m_spendCycles += endCycle > m_current->m_begCycle ? endCycle - m_current->m_begCycle : 0;
		m_current = m_current->m_parent;
		if(!m_current)
		{
			m_frame++;
			this->_dump();
		}
	}
}

void KThreadPerfStat::_dumpCurrentFrame(ILogger* logger)
{
	char cTmp[256];
	char buf[1024];
	if(m_root)
	{
		if(m_mode == PULSE_MODE)
		{
			if(m_root->m_spendCycles >= m_modeParam)
			{
				if(logger)
				{	
					static INT64 cpuSecCycles = GetCpuCyclesPerSecond();
					KFunctionStat::ArrayByCycles arr;
					arr.attach(m_funcs_buf, max_counter, 0);
					m_root->dumpToArray(arr, max_counter);
					int n = kmin(arr.size(), 100);
					for(int i=0; i<n; i++)
					{
						KFunctionStat* p = arr.at(i);
						sprintf(buf, "no.%d %s(%d) %llu %.2f(ms)", i+1, _cleanName(p->m_name,cTmp), p->m_id, p->m_runTimes,
							(double)p->m_spendCycles/cpuSecCycles*1000.f);
						logger->WriteLog("perf: [desc] %s", buf);
					}
				}
				m_root->dump(0, logger);
			}
			else
			{
				//if(logger)
				//{
				//	static INT64 cpuSecCycles = GetCpuCyclesPerSecond();
				//	sprintf(buf, "%s(%d) %llu %.2f(ms)", _cleanName(m_root->m_name,cTmp), m_root->m_id, m_root->m_runTimes,
				//		(double)m_root->m_spendCycles/cpuSecCycles*1000.f);
				//	logger->WriteLog("perf: [frame %llu] %s", m_frame, buf);
				//}
				m_root->dump(0, NULL);
			}
		}
	}
}

void KThreadPerfStat::_dumpStatistic(ILogger* logger)
{
	if(m_mode == STATISTIC_MODE)
	{
		if(!m_nextDumpTime)
		{
			m_nextDumpTime = (time_t)(time(NULL) + ((int)m_modeParam)/1000);
			return;
		}

		time_t now = time(NULL);
		if(now >= m_nextDumpTime)
		{
			static INT64 cpuSecCycles = GetCpuCyclesPerSecond();

			if(logger)
			{
				KFunctionStat::ArrayByCycles arr;
				arr.attach(m_funcs_buf, max_counter, 0);
				m_root->dumpToArray(arr, max_counter);

				char buf[1024];
				char cTmp[256];
				int n = kmin(arr.size(), 100);
				for(int i=0; i<n; i++)
				{
					KFunctionStat* p = arr.at(i);
					sprintf(buf, "no.%d %s(%d) %llu %.2f(ms)", i+1, _cleanName(p->m_name,cTmp), p->m_id, p->m_runTimes,
						(double)p->m_spendCycles/cpuSecCycles*1000.f);
					logger->WriteLog("perf: [desc] %s", buf);
				}
			}
			if(m_root)
				m_root->dumpStatistic(0, logger);

			m_nextDumpTime = (time_t)(now + ((int)m_modeParam)/1000);
		}
	}
}
