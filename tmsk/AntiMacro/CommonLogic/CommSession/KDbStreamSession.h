#ifndef _KDBSTREAM_SESSIONS_H_
#define _KDBSTREAM_SESSIONS_H_

#include <System/KPlatform.h>
#include <mysqldb/KCacheObject.h>
#include <System/Collections/DynArray.h>
#include <lua/KLuaWrap.h>
#include "KStreamSession.h"

class KDbStreamSessions
{
public:
	class cmp
	{
	public:
		typedef StreamSessionUnit* ptr;
		int operator () (const ptr& a, const ptr& b) const
		{
			return (int)(a->key - b->key);
		}
	};
	typedef ::System::Collections::KDynSortedArray<StreamSessionUnit*,cmp,128,64> SessionArray;

public:
	KDbStreamSessions();
	~KDbStreamSessions();

public:
	bool initialize(KCacheObject& cacheObj, int field, int capacity);
	void finalize();

public:
	bool hasSession(DWORD key) const;
	LuaString getSession(DWORD key) const;
	int getSession(DWORD key, void** ppVal) const;
	bool setSession(DWORD key, const void* val, int len);
	void removeSession(DWORD key);
	void clearSessions();

public:
	template<typename T> T get(DWORD key, const T& defVal) const
	{
		if(!this->hasSession(key)) return defVal;
		const StreamSessionUnit* psu = this->_getSession(key);
		if(!psu) return defVal;
		ASSERT(psu->len == sizeof(T));
		return *(T*)&psu->val;
	}
	template<typename T> T* getp(DWORD key)
	{
		if(!this->hasSession(key)) return NULL;
		StreamSessionUnit* psu = this->_getSession(key);
		if(!psu) return NULL;
		ASSERT(psu->len == sizeof(T));
		return (T*)&psu->val;
	}
	template<typename T> bool set(DWORD key, const T& val)
	{
		return this->setSession(key, &val, sizeof(T));
	}

private:
	void _buildIndex(); // ´´½¨Ë÷Òý
	StreamSessionUnit* _getSession(DWORD key);
	const StreamSessionUnit* _getSession(DWORD key) const;
	int _getSessionPosition(StreamSessionUnit* psu);

public:
	int m_field;
	int m_capacity;
	KDbBinaryHead* m_pBinary;
	KCacheObject* m_cacheObj;
	SessionArray m_sessionArray;
};

#endif
