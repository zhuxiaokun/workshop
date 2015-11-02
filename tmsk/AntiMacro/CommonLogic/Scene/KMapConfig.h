#ifndef _K_MAP_CONFIG_H_
#define _K_MAP_CONFIG_H_

#include "KVillage.h"
#include "KMapSetting.h"
#include "KWorldSetting.h"
#include "../Configure/KCoordinateTable.h"
//#include "../Community/KCommunity.h"

class KMapConfig
{
public:
	KMapConfig();
	~KMapConfig();

public:
	bool initialize(const char* settingDir);
	void finalize();

public:
	KMapSettings* getMapSettings();
	KWorldSettings* getWorldSettings();
	KCoordinateTable* getCoordinateTable();
	KVillageDataManager* getVillageDataManager();

public:
	const KCoordinatePoint* getEntryPoint(int mapID, int nation, int village=0);
	const KCoordinatePoint* getBornPoint(int mapID, int nation, int village=0);
	const KCoordinatePoint* getRelivePoint(int mapID, int nation, int village=0);
	const KCoordinatePoint* getExitPoint(int mapID, int nation, int village=0);
	const KCoordinatePoint* getReturnPoint(int mapID, int nation, int village=0);

public:
	bool IsDupMap(int mapID) const;
	int GetNationNormalMap(int nation);
	const KCoordinatePoint* GetPoint(int pointID) const;

public:
	KMapDataManager* m_mapDataManager;
	KMapSettings m_mapSettings;
	KWorldSettings m_worldSettings;
	KCoordinateTable m_pointTable;
	KVillageDataManager m_villageManager;

public:
	BeginDefLuaClassNoCon(KMapConfig);
		DefMemberFunc(getMapSettings);
		DefMemberFunc(getWorldSettings);
		DefMemberFunc(getEntryPoint);
		DefMemberFunc(getBornPoint);
		DefMemberFunc(getRelivePoint);
		DefMemberFunc(getExitPoint);
		DefMemberFunc(getReturnPoint);
		DefMemberFunc(IsDupMap);
		DefMemberFunc(GetNationNormalMap);
		DefMemberFunc(GetPoint);
	EndDef;
};

#endif
