#pragma once
#include <System/KPlatform.h>
#include <System/File/KMMap.h>
#include <stdio.h>
#include <CommSession/KPropertyMap.h>
#include <System/Misc/KDatetime.h>

class LogIndexFile
{
public:
	struct HourIndcator
	{
		int hour;
		int used;
		INT64 offset;
	};
	enum { HOUR_COUNT = 24, IDX_FILE_SIZE = HOUR_COUNT*sizeof(HourIndcator), };

public:
	KMMap m_mmap;
	int m_currHour;
	HourIndcator* m_hours;
	time_t m_begingDayTime, m_endDayTime;
	time_t m_beginHourTime, m_endHourTime;

public:
	LogIndexFile();
	~LogIndexFile();

public:
	bool open(const char* logFilePath);
	void close();

public:
	bool _onCreate();
	bool _onOpen();
	
public:
	bool on_logLine(KDatetime& lineTime, KLocalDatetime& ldt, INT64 lineOffset);
};

class OutputLogFile : public JG_M::KPortableStepPool<OutputLogFile,1024>
{
public:
	FILE* m_dataFile;
	INT64 m_fileOffset;
	LogIndexFile m_idxFile;
	time_t m_lastActive;
	int m_freshLineCount;

public:
	OutputLogFile();
	~OutputLogFile();

public:
	bool open(const char* filePath);
	bool writeLine(KDatetime& lineTime, KLocalDatetime& ldt, const char* verb, KPropertyMap& lmap);
	void flush();
	void close();
};
