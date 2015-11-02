#include "KCacheManager.h"
#include <System/Log/log.h>
#include <System/Memory/KBuffer64k.h>
#include "KTableTask.h"
/// KTableCacheConfigure

KTableCacheConfigure::KTableCacheConfigure()
{

}

KTableCacheConfigure::~KTableCacheConfigure()
{

}

bool KTableCacheConfigure::Load(const char* xmlfile, KSQLConnection& cnn)
{
	XMLDomParser parser;
	if(!parser.Initialize()) return false;
	
	KInputFileStream fi;
	if(!fi.Open(xmlfile))
	{
		Log(LOG_ERROR, "error: open %s, %s", xmlfile, strerror_r2(GetLastError()).c_str());
		return false;
	}

	if(!parser.Parse(fi, m_xmlRoot))
	{
		Log(LOG_ERROR, "error: parse %s, %s", xmlfile, parser.GetErrorMsg());
		return false;
	}

	TableCacheSetting tableSetting;
	XMLNodeColl coll = m_xmlRoot.QueryChildren("Cache/Table");
	int_r n = coll.Size();
	for(int_r i=0; i<n; i++)
	{
		tableSetting.clear(); tableSetting._nextTaskSlot = i;
		XMLElementNode* tableNode = (XMLElementNode*)coll.GetNode((int)i);
		if(!this->_readOne(cnn, tableNode, tableSetting))
			return false;

		m_map[tableSetting.m_tbname] = tableSetting;
		const char* pComma = strchr(tableSetting.m_tbname, '.');
		const char* clearName = pComma ? pComma+1 : tableSetting.m_tbname;
		m_clearNameMap[clearName] = &m_map[tableSetting.m_tbname];
		m_array.push_back(&m_map[tableSetting.m_tbname]);
	}
	
	ASSERT(KTableTask::max_task_queue >= n);
	return true;
}

const TableCacheSetting* KTableCacheConfigure::getTableSetting(const char* tbname) const
{
	const Map::const_iterator it = m_map.find(tbname);
	if(it == m_map.end()) return NULL;
	return &it->second;
}

const KSQLTableSchema* KTableCacheConfigure::getTableSchema(const char* tbname) const
{
	const TableCacheSetting* tbsetting = getTableSetting(tbname);
	if(!tbsetting) return NULL;
	return &tbsetting->m_schema;
}

const KSQLTableSchema* KTableCacheConfigure::getTableSchemaByClearName(const char* tbname) const
{
	ClearNameMap::const_iterator it = m_clearNameMap.find(tbname);
	if(it == m_clearNameMap.end()) return NULL;
	return &it->second->m_schema;
}

size_t KTableCacheConfigure::getCacheCount() const
{
	return m_array.size();
}

const KSQLTableSchema* KTableCacheConfigure::getTableSchema(int idx) const
{
	ASSERT(idx >= 0 && idx < m_array.size());
	return &m_array[idx]->m_schema;
}

bool KTableCacheConfigure::_readOne(KSQLConnection& cnn, XMLElementNode* pXml, TableCacheSetting& setting)
{
	setting.m_flushInterval = 5;
	const StringValue* pVal = pXml->QueryAtt("name");
	if(!pVal)
	{
		Log(LOG_ERROR, "error: table name not set");
		return false;
	}
	setting.m_tbname = KBuffer64k::WriteData(pVal->cstr(), pVal->Size()+1);

	const char* mapfile = NULL;
	pVal = pXml->QueryAtt("Map.file");
	if(!pVal)
	{
		Log(LOG_ERROR, "error: map file not set for table %s", setting.m_tbname);
		return false;
	}
	setting.m_mmapfile = KBuffer64k::WriteData(pVal->cstr(), pVal->Size()+1);

	int recordCapacity = 0;
	pVal = pXml->QueryAtt("Map.capacity");
	if(!pVal)
	{
		Log(LOG_ERROR, "error: cache table %s record capacity not set", setting.m_tbname);
		return false;
	}
	setting.m_capacity = pVal->Integer();
	
	// Windows测试环境下
	#ifdef _WIN32
	setting.m_capacity = 256;	
	#endif

	pVal = pXml->QueryAtt("Flush.modify");
	if(pVal) setting.m_flushInterval = pVal->Integer();

	XMLNodeColl coll = pXml->QueryChildren("Columns/Field");
	int fieldCount = coll.Size();
	if(!fieldCount)
	{
		Log(LOG_ERROR, "error: cache table %s has no fields", setting.m_tbname);
		return false;
	}

	KSQLTableSchema schema;
	if(!cnn.QueryTableSchema(setting.m_tbname, schema))
	{
		Log(LOG_ERROR, "error: cache table %s not found in database", setting.m_tbname);
		return FALSE;
	}

	KSQLTableSchema& m_schema = setting.m_schema;
	m_schema.m_name = schema.m_name;
	m_schema.m_dbName = schema.m_dbName;
	const char* tableName = setting.m_tbname;

	for(int i=0; i<fieldCount; i++)
	{
		XMLElementNode* pXmlField = (XMLElementNode*)coll.GetNode(i);

		const char* fldName;
		pVal = pXmlField->QueryAtt("name");
		if(!pVal)
		{
			Log(LOG_ERROR, "error: cache table %s field %d has no name", tableName, i);
			return false;
		}
		fldName = pVal->cstr();

		int length = 0;
		pVal = pXmlField->QueryAtt("length");
		if(pVal)
		{
			length = pVal->Integer();
		}

		const char* defaultVal = NULL;
		pVal = pXmlField->QueryAtt("default");
		if(pVal)
		{
			defaultVal = pVal->cstr();
		}

		const KSQLTableField* pfld = schema.GetField(fldName);
		if(!pfld)
		{
			Log(LOG_ERROR, "error: %s.%s not exist in database", tableName, fldName);
			return false;
		}

		KSQLTableField field(*pfld);
		switch(field.m_cdt)
		{
		case KSQLTypes::sql_c_string:
		case KSQLTypes::sql_c_varstring:
			{
				if(length) field.m_length = length;
				break;
			}
		case KSQLTypes::sql_c_binary:
			{
				if(!length)
				{
					Log(LOG_ERROR, "error: binary field %s must supply field length", fldName);
					return false;
				}
				field.m_length = length;
				break;
			}
		};
		if(defaultVal) field.m_defaultVal = defaultVal;
		m_schema.Append(field);
	}

	return true;
}

//// KCacheManager

KCacheManager::KCacheManager():m_dbname(NULL)
{

}

KCacheManager::~KCacheManager()
{
	this->Finalize();
}

void KCacheManager::Finalize()
{
	while(!m_map.empty())
	{
		Map::iterator it = m_map.begin();
		KTableCacheReader* tableCache = it->second;
		tableCache->finalize(); delete tableCache;
		m_map.erase(it);
	}
	m_tableCaches.clear();
}

const KSQLConnParam& KCacheManager::GetConnectionParam() const
{
	return m_cnnParam;
}

KTableCacheReader* KCacheManager::GetTableCache(const char* tableName)
{
	char buf[128];
	if(strchr(tableName, '.')) strcpy_k(buf, sizeof(buf), tableName);
	else sprintf_k(buf, sizeof(buf), "%s.%s", m_dbname, tableName);

	Map::iterator it = m_map.find(buf);
	if(it == m_map.end()) return NULL;
	return it->second;
}

size_t KCacheManager::GetCount() const
{
	return m_tableCaches.size();
}

KTableCacheReader* KCacheManager::GetTableCache(int_r i)
{
	if(i<0 || i>= m_tableCaches.size()) return NULL;
	return m_tableCaches.at((int)i);
}