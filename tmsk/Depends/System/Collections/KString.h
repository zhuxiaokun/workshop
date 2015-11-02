/********************************************************************
	created:	2007/11/16
	created:	16:11:2007   11:51
	filename: 	System\Collections\KString.h
	file path:	System\Collections
	file base:	KString
	file ext:	h
	author:		xueyan
	
	purpose:	用模版实现一个字符串类
*********************************************************************/

#ifndef	SystemCollectionsKString
#define	SystemCollectionsKString

#pragma once

#include "../KMacro.h"
#include "../KType.h"

#ifndef WIN32
#include <stdarg.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../KPlatform.h"

namespace System { namespace Collections {

#pragma pack(push,4)
template<size_t total> class KString
{
public:
	KString():m_size(0)
	{
		m_buffer[0] = 0;
	}
	KString(const KString& o):m_size(o.m_size)
	{
		if(m_size)
		{
			::memcpy(m_buffer, o.m_buffer, m_size);
		}
		m_buffer[m_size] = 0;
	}
	KString(const char * s):m_size(0)
	{
		if(!s)
		{
			m_buffer[0] = 0;
			return;
		}
		while(*s && m_size < total-1)
		{
			m_buffer[m_size++] = *s;
			s += 1;
		}
		m_buffer[m_size] = 0;
	}
	KString(const void* s, size_t len)
	{
		ASSERT(len < total);
		memcpy(&m_buffer, s, len);
		m_buffer[len] = '\0';
		m_size = (int)len;
	}
	~KString()
	{
	}
	size_t capacity() const
	{
		return total;
	}
	KString& operator = (const KString& o)
	{
		if(this == &o) return *this;
		m_size = o.m_size;
		if(m_size)
		{
			::memcpy(m_buffer, o.m_buffer, m_size);
		}
		m_buffer[m_size] = 0;
		return *this;
	}
	KString& operator = (const char* str)
	{
		this->clear();
		if(!str) return *this;
		while(*str && m_size < total-1)
		{
			m_buffer[m_size++] = *str;
			str += 1;
		}
		m_buffer[m_size] = 0;
		return *this;
	}
	KString& assign(const char* str, size_t len)
	{
		if(!str) return *this;
		
		if(len >= total)
		{
			ASSERT(FALSE);
			len = total;
		}

		memcpy(m_buffer, str, len);
		m_buffer[len] = 0;
		m_size = (int)len;
		return *this;
	}
	KString& append(const char* str)
	{
		if(!str) return *this;
		while(m_size < total-1 && *str)
		{
			m_buffer[m_size] = *str;
			m_size += 1;
			str += 1;
		}
		m_buffer[m_size] = 0;
		return *this;
	}
	KString& append(char c)
	{
		if(m_size < total-1)
		{
			m_buffer[m_size++] = c;
			m_buffer[m_size] = 0;
		}
		return *this;
	}
	KString& append(int n)
	{
		char cTmp[32];
		::sprintf_k(cTmp, sizeof(cTmp), "%d", n);
		return this->append(cTmp);
	}
	KString& append(unsigned int n)
	{
		char cTmp[32];
		::sprintf_k(cTmp, sizeof(cTmp), "%u", n);
		return this->append(cTmp);
	}
	KString& append(long long n)
	{
		char cTmp[64];
		::sprintf_k(cTmp, sizeof(cTmp), "%lld", n);
		return this->append(cTmp);
	}
	KString& append(unsigned long long n)
	{
		char cTmp[64];
		::sprintf_k(cTmp, sizeof(cTmp), "%llu", n);
		return this->append(cTmp);
	}
	KString& append(float f)
	{
		char cTmp[64];
		::sprintf_k(cTmp, sizeof(cTmp), "%f", f);
		return this->append(cTmp);
	}
	KString& append(double f)
	{
		char cTmp[64];
		::sprintf_k(cTmp, sizeof(cTmp), "%f", f);
		return this->append(cTmp);
	}

	KString& append(const void *p, size_t size)
	{
		ASSERT( size < (int)(total - m_size));
		memcpy(m_buffer + m_size, p, size);
		m_size += (int)size;
		m_buffer[m_size] = '\0';
		return *this;
	}
	size_t size() const
	{
		return m_size;
	}
	BOOL empty() const
	{
		return m_size < 1;
	}
	KString& clear()
	{
		m_size = 0;
		m_buffer[m_size] = 0;
		return *this;
	}
	char* c_str()
	{
		return &m_buffer[0];
	}
	const char* c_str() const
	{
		return &m_buffer[0];
	}
	size_t GetCount() const
	{
		return m_size;
	}
	KString& Printf( const char * fmt, ... )
	{
		ASSERT(fmt);
		va_list args;
		va_start( args, fmt );
		this->VPrintf( fmt, args );
		va_end( args );

		return (* this);
	}
	KString& VPrintf( const char * fmt, va_list args )
	{
		ASSERT(fmt);
#pragma warning(disable:4996)
		int len = vsnprintf( m_buffer, total-1, fmt, args );
#pragma warning(default:4996)
		ASSERT(len >= (int)-1);

		if(len == -1) m_size = total - 1; // 超长
		else m_size = len;

		m_buffer[m_size] = 0;
		return *this;
	}
	operator char* ()
	{
		return &m_buffer[0];
	}
	operator const char* () const
	{
		return &m_buffer[0];
	}
	CHAR* GetCSTR()
	{
		return &m_buffer[0];
	}
	const char* GetCSTR() const
	{
		return &m_buffer[0];
	}
	char& operator [] (int_r index)
	{
		ASSERT(index >= 0 && index < total-1);
		if(index >= (int_r)m_size)
		{
			for(int_r i=m_size; i<=index; i++)
			{
				m_buffer[i] = _type_default_value_<char>::get();
			}
			m_size = (int)(index + 1);
			m_buffer[m_size] = 0;
		}
		return m_buffer[index];
	}
	const char& operator [] (int_r index) const
	{
		ASSERT(index >= 0 && index < m_size);
		return m_buffer[index];
	}
	int compare(const char* str) const
	{
		if(!str)
		{
			if(this->empty()) return 0;
			else return 1;
		}
		return strcmp(m_buffer, str);
	}
	int icompare(const char* str) const
	{
		if(!str)
		{
			if(this->empty()) return 0;
			else return 1;
		}
		return _stricmp(m_buffer, str);
	}
	int compare(const KString& o) const
	{
		return strcmp(m_buffer, o.m_buffer);
	}
	int icompare(const KString& o) const
	{
		return _stricmp(m_buffer, o.m_buffer);
	}
	BOOL operator <  (const KString& o) const
	{
		return this->compare(o) < 0;
	}
	BOOL operator >  (const KString& o) const
	{
		return this->compare(o) > 0;
	}
	BOOL operator == (const KString& o) const
	{
		return this->compare(o) == 0;
	}
	BOOL operator != (const KString& o) const
	{
		return this->compare(o) != 0;
	}
	BOOL operator <= (const KString& o) const
	{
		return this->compare(o) <= 0;
	}
	BOOL operator >= (const KString& o) const
	{
		return this->compare(o) >= 0;
	}
	KString& TrimLeft()
	{
		size_t i = 0;
		while(i < m_size && KString::isspace(m_buffer[i])) i++;
		if(i == 0)
		{
			return *this;
		}
		if(i == m_size)
		{
			m_size = 0;
			m_buffer[m_size] = 0;
			return *this;
		}
		size_t len = (m_size - i) * sizeof(CHAR);
		CHAR* pSrc = &m_buffer[i];
		CHAR* pDst = &m_buffer[0];
		::memmove(pDst, pSrc, len);

		return *this;
	}
	KString& TrimRight()
	{
		size_t i;
		if(this->empty()) return *this;

		i = m_size - 1;
		while(i >= 0 && KString::isspace(m_buffer[i])) i -= 1;
		if(i < 0)
		{
			m_size = 0;
			m_buffer[m_size] = 0;
		}
		else if(i != m_size - 1)
		{
			m_size = i + 1;
			m_buffer[m_size] = 0;
		}
		return *this;
	}
	KString& trim()
	{
		return this->TrimRight().TrimLeft();
	}

public:
	static BOOL isspace(CHAR c)
	{
		return	c == char(' ') || c == char('\t') || c == char('\r') || c == char('\n');
	}

private:
	int m_size;
	char m_buffer[total];
};
#pragma pack(pop)

template <size_t initCapacity=32> class KDString
{
public:
	size_t m_capacity;
	size_t m_size;
	char* m_data;

public:
	KDString():m_capacity(0),m_size(0),m_data(NULL)
	{
		this->resize(initCapacity);
		m_data[m_size] = 0;
	}
	KDString(const char* str):m_capacity(0),m_size(0),m_data(NULL)
	{
		size_t len = strlen(str);
		this->resize(len + 1);
		memcpy(m_data, str, len);
		m_size = len;
		m_data[m_size] = '\0';
	}
	KDString(const char* s, size_t len):m_capacity(0),m_size(0),m_data(NULL)
	{
		this->resize(len + 1);
		::memcpy(m_data, s, len);
		m_size = len;
		m_data[m_size] = '\0';
	}
	KDString(const KDString& o):m_capacity(0),m_size(0),m_data(NULL)
	{
		this->resize(o.m_capacity);
		if(!o.empty())
		{
			memcpy(m_data, o.m_data, o.m_size);
		}
		m_size = o.m_size;
		m_data[m_size] = '\0';
	}
	~KDString()
	{
		if(m_data)
		{
			free_k(m_data);
			m_data = NULL;
			m_capacity = 0;
			m_size = 0;
		}
	}
	void resize(size_t capacity)
	{
		if(!m_data)
		{
			if(capacity < initCapacity) capacity = initCapacity;
			m_capacity = (capacity+31) & (~31);
			m_data = (char*)malloc_k(m_capacity);
			m_data[0] = '\0';
			m_size = 0;
			return;
		}
		if (capacity > m_capacity)
		{
			m_capacity = (capacity+511) & (~511);
			m_data = (char*)realloc_k(m_data, m_capacity);
			m_data[m_size] = '\0';
		}
	}
	KDString& operator = (const KDString& o)
	{
		if(this == &o) return *this;
		if(m_capacity <= o.m_size)
		{
			this->resize(o.m_size + 1);
		}
		::memcpy(m_data, o.m_data, o.m_size);
		m_size = o.m_size;
		m_data[m_size] = '\0';
		return *this;
	}
	/*BOOL operator == (const KDString& o) const
	{
		return !strcmp(m_data, o.m_data);
	}*/
	
	int compare(const char* str) const
	{
		if(!str)
		{
			if(this->empty()) return 0;
			else return 1;
		}
		return strcmp(m_data, str);
	}
	int icompare(const char* str) const
	{
		if(!str)
		{
			if(this->empty()) return 0;
			else return 1;
		}
		return _stricmp(m_data, str);
	}
	int compare(const KDString& o) const
	{
		return strcmp(m_data, o.m_data);
	}
	int icompare(const KDString& o) const
	{
		return _stricmp(m_data, o.m_data);
	}
	BOOL operator <  (const KDString& o) const
	{
		return this->compare(o) < 0;
	}
	BOOL operator >  (const KDString& o) const
	{
		return this->compare(o) > 0;
	}
	BOOL operator == (const KDString& o) const
	{
		return this->compare(o) == 0;
	}
	BOOL operator != (const KDString& o) const
	{
		return this->compare(o) != 0;
	}
	BOOL operator <= (const KDString& o) const
	{
		return this->compare(o) <= 0;
	}
	BOOL operator >= (const KDString& o) const
	{
		return this->compare(o) >= 0;
	}


	BOOL empty() const
	{
		return m_size < 1;
	}
	size_t size() const
	{
		return m_size;
	}
	const char* c_str() const
	{
		return m_data;
	}
	KDString& append(const char* s, size_t len)
	{
		if(!s) return *this;

		if(m_size + len >= m_capacity)
		{
			size_t capacity = m_size + len + 1;
			this->resize(capacity);
		}

		::memcpy(&m_data[m_size], s, len);
		m_size += len;

		m_data[m_size] = '\0';
		return *this;
	}
	KDString& append(size_t count, char chr)
	{
		for(size_t i=0; i<count; i++)
			this->append(chr);
		return *this;
	}
	KDString& append(char chr)
	{
		return this->append(&chr, 1);
	}
	KDString& append(int n)
	{
		char buf[32];
		size_t len = sprintf_k(buf, sizeof(buf), "%d", n);
		return this->append(buf, len);
	}
	KDString& append(DWORD n)
	{
		char buf[32];
		size_t len = sprintf_k(buf, sizeof(buf), "%u", n);
		return this->append(buf, len);
	}
	KDString& append(INT64 n)
	{
		char buf[32];
		size_t len = sprintf_k(buf, sizeof(buf), "%lld", n);
		return this->append(buf, len);
	}
	KDString& append(const char* s)
	{
		if(!s) return *this;
		return this->append(s, (int)strlen(s));
	}
	KDString& append(const KDString& o)
	{
		return this->append(o.c_str(), o.m_size);
	}
	void clear()
	{
		m_size = 0;
		m_data[m_size] = '\0';
	}
	KDString& assign(const char* s, size_t len)
	{
		this->clear();
		return this->append(s, len);
	}
	KDString& erase(size_t beg, size_t count)
	{
		if(beg < 0) return *this;
		if(beg >= m_size) return *this;
		if(beg+count > m_size) count = m_size - beg;
		
		char* pDst = &m_data[beg];
		char* pSrc = &m_data[beg+count];
		size_t len = m_size - (beg+count);
		if(len > 0)
		{
			memmove(pDst, pSrc, len);
		}
		m_size -= count;
		m_data[m_size] = '\0';

		return *this;
	}
	KDString& insert(size_t beg, const char* p, size_t len)
	{
		if(beg < 0) return *this;
		if(beg > m_size) return *this;

		if(m_size + len >= m_capacity)
		{
			size_t capacity = m_size + len;
			this->resize(capacity);
		}

		char* pSrc = &m_data[beg];
		char* pDst = pSrc + len;

		size_t mlen = m_size - beg;
		if(mlen > 0)
		{
			memmove(pDst, pSrc, mlen);
		}

		memcpy(pSrc, p, len);
		m_size += len;
		m_data[m_size] = '\0';
		
		return *this;
	}
	size_t find(const char* s, size_t startPos = 0) const
	{
		if(m_size <= startPos) return -1;
		const char* p = strstr(m_data + startPos, s);
		if(!p) return -1;
		return (int)(p - &m_data[0]);
	}
	KDString& replace(const char* pOld, const char* pNew)
	{
		size_t oldLen = strlen(pOld);
		size_t newLen = strlen(pNew);
		size_t startPos = 0;
		while(TRUE)
		{
			size_t pos = this->find(pOld, startPos);
			if(pos == -1) break;

			this->erase(pos, oldLen);
			this->insert(pos, pNew, newLen);
			startPos += newLen;
		}
		return *this;
	}
	operator char* ()
	{
		return m_data;
	}
	operator const char* () const
	{
		return m_data;
	}
};

} /* Collections */ } /* System */

#endif