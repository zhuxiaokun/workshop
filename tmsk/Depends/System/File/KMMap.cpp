#include "KMMap.h"
#include <string.h>
#include "../File/KFile.h"
#include "../Misc/StrUtil.h"

#if defined(WIN32) || defined(WINDOWS)
#else
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#endif

KMMap::KMMap(void)
{
	memset(&m_createParam, 0, sizeof(m_createParam));
	m_openStatus = enum_Invalid;
	m_fileSize = 0;
	m_mapSize = 0;
	m_pMemory = NULL;
#if defined(WIN32) || defined(WINDOWS)
	m_hFile = INVALID_HANDLE_VALUE;
	m_hMapObj = NULL;
#else
	m_fd = -1;
#endif
	m_lastError = 0;
}

KMMap::~KMMap(void)
{
	this->Finalize();
}

BOOL KMMap::Initialize(const char* fname, DWORD offset, DWORD len, BOOL readonly)
{
	m_createParam.fname = fname;
	m_createParam.offset = offset;
	m_createParam.length = len;
	m_createParam.readonly = readonly;

	if(!this->OpenFile())
	{
		if(readonly || m_lastError) return FALSE;
		if(!this->CreateFile())
			return FALSE;
	}
	
	// 此时文件的大小已经调整好了

	if(!this->CreateMap())
	{
		return FALSE;
	}

	return TRUE;
}

void KMMap::Finalize()
{
#if defined(WIN32) || defined(WINDOWS)
	this->Finalize_Win32();
#else
	this->Finalize_Linux();
#endif
}

const KMMap::MapCreateParam& KMMap::GetCreateParam() const
{
	return m_createParam;
}

KMMap::eFileOpenStatus KMMap::GetOpenStatus() const
{
	return m_openStatus;
}

DWORD KMMap::GetFileSize() const
{
	return m_fileSize;
}

DWORD KMMap::GetMapSize() const
{
	return m_mapSize;
}

void* KMMap::GetMemory()
{
	return m_pMemory;
}

DWORD KMMap::GetError() const
{
	return m_lastError;
}

BOOL KMMap::OpenFile()
{
#if defined(WIN32) || defined(WINDOWS)
	return this->OpenFile_Win32();
#else
	return this->OpenFile_Linux();
#endif
}

BOOL KMMap::CreateFile()
{
#if defined(WIN32) || defined(WINDOWS)
	return this->CreateFile_Win32();
#else
	return this->CreatheFile_Linux();
#endif
}

BOOL KMMap::CreateMap()
{
#if defined(WIN32) || defined(WINDOWS)
	return this->CreateMap_Win32();
#else
	return this->CreateMap_Linux();
#endif
}

#if defined(WIN32) || defined(WINDOWS)
BOOL KMMap::OpenFile_Win32()
{
	DWORD flag = GENERIC_READ;
	if(!m_createParam.readonly) flag |= GENERIC_WRITE;

	char path[512];
	if(currentToLocal(m_createParam.fname, path, sizeof(path)) < 0)
		return FALSE;

	m_hFile = ::CreateFile(path, flag,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if(m_hFile == INVALID_HANDLE_VALUE) return FALSE;

	m_fileSize = ::GetFileSize(m_hFile, NULL);

	if(m_createParam.length == 0)
	{
		m_mapSize = m_fileSize;
	}
	else
	{
		m_mapSize = m_createParam.length;
	}

	// 文件大小不匹配，调整文件的大小
	if(m_fileSize < m_createParam.offset + m_mapSize)
	{
		m_fileSize = m_createParam.offset + m_mapSize;
		DWORD dwRet = ::SetFilePointer(m_hFile, m_fileSize, NULL, FILE_BEGIN);
		if(dwRet != m_fileSize)
		{
			m_lastError = GetLastError();
			return FALSE;
		}
		if(!::SetEndOfFile(m_hFile))
		{
			m_lastError = GetLastError();
			return FALSE;
		}

		m_openStatus = enum_ModifyExists;
	}
	else
	{
		m_openStatus = enum_OpenExists;
	}

	return TRUE;
}

BOOL KMMap::CreateFile_Win32()
{
	DWORD flag = GENERIC_READ;
	if(!m_createParam.readonly) flag |= GENERIC_WRITE;

	char path[512];
	if(currentToLocal(m_createParam.fname, path, sizeof(path)) < 0)
		return FALSE;

	m_hFile = ::CreateFile(path, flag,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if(m_hFile == INVALID_HANDLE_VALUE)
	{
		m_lastError = GetLastError();
		return FALSE;
	}

	if(m_createParam.length == 0)
	{
		m_lastError = 1;
		return FALSE;
	}

	m_mapSize = m_createParam.length;
	m_fileSize = m_createParam.offset + m_mapSize;

	DWORD dwRet = ::SetFilePointer(m_hFile, m_fileSize, NULL, FILE_BEGIN);
	if(dwRet != m_fileSize)
	{
		m_lastError = GetLastError();
		return FALSE;
	}
	if(!::SetEndOfFile(m_hFile))
	{
		m_lastError = GetLastError();
		return FALSE;
	}

	m_openStatus = enum_CreateNew;
	return TRUE;
}

BOOL KMMap::CreateMap_Win32()
{
	DWORD flag = m_createParam.readonly ? PAGE_READONLY : PAGE_READWRITE;

	m_hMapObj = ::CreateFileMapping(m_hFile, NULL, flag, 0, m_fileSize, NULL);
	if (!m_hMapObj)
	{
		m_lastError = ::GetLastError();
		return FALSE;
	}

	flag = FILE_MAP_READ;
	if(!m_createParam.readonly) flag |= FILE_MAP_WRITE;

	m_pMemory = (LPBYTE)::MapViewOfFile(m_hMapObj, flag, 0, m_createParam.offset, m_mapSize);
	if(!m_pMemory)
	{
		m_lastError = ::GetLastError();
		return FALSE;
	}

	return TRUE;
}
void KMMap::Finalize_Win32()
{
	if (m_pMemory)
	{
		::UnmapViewOfFile(m_pMemory);
		m_pMemory = NULL;
	}

	if (m_hMapObj)
	{
		::CloseHandle(m_hMapObj);
		m_hMapObj = NULL;
	}

	if (m_hFile)
	{
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	m_fileSize = 0;
	m_mapSize = 0;
	m_openStatus = enum_Invalid;
	memset(&m_createParam, 0, sizeof(m_createParam));
	m_lastError = ERROR_SUCCESS;
}
#else
BOOL KMMap::OpenFile_Linux()
{
	DWORD openFlags = m_createParam.readonly ? O_RDONLY : O_RDWR;
	m_fd = open(m_createParam.fname, openFlags);
	if(m_fd == -1)
	{
		if(!::System::File::KFileUtil::IsFileExists(m_createParam.fname))
		{
			return FALSE;
		}
		m_lastError = errno;
		return FALSE;
	}

	//m_fileSize = ::GetFileSize(m_hFile, NULL);
	struct stat fstatBuf;
	if(fstat(m_fd, &fstatBuf) == -1)
	{
		m_lastError = errno;
		return FALSE;
	}

	m_fileSize = fstatBuf.st_size;
	if(m_createParam.length == 0)
	{
		m_mapSize = m_fileSize;
	}
	else
	{
		m_mapSize = m_createParam.length;
	}

	// 文件大小不匹配，调整文件的大小
	if(m_fileSize < m_createParam.offset + m_mapSize)
	{
		m_fileSize = m_createParam.offset + m_mapSize;
		if(ftruncate(m_fd, m_fileSize) == -1)
		{
			m_lastError = errno;
			return FALSE;
		}
		m_openStatus = enum_ModifyExists;
	}
	else
	{
		m_openStatus = enum_OpenExists;
	}

	return TRUE;
}

BOOL KMMap::CreatheFile_Linux()
{
	DWORD openFlags = O_RDWR|O_CREAT;
	DWORD createMode = S_IRWXU|S_IRWXG;

	m_fd = open(m_createParam.fname, openFlags, createMode);
	if(m_fd == -1)
	{
		m_lastError = errno;
		return FALSE;
	}

	if(m_createParam.length == 0)
	{
		m_lastError = 1;
		return FALSE;
	}

	m_mapSize = m_createParam.length;
	m_fileSize = m_createParam.offset + m_mapSize;

	if(ftruncate(m_fd, m_fileSize) == -1)
	{
		m_lastError = errno;
		return FALSE;
	}

	m_openStatus = enum_CreateNew;
	return TRUE;
}

BOOL KMMap::CreateMap_Linux()
{
	DWORD prot = m_createParam.readonly ? PROT_READ : PROT_READ|PROT_WRITE;
	m_pMemory = mmap(NULL, m_mapSize, prot, MAP_SHARED, m_fd, m_createParam.offset);
	if(m_pMemory == MAP_FAILED)
	{
		m_pMemory = NULL;
		m_lastError = errno;
		return FALSE;
	}
	return TRUE;
}
void KMMap::Finalize_Linux()
{
	if(m_pMemory)
	{
		munmap(m_pMemory, m_mapSize);
		m_pMemory = NULL;
	}
	if(m_fd != -1)
	{
		close(m_fd);
		m_fd = -1;
	}
	m_fileSize = 0;
	m_mapSize = 0;
	m_openStatus = enum_Invalid;
	memset(&m_createParam, 0, sizeof(m_createParam));
	m_lastError = 0;
}
#endif