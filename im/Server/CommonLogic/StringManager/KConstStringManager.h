#pragma once
#include <System/Collections/KHashTable.h>
#include <System/Memory/KBuffer64k.h>

class KConstStringManager
{
public:
	typedef JG_C::KHashTable<ccstr,bool,KccstrCmp,KStringHasher<ccstr>,JG_S::KNoneLock,2048,2048> Table;

protected:
	Table m_table;
	KBuffer64k* m_buff;

private:
	KConstStringManager(const KConstStringManager&);
	KConstStringManager& operator = (const KConstStringManager&);

public:
	KConstStringManager();
	~KConstStringManager();

public:
	static KConstStringManager& GetInstance();

public:
	const char* getConstString(const char* str) const;
	const char* getConstStringForce(const char* str);
};