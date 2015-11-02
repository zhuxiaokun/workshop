#pragma once
#include <System/Log/AsyncLogger.h>

class KAnalysisLogger : public AsyncDailyFileLogger
{
public:
	KAnalysisLogger();
	KAnalysisLogger(const char* fileName);
	~KAnalysisLogger();

protected:
	BOOL _RealWrite(LogRequestHead* pLogRequest);
	FILE* GetFile();

private:
	::System::Collections::KString<512> GetFileName(TIME_T when, int idx);
};
