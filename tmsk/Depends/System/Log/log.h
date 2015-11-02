///////////////////////////////////////////////////////////////////////////////
//
// FileName  :  log.h
// Creator   :  Jungle
// Date      :  2007-12-17
// Comment   :  logging something to files or log server.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef	_LOG_MODULE_H__
#define	_LOG_MODULE_H__

#include <stdio.h>
#include <stdlib.h>
#include "../KType.h"
#include "../Sync/KSync.h"
#include "../KPlatform.h"

class ILogger;
class LoggerWraper;
LoggerWraper& GetGlobalLogger();
void setGlobalLogger(ILogger* logger);

#define	Log GetGlobalLogger().WriteLog
void _log_(int level, const char* fmt, ...);
int _getlogstr_(char * szBuf, int nSize, const char* fmt, ...);

class ILogger
{
public:
	ILogger();
	virtual ~ILogger();

	virtual void LogMessage(const char* fmt, va_list args) = 0;
	virtual void LogMessage(int level, const char* fmt, va_list args);
	virtual void ForceFlushAllLog() {}

	BOOL IsAllow(int level);
	void SetLogLevel(int levels);
	void SetFlushLines(int lines);

	void WriteLog(const char* fmt, ...);
	void WriteLog(int level, const char* fmt, ...);
	void vLog(int level, const char* fmt, va_list args);

protected:
	int m_levels;
	int m_lines;
	int m_flushLines;
	int m_totalLines;
};

class FileLogger : public ILogger
{
public:
	FileLogger();
	virtual ~FileLogger() { }
	void LogMessage(const char* fmt, va_list args);
	virtual void ForceFlushAllLog();

protected:
	virtual FILE* GetFile() = 0;

protected:
	::System::Sync::KThreadMutex m_mutex;
	int m_bufferLen;
	char* m_buffer;
};

class LoggerWraper
{
public:
	LoggerWraper() : m_pLoggerInterface(NULL) { }
	LoggerWraper(ILogger* pLogger):m_pLoggerInterface(pLogger) { }

	virtual void SetLogger(ILogger* pLogger);
	ILogger* GetLogger();
	BOOL IsAllow(int level) const;
	void SetLogLevel(int levels);
	void SetFlushLines(int lines);
	void WriteLog(int level, const char* fmt, ...);
	void ForceFlushAllLog();

public:
	ILogger* m_pLoggerInterface;
};

class DirectFileLogger : public FileLogger
{
public:
	DirectFileLogger();
	DirectFileLogger(const char* filename, const char* mode = "a+");
	~DirectFileLogger();
	BOOL Open(const char* fileName);
	FILE* GetFile();

protected:
	FILE* m_file;
};

class DirectConsoleLogger : public FileLogger
{
public:
	DirectConsoleLogger();
	~DirectConsoleLogger();
	FILE* GetFile();
	void LogMessage(int level, const char* fmt, va_list args);
};

#endif