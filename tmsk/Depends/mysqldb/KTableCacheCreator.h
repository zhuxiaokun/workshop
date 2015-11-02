#pragma once

#include "KDatabase.h"
#include "KTableCache_X.h"
//#include "KTableCache_A.h"

struct TableCacheSetting;
extern KTableCache_X* _createTableCache(KSQLTableSchema& schema);

enum pk_data_type
{
	pk_key_none = 0,
	pk_key_uint32,
	pk_key_uint64,
	pk_key_string64,
};
#define make_pk_ind2(k1, k2) ((int)((k1<<8)|k2))
pk_data_type get_pk_data_type(const KSQLTableField& field);


class KClientTableCacheCreator
{
public:
	KTableCacheReader* Create(TableCacheSetting& tableSetting);
};

//class KTraditionalCacheCreator
//{
//public:
//	KTableCacheReader* Create(TableCacheSetting& tableSetting);
//};

class KAgentTableCacheCreator
{
public:
	KTableCacheReader* Create(TableCacheSetting& tableSetting);
};

class KSQLTableCacheCreator
{
public:
	KTableCacheReader* Create(TableCacheSetting& tableSetting);
};

class KTraditionalCacheCreator
{
public:
	KTableCacheReader* Create(TableCacheSetting& tableSetting);
};