#ifndef _K_TABFILE_READER_H_
#define _K_TABFILE_READER_H_

#include "./KTextReader.h"
#include "../Memory/RingBuffer.h"
#include "../Memory/KStepObjectPool.h"
#include "../Collections/DynArray.h"
#include "./KTextReader.h"
#include "../Memory/KBuffer64k.h"

// using namespace ::System::File;

enum enum_Col_DataType
{
	enum_DataType_Invalid,
	enum_DataType_Int,
	enum_DataType_Uint,
	enum_DataType_String,
	enum_DataType_Int64,
	enum_DataType_Uint64,
};

class KColumnDeclare
{
public:
	int index;
	char* name;
	enum_Col_DataType dataType;

public:
	KColumnDeclare();
	KColumnDeclare(const KColumnDeclare& o);
	KColumnDeclare(int idx, const char* nm, enum_Col_DataType dt);
};

DECLARE_SIMPLE_TYPE(KColumnDeclare);

class KTabFileReader
{
public:
	typedef char* CharPtr;
	typedef char** StringArray;

	class KColumnCmp
	{
	public:
		BOOL operator () (const KColumnDeclare& a, const KColumnDeclare& b) const
		{
			return strcmp(a.name, b.name) < 0;
		}
	};

	class KLineKeyCmp
	{
	public:
		KColumnDeclare m_keyCol;
		BOOL operator () (const StringArray& a, const StringArray& b) const
		{
			const char* key_a = a[m_keyCol.index];
			const char* key_b = b[m_keyCol.index];
			
			switch(m_keyCol.dataType)
			{
			case enum_DataType_Int:
			case enum_DataType_Uint:
			case enum_DataType_Int64:
			case enum_DataType_Uint64:
				return str2i64(key_a) < str2i64(key_b);
			case enum_DataType_String:
				return strcmp(key_a, key_b) < 0;
			default:
				ASSERT(FALSE);
				return TRUE;
			}
		}
	};

public:
	typedef ::System::Collections::DynArray<KColumnDeclare,64,64> KColumnArray;
	typedef ::System::Collections::DynSortedArray<StringArray,KLineKeyCmp,10240,10240> KLineArray;
	typedef ::System::Collections::DynSortedArray<KColumnDeclare,KColumnCmp,64,64> KSortedColumnArray;
	typedef ::System::Collections::DynArray<KBuffer64k*,64,64> KBufferArray;

public:
	KTabFileReader();
	~KTabFileReader();

public:
	BOOL SetKeyColumn(const char* colName, enum_Col_DataType dataType);
	BOOL Read(const char* filename);

public:
	StringArray FindLine(const char* key);

public:
	char* WriteValue(const char* val, int len=-1);
	BOOL ReadFileHeader(::System::File::KColumnReader& reader);
	BOOL ReadFileBody(::System::File::KColumnReader& reader);
	void Clear();

public:
	int m_keyColIndex;
	char m_keyColName[128];
	enum_Col_DataType m_keyDataType;

	int m_colSize;
	KColumnArray m_colArray;
	KSortedColumnArray m_sortedColArray;
	KLineArray m_lineArray;

	KBufferArray m_bufferArray;

	KStringList m_lineForCmp;
};

#endif