#include "KTableCacheTmpl.h"
#include "KCachePkey.h"

enum pk_data_type
{
	pk_key_none = 0,
	pk_key_uint32,
	pk_key_uint64,
	pk_key_string64,
};

#define make_pk_ind2(k1, k2) ((int)((k1<<8)|k2))

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

KTableCache* KDefaultTableCacheCreator::Create(KSQLTableSchema& schema, KVarBinaryMemory* vbm)
{
	int pknum = schema.GetPrimaryKeyFieldCount();
	if(pknum == 1)
	{
		const KSQLTableField* field = schema.GetField(0);
		pk_data_type keyType = get_pk_data_type(*field);
		switch(keyType)
		{
		case pk_key_uint32:
			return new KTableCacheTmpl<KSQLPkey_U32>(vbm);
		case pk_key_uint64:
			return new KTableCacheTmpl<KSQLPkey_U64>(vbm);
		case pk_key_string64:
			return new KTableCacheTmpl< KSQLPkey_String<64> >(vbm);
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
		case k_u32_u32: return new KTableCacheTmpl<KSQLPkey_U32_U32>(vbm);
		case k_u32_u64: return new KTableCacheTmpl<KSQLPkey_U32_U64>(vbm);
		case k_u32_s64: return new KTableCacheTmpl<KSQLPkey_U32_S64>(vbm);
		case k_u64_u32: return new KTableCacheTmpl<KSQLPkey_U64_U32>(vbm);
		case k_u64_u64: return new KTableCacheTmpl<KSQLPkey_U64_U64>(vbm);
		case k_u64_s64: return new KTableCacheTmpl<KSQLPkey_U64_S64>(vbm);
		case k_s64_u32: return new KTableCacheTmpl<KSQLPkey_S64_U32>(vbm);
		case k_s64_u64: return new KTableCacheTmpl<KSQLPkey_S64_U64>(vbm);
		case k_s64_s64: return new KTableCacheTmpl<KSQLPkey_S64_S64>(vbm);
		}
		return NULL;
	}

	return NULL;
}
