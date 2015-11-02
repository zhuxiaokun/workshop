#pragma once
#include "LogIndexFile.h"
#include <System/Collections/KTree.h>
#include "app_defines.h"

class LogFileWriter
{
public:
	struct FileKey
	{
		ccstr serverName; int yyyymmdd;
		FileKey(const char* sname, int ymd):serverName(sname),yyyymmdd(ymd) { }
		struct cmp
		{
			int operator () (const FileKey& a, const FileKey& b) const
			{
				int n = a.yyyymmdd - b.yyyymmdd; if(n) return n;
				return strcmp(a.serverName, b.serverName);
			}
			static int compare(const FileKey& a, const FileKey& b)
			{
				int n = a.yyyymmdd - b.yyyymmdd; if(n) return n;
				return strcmp(a.serverName, b.serverName);
			}
		};
	};

public:
	typedef JG_C::KMapByTree_staticPooled<FileKey,OutputLogFile*,1024,FileKey::cmp> OutputFileMap;

public:
	OutputFileMap m_fileMap;
	FileKey* m_currKey;
	OutputLogFile* m_currFile;

public:
	LogFileWriter();
	~LogFileWriter();

public:
	bool on_logLine(KPropertyMap& lmap);
	void finalize();
	string_512 buildFilepath(const char* serverName, int yyyymmdd, const char* verb);
	void removeDeactiveFiles(time_t nowTime);
};
