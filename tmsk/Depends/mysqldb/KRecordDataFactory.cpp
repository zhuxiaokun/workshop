#include "KRecordDataFactory.h"

bool KRecordDataFactory::addTable(KSQLTableSchema& schema)
{
	const char* tbname = schema.m_name.c_str();
	Map::iterator it = m_map.find(tbname);
	if(it != m_map.end()) return false;
	
	PoolPair* poolPair = new PoolPair();
	m_map.insert(tbname, poolPair);
	
	poolPair->second = KTableRecordData::calcCapacity(schema);
	return true;
}

KTableRecordData* KRecordDataFactory::allocRecord(const char* tbname)
{
	PoolPair* ppair = this->_getPool(tbname);
	if(!ppair) return NULL;
	KTableRecordData* rec = ppair->first.Alloc();
	rec->resize(ppair->second);
	return rec;
}

bool KRecordDataFactory::freeRecord(const char* tbname, KTableRecordData* record)
{
	PoolPair* ppair = this->_getPool(tbname);
	if(!ppair) return false;
	ppair->first.Free(record);
	return true;
}

KRecordDataFactory::PoolPair* KRecordDataFactory::_getPool(const char* tbname)
{
	Map::iterator it = m_map.find(tbname);
	if(it == m_map.end()) return NULL;
	return it->second;
}