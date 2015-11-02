#include "log.h"
#include "../File/KFile.h"
#include "../Misc/KDatetime.h"
#include "../Misc/StrUtil.h"
#include "../Memory/multi_module.h"

static DirectConsoleLogger g_defaultLogger;
static LoggerWraper g_loggerWraper(&g_defaultLogger);

#if defined(__cplusplus)
extern "C"
{
#endif
_EXPORT_K_ void _set_global_logger_(ILogger* logger)
{
	g_loggerWraper.SetLogger(logger);
}
#if defined(__cplusplus)
}
#endif

void setGlobalLogger(ILogger* logger)
{
	foreach_module("_set_global_logger_", _set_global_logger_, logger);
}

static DirectConsoleLogger& _consoleLogger()
{
	return g_defaultLogger;
}

LoggerWraper& GetGlobalLogger()
{	
	return g_loggerWraper;
}

void _log_(int level, const char* fmt, ...)
{
	ILogger* pLogger = GetGlobalLogger().GetLogger();
	if(!pLogger) return;

	va_list args;
	va_start(args, fmt);
	pLogger->vLog(level, fmt, args);
	va_end(args);
}

int _getlogstr_(char * szBuf, int nSize, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char fmt_2[1024] = {0};
	if(localToCurrent(fmt, fmt_2, sizeof(fmt_2)) < 0) return 0;
	fmt = fmt_2;
	int nRealSize = _vsnprintf_k(szBuf, nSize, fmt, args);
	va_end(args);
	return nRealSize;
}

ILogger::ILogger():m_levels(-1),m_lines(0),m_flushLines(1),m_totalLines(0)
{
}

ILogger::~ILogger()
{
}

void ILogger::LogMessage(int level, const char* fmt, va_list args)
{
	if(m_levels & level)
	{
		this->LogMessage(fmt, args);
	}
}

BOOL ILogger::IsAllow(int level)
{
	return m_levels & level;
}

void ILogger::SetLogLevel(int levels)
{
	m_levels = levels & (~LOG_CONSOLE);
}

void ILogger::SetFlushLines(int lines)
{
	m_flushLines = lines;
}

void ILogger::WriteLog(const char* fmt, ...)
{
	char fmt_2[1024];
	if(localToCurrent(fmt, fmt_2, sizeof(fmt_2)) < 0) return;
	fmt = fmt_2;

	va_list args;
	va_start(args, fmt);
	this->vLog(-1, fmt, args);
	va_end(args);
}

void ILogger::WriteLog(int level, const char* fmt, ...)
{
	char fmt_2[1024];
	if(localToCurrent(fmt, fmt_2, sizeof(fmt_2)) < 0) return;
	fmt = fmt_2;

	va_list args;
	va_start(args, fmt);
	this->vLog(level, fmt, args);
	va_end(args);
}

void ILogger::vLog(int level, const char* fmt, va_list args)
{
	this->LogMessage(level, fmt, args);
}

FileLogger::FileLogger():m_bufferLen(0),m_buffer(NULL)
{

}

void FileLogger::LogMessage(const char* fmt, va_list args)
{
	const int max_buffer = 10*1024;
	char buf[max_buffer];
	if(!this->GetFile()) return;

	JG_S::KAutoThreadMutex mx(m_mutex);

	m_lines += 1;
	m_totalLines += 1;
	int buffer_length = max_buffer;

	if(!m_buffer)
	{
		m_bufferLen = buffer_length;
		m_buffer = (char*)malloc(m_bufferLen);
	}	char* s = m_buffer;

	KDatetime now;
	int length = sprintf_k(s, buffer_length, "%s [%u] ", now.ToString().c_str(), GetCurrentThreadId());
	if(length < 0)
	{
		if(m_lines >= m_flushLines)
		{
			fflush(this->GetFile());
			m_lines = 0;
		}	return;
	}

	s += length; buffer_length -= length;
	length = _vsnprintf_k(s, buffer_length, fmt, args);
	if(length < 0)
	{
		if(m_lines >= m_flushLines)
		{
			fflush(this->GetFile());
			m_lines = 0;
		}	return;
	}

	s += length; *s++ = '\n'; *s++ = '\0';
	length = currentToLocal(m_buffer, buf, max_buffer);
	if(length < 0)
	{
		if(m_lines >= m_flushLines)
		{
			fflush(this->GetFile());
			m_lines = 0;
		}	return;
	}

	fwrite(buf, 1, length, this->GetFile());
	if(m_lines >= m_flushLines)
	{
		fflush(this->GetFile());
		m_lines = 0;
	}
}

void FileLogger::ForceFlushAllLog()
{
	if(!this->GetFile()) return;

	JG_S::KAutoThreadMutex mx(m_mutex);
	fflush(this->GetFile());
}

void LoggerWraper::SetLogger(ILogger* pLogger)
{
	m_pLoggerInterface = pLogger;
}

ILogger* LoggerWraper::GetLogger()
{
	return m_pLoggerInterface;
}

BOOL LoggerWraper::IsAllow(int level) const
{
	if(!m_pLoggerInterface) return FALSE;
	return m_pLoggerInterface->IsAllow(level);
}

void LoggerWraper::SetLogLevel(int levels)
{
	if(m_pLoggerInterface) m_pLoggerInterface->SetLogLevel(levels);
}

void LoggerWraper::SetFlushLines(int lines)
{
	if(m_pLoggerInterface) m_pLoggerInterface->SetFlushLines(lines);
}

void LoggerWraper::WriteLog(int level, const char* fmt, ...)
{
	if(level == LOG_FATAL)
		level |= LOG_CONSOLE;

	char fmt2[1024];
	if(localToCurrent(fmt, fmt2, sizeof(fmt2)) < 0) return;
	fmt = fmt2;

	if(m_pLoggerInterface && level != LOG_CONSOLE)
	{
		va_list va;
		va_start(va, fmt);
		m_pLoggerInterface->vLog(level, fmt, va);
		va_end(va);
	}
	if(level & LOG_CONSOLE)
	{
		char buf[10240];
		va_list va;
		va_start(va, fmt);
		int len = _vsnprintf_k(buf, sizeof(buf), fmt, va);
		va_end(va);
		if(len > 0 && len < sizeof(buf)-2)
		{
			char buf2[10240];
			buf[len++] = '\n'; buf[len++] = '\0';
			if(currentToLocal(buf, buf2, sizeof(buf2)) >0)
				printf(buf2);
		}
	}
}

void LoggerWraper::ForceFlushAllLog()
{
	if(m_pLoggerInterface) m_pLoggerInterface->ForceFlushAllLog();
}

DirectFileLogger::DirectFileLogger() : m_file(NULL)
{
}

DirectFileLogger::DirectFileLogger(const char* filename, const char* mode)
{
	m_file = ::System::File::KFileUtil::OpenFileForce(filename, mode);
}

DirectFileLogger::~DirectFileLogger()
{
	if(m_file)
	{
		fclose(m_file);
		m_file = NULL;
	}
}

BOOL DirectFileLogger::Open(const char* fileName)
{
	m_file = ::System::File::KFileUtil::OpenFileForce(fileName, "a+");
	return m_file != NULL;
}

FILE* DirectFileLogger::GetFile()
{
	return m_file;
}

DirectConsoleLogger::DirectConsoleLogger()
{

}

DirectConsoleLogger::~DirectConsoleLogger()
{

}

FILE* DirectConsoleLogger::GetFile()
{
	return stdout;
}

void DirectConsoleLogger::LogMessage(int level, const char* fmt, va_list args)
{
	if(level & LOG_CONSOLE) return;
	if(level & m_levels)
		FileLogger::LogMessage(fmt, args);
}