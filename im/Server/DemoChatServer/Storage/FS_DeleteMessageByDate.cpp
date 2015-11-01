#include "FS_DeleteMessageByDate.h"
#include "../KDemoChatServerApp.h"
#include <JgErrorCode.h>
#include <System/File/KFile.h>

FS_DeleteMessageByDate::FS_DeleteMessageByDate():m_result(0),m_yyyymmdd(0)
{
	
}

FS_DeleteMessageByDate::~FS_DeleteMessageByDate()
{

}

void FS_DeleteMessageByDate::execute()
{
	int n;
	char path[512];
	JG_F::KDirectory dir;
	JG_F::KDirectory::FileList flist;

	sprintf_k(path, sizeof(path), "%s/private", g_pApp->m_storageAgent.m_dataDir.c_str());
	dir.Open(path);

	flist.clear();
	dir.ListFile(flist);

	n = flist.size();
	for(int i=0; i<n; i++)
	{
		const JG_F::KFileData& fdata = flist.at(i);
		if(!fdata._isDir()) continue;
		int n = str2int(fdata.m_shortName);
		if(!n) continue;
		if(n <= m_yyyymmdd)
		{
			JG_F::KFileData::FilePath filePath = fdata.getPath();
			JG_F::KFileUtil::rmDirRecursive(filePath.c_str());
			Log(LOG_WARN, "warn: recursive rmDir '%s'", filePath.c_str());
		}
	}

	sprintf_k(path, sizeof(path), "%s/public", g_pApp->m_storageAgent.m_dataDir.c_str());
	dir.Open(path);

	flist.clear();
	dir.ListFile(flist);

	n = flist.size();
	for(int i=0; i<n; i++)
	{
		const JG_F::KFileData& fdata = flist.at(i);
		if(!fdata._isDir()) continue;
		int n = str2int(fdata.m_shortName);
		if(!n) continue;
		if(n <= m_yyyymmdd)
		{
			JG_F::KFileData::FilePath filePath = fdata.getPath();
			JG_F::KFileUtil::rmDirRecursive(filePath.c_str());
			Log(LOG_WARN, "warn: recursive rmDir '%s'", filePath.c_str());
		}
	}

	return;
}

void FS_DeleteMessageByDate::onFinish()
{
	
}

void FS_DeleteMessageByDate::reset()
{
	m_result = 0;
	m_yyyymmdd = 0;
	KStorageTask::reset();
}

void FS_DeleteMessageByDate::destroy()
{
	this->reset();
	FS_DeleteMessageByDate::Free(this);
}
