#ifndef _K_TABFILE_LOADER_H_
#define _K_TABFILE_LOADER_H_

#include <System/File/KTabFile.h>

class KTabfileLoader
{
public:
	virtual ~KTabfileLoader() { }

	// filename
	// 服务器端，不包含 Settings 以及上层的路径
	// 客户端，不包含 data/configs 以及上层的路径
	virtual ::System::File::KTabFile2* GetFileReader(const char* filename) = 0;

	// 客户端独有
	virtual ::System::File::KTabFile2* GetFileReader(const char* fileName, const char* resGrpName);

	// 释放 reader 使用的资源
	virtual void CloseFileReader(::System::File::KTabFile2* pReader) = 0;

	// 有的Tabfile是需要保存起来，随时查询的，使用这个接口
	virtual ::System::File::KTabFile* CreateTabfile(const char* filename) = 0;

	// 销毁从 CreateTabfile 获得的对象
	virtual void DestroyTabfile(::System::File::KTabFile* p) = 0;

	virtual ::System::Collections::KString<512> GetBaseFilepath(const char* filename) = 0;
	virtual ::System::Collections::KString<512> GetPatchFilepath(const char* filename) = 0;

public:
	static KTabfileLoader& GetInstance();

protected:
	static KTabfileLoader* m_pInstance;
};

#endif
