#ifndef _K_VAR_BINARY_H_
#define _K_VAR_BINARY_H_

#include <System/KType.h>
#include <System/KMacro.h>
#include <System/File/KMMap.h>
#include <System/Memory/KMemoryRecordset.h>
#include <System/Collections/DynArray.h>
#include <System/Sync/KProcessMutex.h>

class KTableCache;
class KVarBinaryMemory;

///////////////////////////////////////////
// HANDLE : 0 is an invalid handle
///////////////////////////////////////////
class KMmapRecordset
{
public:
	enum { invalid_record_no = 0x7fffffff };
	struct Record
	{
		struct
		{
			DWORD usedTag : 1;
			DWORD next    : 31;
		};
		WORD actualSize;
		char data[2];
	};

public:
	KMmapRecordset();
	~KMmapRecordset();

public:
	bool initialize(KVarBinaryMemory& vbm, int fno);
	void finalize();
	void clear();

public:
	const char* _getName(KVarBinaryMemory& vbm, int fno, char* buf);
	Record* _handle2Rec(HANDLE hRec);
	HANDLE _allocRecord();
	void _freeRecord(HANDLE hRec);

public:
	KMMap m_mmap;
	System::Memory::KMemoryRecordset m_mrs;
};

// 给VarBinary提供内存
// 管理所有的为varbinary服务的所有KMmapRecordset对象
class KVarBinaryMemory
{
public:
	enum { invalid_record_no = KMmapRecordset::invalid_record_no };
	typedef KMmapRecordset::Record Record;
	typedef System::Collections::DynArray<HANDLE,1024> HandleArray;
	typedef System::Collections::DynArray<KMmapRecordset*> MrsArray;

public:
	struct CtrlFile
	{
		volatile int fileCount;
	};

public:
	KVarBinaryMemory();
	~KVarBinaryMemory();

public:
	bool initialize(const char* fname, int pieceSize, int capacity);
	void finalize();
	void clear();			// remove m-maps files and record-sets
	void clearDataOnly();	// remove data only, keep m-maps and record-sets

public:
	HANDLE allocMemory(int size);
	HANDLE reallocMemory(HANDLE hRec, int size);
	HANDLE assureMemory(HANDLE hRec, int size);		// assure a memory handle has size length, realloc when need
	bool checkMemory(HANDLE hRec);					// check whether a memory handle is valid
	int getDataSize(HANDLE hRec);					// get data size stored in the memory specified by handle
	void freeMemory(HANDLE hRec);

public:
	bool _setValue(HANDLE hRec, const void* val, int len);
	bool _setValue(HANDLE hRec, int pos, const void* val, int len);
	int _getValue(HANDLE hRec, void* val, int len);
	int _getValue(HANDLE hRec, int pos, void* val, int len);

private:
	HANDLE _reallocMemory(HANDLE hRec, int size);

private:
	void _setRecordValue(Record* pRec, const void* p, int len);
	void _setRecordValue(Record* pRec, int pos, const void* p, int len);

private:
	KMmapRecordset* _createRecordset(int fileno, bool sync);
	Record* _handle2Record(HANDLE hRec);
	HANDLE _allocOneRecord();
	void _freeOneRecord(HANDLE hRec);
	bool _checkOneHandle(HANDLE hRec);
	HANDLE _linkTmpArray(int beg);
	int _link2TmpArray(HANDLE hRec);
	void _freeTmpArray(int beg);
	HANDLE _subHandle(HANDLE hRec, KMmapRecordset** pp);
	HANDLE _uniHandle(HANDLE hRec, int fno);
	const char* _getCtrlFname(char* buf);
	bool _syncWithCtrlFile();

public:
	bool initCtrlFile();

public:
	char m_fname[512];
	int m_recSize;
	int m_stepCapacity;
	
	MrsArray m_mrsArray;

	KMMap m_ctrlMmap;
	CtrlFile* m_ctrlFile;

	KProcessMutex m_mx;
	HandleArray m_tmpHarray;
};

class KVarBinary
{
public:
	KVarBinary();
	KVarBinary(const KVarBinary& o);
	KVarBinary(KVarBinaryMemory& vbm);
	KVarBinary(KVarBinaryMemory& vbm, HANDLE hRec);
	KVarBinary(KVarBinaryMemory& vbm, HANDLE* hptr);
	~KVarBinary();

public:
	KVarBinary& operator = (const KVarBinary& o);
	bool isValid() const;
	bool hasData() const;
	bool checkMemory();

public:
	void attach(KVarBinaryMemory& vbm);
	void attach(KVarBinaryMemory& vbm, HANDLE hRec);
	void attach(KVarBinaryMemory& vbm, HANDLE* hptr);
	void clear();
	
public:
	bool setValue(const void* val, int len);
	bool setValue(int pos, const void* val, int len);
	int  getValue(void* val, int len);
	int  getValue(int pos, void* val, int len);
	int size() const;

private:
	HANDLE _handle() { return *m_hptr; }
	HANDLE _handle() const { return *m_hptr; }
	void _setHandle(HANDLE h) { *m_hptr = h; }

public:
	HANDLE m_hRec;
	HANDLE* m_hptr;
	KVarBinaryMemory* m_vbm;
};

#endif