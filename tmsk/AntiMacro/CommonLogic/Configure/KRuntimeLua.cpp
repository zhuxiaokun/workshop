#include "KRuntimeLua.h"
#include <System/Misc/rand.h>
#include <System/Memory/KBuffer64k.h>
#include <System/File/KFile.h>

using namespace ::System::Collections;
using namespace ::System::File;

static System::Misc::CRandom m_rand;

class JgDefaultFilter : public KFileFilter
{
public:
	BOOL Accept(const KFileData& fileData)
	{
		return fileData.m_shortName[0] != '.';
	}
};

KRuntimeLuaFile::KRuntimeLuaFile(const char* filepath)
{
	m_lastModify = 0;
	m_lastCheckTick = 0;
	m_filepath = KBuffer64k::WriteData(filepath, strlen(filepath)+1);
}

KRuntimeLuaFile::~KRuntimeLuaFile()
{

}

DWORD KRuntimeLuaFile::GetNextActivateTick() const
{
	return m_lastCheckTick + m_rand.GetRandom(4000, 6000);
}

void KRuntimeLuaFile::OnActivate()
{
	m_lastCheckTick = GetTickCount();
	TIME_T lastModity = KFileUtil::GetLastModified(m_filepath);
	if(lastModity > m_lastModify)
	{
		m_lastModify = lastModity;
		this->loadScript();
	}
}

void KRuntimeLuaFile::Destroy()
{
	return;
}

BOOL KRuntimeLuaFile::loadScript()
{
	int nRet = LuaWraper.doScripts(m_filepath);
	return nRet > 0;
}

KRuntimeLuaDir::KRuntimeLuaDir(const char* dirname)
{
	m_lastModify = 0;
	m_lastCheckTick = 0;
	m_dirname = KBuffer64k::WriteData(dirname, strlen(dirname)+1);
}

KRuntimeLuaDir::~KRuntimeLuaDir()
{

}

DWORD KRuntimeLuaDir::GetNextActivateTick() const
{
	return m_rand.GetRandom(4000, 6000) + m_lastCheckTick;
}

void  KRuntimeLuaDir::OnActivate()
{
	m_lastCheckTick = GetTickCount();
	TIME_T lastModify = KFileUtil::GetLastModified(m_dirname);
	if(lastModify > m_lastModify)
	{
		m_lastModify = lastModify;
		this->loadScriptFiles();
	}
}

void  KRuntimeLuaDir::Destroy()
{
	return ;
}

void KRuntimeLuaDir::OnNewFile(KRuntimeLuaFile* luafile)
{
	return;
}

BOOL KRuntimeLuaDir::isExists(const char* fn)
{
	int pos = m_files.find((char*)fn);
	return pos >= 0;
}

BOOL KRuntimeLuaDir::loadScriptFiles()
{
	KDirectory dir;
	KFileSuffixFilter filter(".lua");

	if(!dir.Open(m_dirname))
	{
		Log(LOG_ERROR, "error: open directory '%s'", m_dirname);
		return FALSE;
	}

	KDirectory::FileList fl;
	if(!dir.ListFile(fl, &filter))
	{
		Log(LOG_ERROR, "error: list directory '%s'", m_dirname);
		return FALSE;
	}

	LuaString fullpath;
	for(int i=0; i<fl.size(); i++)
	{
		KFileData& fd = fl[i];
		if(this->isExists(fd.m_shortName))
			continue;

		char* fn = KBuffer64k::WriteData(fd.m_shortName, strlen(fd.m_shortName)+1);
		m_files.insert_unique(fn);

		fullpath = m_dirname;
		fullpath.append('/');
		fullpath.append(fd.m_shortName);

		KRuntimeLuaFile* pfl = new KRuntimeLuaFile(fullpath.c_str());

		pfl->OnActivate();
		this->OnNewFile(pfl);
	}

	return TRUE;
}

// KRuntimeFile

KRuntimeFile::KRuntimeFile(const char* filename)
	: m_lastModified(0)
	, m_nextCheckTick(0)
	, m_fullpath(filename)
	, m_pObserver(NULL)
{
	
}

KRuntimeFile::KRuntimeFile(const char* filename, IFileObserver& observer)
	: m_lastModified(0)
	, m_nextCheckTick(0)
	, m_fullpath(filename)
	, m_pObserver(&observer)
{

}

KRuntimeFile::~KRuntimeFile()
{

}

DWORD KRuntimeFile::GetNextActivateTick() const
{
	return m_nextCheckTick;
}

void  KRuntimeFile::OnActivate()
{
	m_nextCheckTick = GetTickCount() + 10*1000;

	if(KFileUtil::IsFileExists(m_fullpath.c_str()))
	{
		TIME_T lastModified = KFileUtil::GetLastModified(m_fullpath.c_str());
		if(lastModified > m_lastModified)
		{
			m_lastModified = lastModified;
			if(m_pObserver)
			{
				m_pObserver->on_file_changed(m_fullpath.c_str());
			}
		}
	}
	else
	{
		if(m_lastModified)
		{
			if(m_pObserver)
			{
				m_pObserver->on_file_deleted(m_fullpath.c_str());
			}
		}
	}
}

void  KRuntimeFile::Destroy()
{

}

// KRuntimeDir

KRuntimeDir::KRuntimeDir(const char* dir)
	: m_pfilter(NULL)
	, m_pObserver(NULL)
	, m_lastModified(0)
	, m_nextCheckTick(0)
{
	char fpath[512];
	getfullpath(fpath, dir, sizeof(fpath));
	m_fullpath = fpath;
}

KRuntimeDir::KRuntimeDir(const char* dir, IFileObserver& observer)
	: m_pfilter(NULL)
	, m_pObserver(&observer)
	, m_lastModified(0)
	, m_nextCheckTick(0)
{
	char fpath[512];
	getfullpath(fpath, dir, sizeof(fpath));
	m_fullpath = fpath;
}

KRuntimeDir::KRuntimeDir(const char* dir, KFileFilter& filter)
	: m_pfilter(&filter)
	, m_pObserver(NULL)
	, m_lastModified(0)
	, m_nextCheckTick(0)
{
	char fpath[512];
	getfullpath(fpath, dir, sizeof(fpath));
	m_fullpath = fpath;
}

KRuntimeDir::KRuntimeDir(const char* dir, KFileFilter& filter, IFileObserver& observer)
	: m_pfilter(&filter)
	, m_pObserver(&observer)
	, m_lastModified(0)
	, m_nextCheckTick(0)
{
	char fpath[512];
	getfullpath(fpath, dir, sizeof(fpath));
	m_fullpath = fpath;
}

KRuntimeDir::~KRuntimeDir()
{

}

DWORD KRuntimeDir::GetNextActivateTick() const
{
	return m_nextCheckTick;
}

void  KRuntimeDir::OnActivate()
{
	m_nextCheckTick = GetTickCount() + 10*1000;

	if(!m_dirArray.size())
	{
		FileTrace ft;
		
		ft.fullpath = m_fullpath;
		ft.lastModified = 0;

		m_dirArray.insert_unique(ft);

		this->loopDirRecursive(m_fullpath.c_str());
	}

	// ÏÈ¼ì²éÄ¿Â¼
	int c = m_dirArray.size();
	for(int i=c-1; i >= 0; i--)
	{
		FileTrace& fileTrace = m_dirArray[i];
		const char* dir = fileTrace.fullpath.c_str();
		if(!KFileUtil::IsFileExists(dir))
		{
			this->removeDir(dir);
		}
		else
		{
			TIME_T lastModified = KFileUtil::GetLastModified(fileTrace.fullpath.c_str());
			if(lastModified > fileTrace.lastModified)
			{
				fileTrace.lastModified = lastModified;
				this->loopDir(fileTrace.fullpath.c_str());
			}
		}
	}

	c = m_fileArray.size();
	for(int i=c-1; i>=0; i--)
	{
		FileTrace& fileTrace = m_fileArray[i];
		const char* file = fileTrace.fullpath.c_str();
		if(!KFileUtil::IsFileExists(file))
		{
			this->removeFile(file);
		}
		else
		{
			TIME_T lastModified = KFileUtil::GetLastModified(file);
			if(lastModified > fileTrace.lastModified)
			{
				fileTrace.lastModified = lastModified;
				if(m_pObserver)
				{
					m_pObserver->on_file_changed(file);
				}
			}
		}
	}
}

void  KRuntimeDir::Destroy()
{

}

void KRuntimeDir::loopDir(const char* fullpath)
{
	m_pendings.clear();
	this->_loopDir(fullpath);
	m_pendings.clear();
}

void KRuntimeDir::loopDirRecursive(const char* fullpath)
{
	m_pendings.clear();
	m_pendings.push_back(KString<512>(fullpath));
	while(m_pendings.size())
	{
		KString<512> strPath = m_pendings[0];
		m_pendings.erase(0);
		this->_loopDir(strPath.c_str());
	}
	m_pendings.clear();
}

void KRuntimeDir::_loopDir(const char* fullpath)
{
	KDirectory dir;
	JgDefaultFilter filter;

	KFileFilter* pfilter = m_pfilter;
	if(!pfilter) pfilter = &filter;

	if(!dir.Open(fullpath))
	{
		Log(LOG_ERROR, "error: open directory '%s'", fullpath);
		return;
	}

	m_flist.clear();
	if(!dir.ListFile(m_flist, pfilter))
	{
		Log(LOG_ERROR, "error: list directory '%s'", fullpath);
		return;
	}

	char fpath[512];
	LuaString strPath;

	for(int i=0; i<m_flist.size(); i++)
	{
		KFileData& fd = m_flist[i];

		strPath = fullpath;
		strPath.append('/');
		strPath.append(fd.m_shortName);
		getfullpath(fpath, strPath.c_str(), sizeof(fpath));
		strPath = fpath;

		if(KFileUtil::IsDir(strPath.c_str()))
		{
			m_pendings.push_back(strPath);
			if(!this->getDirTrace(strPath.c_str()))
			{
				this->addDir(strPath.c_str(), 0);
			}
		}
		else
		{
			if(!this->getFileTrace(strPath.c_str()))
			{
				this->addFile(strPath.c_str(), 0);
			}
		}
	}
}

void KRuntimeDir::addFile(const char* fullpath, TIME_T lastModified)
{
	if(!lastModified)
	{
		if(m_pObserver)
		{
			m_pObserver->on_new_file(fullpath);
		}
		lastModified = KFileUtil::GetLastModified(fullpath);
	}

	FileTrace fileTrace;
	fileTrace.fullpath = fullpath;
	fileTrace.lastModified = lastModified;
	m_fileArray.insert_unique(fileTrace);
}

void KRuntimeDir::removeFile(const char* fullpath)
{
	if(m_pObserver)
	{
		m_pObserver->on_file_deleted(fullpath);
	}

	FileTrace fileTrace;
	fileTrace.fullpath = fullpath;
	fileTrace.lastModified = 0;
	int pos = m_fileArray.find(fileTrace);

	if(pos >= 0)
	{
		m_fileArray.erase(pos);
	}
}

void KRuntimeDir::addDir(const char* fullpath, TIME_T lastModified)
{
	FileTrace fileTrace;
	fileTrace.fullpath = fullpath;
	fileTrace.lastModified = lastModified;
	m_dirArray.insert_unique(fileTrace);

	if(m_pObserver)
	{
		m_pObserver->on_new_file(fullpath);
	}
}

void KRuntimeDir::removeDir(const char* fullpath)
{
	if(m_pObserver)
	{
		m_pObserver->on_file_deleted(fullpath);
	}

	int c = m_fileArray.size();
	for(int i=c-1; i>=0; i--)
	{
		FileTrace& fileTrace = m_fileArray[i];
		const char* p = strstr(fileTrace.fullpath.c_str(), fullpath);
		if(p == fileTrace.fullpath.c_str())
		{
			if(m_pObserver)
			{
				m_pObserver->on_file_deleted(fileTrace.fullpath.c_str());
			}
			m_fileArray.erase(i);
		}
	}

	FileTrace ft;
	ft.fullpath = fullpath;
	ft.lastModified = 0;
	int pos = m_dirArray.find(ft);

	if(pos >= 0)
	{
		m_dirArray.erase(pos);
	}
}

KRuntimeDir::FileTrace* KRuntimeDir::getDirTrace(const char* fullpath)
{
	FileTrace ft;

	ft.fullpath = fullpath;
	ft.lastModified = 0;
	int pos = m_dirArray.find(ft);

	if(pos < 0) return NULL;
	return &m_dirArray[pos];
}

KRuntimeDir::FileTrace* KRuntimeDir::getFileTrace(const char* fullpath)
{
	FileTrace ft;
	ft.fullpath = fullpath;
	ft.lastModified = 0;
	int pos = m_fileArray.find(ft);

	if(pos < 0) return NULL;
	return &m_fileArray[pos];
}
