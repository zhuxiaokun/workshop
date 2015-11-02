#pragma once

#include <System/Memory/KStepObjectPool.h>
#include <System/Sync/KSync.h>
#include <System/Collections/KMapByVector.h>
#include "KDatabase.h"
#include "KTableRecordData.h"

class KRecordDataFactory
{
public:
	typedef System::Memory::KStepObjectPool<KTableRecordData,32,System::Sync::KMTLock> Pool;
	typedef KPair<Pool,int_r> PoolPair;
	typedef System::Collections::KMapByVector2<ccstr,PoolPair*,KccstrCmp> Map;

public:
	bool addTable(KSQLTableSchema& schema);
	KTableRecordData* allocRecord(const char* tbname);
	bool freeRecord(const char* tbname, KTableRecordData* record);

private:
	PoolPair* _getPool(const char* tbname);

public:
	Map m_map;
};