#ifndef _K_RUNTIMELUA_H_
#define _K_RUNTIMELUA_H_

#include <lua/KLuaWrap.h>
#include "KTaskSchedule.h"
#include <System/File/KFile.h>

class KRuntimeLuaFile : public KTaskObject
{
public:
	KRuntimeLuaFile(const char* filepath);
	~KRuntimeLuaFile();

public:
	DWORD GetNextActivateTick() const;
	void  OnActivate();
	void  Destroy();

public:
	BOOL loadScript();

public:
	TIME_T m_lastModify;
	DWORD m_lastCheckTick;
	char* m_filepath;
};

class KRuntimeLuaDir : public KTaskObject
{
public:
	typedef char* charp;
	class charpCmp
	{
	public:
		BOOL operator () (const charp& a, const charp& b) const
		{
			return stricmp(a, b) < 0;
		}
	};
	typedef ::System::Collections::DynSortedArray<charp,charpCmp> LuaFileArray;

public:
	KRuntimeLuaDir(const char* dirname);
	~KRuntimeLuaDir();

public:
	DWORD GetNextActivateTick() const;
	void  OnActivate();
	void  Destroy();
	virtual void  OnNewFile(KRuntimeLuaFile* luafile);

public:
	BOOL isExists(const char* fn);
	BOOL loadScriptFiles();

public:
	char* m_dirname;
	LuaFileArray m_files;
	TIME_T m_lastModify;
	DWORD m_lastCheckTick;
};

class IFileObserver
{
public:
	virtual ~IFileObserver() { }
	virtual void on_new_file(const char* path) = 0;
	virtual void on_file_deleted(const char* path) = 0;
	virtual void on_file_changed(const char* path) = 0;
};

class KRuntimeFile : public KTaskObject
{
public:
	KRuntimeFile(const char* filename);
	KRuntimeFile(const char* filename, IFileObserver& observer);
	~KRuntimeFile();

public:
	DWORD GetNextActivateTick() const;
	void  OnActivate();
	void  Destroy();

public:
	TIME_T m_lastModified;
	DWORD m_nextCheckTick;
	::System::Collections::KString<512> m_fullpath;
	IFileObserver* m_pObserver;
};

class KRuntimeDir : public KTaskObject
{
public:
	struct FileTrace
	{
		TIME_T lastModified;
		::System::Collections::KString<512> fullpath;
	};
	class FileTraceCmp
	{
	public:
		int operator() (const FileTrace& a, const FileTrace& b) const
		{
			return strcmp(a.fullpath.c_str(), b.fullpath.c_str());
		}
	};
	typedef System::Collections::KVector<::System::Collections::KString<512> > DirArray;
	typedef System::Collections::KSortedVector2<FileTrace,FileTraceCmp> FileArray;

public:
	KRuntimeDir(const char* dir);
	KRuntimeDir(const char* dir, IFileObserver& observer);
	KRuntimeDir(const char* dir, ::System::File::KFileFilter& filter);
	KRuntimeDir(const char* dir, ::System::File::KFileFilter& filter, IFileObserver& observer);
	virtual ~KRuntimeDir();

public:
	DWORD GetNextActivateTick() const;
	void  OnActivate();
	void  Destroy();

private:
	void loopDir(const char* fullpath);
	void loopDirRecursive(const char* fullpath);

	void _loopDir(const char* fullpath);

	void addFile(const char* fullpath, TIME_T lastModified);
	void removeFile(const char* fullpath);

	void addDir(const char* fullpath, TIME_T lastModified);
	void removeDir(const char* fullpath);

	FileTrace* getDirTrace(const char* fullpath);
	FileTrace* getFileTrace(const char* fullpath);

public:
	::System::Collections::KString<512> m_fullpath;

	FileArray m_dirArray;
	FileArray m_fileArray;
	
	::System::File::KFileFilter* m_pfilter;
	IFileObserver* m_pObserver;

	TIME_T m_lastModified;
	DWORD m_nextCheckTick;

	DirArray m_pendings;
	::System::File::KDirectory::FileList m_flist;
};

#endif
