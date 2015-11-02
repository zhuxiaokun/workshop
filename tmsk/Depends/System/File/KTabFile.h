
/********************************************************************
	created:	2008/02/21
	created:	21:2:2008   14:42
	filename: 	Athena\Base\System\File\KTabFile.h
	file path:	Athena\Base\System\File
	file base:	KTabFile
	file ext:	h
	author:		xueyan
	
	purpose:	能够读取以tab制表符为分隔的文本文件，跨平台.
*********************************************************************/


#pragma once

#include "KFile.h"
#include "../Misc/KStream.h"
#include "KTabFileDiff.h"
#include "../Misc/StrUtil.h"

namespace System {			namespace File {

	/**
	 * // how to use it
	 * 
	 * const char* fileName = "./Skill/SkillCreateInfo.txt";
	 * 
	 * KTabFile2 tabFile;
	 * if(!tabFile.Open(fileName)) return FALSE;
	 * 
	 * int skillID;
	 * 
	 * while(TRUE)
	 * {
	 * 	int nRet = tabFile.ReadLine();
	 * 	if(nRet == -1) return FALSE; // error
	 * 	if(nRet == 0) break;         // over
	 * 
	 * 	tabFile.GetInteger("SkillID", 0, &skillID);
	 * }
	 *
	 * // other process
	 *
	 */
	class KTabFile2
	{
	public:
		KTabFile2();
		~KTabFile2();
		void Clear();

	public:
		BOOL Open(const char* pPath);
		BOOL OpenDirect(const char* pPath);

	public:
		BOOL Open(const char* baseFile, const char* diffFile);
		BOOL Open(StreamInterface& fiBase);
		BOOL Open(StreamInterface& fiBase, StreamInterface& fiDiff);

		int GetWidth();

		// return value:
		// -1   : error occured
		//  0   : eof, no line to read
		//  1   : success, there are more line to read
		int ReadLine();

	public:
		//
		// 返回值:
		// FALSE : 没找到，使用的是缺省值
		// TRUE  : 找到了
		//
		BOOL GetString(const char* szColumn, const char* lpDefault, char* lpRString, unsigned int dwSize);
		BOOL GetString(int nColumn, const char* lpDefault, char* lpRString, unsigned int dwSize);
		BOOL GetInteger(const char* szColumn, int nDefault, int *pnValue);
		BOOL GetInteger(int nColumn, int nDefault, int *pnValue);
		BOOL GetIntegerBySpecialVal(int nColumn, int nDefault, int *pnValue,INT nRadix);
		BOOL GetIntegerBySpecialVal(const char* szColumn, int nDefault, int *pnValue,INT nRadix);
		BOOL GetFloat(const char* szColumn, float fDefault, float *pfValue);
		BOOL GetFloat(int nColumn, float fDefault, float *pfValue);
		const char* GetColName(int colIdx) const;
		int  GetColumnIndex(const char* szColumn);

	public:
		KTabfileMergeReader m_merge;
	};

	// 假定第一列是 Key
	class KTabFile
	{
	public:
		typedef char* Charp;
		typedef char** Charpp;
		
		class KColCmp
		{
		public:
			BOOL operator () (const KTabfileCol& a, const KTabfileCol& b) const
			{
				return strcmp(a.colName, b.colName) < 0;
			}
		};
		class KCharppCmp
		{
		public:
			BOOL operator () (const Charpp& a, const Charpp& b) const
			{
				return _stricmp(a[0], b[0]) < 0;
			}
		};

		typedef ::System::Collections::DynSortedArray<KTabfileCol,KColCmp> KColArray;
		typedef ::System::Collections::DynArray<Charpp> KLineArray;
		typedef ::System::Collections::DynSortedArray<Charpp, KCharppCmp> KSortedLineArray;
		typedef ::System::Collections::DynArray<KBuffer64k*> KBufferArray;

	public:
		KTabFile();
		~KTabFile();

	public:
		BOOL Open(const char* filename);
		BOOL CreateFrom(KTabFile2& fileReader);
		void Clear();

	public:
		int  GetWidth();
		int	 GetHeight();
		BOOL GetString(int nRow, const char* szColumn, const char* lpDefault, char* lpRString, unsigned int dwSize);
		BOOL GetString(int nRow, int nColumn, const char* lpDefault, char* lpRString, unsigned int dwSize);
		BOOL GetInteger(int nRow, const char* szColumn, int nDefault, int *pnValue);
		BOOL GetInteger(int nRow, int nColumn, int nDefault, int *pnValue);
		BOOL GetIntegerBySpecialVal(int nRow, int nColumn, int nDefault, int *pnValue,INT nRadix);
		BOOL GetIntegerBySpecialVal(int nRow, const char* szColumn, int nDefault, int *pnValue,INT nRadix);
		BOOL GetFloat(int nRow, const char* szColumn, float fDefault, float *pfValue);
		BOOL GetFloat(int nRow, int nColumn, float fDefault, float *pfValue);

	public:
		BOOL GetIntegerByKey(int key, const char* colName, int* pVal);
		BOOL GetFloatByKey(int key, const char* colName, float* pVal);
		BOOL GetStringByKey(int key, const char* colName, char* ret, int len);
		BOOL GetIntegerByKey(const char* key, const char* colName, int* pVal);
		BOOL GetFloatByKey(const char* key, const char* colName, float* pVal);
		BOOL GetStringByKey(const char* key, const char* colName, char* ret, int len);

	private:
		int GetColIndex(const char* colName);
		BOOL CheckRowCol(int rowNo, int colno);
		char* WriteValue(const void* pData, int len);

	public:
		int m_width, m_height;
		KColArray m_headers;
		KLineArray m_lines;
		KSortedLineArray m_sortedLines;
		static KBufferArray m_bufferArray;
	};

} /* end File */		} /* System */

// using namespace ::System::File;