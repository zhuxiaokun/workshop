#include "KTabFileDiff.h"
#include "../Log/log.h"

using namespace ::System::File;

KTabFileDiff::KTabFileDiff()
{
	m_file1 = NULL;
	m_file2 = NULL;
	m_keyIndex = 0;
	m_keyDataType = enum_DataType_String;
}

KTabFileDiff::~KTabFileDiff()
{
	this->Clear();
}

BOOL KTabFileDiff::Diff(const char* baseFile, const char* newFile)
{
	KTabFileReader reader1;
	if(!reader1.Read(baseFile))
	{
		Log(LOG_FATAL, "fatal: read tab file %s", baseFile);
		return FALSE;
	}

	KTabFileReader reader2;
	if(!reader2.Read(newFile))
	{
		Log(LOG_FATAL, "fatal: read tab file %s", newFile);
		return FALSE;
	}

	return this->Diff(reader1, reader2);
}

BOOL KTabFileDiff::Diff(KTabFileReader& file1, KTabFileReader& file2)
{
	this->Clear();

	m_file1 = &file1;
	m_file2 = &file2;

	m_keyIndex = file1.m_keyColIndex;
	m_keyDataType = file1.m_keyDataType;

	if(!this->DiffHeader()) return FALSE;
	if(!this->DiffBody()) return FALSE;

	return TRUE;
}

BOOL KTabFileDiff::IsDifferent() const
{
	return !m_headerDiff.empty() || !m_bodyDiff.empty();
}

BOOL KTabFileDiff::WriteResult(const char* filename)
{
	KOutputFileStream fo;
	if(!fo.OpenForce(filename, "w+"))
	{
		DWORD errCode = GetLastError();
		Log(LOG_FATAL, "fatal: open file %s for write, %s", filename, strerror_r2(errCode).c_str());
		return FALSE;
	}

	if(this->IsDifferent())
	{
		this->Write_Key_DataType(fo);
		this->Write_Header_Diff(fo);
		this->Write_Body_Diff(fo);
	}

	return TRUE;
}

void KTabFileDiff::Clear()
{
	m_file1 = NULL;
	m_file2 = NULL;
	
	m_keyIndex = 0;
	m_keyDataType = enum_DataType_String;
	m_headerDiff.clear();
	m_bodyDiff.clear();

	while(!m_bufferArray.empty())
	{
		int idx = m_bufferArray.size() - 1;
		KBuffer64k* pPiece = m_bufferArray[idx];
		m_bufferArray.erase(idx);
		KBuffer64k::Free(pPiece);
	}
}

BOOL KTabFileDiff::DiffHeader()
{
	int colSize1 = m_file1->m_colArray.size();
	int colSize2 = m_file2->m_colArray.size();
	
	int colPos = 0;
	for(int i=0; i<colSize1; i++)
	{
		KColumnDeclare& colDecl1 = m_file1->m_colArray[i];
		if(colPos >= colSize2)
		{
			KHeaderColDiff colDiff;
			colDiff.op = enum_Col_Delete;
			colDiff.pColDecl = &colDecl1;
			m_headerDiff.push_back(colDiff);
			continue;
		}

		KColumnDeclare& colDecl2 = m_file2->m_colArray[colPos];
		if(strcmp(colDecl1.name, colDecl2.name) == 0)
		{
			colPos += 1;
		}
		else
		{
			KHeaderColDiff colDiff;
			colDiff.op = enum_Col_Delete;
			colDiff.pColDecl = &colDecl1;
			m_headerDiff.push_back(colDiff);
		}
	}

	for(int i=colPos; i<colSize2; i++)
	{
		KColumnDeclare& colDecl2 = m_file2->m_colArray[i];
		KHeaderColDiff colDiff;
		colDiff.op = enum_Col_Insert;
		colDiff.pColDecl = &colDecl2;
		m_headerDiff.push_back(colDiff);
	}

	this->Compact_HeaderDiff(m_headerDiff);

	int n = m_headerDiff.size();
	for(int i=0; i<n; i++)
	{
		KHeaderColDiff& colDiff = m_headerDiff[i];
		colDiff.pColDecl = (KColumnDeclare*)this->WriteValue((const char*)colDiff.pColDecl, sizeof(KColumnDeclare));
		if(colDiff.pColDecl->name)
		{
			colDiff.pColDecl->name = this->WriteValue(colDiff.pColDecl->name, (INT)strlen(colDiff.pColDecl->name)+1);
		}
	}

	return TRUE;
}

BOOL KTabFileDiff::DiffBody()
{
	int lineCount = m_file1->m_lineArray.size();
	int keyColIndex = m_file1->m_keyColIndex;
	
	for(int i=0; i<lineCount; i++)
	{
		KTabFileReader::StringArray lineCols1 = m_file1->m_lineArray[i];
		const char* keyVal = lineCols1[keyColIndex];
		KTabFileReader::StringArray lineCols2 = m_file2->FindLine(keyVal);
		if(!lineCols2)
		{
			KBodyLineDiff lineDiff;
			lineDiff.keyVal = (char*)keyVal;
			lineDiff.op = enum_Line_Delete;
			lineDiff.colCount = 0;
			lineDiff.pDiffs = NULL;
			m_bodyDiff.push_back(lineDiff);
		}
		else
		{
			this->DiffBodyLine(lineCols1, lineCols2);
		}
	}

	keyColIndex = m_file2->m_keyColIndex;
	lineCount = m_file2->m_lineArray.size();
	for(int i=0; i<lineCount; i++)
	{
		KTabFileReader::StringArray lineCols2 = m_file2->m_lineArray[i];
		const char* keyVal = lineCols2[keyColIndex];
		KTabFileReader::StringArray lineCols1 = m_file1->FindLine(keyVal);
		if(!lineCols1)
		{
			KBodyLineDiff lineDiff;
			lineDiff.keyVal = (char*)keyVal;
			lineDiff.op = enum_Line_Add;
			lineDiff.colCount = m_file2->m_colSize;

			m_currLineDiff.clear();
			for(int k=0; k<lineDiff.colCount; k++)
			{
				KBodyColDiff colDiff;
				colDiff.colIndex = k;
				colDiff.op = enum_Col_Insert;
				colDiff.colVal = lineCols2[k];
				m_currLineDiff.push_back(colDiff);
			}

			char* pDiffs = (char*)&m_currLineDiff[0];
			int len = m_currLineDiff.size() * sizeof(KBodyColDiff);
			lineDiff.pDiffs = (KBodyColDiff*)this->WriteValue(pDiffs, len);

			m_bodyDiff.push_back(lineDiff);
		}
	}

	return TRUE;
}

BOOL KTabFileDiff::DiffBodyLine(KTabFileDiff::KBodyColumns& cols1, KTabFileDiff::KBodyColumns& cols2)
{
	int colSize1 = m_file1->m_colSize;
	int colSize2 = m_file2->m_colSize;
	int headerDiffSize = m_headerDiff.size();

	KBodyLineDiff lineDiff;
	lineDiff.keyVal = cols1[m_file1->m_keyColIndex];
	lineDiff.op = enum_Line_Update;
	lineDiff.colCount = 0;
	lineDiff.pDiffs = NULL;

	m_currLineDiff.clear();
	m_currMergedLine.clear();

	for(int i=0; i<colSize1; i++)
	{
		m_currMergedLine.push_back(cols1[i]);
	}

	for(int i=0; i<headerDiffSize; i++)
	{
		KHeaderColDiff& headerColDiff = m_headerDiff[i];
		if(headerColDiff.op == enum_Col_Delete)
		{
			//KBodyColDiff colDiff;
			//colDiff.colIndex = headerColDiff.pColDecl->index;
			//colDiff.op = enum_Col_Delete;
			//colDiff.colVal = NULL;
			//m_currLineDiff.push_back(colDiff);
			m_currMergedLine.erase(headerColDiff.pColDecl->index);
		}
		else if(headerColDiff.op == enum_Col_Insert)
		{
			KBodyColDiff colDiff;
			colDiff.colIndex = headerColDiff.pColDecl->index;
			colDiff.op = enum_Col_Insert;
			colDiff.colVal = cols2[colDiff.colIndex];
			colDiff.colVal = this->WriteValue(colDiff.colVal, (INT)strlen(colDiff.colVal)+1);
			m_currLineDiff.push_back(colDiff);

			m_currMergedLine.insert(colDiff.colIndex, cols2[colDiff.colIndex]);
		}
		else
		{
			//ASSERT(FALSE);
		}
	}

	for(int i=0; i<colSize2; i++)
	{
		char* val1 = m_currMergedLine[i];
		char* val2 = cols2[i];
		if(strcmp(val1, val2))
		{
			KBodyColDiff diff;
			diff.colIndex = i;
			diff.op = enum_Col_Update;
			diff.colVal = this->WriteValue(val2, (INT)strlen(val2)+1);
			m_currLineDiff.push_back(diff);
		}
	}

	if(!m_currLineDiff.empty())
	{
		const char* pDiffs = (const char*)&m_currLineDiff[0];
		int len = m_currLineDiff.size() * sizeof(KBodyColDiff);

		lineDiff.colCount = m_currLineDiff.size();
		lineDiff.pDiffs = (KBodyColDiff*)this->WriteValue(pDiffs, len);

		m_bodyDiff.push_back(lineDiff);
	}
	
	return TRUE;
}

char* KTabFileDiff::WriteValue(const char* val, int len)
{
	KBuffer64k* pPiece;
	if(len == -1) len = (int)strlen(val) + 1;
	if(m_bufferArray.empty())
	{
		pPiece = KBuffer64k::Alloc();
		pPiece->Reset();
		m_bufferArray.push_back(pPiece);
	}
	else
	{
		int idx = m_bufferArray.size() - 1;
		pPiece = m_bufferArray[idx];
		if(pPiece->WriteAvail() < len)
		{
			pPiece = KBuffer64k::Alloc();
			pPiece->Reset();
			m_bufferArray.push_back(pPiece);
		}
	}

	char* pRet = pPiece->GetWriteBuffer();
	pPiece->Write(val, len);
	return pRet;
}

BOOL KTabFileDiff::Compact_HeaderDiff(KTabFileDiff::KHeaderDiff& headerDiff)
{
	int count = headerDiff.size();
	for(int i=0; i<count; i++)
	{
		KHeaderColDiff& colDiff = headerDiff[i];
		if(colDiff.op == enum_Col_Delete)
		{
			for(int k=i+1; k<count; k++)
			{
				KHeaderColDiff& colDiff2 = headerDiff[k];
				if(colDiff2.op == enum_Col_Insert && colDiff2.pColDecl->index == colDiff.pColDecl->index)
				{// 两者可以合并
					const char* colName1 = colDiff.pColDecl->name;
					const char* colName2 = colDiff2.pColDecl->name;
					if(strcmp(colName1, colName2))
					{//合并为Update
						colDiff.op = enum_Col_Update;
						colDiff.pColDecl = colDiff2.pColDecl;
						colDiff2.op = enum_Col_Invalid;
					}
					else
					{//两者抵消
						colDiff.op = enum_Col_Invalid;
						colDiff2.op = enum_Col_Invalid;
					}
				}
			}
		}
	}

	for(int i=count-1; i>=0; i--)
	{
		KHeaderColDiff& colDiff = headerDiff[i];
		if(colDiff.op == enum_Col_Invalid)
		{
			headerDiff.erase(i);
		}
	}

	return TRUE;
}

BOOL KTabFileDiff::Compact_LineDiff(KBodyLineDiff& lineDiff)
{
	return TRUE;
}

BOOL KTabFileDiff::Write_Key_DataType(StreamInterface& so)
{
	switch(m_keyDataType)
	{
	case enum_DataType_Int:
		so.WriteData("i\n", 2);
		break;
	case enum_DataType_Uint:
		so.WriteData("ui\n", 3);
		break;
	case enum_DataType_String:
		so.WriteData("s\n", 2);
		break;
	case enum_DataType_Int64:
		so.WriteData("i64\n", 4);
		break;
	case enum_DataType_Uint64:
		so.WriteData("ui64\n", 5);
		break;
	default: ASSERT(FALSE);
	};
	return TRUE;
}

BOOL KTabFileDiff::Write_Header_Diff(StreamInterface& so)
{
	int len;
	char cTmp[512];

	so.WriteData("h", 1);
	int colSize = m_headerDiff.size();
	for(int i=0; i<colSize; i++)
	{
		KHeaderColDiff& diff = m_headerDiff[i];
		switch(diff.op)
		{
		case enum_Col_Insert:
			len = (int)sprintf_k(cTmp, sizeof(cTmp), "\ti:%d:%s", diff.pColDecl->index, diff.pColDecl->name);
			break;
		case enum_Col_Update:
			len = (int)sprintf_k(cTmp, sizeof(cTmp), "\tu:%d:%s", diff.pColDecl->index, diff.pColDecl->name);
			break;
		case enum_Col_Delete:
			len = (int)sprintf_k(cTmp, sizeof(cTmp), "\td:%d", diff.pColDecl->index);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		so.WriteData(cTmp, len);
	}
	
	so.WriteData("\n", 1);
	return TRUE;
}

BOOL KTabFileDiff::Write_Body_Diff(StreamInterface& so)
{
	int lineCount = m_bodyDiff.size();
	for(int i=0; i<lineCount; i++)
	{
		KBodyLineDiff& lineDiff = m_bodyDiff[i];
		this->Write_BodyLine_Diff(so, lineDiff);
	}
	return TRUE;
}

BOOL KTabFileDiff::Write_BodyLine_Diff(StreamInterface& so, KBodyLineDiff& lineDiff)
{
	int len;
	char cTmp[512];
	
	switch(lineDiff.op)
	{
	case enum_Line_Add:
		len = (int)sprintf_k(cTmp, sizeof(cTmp), "a");
		break;
	case enum_Line_Update:
		len = (int)sprintf_k(cTmp, sizeof(cTmp), "u:%s", lineDiff.keyVal);
		break;
	case enum_Line_Delete:
		len = (int)sprintf_k(cTmp, sizeof(cTmp), "d:%s", lineDiff.keyVal);
		break;
	default:
		ASSERT(FALSE); break;
	};

	so.WriteData(cTmp, len);

	for(int i=0; i<lineDiff.colCount; i++)
	{
		KBodyColDiff& colDiff = lineDiff.pDiffs[i];
		switch(colDiff.op)
		{
		case enum_Col_Insert:
			if(lineDiff.op == enum_Line_Add)
			{
				len = (int)sprintf_k(cTmp, sizeof(cTmp), "\t%s", colDiff.colVal);
			}
			else
				len = (int)sprintf_k(cTmp, sizeof(cTmp), "\ti:%d:%s", colDiff.colIndex, colDiff.colVal);
			break;
		case enum_Col_Update:
			len = (int)sprintf_k(cTmp, sizeof(cTmp), "\tu:%d:%s", colDiff.colIndex, colDiff.colVal);
			break;
		case enum_Col_Delete:
			len = (int)sprintf_k(cTmp, sizeof(cTmp), "\td:%d", colDiff.colIndex);
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		so.WriteData(cTmp, len);
	}

	so.WriteData("\n", 1);
	return TRUE;
}

KDiffFileReader::KDiffFileReader():m_currLine(0)
{
	m_keyDatatype = enum_DataType_String;
}

KDiffFileReader::~KDiffFileReader()
{
	this->Clear();
}

void KDiffFileReader::Clear()
{
	m_currLine = 0;
	m_keyDatatype = enum_DataType_String;
	m_headerDiff.clear();
	m_bodyDiff.clear();
	m_newBodyLines.clear();
	m_currLineDiff.clear();

	while(!m_bufferArray.empty())
	{
		int idx = m_bufferArray.size() - 1;
		KBuffer64k* pPiece = m_bufferArray[idx];
		m_bufferArray.erase(idx);
		KBuffer64k::Free(pPiece);
	}
}

BOOL KDiffFileReader::Read(StreamInterface& si)
{
	char** cols;
	m_currLine = 0;

	KColumnReader reader;
	reader.SetInputStream(si);

	while(TRUE)
	{
		int nCol = reader.ReadLine(cols, '\t');
		if(nCol == -1) break;
		if(nCol == 0) continue;

		if(m_currLine == 0)
		{
			if(!this->ParseKeyDatatype(cols, nCol))
				return FALSE;
		}
		else if(m_currLine == 1)
		{
			if(!this->ParseHeaderDiff(cols, nCol))
				return FALSE;
		}
		else
		{
			if(!this->ParseLineDiff(cols, nCol))
				return FALSE;
		}

		m_currLine += 1;
	}

	return TRUE;
}

KHeaderDiffUnit* KDiffFileReader::GetHeaderDiff(int colIndex)
{
	int c = m_headerDiff.size();
	for(int i=0; i<c; i++)
	{
		KHeaderDiffUnit& diff = m_headerDiff[i];
		if(diff.colIndex == colIndex)
			return &diff;
	}
	return NULL;
}

KBodyLineDiff* KDiffFileReader::GetLineDiff(const char* key)
{
	KBodyLineDiff lineDiff;
	lineDiff.keyVal = (char*)key;
	int pos = m_bodyDiff.find(lineDiff);
	if(pos < 0) return NULL;
	return &m_bodyDiff[pos];
}

BOOL KDiffFileReader::ParseKeyDatatype(char** cols, int size)
{
	if(size != 1) return FALSE;
	char* col = cols[0];
	if(strcmp(col, "i") == 0)
	{
		m_keyDatatype = enum_DataType_Int;
	}
	else if(strcmp(col, "ui") == 0)
	{
		m_keyDatatype = enum_DataType_Uint;
	}
	else if(strcmp(col, "s") == 0)
	{
		m_keyDatatype = enum_DataType_String;
	}
	else if(strcmp(col, "i64") == 0)
	{
		m_keyDatatype = enum_DataType_Int64;
	}
	else if(strcmp(col, "ui64") == 0)
	{
		m_keyDatatype = enum_DataType_Uint64;
	}
	else
	{
		Log(LOG_FATAL, "fatal: unknown key data type %s", col);
		return FALSE;
	}

	m_bodyDiff.m_cmp.m_less.m_keyDatatype = m_keyDatatype;
	return TRUE;
}

// h	i:colIndex:colName	u:colIndex:colName	d:colIndex
BOOL KDiffFileReader::ParseHeaderDiff(char** cols, int size)
{
	char* col = cols[0];
	if(col[0] != 'h' || col[1] != '\0') return FALSE;

	for(int i=1; i<size; i++)
	{
		col = cols[i];
		switch(col[0])
		{
		case 'i':
			{
				if(col[1] != ':')
				{
					Log(LOG_FATAL, "fatal: invalid header diff unit, %s", cols[i]);
					return FALSE;
				}

				col += 2;
				char* pColIdx = strtok(col, ":");
				if(!pColIdx || !isnumber3(pColIdx))
				{
					Log(LOG_FATAL, "fatal: invalid header diff unit, %s", cols[i]);
					return FALSE;
				}
				char* pColName = strtok(NULL, ":");
				if(!pColName || strtok(NULL, ":"))
				{
					Log(LOG_FATAL, "fatal: invalid header diff unit, %s", cols[i]);
					return FALSE;
				}

				KHeaderDiffUnit diff;
				diff.op = enum_Col_Insert;
				diff.colIndex = str2int(pColIdx);
				diff.colName = this->WriteValue(pColName, (INT)strlen(pColName)+1);
				m_headerDiff.push_back(diff);
				break;
			}
		case 'u':
			{
				if(col[1] != ':')
				{
					Log(LOG_FATAL, "fatal: invalid header diff unit, %s", cols[i]);
					return FALSE;
				}
				col += 2;
				char* pColIdx = strtok(col, ":");
				if(!pColIdx || !isnumber3(pColIdx))
				{
					Log(LOG_FATAL, "fatal: invalid header diff unit, %s", cols[i]);
					return FALSE;
				}
				char* pColName = strtok(NULL, ":");
				if(!pColName || strtok(NULL, ":"))
				{
					Log(LOG_FATAL, "fatal: invalid header diff unit, %s", cols[i]);
					return FALSE;
				}

				KHeaderDiffUnit diff;
				diff.op = enum_Col_Update;
				diff.colIndex = str2int(pColIdx);
				diff.colName = this->WriteValue(pColName, (INT)strlen(pColName)+1);
				m_headerDiff.push_back(diff);
				break;
			}
		case 'd':
			{
				if(col[1] != ':')
				{
					Log(LOG_FATAL, "fatal: invalid header diff unit, %s", cols[i]);
					return FALSE;
				}
				col += 2;
				char* pColIdx = strtok(col, ":");
				if(!pColIdx || !isnumber3(pColIdx))
				{
					Log(LOG_FATAL, "fatal: invalid header diff unit, %s", cols[i]);
					return FALSE;
				}
				if(strtok(NULL, ":"))
				{
					Log(LOG_FATAL, "fatal: invalid header diff unit, %s", cols[i]);
					return FALSE;
				}

				KHeaderDiffUnit diff;
				diff.op = enum_Col_Delete;
				diff.colIndex = str2int(pColIdx);
				diff.colName = NULL;

				m_headerDiff.push_back(diff);
				break;
			}
		default:
			return FALSE;
		}
	}
	return TRUE;
}

// a	col-values
// u:key	col-update-s
// d:key
BOOL KDiffFileReader::ParseLineDiff(char** cols, int size)
{
	char* col = cols[0];
	switch(col[0])
	{
	case 'a':
		return this->ParseLineInsert(cols, size);
	case 'u':
		return this->ParseLineUpdate(cols, size);
	case 'd':
		return this->ParseLineDelete(cols, size);
	default:
		{
			Log(LOG_FATAL, "fatal: unknown line diff type %s", col);
			return FALSE;
		}
	}
}

// val1	val2...
BOOL KDiffFileReader::ParseLineInsert(char** cols, int size)
{
	char* col = cols[0];
	if(col[1] != '\0')
	{
		Log(LOG_FATAL, "fatal: invalid line insert indicator %s", col);
		return FALSE;
	}

	m_currLineDiff.clear();
	for(int i=1; i<size; i++)
	{
		char* col = cols[i];

		KBodyColDiff colDiff;
		colDiff.op = enum_Col_Insert;
		colDiff.colIndex = i-1;
		colDiff.colVal = this->WriteValue(col, (INT)strlen(col)+1);

		m_currLineDiff.push_back(colDiff);
	}

	KBodyLineDiff lineDiff;
	lineDiff.keyVal = NULL;
	lineDiff.op = enum_Line_Add;
	lineDiff.colCount = m_currLineDiff.size();

	char* p = (char*)&m_currLineDiff.m_vals[0];
	int len = lineDiff.colCount * sizeof(KBodyColDiff);
	lineDiff.pDiffs = (KBodyColDiff*)this->WriteValue(p, len);

	m_newBodyLines.push_back(lineDiff);
	return TRUE;
}

// d:key
BOOL KDiffFileReader::ParseLineDelete(char** cols, int size)
{
	if(size != 1)
	{
		Log(LOG_FATAL, "fatal: invalid line delete diff, %s", cols[0]);
		return FALSE;
	}

	char* col = cols[0];
	if(col[1] != ':')
	{
		Log(LOG_FATAL, "fatal: invalid line delete diff %s", col);
		return FALSE;
	}

	char* pKey = &col[2];
	if(!pKey[0])
	{
		Log(LOG_FATAL, "fatal: invalid line delete diff %s", col);
		return FALSE;
	}

	KBodyLineDiff lineDiff;
	lineDiff.op = enum_Line_Delete;
	lineDiff.keyVal = this->WriteValue(pKey, (INT)strlen(pKey)+1);
	lineDiff.colCount = 0;
	lineDiff.pDiffs = NULL;

	int pos = m_bodyDiff.insert_unique(lineDiff);
	if(pos < 0)
	{
		Log(LOG_FATAL, "fatal: delete line diff for %s already exists", lineDiff.keyVal);
		return FALSE;
	}

	return TRUE;
}

// u:key	i:colIdx:val u:colIdx:val d:colIdx
BOOL KDiffFileReader::ParseLineUpdate(char** cols, int size)
{
	char* col = cols[0];
	if(col[1] != ':')
	{
		Log(LOG_FATAL, "fatal: invalid update line diff %s", col);
		return FALSE;
	}

	char* pKey = &col[2];
	if(!pKey[0])
	{
		Log(LOG_FATAL, "fatal: invalid update line diff %s", col);
		return FALSE;
	}

	m_currLineDiff.clear();
	
	KBodyLineDiff lineDiff;
	lineDiff.op = enum_Line_Update;
	lineDiff.keyVal = this->WriteValue(pKey, (INT)strlen(pKey)+1);

	for(int i=1; i<size; i++)
	{
		col = cols[i];
		if(!this->ParseLineColDiff(col))
			return FALSE;
	}

	lineDiff.colCount = m_currLineDiff.size();
	char* p = (char*)m_currLineDiff.m_vals;
	int len = sizeof(KBodyColDiff) * m_currLineDiff.size();
	lineDiff.pDiffs = (KBodyColDiff*)this->WriteValue(p, len);

	int pos = m_bodyDiff.insert_unique(lineDiff);
	if(pos < 0)
	{
		Log(LOG_FATAL, "fatal: update line diff for %s already exists", lineDiff.keyVal);
		return FALSE;
	}

	return TRUE;
}

// i:colIdx:val u:colIdx:val d:colIdx
BOOL KDiffFileReader::ParseLineColDiff(char* col)
{
	switch(col[0])
	{
	case 'i':
		{
			if(col[1] != ':')
			{
				Log(LOG_FATAL, "fatal: invalid line col diff %s", col);
				return FALSE;
			}
			col += 2;
			char* pColIdx = strtok(col, ":");
			if(!pColIdx || !isnumber3(pColIdx))
			{
				Log(LOG_FATAL, "fatal: invalid line col diff %s", col);
				return FALSE;
			}
			char* pColName = strtok(NULL, ":");
			if(!pColName || strtok(NULL, ":"))
			{
				Log(LOG_FATAL, "fatal: invalid line col diff %s", col);
				return FALSE;
			}

			KBodyColDiff diff;
			diff.op = enum_Col_Insert;
			diff.colIndex = str2int(pColIdx);
			diff.colVal = this->WriteValue(pColName, (INT)strlen(pColName)+1);

			m_currLineDiff.push_back(diff);
			break;
		}
	case 'u':
		{
			if(col[1] != ':')
			{
				Log(LOG_FATAL, "fatal: invalid line col diff %s", col);
				return FALSE;
			}
			col += 2;
			char* pColIdx = strtok(col, ":");
			if(!pColIdx || !isnumber3(pColIdx))
			{
				Log(LOG_FATAL, "fatal: invalid line col diff %s", col);
				return FALSE;
			}
			char* pColName = strtok(NULL, ":");
			if(!pColName || strtok(NULL, ":"))
			{
				Log(LOG_FATAL, "fatal: invalid line col diff %s", col);
				return FALSE;
			}

			KBodyColDiff diff;
			diff.op = enum_Col_Update;
			diff.colIndex = str2int(pColIdx);
			diff.colVal = this->WriteValue(pColName, (INT)strlen(pColName)+1);
			m_currLineDiff.push_back(diff);
			break;
		}
	case 'd':
		{
			if(col[1] != ':')
			{
				Log(LOG_FATAL, "fatal: invalid line col diff %s", col);
				return FALSE;
			}
			col += 2;
			char* pColIdx = strtok(col, ":");
			if(!pColIdx || !isnumber3(pColIdx))
			{
				Log(LOG_FATAL, "fatal: invalid line col diff %s", col);
				return FALSE;
			}
			if(strtok(NULL, ":"))
			{
				Log(LOG_FATAL, "fatal: invalid line col diff %s", col);
				return FALSE;
			}

			KBodyColDiff diff;
			diff.op = enum_Col_Delete;
			diff.colIndex = str2int(pColIdx);
			diff.colVal = NULL;

			m_currLineDiff.push_back(diff);
			break;
		}
	default:
		return FALSE;
	}
	return TRUE;
}

char* KDiffFileReader::WriteValue(const char* val, int len)
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
	pPiece->Write(val, len);
	return pRet;
}

KTabfileMergeReader::KTabfileMergeReader()
{

}

KTabfileMergeReader::~KTabfileMergeReader()
{
	this->Clear();
}

void KTabfileMergeReader::Clear()
{
	m_fiBase.Close();
	m_fiDiff.Close();
	this->_Clear();
}

BOOL KTabfileMergeReader::Open(const char* baseFile,int keyIdx, enum_Col_DataType keydt)
{
	KInputFileStream& fi = m_fiBase; fi.Close();
	if(!fi.Open(baseFile))
	{
		Log(LOG_FATAL, "fatal: open %s", baseFile);
		return FALSE;
	}

	return this->Open(&fi, NULL, keyIdx, keydt);
}

BOOL KTabfileMergeReader::Open(const char* baseFile, const char* diffFile,int keyIdx, enum_Col_DataType keydt)
{
	KInputFileStream& fi = m_fiBase; fi.Close();
	if(!fi.Open(baseFile))
	{
		Log(LOG_FATAL, "fatal: open %s", baseFile);
		return FALSE;
	}

	KInputFileStream& fiDiff = m_fiDiff; fiDiff.Close();
	if(!fiDiff.Open(diffFile))
	{
		Log(LOG_FATAL, "fatal: open %s", diffFile);
		return FALSE;
	}

	return this->Open(&fi, &fiDiff, keyIdx, keydt);
}

BOOL KTabfileMergeReader::Open(StreamInterface* psiTabfile, StreamInterface* psiDiff,int keyIdx, enum_Col_DataType keydt)
{
	if(psiTabfile != (StreamInterface*)&m_fiBase)
	{
		m_fiBase.Close();
	}
	if(psiDiff != (StreamInterface*)&m_fiDiff)
	{
		m_fiDiff.Close();
	}

	this->_Clear();

	ASSERT(psiTabfile);

	m_keyIndex = keyIdx;
	m_keyDatatype = keydt;
	m_newLineIdx = -1;
	m_nLineNum = 0;

	m_colReader.SetInputStream(*psiTabfile);

	if(psiDiff)
	{
		if(!m_diffReader.Read(*psiDiff))
			return FALSE;
	}

	if(!this->ReadHeader())
		return FALSE;

	return TRUE;
}

int KTabfileMergeReader::ReadLine()
{
	if(m_newLineIdx != -1)
	{
		m_newLineIdx += 1;
		return this->ReadNewLine();
	}

start_read:
	m_currLine.clear();
	m_currLine.detach();

	int nCol;
	char** cols;
	while(TRUE)
	{
		nCol = m_colReader.ReadLine(cols, '\t');
		m_nLineNum++;
		if(nCol == -1)
		{
			m_newLineIdx = 0;
			return this->ReadNewLine();
		}
		if(nCol == 0) continue;
		break;
	}
	
	KBodyLineDiff* pLineDiff = m_diffReader.GetLineDiff(cols[m_keyIndex]);
	if(m_delCols.empty())
	{	
		if(!pLineDiff)
		{
			if(nCol != m_colSize)
			{
				Log(LOG_FATAL, "fatal: column size not match the header, %d:%d", nCol, m_colSize);
				return -1;
			}
			m_currLine.attach(cols, nCol, nCol);
			return 1;
		}
	}

	if(pLineDiff && pLineDiff->op == enum_Line_Delete)
		goto start_read;

	for(int i=0; i<nCol; i++) m_currLine.push_back(cols[i]);

	if(!m_delCols.empty())
	{
		int delColSize = m_delCols.size();
		for(int i=delColSize-1; i>=0; i--)
		{
			int colIdx = m_delCols[i];
			m_currLine.erase(colIdx);
		}
	}
	if(pLineDiff)
	{
		for(int i=0; i<pLineDiff->colCount; i++)
		{
			KBodyColDiff* pDiff = &pLineDiff->pDiffs[i];
			int colIdx = pDiff->colIndex;
			switch(pDiff->op)
			{
			case enum_Col_Insert:
				m_currLine.insert(colIdx, pDiff->colVal);
				break;
			case enum_Col_Update:
				m_currLine[colIdx] = pDiff->colVal;
				break;
			}
		}
	}

	if(m_currLine.size() != m_colSize)
	{
		Log(LOG_FATAL, "fatal: column size not match on the %d line, data:%s", m_nLineNum, cols[0]);
		return -1;
	}
	
	return 1;
}

const char* KTabfileMergeReader::GetColumn(int colIndex)
{
	if(colIndex < 0 || colIndex>= m_colSize) return NULL;
	return m_currLine[colIndex];
}

const char* KTabfileMergeReader::GetColumn(const char* colName)
{
	int colIndex = this->GetColIndex(colName);
	return this->GetColumn(colIndex);
}

const char* KTabfileMergeReader::GetColumnName(int colIdx) const
{
	int n = m_colArray.size();
	if(colIdx < 0 || colIdx >= n) return NULL;
	return m_colArray[colIdx];
}

BOOL KTabfileMergeReader::ReadHeader()
{
	int nCol;
	char** cols;

	while(TRUE)
	{
		nCol = m_colReader.ReadLine(cols, '\t');
		if(nCol == -1)
		{// 没有数据可读，应该是一个空文件
			Log(LOG_FATAL, "fatal: tab file no header line found");
			return TRUE;
		}
		if(nCol == 0) continue;
		break;
	}
	
	m_colArray.clear();

	for(int i=0; i<nCol; i++)
	{
		char* col = cols[i];
		IStringTranslator* pTranslate = IStringTranslator::GetGlobalTranslator();
		if(pTranslate)
		{
			col = (char*)pTranslate->Translate(col);
		}
		m_colArray.push_back(this->WriteValue(col, (INT)strlen(col)+1));
	}

	int c;
	m_delCols.clear();

	// 先记录下被删除的列
	if(!m_diffReader.m_headerDiff.empty())
	{
		c = m_diffReader.m_headerDiff.size();
		for(int i=0; i<c; i++)
		{
			KHeaderDiffUnit& diffUnit = m_diffReader.m_headerDiff[i];
			if(diffUnit.op == enum_Col_Delete)
			{
				m_delCols.insert_unique(diffUnit.colIndex);
			}
		}
	}

	// 把删除的列从原始的列数组中删除
	c = m_delCols.size();
	for(int i=c-1; i>=0; i--)
	{
		m_colArray.erase(m_delCols[i]);
	}

	// 删除差异已经处理过了，这里不需要考虑
	if(!m_diffReader.m_headerDiff.empty())
	{
		int c = m_diffReader.m_headerDiff.size();
		for(int i=0; i<c; i++)
		{
			KHeaderDiffUnit& diffUnit = m_diffReader.m_headerDiff[i];
			if(diffUnit.op == enum_Col_Insert)
			{
				m_colArray.insert(diffUnit.colIndex, diffUnit.colName);
			}
			else if(diffUnit.op == enum_Col_Update)
			{
				m_colArray[diffUnit.colIndex] = diffUnit.colName;
			}
		}
	}

	m_sortedColArray.clear();
	m_colSize = m_colArray.size();
	for(int i=0; i<m_colSize; i++)
	{
		KTabfileCol tabfileCol;
		tabfileCol.colIdx = i;
		tabfileCol.colName = m_colArray[i];

		int pos = m_sortedColArray.insert_unique(tabfileCol);
		if(pos < 0)
		{
			Log(LOG_FATAL, "fatal: column %s already exists", tabfileCol.colName);
			return FALSE;
		}
	}
	
	return TRUE;
}

int KTabfileMergeReader::ReadNewLine()
{
	if(m_newLineIdx >= m_diffReader.m_newBodyLines.size()) return 0;

	m_currLine.clear();
	m_currLine.detach();
	
	KBodyLineDiff& lineDiff = m_diffReader.m_newBodyLines[m_newLineIdx];
	if(lineDiff.colCount != m_colSize)
	{
		Log(LOG_FATAL, "fatal: new line in diff file, col size mis-match, %d:%d", lineDiff.colCount, m_colSize);
		return -1;
	}

	for(int i=0; i<m_colSize; i++)
	{
		KBodyColDiff& colDiff = lineDiff.pDiffs[i];
		m_currLine.push_back(colDiff.colVal);
	}

	return 1;
}

int KTabfileMergeReader::GetColIndex(const char* colName)
{
	KTabfileCol col2 = {0, (char*)colName};
	int pos = m_sortedColArray.find(col2);
	if(pos < 0) return -1;

	KTabfileCol& col = m_sortedColArray[pos];
	return col.colIdx;
}

char* KTabfileMergeReader::WriteValue(const char* val, int len)
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
	pPiece->Write(val, len);
	return pRet;
}

void KTabfileMergeReader::_Clear()
{
	m_keyIndex = 0;
	m_keyDatatype = enum_DataType_String;
	m_colSize = 0;
	m_colReader.Clear();
	m_diffReader.Clear();

	m_colArray.clear();
	m_sortedColArray.clear();
	m_delCols.clear();

	m_newLineIdx = -1;
	m_currLine.clear();

	while(!m_bufferArray.empty())
	{
		int idx = m_bufferArray.size() - 1;
		KBuffer64k* pPiece = m_bufferArray[idx];
		m_bufferArray.erase(idx);
		KBuffer64k::Free(pPiece);
	}
}