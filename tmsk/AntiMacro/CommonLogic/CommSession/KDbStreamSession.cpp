#include "KDbStreamSession.h"

KDbStreamSessions::KDbStreamSessions()
	: m_field(0)
	, m_capacity(0)
	, m_pBinary(NULL)
	, m_cacheObj(NULL)
{

}

KDbStreamSessions::~KDbStreamSessions()
{
	this->finalize();
}

bool KDbStreamSessions::initialize(KCacheObject& cacheObj, int field, int capacity)
{
	m_field = field;
	m_capacity = capacity;
	m_cacheObj = &cacheObj;
	ASSERT(m_cacheObj->BindColumnTo(field, m_pBinary));
	this->_buildIndex();
	return true;
}

void KDbStreamSessions::finalize()
{
	m_field = 0;
	m_capacity = 0;
	m_cacheObj = NULL;
	m_pBinary = NULL;
}

bool KDbStreamSessions::hasSession(DWORD key) const
{
	return this->_getSession(key) != NULL;
}

LuaString KDbStreamSessions::getSession(DWORD key) const
{
	const StreamSessionUnit* psu = this->_getSession(key);
	if(!psu) return LuaString();
	return LuaString(psu->val, psu->len);
}

int KDbStreamSessions::getSession(DWORD key, void** ppVal) const
{
	const StreamSessionUnit* psu = this->_getSession(key);
	if(!psu) return -1;
	*ppVal = (void*)&psu->val;
	return psu->len;
}

bool KDbStreamSessions::setSession(DWORD key, const void* val, int len)
{
	ASSERT(len < 256);

	StreamSessionUnit* psu = this->_getSession(key);
	if(psu)
	{
		if(psu->len == len)
		{
			if(memcmp(&psu->val, val, len))
			{
				m_cacheObj->BeginUpdate(m_field);
				{
					memcpy(&psu->val, val, len);
				}
				m_cacheObj->EndUpdate(m_field);
			}
		}
		else
		{
			char* dst = (char*)psu;
			char* src = (char*)psu + psu->size();
			
			int old_size = psu->size();
			int new_size = StreamSessionUnit::size(len);
			if(m_pBinary->length - old_size + new_size > m_capacity)
			{
				Log(LOG_ERROR, "error: KPlayerSessions::setSession(%u), no avail space", key);
				return false;
			}

			int length = m_pBinary->length - this->_getSessionPosition((StreamSessionUnit*)src);
			
			m_cacheObj->BeginUpdate(m_field);
			{
				m_pBinary->length -= old_size;
				if(length > 0) memmove(dst, src, length);
				psu = (StreamSessionUnit*)(&m_pBinary->data[0] + m_pBinary->length);
				psu->key = key;
				psu->len = len;
				memcpy(&psu->val, val, len);
				m_pBinary->length += new_size;
			}
			m_cacheObj->EndUpdate(m_field);

			this->_buildIndex();
		}
	}
	else
	{
		int want = StreamSessionUnit::size(len);
		int avail = m_capacity - m_pBinary->length;
		if(avail < want)
		{
			Log(LOG_ERROR, "error: KPlayerSessions::setSession(%u), no space %d/%d", key, want, avail);
			return false;
		}

		m_cacheObj->BeginUpdate(m_field);
		{
			psu = (StreamSessionUnit*)(&m_pBinary->data[0] + m_pBinary->length);
			psu->key = key;
			psu->len = len;
			memcpy(&psu->val, val, len);
			m_pBinary->length += want;
		}
		m_cacheObj->EndUpdate(m_field);

		this->_buildIndex();
	}
	
	return true;
}

void KDbStreamSessions::removeSession(DWORD key)
{
	StreamSessionUnit* psu = this->_getSession(key);
	if(!psu) return;
	
	char* dst = (char*)psu;
	char* src = (char*)psu + psu->size();
	int length = m_pBinary->length - this->_getSessionPosition((StreamSessionUnit*)src);

	m_cacheObj->BeginUpdate(m_field);
	{
		m_pBinary->length -= psu->size();
		if(length > 0) memmove(dst, src, length);
	}
	m_cacheObj->EndUpdate(m_field);

	this->_buildIndex();
}

void KDbStreamSessions::clearSessions()
{
	if(m_pBinary->length > 0)
	{
		m_cacheObj->BeginUpdate(m_field);
		{
			m_pBinary->length = 0;
		}
		m_cacheObj->EndUpdate(m_field);
	}
	this->_buildIndex();
}

void KDbStreamSessions::_buildIndex()
{
	int pos = 0;
	int length = m_pBinary->length;

	m_sessionArray.clear();

	while(pos < length)
	{
		StreamSessionUnit* psu = (StreamSessionUnit*)&m_pBinary->data[pos];
		m_sessionArray.insert_unique(psu);
		pos += psu->size();
	}
}

StreamSessionUnit* KDbStreamSessions::_getSession(DWORD key)
{
	StreamSessionUnit u; u.key = key;
	int pos = m_sessionArray.find(&u);
	if(pos < 0) return NULL;
	return m_sessionArray[pos];
}

const StreamSessionUnit* KDbStreamSessions::_getSession(DWORD key) const
{
	KDbStreamSessions* pThis = (KDbStreamSessions*)this;
	return pThis->_getSession(key);
}

int KDbStreamSessions::_getSessionPosition(StreamSessionUnit* psu)
{
	int pos = (int)((char*)psu - (char*)&m_pBinary->data);
	ASSERT(pos >= 0);
	return pos;
}
