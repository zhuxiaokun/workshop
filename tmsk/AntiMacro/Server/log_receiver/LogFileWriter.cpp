#include "LogFileWriter.h"
#include <System/Misc/KDatetime.h>
#include "Application.h"

// OutputFileMap m_fileMap;
// FileKey* m_currKey;
// OutputLogFile* m_currFile;

LogFileWriter::LogFileWriter():m_currKey(NULL),m_currFile(NULL)
{

}

LogFileWriter::~LogFileWriter()
{

}

bool LogFileWriter::on_logLine(KPropertyMap& lmap)
{
	char buf[64]; char buf2[64];
	const char* tstr = lmap.get("time"); strcpy(buf, tstr); tstr = buf; lmap.erase("time");
	const char* verb = lmap.get("verb"); strcpy(buf2, verb); verb = buf2; lmap.erase("verb");
	
	time_t t = (time_t)str2int(tstr);
	KDatetime dt(t);

	KLocalDatetime ldt = dt.GetLocalTime();
	
	int yyyy = ldt.year, mm = ldt.month, dd = ldt.day;

	const char* serverName = g_pApp->m_strPool.get(lmap.get("server"));
	int yyyymmdd = yyyy*10000 + mm*100 + dd;

	FileKey fk(serverName, yyyymmdd);
	if(m_currKey)
	{
		if(!FileKey::cmp::compare(*m_currKey, fk))
		{
			return m_currFile->writeLine(dt, ldt, verb, lmap);
		}
		else
		{
			m_currFile->flush();
		}
	}

	OutputFileMap::iterator it = m_fileMap.find(fk);
	if(it != m_fileMap.end())
	{
		m_currKey = &it->first;
		m_currFile = it->second;
		return m_currFile->writeLine(dt, ldt, verb, lmap);
	}

	string_512 filePath = this->buildFilepath(serverName, yyyymmdd, verb);
	OutputLogFile* file = OutputLogFile::Alloc();
	if(!file->open(filePath.c_str()))
	{
		Log(LOG_ERROR, "error: log file writer open %s", filePath.c_str());
		return false;
	}

	it = m_fileMap.insert(fk, file);
	m_currKey = &it->first;
	m_currFile = it->second;
	return m_currFile->writeLine(dt, ldt, verb, lmap);
}

void LogFileWriter::finalize()
{
	while(!m_fileMap.empty())
	{
		OutputFileMap::iterator it = m_fileMap.begin();
		OutputLogFile* file = it->second;
		m_fileMap.erase(it);
		file->close();
		OutputLogFile::Free(file);
	}
}

string_512 LogFileWriter::buildFilepath(const char* serverName, int yyyymmdd, const char* verb)
{
	char buf[512];
	
	const char* dirName = g_pApp->getDataDir();
	const char* platformName = g_pApp->getPlatformName();
	const char* baseFilename = g_pApp->getBaseFileName();
	const char* fileExt = g_pApp->getLogFileExt();

	int n = sprintf_k(buf,sizeof(buf), "%s\\%s\\server%s\\%s_%d%s", dirName, platformName, serverName, baseFilename, yyyymmdd, fileExt);
	return string_512(buf, n);
}

void LogFileWriter::removeDeactiveFiles(time_t nowTime)
{
	OutputFileMap::iterator it = m_fileMap.begin();
	OutputFileMap::iterator ite = m_fileMap.end();
	while(it != ite)
	{
		OutputLogFile* file = it->second;
		if(file->m_lastActive + 5*60 < nowTime)
		{
			OutputFileMap::iterator it2 = it; it++;
			m_fileMap.erase(it2);

			if(file == m_currFile)
			{
				m_currKey = NULL;
				m_currFile = NULL;
			}

			file->close();
			OutputLogFile::Free(file);
		}
		else
		{
			it++;
		}
	}
}
