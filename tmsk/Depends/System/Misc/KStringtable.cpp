#include "KStringtable.h"
#include "../File/KTabFile.h"
#include "../File/KTextReader.h"

// KStringTable
KStringtable::KStringtable():m_buffer(NULL)
{

}

KStringtable::~KStringtable()
{
	this->reset();
}

bool KStringtable::load(StreamInterface& si)
{
	char _buf[32];
	_KString str;
	::System::File::KColumnReader r;
	if(!r.SetInputStream(si)) return false;

	char** cols;
	while(TRUE)
	{
		int n = r.ReadLine(cols, '\t');
		if(n < 0) break;
		if(!n) continue; if(n != 2) break;
		const char* key = cols[0];

		// "0" -> "0(12001)"
		char* _cols = cols[1];
#if 0	// 查 0 id的来源
		if (_cols[0] == '0' && _cols[1] == 0)
		{
			_cols = _buf;
			::sprintf_k(_buf, sizeof(_buf), "%s(%s)", cols[1], key);
		}
#endif
		const char* val = this->translateSourceString(_cols, str);
		if(!isnumber3(key)) continue;
		m_map[str2int(key)] = this->allocString(val, str.size());
	}
	
	return true;
}

void KStringtable::reset()
{
	while(!m_bufferList.empty())
	{
		Buffer* p = m_bufferList.pop_front();
		delete p;
	}
	m_buffer = NULL;
	m_map.clear();
}

const char* KStringtable::getString(int id) const
{
	Map::const_iterator it = m_map.find(id);
	if(it == m_map.end()) return NULL;
	return it->second;
}

const char* KStringtable::getString2(int id, const char* defVal) const
{
	const char* str = this->getString(id);
	return str ? str : defVal;
}

bool KStringtable::foreach(IVisitor& v) const
{
	int n = m_map.size();
	for(int i=0; i<n; i++)
	{
		Map::const_iterator it = m_map.at(i);
		if(!v.visit(it->first, it->second))
			return false;
	}
	return true;
}

bool KStringtable::patch(KDiffFileReader& diff)
{
	_KString str;
	// new lines
	int n = diff.m_newBodyLines.size();
	for(int i=0; i<n; i++)
	{
		KBodyLineDiff& lineDiff = diff.m_newBodyLines[i];
		if(lineDiff.colCount != 2) continue;
		const char* key = lineDiff.pDiffs[0].colVal;
		const char* val = this->translateSourceString(lineDiff.pDiffs[1].colVal, str);
		if(!isnumber3(key)) continue;
		m_map[str2int(key)] = this->allocString(val, str.size());
	}

	// update
	n = diff.m_bodyDiff.size();
	for(int i=0; i<n; i++)
	{
		KBodyLineDiff& lineDiff = diff.m_bodyDiff[i];
		const char* key = lineDiff.keyVal;
		
		if(!isnumber3(key)) continue;
		int id = str2int(key);

		if(lineDiff.op == enum_Line_Delete)
		{
			this->setString(id, NULL);
			continue;
		}

		if(lineDiff.op != enum_Line_Update) continue;
		if(lineDiff.colCount != 1) continue;
		const char* val = this->translateSourceString(lineDiff.pDiffs[0].colVal, str);
		
		this->setString(str2int(key), val);
	}
	
	return true;
}

bool KStringtable::setString(int id, const char* val)
{
	Map::iterator it = m_map.find(id);
	if(!val)
	{	
		if(it != m_map.end()) m_map.erase(it);
		return true;
	}
	if(it == m_map.end() || strcmp(it->second, val))
	{
		m_map[id] = this->allocString(val, strlen(val));
	}
	return true;
}

const char* KStringtable::allocString(const char* str, size_t len)
{
	if(!m_buffer || m_buffer->WriteAvail() < (int_r)len+1)
	{
		m_buffer = new Buffer();
		m_buffer->Reset();
		m_bufferList.push_back(m_buffer);
	}
	const char* p = m_buffer->GetWriteBuffer();
	m_buffer->Write(str, len+1);
	return p;
}

const char* KStringtable::translateSourceString(const char* s, KStringtable::_KString& str)
{
	char ch1, ch2, ch3;
	int p1 = 0, p2, p3;

	str.clear();
	while(ch1 = s[p1], ch1)
	{
		p2 = p1 + 1;
		p3 = p1 + 2;

		if(ch1 == '#')
		{
			ch2 = s[p2];
			if(ch2 == '\\')
			{
				ch3 = s[p3];
				if(ch3 == 'n' || ch3 == 't')
				{
					str.append(&s[p1], 3);
					p1 += 3;
					continue;
				}
			}
		}
		else if(ch1 == '\\')
		{
			ch2 = s[p2];
			if(ch2 == 'r')
			{
				str.append('\r');
				p1 += 2;
				continue;
			}
			else if(ch2 == 'n')
			{
				str.append('\n');
				p1 += 2;
				continue;
			}
			else if(ch2 == 't')
			{
				str.append('\t');
				p1 += 2;
				continue;
			}
			else if(ch2 == 'b')
			{
				str.append('\b');
				p1 += 2;
				continue;
			}

			str.append(ch2);
			p1 += 2;
			continue;
		}
		//else if(ch1 == '%')
		//{
		//	ch2 = s[p2];
		//	if(ch2 == '%')
		//	{
		//		str.append('%');
		//		p1 += 2;
		//		continue;
		//	}
		//}

		str.append(ch1);
		p1 += 1;
	}

	return str.c_str();
}

/// KStringtableManager

KStringtableManager::KStringtableManager()
{
	memset(&m_tables, 0, sizeof(m_tables));
}

KStringtableManager::~KStringtableManager()
{

}

const KStringtable* KStringtableManager::getByLang(int lang) const
{
	KLanguage language;
	if(!language.isValidLanguage(lang)) return NULL;
	return m_tables[lang];
}

KStringtable* KStringtableManager::_getByLang(int lang)
{
	KLanguage language;
	if(!language.isValidLanguage(lang)) return NULL;
	KStringtable*& table = m_tables[lang];
	if(!table) table = new KStringtable();
	return table;
}

KStringtable* KStringtableManager::_getByLangName(const char* langName)
{
	KLanguage language;
	int lang = language.languageFromName(langName);
	return this->_getByLang(lang);
}