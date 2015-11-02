#ifndef _DATETIME_H_
#define _DATETIME_H_

#include "../KType.h"
#include "../Collections/KString.h"
#include <time.h>

#ifdef ToString
#undef ToString
#endif

struct KLocalTime
{
	int hour;
	int minute;
	int second;
};

struct KLocalDatetime
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
};

class KDatetime
{
public:
	KDatetime();
	KDatetime(INT64 t);
	KDatetime(const char* dtstr);
	KDatetime(const KLocalDatetime& ldt);
	KDatetime(const KDatetime& other);
	KDatetime& operator=(const KDatetime& other);
	~KDatetime();

public:
	static TIME_T getDayFirstSecond(TIME_T t);
	static TIME_T getWeekFirstSecond(TIME_T t);
	static TIME_T getMonthFirstSecond(TIME_T t);
	static TIME_T nextDayFirstSecond(TIME_T t);
	static TIME_T nextWeekFirstSecond(TIME_T t);
	static TIME_T nextMonthFirstSecond(TIME_T t);
	static TIME_T nextYearFirstSecond(TIME_T t);	
public:
	BOOL operator==(const KDatetime& other) const;
	BOOL operator!=(const KDatetime& other) const;
	BOOL operator >(const KDatetime& other) const;
	BOOL operator <(const KDatetime& other) const;
	BOOL operator>=(const KDatetime& other) const;
	BOOL operator<=(const KDatetime& other) const;

public:
	KDatetime& Refresh();				// refresh to current time
	KDatetime& AddSecond(int secs);
	KDatetime& AddDay(int days);
	BOOL SetLocalTime(const KLocalDatetime& ldt);

	// YYYY-MM-DD HH24:MI:SS
	BOOL SetDatetimeString(const char* datetimeStr);
	BOOL SetDatetimeString2(const char* datetimeStr);
	
	KLocalDatetime GetLocalTime() const;
	INT64 Time() const;					// seconds since 1970/1/1 0:0:0 (UTC)
	int ToDays() const;					// days since 1970/1/1 0:0:0 (UTC)
	int ToLocalDays() const;			// to local days
	int WeekDay() const;
	int MonthDay() const;

	::System::Collections::KString<32> ToString() const;		// YYYY-MM-DD HH24:MI:SS

public:
	enum   { day_seconds=24*3600,hour_seconds=3600,min_seconds=60 };
	static BOOL  IsLeapYear(int year);
	static BOOL  IsValidDatetime(const KLocalDatetime& ldt);
	static int   GetMonthDays(int year, int month);
	static INT64 GetYearFirstTime(int year);
	static INT64 GetYearFirstTimeFast(int year);

private:
	INT64 m_secs; // seconds since 1970/1/1 0:0:0 (UTC)
};
#endif