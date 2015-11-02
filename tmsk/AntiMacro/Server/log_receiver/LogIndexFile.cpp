#include "LogIndexFile.h"
#include <System/File/KFile.h>
#include <System/Misc/KDatetime.h>
#include "Application.h"

// KMMap m_mmap;
// int m_currHour;
// HourIndcator* m_hours;
// time_t m_dayExpireTime;
// time_t m_hourExpireTime;

LogIndexFile::LogIndexFile()
{
	m_currHour = 0;
	m_hours = NULL;
	m_begingDayTime = m_endDayTime = 0;
	m_beginHourTime = m_endHourTime = 0;
}

LogIndexFile::~LogIndexFile()
{

}

bool LogIndexFile::open(const char* logFilePath)
{
	char buf[512];
	const char* dirName = JG_F::KFileUtil::dirname_r(logFilePath, buf);
	const char* shortName = JG_F::KFileUtil::basename_r(logFilePath);

	const char* underline = strrchr(shortName, '_');
	const char* dot = strrchr(shortName, '.');

	int pos = 0;
	char baseName[64];
	const char* pc = shortName;
	while(pc < underline) baseName[pos++] = *pc++; baseName[pos] = '\0';

	int yyyymmdd;
	int n = sscanf(underline+1, "%d.log", &yyyymmdd);
	if(n != 1)
	{
		Log(LOG_ERROR, "error: invalid log file name %s", logFilePath);
		return false;
	}

	char idxFilePath[512];
	int len = sprintf_k(idxFilePath, sizeof(idxFilePath), "%s\\%s_%d.idx", dirName, baseName, yyyymmdd);
	if(!m_mmap.Initialize(idxFilePath, 0, IDX_FILE_SIZE, FALSE))
	{
		Log(LOG_ERROR, "error: initialize mmap for %s", idxFilePath);
		return false;
	}

	int yyyy = yyyymmdd / 10000;
	int mm = (yyyymmdd - yyyy*10000) / 100;
	int dd = yyyymmdd - yyyy * 10000 - mm * 100;

	KLocalDatetime ldt = {yyyy,mm,dd,0,0,0};
	KDatetime dt(ldt);

	m_begingDayTime = (time_t)dt.Time();
	m_endDayTime = m_begingDayTime + KDatetime::day_seconds;

	KMMap::eFileOpenStatus openStatus = m_mmap.GetOpenStatus();
	switch(openStatus)
	{
	case KMMap::enum_CreateNew:
		return this->_onCreate();
	case KMMap::enum_OpenExists:
		return this->_onOpen();
	case KMMap::enum_ModifyExists:
		return this->_onCreate();
	}
	return false;
}

void LogIndexFile::close()
{
	m_mmap.Finalize();
}

bool LogIndexFile::_onCreate()
{
	m_hours = (HourIndcator*)m_mmap.GetMemory();
	memset(m_hours, 0, IDX_FILE_SIZE);
	return true;
}

bool LogIndexFile::_onOpen()
{
	m_hours = (HourIndcator*)m_mmap.GetMemory();
	return true;
}

bool LogIndexFile::on_logLine(KDatetime& lineTime, KLocalDatetime& ldt, INT64 lineOffset)
{
	if(lineTime < m_begingDayTime)
		return false;
	
	if(lineTime >= m_endDayTime)
		return false;

	KDatetime& dt(lineTime);
	int hour = ldt.hour;

	if(m_hours[hour].used)
		return true;

	m_hours[hour].used = 1;
	m_hours[hour].offset = lineOffset;

	return true;
}

////

//FILE* m_dataFile;
//LogIndexFile m_idxFile;

OutputLogFile::OutputLogFile()
{
	m_dataFile = NULL;
	m_fileOffset = 0;
	m_lastActive = 0;
	m_freshLineCount = 0;
}

OutputLogFile::~OutputLogFile()
{
	this->close();
}

bool OutputLogFile::open(const char* filePath)
{
	m_dataFile = JG_F::KFileUtil::OpenFileForce(filePath, "a+b");
	if(!m_dataFile)
	{
		Log(LOG_ERROR, "error: open or create %s", filePath);
		return false;
	}
	if(!m_idxFile.open(filePath))
	{
		this->close();
		return false;
	}
	m_fileOffset = ftell(m_dataFile);
	return true;
}

bool OutputLogFile::writeLine(KDatetime& lineTime, KLocalDatetime& ldt, const char* verb, KPropertyMap& lmap)
{
	char line[1024];

	KDatetime& dt(lineTime);
	time_t time = (time_t)dt.Time();
	m_lastActive = g_pApp->m_nowTime;
	
	int len = sprintf_k(line, sizeof(line), "%s: %s ", dt.ToString().c_str(), verb);
	len += lmap.serialize(&line[len], sizeof(line)-len);
	line[len++] = '\n';
	line[len] = '\0';

	int bytes = (int)fwrite(line, 1, len, m_dataFile);
	if(bytes != len)
	{
		Log(LOG_ERROR, "error: fail to write data log file");
		return false;
	}

	m_idxFile.on_logLine(lineTime, ldt, m_fileOffset);
	m_fileOffset += len;
	
	if(++m_freshLineCount >= 32)
	{
		fflush(m_dataFile);
		m_freshLineCount = 0;
	}

	return true;
}

void OutputLogFile::flush()
{
	if(m_dataFile)
	{
		fflush(m_dataFile);
		m_freshLineCount = 0;
	}
}

void OutputLogFile::close()
{
	if(m_dataFile)
	{
		fflush(m_dataFile);
		fclose(m_dataFile);
		m_dataFile = NULL;
	}
	m_idxFile.close();
	m_fileOffset = 0;
	m_lastActive = 0;
	m_freshLineCount = 0;
}
