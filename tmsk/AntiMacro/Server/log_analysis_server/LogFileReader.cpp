#include "LogFileReader.h"
#include <System/File/KFile.h>
#include <System/Misc/KDatetime.h>

// KMMap m_mmap;
// int m_currHour;
// HourIndcator* m_hours;
// time_t m_dayExpireTime;
// time_t m_hourExpireTime;

LogIndexFile::LogIndexFile()
{
	m_hours = NULL;
}

LogIndexFile::~LogIndexFile()
{

}

bool LogIndexFile::open(const char* idxFilePath)
{
	if(!JG_F::KFileUtil::IsFileExists(idxFilePath))
	{
		Log(LOG_DEBUG, "warn: index file %s not found.", idxFilePath);
		return false;
	}

	if(!m_mmap.Initialize(idxFilePath, 0, IDX_FILE_SIZE, FALSE))
	{
		Log(LOG_ERROR, "error: initialize mmap for %s", idxFilePath);
		return false;
	}

	KMMap::eFileOpenStatus openStatus = m_mmap.GetOpenStatus();
	switch(openStatus)
	{
	case KMMap::enum_CreateNew:
		return false;
	case KMMap::enum_OpenExists:
		return this->_onOpen();
	case KMMap::enum_ModifyExists:
		return false;
	}
	return false;
}

void LogIndexFile::close()
{
	m_mmap.Finalize();
}

INT64 LogIndexFile::getHourOffset(int hour)
{
	if(!m_hours) return 0;
	
	if(m_hours[hour].used)
		return m_hours[hour].offset;

	for(int i=hour+1; i<24; i++)
	{
		if(m_hours[i].used)
			return m_hours[i].offset;
	}

	return -1;
}

bool LogIndexFile::_onOpen()
{
	m_hours = (HourIndcator*)m_mmap.GetMemory();
	return true;
}

////

// LogIndexFile m_idxFile;
// KTextFileReader m_fileReader;

LogFileReader::LogFileReader()
{
	m_readState = ReadState::none;
}

LogFileReader::~LogFileReader()
{

}

bool LogFileReader::open(const char* filePath, const char* idxFilePath, int hour)
{
	m_readBuf.Reset();

	if(!m_fi.Open(filePath, "rb"))
		return false;

	m_idxFile.open(idxFilePath);
	
	INT64 offset = m_idxFile.getHourOffset(hour);
	if(offset < 0)
		return false;

	m_fi.Seek((size_t)offset);

	m_readState = ReadState::reading;
	return true;
}

void LogFileReader::close()
{
	m_fi.Close();
	m_idxFile.close();
	m_readBuf.Reset();
}

bool LogFileReader::_readToBuffer()
{
	if(!m_fi.Good())
		return false;

	m_readBuf.Compact();

	void* p = m_readBuf.GetWriteBuffer();
	size_t n = m_readBuf.WriteAvail();

	int readed = m_fi.ReadData(p, n);
	if(readed < (int)n)
	{
		m_readState = ReadState::eof;
	}

	if(readed < 1)
		return false;

	m_readBuf.m_wp += readed;
	return true;
}

int LogFileReader::readLine(char** linepp)
{
	int n = this->_readLine(linepp);
	if(n < 0)
	{
		if(m_readState == ReadState::eof)
			return -1;

		if(!this->_readToBuffer())
			return -1;

		return this->_readLine(linepp);

	}
	else if(!n)
	{
		if(*linepp)
			return n;

		if(m_readState == ReadState::eof)
			return -1;

		if(!this->_readToBuffer())
			return -1;

		return this->_readLine(linepp);
	}
	else
	{
		return n;
	}
}

int LogFileReader::_readLine(char** linepp)
{
	int avail = m_readBuf.ReadAvail();
	if(avail < 1)
	{
		*linepp = NULL;
		return -1;
	}

	char ch;
	const char* pc = m_readBuf.GetReadBuffer();
	*linepp = (char*)pc;

	int i = 0;
	while(i < avail)
	{
		ch = *pc;
		if(ch == '\r')
		{
			*(char*)pc = '\0';
		}
		else if(ch == '\n')
		{
			*(char*)pc = '\0';
			m_readBuf.m_rp += i + 1;
			return i;
		}
		pc++; i++;
	}

	*linepp = NULL;
	return 0;
}
