#include "KCachePkey.h"

using namespace System::Collections;

// KSQLPkey_DWORD

BOOL KSQLPkey_U32::SetPrimaryKey(KCacheObject& cache, const KSQLPkey_U32::key_type& val)
{
	return cache.SetInt64(0, val);
}

BOOL KSQLPkey_U32::GetPrimaryKey(KCacheObject& cache, KSQLPkey_U32::key_type& val) const
{
	INT64 ret;
	if(!cache.GetInt64(0, ret))
		return FALSE;

	val = (DWORD)ret;
	return TRUE;
}

BOOL KSQLPkey_U32::SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
{
	key_type val;

	if(!this->GetPrimaryKey(cache, val))
		return FALSE;

	return cmd.SetInt64(paramBaseIdx, val);
}

KString<512> KSQLPkey_U32::ToString(const KSQLPkey_U32::key_type& val) const
{
	char buf[64];
	sprintf(buf, "%u", val);
	return buf;
}

BOOL KSQLPkey_U32::SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
{
	return cache.SetInt64(0, key);
}

BOOL KSQLPkey_U32::Read(KTableRecordData& recData, KSQLPkey_U32::key_type& key)
{
	KTableRecordData::Field* field = recData.begin();
	if(!field) return FALSE;
	ASSERT(field->length == sizeof(key_type));
	memcpy(&key, &field->value, field->length);
	return TRUE;
}

// KSQLPkey_UI64

BOOL KSQLPkey_U64::SetPrimaryKey(KCacheObject& cache, const KSQLPkey_U64::key_type& val)
{
	return cache.SetInt64(0, val);
}

BOOL KSQLPkey_U64::GetPrimaryKey(KCacheObject& cache, KSQLPkey_U64::key_type& val) const
{
	INT64 ret;
	if(!cache.GetInt64(0, ret))
		return FALSE;

	val = ret;
	return TRUE;
}

BOOL KSQLPkey_U64::SetSQLParam(KSQLCommand& cmd, int paramBaseIdx, KCacheObject& cache)
{
	key_type val;

	if(!this->GetPrimaryKey(cache, val))
		return FALSE;

	return cmd.SetInt64(paramBaseIdx, val);
}

KString<512> KSQLPkey_U64::ToString(const KSQLPkey_U64::key_type& val) const
{
	char buf[64];
	sprintf(buf, "%llu", val);
	return buf;
}

BOOL KSQLPkey_U64::SetAutoIncreaseKey(KCacheObject& cache, UINT64 key)
{
	return cache.SetInt64(0, key);
}

BOOL KSQLPkey_U64::Read(KTableRecordData& recData, KSQLPkey_U64::key_type& key)
{
	KTableRecordData::Field* field = recData.begin();
	if(!field) return FALSE;
	ASSERT(field->length == sizeof(key_type));
	memcpy(&key, &field->value, field->length);
	return TRUE;
}