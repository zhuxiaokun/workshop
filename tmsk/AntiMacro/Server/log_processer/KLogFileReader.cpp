#include "KLogFileReader.h"
#include <System/Misc/KDatetime.h>
#include <System/File/KFile.h>
#include "Application.h"
#include <time.h>
#include <System/Misc/StrUtil.h>
#include "app_events.h"

KLogFileReader::KLogFileReader(KLogFileReader::LogFile& logFile)
{
	m_readState = ReadState::none;
	m_logFile = &logFile;
	m_lastModifyTime = 0;
	m_lastSuccessReadTime = 0;
	m_frameLineCount = 0;
	m_breatheLineCount = 0;
	this->startTimer(AppTimer_SearchLogFile, 0, 0, NULL);
}

KLogFileReader::~KLogFileReader()
{

}

void KLogFileReader::breathe(int interval)
{
	m_breatheLineCount = 0;
	while(1)
	{
		m_frameLineCount = 0;
		KEventEntity::breathe(interval);
		m_breatheLineCount += m_frameLineCount;
		if(!m_frameLineCount || m_breatheLineCount > 10000)
			break;
		interval = 0;
	}
}

bool KLogFileReader::openCurrentLogFile()
{
	string_512 filePath = g_pApp->m_statusFile.buildLogFilePath(*m_logFile);
	
	if(!m_fi.Open(filePath.c_str(), "rb"))
		return false;

	if(!m_fi.Seek(m_logFile->readOffset))
	{
		string_512 errText = strerror_r2(GetLastError());
		Log(LOG_ERROR, "error: '%s' seek to offset %d, %s", filePath.c_str(), m_logFile->readOffset, errText.c_str());
		m_fi.Close();
		return false;
	}
	
	m_readState = ReadState::reading;
	m_lastSuccessReadTime = time(NULL);
	m_lastModifyTime = JG_F::KFileUtil::GetLastModified(filePath.c_str());

	return true;
}

bool KLogFileReader::searchNextLogFile(OUT LogFile* logFile)
{
	KDatetime now;
	KLocalDatetime ldt = now.GetLocalTime();
	ldt.hour = ldt.minute = ldt.second = 0;
	now.SetLocalTime(ldt);

	memset(&ldt, 0, sizeof(ldt));
	ldt.year = m_logFile->yyyymmdd / 10000;
	ldt.month = (m_logFile->yyyymmdd / 100) % 100;
	ldt.day = m_logFile->yyyymmdd % 100;
	KDatetime dt(ldt); dt.AddDay(1);

	*logFile = *m_logFile;
	while(dt <= now)
	{
		ldt = dt.GetLocalTime();
		int year = ldt.year; int month = ldt.month; int day = ldt.day;
		int yyyymmdd = year * 10000 + month * 100 + day;
		logFile->yyyymmdd = yyyymmdd;
		logFile->sequence = 0;
		logFile->readOffset = 0;
		string_512 filePath = g_pApp->m_statusFile.buildLogFilePath(*logFile);
		if(JG_F::KFileUtil::IsFileExists(filePath.c_str()))
			return true;
		dt.AddDay(1);
	}

	return false;
}

void KLogFileReader::_close()
{
	m_fi.Close();
	m_readBuf.Reset();
	m_readState = ReadState::none;
	m_frameLineCount = 0;
	m_breatheLineCount = 0;
}

bool KLogFileReader::_readToBuffer()
{
	if(!m_fi.Good())
		return false;
	
	//printf("begin read buffer from %d\n", m_logFile->readOffset);

	if(!m_fi.Seek(m_logFile->readOffset))
		return false;

	m_readBuf.Reset();

	void* p = m_readBuf.GetWriteBuffer();
	size_t n = m_readBuf.Capacity();

	int readed = m_fi.ReadData(p, n);
	if(readed < (int)n)
	{
		m_readState = ReadState::eof;
	}

	if(readed < 1)
		return false;

	m_readBuf.m_wp += readed;
	m_lastSuccessReadTime = time(NULL);

	return true;
}

int KLogFileReader::_readLine(char** linepp)
{
	int avail = m_readBuf.ReadAvail();
	if(avail < 1)
	{
		*linepp = NULL;
		return -1;
	}

	char ch;
	const char* pc = m_readBuf.GetReadBuffer();
	*linepp = (char*)pc;

	int i = 0;
	while(i < avail)
	{
		ch = *pc;
		if(ch == '\n')
		{
			*(char*)pc = '\0';
			m_readBuf.m_rp += i + 1;
			m_logFile->readOffset += i + 1;
			return i;
		}
		pc++; i++;
	}

	m_readBuf.Reset();

	*linepp = NULL;
	return 0;
}

extern void process_log_line(const char* line);
void KLogFileReader::_processLogLine(char* line)
{
	Log(LOG_DEBUG, "[line] %s", line);
	process_log_line(line);
	return;
}

bool KLogFileReader::onEvent(int evtId, const void* data, size_t len)
{
	return true;
}

bool KLogFileReader::onTimer(DWORD name, Identity64 cookie, const void* data)
{
	switch(name)
	{
	case AppTimer_SearchLogFile:
		return this->onTimer_searchLogFile(name, cookie, data);
	case AppTimer_SearchNextLogFile:
		return this->onTimer_searchNextLogFile(name, cookie, data);
	case AppTimer_ReadLogFile:
		return this->onTimer_readLogFile(name, cookie, data);
	case AppTimer_WaitDatabase:
		return this->onTimer_waitDatabase(name, cookie, data);
	}
	return true;
}

bool KLogFileReader::onTimer_searchLogFile(DWORD name, Identity64 cookie, const void* data)
{
	if(!this->openCurrentLogFile())
	{
		this->startTimer(AppTimer_SearchNextLogFile, 0, 5.0f, NULL);
		return true;
	}

	this->startTimer(AppTimer_ReadLogFile, 0, 0, NULL);
	return true;
}

bool KLogFileReader::onTimer_searchNextLogFile(DWORD name, Identity64 cookie, const void* data)
{
	LogFile logFile;
	if(this->searchNextLogFile(&logFile))
	{
		*m_logFile = logFile;
		this->startTimer(AppTimer_SearchLogFile, 0, 0.0f, NULL);
		return true;
	}
	this->startTimer(AppTimer_SearchLogFile, 0, 5.0f, NULL);
	return true;
}

static int lineCount = 0;
// read to buffer
bool KLogFileReader::onTimer_readLogFile(DWORD name, Identity64 cookie, const void* data)
{
	if(!m_readBuf.ReadAvail())
	{
		if(!this->_readToBuffer())
		{
			time_t now = time(NULL);
			if(now - m_lastSuccessReadTime > 20)
			{
				this->_close();
				this->startTimer(AppTimer_SearchNextLogFile, 0, 1.0f, NULL);
			}
			else
			{
				this->startTimer(AppTimer_ReadLogFile, 0, 1.0f, NULL);
			}
			return true;
		}
	}

	char* line;
	while(1)
	{
		if(!g_pApp->isDatabaseReady())
		{
			this->startTimer(AppTimer_WaitDatabase, 0, 2.f, NULL);
			return true;
		}

		int n = this->_readLine(&line);
		if(n < 0 || !line) break;

		lineCount++;
		
		m_frameLineCount++;
		line = _trim(line);

		if(line[0])
		{
			this->_processLogLine(line);
			g_pApp->m_msFrameSleep = 0;
		}
	}

	this->startTimer(AppTimer_ReadLogFile, 0, 0.0f, NULL);
	return true;
}

bool KLogFileReader::onTimer_waitDatabase(DWORD name, Identity64 cookie, const void* data)
{
	if(g_pApp->isDatabaseReady())
	{
		this->startTimer(AppTimer_ReadLogFile, 0, 0.0f, NULL);
		return true;
	}
	this->startTimer(AppTimer_WaitDatabase, 0, 2.f, NULL);
	return true;
}
