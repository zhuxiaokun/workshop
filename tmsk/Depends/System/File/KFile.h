/********************************************************************
	created:	2008/02/21
	created:	21:2:2008   14:41
	filename: 	Athena\Base\System\File\KFile.h
	file path:	Athena\Base\System\File
	file base:	KFile
	file ext:	h
	author:		xueyan
	
	purpose:	按照一个最常规的做法打开一个文件，能够跨平台
*********************************************************************/
#pragma once

#include "../KType.h"
#include "../KMacro.h"
#include <stdio.h>
#include <stdlib.h>
#include "../Collections/KString.h"
#include "../Collections/KVector.h"

extern "C" FILE* fopen_k(const char* filename, const char* mode);
extern "C" FILE* freopen_k(const char* filename, const char* mode, FILE* f);

namespace System { namespace File {

class KFileUtil
{
public:
	static const char* basename_r(const char* fileName);
	static const char* cleanname_r(const char* fileName, char* cleanName);
	static const char* dirname_r(const char* fileName, char* buf);
	static DWORD GetFileAttribute(const char* fileName);
	static BOOL  IsDir(const char* fileName);
	static BOOL  MakeDir(const char* dirName);
	static BOOL  IsFileExists(const char* fileName);
	static FILE* OpenFile(const char* fileName, const char* mode);
	static FILE* OpenFileForce(const char* fileName, const char* mode);
	static TIME_T GetLastModified(const char* fileName);
	static DWORD GetFileSize(const char* fileName);
	static BOOL SetFileSize(FILE* f, size_t size);
	static BOOL deleteFile(const char* fileName);
	static size_t GetFileSize(FILE* filep);
	static BOOL ftruncate(FILE* filep, size_t fsize);
	static BOOL remove(const char* fpath);
	static BOOL rename(const char* fpath, const char* newfpath);
	static BOOL isUtf8File(const char* fpath, BOOL* signature);
	static BOOL convertToUtf8(const char* fpath, BOOL signature, BOOL* done=NULL);
	static BOOL addUtf8Signature(const char* fpath, BOOL* done=NULL);
	static BOOL convertToGbk(const char* fpath, BOOL* done=NULL);
	static BOOL rmDir(const char* fpath);
	static BOOL rmDirRecursive(const char* fpath);

};

struct KFileData
{
	DWORD m_attributes;
	char  m_dirName[512];
	char  m_shortName[128];
	///////////////////////////////////////////////////
	typedef ::System::Collections::KString<512> FilePath;
	FilePath getPath() const;
	bool IsDir() const;
	bool _isDir() const;
};

class KFileFilter
{
public:
	virtual ~KFileFilter() { }
	virtual BOOL Accept(const KFileData& fileData) = 0;
};

class KFileAllAcceptFilter : public KFileFilter
{
public:
	// 不包含"."目录和".."目录
	BOOL Accept(const KFileData& fileData);
};

class KFileSuffixFilter : public KFileFilter
{
public:
	KFileSuffixFilter(const char* suffix);
	~KFileSuffixFilter();
	BOOL Accept(const KFileData& fileData);

protected:
	::System::Collections::KString<32> m_suffix;
};

class KFileSuffixMultiFilter : public KFileFilter
{
public:
	KFileSuffixMultiFilter(const char* suffixs);
	~KFileSuffixMultiFilter();
	BOOL Accept(const KFileData& fileData);

protected:
	typedef ::System::Collections::KString<32> FilterString;
	typedef ::System::Collections::KVector< FilterString > FilterStringVector;
	FilterStringVector m_SuffixMulti;
};

class KFileAccurateFilter : public KFileFilter // 精确查找某一个文件，大小敏感
{
public:
	KFileAccurateFilter( const char* filename );
	~KFileAccurateFilter();
	BOOL Accept(const KFileData& fileData);
protected:
	::System::Collections::KString<128> m_fileName;
};

class KDirectory
{
public:
	typedef ::System::Collections::KVector<KFileData> FileList;
	class IFileVistor
	{
	public:
		virtual ~IFileVistor() { }
		virtual bool visit(const KFileData& fdata) = 0;
	};

public:
	KDirectory();
	~KDirectory();

public:
	BOOL Open(const char* dirName);
	void Close();
	BOOL ListFile(FileList& fileList, KFileFilter* pFilter);
	BOOL ListFile(FileList& fileList);
	BOOL RecursiveListFile(FileList& fileList, KFileFilter* pFilter);
	BOOL RecursiveListFile(FileList& fileList);
	bool foreach_file(IFileVistor& v);
	bool recursiveDelete();

protected:
	::System::Collections::KString<512> m_dirName;
};

class KFile
{
	// attributes
private:
	FILE*			m_hFile;				// 文件对象句柄
	void*			m_pContentBuffer;		// 内容缓冲区
	unsigned long	m_nContentBufferSize;	// 内容缓冲区大小

	// functions
public:
	KFile(void);
	~KFile(void);

	// 读取文件数据
	unsigned long	Read(void* Buffer, unsigned long ReadBytes);
	// 写入文件数据
	unsigned long	Write(const void* Buffer, unsigned long WriteBytes);
	// 获得文件内容Buffer
	unsigned long	GetBuffer(void* Buffer, long lOffset, unsigned long ulMaxSize);
	unsigned long	SetBuffer(const void* Buffer, long lOffset, unsigned long WriteBytes);

	int				ResetBufferSize(unsigned long ulMaxSize);

	int				IsFileExist(const char* pszFileName);

	// 移动文件指针位置，Origin	-> Initial position: SEEK_CUR SEEK_END SEEK_SET
	long			Seek(long Offset, int Origin);
	// 取得文件指针位置,失败返回-1。
	long			Tell();
	// 取得文件长度,失败返回0。
	unsigned long	Size();

	// 这是一个创建和打开的通用函数。不存在就创建，返回1，存在就打开，返回2
	int				Create(const char* FileName, unsigned long ulMaxSize, int WriteSupport = true);

	// 关闭打开的文件
	int				Close();
};

} /* end File */ } /* System */