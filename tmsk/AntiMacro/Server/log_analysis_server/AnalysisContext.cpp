#include "AnalysisContext.h"
#include <System/File/KFile.h>
#include "Application.h"
#include "LogFileReader.h"
#include "line_processor.h"

AnalysisContext::AnalysisContext()
{

}

AnalysisContext::~AnalysisContext()
{
	this->finalize();
}

bool AnalysisContext::initialize(const char* dirName, const char* platformName, const char* fileName, KDatetime& ftime, KDatetime& ttime)
{
	m_dataDir = dirName;
	m_platformName = platformName;

	char buf[64]; sprintf_k(buf,sizeof(buf), fileName);
	char* dot = strrchr(buf, '.');
	if(dot)
	{
		m_fileExtName = dot; *dot = '\0';
		m_baseFileName = buf;
	}
	else
	{
		m_baseFileName = buf;
		m_fileExtName = "";
	}

	m_fromTime = ftime;
	m_toTime = ttime;

	if(ttime <= ftime)
	{
		Log(LOG_ERROR, "error: from-time %s must less than to-time %s", ftime.ToString().c_str(), ttime.ToString().c_str());
		return false;
	}

	if(!this->_scanServerList())
		return false;

	return true;
}

bool AnalysisContext::process()
{
	int fhour, thour;
	int fyyyymmdd,tyyyymmdd;

	KLocalDatetime fldt = m_fromTime.GetLocalTime();
	fyyyymmdd = fldt.year*10000 + fldt.month*100 + fldt.day;
	fhour = fldt.hour;

	KLocalDatetime tldt = m_toTime.GetLocalTime();
	tyyyymmdd = tldt.year*10000 + tldt.month*100 + tldt.day;
	thour = tldt.hour;

	KPropertyMap lmap;
	LogFileReader fileReader;
	bool serverDone = false;

	int fileCount = 0;

	KLargeNameList::iterator it = m_serverList.begin();
	KLargeNameList::iterator ite = m_serverList.end();
	for(; it != ite; it++)
	{
		const char* serverName = *it;
		for(int i=fyyyymmdd; i<=tyyyymmdd; i++)
		{
			serverDone = false;

			if(i == tyyyymmdd && !tldt.hour && !tldt.minute && !tldt.second)
				break;

			string_512 logFilePath = this->getLogFilePath(serverName, i);
			string_512 idxFilePath = this->getIndexFilePath(serverName, i);

			// ºöÂÔ
			if(logFilePath.empty())
				continue;

			int hour = i == fyyyymmdd ? fldt.hour : 0;

			fileReader.close();
			if(!fileReader.open(logFilePath.c_str(), idxFilePath.c_str(), hour))
			{
				fileReader.close();
				continue;
			}

			fileCount++;
			DWORD startTime = GetTickCount();
			
			int lineNo = 0;
			char* line = NULL;
			while(1)
			{
				int nRet = fileReader.readLine(&line);
				if(nRet < 0)
					break;
				
				lineNo++;
				line[nRet] = '\0';

				time_t lineTime; const char* verb;
				if(!parseLogLine(line, lmap, &lineTime, &verb)) continue;
				if(!this->acceptTime(lineTime))
				{
					if(this->isTimeExceed(lineTime))
					{
						serverDone = true;
						break;
					}
					else
						continue;
				}
				process_log_line(lineTime, verb, lmap);
			}

			float secs = (GetTickCount() - startTime)/1000.0f;
			Log(LOG_CONSOLE|LOG_DEBUG, "no.%d lines:%d secs:%.2f, %s", fileCount, lineNo, secs, logFilePath.c_str());

			if(serverDone)
				break;
		}
	}

	return true;
}

void AnalysisContext::finalize()
{
	m_dataDir.clear();
	m_platformName.clear();
	m_baseFileName.clear();
	m_fileExtName.clear();
	m_serverList.reset();
}

string_512 AnalysisContext::getLogFilePath(const char* server, int yyyymmdd)
{
	char buf[512];
	if(g_pApp->m_useLuaDirFunc)
	{
		const char* dirName;
		if(g_pApp->m_cfgTable->retTableCall<ccstr,ccstr,ccstr>(dirName, "getLogDir", m_dataDir.c_str(), m_platformName.c_str(), server))
		{
			if(!dirName) return string_512();
			int n = sprintf_k(buf, sizeof(buf), "%s\\%s_%d%s",
				dirName, m_baseFileName.c_str(), yyyymmdd, m_fileExtName.c_str());
			return string_512(buf, n);
		}
	}
	{
		int n = sprintf_k(buf, sizeof(buf), "%s\\%s\\%s_%d%s",
			m_dataDir.c_str(), server, m_baseFileName.c_str(), yyyymmdd, m_fileExtName.c_str());
		return string_512(buf, n);
	}
}

string_512 AnalysisContext::getIndexFilePath(const char* server, int yyyymmdd)
{
	char buf[512];
	if(g_pApp->m_useLuaDirFunc)
	{
		const char* dirName;
		if(g_pApp->m_cfgTable->retTableCall<ccstr,ccstr,ccstr>(dirName, "getLogDir", m_dataDir.c_str(), m_platformName.c_str(), server))
		{
			if(!dirName) return string_512();
			int n = sprintf_k(buf, sizeof(buf), "%s\\%s_%d%s",
				dirName, m_baseFileName.c_str(), yyyymmdd, ".idx");
			return string_512(buf, n);
		}
	}
	{
		int n = sprintf_k(buf, sizeof(buf), "%s\\%s\\%s_%d%s",
			m_dataDir.c_str(), server, m_baseFileName.c_str(), yyyymmdd, ".idx");
		return string_512(buf, n);
	}
}

bool AnalysisContext::acceptTime(time_t lineTime) const
{
	return lineTime >= m_fromTime.Time() && lineTime < m_toTime.Time();
}

bool AnalysisContext::isTimeExceed(time_t lineTime) const
{
	return lineTime >= m_toTime.Time();
}

class TheFilter : public JG_F::KFileFilter
{
public:
	KLargeNameList& m_serverList;
	TheFilter(KLargeNameList& lst):m_serverList(lst)
	{

	}
	BOOL Accept(const JG_F::KFileData& fileData)
	{
		const char* shortName = fileData.m_shortName;
		char ch0 = shortName[0];
		char ch1 = shortName[1];
		char ch2 = shortName[2];
		if(ch0 == '.' && ch1 == '\0') return false;
		if(ch0 == '.' && ch1 == '.' && ch2 == '\0') return false;
		int len = strlen("server");
		if(strnicmp(shortName, "server", len) == 0)
		{
			if(fileData._isDir())
			{
				if(isnumber3(shortName + len))
				{
					m_serverList.append(g_stringPool.get(shortName));
				}
			}	
		}
		return TRUE;
	}
};


bool AnalysisContext::_scanServerList()
{
	m_serverList.reset();

	const char* logDir = m_dataDir.c_str();
	JG_F::KDirectory dir;
	dir.Open(logDir);

	TheFilter filter(m_serverList);
	JG_F::KDirectory::FileList fileList;

	if(!dir.ListFile(fileList, &filter))
	{
		return false;
	}

	return true;
}
