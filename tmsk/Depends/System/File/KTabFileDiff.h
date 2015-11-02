#ifndef _K_TABFILE_DIFF_H_
#define _K_TABFILE_DIFF_H_

#include "KTabFileReader.h"

enum enum_Line_Operation
{
	enum_Line_Invalid,
	enum_Line_Add,
	enum_Line_Update,
	enum_Line_Delete,
};

enum enum_Col_Operation
{
	enum_Col_Invalid,
	enum_Col_Insert,
	enum_Col_Update,
	enum_Col_Delete,
};

struct KHeaderColDiff
{
	enum_Col_Operation op;
	KColumnDeclare* pColDecl;
};

struct KBodyColDiff
{
	int colIndex;
	enum_Col_Operation op;
	char* colVal;
};

struct KBodyLineDiff
{
	char* keyVal;
	enum_Line_Operation op;
	int colCount;
	KBodyColDiff* pDiffs;
};

DECLARE_SIMPLE_TYPE(KHeaderColDiff);
DECLARE_SIMPLE_TYPE(KBodyColDiff);
DECLARE_SIMPLE_TYPE(KBodyLineDiff);

class KTabFileDiff
{
public:
	typedef char* CharPtr;
	typedef ::System::Collections::DynArray<CharPtr,256,256> KCstrArray;
	typedef ::System::Collections::DynArray<KHeaderColDiff> KHeaderDiff;
	typedef ::System::Collections::DynArray<KBodyColDiff> KBodyColDiffArray;
	typedef ::System::Collections::DynArray<KBodyLineDiff> KBodyDiff;
	typedef KTabFileReader::StringArray KBodyColumns;
	typedef ::System::Collections::DynArray<KBuffer64k*> KBufferArray;

public:
	KTabFileDiff();
	~KTabFileDiff();

public:
	BOOL Diff(const char* baseFile, const char* newFile);
	BOOL Diff(KTabFileReader& file1, KTabFileReader& file2);
	BOOL IsDifferent() const;
	BOOL WriteResult(const char* filename);

public:
	void Clear();
	BOOL DiffHeader();
	BOOL DiffBody();
	BOOL DiffBodyLine(KBodyColumns& cols1, KBodyColumns& cols2);

public:
	char* WriteValue(const char* val, int len=-1);

public:
	BOOL Compact_HeaderDiff(KHeaderDiff& headerDiff);
	BOOL Compact_LineDiff(KBodyLineDiff& lineDiff);

public:
	BOOL Write_Key_DataType(StreamInterface& so);
	BOOL Write_Header_Diff(StreamInterface& so);
	BOOL Write_Body_Diff(StreamInterface& so);
	BOOL Write_BodyLine_Diff(StreamInterface& so, KBodyLineDiff& lineDiff);

public:
	KTabFileReader* m_file1;
	KTabFileReader* m_file2;

	KBodyColDiffArray m_currLineDiff;
	KCstrArray m_currMergedLine;

	// Diff的结果
	int m_keyIndex;
	enum_Col_DataType m_keyDataType;
	KHeaderDiff m_headerDiff;
	KBodyDiff m_bodyDiff;

	// 使用的内存
	KBufferArray m_bufferArray;
};

struct KHeaderDiffUnit
{
	enum_Col_Operation op;
	int colIndex;
	char* colName;
};

DECLARE_SIMPLE_TYPE(KHeaderDiffUnit);

class KDiffFileReader
{
public:
	typedef ::System::Collections::DynArray<KHeaderDiffUnit> KHeaderDiff;
	typedef ::System::Collections::DynArray<KBodyColDiff> KBodyColDiffArray;
	typedef ::System::Collections::DynArray<KBuffer64k*> KBufferArray;

	class KLineKeyCmp
	{
	public:
		enum_Col_DataType m_keyDatatype;
		BOOL operator () (const KBodyLineDiff& a, const KBodyLineDiff& b) const
		{
			const char* key_a = a.keyVal;
			const char* key_b = b.keyVal;

			switch(m_keyDatatype)
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

	typedef ::System::Collections::DynSortedArray<KBodyLineDiff,KLineKeyCmp,1024,1024> KBodyDiff;
	typedef ::System::Collections::DynArray<KBodyLineDiff> KNewBodyDiff;

public:
	KDiffFileReader();
	~KDiffFileReader();
	void Clear();

public:
	BOOL Read(StreamInterface& si);
	KHeaderDiffUnit* GetHeaderDiff(int colIndex);
	KBodyLineDiff* GetLineDiff(const char* key);

public:
	BOOL ParseKeyDatatype(char** cols, int size);
	BOOL ParseHeaderDiff(char** cols, int size);
	BOOL ParseLineDiff(char** cols, int size);
	BOOL ParseLineInsert(char** cols, int size);
	BOOL ParseLineDelete(char** cols, int size);
	BOOL ParseLineUpdate(char** cols, int size);
	BOOL ParseLineColDiff(char* col);

public:
	char* WriteValue(const char* val, int len);

public:
	int m_currLine;
	enum_Col_DataType m_keyDatatype;

	// 列名差异中，删除列使用的是原始列序号
	// 增加和修改列，使用的是新的列序号
	KHeaderDiff m_headerDiff;
	KBodyDiff m_bodyDiff;
	KNewBodyDiff m_newBodyLines;

	KBodyColDiffArray m_currLineDiff;
	KBufferArray m_bufferArray;
};

struct KTabfileCol
{
	int colIdx;
	char* colName;
};
DECLARE_SIMPLE_TYPE(KTabfileCol);

class KTabfileMergeReader
{
public:
	typedef char* Charp;
	typedef ::System::Collections::DynSortedArray<int> KIntArray;
	typedef ::System::Collections::DynArray<Charp> KColArray;
	typedef ::System::Collections::DynArray<Charp> KValArray;
	typedef ::System::Collections::DynArray<KBuffer64k*> KBufferArray;

	class KColCmp
	{
	public:
		BOOL operator () (const KTabfileCol& a, const KTabfileCol& b) const
		{
			return _stricmp(a.colName, b.colName) < 0;
		}
	};

	typedef ::System::Collections::DynSortedArray<KTabfileCol,KColCmp> KSortedColArray;

public:
	KTabfileMergeReader();
	~KTabfileMergeReader();
	void Clear();

public:
	BOOL Open(const char* baseFile,int keyIdx=0, enum_Col_DataType keydt=enum_DataType_String);
	BOOL Open(const char* baseFile, const char* diffFile,int keyIdx=0, enum_Col_DataType keydt=enum_DataType_String);
	BOOL Open(StreamInterface* psiTabfile, StreamInterface* psiDiff,int keyIdx=0, enum_Col_DataType keydt=enum_DataType_String);

	// return value:
	// -1   : error occured
	//  0   : eof, no line to read
	//  1   : success, there are more line to read
	int ReadLine();
	int m_nLineNum;
public:
	const char* GetColumn(int colIndex);
	const char* GetColumn(const char* colName);
	const char* GetColumnName(int colIdx) const;

public:
	BOOL ReadHeader();
	int ReadNewLine();
	int GetColIndex(const char* colName);
	char* WriteValue(const char* val, int len);
	void _Clear();

public:
	int m_keyIndex;
	enum_Col_DataType m_keyDatatype;
	int m_colSize;

	::System::File::KColumnReader m_colReader;
	KDiffFileReader m_diffReader;

	KColArray m_colArray;
	KSortedColArray m_sortedColArray;
	KIntArray m_delCols;

	int m_newLineIdx;
	KValArray m_currLine;
	KBufferArray m_bufferArray;

	KInputFileStream m_fiBase;
	KInputFileStream m_fiDiff;
};

#endif