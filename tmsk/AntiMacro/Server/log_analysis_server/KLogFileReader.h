#pragma once
#include <System/Misc/KStream.h>
#include "KStatusFile.h"
#include "app_defines.h"
#include <System/Memory/KDataBuffer.h>
#include <System/Memory/RingBuffer.h>
#include <EventSystem/KEventEntity.h>

class KLogFileReader : public KEventEntity
{
public:
	typedef KStatusFile::LogFile LogFile;
	struct ReadState
	{
		enum State
		{
			none,
			reading,
			eof,
		};
	};

public:
	LogFile* m_logFile;
	KInputFileStream m_fi;
	ReadState::State m_readState;
	time_t m_lastModifyTime;
	time_t m_lastSuccessReadTime;
	KPieceBuffer<4096> m_readBuf;

public:
	int m_frameLineCount;
	int m_breatheLineCount;

public:
	KLogFileReader(LogFile& logFile);
	~KLogFileReader();

public:
	virtual bool onEvent(int evtId, const void* data, size_t len);
	virtual bool onTimer(DWORD name, Identity64 cookie, const void* data);

public:
	void breathe(int interval);

public:
	bool openCurrentLogFile();
	bool searchNextLogFile(OUT LogFile* logFile);

private:
	void _close();
	bool _readToBuffer();
	int  _readLine(char** linepp);
	void _processLogLine(char* line);

private:
	bool onTimer_readLogFile(DWORD name, Identity64 cookie, const void* data);
	bool onTimer_searchLogFile(DWORD name, Identity64 cookie, const void* data);
	bool onTimer_searchNextLogFile(DWORD name, Identity64 cookie, const void* data);
	bool onTimer_waitDatabase(DWORD name, Identity64 cookie, const void* data);
};
