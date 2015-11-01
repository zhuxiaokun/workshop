#include <System/File/KTabfileLoader.h>
#include "../KGlobalFunction.h"
#include <System/Memory/KBuffer64k.h>
#include <System/Misc/StrUtil.h>
#include "KMsgTanslate.h"
#include "KStringManager.h"
#include <System/Charset/KCharset.h>
#include <System/Memory/KDataBuffer.h>

//////////////////////////////////////////////////////////////////////////
//KStringCreate

KStringManager* g_strMgr = NULL;

KStringManager::KStringManager()
{
	g_strMgr = this;
	KLanguage language;
	m_language = language.getCurrentLanguage();
	IStringTranslator::SetGlobalTranslator(this);
}

KStringManager::~KStringManager()
{

}

const char* KStringManager::Translate(const char* str)
{
	if(!str) return NULL;
	if(str[0] == 's' && str[1] == ':')
	{
		const char* pID = str + 2;
		if(isnumber3(pID))
		{
			int strID = str2int(pID);
			const char* s = this->getString(strID, NULL);
			return s ? s : str;
		}
	}
	return str;
}

bool KStringManager::loadAll(const char* path)
{
	JG_F::KDirectory dir;
	JG_F::KDirectory::FileList files;
	if(!dir.Open(path)) return false;
	if(!dir.ListFile(files)) return false;
	
	KLanguage language;
	int n = files.size();
	for(int i=0; i<n; i++)
	{
		JG_F::KFileData& fdata = files[i];
		if(!fdata.IsDir()) continue;
		int lang = language.languageFromName(fdata.m_shortName);
		if(!lang) continue;
		char filename[512];
		sprintf(filename, "%s%c%s%cList.txt", path, PATH_SEPERATOR, fdata.m_shortName, PATH_SEPERATOR);
		this->loadFileList(lang, filename);
	}

	return true;
}

bool KStringManager::loadFileList(int lang, const char* listfile)
{
	char buf[512];
	char buf2[512];

	KLanguage language;
	if(!language.isValidLanguage(lang)) return false;
	
	const char* dir = JG_F::KFileUtil::dirname_r(listfile, buf);

	JG_F::KColumnFileReader r(listfile);
	KStringtable* stable = m_strTableManager._getByLang(lang);

	char** cols; int line = 0;
	while(TRUE)
	{
		int n = r.ReadLine(cols, '\t');
		if(n == 0) continue; if(n < 0) break;
		if(!line++) continue;
		const char* fn = cols[0];
		sprintf_k(buf2, sizeof(buf2), "%s%c%s", dir, PATH_SEPERATOR, fn);

		KInputFileStream fi(buf2);
		if(!fi.Good())
		{
			DWORD errCode = GetLastError();
			Log(LOG_ERROR, "error: open %s, %s", fn, strerror_r2(errCode).c_str());
			continue;
		}
		stable->load(fi);
	}

	return true;
}

bool KStringManager::loadForLanguage(const char* path, int lang)
{
	JG_F::KDirectory dir;
	JG_F::KDirectory::FileList files;
	if(!dir.Open(path)) return false;
	if(!dir.ListFile(files)) return false;

	KLanguage language;
	int n = files.size();
	for(int i=0; i<n; i++)
	{
		JG_F::KFileData& fdata = files[i];
		if(!fdata.IsDir()) continue;
		int l2 = language.languageFromName(fdata.m_shortName);
		if(l2 == lang)
		{
			char filename[512];
			sprintf(filename, "%s%c%s%cList.txt", path, PATH_SEPERATOR, fdata.m_shortName, PATH_SEPERATOR);
			this->loadFileList(lang, filename);
			return true;
		}
	}
	return false;
}

bool KStringManager::loadLocalLang(const char* path)
{
	char buf[512];
	if(!m_language) return false;
	const char* pattern = "StringManager";
	int pos = stristr(path, pattern);
	if(pos >= 0)
	{
		strcpy(buf, path); path = &buf[0];
		((char*)path)[pos+strlen(pattern)] = '\0';
	}
	return this->loadForLanguage(path, m_language);
}

bool KStringManager::patch(int lang, StreamInterface& si)
{
	KLanguage language;
	if(!language.isValidLanguage(lang)) return false;
	KDiffFileReader diff;
	if(!diff.Read(si)) return false;
	KStringtable* stable = m_strTableManager._getByLang(lang);
	return stable->patch(diff);
}

BOOL KStringManager::Init(const char* filename)
{
	int lang = this->_getLangFromFilename(filename);
	if(lang)
	{
		if(!JG_F::KFileUtil::IsDir(filename))
			return this->loadFileList(lang, filename);
		JG_C::KString<512> resPath = this->_getResourceDir(filename);
		return this->loadForLanguage(resPath.c_str(), lang);
	}
	else
	{
		if(!JG_F::KFileUtil::IsDir(filename))
			return this->loadFileList(m_language, filename);
		JG_C::KString<512> resPath = this->_getResourceDir(filename);
		return this->loadAll(resPath.c_str());
	}
}

BOOL KStringManager::InitInvalidString(bool checkName)
{
	const char* filename = "StringManager/IllegalChars.txt";
	KTabfileLoader* loader = &KTabfileLoader::GetInstance();
	StreamInterface* fi = loader->GetFileStream(filename, "General");
	if(!fi) return FALSE;
	KCharset* cs = KCharset::getLocalCharset();
	BOOL bRet = cs->loadIllegalWords(*fi);
	loader->CloseFileStream(fi);
	return bRet;
}

BOOL KStringManager::InitIllegalWords(const char* filename)
{
	char path[512];

	if(!JG_F::KFileUtil::IsDir(filename))
		return this->_loadIllegalWords(m_language, filename);

	int lang = this->_getLangFromFilename(filename);
	if(!lang)
	{
		int len = sprintf_k(path, sizeof(path), "%s%cIllegalChars.txt", filename, PATH_SEPERATOR);
		if(JG_F::KFileUtil::IsFileExists(path))
			return this->_loadIllegalWords(m_language, path);
	}

	JG_F::KDirectory dir;
	JG_F::KDirectory::FileList files;

	if(!dir.Open(path)) return FALSE;
	if(!dir.ListFile(files)) return FALSE;

	KLanguage language;
	int n = files.size();
	for(int i=0; i<n; i++)
	{
		JG_F::KFileData& fdata = files[i];
		if(!fdata.IsDir()) continue;

		int lang = language.languageFromName(fdata.m_shortName);
		if(!lang) continue;

		char path[512];
		int len = sprintf_k(path, sizeof(path), "%s%c%s%cIllegalChars.txt", filename, PATH_SEPERATOR, fdata.m_shortName, PATH_SEPERATOR);
		this->_loadIllegalWords(lang, path);
	}

	return TRUE;
}

const char* KStringManager::getString(int lang, int strID)
{
	if(!lang) lang = m_language;
	const KStringtable* tbl = m_strTableManager.getByLang(lang);
	if(!tbl) return NULL;
	return tbl->getString(strID);
}

const char* KStringManager::GetStringByID(int strID)
{
	if(!m_language) return "";
	const char* s = this->getString(m_language, strID);
	return s ? s : "";
}

const char* KStringManager::getStringByID(int lang, int strID, const char* defVal)
{
	const char* s = this->getString(lang, strID);
	return s ? s : defVal;
}

BOOL KStringManager::CheckString(int lang, const char* str)
{
	KCharset* cs = KCharset::getCharset((KLanguage::EnumLanguage)lang);
	if(!cs) return TRUE;
	return cs->checkIllegal(str) ? FALSE : TRUE;
}

BOOL KStringManager::CheckAndReplaceString(int lang, char* str, size_t length, int chr)
{
	KCharset* cs = KCharset::getCharset((KLanguage::EnumLanguage)lang);
	if(!cs) return TRUE;
	return cs->replaceIllegal(str, chr);
}

BOOL KStringManager::CheckNameValid(const char* name)
{
	return this->CheckString(m_language, name);
}

BOOL KStringManager::CheckChatValid(const char* str)
{
	return this->CheckString(m_language, str);
}

BOOL KStringManager::IsOwnerFile(const char* filename)
{
	int prefixlen = 14;
	const char* prefix = "StringManager/";
	return strncmp(filename, prefix, prefixlen) == 0;
}

JG_C::KString<512> KStringManager::_getResourceDir(const char* filename)
{
	char fpath[512];
	filename = getfullpath(fpath, filename, sizeof(fpath));
	const char* pattern = "StringManager";
	int pos = stristr(filename, pattern);
	if(pos < 0)
		return JG_C::KString<512>(filename);
	((char*)filename)[pos+strlen(pattern)] = '\0';
	return JG_C::KString<512>(filename);
}

int KStringManager::_getLangFromFilename(const char* filename)
{
	char fpath[512];
	filename = getfullpath(fpath, filename, sizeof(fpath));

	const char* pattern = "StringManager";
	int pos = stristr(filename, pattern);
	if(!pos) return 0;
	filename += pos + strlen(pattern) + 1;

	KLanguage language;
	const char* pSlash = strchr(filename, PATH_SEPERATOR);
	if(pSlash)
	{
		*(char*)pSlash = '\0';
		return language.languageFromName(filename);
	}
	else
	{
		if(!filename[0]) return 0;
		return language.languageFromName(filename);
	}
}

BOOL KStringManager::_loadIllegalWords(int lang, const char* dir)
{
	KCharset* cs = KCharset::getCharset((KLanguage::EnumLanguage)lang);
	if(!cs)
	{
		Log(LOG_ERROR, "error: 'KStringManager::_loadIllegalWords' language:%d not support", lang);
		return FALSE;
	}

	char path[512];
	int len = sprintf_k(path, sizeof(path), "%s%cIllegalChars.txt", dir, PATH_SEPERATOR);

	KInputFileStream fi(path);
	if(!fi.Good())
	{
		Log(LOG_ERROR, "error: 'KStringManager::_loadIllegalWords' file %s not found", path);
		return FALSE;
	}

	if(!cs->loadIllegalWords(fi))
	{
		Log(LOG_ERROR, "error: 'KStringManager::_loadIllegalWords' charset load illegal words.");
		return FALSE;
	}

	return TRUE;
}
