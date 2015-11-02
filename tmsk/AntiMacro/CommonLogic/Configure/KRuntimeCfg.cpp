#include "KRuntimeCfg.h"
#include <System/File/KFile.h>

using namespace System::File;

KRuntimeCfg::KRuntimeCfg()
{
	m_lastCheck = 0;
	m_lastModify = 0;
	m_pObv = NULL;
}

KRuntimeCfg::~KRuntimeCfg()
{

}

BOOL KRuntimeCfg::Initialize(const char* path, KIniObserver* pObv)
{
	if(!KFileUtil::IsFileExists(path))
		return FALSE;

	m_path = path;
	m_pObv = pObv;

	m_lastCheck = GetTickCount();
	m_lastModify = KFileUtil::GetLastModified(path);

	if(m_pObv)
	{
		KIniFileReader reader(m_path.c_str());
		m_pObv->OnIniUpdate(reader);
	}

	return TRUE;
}

DWORD KRuntimeCfg::GetNextActivateTick() const
{
	return m_lastCheck + 5000;
}

void KRuntimeCfg::OnActivate()
{
	m_lastCheck = GetTickCount();
	TIME_T lastModify = KFileUtil::GetLastModified(m_path.c_str());
	
	if(lastModify <= m_lastModify) return;
	m_lastModify = lastModify;

	if(m_pObv)
	{
		KIniFileReader reader(m_path.c_str());
		m_pObv->OnIniUpdate(reader);
	}
}

void KRuntimeCfg::Destroy()
{
	return;
}
