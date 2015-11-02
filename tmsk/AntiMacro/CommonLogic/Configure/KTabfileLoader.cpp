#include "KTabfileLoader.h"

KTabfileLoader* KTabfileLoader::m_pInstance = NULL;

::System::File::KTabFile2* KTabfileLoader::GetFileReader(const char* fileName, const char* resGrpName)
{
	return NULL;
}

KTabfileLoader& KTabfileLoader::GetInstance()
{
	ASSERT(m_pInstance);
	return *m_pInstance;
}
