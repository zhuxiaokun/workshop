#ifndef _K_CACHE_OBJECT_H_
#define _K_CACHE_OBJECT_H_

#include <System/KType.h>
#include "KTableCacheReader.h"
#include "KTableRecordData.h"

/**
 * 共享内存中一条记录的内存映像的格式
 *
 * 1. 边界是与BYTE对齐的 pack(1)
 * 2. 字符串类型的字段采用的前面两个字节的长度，后面跟没有结束符的字符串内容
 * 3. 二进制的字段采用前面两个字节的长度，后面跟内容
 * 4. 字符串类型需要注意数据库表使用的字符集，如GB2312的一个字符是两个BYTE
 * 
 * 如果想把共享内存中的记录直接映射为应用程序中的结构，
 * 必须遵从上面的上个原则，否则后果不可预料
 */

#pragma pack(push,1)
template <size_t size> struct KDBString // size 可容纳的数据长度
{
	WORD actualLength;  // 实际数据长度
	char strData[size]; // 不包含结束字符
	BOOL Set(const char* str, size_t len=0)
	{
		if(!len) len = strlen(str);
		if(len < 0 || len > size) return FALSE;
		memcpy(strData, str, len);
		actualLength = (WORD)len;
		return TRUE;
	}
	const char* Get(char* buf) const
	{
		memcpy(buf, strData, actualLength);
		buf[actualLength] = '\0';
		return buf;
	}
	const char* Get(char* buf, int bufferLen) const
	{
		ASSERT_RETURN(bufferLen >= actualLength + 1, "");
		memcpy(buf, strData, actualLength);
		buf[actualLength] = '\0';
		return buf;
	}
};
struct KDBBaseBinary
{
	WORD actualLength;  // 实际数据长度
	char binData[1];
};
template <int size> struct KDBBinary : public KDBBaseBinary// size 可容纳的数据长度
{

	char _binData[size-1];
	BOOL Set(void* pData, int len)
	{
		if(len < 0 || len > size) return FALSE;
		memcpy(binData, pData, len);
		actualLength = len;
		return TRUE;
	}
	int Get(void*& buf)
	{
		buf = &binData[0];
		return actualLength;
	}
	int GetSize() const
	{
		return size;
	}
};
struct KDbBinaryHead
{
	WORD length;
	char data[2];
};
struct KDBTimestamp
{
	MYSQL_TIME_K val;
	BOOL Set(TIME_T t);
	TIME_T Get() const;
};
#pragma pack(pop)

class KCacheObject
{
public:
	KCacheObject();
	virtual ~KCacheObject();

public:
	// 整理记录的数据，主要功能是修正Binary字段的真实长度
	// 派生类可以把Binary字段的长度调整到和游戏中实际用到的数据长度一致
	virtual void TrimRecordData();

public:
	BOOL Attach(KTableCacheReader* pCache, HANDLE hRec);
	void Detach();

	BOOL BeginUpdate();
	BOOL EndUpdate();
	BOOL BeginUpdate(int fieldIdx);
	BOOL EndUpdate(int fieldIdx);
	BOOL Delete();
	BOOL Checkout();

	BOOL SetData(KTableRecordData& recData);
	BOOL SetUpdateData(KTableRecordData& recData);

public:
	BOOL GetInteger(const char* colName, int& value);
	BOOL GetInt64(const char* colName, INT64& value);
	BOOL GetFloat32(const char* colName, float& value);
	BOOL GetFloat64(const char* colName, double& value);
	BOOL GetString(const char* colName, char* value, int len, int& readed);
	BOOL GetBinary(const char* colName, void* value, int len, int& readed);
	BOOL GetBinary(const char* colName, int offset, void* value, int len, int& readed);

	BOOL GetTimestamp(const char* colName, time_t& value);

public:
	BOOL GetInteger(int colIdx, int& value);
	BOOL GetInt64(int colIdx, INT64& value);
	BOOL GetFloat32(int colIdx, float& value);
	BOOL GetFloat64(int colIdx, double& value);
	BOOL GetString(int colIdx, char* value, int len, int& readed);
	BOOL GetBinary(int colIdx, void* value, int len, int& readed);
	BOOL GetBinary(int colIdx, int offset, void* value, int len, int& readed);

	BOOL GetTimestamp(int colIdx, time_t& value);

public:
	BOOL SetInteger(const char* colName, int value);
	BOOL SetInt64(const char* colName, INT64 value);
	BOOL SetFloat32(const char* colName, float  value);
	BOOL SetFloat64(const char* colName, double value);
	BOOL SetString(const char* colName, const char* value, int len=0);
	BOOL SetBinary(const char* colName, const void* value, int len);
	BOOL SetBinary(const char* colName, int offset, const void* value, int len);

	BOOL SetTimestamp(const char* colName, time_t value);

public:
	BOOL SetInteger(int colIdx, int value);
	BOOL SetInt64(int colIdx, INT64 value);
	BOOL SetFloat32(int colIdx, float value);
	BOOL SetFloat64(int colIdx, double value);
	BOOL SetString(int colIdx, const char* value, int len=0);
	BOOL SetBinary(int colIdx, const void* value, int len);
	BOOL SetBinary(int colIdx, int offset, const void* value, int len);

	BOOL SetTimestamp(int colIdx, time_t value);

public:
	HANDLE m_hRec;
	KTableCacheReader* m_pCache;

public:
	// 把记录的内存映像直接映射为一个数据结构
	// 慎用，请参看上面提到的几条原则
	template <typename T> BOOL BindRecordTo(T*& ptr)
	{
		size_t recSize = m_pCache->GetRecordDataLength();
		if(recSize < sizeof(T)) return FALSE;

		void* pRecData = m_pCache->GetRecordData(m_hRec);
		if(!pRecData) return FALSE;
		ptr = (T*)pRecData;
		return TRUE;
	}

	// 把字段的内存绑定到一个数据类型的指针
	// 对于String和Binary类型，请使用KDBString和KDBBinary
	template <typename T> BOOL BindColumnTo(const char* colName, T*& ptr)
	{
		int fldIndex = m_pCache->GetFieldIndex(colName);
		if(fldIndex < 0) return FALSE;
		return this->BindColumnTo(fldIndex, ptr);
	}

	template <typename T> BOOL BindColumnTo(int colIdx, T*& ptr)
	{
		int colCount = m_pCache->m_tableSchema->GetFieldCount();
		if(colIdx < 0 || colIdx >= colCount) return FALSE;

		const KSQLTableField* pfld = m_pCache->m_tableSchema->GetField(colIdx);
		switch(pfld->m_cdt)
		{
		case KSQLTypes::sql_c_string:
		case KSQLTypes::sql_c_varstring:
		case KSQLTypes::sql_c_binary:
			{
				if(pfld->m_length+sizeof(WORD) < sizeof(T)) return FALSE;
				break;
			}
		case KSQLTypes::sql_c_timestamp:
			ASSERT(sizeof(T) == sizeof(MYSQL_TIME_K));
			break;
		default:
			{
				if(pfld->m_length < sizeof(T)) return FALSE;
				break;
			}
		}

		KTableCacheReader::KRecordHead* pHead =  m_pCache->GetRecordHead(m_hRec);
		if(!pHead) return FALSE;

		//char* pColData = (char*)pHead + m_pCache->m_dataOffsets[colIdx];
		//ptr = (T*)pColData;
		ptr = (T*)m_pCache->_getColumnData(pHead, colIdx);

		return TRUE;
	}

	// 把Binary类型的字段绑定到结构上，这里的结构不包含前面的WORD (actualLength)
	// 绑定完成后，会把actualLength置为sizeof(T)
	template <typename T> BOOL BindBinaryTo(const char* colName, T*& ptr)
	{
		int fldIndex = m_pCache->GetFieldIndex(colName);
		if(fldIndex < 0) return FALSE;
		return this->BindBinaryTo(fldIndex, ptr);
	}

	template <typename T> BOOL BindBinaryTo(int colIdx, T*& ptr)
	{
		int colCount = m_pCache->m_tableSchema->GetFieldCount();
		if(colIdx < 0 || colIdx >= colCount) return FALSE;

		const KSQLTableField* pfld = m_pCache->m_tableSchema->GetField(colIdx);
		if(pfld->m_cdt != KSQLTypes::sql_c_binary) return FALSE;

		if(pfld->m_length < sizeof(T)) return FALSE;

		KTableCacheReader::KRecordHead* pHead =  m_pCache->GetRecordHead(m_hRec);
		if(!pHead) return FALSE;

		//char* pColData = (char*)pHead + m_pCache->m_dataOffsets[colIdx];
		//ptr = (T*)(pColData+sizeof(WORD));
		WORD* pCol = (WORD*)m_pCache->_getColumnData(pHead, colIdx);
		ptr = (T*)(pCol + 1);

		if(*pCol == sizeof(T)) return TRUE;

		if(this->BeginUpdate(colIdx))
		{
			*pCol = sizeof(T);
			this->EndUpdate(colIdx);
			return TRUE;
		}
		
		return FALSE;
	}
};

#endif
