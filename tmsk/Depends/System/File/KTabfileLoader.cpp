#include "KTabfileLoader.h"

KTabfileLoader* KTabfileLoader::m_pInstance = NULL;

JG_F::KTabFile2* KTabfileLoader::GetFileReader(const char* fileName, const char* resGrpName)
{
	return NULL;
}

void KTabfileLoader::SetInstance(KTabfileLoader& loader)
{
	m_pInstance = &loader;
}

KTabfileLoader& KTabfileLoader::GetInstance()
{
	return *m_pInstance;
}