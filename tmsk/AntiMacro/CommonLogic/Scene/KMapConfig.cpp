#include "KMapConfig.h"

// Map Visitor

class KMapVisitor_initData : public KMapSetting::IVisitor
{
public:
	KMapDataManager& m_dataManager;
	KMapVisitor_initData(KMapDataManager& dataManager):m_dataManager(dataManager)
	{
	}
	bool visit(const KMapSetting& ms)
	{
		if(!m_dataManager.openMap(ms.m_mapName))
		{
			Log(LOG_ERROR, "error: map %s no data found", ms.m_mapName);
			return true;
		}

		KMapData* mapData = m_dataManager.getMapData(ms.m_mapName);
		if(!mapData)
		{
			Log(LOG_ERROR, "error: map %s no data found", ms.m_mapName);
			return true;
		}

		((KMapSetting*)&ms)->m_logicMapData = *mapData;
		return true;
	}
};

/// KMapConfig ///

KMapConfig::KMapConfig():m_mapDataManager(NULL)
{

}

KMapConfig::~KMapConfig()
{

}

bool KMapConfig::initialize(const char* settingDir)
{
	char cPath[512];

	ASSERT(!m_mapDataManager);
	sprintf_k(cPath, sizeof(cPath), "%s/WorldMap", settingDir);
	m_mapDataManager = new KMapDataManager(cPath);

	sprintf_k(cPath, sizeof(cPath), "%s/world/CoordinateStatus.txt", settingDir);
	if(!m_pointTable.Initialize(cPath)) return false;

	sprintf_k(cPath, sizeof(cPath), "%s/world/MapSettings.xml", settingDir);
	if(!m_mapSettings.initialize(cPath)) return false;

	// 初始化逻辑地图数据
	KMapVisitor_initData v(*m_mapDataManager);
 	m_mapSettings.foreach(v);

	sprintf_k(cPath, sizeof(cPath), "%s/world/Villages.xml", settingDir);
	if(!m_villageManager.Initialize(m_mapSettings, cPath)) return false;

	//sprintf_k(cPath, sizeof(cPath), "%s/world/NationSettings.xml", settingDir);
	//if(!m_nationConfigure.Initialize(cPath)) return false;

	sprintf_k(cPath, sizeof(cPath), "%s/world/worldinfo.cfg", settingDir);
	if(!m_worldSettings.initialize(m_mapSettings, cPath)) return false;

	return true;
}

void KMapConfig::finalize()
{

}

KMapSettings* KMapConfig::getMapSettings()
{
	return &m_mapSettings;
}

KWorldSettings* KMapConfig::getWorldSettings()
{
	return &m_worldSettings;
}

KCoordinateTable* KMapConfig::getCoordinateTable()
{
	return &m_pointTable;
}

//KNationConfigure* KMapConfig::getNationConfigure()
//{
//	return &m_nationConfigure;
//}

KVillageDataManager* KMapConfig::getVillageDataManager()
{
	return &m_villageManager;
}

const KCoordinatePoint* KMapConfig::getEntryPoint(int mapID, int nation, int village)
{
	const KMapSetting* pms = m_mapSettings.getMapSetting(mapID);
	if(!pms) return NULL;
	int pid = pms->entryPoint(nation);
	return m_pointTable.GetPoint(pid);
}

const KCoordinatePoint* KMapConfig::getBornPoint(int mapID, int nation, int village)
{
	const KMapSetting* pms = m_mapSettings.getMapSetting(mapID);
	if(!pms) return NULL;
	int pid = pms->bornPoint(nation);
	return m_pointTable.GetPoint(pid);
}

const KCoordinatePoint* KMapConfig::getRelivePoint(int mapID, int nation, int village)
{
	if(village)
	{
		const KVillageData* pVillage = m_villageManager.GetVillageData(village);
		if(pVillage)
		{
			int pid = pVillage->m_relivePoints[nation];
			if(pid)
			{
				return m_pointTable.GetPoint(pid);
			}
		}
	}
	{
		const KMapSetting* pms = m_mapSettings.getMapSetting(mapID);
		if(!pms) return NULL;
		int pid = pms->relivePoint(nation);
		return m_pointTable.GetPoint(pid);
	}
}

const KCoordinatePoint* KMapConfig::getExitPoint(int mapID, int nation, int village)
{
	const KMapSetting* pms = m_mapSettings.getMapSetting(mapID);
	if(!pms) return NULL;
	int pid = pms->exitPoint(nation);
	return m_pointTable.GetPoint(pid);
}

const KCoordinatePoint* KMapConfig::getReturnPoint(int mapID, int nation, int village)
{
	const KMapSetting* pms = m_mapSettings.getMapSetting(mapID);
	if(!pms) return NULL;
	int pid = pms->returnPoint(nation);
	return m_pointTable.GetPoint(pid);
}

bool KMapConfig::IsDupMap(int mapID) const
{
	const KMapSetting* ms = m_mapSettings.getMapSetting(mapID);
	if(!ms) return true;
	return ms->m_duplicate ? true : false;
}

int KMapConfig::GetNationNormalMap(int nation)
{
	return 10000;
}

const KCoordinatePoint* KMapConfig::GetPoint(int pointID) const
{
	return m_pointTable.GetPoint(pointID);
}
