#ifndef _K_LOGIC_MAP_H_
#define _K_LOGIC_MAP_H_

#include <System/KType.h>
#include <System/KMacro.h>
#include <System/File/KFile.h>
#include <System/Memory/KStepObjectPool.h>
#include <System/Collections/DynArray.h>
#include <System/Misc/KStream.h>
#include <System/Collections/KMapByVector.h>
#include <System/Memory/KBuffer64k.h>

#define MAX_MAP_WIDTH		(10240)
#define MAX_MAP_HEIGHT		(10240)
#define REGION_WIDTH_BITS	6
#define REGION_HEIGHT_BITS	6
#define REGION_WIDTH		(1<<REGION_WIDTH_BITS)
#define REGION_HEIGHT		(1<<REGION_HEIGHT_BITS)
#define METER_CELLS			4
#define NORMAL_MAP_REGIONS	(20*20)

enum
{
	cell_allow_npc_walk		= 0,
	cell_allow_hero_walk	= 1,
	cell_allow_put_tower	= 2,
	cell_block				= 3,
};

struct KCellData
{
	DWORD flags    : 8;		// 格子的标志位
	DWORD village  : 10;	// CELL_VILLAGE_BITS 村庄ID，地图唯一
	DWORD reserved : 14;	// (32-8-CELL_VILLAGE_BITS);
};

struct KRegionData
{
	KCellData cells[REGION_HEIGHT][REGION_WIDTH];
};

class KMapData
{
public:
	KMapData();
	KMapData(const KMapData& o);
	~KMapData();

public:
	KMapData& operator=(const KMapData& o);

public:
	bool initialize(int width, int height);
	bool initialize(StreamInterface& si);
	void finalize();

public:
	KCellData* getCell(int cx, int cy);
	KRegionData* getRegion(int rx, int ry);
	const KCellData* getCell(int cx, int cy) const;
	const KRegionData* getRegion(int rx, int ry) const;

public:
	bool save(StreamInterface& fo);

public:
	int m_width;	// cells
	int m_height;	// cells
	KCellData* m_cells;
	int* m_refCount;
};

class KMapDataManager
{
public:
	struct MapDesc
	{
		char* mapName;
		KMapData mapData;
	};
	typedef JG_C::KMapByVector2<ccstr,MapDesc,KccstrCmp> MapByName;

public:
	KMapDataManager(const char* mapDir);
	~KMapDataManager();

public:
	bool openMap(const char* mapName);
	bool createMap(const char* mapName, int width, int height);
	bool openOrCreate(const char* mapName, int width, int height);
	bool closeMap(const char* mapName);
	bool deleteMap(const char* mapName);

public:
	KMapData* getMapData(const char* mapName);

protected:
	JG_C::KString<512> _getFilePath(const char* mapName) const;

public:
	MapByName m_mapByName;
	JG_C::KString<512> m_mapDir;
};

class KLogicMap
{
public:
	KLogicMap();
	KLogicMap(KMapData& mapData);
	~KLogicMap();

public:
	KLogicMap& operator = (KMapData& mapData);
	void clear() { m_mapData = KMapData(); }

public:
	int cellWidth() const;
	int cellHeight() const;
	int regionWidth() const;
	int regionHeight() const;

public:
	bool has(float fx, float fy) const;

public:
	KCellData* getCell(float fx, float fy);
	const KCellData* getCell(float fx, float fy) const;
	bool setCell(float fx, float fy, const KCellData& cell);

public:
	bool setNpcWalk(float fx, float fy, bool val);
	bool setHeroWalk(float fx, float fy, bool val);
	bool setPutTower(float fx, float fy, bool val);
	bool setBlock(float fx, float fy, bool val);

public:
	bool isNpcWalk(float fx, float fy) const;
	bool isHeroWalk(float fx, float fy) const;
	bool isPutTower(float fx, float fy) const;
	bool isBlock(float fx, float fy) const;

public:
	void _setCellFlag(KCellData& cell, int flagBit);
	void _clearCellFlag(KCellData& cell, int flagBit);
	bool _isCellFlag(const KCellData& cell, int flagBit) const;

public:
	KMapData m_mapData;
};

#endif
