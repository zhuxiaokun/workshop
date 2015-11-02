#include "AsyncLogger.h"
#include "../Misc/KDatetime.h"
#include "../File/KFile.h"
#include "../Misc/StrUtil.h"

static const DWORD max_logfile_size = 1<<30;

IAsyncLogger::IAsyncLogger():m_stopTick(0)
{
	m_reqCount = 0;
	m_writeCount = 0;
	m_stopDesired = FALSE;
	ASSERT(m_ringBuffer.SetCapacity(RING_BUFFER_SIZE));
	KThread::Run();
}

IAsyncLogger::~IAsyncLogger()
{
	this->Stop();
}

void IAsyncLogger::LogMessage(const char* fmt, va_list args)
{
	JG_S::KAutoThreadMutex mx(m_mx);

	LogRequestHead* pReq = (LogRequestHead*)&m_inputBuffer;
	int maxmsglen = sizeof(m_inputBuffer) - sizeof(LogRequestHead) - 1;
	int len = _vsnprintf_k((char*)(pReq+1), maxmsglen, fmt, args);
	if(len < 0) return;

	pReq->threadID = GetCurrentThreadId();
	pReq->when = (TIME_T)time(NULL);
	pReq->length = len + 1;
	((char*)(pReq+1))[len] = '\0';

	len = sizeof(LogRequestHead) + len + 1;
	if(m_ringBuffer.WriteAvailable() >= len)
	{
		m_ringBuffer.Write(m_inputBuffer, len);
		m_reqCount += 1;
		m_event.Set();
	}
}

void IAsyncLogger::LogMessage(int level, const char* fmt, va_list args)
{
	// char buf[1024];
	if(m_levels & level)
	{
		//if(level == LOG_FATAL)
		//{	// fatal日志直接写到控制台上面
		//	KDatetime now;
		//	sprintf_k(buf, sizeof(buf), "%s [%u] %s\n", now.ToString().c_str(), GetCurrentThreadId(), fmt);
		//	vprintf(buf, args);
		//}
		// 还要写到日志文件中
		this->LogMessage(fmt, args);
	}
}

void IAsyncLogger::Execute()
{
	//while(!m_stopDesired)
	while(TRUE)
	{
		m_mx.Lock();
		if(m_ringBuffer.ReadAvailable() < sizeof(LogRequestHead))
		{
			m_mx.Unlock();
			if(m_lines) { this->_Flush(); m_lines = 0; }
			if(m_stopDesired) break;
			m_event.Block(100);
			continue;
		}

		if(m_stopDesired)
		{
			// 最多等2秒写日志时间
			DWORD nTick = GetTickCount();
			if(nTick - m_stopTick > 2000)
			{
				m_mx.Unlock();
				break;
			}
		}
		
		m_ringBuffer.Read(sizeof(LogRequestHead), m_outputBuffer);
		LogRequestHead* pReq = (LogRequestHead*)&m_outputBuffer;
		m_ringBuffer.Read(pReq->length, (char*)(pReq+1));
		m_mx.Unlock();

		this->_RealWrite(pReq);
		m_writeCount += 1;
		m_lines += 1;

		if(m_lines >= m_flushLines)
		{
			this->_Flush();
			m_lines = 0;
		}
	}
	
	this->_Flush();
}

void IAsyncLogger::ForceFlushAllLog()
{
	m_mx.Lock();
	while(TRUE)
	{
		if(m_ringBuffer.ReadAvailable() < sizeof(LogRequestHead))
		{
			break;
		}

		m_ringBuffer.Read(sizeof(LogRequestHead), m_outputBuffer);
		LogRequestHead* pReq = (LogRequestHead*)&m_outputBuffer;
		m_ringBuffer.Read(pReq->length, (char*)(pReq+1));

		this->_RealWrite(pReq);
		m_writeCount += 1;
		m_lines += 1;
	}

	this->_Flush();
	m_lines = 0;
	m_mx.Unlock();
}

void IAsyncLogger::Stop()
{
	m_stopDesired = TRUE;
	if(!m_stopTick) m_stopTick = GetTickCount();
	KThread::WaitFor();
}

void IAsyncLogger::_Flush()
{

}

BOOL IAsyncFileLogger::_RealWrite(IAsyncLogger::LogRequestHead* pLogRequest)
{
	const int max_buffer = 10240;
	FILE* fo = this->GetFile();
	if(!fo) return FALSE;

	char buf[max_buffer];
	KDatetime reqTime(pLogRequest->when);
	if(currentToLocal((char*)(pLogRequest+1), buf, sizeof(buf)) < 0)
		return FALSE;
	
	fprintf(fo, "%s [%u] %s\n", reqTime.ToString().c_str(), pLogRequest->threadID, buf);
	return TRUE;
}

FILE* AsyncConsoleLogger::GetFile()
{
	return stdout;
}

BOOL AsyncConsoleLogger::_OnBufferOut(const char* fmt, va_list args)
{
	//m_outLogger.LogMessage(fmt, args);
	return TRUE;
}

AsyncFileLogger::AsyncFileLogger()
{
	m_fo = NULL;
	m_fileSize = 0;
}

AsyncFileLogger::AsyncFileLogger(const char* fileName)
{
	m_fo = NULL;
	this->Open(fileName);
}

AsyncFileLogger::AsyncFileLogger(const char* fileName, const char * pchOpenMode)
{
	m_fo = NULL;
	this->Open(fileName, pchOpenMode);
}
AsyncFileLogger::AsyncFileLogger(const char* fileName, bool bCreate)
{
	m_fo = NULL;
	Open( fileName, bCreate );
}

AsyncFileLogger::~AsyncFileLogger()
{
	this->Stop();
	if(m_fo)
	{
		fflush(m_fo);
		fclose(m_fo);
		m_fo = NULL;
	}
}

BOOL AsyncFileLogger::Open(const char* fileName)
{
	if(m_fo) fclose(m_fo);
	m_fo = ::System::File::KFileUtil::OpenFileForce(fileName, "a+");
	if(!m_fo) return FALSE;
	
	m_fileSize = ftell(m_fo);
	return TRUE;
}

BOOL	AsyncFileLogger::Open(const char* fileName, const char * pchOpenMode)
{
	if(m_fo) fclose(m_fo);
	m_fo = ::System::File::KFileUtil::OpenFileForce(fileName, pchOpenMode);
	if(!m_fo) return FALSE;

	m_fileSize = ftell(m_fo);
	return TRUE;
}

BOOL	AsyncFileLogger::Open(const char * fileName, bool bCreate)
{
	char * pchOpenMode = "a+";
	if( bCreate )
	{
		pchOpenMode = "w+";
	}
	return Open( fileName, pchOpenMode );
}

FILE* AsyncFileLogger::GetFile()
{
	return m_fo;
}

BOOL AsyncFileLogger::_RealWrite(LogRequestHead* pLogRequest)
{
	// 文件最多打到1G
	if(m_fileSize >= max_logfile_size) return FALSE;
	IAsyncFileLogger::_RealWrite(pLogRequest);
	m_fileSize += pLogRequest->length;
	return TRUE;
}

BOOL AsyncFileLogger::_OnBufferOut(const char* fmt, va_list args)
{
	//m_outLogger.LogMessage(fmt, args);
	return TRUE;
}

void AsyncFileLogger::_Flush()
{
	if(m_fo) fflush(m_fo);
}

AsyncDailyFileLogger::AsyncDailyFileLogger()
{
	m_fo = NULL;
	m_serveEnd = 0;
	m_fileSize = 0;
}

AsyncDailyFileLogger::AsyncDailyFileLogger(const char* fileName)
{
	m_fo = NULL;
	m_fileSize = 0;
	m_serveEnd = 0;
	this->Open(fileName);
}

AsyncDailyFileLogger::~AsyncDailyFileLogger()
{
	this->Stop();
	if(m_fo)
	{
		fflush(m_fo);
		fclose(m_fo);
		m_fo = NULL;
	}
}

BOOL AsyncDailyFileLogger::Open(const char* fileName)
{
	this->ParseFileName(fileName);
	m_fo = this->GetFile();
	//if(m_fo) m_outLogger.Open(fileName);
	return m_fo != NULL;
}

void AsyncDailyFileLogger::AutoClear()
{
	// 超过n天前的日志将被删除
	const int keepDays = 3;

	// 当前日期
	KDatetime currDate;
	currDate.GetLocalTime();
	int currDays = currDate.ToLocalDays();
	// 日志文件日期
	KLocalDatetime lt;
	memset(&lt, 0, sizeof(lt));

	char workdir[260], filename[260];
	GetCurrentDirectory_k(sizeof(workdir), workdir);
	sprintf_k(filename, sizeof(filename), "%s\\*.log", workdir);
#if defined(WIN32) || defined(WINDOWS)
	const char *p;
	for (p = m_namePrefix.c_str(); ;p++)
	{
		if(*p != '.' && *p != '/' && *p != '\\')
			break;
	}

	WIN32_FIND_DATA fd;
	HANDLE hfind = FindFirstFile(filename, &fd);
	if(!hfind) return;
	for (;;)
	{
		char* q = strstr(fd.cFileName, p);
		if(q)
		{
			long dat = atol(q+strlen(p)+1);
			lt.year = dat / 10000;
			lt.month = dat % 10000 / 100;
			lt.day = dat  % 100;
			KDatetime ft(lt);
			int fileDays = ft.ToLocalDays();
			if(currDays - fileDays >= keepDays)
			{
				sprintf_k(filename, sizeof(filename), "%s\\%s", workdir, fd.cFileName);
				DeleteFile(filename);
			}
		}

		// 查找下一个日志
		if(!FindNextFile(hfind, &fd))
		{
			FindClose(hfind);
			break;
		}
	}
#endif
}

FILE* AsyncDailyFileLogger::GetFile()
{
	TIME_T now = (TIME_T)time(NULL);

	if(now >= m_serveEnd)
	{
		if(m_fo)
		{
			fflush(m_fo);
			fclose(m_fo);
			m_fo = NULL;
			m_fileSize = 0;
		}

		KDatetime dt(now);
		KLocalDatetime ldt = dt.GetLocalTime();
		ldt.hour = 0;
		ldt.minute = 0;
		ldt.second = 0;
		dt.SetLocalTime(ldt);
		dt.AddDay(1);
		m_serveEnd = (TIME_T)dt.Time();

		int i = 0;
		while(TRUE)
		{
			::System::Collections::KString<512> fileName = this->GetFileName(now, i);
			m_fo = ::System::File::KFileUtil::OpenFileForce(fileName.c_str(), "a+");
			if(!m_fo) break;

			fseek(m_fo, 0, SEEK_END);
			DWORD fileSize = ftell(m_fo);
			if(fileSize >= max_logfile_size)
			{
				fclose(m_fo);
				m_fo = NULL;
			}
			else
			{
				m_fileSize = fileSize;
				break;
			}

			i += 1;
		}
	}
	else if(m_fileSize >= max_logfile_size)
	{	//如果日志文件已经大于1G，再开一个文件
		if(m_fo)
		{
			fflush(m_fo);
			fclose(m_fo);
			m_fo = NULL;
		}

		int i = 1;
		m_fileSize = 0;

		while(TRUE)
		{
			::System::Collections::KString<512> fileName = this->GetFileName(now, i);
			if(::System::File::KFileUtil::IsFileExists(fileName.c_str()))
			{
				i += 1;
				continue;
			}

			m_fo = ::System::File::KFileUtil::OpenFileForce(fileName.c_str(), "a+");
			break;
		}
	}

	return m_fo;
}

BOOL AsyncDailyFileLogger::_RealWrite(LogRequestHead* pLogRequest)
{
	IAsyncFileLogger::_RealWrite(pLogRequest);
	if(m_fo) m_fileSize = ftell(m_fo);
	return TRUE;
}

BOOL AsyncDailyFileLogger::_OnBufferOut(const char* fmt, va_list args)
{
	//m_outLogger.LogMessage(fmt, args);
	return TRUE;
}

void AsyncDailyFileLogger::_Flush()
{
	if(m_fo) fflush(m_fo);
}

BOOL AsyncDailyFileLogger::ParseFileName(const char* fileName)
{
	const char* pDot = strrchr(fileName, '.');
	if(pDot)
	{
		m_nameSuffix = pDot;
		if(m_nameSuffix.size() > 4)
		{
			m_nameSuffix = ".log";
			m_namePrefix = fileName;
		}
		else
		{
			m_namePrefix.assign(fileName, (int)(pDot-fileName));
		}
	}
	else
	{
		m_nameSuffix = ".log";
		m_namePrefix = fileName;
	}
	return TRUE;
}

::System::Collections::KString<512> AsyncDailyFileLogger::GetFileName(TIME_T when)
{
	return this->GetFileName(when, 0);
}

::System::Collections::KString<512> AsyncDailyFileLogger::GetFileName(TIME_T when, int idx)
{
	char cTmp[64];
	KDatetime dt(when);
	KLocalDatetime ldt = dt.GetLocalTime();
	::System::Collections::KString<512> path(m_namePrefix);

	if(idx == 0)
	{
		sprintf_k(cTmp, sizeof(cTmp), "-%04d%02d%02d", ldt.year, ldt.month, ldt.day);
	}
	else
	{
		sprintf_k(cTmp, sizeof(cTmp), "-%04d%02d%02d-%d", ldt.year, ldt.month, ldt.day, idx);
	}

	path.append(cTmp).append(m_nameSuffix);
	return path;
}