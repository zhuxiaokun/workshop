#include "KTabFileReader.h"
#include "../Log/log.h"

using namespace ::System::File;

KColumnDeclare::KColumnDeclare()
	: index(-1)
	, name(NULL)
	, dataType(enum_DataType_Invalid)
{

}

KColumnDeclare::KColumnDeclare(const KColumnDeclare& o)
	: index(o.index)
	, name(o.name)
	, dataType(o.dataType)
{

}

KColumnDeclare::KColumnDeclare(int idx, const char* nm, enum_Col_DataType dt)
	: index(idx)
	, name((char*)nm)
	, dataType(dt)
{

}

KTabFileReader::KTabFileReader()
{
	m_keyColIndex = -1;
	m_keyColName[0] = '\0';
	m_keyDataType = enum_DataType_Invalid;
	m_colSize = 0;
}

KTabFileReader::~KTabFileReader()
{
	this->Clear();
}

BOOL KTabFileReader::SetKeyColumn(const char* colName, enum_Col_DataType dataType)
{
	strcpy_k(m_keyColName, sizeof(m_keyColName), colName);
	m_keyDataType = dataType;
	return TRUE;
}

BOOL KTabFileReader::Read(const char* filename)
{
	//this->Clear();
	KColumnFileReader reader(filename);
	if(!this->ReadFileHeader(reader)) return FALSE;
	if(!this->ReadFileBody(reader)) return FALSE;
	return TRUE;
}

KTabFileReader::StringArray KTabFileReader::FindLine(const char* key)
{
	m_lineForCmp[m_colSize-1] = NULL;
	m_lineForCmp[m_keyColIndex] = (char*)key;
	int pos = m_lineArray.find(&m_lineForCmp[0]);
	if(pos < 0) return NULL;
	return m_lineArray[pos];
}

char* KTabFileReader::WriteValue(const char* val, int len)
{
	KBuffer64k* pPiece = NULL;

	if(len < 0)
	{
		len = (int)strlen(val) + 1;
	}
	
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

BOOL KTabFileReader::ReadFileHeader(KColumnReader& reader)
{
	char** pCols;
	int colCount = reader.ReadLine(pCols, '\t');
	if(colCount < 1) return FALSE;

	for(int i=0; i<colCount; i++)
	{
		char* colName = this->WriteValue(pCols[i]);
		KColumnDeclare colDecl(i, colName, enum_DataType_String);
		m_colArray.push_back(colDecl);
		if(m_sortedColArray.insert_unique(colDecl) < 0)
		{
			Log(LOG_ERROR, "error: column %s already exits", colName);
			return FALSE;
		}
	}

	if(!m_keyColName[0])
	{
		m_keyColIndex = 0;
		strcpy_k(m_keyColName, sizeof(m_keyColName), m_colArray[0].name);

		m_keyDataType = m_colArray[0].dataType;
	}
	else
	{
		KColumnDeclare colDecl(0,m_keyColName,enum_DataType_String);
		int pos = m_sortedColArray.find(colDecl);
		if(pos < 0)
		{
			Log(LOG_ERROR, "error: key column %s not found", m_keyColName);
			return FALSE;
		}

		KColumnDeclare& keyCol = m_sortedColArray[pos];
		m_keyColIndex = keyCol.index;
		keyCol.dataType = m_keyDataType;
		m_colArray[m_keyColIndex].dataType = m_keyDataType;
	}

	m_colSize = m_colArray.size();
	memcpy(&m_lineArray.m_cmp.m_less.m_keyCol, &m_colArray[m_keyColIndex], sizeof(KColumnDeclare));
	return TRUE;
}

BOOL KTabFileReader::ReadFileBody(KColumnReader& reader)
{
	char* pline;
	KStringList ss;
	while(TRUE)
	{
		int nBytes = reader.ReadLine(pline);
		if(nBytes == -1) break;
		if(nBytes == 0) continue;

		char* pmyLine = this->WriteValue(pline, nBytes+1);
		pmyLine[nBytes] = '\0';

		int cols = split(pmyLine, '\t', ss);
		if(cols != m_colSize)
		{
			Log(LOG_ERROR, "error: line:%s, column size not match", pmyLine);
			return FALSE;
		}
		
		char** pCols = &ss.m_vals[0];
		int len = cols * sizeof(char*);

		pCols = (char**)this->WriteValue((char*)pCols, len);
		if(m_keyColIndex != -1)
		{
			if(m_lineArray.insert_unique(pCols) < 0)
			{
				Log(LOG_ERROR, "error: column with key:%s already exists", pCols[m_keyColIndex]);
				return FALSE;
			}
		}
		else
		{
			m_lineArray.insert(pCols);
		}
	}
	return TRUE;
}

void KTabFileReader::Clear()
{
	m_keyColIndex = -1;
	m_keyColName[0] = '\0';
	m_keyDataType = enum_DataType_Invalid;

	m_colSize = 0;
	m_colArray.clear();
	m_sortedColArray.clear();
	m_lineArray.clear();

	while(!m_bufferArray.empty())
	{
		int idx = m_bufferArray.size() - 1;
		KBuffer64k* pPiece = m_bufferArray[idx];
		m_bufferArray.erase(idx);
		KBuffer64k::Free(pPiece);
	}
}