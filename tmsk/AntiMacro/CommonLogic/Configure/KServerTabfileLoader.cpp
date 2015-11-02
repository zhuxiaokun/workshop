#include "KServerTabfileLoader.h"
#include <System/Log/log.h>
#include <StringManager/KStringManager.h>

using namespace System::File;
using namespace System::Collections;

KServerTabFileReader::KServerTabFileReader()
{
	ASSERT_I(KStringManager::GetInstancePtr());
}

void KServerTabFileReader::Close()
{
	m_fileReader.Clear();
}

void KServerTabFileReader::Release()
{
	this->Close();
	KServerTabFileReader::Free(this);
}

KServerTabfileLoader::KServerTabfileLoader()
{
	m_pInstance = this;
}

KServerTabfileLoader::~KServerTabfileLoader()
{

}

KServerTabfileLoader& KServerTabfileLoader::GetInstance()
{
	static KServerTabfileLoader instance;
	return instance;
}

KTabFile2* KServerTabfileLoader::GetFileReader(const char* filename)
{
	Log(LOG_FATAL, "tabfile: begin read %s by tabfile2", filename);

	KServerTabFileReader* pFileReader = KServerTabFileReader::Alloc();
	ASSERT(pFileReader); pFileReader->Close();

	KTabFile2& m_fileReader = pFileReader->m_fileReader;

	KString<512> baseFile = this->GetBaseFilepath(filename);
	KString<512> patchFile = this->GetPatchFilepath(filename);
	
	if(!KFileUtil::IsFileExists(baseFile.c_str()))
	{
		pFileReader->Release();
		return FALSE;
	}

	if(KFileUtil::IsFileExists(patchFile.c_str()))
	{
		if(!m_fileReader.Open(baseFile.c_str(), patchFile.c_str()))
		{
			pFileReader->Release();
			return NULL;
		}
	}
	else
	{
		if(!m_fileReader.Open(baseFile.c_str()))
		{
			pFileReader->Release();
			return NULL;
		}
	}

	return &m_fileReader;
}

void KServerTabfileLoader::CloseFileReader(KTabFile2* pReader)
{
	KServerTabFileReader* pFileReader = (KServerTabFileReader*)pReader;
	pFileReader->Release();
}

KTabFile* KServerTabfileLoader::CreateTabfile(const char* filename)
{
	Log(LOG_FATAL, "tabfile: begin read %s by original tabfile", filename);

	KTabFile2* pReader = this->GetFileReader(filename);
	if(!pReader) return NULL;

	KTabFile* pTabfile = new KTabFile();
	if(!pTabfile->CreateFrom(*pReader))
	{
		this->CloseFileReader(pReader);
		delete pTabfile;
		return NULL;
	}

	this->CloseFileReader(pReader);
	return pTabfile;
}

void KServerTabfileLoader::DestroyTabfile(KTabFile* p)
{
	if(!p) return;
	delete p;
}

KString<512> KServerTabfileLoader::GetBaseFilepath(const char* filename)
{
	KString<512> path("../Settings/");
	path.append(filename);
	return path;
}

KString<512> KServerTabfileLoader::GetPatchFilepath(const char* filename)
{
	KString<512> path("../Patch/Server/");
	path.append(filename);
	return path;
}

