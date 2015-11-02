#include "KStreamSession.h"
#include <System/Log/log.h>

KStreamSession::KStreamSession()
	: m_pData(NULL)
	, m_dataSize(0)
	, m_capacity(0)
{

}

KStreamSession::~KStreamSession()
{
	this->detach();
}

bool KStreamSession::attach(void* pData, int capacity, int dataSize)
{
	ASSERT(pData && capacity >= dataSize);
	m_pData = (char*)pData;
	m_capacity = capacity;
	m_dataSize = dataSize;
	this->_buildIndex();
	return true;
}

void KStreamSession::detach()
{
	m_pData = NULL;
	m_dataSize = 0;
	m_capacity = 0;
	m_sessionArray.clear();
}

bool KStreamSession::copy(const KStreamSession& o)
{
	if(m_capacity < o.size()) return false;

	m_dataSize = o.m_dataSize;
	memcpy(m_pData, o.m_pData, o.m_dataSize);
	m_sessionArray.clear();
	
	this->_buildIndex();
	return true;
}

bool KStreamSession::concat(const KStreamSession& o)
{
	int n = o.m_sessionArray.size();
	for(int i=0; i<n; i++)
	{
		const StreamSessionUnit* psu = o.m_sessionArray[i];
		if(!this->setSession(psu->key, psu->val, psu->len))
			return false;
	}
	return true;
}

int KStreamSession::size() const
{
	return m_dataSize;
}

bool KStreamSession::hasSession(DWORD key) const
{
	return this->_getSession(key) != NULL;
}

LuaString KStreamSession::getSession(DWORD key) const
{
	const StreamSessionUnit* psu = this->_getSession(key);
	if(!psu) return LuaString();
	return LuaString(psu->val, psu->len);
}

int KStreamSession::getSession(DWORD key, void** ppData)
{
	StreamSessionUnit* psu = this->_getSession(key);
	if(!psu) return -1;
	*ppData = &psu->val;
	return psu->len;
}

bool KStreamSession::setSession(DWORD key, const void* val, int len)
{
	ASSERT(len < 256);

	StreamSessionUnit* psu = this->_getSession(key);
	if(psu)
	{
		if(psu->len == len)
		{
			memcpy(&psu->val, val, len);
		}
		else
		{
			char* dst = (char*)psu;
			char* src = (char*)psu + psu->size();

			int old_size = psu->size();
			int new_size = StreamSessionUnit::size(len);
			if(new_size > old_size)
			{
				if(m_dataSize + (new_size-old_size) > m_capacity)
				{
					Log(LOG_ERROR, "error: KStreamSession::setSession(%u), no avail space", key);
					return false;
				}
			}

			int length = m_dataSize - this->_getSessionPosition((StreamSessionUnit*)src);
			if(length > 0) memmove(dst, src, length);

			m_dataSize -= old_size;
			psu = (StreamSessionUnit*)(m_pData + m_dataSize);
			psu->key = key;
			psu->len = len;
			memcpy(&psu->val, val, len);
			m_dataSize += new_size;

			this->_buildIndex();
		}
	}
	else
	{
		int want = StreamSessionUnit::size(len);
		int avail = m_capacity - m_dataSize;
		if(avail < want)
		{
			Log(LOG_ERROR, "error: KStreamSession::setSession(%u), no space avail, %d/%d", key, want, avail);
			return false;
		}

		psu = (StreamSessionUnit*)(m_pData + m_dataSize);
		psu->key = key;
		psu->len = len;
		memcpy(&psu->val, val, len);
		m_dataSize += want;

		this->_buildIndex();
	}

	return true;
}

void KStreamSession::removeSession(DWORD key)
{
	StreamSessionUnit* psu = this->_getSession(key);
	if(!psu) return;

	char* dst = (char*)psu;
	char* src = (char*)psu + psu->size();
	int length = m_dataSize - this->_getSessionPosition((StreamSessionUnit*)src);

	m_dataSize -= psu->size();
	if(length > 0) memmove(dst, src, length);

	this->_buildIndex();
}

void KStreamSession::clearSessions()
{
	m_dataSize = 0;
	this->_buildIndex();
}

bool  KStreamSession::setInteger(DWORD key, int val)
{
	return this->setSession(key, &val, sizeof(val));
}

bool  KStreamSession::setFloat(DWORD key, float val)
{
	return this->setSession(key, &val, sizeof(val));
}

bool  KStreamSession::setInt64(DWORD key, INT64 val)
{
	return this->setSession(key, &val, sizeof(val));
}

int	KStreamSession::getInteger(DWORD key, int defVal) const
{
	const StreamSessionUnit* psu = this->_getSession(key);
	if(!psu) return defVal;
	ASSERT(psu->len == sizeof(int));

	int ret;
	memcpy(&ret, &psu->val, sizeof(int));
	return ret;
}

float KStreamSession::getFloat(DWORD key, float defVal) const
{
	const StreamSessionUnit* psu = this->_getSession(key);
	if(!psu) return defVal;
	ASSERT(psu->len == sizeof(float));

	float ret;
	memcpy(&ret, &psu->val, sizeof(float));
	return ret;
}

INT64 KStreamSession::getInt64(DWORD key, INT64 defVal) const
{
	const StreamSessionUnit* psu = this->_getSession(key);
	if(!psu) return defVal;
	ASSERT(psu->len == sizeof(INT64));

	INT64 ret;
	memcpy(&ret, &psu->val, sizeof(INT64));
	return ret;
}

void KStreamSession::_buildIndex()
{
	int pos = 0;
	int length = m_dataSize;

	m_sessionArray.clear();

	while(pos < length)
	{
		StreamSessionUnit* psu = (StreamSessionUnit*)&m_pData[pos];
		m_sessionArray.insert_unique(psu);
		pos += psu->size();
	}
}

StreamSessionUnit* KStreamSession::_getSession(DWORD key)
{
	StreamSessionUnit u; u.key = key;
	int pos = m_sessionArray.find(&u);
	if(pos < 0) return NULL;
	return m_sessionArray[pos];
}

const StreamSessionUnit* KStreamSession::_getSession(DWORD key) const
{
	KStreamSession* pThis = (KStreamSession*)this;
	return pThis->_getSession(key);
}

int KStreamSession::_getSessionPosition(StreamSessionUnit* psu)
{
	int pos = (int)((char*)psu - (char*)m_pData);
	ASSERT(pos >= 0);
	return pos;
}
