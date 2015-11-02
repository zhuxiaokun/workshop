#pragma once
#include <System/KPlatform.h>
#include <lua/KLuaWrap.h>
#include <System/File/KMMap.h>
#include <System/Collections/KTree.h>
#include "app_defines.h"

class KStatusFile
{
public:
	struct FileHead
	{
		int nextLogFileOffset;
		int nextStringOffset;
	};
	struct LogFile
	{
		int dirOffset;
		int fileNameOffset;
		int yyyymmdd;
		short sequence;
		short used;
		int readOffset;
	};
	enum
	{
		SECTION_SIZE = 512*1024,
		STATUS_FILE_SIZE = sizeof(FileHead) + SECTION_SIZE*2,
		LOG_FILE_OFFSET = sizeof(FileHead),
		STRING_OFFSET = LOG_FILE_OFFSET + SECTION_SIZE,
	};

public:
	typedef KPair<ccstr,ccstr> LogFileName; // dir + filename
	class LogFileCmp
	{
	public:
		int operator () (const LogFileName& a, const LogFileName& b) const
		{
			int n = stricmp(a.first, b.first); if(n) return n;
			return stricmp(a.second, b.second);
		}
	};
	typedef JG_C::KMapByTree_staticPooled<ccstr,int,256,KccstriCmp> NameMap;
	typedef JG_C::KMapByTree_staticPooled<LogFileName,LogFile*,256,LogFileCmp> LogFileMap;

public:
	KMMap m_mmap;
	FileHead* m_fileHead;
	int m_logFileCount;
	LogFile* m_logFiles;
	NameMap m_dirNameMap;
	NameMap m_fileNameMap;
	LogFileMap m_logFileMap;

public:
	KStatusFile();
	~KStatusFile();

public:
	bool initialize(const char* filePath);
	void finalize();

public:
	int getLogFileCount() const;
	const char* getLogDirName(int index) const;
	const char* getLogFileName(int index) const;
	int getLogFileSequence(int index) const;
	int getLogFileOffset(int index) const;

	bool hasLogFile(const char* dirName, const char* fileName) const;
	bool addLogFile(const char* dirName, const char* fileName, int yyyymmdd);
	void removeLogFile(const char* dirName, const char* fileName);
	void removeUnusedFiles();

	string_512 buildLogFilePath(LogFile& logFile) const;

private:
	bool _on_createStatusFile();
	bool _on_openStatusFile();
	const LogFile* _findLogFile(const char* dirName, const char* fileName) const;
	const char* _getString(int offset) const;
	const char* _getExtName(const char* fileName) const;
};

DECLARE_SIMPLE_TYPE(KStatusFile::LogFile);

