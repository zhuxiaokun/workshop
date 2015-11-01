#include "KConstStringManager.h"

KConstStringManager::KConstStringManager():m_buff(NULL)
{

}

KConstStringManager::~KConstStringManager()
{

}

KConstStringManager& KConstStringManager::GetInstance()
{
	static KConstStringManager inst;
	return inst;
}

const char* KConstStringManager::getConstString(const char* str) const
{
	Table::const_iterator it = m_table.find(str);
	if(it == m_table.end()) return NULL;
	return it->first;
}

const char* KConstStringManager::getConstStringForce(const char* str)
{
	Table::iterator it = m_table.find(str);
	if(it != m_table.end()) return it->first;

	size_t l = strlen(str) + 1;
	if(!m_buff || m_buff->WriteAvail() < (int_r)l)
	{
		m_buff = new KBuffer64k();
		m_buff->Reset();
	}
	
	const char* ret = m_buff->GetWriteBuffer();
	m_buff->Write(str, l);
	m_table[ret] = true;

	return ret;
}