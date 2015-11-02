#ifndef _K_TABFILE_LOADER_H_
#define _K_TABFILE_LOADER_H_

#include "KTabFile.h"

class KTabfileLoader
{
public:
	virtual ~KTabfileLoader() { }

	// filename
	// 服务器端，不包含 Settings 以及上层的路径
	// 客户端，不包含 data/configs 以及上层的路径
	virtual JG_F::KTabFile2* GetFileReader(const char* filename) = 0;

	// 客户端独有
	virtual JG_F::KTabFile2* GetFileReader(const char* fileName, const char* resGrpName);

	// 释放 reader 使用的资源
	virtual void CloseFileReader(JG_F::KTabFile2* pReader) = 0;

	// 有的Tabfile是需要保存起来，随时查询的，使用这个接口
	virtual JG_F::KTabFile* CreateTabfile(const char* filename) = 0;

	// 销毁从 CreateTabfile 获得的对象
	virtual void DestroyTabfile(JG_F::KTabFile* p) = 0;

	virtual JG_C::KString<512> GetBaseFilepath(const char* filename) = 0;
	virtual JG_C::KString<512> GetPatchFilepath(const char* filename) = 0;

	virtual StreamInterface* GetFileStream(const char* filename, const char* grp) { return NULL; }
	virtual void CloseFileStream(StreamInterface* stream)  { return; }

public:
	static void SetInstance(KTabfileLoader& loader);
	static KTabfileLoader& GetInstance();

private:
	static KTabfileLoader* m_pInstance;
};

#endif