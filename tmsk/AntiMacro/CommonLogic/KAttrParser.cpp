#include "KAttrParser.h"

KAttrParser::KAttrParser():m_buf(NULL)
{

}

KAttrParser::~KAttrParser()
{
	this->Clear();
}

bool KAttrParser::Parse(const char* attrs, char delimeter)
{
	char* ss[1024];

	this->Clear();

	KBuffer64k* buf = this->getBuffer();
	char* src = buf->WriteData(attrs, (int)strlen(attrs)+1);
	
	int ns = split(src, delimeter, ss, 1024);
	for(int i=0; i<ns; i++)
	{
		char* ss2[2];
		char* s = ss[i];
		int n = split(s, "=", ss2, 2);
		if(n < 2) continue;

		const char* name = _trim(ss2[0]);
		const char* val = n>1 ? _trim(ss2[1]) : "";

		this->Set(name, val);
	}
	return true;
}

bool KAttrParser::GetParameter(int idx, char*& name, char*& val)
{
	if(idx < 0 || idx >= m_attrs.size()) return false;
	const KAttrPair& pair = m_attrs[idx];
	name = (char*)pair.name;
	val = (char*)pair.val;
	return true;
}

bool KAttrParser::HasAttr(const char* name) const
{
	return ((KAttrParser*)this)->get(name) != NULL;
}

const char* KAttrParser::Get(const char* name) const
{
	KAttrPair* pPair = ((KAttrParser*)this)->get(name);
	if(!pPair) return NULL;
	return pPair->val;
}

const KAttrPair& KAttrParser::Get(int idx) const
{
	ASSERT(idx>=0 && idx<m_attrs.size());
	return m_attrs[idx];
}

int KAttrParser::GetInteger(const char* name, int defVal)
{
	KAttrPair* pair = this->get(name);
	if(!pair) return defVal;
	if(!isnumber3(pair->val)) return defVal;
	return str2int(pair->val);
}

const char* KAttrParser::GetString(const char* name, const char* defVal)
{
	KAttrPair* pair = this->get(name);
	if(!pair) return defVal;
	return pair->val;
}

bool KAttrParser::Set(const char* name, const char* val)
{
	KAttrPair* pPair = this->get(name);
	if(pPair)
	{
		if(val == pPair->val) return true;
		if(!val)
		{
			pPair->val = NULL;
			return true;
		}
		else
		{
			KBuffer64k* buf = this->getBuffer();
			pPair->val = buf->WriteData(val, (int)strlen(val)+1);
			return pPair->val != NULL;
		}
	}
	else
	{
		KBuffer64k* buf = this->getBuffer();

		KAttrPair pair;
		pair.name = buf->WriteData(name, (int)strlen(name)+1);
		pair.val = buf->WriteData(val, (int)strlen(val)+1);
		if(!pair.name || !pair.val) return false;
		m_attrs.insert_unique(pair);
		return true;
	}
}

bool KAttrParser::Remove(const char* name)
{
	KAttrPair pair = { name, NULL};
	int pos = m_attrs.find(pair);
	if(pos < 0) return false;
	m_attrs.erase(pos);
	return true;
}

void KAttrParser::Clear()
{
	m_attrs.clear();
	if(m_buf)
	{
		m_buf->Reset();
		KBuffer64k::Free(m_buf);
		m_buf = NULL;
	}
}

int KAttrParser::Size() const
{
	return m_attrs.size();
}

size_t KAttrParser::ToString(char* buf, size_t len) const
{
	size_t pos = 0;
	int c = this->Size();
	for(int i=0; i<c; i++)
	{
		const KAttrPair& pair = m_attrs[i];
		pos += sprintf_k(buf+pos, len-pos, "%s=%s", pair.name, pair.val?pair.val:"");
		ASSERT(pos < len);
	}
	buf[pos] = '\0';
	return pos;
}

KBuffer64k* KAttrParser::getBuffer()
{
	if(!m_buf)
	{
		m_buf = KBuffer64k::Alloc();
		m_buf->Reset();
	}
	return m_buf;
}

KAttrPair* KAttrParser::get(const char* name)
{
	KAttrPair pair = { name, NULL};
	int pos = m_attrs.find(pair);
	if(pos < 0) return NULL;
	return &m_attrs[pos];
}