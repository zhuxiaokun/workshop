#include "KCoordinateTable.h"
#include <System/File/KTabFile.h>
#include <System/Log/log.h>
#include <KCommonDefine.h>
#include "KTabfileLoader.h"

KCoordinateTable::KCoordinateTable()
{

}

KCoordinateTable::~KCoordinateTable()
{

}

BOOL KCoordinateTable::_add(const KCoordinatePoint& point)
{
	m_map[point.m_Id] = point;
	return TRUE;
}

BOOL KCoordinateTable::_remove(int pointID)
{
	Map::iterator it = m_map.find(pointID);
	if(it == m_map.end()) return FALSE;
	m_map.erase(it);
	return TRUE;
}

const KCoordinatePoint* KCoordinateTable::GetPoint(int pointID) const
{
	Map::const_iterator it = m_map.find(pointID);
	if(it == m_map.end()) return NULL;
	return &it->second;
}

BOOL KCoordinateTable::Initialize(const char* filename)
{
	KTabfileLoader& loader = KTabfileLoader::GetInstance();

	JG_F::KTabFile2* fileReader = loader.GetFileReader(filename);
	if(!fileReader)
	{
		Log(LOG_ERROR, "error: open coordinate file %s", filename);
		return FALSE;
	}

	int rowNo = 2;
	char desc[256];
	KCoordinatePoint point;

	while(TRUE)
	{
		int nRet = fileReader->ReadLine();
		if(nRet == -1) { loader.CloseFileReader(fileReader); return FALSE; }
		if(nRet == 0) break;

		if(!fileReader->GetInteger("Id", 0, &point.m_Id)) break;
		if(point.m_Id == 0) continue;

		if(!fileReader->GetInteger("MapId", 0, &point.m_MapID)) return FALSE;
		if(!fileReader->GetInteger("Xaxis", 0, &point.m_Xaxis)) return FALSE;
		if(!fileReader->GetInteger("Yaxis", 0, &point.m_Yaxis)) return FALSE;
		if(!fileReader->GetInteger("CDirection", 0, &point.m_CDirection)) return FALSE;

		if(!fileReader->GetInteger("CAngle", 0, &point.m_CAngle)) return FALSE;
		if(!fileReader->GetInteger("CFaces", 0, &point.m_CFaces)) return FALSE;
		if(!fileReader->GetInteger("RDirection", 0, &point.m_RDirection)) return FALSE;
		
		if(!fileReader->GetString("Type", "", desc, sizeof(desc))) return FALSE;
		point.m_desc = KBuffer64k::WriteData(desc, strlen(desc)+1);

		this->_add(point);
		rowNo += 1;
	}

	loader.CloseFileReader(fileReader);
	return TRUE;
}

void KCoordinateTable::Finalize()
{
	return;
}
