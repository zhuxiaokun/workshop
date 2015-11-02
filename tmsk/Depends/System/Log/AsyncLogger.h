#ifndef _ASYNC_LOGGER_H_
#define _ASYNC_LOGGER_H_

#include "log.h"
#include "../Memory/RingBuffer.h"
#include "../Sync/KSync.h"
#include "../Thread/KThread.h"
#include "../Collections/KString.h"

class IAsyncLogger : public ::System::Thread::KThread, public ILogger
{
public:
	enum
	{
		RING_BUFFER_SIZE = 2*1024*1024,
		REQUEST_BUFFER_SIZE = 1024*10,
	};
	#pragma pack(push,1)
	struct LogRequestHead
	{
		DWORD threadID;
		TIME_T when;
		UINT16 length;
	};
	#pragma pack(pop)

public:
	IAsyncLogger();
	virtual ~IAsyncLogger();
	void LogMessage(const char* fmt, va_list args);
	void LogMessage(int level, const char* fmt, va_list args);
	virtual void ForceFlushAllLog();

public:
	void Execute();

protected:
	void Stop();
	// 由派生类实现
	virtual BOOL _RealWrite(LogRequestHead* pLogRequest) = 0;
	// 当RingBuffer没有可用空间时，通知派生类, 这是因为写日志的请求太多以至于处理不过来造成的
	virtual BOOL _OnBufferOut(const char* fmt, va_list args) = 0;
	virtual void _Flush();

private:
	INT64 m_reqCount;
	INT64 m_writeCount;
	::System::Sync::KThreadMutex m_mx;
	::System::Sync::KSync_PulseEvent m_event;
	RingBuffer<char> m_ringBuffer;
	char m_inputBuffer[REQUEST_BUFFER_SIZE];
	char m_outputBuffer[REQUEST_BUFFER_SIZE];
	volatile BOOL m_stopTick;
	volatile BOOL m_stopDesired;
};

class IAsyncFileLogger : public IAsyncLogger
{
protected:
	virtual FILE* GetFile() = 0;
	BOOL _RealWrite(IAsyncLogger::LogRequestHead* pLogRequest);
};

class AsyncConsoleLogger : public IAsyncFileLogger
{
protected:
	FILE* GetFile();
	BOOL _OnBufferOut(const char* fmt, va_list args);

private:
	::System::Sync::KThreadMutex m_mx;
	//DirectConsoleLogger m_outLogger;
};

class AsyncFileLogger : public IAsyncFileLogger
{
public:
	AsyncFileLogger();
	AsyncFileLogger(const char* fileName);
	AsyncFileLogger(const char* fileName, const char * pchOpenMode);
	AsyncFileLogger(const char* fileName, bool bCreate);
	~AsyncFileLogger();

public:
	BOOL Open(const char* fileName);
	BOOL Open(const char * fileName, const char * pchOpenMode);
	BOOL Open(const char * fileName, bool bCreate);

protected:
	FILE* GetFile();
	BOOL _RealWrite(LogRequestHead* pLogRequest);
	BOOL _OnBufferOut(const char* fmt, va_list args);
	void _Flush();

private:
	FILE* m_fo;
	DWORD m_fileSize;
};

class AsyncDailyFileLogger : public IAsyncFileLogger
{
public:
	AsyncDailyFileLogger();
	AsyncDailyFileLogger(const char* fileName);
	~AsyncDailyFileLogger();

public:
	BOOL Open(const char* fileName);
	void AutoClear();

protected:
	FILE* GetFile();
	BOOL _RealWrite(LogRequestHead* pLogRequest);
	BOOL _OnBufferOut(const char* fmt, va_list args);
	void _Flush();

protected:
	BOOL ParseFileName(const char* fileName);
	::System::Collections::KString<512> GetFileName(TIME_T when);
	::System::Collections::KString<512> GetFileName(TIME_T when, int idx);

protected:
	FILE* m_fo;
	DWORD m_fileSize;
	TIME_T m_serveEnd;
	::System::Collections::KString<512> m_namePrefix;
	::System::Collections::KString<32> m_nameSuffix;
	//DirectFileLogger m_outLogger;
};

#endif
