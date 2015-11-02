#pragma once

#include <System/KType.h>
#include <System/KMacro.h>
#include <System/Memory/KDataBuffer.h>
#include <System/Misc/KStream.h>
#include "KDatabase.h"

class KTableCacheReader;
class KTableRecordData
{
public:
	#pragma warning(disable:4200)
	struct Field
	{
		DWORD fieldIndex : 16;
		DWORD length     : 16;
		char value[0];
	//-------------------------------------------------------
		size_t size() const { return sizeof(Field)+length; }
	};
	#pragma warning(default:4200)

public:
	KTableRecordData();
	~KTableRecordData();

public:
	static size_t calcCapacity(const KSQLTableSchema& schema);

public:
	void attach(const void* pData, size_t capacity, size_t len);
	BOOL copyKey(const KSQLTableSchema& schema, KTableRecordData& o);

	int getFieldCount();

public:
	bool resize(size_t capacity);
	size_t capacity() const;

	size_t size() const;
	const void* dataPtr() const;
	void clear();

public: // read
	UINT64 version();
	Field* begin();
	Field* next(Field* field);
	size_t tail(Field* field, void** ppData); // 获得记录数据后的附加数据
	KMemoryStream& tail_r(Field* field);
	KMemoryStream& tail_r();

public: // write
	void setVersion(UINT64 ver);
	void updateVersion(UINT64 ver);
	void append(int_r fieldIndex, const void* data, size_t len);
	void append(KTableCacheReader* tableCache, HANDLE h, int_r fieldIndex);
	void eof();
	KMemoryStream& tail_w();

public:
	BOOL pushData(const void* pData, size_t len);

public:
	const char* m_tbname;
	System::Memory::KDataBuffer* m_dataBuffer;
	System::Memory::KDataBuffer m_tmpBuffer;
	System::Memory::KDataBufferImpl* m_bufferImpl;
	KMemoryStream m_tailr;
	KMemoryStream m_tailw;
};