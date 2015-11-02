#ifndef _K_TABLE_CACHE_TMPL_H_
#define _K_TABLE_CACHE_TMPL_H_

#include "KTableCache.h"
#include "KCacheObject.h"
#include "KVarBinary.h"

class ITableCacheCreator
{
public:
	virtual ~ITableCacheCreator() { }
	virtual KTableCache* Create(KSQLTableSchema& schema, KVarBinaryMemory* vbm) = 0;
};

class KDefaultTableCacheCreator : public ITableCacheCreator
{
public:
	KTableCache* Create(KSQLTableSchema& schema, KVarBinaryMemory* vbm);
};

/**
 * ��Ϊ Cache Table ���������������� PKEY ��Ҫ���㼸������
 *
 * 1. public inner type (1) key_type (2) key_cmp
 * 2. BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
 * 3. BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
 * 4. BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
 * 5. KString<512> ToString(const key_type& val) const
 *
 * ����ٶ��������һ���Ǵӵ�һ���ֶο�ʼ�����ֶ�֮����������
 *
 * һЩȱʡ�������Ѿ��� KCachePkey.h �ж���
 * �����Ҫδ�ṩ���������ͣ���Ҫдһ��PKEY��
 * ���Ҽ̳�KDefaultTableCacheCreator�������Լ���Ҫ��KTableCache��������
 */

template <typename PKEY>
class KTableCacheTmpl : public KTableCache
{
public:
	typedef System::Collections::KMapNode<typename PKEY::key_type,HANDLE> node_type;
	typedef System::Memory::KStepBlockAlloc<node_type,1024> alloc_type;
	typedef System::Collections::KMapByVector2<typename PKEY::key_type,HANDLE,typename PKEY::key_cmp,alloc_type> RecordMap;

public:
	KTableCacheTmpl()
	{

	}
	KTableCacheTmpl(KVarBinaryMemory* vbm):KTableCache(vbm)
	{

	}
	~KTableCacheTmpl()
	{

	}

public:
	BOOL SetPrimaryKey(KCacheObject& cache, const void* pKey)
	{
		const PKEY::key_type& key = *(PKEY::key_type*)pKey;
		return m_keyCls.SetPrimaryKey(cache, key);
	}

	BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	{
		return m_keyCls.SetSQLParam(cmd, paramBaseIdx, cache);
	}

	System::Collections::KString<512> Key2String(const void* pKey) const
	{
		const PKEY::key_type& key = *(PKEY::key_type*)pKey;
		return m_keyCls.ToString(key);
	}

	System::Collections::KString<512> GetKeyString(HANDLE hRec) const
	{
		KCacheObject cacheObj;
		ASSERT(cacheObj.Attach((KTableCache*)this, hRec));
		
		PKEY::key_type key;
		if(!((KTableCache*)this)->GetPrimaryKey(cacheObj, &key))
		{
			return System::Collections::KString<512>("<error:pkey>");
		}

		return this->Key2String(&key);
	}

	BOOL GetPrimaryKey(KCacheObject& cache, void* pKey)
	{
		PKEY::key_type& key = *(PKEY::key_type*)pKey;
		return m_keyCls.GetPrimaryKey(cache, key);
	}

	BOOL IsPrimaryKey(KCacheObject& cache, const void* pKey)
	{
		const PKEY::key_type& key_1 = *(PKEY::key_type*)pKey;

		PKEY::key_type key_2;
		if(!m_keyCls.GetPrimaryKey(cache, key_2))
			return FALSE;

		typename PKEY::key_cmp cmp;
		return cmp(key_1, key_2) == 0;
	}

	BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
	{
		return m_keyCls.SetAutoIncreaseKey(cache, key);
	}

	HANDLE _FindRecord(const void* pKey)
	{
		const PKEY::key_type& key = *(PKEY::key_type*)pKey;
		RecordMap::iterator it = m_recMap.find(key);
		if(it == m_recMap.end()) return INVALID_HREC;
		return it->second;
	}

protected: // ��ģ����ʵ�ֵķ���

	// ������һ���µļ�¼����ӵ��ڲ���MAP��
	BOOL OnRecordLoaded(HANDLE hRec)
	{
		KCacheObject cacheObj;
		ASSERT(cacheObj.Attach(this, hRec));

		PKEY::key_type key;
		if(!m_keyCls.GetPrimaryKey(cacheObj, key))
			return FALSE;

		m_recMap.insert_unique(key, hRec);
		return TRUE;
	}

	// ж����һ����¼����MAP��ɾ��
	BOOL OnRecordUnload(HANDLE hRec)
	{
		KCacheObject cacheObj;
		ASSERT(cacheObj.Attach(this, hRec));

		PKEY::key_type key;
		if(!m_keyCls.GetPrimaryKey(cacheObj, key))
			return FALSE;
		
		RecordMap::iterator it = m_recMap.find(key);
		if(it == m_recMap.end())
			return FALSE;

		m_recMap.erase(it);
		return TRUE;
	}

	BOOL _checkKeyLength(int len) const
	{
		return len == sizeof(PKEY::key_type);
	}

public:
	PKEY m_keyCls;
	RecordMap m_recMap;
};

#endif
