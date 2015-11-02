#ifndef _K_TABFILE_LOADER_H_
#define _K_TABFILE_LOADER_H_

#include <System/File/KTabFile.h>

class KTabfileLoader
{
public:
	virtual ~KTabfileLoader() { }

	// filename
	// �������ˣ������� Settings �Լ��ϲ��·��
	// �ͻ��ˣ������� data/configs �Լ��ϲ��·��
	virtual ::System::File::KTabFile2* GetFileReader(const char* filename) = 0;

	// �ͻ��˶���
	virtual ::System::File::KTabFile2* GetFileReader(const char* fileName, const char* resGrpName);

	// �ͷ� reader ʹ�õ���Դ
	virtual void CloseFileReader(::System::File::KTabFile2* pReader) = 0;

	// �е�Tabfile����Ҫ������������ʱ��ѯ�ģ�ʹ������ӿ�
	virtual ::System::File::KTabFile* CreateTabfile(const char* filename) = 0;

	// ���ٴ� CreateTabfile ��õĶ���
	virtual void DestroyTabfile(::System::File::KTabFile* p) = 0;

	virtual ::System::Collections::KString<512> GetBaseFilepath(const char* filename) = 0;
	virtual ::System::Collections::KString<512> GetPatchFilepath(const char* filename) = 0;

public:
	static KTabfileLoader& GetInstance();

protected:
	static KTabfileLoader* m_pInstance;
};

#endif
