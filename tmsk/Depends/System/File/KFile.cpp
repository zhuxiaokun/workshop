#include "KFile.h"
#include <sys/types.h>
#include "../Misc/StrUtil.h"
#if defined(WIN32) || defined(WINDOWS)
#include <io.h>
#include <direct.h>
#else
#include <dirent.h>
#endif
#include <sys/stat.h>
#include "../KPlatform.h"
#include "../Sync/KSync.h"
#include "../Misc/KStream.h"
#include "KTextReader.h"

FILE* fopen_k(const char* filename, const char* mode)
{
	char path[512];
	if(currentToLocal(filename, path, 512) < 0)
		return NULL;
	return fopen(path, mode);
}

FILE* freopen_k(const char* filename, const char* mode, FILE* f)
{
	char path[512];
	if(currentToLocal(filename, path, 512) < 0)
		return NULL;
	return freopen(path, mode, f);
}

namespace System { namespace File {

const char* KFileUtil::basename_r(const char* fileName)
{
	const char* p1 = strrchr(fileName, '\\');
	const char* p2 = strrchr(fileName, '/');
	
	if(p1 && p2)
	{
		return p1 > p2 ? p1+1 : p2+1;
	}
	else if(p1 || p2)
	{
		return p1 ? p1+1 : p2+1;
	}
	else
	{
		return fileName;
	}
}

const char* KFileUtil::cleanname_r(const char* fileName, char* cleanName)
{
	int len = 0;
	const char* baseName = KFileUtil::basename_r(fileName);
	while(baseName[0])
	{
		if(baseName[0] == '.') break;
		cleanName[len++] = baseName[0];
		baseName += 1;
	}
	cleanName[len] = '\0';
	return cleanName;
}

const char* KFileUtil::dirname_r(const char* fileName, char* buf)
{
	const char* bn = KFileUtil::basename_r(fileName);
	if(bn == fileName)
	{
		buf[0] = '.';
		buf[1] = 0;
		return buf;
	}
	memcpy(buf, fileName, bn-fileName-1);
	buf[bn-fileName-1] = '\0';
	return buf;
}

DWORD KFileUtil::GetFileAttribute(const char* fileName)
{
	char path[512];
	if(currentToLocal(fileName, path, 512) < 0)
		return 0;
	struct stat statBuffer;
	if(stat(path, &statBuffer) == -1)
		return 0;
	return statBuffer.st_mode;
}

BOOL KFileUtil::IsDir(const char* fileName)
{
	char path[512];
	if(currentToLocal(fileName, path, 512) < 0)
		return 0;
	struct stat statBuffer;
	if(stat(path, &statBuffer) == -1)
		return FALSE;
	return (statBuffer.st_mode & S_IFDIR) > 0;
}

BOOL  KFileUtil::MakeDir(const char* dirName)
{
	char parentDir[MAX_PATH];
	if(KFileUtil::IsFileExists(dirName)) return TRUE;

	KFileUtil::dirname_r(dirName, parentDir);
	if(!KFileUtil::IsFileExists(parentDir))
	{
		if(!KFileUtil::MakeDir(parentDir))
			return FALSE;
	}

	char path[512];
	if(currentToLocal(dirName, path, 512) < 0)
		return 0;
	int nret = makedir(path);
	return nret == 0;
}

BOOL KFileUtil::IsFileExists(const char* fileName)
{
	if(!fileName || !fileName[0])
		return FALSE;

#if defined(WIN32)
	if(fileName[1] == ':' && fileName[2] == '\0')
	{
		char drive = (char)toupper(fileName[0]);
		int driveNo = drive - 'A';
		if(driveNo < 0) return FALSE;
		unsigned long driveMasks = _getdrives();
		return driveMasks & (1<<driveNo);
	}
#endif

	char path[512];
	if(currentToLocal(fileName, path, 512) < 0)
		return 0;
	struct stat statBuffer;
	if(stat(path, &statBuffer) == -1)
		return FALSE;
	return TRUE;
}

FILE* KFileUtil::OpenFile(const char* fileName, const char* mode)
{
	return fopen_k(fileName, mode);
}

FILE* KFileUtil::OpenFileForce(const char* fileName, const char* mode)
{
	char dirName[512];
	FILE* pf = KFileUtil::OpenFile(fileName, mode);
	if(!pf)
	{
		KFileUtil::dirname_r(fileName, dirName);
		if(!KFileUtil::MakeDir(dirName)) return NULL;
		return KFileUtil::OpenFile(fileName, mode);
	}
	return pf;
}

TIME_T KFileUtil::GetLastModified(const char* fileName)
{
	char path[512];
	if(currentToLocal(fileName, path, 512) < 0)
		return 0;
	struct stat statBuffer;
	if(stat(path, &statBuffer) == -1)
		return 0x7fffffff;
	return (TIME_T)statBuffer.st_mtime;
}

DWORD KFileUtil::GetFileSize(const char* fileName)
{
	char path[512];
	if(currentToLocal(fileName, path, 512) < 0)
		return 0;
	struct stat statBuffer;
	if(stat(path, &statBuffer) == -1)
		return 0;
	return statBuffer.st_size;
}

BOOL KFileUtil::SetFileSize(FILE* f, size_t size)
{
#if defined(WIN32) || defined(WINDOWS)
	int fd = _fileno(f);
	return _chsize(fd, (long)size) == 0;
#else
	int fd = fileno(f);
	return ::ftruncate(fd, size) == 0;
#endif
}

BOOL KFileUtil::deleteFile(const char* fileName)
{
	char path[512];
	if(currentToLocal(fileName, path, 512) < 0)
		return 0;
#if defined(WIN32) || defined(WINDOWS)
	return ::DeleteFileA(path);
#else
	return remove(path) == 0;
#endif
}

size_t KFileUtil::GetFileSize(FILE* filep)
{
#if defined(WIN32) || defined(WINDOWS)
	struct _stat buffer;
	int fd = _fileno(filep);
	if(!_fstat(fd, &buffer))
		return buffer.st_size;
	return 0;
#else
	struct stat buffer;
	int fd = fileno(filep);
	if(!fstat(fd, &buffer))
		return buffer.st_size;
	return 0;
#endif
}

BOOL KFileUtil::ftruncate(FILE* filep, size_t fsize)
{
	return KFileUtil::SetFileSize(filep, fsize);
}

BOOL KFileUtil::remove(const char* fileName)
{
	char path[512];
	if(currentToLocal(fileName, path, 512) < 0)
		return 0;
	return ::remove(path) == 0;
}

BOOL KFileUtil::rename(const char* fpath, const char* newfpath)
{
	char src[512];
	if(currentToLocal(fpath, src, sizeof(src)) < 0)
		return FALSE;

	char dst[512];
	if(currentToLocal(newfpath, dst, sizeof(dst)) < 0)
		return FALSE;

	return ::rename(src, dst) == 0;
}

BOOL KFileUtil::isUtf8File(const char* fpath, BOOL* signature)
{
	KInputFileStream si;
	if(!si.Open(fpath)) return FALSE;
	DWORD dwSignature = 0;
	if(si.ReadData(&dwSignature, 3) == 3 && dwSignature == utf8_signature)
	{
		*signature = TRUE;
		return TRUE;
	}
	return FALSE;
	//*signature = FALSE;
	//si.Seek(0);
	//JG_F::KTextReader r;
	//r.SetInputStream(si);
	//char* line;
	//while(TRUE)
	//{
	//	int n = r.ReadLine(line, "\r\n");
	//	if(n < 0) break;
	//	if(!n) continue;
	//	line[n] = '\0';
	//	if(!_is_utf8_string(line))
	//		return FALSE;
	//}
	//return TRUE;
}

BOOL KFileUtil::convertToUtf8(const char* fpath, BOOL signature, BOOL* done)
{
	BOOL withSignature;
	BOOL isUtf8 = FALSE;

	if(done) *done = FALSE;
	if(isUtf8 = isUtf8File(fpath, &withSignature), isUtf8)
	{
		if(signature == withSignature)
			return TRUE;
	}

	KInputFileStream fi;
	if(!fi.Open(fpath, "rb")) return FALSE;

	EnumEncoding oldEncoding = g_encoding;
	g_encoding = encode_utf8;

	KTextReader r;
	r.SetInputStream(fi);

	JG_C::KString<512> nfpath(fpath);
	nfpath.append(".utf8");

	KOutputFileStream fo;
	if(!fo.Open(nfpath.c_str(), "w+b"))
	{
		g_encoding = oldEncoding;
		return FALSE;
	}

	if(signature)
		fo.WriteData(&utf8_signature, 3);

	int lineNum = 0;
	char line[10240];
	BOOL success = TRUE;
	while(TRUE)
	{
		lineNum++;
		int n = r.ReadLine(line, sizeof(line), "\r\n");
		if(n < 0) break;
		if(!n) {  fo.WriteData("\n", 1); continue; }
		fo.WriteData(line, n);
		if(!r.eof()) fo.WriteData("\n", 1);
	}
	g_encoding = oldEncoding;
	fi.Close(); fo.Close();

	if(success
		&& KFileUtil::remove(fpath)
		&& KFileUtil::rename(nfpath.c_str(), fpath))
	{
		if(done) *done = TRUE;
		return TRUE;
	}

	DWORD errCode = GetLastError();
	KFileUtil::remove(nfpath.c_str());
	return FALSE;
}

BOOL KFileUtil::addUtf8Signature(const char* fpath, BOOL* done)
{
	return FALSE; // this function not supported

	if(done) *done = FALSE;

	KInputFileStream fi;
	if(!fi.Open(fpath, "rb")) return FALSE;

	DWORD flag = 0;
	if(fi.ReadData(&flag, 3) == 3 && flag == utf8_signature)
		return TRUE;

	fi.Seek(0);

	JG_C::KString<512> nfpath(fpath);
	nfpath.append(".utf8");

	KOutputFileStream fo;
	if(!fo.Open(nfpath.c_str(), "w+b"))
		return FALSE;

	fo.WriteData(&utf8_signature, 3);

	size_t n; char buf[4096];
	while(n = (size_t)fi.ReadData(buf, sizeof(buf)), n>0)
		fo.WriteData(buf, n);
	fi.Close(); fo.Close();

	if(KFileUtil::remove(fpath)
		&& KFileUtil::rename(nfpath.c_str(), fpath))
	{
		if(done) *done = TRUE;
		return TRUE;
	}

	DWORD errCode = GetLastError();
	KFileUtil::remove(nfpath.c_str());
	return FALSE;
}

BOOL KFileUtil::rmDir(const char* fpath)
{
#if defined(WIN32)
	return _rmdir(fpath) == 0;
#else
	return rmdir(fpath) == 0;
#endif
}

BOOL KFileUtil::rmDirRecursive(const char* fpath)
{
	KDirectory dir; dir.Open(fpath);
	return dir.recursiveDelete() ? TRUE : FALSE;
}

BOOL KFileUtil::convertToGbk(const char* fpath, BOOL* done)
{
	BOOL withSignature;
	if(done) *done = FALSE;
	if(!isUtf8File(fpath, &withSignature))
		return TRUE;

	KInputFileStream fi;
	if(!fi.Open(fpath, "r")) return FALSE;

	EnumEncoding oldEncoding = g_encoding;
	g_encoding = encode_ansii;

	KTextReader r;
	r.SetInputStream(fi);

	JG_C::KString<512> nfpath(fpath);
	nfpath.append(".gbk");

	KOutputFileStream fo;
	if(!fo.Open(nfpath.c_str(), "w+"))
	{
		g_encoding = oldEncoding;
		return FALSE;
	}

	char line[10240];
	BOOL success = TRUE;
	while(TRUE)
	{
		int n = r.ReadLine(line, sizeof(line), "\r\n");
		if(n < 0) break;
		if(!n) {  fo.WriteData("\n", 1); continue; }
		fo.WriteData(line, n);
		if(!r.eof()) fo.WriteData("\n", 1);
	}
	fi.Close(); fo.Close();
	g_encoding = oldEncoding;

	if(success
		&& KFileUtil::remove(fpath)
		&& KFileUtil::rename(nfpath.c_str(), fpath))
	{
		if(done) *done = TRUE;
		return TRUE;
	}

	DWORD errCode = GetLastError();
	KFileUtil::remove(nfpath.c_str());
	return FALSE;
}

/// KFileData //

KFileData::FilePath KFileData::getPath() const
{
	FilePath path(m_dirName);
#if defined(WIN32) || defined(WINDOWS)
	path.append('\\');
#else
	path.append('/');
#endif
	path.append(m_shortName);
	return path;
}

bool KFileData::IsDir() const
{
	FilePath fpath = this->getPath();
	return KFileUtil::IsDir(fpath.c_str()) ? true : false;
}

bool KFileData::_isDir() const
{
#if defined(WIN32)
	return (m_attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
	return (m_attributes & DT_DIR) != 0;
#endif
}

BOOL KFileAllAcceptFilter::Accept( const KFileData& fileData )
{
	return ( strcmp( fileData.m_shortName, "." ) != 0
		&& strcmp( fileData.m_shortName, ".." ) != 0 );
}

KFileSuffixFilter::KFileSuffixFilter(const char* suffix)
{
	m_suffix = suffix;
}

KFileSuffixFilter::~KFileSuffixFilter()
{

}

BOOL KFileSuffixFilter::Accept(const KFileData& fileData)
{
	size_t nameLen = strlen(fileData.m_shortName);
	if(nameLen < m_suffix.size()) return FALSE;
	const char* pSuffix = &fileData.m_shortName[nameLen - m_suffix.size()];
	return _stricmp(m_suffix.c_str(), pSuffix) == 0;
}

KFileSuffixMultiFilter::KFileSuffixMultiFilter(const char* suffixs)
{
	if( !suffixs ) return;
	if( strlen(suffixs) >= MAX_PATH ) return;
	char lszSuffixs[MAX_PATH] = {0};
	strcpy_k(lszSuffixs, MAX_PATH, suffixs);
	char * lpText = NULL;
	if( 0 != strcmp("", lszSuffixs) )
	{
		lpText = strtok(lszSuffixs, "|");
		while(lpText) { m_SuffixMulti.push_back( lpText ); lpText = strtok( NULL, "|" ); }
	}
}

KFileSuffixMultiFilter::~KFileSuffixMultiFilter()
{
	m_SuffixMulti.clear();
}

BOOL KFileSuffixMultiFilter::Accept(const KFileData& fileData)
{
	int nameLen = (int)strlen(fileData.m_shortName);
	FilterString lstrSuffix;
	for(INT i = 0; i < m_SuffixMulti.size(); ++i)
	{
		lstrSuffix = m_SuffixMulti[i];
		const char* pSuffix = &fileData.m_shortName[nameLen - lstrSuffix.size()];
		if( 0 == _stricmp(lstrSuffix.c_str(), pSuffix) ) return TRUE;
	}
	return FALSE;
}

KFileAccurateFilter::KFileAccurateFilter( const char* filename )
{
	m_fileName = filename;
}

KFileAccurateFilter::~KFileAccurateFilter()
{

}

BOOL KFileAccurateFilter::Accept( const KFileData& fileData )
{
	return strcmp( m_fileName.c_str(), fileData.m_shortName ) == 0;
}


KDirectory::KDirectory()
{

}

KDirectory::~KDirectory()
{

}

BOOL KDirectory::Open(const char* dirName)
{
	m_dirName = dirName;
	return TRUE;
}

void KDirectory::Close()
{

}

BOOL KDirectory::ListFile(FileList& fileList, KFileFilter* pFilter)
{
	fileList.clear();
	KFileData fileData;

	memcpy(fileData.m_dirName, m_dirName.c_str(), m_dirName.size());
	fileData.m_dirName[m_dirName.size()] = '\0';

#if defined(WIN32) || defined(WINDOWS)
	char buf[512];
	int c = currentToLocal(m_dirName.c_str(), buf, 512);
	if(c < 0) return FALSE;
	
	buf[c++] = PATH_SEPERATOR;
	buf[c++] = '*';
	buf[c++] = '\0';

	WIN32_FIND_DATA fdata;
	HANDLE hfile = FindFirstFile(buf, &fdata);
	if(hfile == INVALID_HANDLE_VALUE)
		return FALSE;

	while(TRUE)
	{
		char* fname = &fdata.cFileName[0];
		char* pSlash = strrchr(fdata.cFileName, PATH_SEPERATOR);
		if(pSlash) fname = pSlash + 1;
		if(localToCurrent(fname, fileData.m_shortName, sizeof(fileData.m_shortName)) < 0)
			return FALSE;
		fileData.m_attributes = fdata.dwFileAttributes;
		if(pFilter->Accept(fileData))
		{
			fileList.push_back(fileData);
		}
		if(!FindNextFile(hfile, &fdata))
			break;
	}
	
	FindClose(hfile);
#else
	static JG_S::KThreadMutex m_mx;
	JG_S::KAutoThreadMutex mx(m_mx);

	DIR* pDir = opendir(m_dirName.c_str());
	if(!pDir) return FALSE;
	
	dirent* pFile = readdir(pDir);
	while(pFile)
	{
		char* fname = &pFile->d_name[0];
		char* pSlash = strrchr(fname, '/');
		if(pSlash) fname = pSlash + 1;

		strcpy_k(fileData.m_shortName, sizeof(fileData.m_shortName), fname);
		fileData.m_attributes = pFile->d_type;

		if(pFilter->Accept(fileData))
		{
			fileList.push_back(fileData);
		}

		pFile = readdir(pDir);
	}
	closedir(pDir);
#endif

	return TRUE;
}
BOOL KDirectory::ListFile(FileList& fileList)
{
	KFileAllAcceptFilter FileAllAcceptFilter;
	return ListFile( fileList, &FileAllAcceptFilter );
}

BOOL KDirectory::RecursiveListFile( FileList& fileList, KFileFilter* pFilter )
{
	FileList TempFileList;

	if ( !ListFile( TempFileList ) )
		return FALSE;

	int nFileSize = TempFileList.size();
	for ( int i = 0; i < nFileSize; ++i )
	{
		const KFileData& FileData = TempFileList[i];
		if ( FileData.IsDir() )
		{
			::System::File::KDirectory DirObj;
			DirObj.Open( FileData.getPath().c_str() );
			DirObj.RecursiveListFile( fileList, pFilter );
		}

		if ( pFilter->Accept( FileData ) )
			fileList.push_back( FileData );
	}

	return TRUE;
}

BOOL KDirectory::RecursiveListFile( FileList& fileList )
{
	KFileAllAcceptFilter FileAllAcceptFilter;
	return RecursiveListFile( fileList, &FileAllAcceptFilter );
}

bool KDirectory::foreach_file(KDirectory::IFileVistor& v)
{
	JG_C::KVector<KFileData::FilePath> dirs;
	dirs.push_back(m_dirName);

	KDirectory dir;
	KDirectory::FileList flist;

	while(dirs.size())
	{
		KFileData::FilePath fpath = dirs.at(0);
		dirs.erase(0);

		if(!dir.Open(fpath.c_str())) continue;
		if(!dir.ListFile(flist)) continue;
	
		int n = flist.size();
		for(int i=0; i<n; i++)
		{
			const KFileData& fdata = flist.at(i);
			if(fdata.IsDir())
			{
				if(v.visit(fdata))
				{
					KFileData::FilePath fpath = fdata.getPath();
					dirs.push_back(fpath);
				}
			}
			else
			{
				if(!v.visit(fdata))
					break;
			}
		}
	}

	return true;
}

bool KDirectory::recursiveDelete()
{
	KDirectory::FileList flist;
	if(!this->ListFile(flist)) return false;

	int n = flist.size();
	for(int i=0; i<n; i++)
	{
		const KFileData& fdata = flist.at(i);
		if(fdata._isDir())
		{
			KDirectory dir2; dir2.Open(fdata.getPath().c_str());
			dir2.recursiveDelete();
		}
		else
		{
			KFileUtil::deleteFile(fdata.getPath().c_str());
		}
	}

	KFileUtil::rmDir(m_dirName.c_str());
	return true;
}

/// KFile

KFile::KFile(void)
{
	//ResetBufferSize(FILE_BUFFER_DEFAULT_SIZE);
	m_hFile = NULL;
	m_pContentBuffer = NULL;
	m_nContentBufferSize = 0;
}

KFile::~KFile(void)
{
}

//---------------------------------------------------------------------------
// 功能:	打开一个文件，准备读取写
// 参数:	FileName	文件名
// 返回:	成功返回TRUE，失败返回FALSE。
//---------------------------------------------------------------------------
int	KFile::Create(const char* FileName, unsigned long ulMaxSize, int WriteSupport /*= true*/)
{
	//入口参数合法性判断有engine的对外接口函数负责，这里不再处理

	int nRet = 0;

	Close();

	char PathName[MAX_PATH];
	strcpy_k(PathName, MAX_PATH, FileName);

	//g_GetFullPath(PathName, FileName);

#ifdef __linux
	{
		char *ptr = PathName;
		while(*ptr)
		{
			if (*ptr == '\\')
				*ptr = '/';
			ptr++;
		}
	}
#endif	// #ifdef __linux

	const char*	pMode = "rb";
	if (WriteSupport)
	{
		if (IsFileExist(PathName))
		{
			pMode = "r+b";
			nRet = 2;

		} // end if
		else
		{
			pMode = "w+b";
			nRet = 1;
		} // end else
	}
	else
	{
		pMode = "rb";
		nRet = 1;
	}
	m_hFile = JG_F::KFileUtil::OpenFile(PathName, pMode);

#ifdef __linux
	{
		if (m_hFile == NULL)
		{
			g_StrLower(PathName);
			m_hFile = JG_F::KFileUtil::OpenFile(PathName, pMode);
		}
	}
#endif	// #ifdef __linux

	if (m_hFile == NULL)
		nRet = 0;

	return nRet;
}

// 获得文件内容Buffer
unsigned long	KFile::GetBuffer(void* Buffer, long lOffset, unsigned long ulMaxSize)
{
	unsigned long ulRet = 0;

	if (m_hFile && ulMaxSize)
	{

		Seek(lOffset, SEEK_SET);
		if (Read(Buffer, ulMaxSize) == ulMaxSize)
		{
			ulRet = ulMaxSize;
		} // end if

	} // end if

	return ulRet;
	/*
	void* pRet = NULL;


	if (ulMaxSize > m_nContentBufferSize)
	{
	if ( ResetBufferSize(ulMaxSize) )
	{
	if (m_pContentBuffer)
	{
	Seek(lOffset, SEEK_SET);
	if (Read(m_pContentBuffer, ulMaxSize) == ulMaxSize)
	{
	pRet = m_pContentBuffer;
	} // end if

	} // end if m_pContentBuffer

	} // end if

	} // end if

	return pRet;
	*/
}

unsigned long KFile::SetBuffer(const void* Buffer, long lOffset, unsigned long ulMaxSize)
{
	unsigned long ulRet = 0;

	if (m_hFile && ulMaxSize)
	{
		Seek(lOffset, SEEK_SET);
		if (Write(Buffer, ulMaxSize) == ulMaxSize)
		{
			ulRet = ulMaxSize;
		} // end if
	} // end if
	return ulRet;
} // end SetBuffer


int KFile::ResetBufferSize(unsigned long ulMaxSize)
{
	const int FILE_BUFFER_MAX_SIZE = 102400;

	int nRet = 0;
	if (ulMaxSize > FILE_BUFFER_MAX_SIZE)
	{
		return nRet;
	} // end if


	SAFE_DELETE_ARRAY_2(BYTE, m_pContentBuffer);

	m_pContentBuffer = SAFE_NEW_ARRAY(BYTE, ulMaxSize);

	if (m_pContentBuffer != NULL)
	{
		m_nContentBufferSize = ulMaxSize;
		nRet = 1;
	} // end if

	return nRet;
} // end ResetBufferSize

int	KFile::IsFileExist(const char* pszFileName)
{
	int nRet = 0;

	if (pszFileName && pszFileName[0])
	{
		FILE* pTemp = NULL;
		pTemp = JG_F::KFileUtil::OpenFile(pszFileName, "r");
		if (pTemp)
		{
			nRet = 1;
			fclose(pTemp);
		} // end if

	} // end if
	return nRet;
} // end IsFileExist


//---------------------------------------------------------------------------
// 功能:	读取文件数据
// 参数:	Buffer	读取数据存放的内存区域
//			ReadBytes	读取数据的字节数
// 返回:	成功返回读取的字节数，失败返回0。
//---------------------------------------------------------------------------
unsigned long KFile::Read(void* Buffer, unsigned long ReadBytes)
{
	long BytesReaded;
	if (m_hFile)
		BytesReaded = (unsigned long)fread(Buffer, 1, ReadBytes, m_hFile);
	else
		BytesReaded = 0;
	return BytesReaded;
}

//---------------------------------------------------------------------------
// 功能:	写入文件数据
// 参数:	Buffer		写入数据存放的内存区域
//			WriteBytes	写入数据的字节数
// 返回:	成功返回写入的字节数，失败返回0。
//---------------------------------------------------------------------------
unsigned long KFile::Write(const void* Buffer, unsigned long WriteBytes)
{
	unsigned long BytesWrited;
	if (m_hFile)
		BytesWrited = (unsigned long)fwrite(Buffer, 1, WriteBytes, m_hFile);
	else
		BytesWrited = 0;

	return BytesWrited;
}

//---------------------------------------------------------------------------
// 功能:	移动文件指针位置
// 参数:	Offset		Number of bytes from origin. 
//			Origin		Initial position: SEEK_CUR SEEK_END SEEK_SET
// 返回:	成功返回指针位置，失败返回-1。
//---------------------------------------------------------------------------
long KFile::Seek(long Offset, int Origin)
{
	if (m_hFile)
	{
		fseek(m_hFile, Offset, Origin);
		Offset = ftell(m_hFile);
	}
	else
	{
		Offset = -1;
	}

	return Offset;
}

//---------------------------------------------------------------------------
// 功能:	取得文件指针位置
// 返回:	成功返回指针位置，失败返回-1。
//---------------------------------------------------------------------------
long KFile::Tell()
{
	long Offset;
	if (m_hFile)
		Offset = ftell(m_hFile);
	else
		Offset = -1;
	return Offset;
}

//---------------------------------------------------------------------------
// 功能:	取得文件长度
// 返回:	成功返回文件长度，失败返回0。
//---------------------------------------------------------------------------
unsigned long KFile::Size()
{
	unsigned long Size;
	if (m_pContentBuffer)
	{
		Size = m_nContentBufferSize;
	}
	else if (m_hFile)
	{
		long Offset = ftell(m_hFile);
		fseek(m_hFile, 0, SEEK_END);
		Size = ftell(m_hFile);
		fseek(m_hFile, Offset, SEEK_SET);
		//		if (Size == (unsigned long)(-1))
		//			Size = 0;
	}
	else
	{
		Size = 0;
	}
	return Size;
}

//---------------------------------------------------------------------------
// 功能:	关闭打开的文件
//---------------------------------------------------------------------------
int KFile::Close()
{
	int nRet = 0;
	m_nContentBufferSize = 0;
	SAFE_DELETE_ARRAY_2(BYTE,m_pContentBuffer);
	if (m_hFile)
	{
		fclose(m_hFile);
		m_hFile	= NULL;
		nRet = 1;
	}
	return nRet;
}

} /* end File */ } /* System */