#ifndef _K_SERVER_TABFILE_LOADER_H_
#define _K_SERVER_TABFILE_LOADER_H_

#include "KTabfileLoader.h"

class KServerTabFileReader : public System::Memory::KPortableStepPool<KServerTabFileReader,4>
{
public:
	System::File::KTabFile2 m_fileReader;

public:
	KServerTabFileReader();
	void Close();
	void Release();
};

class KServerTabfileLoader : public KTabfileLoader
{
public:
	KServerTabfileLoader();
	~KServerTabfileLoader();
	static KServerTabfileLoader& GetInstance();

public:
	// filename
	// �������ˣ������� Settings �Լ��ϲ��·��
	// �ͻ��ˣ������� data/configs �Լ��ϲ��·��
	System::File::KTabFile2* GetFileReader(const char* filename);
	void CloseFileReader(::System::File::KTabFile2* pReader);

	::System::File::KTabFile* CreateTabfile(const char* filename);
	void DestroyTabfile(::System::File::KTabFile* p);

private:
	System::Collections::KString<512> GetBaseFilepath(const char* filename);
	System::Collections::KString<512> GetPatchFilepath(const char* filename);
};

#endif
