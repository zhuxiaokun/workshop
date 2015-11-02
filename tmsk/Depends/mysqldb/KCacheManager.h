#pragma once

#include "KTableCache.h"
#include <System/tinyxml/xmldom.h>
#include <System/Collections/KTree.h>
#include <System/Collections/KMapByVector.h>
#include <System/Log/log.h>

/*
<Cache>
	<Connection host="localhost" db="test" user="root", password=""/>
	<Table name="tblCache">
		<Map file="tblCache.mmap" capacity="1000" />
		<Flush modify="5" delete="5" checkout="5" />
		<Columns>
			<Field name="ObjectID"/>
			<Field name="FirstName" length="32"/>
			<Field name="LastName" length="32"/>
			<Field name="Sex"/>
			<Field name="LastModified"/>
		</Columns>
	</Table>
</Cache>
*/

class KTableCacheConfigure
{
public:
	typedef System::Collections::DynArray<TableCacheSetting*> SettingArray;
	typedef System::Collections::KMapByTree_staticPooled<ccstr,TableCacheSetting,32,KccstriCmp> Map;
	typedef System::Collections::KMapByVector2<ccstr,TableCacheSetting*,KccstriCmp> ClearNameMap;

public:
	KTableCacheConfigure();
	~KTableCacheConfigure();

public:
	bool Load(const char* xmlfile, KSQLConnection& cnn);
	const TableCacheSetting* getTableSetting(const char* tbname) const;
	const KSQLTableSchema* getTableSchema(const char* tbname) const;
	const KSQLTableSchema* getTableSchemaByClearName(const char* tbname) const;

	size_t getCacheCount() const;
	const KSQLTableSchema* getTableSchema(int idx) const;

private:
	bool _readOne(KSQLConnection& cnn, XMLElementNode* tableNode, TableCacheSetting& setting);

public:
	Map m_map;
	SettingArray m_array;
	ClearNameMap m_clearNameMap;
	XMLElementNode m_xmlRoot;
};

/// KCacheManager

class KCacheManager
{
public:
	typedef TableCacheSetting TableSetting;
	typedef KTableCacheConfigure::Map TableSettingMap;
	typedef System::Collections::KMapByTree_staticPooled<ccstr,KTableCacheReader*,32,KccstriCmp> Map;
	typedef System::Collections::DynArray<KTableCacheReader*,64,64> Array;

public:
	KCacheManager();
	~KCacheManager();

public:
	template <typename Creator> BOOL Initialize(const char* xmlfile, const KSQLConnParam& cnnParam)
	{
		m_cnnParam = cnnParam;

		KSQLConnection cnn;
		if(!cnn.Connect(cnnParam))
		{
			Log(LOG_ERROR, "error: connect to database, %s", cnn.GetLastErrorMsg());
			return FALSE;
		}

		if(!m_configure.Load(xmlfile, cnn))
			return FALSE;

		TableSettingMap::iterator it = m_configure.m_map.begin();
		TableSettingMap::iterator ite = m_configure.m_map.end();
		for(; it!=ite; it++)
		{
			TableSetting& tableSetting = it->second;
			tableSetting._connParam = &cnnParam;

			// 随便取一个表的DBName，假定大家都一样
			if(!m_dbname) m_dbname = tableSetting.m_schema.m_dbName.c_str();
			KTableCacheReader* tableCache = this->_create<Creator>(tableSetting);
			if(!tableCache)
			{
				Log(LOG_ERROR, "error: carete table cache for %s", tableSetting.m_tbname);
				return FALSE;
			}

			m_map[tableSetting.m_tbname] = tableCache;
			m_tableCaches.push_back(tableCache);
		}
		return TRUE;
	}
	void Finalize();

public:
	const KSQLConnParam& GetConnectionParam() const;
	KTableCacheReader* GetTableCache(const char* tableName);

public:
	size_t GetCount() const;
	KTableCacheReader* GetTableCache(int_r i);

private:
	template <typename Creator> KTableCacheReader* _create(TableSetting& tableSetting)
	{
		Creator creator;
		return creator.Create(tableSetting);
	}

public:
	Map m_map;
	Array m_tableCaches;
	const char* m_dbname;
	KSQLConnParam m_cnnParam;
	KTableCacheConfigure m_configure;
};