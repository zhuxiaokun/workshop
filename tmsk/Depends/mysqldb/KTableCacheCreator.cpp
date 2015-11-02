#include "KTableCacheCreator.h"
#include "KTableCache_X_Tmpl.h"
#include "KTableCache_Tmpl.h"
#include "KCachePkey.h"
#include "KCacheManager.h"
#include "KTableCache_X_Thread.h"
#include "KSQLTableCache_X_Thread.h"

/////

pk_data_type get_pk_data_type(const KSQLTableField& field)
{
	switch(field.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
	case KSQLTypes::sql_c_uint8:
	case KSQLTypes::sql_c_int16:
	case KSQLTypes::sql_c_uint16:
	case KSQLTypes::sql_c_int32:
	case KSQLTypes::sql_c_uint32:
		return pk_key_uint32;
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_uint64:
		return pk_key_uint64;
	case KSQLTypes::sql_c_float32:
	case KSQLTypes::sql_c_float64:
		return pk_key_none;
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
		{
			if(field.m_length < 64)
			{
				return pk_key_string64;
			}
			else
			{
				return pk_key_none;
			}
		}
	case KSQLTypes::sql_c_binary:
	case KSQLTypes::sql_c_timestamp:
		return pk_key_none;
	}

	return pk_key_none;
}

KTableCache_X* _createTableCache(KSQLTableSchema& schema)
{
	int pknum = schema.GetPrimaryKeyFieldCount();
	if(pknum == 1)
	{
		const KSQLTableField* field = schema.GetField(0);
		pk_data_type keyType = get_pk_data_type(*field);
		switch(keyType)
		{
		case pk_key_uint32:
			return new KTableCache_X_Tmpl<KSQLPkey_U32>();
		case pk_key_uint64:
			return new KTableCache_X_Tmpl<KSQLPkey_U64>();
		case pk_key_string64:
			return new KTableCache_X_Tmpl< KSQLPkey_String<64> >();
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
		case k_u32_u32: return new KTableCache_X_Tmpl<KSQLPkey_U32_U32>();
		case k_u32_u64: return new KTableCache_X_Tmpl<KSQLPkey_U32_U64>();
		case k_u32_s64: return new KTableCache_X_Tmpl<KSQLPkey_U32_S64>();
		case k_u64_u32: return new KTableCache_X_Tmpl<KSQLPkey_U64_U32>();
		case k_u64_u64: return new KTableCache_X_Tmpl<KSQLPkey_U64_U64>();
		case k_u64_s64: return new KTableCache_X_Tmpl<KSQLPkey_U64_S64>();
		case k_s64_u32: return new KTableCache_X_Tmpl<KSQLPkey_S64_U32>();
		case k_s64_u64: return new KTableCache_X_Tmpl<KSQLPkey_S64_U64>();
		case k_s64_s64: return new KTableCache_X_Tmpl<KSQLPkey_S64_S64>();
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
		return new KTableCache_X_Tmpl<KSQLPkey_U32_U32_U32>();
	}

	return NULL;
}

//// KClientTableCacheCreator

KTableCacheReader* KClientTableCacheCreator::Create(TableCacheSetting& tableSetting)
{
	KTableCacheReader::CreateParam createParam;
	createParam.capacity = tableSetting.m_capacity;
	createParam.flushInterval = tableSetting.m_flushInterval;
	createParam.mmapfile = tableSetting.m_mmapfile;

	KTableCacheReader* tableCache = new KTableCacheReader();
	if(!tableCache->initialize(tableSetting.m_schema, createParam))
	{
		tableCache->finalize();
		delete tableCache;
		return NULL;
	}
	
	return tableCache;
}

/// KTraditionalCacheCreator

//KTableCacheReader* KTraditionalCacheCreator::Create(TableCacheSetting& tableSetting)
//{
//	KTableCacheReader::CreateParam createParam;
//	createParam.capacity = tableSetting.m_capacity;
//	createParam.flushInterval = tableSetting.m_flushInterval;
//	createParam.mmapfile = tableSetting.m_mmapfile;
//
//	KTableCache* tableCache = new KTableCache();
//	if(!tableCache) return NULL;
//
//	if(!tableCache->initialize(tableSetting.m_schema, createParam, *tableSetting._connParam))
//	{
//		tableCache->finalize();
//		delete tableCache;
//		return NULL;
//	}
//
//	return tableCache;
//}

//// KAgentTableCacheCreator

KTableCacheReader* KAgentTableCacheCreator::Create(TableCacheSetting& tableSetting)
{
	KTableCacheReader::CreateParam createParam;
	createParam.capacity = tableSetting.m_capacity;
	createParam.flushInterval = tableSetting.m_flushInterval;
	createParam.mmapfile = tableSetting.m_mmapfile;

	KTableCache_X* tableCache = _createTableCache(tableSetting.m_schema);
	if(!tableCache) return NULL;

	if(!tableCache->initialize(tableSetting.m_schema, createParam))
	{
		tableCache->finalize();
		delete tableCache;
		return NULL;
	}

	tableCache->m_thread = new KTableCache_X_Thread(*tableCache);
	tableCache->m_thread->setNextTaskSlot(tableSetting._nextTaskSlot);
	tableCache->m_thread->Run();
	return tableCache;
}

//// KSQLTableCacheCreator

KTableCacheReader* KSQLTableCacheCreator::Create(TableCacheSetting& tableSetting)
{
	KTableCacheReader::CreateParam createParam;
	createParam.capacity = tableSetting.m_capacity;
	createParam.flushInterval = tableSetting.m_flushInterval;
	createParam.mmapfile = tableSetting.m_mmapfile;

	KTableCache_X* tableCache = _createTableCache(tableSetting.m_schema);
	if(!tableCache) return NULL;

	if(!tableCache->initialize(tableSetting.m_schema, createParam))
	{
		tableCache->finalize();
		delete tableCache;
		return NULL;
	}

	tableCache->m_thread = new KSQLTableCache_X_Thread(*tableCache, *tableSetting._connParam);
	tableCache->m_thread->setNextTaskSlot(tableSetting._nextTaskSlot);
	tableCache->m_thread->Run();
	return tableCache;
}

///////

KTableCache* _createTableCache_A(KSQLTableSchema& schema)
{
	int pknum = schema.GetPrimaryKeyFieldCount();
	if(pknum == 1)
	{
		const KSQLTableField* field = schema.GetField(0);
		pk_data_type keyType = get_pk_data_type(*field);
		switch(keyType)
		{
		case pk_key_uint32:
			return new KTableCache_Tmpl<KSQLPkey_U32>();
		case pk_key_uint64:
			return new KTableCache_Tmpl<KSQLPkey_U64>();
		case pk_key_string64:
			return new KTableCache_Tmpl< KSQLPkey_String<64> >();
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
		case k_u32_u32: return new KTableCache_Tmpl<KSQLPkey_U32_U32>();
		case k_u32_u64: return new KTableCache_Tmpl<KSQLPkey_U32_U64>();
		case k_u32_s64: return new KTableCache_Tmpl<KSQLPkey_U32_S64>();
		case k_u64_u32: return new KTableCache_Tmpl<KSQLPkey_U64_U32>();
		case k_u64_u64: return new KTableCache_Tmpl<KSQLPkey_U64_U64>();
		case k_u64_s64: return new KTableCache_Tmpl<KSQLPkey_U64_S64>();
		case k_s64_u32: return new KTableCache_Tmpl<KSQLPkey_S64_U32>();
		case k_s64_u64: return new KTableCache_Tmpl<KSQLPkey_S64_U64>();
		case k_s64_s64: return new KTableCache_Tmpl<KSQLPkey_S64_S64>();
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
		return new KTableCache_Tmpl<KSQLPkey_U32_U32_U32>();
	}

	return NULL;
}

KTableCacheReader* KTraditionalCacheCreator::Create(TableCacheSetting& tableSetting)
{
	KTableCacheReader::CreateParam createParam;
	createParam.capacity = tableSetting.m_capacity;
	createParam.flushInterval = tableSetting.m_flushInterval;
	createParam.mmapfile = tableSetting.m_mmapfile;

	KTableCache* tableCache = _createTableCache_A(tableSetting.m_schema);
	if(!tableCache) return NULL;

	if(!tableCache->Initialize(*tableSetting._connParam, tableSetting.m_schema, createParam))
	{
		tableCache->finalize();
		delete tableCache;
		return NULL;
	}

	return tableCache;
}