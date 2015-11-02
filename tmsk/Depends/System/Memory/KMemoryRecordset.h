#ifndef _K_MEMORY_RECORDSET_H_
#define _K_MEMORY_RECORDSET_H_

#include "../KMacro.h"
#include "../KType.h"

namespace System { namespace Memory {

/**
 * 可以分配固定大小内存块的内存管理器
 * 在初始化是会预分配一大块内存，然后每次分配大小固定的一块
 * 较之从Heap中动态分配，性能要好
 * 局限就是只能分配固定大小的内存，并且总量也是固定的
 * 最小的分配单元4字节
 */

struct RecordsetHeader
{
	volatile DWORD usedCount;	// 已使用的记录数量
	DWORD firstFree;			// 空闲记录链表头
	DWORD recSize;				// 记录的长度，包括记录的头部
	DWORD capacity;				// 最多可容纳多少条记录
};

class KMemoryRecordset
{
private:
	enum { invalid_rec_no = 0x7fffffff };
	struct MemoryRecord
	{
		union 
		{
			DWORD flag;
			char  rec[1];
		} data;
	
		void SetNextNo(DWORD nextNo)
		{
			data.flag = nextNo;
		}
		DWORD NextNo()
		{
			return data.flag;
		}
	};

public:
	KMemoryRecordset();
	~KMemoryRecordset();

public:
	BOOL Initialize(int recSize, int capacity);
	BOOL Attach(RecordsetHeader& header, void* startAddr);
	void Detach();

public:
	void Finalize();
	void Clear();

public:
	BOOL IsResponsible(const void* pData) const;

	HANDLE AllocRecord();
	BOOL   FreeRecord(HANDLE hData);
	void*  GetRecordData(HANDLE hData);
	HANDLE GetHandle(void* p);

	int GetRecordNo(HANDLE h);
	HANDLE GetHandle(int recordNo);

public:
	HANDLE HandleFromRecNo(int recNo);
	DWORD  RecNoFromHandle(HANDLE hData);
	DWORD  GetTotalMemorySize(int recSize, int capacity);
	HANDLE AllocBottom();
	HANDLE AllocFree();

public:
	//int m_recSize;
	//int m_capacity;
	//int m_count;
	//DWORD m_firstFree; // first free record no

	RecordsetHeader m_header;
	RecordsetHeader* m_myHeader;

	char* m_pRecordset;
	char* m_endPointer;

	BOOL m_resposible; // responsible for memory cleanup
};

}}

#endif