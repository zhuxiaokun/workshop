#include "usePoolTableCacheCreator.h"
#include <System/Collections/KTree.h>
#include <System/Collections/KString.h>
#include <mysqldb/KTableCacheCreator.h>
#include <mysqldb/KCachePkey.h>

namespace jg { namespace mysql { 

	/**
	* 作为 Cache Table 的主键的数据类型 PKEY 需要满足几个条件
	*
	* 1. public inner type (1) key_type (2) key_cmp
	* 2. BOOL SetPrimaryKey(KCacheObject& cache, const key_type& val)
	* 3. BOOL GetPrimaryKey(KCacheObject& cache, key_type& val) const
	* 4. BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
	* 5. KString<512> ToString(const key_type& val) const
	*
	* 这里假定表的主键一定是从第一个字段开始并且字段之间是连续的
	*
	* 一些缺省的类型已经在 KCachePkey.h 中定义
	* 如果需要未提供的主键类型，需要写一个PKEY，
	* 并且继承KDefaultTableCacheCreator，生成自己需要的KTableCache_X数据类型
	*/

	template <typename PKEY>
	class TableCacheUsePool_Tmpl : public TableCacheUsePool
	{
	public:
		typedef System::Collections::KMapByTree_pooled<typename PKEY::key_type,HANDLE,10240,typename PKEY::key_cmp> RecordMap;

	public:
		PKEY m_keyCls;
		RecordMap m_recMap;

	public:
		TableCacheUsePool_Tmpl()
		{

		}
		~TableCacheUsePool_Tmpl()
		{
			this->finalize();
		}

	public: // 由模板类实现的方法
		BOOL SetPrimaryKey(KCacheObject& cache, const void* pKey)
		{
			const typename PKEY::key_type& key = *(typename PKEY::key_type*)pKey;
			return m_keyCls.SetPrimaryKey(cache, key);
		}

		BOOL SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
		{
			return m_keyCls.SetSQLParam(cmd, paramBaseIdx, cache);
		}

		System::Collections::KString<512> Key2String(const void* pKey) const
		{
			const typename PKEY::key_type& key = *(typename PKEY::key_type*)pKey;
			return m_keyCls.ToString(key);
		}

		System::Collections::KString<512> GetKeyString(HANDLE hRec) const
		{
			KCacheObject cacheObj;
			ASSERT(cacheObj.Attach((KTableCache_X*)this, hRec));

			typename PKEY::key_type key;
			if(!((KTableCache_X*)this)->GetPrimaryKey(cacheObj, &key))
			{
				return System::Collections::KString<512>("<error:pkey>");
			}

			return this->Key2String(&key);
		}

		System::Collections::KString<512> GetKeyString(KTableRecordData& recData) const
		{
			TableCacheUsePool_Tmpl* pThis = (TableCacheUsePool_Tmpl*)this;
			typename PKEY::key_type key;
			if(!pThis->m_keyCls.Read(recData, key))
				return System::Collections::KString<512>("error primary key");
			return this->Key2String(&key);
		}

		BOOL GetPrimaryKey(KCacheObject& cache, void* pKey)
		{
			typename PKEY::key_type& key = *(typename PKEY::key_type*)pKey;
			return m_keyCls.GetPrimaryKey(cache, key);
		}

		BOOL IsPrimaryKey(KCacheObject& cache, const void* pKey)
		{
			const typename PKEY::key_type& key_1 = *(typename PKEY::key_type*)pKey;

			typename PKEY::key_type key_2;
			if(!m_keyCls.GetPrimaryKey(cache, key_2))
				return FALSE;

			typename PKEY::key_cmp cmp;
			return cmp(key_1, key_2) == 0;
		}

		BOOL SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
		{
			return m_keyCls.SetAutoIncreaseKey(cache, key);
		}

		HANDLE findRecord(KTableRecordData& recData)
		{
			typename PKEY::key_type key;
			if(!m_keyCls.Read(recData, key))
				return INVALID_HREC;
			return this->_findRecord(&key);
		}

		BOOL dumpKey(const void* pKey, size_t len, KTableRecordData& recData)
		{
			ASSERT(this->_checkKeyLength((int)len));
			const typename PKEY::key_type& key = *(typename PKEY::key_type*)pKey;
			recData.setVersion(0);
			return m_keyCls.Write(key, recData);
		}

		HANDLE _findRecord(const void* pKey)
		{
			const typename PKEY::key_type& key = *(typename PKEY::key_type*)pKey;
			typename RecordMap::iterator it = m_recMap.find(key);
			if(it == m_recMap.end()) return (HANDLE)invalid_hrec;
			return it->second;
		}

	protected:

		// 加载了一条新的记录，添加到内部的MAP中
		BOOL on_recordLoaded(HANDLE hRec)
		{
			KCacheObject cacheObj;
			ASSERT(cacheObj.Attach(this, hRec));

			typename PKEY::key_type key;
			if(!m_keyCls.GetPrimaryKey(cacheObj, key))
				return FALSE;

			m_recMap.insert(key, hRec);
			return TRUE;
		}

		// 卸载了一条记录，从MAP中删除
		BOOL on_unloadRecord(HANDLE hRec)
		{
			KCacheObject cacheObj;
			ASSERT(cacheObj.Attach(this, hRec));

			typename PKEY::key_type key;
			if(!m_keyCls.GetPrimaryKey(cacheObj, key))
				return FALSE;

			typename RecordMap::iterator it = m_recMap.find(key);
			if(it == m_recMap.end())
				return FALSE;

			m_recMap.erase(it);
			return TRUE;
		}

		BOOL _checkKeyLength(int len) const
		{
			return len == sizeof(typename PKEY::key_type);
		}
	};

	TableCacheUsePool* _createTableCache_UsePool(KSQLTableSchema& schema)
	{
		int pknum = schema.GetPrimaryKeyFieldCount();
		if(pknum == 1)
		{
			const KSQLTableField* field = schema.GetField(0);
			pk_data_type keyType = get_pk_data_type(*field);
			switch(keyType)
			{
			case pk_key_uint32:
				return new TableCacheUsePool_Tmpl<KSQLPkey_U32>();
			case pk_key_uint64:
				return new TableCacheUsePool_Tmpl<KSQLPkey_U64>();
			case pk_key_string64:
				return new TableCacheUsePool_Tmpl< KSQLPkey_String<64> >();
			}
			return NULL;
		}
		else if(pknum == 2)
		{
			const KSQLTableField* field1 = schema.GetField(0);
			const KSQLTableField* field2 = schema.GetField(1);

			pk_data_type k1 = get_pk_data_type(*field1);
			pk_data_type k2 = get_pk_data_type(*field2);

			const int k_u32_u32 = make_pk_ind2(pk_key_uint32, pk_key_uint32);
			const int k_u32_u64 = make_pk_ind2(pk_key_uint32, pk_key_uint64);
			const int k_u32_s64 = make_pk_ind2(pk_key_uint32, pk_key_string64);
			const int k_u64_u32 = make_pk_ind2(pk_key_uint64, pk_key_uint32);
			const int k_u64_u64 = make_pk_ind2(pk_key_uint64, pk_key_uint64);
			const int k_u64_s64 = make_pk_ind2(pk_key_uint64, pk_key_string64);
			const int k_s64_u32 = make_pk_ind2(pk_key_string64, pk_key_uint32);
			const int k_s64_u64 = make_pk_ind2(pk_key_string64, pk_key_uint64);
			const int k_s64_s64 = make_pk_ind2(pk_key_string64, pk_key_string64);

			switch(make_pk_ind2(k1, k2))
			{
			case k_u32_u32: return new TableCacheUsePool_Tmpl<KSQLPkey_U32_U32>();
			case k_u32_u64: return new TableCacheUsePool_Tmpl<KSQLPkey_U32_U64>();
			case k_u32_s64: return new TableCacheUsePool_Tmpl<KSQLPkey_U32_S64>();
			case k_u64_u32: return new TableCacheUsePool_Tmpl<KSQLPkey_U64_U32>();
			case k_u64_u64: return new TableCacheUsePool_Tmpl<KSQLPkey_U64_U64>();
			case k_u64_s64: return new TableCacheUsePool_Tmpl<KSQLPkey_U64_S64>();
			case k_s64_u32: return new TableCacheUsePool_Tmpl<KSQLPkey_S64_U32>();
			case k_s64_u64: return new TableCacheUsePool_Tmpl<KSQLPkey_S64_U64>();
			case k_s64_s64: return new TableCacheUsePool_Tmpl<KSQLPkey_S64_S64>();
			}
			return NULL;
		}
		else if(pknum == 3)
		{
			const KSQLTableField* field1 = schema.GetField(0);
			const KSQLTableField* field2 = schema.GetField(1);
			const KSQLTableField* field3 = schema.GetField(2);
			pk_data_type k1 = get_pk_data_type(*field1);
			pk_data_type k2 = get_pk_data_type(*field2);
			pk_data_type k3 = get_pk_data_type(*field3);
			ASSERT(pk_key_uint32 == k1);
			ASSERT(pk_key_uint32 == k2);
			ASSERT(pk_key_uint32 == k3);
			return new TableCacheUsePool_Tmpl<KSQLPkey_U32_U32_U32>();
		}

		return NULL;
	}

	TableCacheUsePool* UsePoolTableCacheCreator::Create(TableCacheSetting& tableSetting)
	{
		KTableCacheReader::CreateParam createParam;
		createParam.capacity = tableSetting.m_capacity;
		createParam.flushInterval = tableSetting.m_flushInterval;
		createParam.mmapfile = tableSetting.m_mmapfile;

		TableCacheUsePool* tableCache = _createTableCache_UsePool(tableSetting.m_schema);
		if(!tableCache) return NULL;

		if(!tableCache->Initialize((funcPrepareSqlContext)tableSetting._connParam, tableSetting.m_schema, createParam))
		{
			tableCache->finalize();
			delete tableCache;
			return NULL;
		}

		return tableCache;
	}

} }
