#ifndef _K_MMAP_H_
#define _K_MMAP_H_

#include "../KType.h"
#include "../KMacro.h"

class KMMap
{
public:
	enum eFileOpenStatus
	{
		enum_Invalid,
		enum_CreateNew,
		enum_OpenExists,
		enum_ModifyExists, // 如果长度不匹配也会导致文件重新创建
	};
	struct MapCreateParam
	{
		const char* fname;
		DWORD offset;
		DWORD length;
		BOOL readonly;
	};

public:
	KMMap();
	~KMMap();

public:
	// len == 0 : 映射长度为文件长度
	BOOL Initialize(const char* fname, DWORD offset=0, DWORD len=0, BOOL readonly=FALSE);
	void Finalize();

public:
	const MapCreateParam& GetCreateParam() const;
	eFileOpenStatus GetOpenStatus() const;
	DWORD GetFileSize() const;
	DWORD GetMapSize() const;
	void* GetMemory();
	DWORD GetError() const;

private:
	BOOL OpenFile();
	BOOL CreateFile();
	BOOL CreateMap();

#if defined(WIN32) || defined(WINDOWS)
	BOOL OpenFile_Win32();
	BOOL CreateFile_Win32();
	BOOL CreateMap_Win32();
	void Finalize_Win32();
#else
	BOOL OpenFile_Linux();
	BOOL CreatheFile_Linux();
	BOOL CreateMap_Linux();
	void Finalize_Linux();
#endif

private:
	MapCreateParam m_createParam;
	eFileOpenStatus m_openStatus;

	DWORD m_fileSize;  // 文件真实长度
	DWORD m_mapSize;   // 映射到内存中的长度
	void* m_pMemory;   // 映射的首地址

#if defined(WIN32) || defined(WINDOWS)
	HANDLE m_hFile;
	HANDLE m_hMapObj;
#else
	int m_fd;          // 文件描述符
#endif

	DWORD m_lastError;
};

#endif