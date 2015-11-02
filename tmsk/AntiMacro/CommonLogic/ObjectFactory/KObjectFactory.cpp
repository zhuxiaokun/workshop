#include "KObjectFactory.h"

KObjectFactory::KObjectFactory()
{

}

KObjectFactory::~KObjectFactory()
{

}

KObjectFactory& KObjectFactory::GetInstance()
{
	static KObjectFactory factory;
	return factory;
}

BOOL KObjectFactory::Register(const char* className, KObjectFactory::Creator creator)
{
	::System::Sync::KAutoThreadMutex mx(m_mx);
	m_classMap[className] = creator;
	return TRUE;
}

void* KObjectFactory::Create(const char* className)
{
	::System::Sync::KAutoThreadMutex mx(m_mx);

	Iterator it = m_classMap.find(className);
	if(it == m_classMap.end()) return NULL;

	Creator creator = it->second;
	return creator();
}
