#pragma once
#include <System/KType.h>
#include <lua/KLuaTableHolder.h>
#include <System/Collections/KTree.h>
#include <System/Collections/DynArray.h>
#include "redis_scan.h"
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/KList.h>
#include <CommSession/KPropertyMap.h>
#include "app_defines.h"
#include <System/Misc/KDatetime.h>

class AnalysisContext
{
public:
	string_512 m_dataDir;
	string_32 m_platformName;
	string_64 m_baseFileName;
	string_32 m_fileExtName;
	KDatetime m_fromTime, m_toTime;
	KLargeNameList m_serverList;

public:
	AnalysisContext();
	~AnalysisContext();

public:
	bool initialize(const char* dirName, const char* platformName, const char* fileName, KDatetime& ftime, KDatetime& ttime);
	bool process();
	void finalize();

public:
	string_512 getLogFilePath(const char* server, int yyyymmdd);
	string_512 getIndexFilePath(const char* server, int yyyymmdd);
	bool acceptTime(time_t lineTime) const;
	bool isTimeExceed(time_t lineTime) const;

private:
	bool _scanServerList();
};
