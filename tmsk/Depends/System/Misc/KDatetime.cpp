#include "KDatetime.h"
#include "StrUtil.h"
#include <stdio.h>
#include "../Sync/KSync.h"
#include <time.h>

// 初始化timezone全局变量
static KDatetime dt_dummy;
static KLocalDatetime ldt_dummy = dt_dummy.GetLocalTime();

KDatetime::KDatetime():m_secs(time(NULL))
{
}

KDatetime::KDatetime(INT64 t):m_secs(t)
{
}

KDatetime::KDatetime(const char* dtstr):m_secs(0)
{
	this->SetDatetimeString(dtstr);
}

KDatetime::KDatetime(const KLocalDatetime& ldt)
{
	ASSERT(this->SetLocalTime(ldt));
}

KDatetime::KDatetime(const KDatetime& other):m_secs(other.m_secs)
{
}

KDatetime& KDatetime::operator=(const KDatetime& other)
{
	if(this == &other) return *this;
	m_secs = other.m_secs;
	return *this;
}

KDatetime::~KDatetime()
{
}

TIME_T KDatetime::getDayFirstSecond(TIME_T t)
{
	KDatetime dt(t);
	KLocalDatetime ldt = dt.GetLocalTime();
	ldt.hour = ldt.minute = ldt.second = 0;
	dt.SetLocalTime(ldt);
	return (TIME_T)dt.Time();
}

TIME_T KDatetime::getWeekFirstSecond(TIME_T t)
{
	KDatetime dt(t);
	
	KLocalDatetime ldt = dt.GetLocalTime();
	ldt.hour = ldt.minute = ldt.second = 0;
	dt.SetLocalTime(ldt);

	int weekDay = dt.WeekDay();
	dt.AddDay(1-weekDay);
	return (TIME_T)dt.Time();
}

TIME_T KDatetime::getMonthFirstSecond(TIME_T t)
{
	KDatetime dt(t);
	KLocalDatetime ldt = dt.GetLocalTime();
	ldt.hour = ldt.minute = ldt.second = 0;
	dt.SetLocalTime(ldt);
	
	dt.AddDay(1-ldt.day);
	return (TIME_T)dt.Time();
}

TIME_T KDatetime::nextDayFirstSecond(TIME_T t)
{
	return KDatetime::getDayFirstSecond(t) + day_seconds;
}

TIME_T KDatetime::nextWeekFirstSecond(TIME_T t)
{
	return KDatetime::getWeekFirstSecond(t) + day_seconds*7;
}

TIME_T KDatetime::nextMonthFirstSecond(TIME_T t)
{
	KDatetime dt(t);
	KLocalDatetime ldt = dt.GetLocalTime();
	ldt.hour = ldt.minute = ldt.second = 0;
	dt.SetLocalTime(ldt);

	dt.AddDay(1-ldt.day);

	int mdays = KDatetime::GetMonthDays(ldt.year,ldt.month);
	return (TIME_T)dt.Time() + day_seconds*mdays;
}

TIME_T KDatetime::nextYearFirstSecond(TIME_T t)
{
	KDatetime dt(t);
	KLocalDatetime ldt = dt.GetLocalTime();
	return  (TIME_T)KDatetime::GetYearFirstTime(ldt.year+1);
}

BOOL KDatetime::operator==(const KDatetime& other) const
{
	return m_secs == other.m_secs;
}

BOOL KDatetime::operator!=(const KDatetime& other) const
{
	return m_secs != other.m_secs;
}

BOOL KDatetime::operator >(const KDatetime& other) const
{
	return m_secs > other.m_secs;
}

BOOL KDatetime::operator <(const KDatetime& other) const
{
	return m_secs < other.m_secs;
}

BOOL KDatetime::operator>=(const KDatetime& other) const
{
	return m_secs >= other.m_secs;
}

BOOL KDatetime::operator<=(const KDatetime& other) const
{
	return m_secs <= other.m_secs;
}

KDatetime& KDatetime::Refresh()
{
	m_secs = time(NULL);
	return *this;
}

KDatetime& KDatetime::AddSecond(int secs)
{
	m_secs += secs;
	return *this;
}

KDatetime& KDatetime::AddDay(int days)
{
	m_secs += days * day_seconds;
	return *this;
}

BOOL KDatetime::SetLocalTime(const KLocalDatetime& ldt)
{
	if(!KDatetime::IsValidDatetime(ldt)) return FALSE;
	
	m_secs = KDatetime::GetYearFirstTimeFast(ldt.year);
	if(m_secs < 0) m_secs = 0;

	for(int i=1; i<ldt.month; i++)
	{
		int days = KDatetime::GetMonthDays(ldt.year, i);
		m_secs += days * day_seconds;
	}

	m_secs += (ldt.day-1) * day_seconds;
	m_secs += ldt.hour * hour_seconds;
	m_secs += ldt.minute * min_seconds;
	m_secs += ldt.second;
	return TRUE;
}

BOOL KDatetime::SetDatetimeString(const char* datetimeStr)
{
	KLocalDatetime ldt;
	sscanf(datetimeStr, "%d-%d-%d %d:%d:%d",
		&ldt.year, &ldt.month, &ldt.day,
		&ldt.hour, &ldt.minute, &ldt.second);
	return this->SetLocalTime(ldt);
}

BOOL KDatetime::SetDatetimeString2(const char* datetimeStr)
{
	char* ss[6];
	char cTmp[128];

	int len = (int)strlen(datetimeStr);
	if(len >= 128) len = 100;

	memcpy(cTmp, datetimeStr, len);
	cTmp[len] = '\0';

	KLocalDatetime ldt;
	memset(&ldt, 0, sizeof(ldt));

	int n = split(cTmp, "+-:/\\ \t", ss, 6);
	if(n < 3) return FALSE;

	ldt.year = str2int(ss[0]);
	ldt.month = str2int(ss[1]);
	ldt.day = str2int(ss[2]);

	if(n >= 4) ldt.hour = str2int(ss[3]);
	if(n >= 5) ldt.minute = str2int(ss[4]);
	if(n >= 6) ldt.second = str2int(ss[5]);

	return this->SetLocalTime(ldt);
}

KLocalDatetime KDatetime::GetLocalTime() const
{
	enum { max_year_secs = 366*day_seconds };
	static const KLocalDatetime emptyDatetime = {0,0,0,0,0,0};

	if(m_secs < 0) return emptyDatetime;

	int ys = (int)(m_secs/max_year_secs);
	KLocalDatetime ldt = {ys+1970,1,1,0,0,0};

	INT64 secs = KDatetime::GetYearFirstTimeFast(ldt.year);
	if(secs < 0) return emptyDatetime;

	while(TRUE)
	{
		INT64 secs2 = KDatetime::GetYearFirstTimeFast(ldt.year+1);
		if(secs2 < 0) return emptyDatetime;

		if(m_secs < secs2) break;
		ldt.year += 1;
		secs = secs2;
	}

	INT64 delta = m_secs - secs;
	for(; ldt.month<=12; ldt.month++)
	{
		int days = KDatetime::GetMonthDays(ldt.year, ldt.month);
		if(delta >= days*day_seconds)
		{
			delta -= days*day_seconds;
		}
		else
		{
			for(; ldt.day<=days; ldt.day++)
			{
				if(delta >= day_seconds)
				{
					delta -= day_seconds;
				}
				else
				{
					ldt.hour = (int)(delta/hour_seconds);
					ldt.minute = (int)((delta%hour_seconds)/min_seconds);
					ldt.second = (int)((delta%hour_seconds)%min_seconds);
					return ldt;
				}
			}
		}
	}

	// 不可达
	return emptyDatetime;
}

INT64 KDatetime::Time() const
{
	return m_secs;
}

int KDatetime::ToDays() const
{
#if defined(WIN32) || defined(WINDOWS)
	TIME_ZONE_INFORMATION tzi;
	DWORD dwRet(GetTimeZoneInformation(&tzi));
	int iTimeZone((0 - tzi.Bias) / 60);
#else
	int iTimeZone = -timezone / 60 / 60;
#endif

	return (int)((m_secs + iTimeZone*3600) / day_seconds + 1);
	//return (int)(m_secs / day_seconds + 1);
}

int KDatetime::ToLocalDays() const
{
#if defined(WIN32) || defined(WINDOWS)
	TIME_ZONE_INFORMATION tzi;
	DWORD dwRet(GetTimeZoneInformation(&tzi));
	int iTimeZone((0 - tzi.Bias) / 60);
#else
	int iTimeZone = -timezone / 60 / 60;
#endif
	return (int)((m_secs+iTimeZone*3600) / day_seconds + 1);
}

int KDatetime::WeekDay() const
{
	int days = this->ToLocalDays();
	//1970年1月1日是星期四
	//周1返回1...周日返回7
	int n = (days + 3) % 7;
	return n ? n : 7;
}

int KDatetime::MonthDay() const
{
	KLocalDatetime ldt = this->GetLocalTime();
	return ldt.day;
}

::System::Collections::KString<32> KDatetime::ToString() const
{
	char cdt[32];
	KLocalDatetime ldt = this->GetLocalTime();
	sprintf_k(cdt, sizeof(cdt), "%04d-%02d-%02d %02d:%02d:%02d",
		ldt.year,
		ldt.month,
		ldt.day,
		ldt.hour,
		ldt.minute,
		ldt.second);
	return ::System::Collections::KString<32>(cdt);
}

BOOL KDatetime::IsLeapYear(int year)
{
	if((year % 4) == 0)
	{
		if((year % 100) == 0 && (year % 400) != 0)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL KDatetime::IsValidDatetime(const KLocalDatetime& ldt)
{
	if(ldt.month < 1 || ldt.month > 12) return FALSE;
	int days = KDatetime::GetMonthDays(ldt.year, ldt.month);
	if(ldt.day < 1 || ldt.day > days) return FALSE;
	if(ldt.hour < 0 || ldt.hour > 24) return FALSE;
	if(ldt.minute < 0 || ldt.minute > 60) return FALSE;
	if(ldt.second < 0 || ldt.second > 60) return FALSE;
	return TRUE;
}

int KDatetime::GetMonthDays(int year, int month)
{
	static int leapDays[] = {31,29,31,30,31,30,31,31,30,31,30,31};
	static int nonleapDays[] = {31,28,31,30,31,30,31,31,30,31,30,31};
	if(KDatetime::IsLeapYear(year))
	{
		return leapDays[month-1];
	}
	else
	{
		return nonleapDays[month-1];
	}
}

INT64 KDatetime::GetYearFirstTime(int year)
{
	struct tm stm = {0,0,0,1,0,year-1900,0,0,0};
	return mktime(&stm);
}

INT64 KDatetime::GetYearFirstTimeFast(int year)
{
	#define year_count 200
	const int startYear = 1971;
	const int endYear = startYear + year_count;

	static ::System::Sync::KThreadMutex mx;
	static INT64 yearSecs[year_count];
	volatile static BOOL bInit = FALSE;

	if(year<startYear || year>=endYear)
	{
		return KDatetime::GetYearFirstTime(year);
	}
	else
	{
		if(bInit)
		{
			return yearSecs[year-startYear];
		}
		else
		{
			::System::Sync::KAutoThreadMutex am(mx);
			if(bInit)
			{
				return yearSecs[year-startYear];
			}
			else
			{
				for(int i=0; i<year_count; i++)
				{
					yearSecs[i] = KDatetime::GetYearFirstTime(startYear+i);
				}
				bInit = TRUE;
				return yearSecs[year-startYear];
			}
		}
	}
}