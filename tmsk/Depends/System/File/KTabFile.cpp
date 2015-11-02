#include "KTabFile.h"
#include "../KPlatform.h"
#include "../Log/log.h"

namespace System { namespace File {

KTabFile2::KTabFile2()
{

}

KTabFile2::~KTabFile2()
{

}

void KTabFile2::Clear()
{
	m_merge.Clear();
}

BOOL KTabFile2::Open(const char* pPath)
{
	return m_merge.Open(pPath, 0, enum_DataType_String);
}

BOOL KTabFile2::OpenDirect(const char* pPath)
{
	return m_merge.Open(pPath, 0, enum_DataType_String);
}

BOOL KTabFile2::Open(const char* baseFile, const char* diffFile)
{
	return m_merge.Open(baseFile, diffFile);
}

BOOL KTabFile2::Open(StreamInterface& fiBase)
{
	return m_merge.Open(&fiBase, NULL);
}

BOOL KTabFile2::Open(StreamInterface& fiBase, StreamInterface& fiDiff)
{
	ASSERT_I(IStringTranslator::GetGlobalTranslator());
	return m_merge.Open(&fiBase, &fiDiff);
}

int KTabFile2::GetWidth()
{
	return m_merge.m_colSize;
}

// return value:
// -1   : error occured
//  0   : eof, no line to read
//  1   : success, there are more line to read
int KTabFile2::ReadLine()
{
	return m_merge.ReadLine();
}

BOOL KTabFile2::GetString(const char* szColumn, const char* lpDefault, char* lpRString, unsigned int dwSize)
{
	const char* val = m_merge.GetColumn(szColumn);
	if(val)
	{
		IStringTranslator* pTranslator = IStringTranslator::GetGlobalTranslator();
		if(pTranslator)
		{
			const char* s = pTranslator->Translate(val);
			if(s && s != val)
			{
				val = s;
			}
		}

		g_StrCpyLen(lpRString, val, dwSize);
		return TRUE;
	}
	else
	{
		g_StrCpyLen(lpRString, lpDefault, dwSize);
		return FALSE;
	}
}

BOOL KTabFile2::GetString(int nColumn, const char* lpDefault, char* lpRString, unsigned int dwSize)
{
	const char* val = m_merge.GetColumn(nColumn);
	if(val)
	{
		IStringTranslator* pTranslator = IStringTranslator::GetGlobalTranslator();
		if(pTranslator)
		{
			const char* s = pTranslator->Translate(val);
			if(s && s != val)
			{
				val = s;
			}
		}

		g_StrCpyLen(lpRString, val, dwSize);
		return TRUE;
	}
	else
	{
		g_StrCpyLen(lpRString, lpDefault, dwSize);
		return FALSE;
	}
}

BOOL KTabFile2::GetInteger(const char* szColumn, int nDefault, int *pnValue)
{
	const char* val = m_merge.GetColumn(szColumn);
	if(val)
	{
		*pnValue = str2int(val);
		return TRUE;
	}
	else
	{
		*pnValue = nDefault;
		return FALSE;
	}
}

BOOL KTabFile2::GetInteger(int nColumn, int nDefault, int *pnValue)
{
	const char* val = m_merge.GetColumn(nColumn);
	if(val)
	{
		*pnValue = str2int(val);
		return TRUE;
	}
	else
	{
		*pnValue = nDefault;
		return FALSE;
	}
}

BOOL KTabFile2::GetIntegerBySpecialVal(int nColumn, int nDefault, int *pnValue,INT nRadix)
{
	const char* val = m_merge.GetColumn(nColumn);
	if(val)
	{
		if(!isnumchar(val[0])) val += 1;
		*pnValue = strtol(val, NULL, nRadix);
		return TRUE;
	}
	else
	{
		*pnValue = nDefault;
		return FALSE;
	}
}

BOOL KTabFile2::GetIntegerBySpecialVal(const char* szColumn, int nDefault, int *pnValue,INT nRadix)
{
	const char* val = m_merge.GetColumn(szColumn);
	if(val)
	{
		if(!isnumchar(val[0])) val += 1;
		*pnValue = strtol(val, NULL, nRadix);
		return TRUE;
	}
	else
	{
		*pnValue = nDefault;
		return FALSE;
	}
}

BOOL KTabFile2::GetFloat(const char* szColumn, float fDefault, float *pfValue)
{
	const char* val = m_merge.GetColumn(szColumn);
	if(val)
	{
		*pfValue = (float)strtod(val, NULL);
		return TRUE;
	}
	else
	{
		*pfValue = fDefault;
		return FALSE;
	}
}

BOOL KTabFile2::GetFloat(int nColumn, float fDefault, float *pfValue)
{
	const char* val = m_merge.GetColumn(nColumn);
	if(val)
	{
		*pfValue = (float)strtod(val, NULL);
		return TRUE;
	}
	else
	{
		*pfValue = fDefault;
		return FALSE;
	}
}

const char* KTabFile2::GetColName(int colIdx) const
{
	return ((KTabFile2*)this)->m_merge.GetColumnName(colIdx);
}

int  KTabFile2::GetColumnIndex(const char* szColumn)
{
	return m_merge.GetColIndex(szColumn);
}

KTabFile::KTabFile():m_width(0),m_height(0)
{

}

KTabFile::~KTabFile()
{

}

BOOL KTabFile::Open(const char* filename)
{
	KTabFile2 f2;
	if(!f2.Open(filename)) return FALSE;
	return this->CreateFrom(f2);
}

BOOL KTabFile::CreateFrom(KTabFile2& fileReader)
{
	this->Clear();

	KTabfileCol fileCol;
	int nCol = fileReader.m_merge.m_sortedColArray.size();
	if(nCol < 1) return TRUE;

	for(int i=0; i<nCol; i++)
	{
		KTabfileCol& col = fileReader.m_merge.m_sortedColArray[i];
		fileCol.colIdx = col.colIdx;
		fileCol.colName = this->WriteValue(col.colName, (int)strlen(col.colName)+1);
		m_headers.insert_unique(fileCol);
	}

	m_width = m_headers.size();

	int nLineNum = 1;
	while(TRUE)
	{
		int nRet = fileReader.ReadLine();
		if(nRet == -1) return FALSE;
		if(nRet == 0) break;

		nLineNum++;
		char** pp = &fileReader.m_merge.m_currLine[0];
		int len = sizeof(Charp) * fileReader.m_merge.m_currLine.size();
		Charpp cols = (Charpp)this->WriteValue((const void*)pp, len);

		for(int i=0; i<m_width; i++)
		{
			cols[i] = this->WriteValue(cols[i], (int)strlen(cols[i])+1);
		}

		m_lines.push_back(cols);

		if (m_sortedLines.insert_unique(cols) < 0)
		{
			Log(LOG_FATAL, "第%d行的[%d]重复", nLineNum, cols[0]);
			// ASSERT(FALSE); 允许重复
		}
	}

	m_height = m_lines.size() + 1;

	return TRUE;
}

void KTabFile::Clear()
{
	m_width = m_height = 0;
	m_headers.clear();
	m_lines.clear();
	m_sortedLines.clear();
}

int KTabFile::GetWidth()
{
	return m_width;
}

int	KTabFile::GetHeight()
{
	return m_height;
}

BOOL KTabFile::GetString(int nRow, const char* szColumn, const char* lpDefault, char* lpRString, unsigned int dwSize)
{
	int colNo = this->GetColIndex(szColumn);
	if(!this->CheckRowCol(nRow, colNo))
	{
		g_StrCpyLen(lpRString, lpDefault, dwSize);
		return FALSE;
	}
	else
	{
		const char* val = m_lines[nRow-2][colNo];

		IStringTranslator* pTranslator = IStringTranslator::GetGlobalTranslator();
		if(pTranslator)
		{
			const char* s = pTranslator->Translate(val);
			if(s && s != val)
			{
				val = s;
			}
		}

		g_StrCpyLen(lpRString, val, dwSize);
		return TRUE;
	}
}

BOOL KTabFile::GetString(int nRow, int nColumn, const char* lpDefault, char* lpRString, unsigned int dwSize)
{
	if(!this->CheckRowCol(nRow, nColumn))
	{
		g_StrCpyLen(lpRString, lpDefault, dwSize);
		return FALSE;
	}
	else
	{
		const char* val = m_lines[nRow-2][nColumn];

		IStringTranslator* pTranslator = IStringTranslator::GetGlobalTranslator();
		if(pTranslator)
		{
			const char* s = pTranslator->Translate(val);
			if(s && s != val)
			{
				val = s;
			}
		}

		g_StrCpyLen(lpRString, val, dwSize);
		return TRUE;
	}
}

BOOL KTabFile::GetInteger(int nRow, const char* szColumn, int nDefault, int *pnValue)
{
	int colNo = this->GetColIndex(szColumn);
	if(!this->CheckRowCol(nRow, colNo))
	{
		*pnValue = nDefault;
		return FALSE;
	}
	else
	{
		const char* val = m_lines[nRow-2][colNo];
		*pnValue = strtol(val, NULL, 10);
		return TRUE;
	}
}

BOOL KTabFile::GetInteger(int nRow, int nColumn, int nDefault, int *pnValue)
{
	if(!this->CheckRowCol(nRow, nColumn))
	{
		*pnValue = nDefault;
		return FALSE;
	}
	else
	{
		const char* val = m_lines[nRow-2][nColumn];
		*pnValue = strtol(val, NULL, 10);
		return TRUE;
	}
}

BOOL KTabFile::GetIntegerBySpecialVal(int nRow, int nColumn, int nDefault, int *pnValue,INT nRadix)
{
	if(!this->CheckRowCol(nRow, nColumn))
	{
		*pnValue = nDefault;
		return FALSE;
	}
	else
	{
		const char* val = m_lines[nRow-2][nColumn];
		if(!isnumchar(val[0])) val += 1;
		*pnValue = strtol(val, NULL, nRadix);
		return TRUE;
	}
}

BOOL KTabFile::GetIntegerBySpecialVal(int nRow, const char* szColumn, int nDefault, int *pnValue,INT nRadix)
{
	int nColumn = this->GetColIndex(szColumn);
	if(!this->CheckRowCol(nRow, nColumn))
	{
		*pnValue = nDefault;
		return FALSE;
	}
	else
	{
		const char* val = m_lines[nRow-2][nColumn];
		if(!isnumchar(val[0])) val += 1;
		*pnValue = strtol(val, NULL, nRadix);
		return TRUE;
	}
}

BOOL KTabFile::GetFloat(int nRow, const char* szColumn, float fDefault, float *pfValue)
{
	int nColumn = this->GetColIndex(szColumn);
	if(!this->CheckRowCol(nRow, nColumn))
	{
		*pfValue = fDefault;
		return FALSE;
	}
	else
	{
		const char* val = m_lines[nRow-2][nColumn];
		*pfValue = (float)strtod(val, NULL);
		return TRUE;
	}
}

BOOL KTabFile::GetFloat(int nRow, int nColumn, float fDefault, float *pfValue)
{
	if(!this->CheckRowCol(nRow, nColumn))
	{
		*pfValue = fDefault;
		return FALSE;
	}
	else
	{
		const char* val = m_lines[nRow-2][nColumn];
		*pfValue = (float)strtod(val, NULL);
		return TRUE;
	}
}

BOOL KTabFile::CheckRowCol(int rowNo, int colno)
{
	if(rowNo < 1) return FALSE;
	if(rowNo > m_height) return FALSE;
	if(colno < 0) return FALSE;
	if(colno >= m_width) return FALSE;
	return TRUE;
}

char* KTabFile::WriteValue(const void* pData, int len)
{
	KBuffer64k* pPiece = NULL;
	if(m_bufferArray.empty())
	{
		pPiece = KBuffer64k::Alloc(); pPiece->Reset();
		m_bufferArray.push_back(pPiece);
	}
	else
	{
		int idx = m_bufferArray.size() - 1;
		pPiece = m_bufferArray[idx];
		if(pPiece->WriteAvail() < len)
		{
			pPiece = KBuffer64k::Alloc(); pPiece->Reset();
			m_bufferArray.push_back(pPiece);
		}
	}

	char* pRet = pPiece->GetWriteBuffer();
	pPiece->Write(pData, len);
	return pRet;
}

BOOL KTabFile::GetIntegerByKey(int key, const char* colName, int* pVal)
{
	char buf[64]; sprintf_k(buf, sizeof(buf), "%d", key);
	return this->GetIntegerByKey(buf, colName, pVal);
}

BOOL KTabFile::GetFloatByKey(int key, const char* colName, float* pVal)
{
	char buf[64]; sprintf_k(buf, sizeof(buf), "%d", key);
	return this->GetFloatByKey(buf, colName, pVal);
}

BOOL KTabFile::GetStringByKey(int key, const char* colName, char* ret, int len)
{
	char buf[64]; sprintf_k(buf, sizeof(buf), "%d", key);
	return this->GetStringByKey(buf, colName, ret, len);
}

BOOL KTabFile::GetIntegerByKey(const char* key, const char* colName, int* pVal)
{
	Charpp pp = (char**)&key;
	int pos = m_sortedLines.find(pp);
	if(pos < 0) return FALSE;

	Charpp cols = m_sortedLines[pos];
	int colIdx = this->GetColIndex(colName);
	if(colIdx < 0) return FALSE;

	*pVal = strtol(cols[colIdx], NULL, 10);
	return TRUE;
}

BOOL KTabFile::GetFloatByKey(const char* key, const char* colName, float* pVal)
{
	Charpp pp = (char**)&key;
	int pos = m_sortedLines.find(pp);
	if(pos < 0) return FALSE;

	Charpp cols = m_sortedLines[pos];
	int colIdx = this->GetColIndex(colName);
	if(colIdx < 0) return FALSE;

	*pVal = (float)strtod(cols[colIdx], NULL);
	return TRUE;
}

BOOL KTabFile::GetStringByKey(const char* key, const char* colName, char* ret, int len)
{
	Charpp pp = (char**)&key;
	int pos = m_sortedLines.find(pp);
	if(pos < 0) return FALSE;

	Charpp cols = m_sortedLines[pos];
	int colIdx = this->GetColIndex(colName);
	if(colIdx < 0) return FALSE;

	g_StrCpyLen(ret, cols[colIdx], len);
	return TRUE;
}

int KTabFile::GetColIndex(const char* colName)
{
	KTabfileCol col = {0, (char*)colName};
	int pos = m_headers.find(col);
	if(pos < 0) return -1;
	return m_headers[pos].colIdx;
}

KTabFile::KBufferArray KTabFile::m_bufferArray;

} /* end File */		} /* System */