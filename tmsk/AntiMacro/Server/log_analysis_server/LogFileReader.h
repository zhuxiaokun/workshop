#include <System/KPlatform.h>
#include <System/File/KMMap.h>
#include <stdio.h>
#include <CommSession/KPropertyMap.h>
#include <System/File/KTextReader.h>
#include <System/Misc/KStream.h>

class LogIndexFile
{
public:
	struct HourIndcator
	{
		int hour;
		int used;
		INT64 offset;
	};
	enum { HOUR_COUNT = 24, IDX_FILE_SIZE = HOUR_COUNT*sizeof(HourIndcator), };

public:
	KMMap m_mmap;
	HourIndcator* m_hours;

public:
	LogIndexFile();
	~LogIndexFile();

public:
	bool open(const char* logFilePath);
	void close();
	INT64 getHourOffset(int hour);

public:
	bool _onOpen();
};

class LogFileReader
{
public:
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
	LogIndexFile m_idxFile;
	ReadState::State m_readState;
	KInputFileStream m_fi;
	KPieceBuffer<1024*20> m_readBuf;

	//JG_F::KTextFileReader m_fileReader;

public:
	LogFileReader();
	~LogFileReader();

public:
	bool open(const char* filePath, const char* idxFilePath, int hour);
	int readLine(char** linepp);
	void close();

public:
	bool _readToBuffer();
	int  _readLine(char** linepp);
};
