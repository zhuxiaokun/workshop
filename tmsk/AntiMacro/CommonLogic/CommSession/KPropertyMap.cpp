#include "KPropertyMap.h"
#include <System/Misc/StrUtil.h>

int KPropertyMap::keycmp::operator () (const KPropertyMap::string_32& a, const KPropertyMap::string_32& b) const
{
	return stricmp(a.c_str(), b.c_str());
}

KPropertyMap::KPropertyMap()
{

}

KPropertyMap::KPropertyMap(const KPropertyMap& o):m_kvmap(o.m_kvmap)
{

}

KPropertyMap::~KPropertyMap()
{
	m_kvmap.clear();
}

KPropertyMap& KPropertyMap::operator = (const KPropertyMap& o)
{
	if(this == &o) return *this;
	m_kvmap = o.m_kvmap;
	return *this;
}

bool KPropertyMap::parseLine(char* propertyLine)
{
	m_kvmap.clear();
	char* ss[256];
	int n = split(propertyLine, " \t", ss, 256);
	for(int i=0; i<n; i++) this->set(ss[i]);
	return true;
}

// key value separated by ':'
bool KPropertyMap::parse(char* keyValue, char** keyp, char** valp)
{
	char* ss[2];
	int n = split(keyValue, '=', ss, 2);
	if(n != 2) return false;
	*keyp = ss[0]; *valp = ss[1];
	return true;
}

// key value separated by ':'
void KPropertyMap::set(char* keyValue)
{
	char* key; char* val;
	if(parse(keyValue, &key, &val))
	{
		m_kvmap[key] = val;
	}
}

void KPropertyMap::_set(const char* key, const char* value)
{
	m_kvmap[key] = value;
}

bool KPropertyMap::has(const char* key) const
{
	return m_kvmap.find(key) != m_kvmap.end();
}

bool KPropertyMap::empty() const
{
	return m_kvmap.empty();
}

int KPropertyMap::size() const
{
	return m_kvmap.size();
}

const char* KPropertyMap::get(const char* key) const
{
	KeyValueMap::const_iterator it = m_kvmap.find(key);
	if(it == m_kvmap.end()) return NULL;
	return it->second.c_str();
}

void KPropertyMap::clear()
{
	m_kvmap.clear();
}

bool KPropertyMap::foreachPair(LMREG::l_func func, LMREG::l_stackval ctx)
{
	KeyValueMap::iterator it = m_kvmap.begin();
	KeyValueMap::iterator it2 = m_kvmap.end();
	for(; it!=it2; it++)
	{
		const char* key = it->first;
		const char* val = it->second;
		if(func.r_invoke<bool,const char*,const char*,LMREG::l_stackval>(key, val, ctx))
			return false;
	}
	return true;
}

LongLuaString KPropertyMap::toString() const
{
	char buf[1024];
	int n = this->serialize(buf, sizeof(buf));
	return LongLuaString(buf, n);
}

void KPropertyMap::erase(const char* key)
{
	KeyValueMap::const_iterator it = m_kvmap.find(key);
	if(it == m_kvmap.end()) return;
	m_kvmap.erase(key);
}

int KPropertyMap::serialize(char* buf, int len) const
{
	int pos = 0;
	char cTmp[1024];

	KeyValueMap::const_iterator it = m_kvmap.begin();
	KeyValueMap::const_iterator ite = m_kvmap.end();
	while(it != ite)
	{
		const char* k = it->first.c_str();
		const char* v = it->second.c_str();
		it++;
		int n = sprintf_k(cTmp, sizeof(cTmp), "%s=%s", k, v);
		if(n+1 < len)
		{
			if(pos) buf[pos++] = ' ';
			memcpy(buf+pos, cTmp, n);
			pos += n;
		}
	}

	buf[pos] = '\0';
	return pos;
}
