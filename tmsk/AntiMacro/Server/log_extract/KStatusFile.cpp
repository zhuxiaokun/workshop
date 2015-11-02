#include "KStatusFile.h"
#include <System/File/KFile.h>
#include <System/Collections/DynArray.h>

KStatusFile::KStatusFile()
{
	m_fileHead = NULL;
	m_logFileCount = 0;
	m_logFiles = NULL;
}

KStatusFile::~KStatusFile()
{
	this->finalize();
}

bool KStatusFile::initialize(const char* filePath)
{
	if(!m_mmap.Initialize(filePath, 0, STATUS_FILE_SIZE))
	{
		Log(LOG_ERROR, "error: initialize mmap for '%s'", filePath);
		return false;
	}

	KMMap::eFileOpenStatus openStatus = m_mmap.GetOpenStatus();
	const KMMap::MapCreateParam createParam = m_mmap.GetCreateParam();

	switch(openStatus)
	{
	case KMMap::enum_Invalid:
		ASSERT(FALSE);
		break;
	case KMMap::enum_CreateNew:
		if(!this->_on_createStatusFile()) return false;
		break;
	case KMMap::enum_OpenExists:
		if(!this->_on_openStatusFile()) return false;
		break;
	case KMMap::enum_ModifyExists:
		Log(LOG_WARN, "warn: status_file '%s' has an invalid length, reset it.");
		if(!this->_on_createStatusFile()) return false;
		break;
	}

	return true;
}

void KStatusFile::finalize()
{
	if(m_fileHead)
	{
		m_mmap.Finalize();
		m_fileHead = NULL;
		m_logFileCount = 0;
		m_logFiles = NULL;
		m_dirNameMap.clear();
		m_fileNameMap.clear();
		m_logFileMap.clear();
	}
}

void KStatusFile::removeUnusedFiles()
{
	int n = m_logFileCount;
	for(int i=n-1; i>=0; i--)
	{
		LogFile& logFile = m_logFiles[i];
		if(!logFile.used)
		{
			const char* dirName = this->_getString(logFile.dirOffset);
			const char* fileName = this->_getString(logFile.fileNameOffset);
			this->removeLogFile(dirName, fileName);
		}
	}
}

string_512 KStatusFile::buildLogFilePath(KStatusFile::LogFile& logFile) const
{
	char cleanNameBuff[128];

	const char* dirName = this->_getString(logFile.dirOffset);
	const char* fileName = this->_getString(logFile.fileNameOffset);
	const char* cleanName = JG_F::KFileUtil::cleanname_r(fileName, cleanNameBuff);
	const char* extName = this->_getExtName(fileName);

	int sequence = logFile.sequence;

	int n;
	char buf[512];

	if(!sequence)
		n = sprintf_k(buf, sizeof(buf), "%s%c%s_%08d.%s",
			dirName, PATH_SEPERATOR, cleanName,
			logFile.yyyymmdd, extName);
	else
		n = sprintf_k(buf, sizeof(buf), "%s%c%s_%08d_%d.%s",
			dirName, PATH_SEPERATOR, cleanName,
			logFile.yyyymmdd, logFile.sequence, extName);

	return string_512(buf, n);
}

bool KStatusFile::_on_createStatusFile()
{
	m_fileHead = (FileHead*)m_mmap.GetMemory();
	m_fileHead->nextLogFileOffset = LOG_FILE_OFFSET;
	m_fileHead->nextStringOffset = STRING_OFFSET;
	m_logFiles = (LogFile*)(m_fileHead + 1);
	return true;
}

bool KStatusFile::_on_openStatusFile()
{
	m_fileHead = (FileHead*)m_mmap.GetMemory();
	
	m_logFileCount = (m_fileHead->nextLogFileOffset - LOG_FILE_OFFSET) / sizeof(LogFile);
	m_logFiles = (LogFile*)(m_fileHead + 1);

	for(int i=0; i<m_logFileCount; i++)
	{
		LogFile& logFile = m_logFiles[i];
		logFile.used = 0;
		const char* dirName = (char*)m_fileHead + logFile.dirOffset;
		const char* fileName = (char*)m_fileHead + logFile.fileNameOffset;
		m_dirNameMap[dirName] = logFile.dirOffset;
		m_fileNameMap[fileName] = logFile.fileNameOffset;
		m_logFileMap[LogFileName(dirName,fileName)] = &logFile;
	}

	return true;
}

int KStatusFile::getLogFileCount() const
{
	return m_logFileCount;
}

const char* KStatusFile::getLogDirName(int index) const
{
	if(index < 0 || index >= m_logFileCount)
		return NULL;
	return (char*)m_fileHead + m_logFiles[index].dirOffset;
}

const char* KStatusFile::getLogFileName(int index) const
{
	if(index < 0 || index >= m_logFileCount)
		return NULL;
	return (char*)m_fileHead + m_logFiles[index].fileNameOffset;
}

int KStatusFile::getLogFileSequence(int index) const
{
	if(index < 0 || index >= m_logFileCount)
		return NULL;
	return m_logFiles[index].sequence;
}

int KStatusFile::getLogFileOffset(int index) const
{
	if(index < 0 || index >= m_logFileCount)
		return NULL;
	return m_logFiles[index].readOffset;
}

bool KStatusFile::hasLogFile(const char* dirName, const char* fileName) const
{
	return this->_findLogFile(dirName, fileName) != NULL;
}

bool KStatusFile::addLogFile(const char* dirName, const char* fileName, int yyyymmdd)
{
	int dirOffset = 0;
	int fileNameOffset = 0;

	LogFile* pLogFile = (LogFile*)((KStatusFile*)this)->_findLogFile(dirName, fileName);
	if(pLogFile)
	{
		pLogFile->used = 1;
		if(pLogFile->yyyymmdd < yyyymmdd)
		{
			pLogFile->yyyymmdd = yyyymmdd;
			pLogFile->sequence = 0;
		}
		return true;
	}

	NameMap::iterator it = m_dirNameMap.find(dirName);
	if(it != m_dirNameMap.end())
	{
		dirOffset = it->second;
	}
	else
	{
		dirOffset = m_fileHead->nextStringOffset;
		char* p = (char*)m_fileHead + dirOffset;
		m_fileHead->nextStringOffset += strcpy_k(p, 1024, dirName) + 1;
	}

	it = m_fileNameMap.find(fileName);
	if(it != m_fileNameMap.end())
	{
		fileNameOffset = it->second;
	}
	else
	{
		fileNameOffset = m_fileHead->nextStringOffset;
		char* p = (char*)m_fileHead + fileNameOffset;
		m_fileHead->nextStringOffset += strcpy_k(p, 1024, fileName) + 1;
	}

	LogFile& logFile = m_logFiles[m_logFileCount++];
	m_fileHead->nextLogFileOffset += sizeof(LogFile);

	logFile.dirOffset = dirOffset;
	logFile.fileNameOffset = fileNameOffset;
	logFile.yyyymmdd = yyyymmdd;
	logFile.sequence = 0;
	logFile.used = 1;
	logFile.readOffset = 0;

	dirName = (char*)m_fileHead + logFile.dirOffset;
	fileName = (char*)m_fileHead + logFile.fileNameOffset;
	m_logFileMap[LogFileName(dirName,fileName)] = &logFile;

	return true;
}

void KStatusFile::removeLogFile(const char* dirName, const char* fileName)
{
	LogFile* pLogFile = (LogFile*)((KStatusFile*)this)->_findLogFile(dirName, fileName);
	if(!pLogFile) return;
	
	JG_C::DynArray<LogFile> fileArray;
	fileArray.attach(m_logFiles, m_logFileCount, m_logFileCount);

	int index = (int)((uint_r)pLogFile - (uint_r)m_logFiles)/sizeof(LogFile);
	fileArray.erase(index);

	m_logFileCount -= 1;
	m_fileHead->nextLogFileOffset -= sizeof(LogFile);

	m_logFileMap.erase(LogFileName(dirName, fileName));
}

const KStatusFile::LogFile* KStatusFile::_findLogFile(const char* dirName, const char* fileName) const
{
	LogFileName n(dirName, fileName);
	LogFileMap::const_iterator it = m_logFileMap.find(n);
	if(it == m_logFileMap.end()) return NULL;
	return it->second;
}

const char* KStatusFile::_getString(int offset) const
{
	return (const char*)m_fileHead + offset;
}

const char* KStatusFile::_getExtName(const char* fileName) const
{
	const char* p = strrchr(fileName, '.');
	if(!p) return "";
	return p+1;
}
