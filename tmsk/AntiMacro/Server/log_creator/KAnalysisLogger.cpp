#include "KAnalysisLogger.h"
#include <System/Misc/KDatetime.h>
#include <System/Misc/StrUtil.h>
#include <System/File/KFile.h>

typedef JG_C::KString<512> string_512;
static const DWORD max_logfile_size = 1<<30;

KAnalysisLogger::KAnalysisLogger()
{

}

KAnalysisLogger::KAnalysisLogger(const char* fileName) : AsyncDailyFileLogger()
{
	AsyncDailyFileLogger::Open(fileName);
}

KAnalysisLogger::~KAnalysisLogger()
{

}

BOOL KAnalysisLogger::_RealWrite(LogRequestHead* pLogRequest)
{
	const int max_buffer = 10240;
	FILE* fo = this->GetFile();
	if(!fo) return FALSE;

	char buf[max_buffer];

	KDatetime reqTime(pLogRequest->when);
	JG_C::KString<32> dstr = reqTime.ToString(); dstr.append(": ", 2);

	int i = 0;
	char* pc = &buf[0];

	memcpy(pc+i, dstr.c_str(), dstr.size()); i+= dstr.size();

	int len = currentToLocal((char*)(pLogRequest+1), pc+i, sizeof(buf)-i);
	if(len < 1) return FALSE; i += len;
	buf[i++] = '\n'; buf[i] = '\0';

	fwrite(buf, 1, i, fo);
	
	return TRUE;
}

FILE* KAnalysisLogger::GetFile()
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

		while(TRUE)
		{
			::System::Collections::KString<512> fileName = this->GetFileName(now, 0);
			m_fo = ::System::File::KFileUtil::OpenFileForce(fileName.c_str(), "a+b");
			break;
		}
	}
	return m_fo;
}

string_512 KAnalysisLogger::GetFileName(TIME_T when, int idx)
{
	char cTmp[64];
	KDatetime dt(when);
	KLocalDatetime ldt = dt.GetLocalTime();
	string_512 path(m_namePrefix);

	if(idx == 0)
	{
		sprintf_k(cTmp, sizeof(cTmp), "_%04d%02d%02d", ldt.year, ldt.month, ldt.day);
	}
	else
	{
		sprintf_k(cTmp, sizeof(cTmp), "_%04d%02d%02d_%d", ldt.year, ldt.month, ldt.day, idx);
	}

	path.append(cTmp).append(m_nameSuffix);
	return path;
}
