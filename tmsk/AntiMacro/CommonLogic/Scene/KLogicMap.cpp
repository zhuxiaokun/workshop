#include "KLogicMap.h"
#include <System/Misc/StrUtil.h>
#include <System/File/KFile.h>

// KMapData ///

KMapData::KMapData()
	: m_width(0)
	, m_height(0)
	, m_cells(NULL)
	, m_refCount(NULL)
{

}

KMapData::KMapData(const KMapData& o)
	: m_width(o.m_width)
	, m_height(o.m_height)
	, m_cells(o.m_cells)
	, m_refCount(o.m_refCount)
{
	if(m_refCount) (*m_refCount)++;
}

KMapData::~KMapData()
{
	this->finalize();
}

KMapData& KMapData::operator=(const KMapData& o)
{
	if(this == &o) return *this;
	this->finalize();
	m_width = o.m_width;
	m_height = o.m_height;
	m_cells = o.m_cells;
	m_refCount = o.m_refCount;
	if(m_refCount) (*m_refCount)++;
	return *this;
}

bool KMapData::initialize(int width, int height)
{
	this->finalize();

	ASSERT(width > 0 && width <= MAX_MAP_WIDTH);
	ASSERT(height > 0 && height <= MAX_MAP_HEIGHT);
	ASSERT(width%REGION_WIDTH == 0);
	ASSERT(height%REGION_HEIGHT == 0);

	int len = width * height * sizeof(KCellData);
	KCellData* cells = (KCellData*)malloc_k(len);
	memset(cells, 0, len);
	
	m_width = width;
	m_height = height;
	m_cells = cells;
	m_refCount = new int(1);

	return true;
}

bool KMapData::initialize(StreamInterface& si)
{
	this->finalize();
	DWORD width, height;

	if(si.ReadData(&width, sizeof(width)) != sizeof(width))
		return false;
	
	if(si.ReadData(&height, sizeof(height)) != sizeof(height))
		return false;

	ASSERT(width <= MAX_MAP_WIDTH);
	ASSERT(height <= MAX_MAP_HEIGHT);
	ASSERT(width%REGION_WIDTH == 0);
	ASSERT(height%REGION_HEIGHT == 0);

	int len = width * height * sizeof(KCellData);
	KCellData* cells = (KCellData*)malloc_k(len);
	if(si.ReadData(cells, len) != len)
	{
		free_k(cells);
		return false;
	}
	
	m_width = width;
	m_height = height;
	m_cells = cells;
	m_refCount = new int(1);

	return true;
}

void KMapData::finalize()
{
	if(!m_refCount) return;

	int& refCount = *m_refCount;
	ASSERT(refCount > 0); refCount--;
	if(refCount)
	{
		m_width = 0;
		m_height = 0;
		m_cells = NULL;
		m_refCount = NULL;
		return;
	}

	if(m_cells) free_k(m_cells);
	m_width = 0;
	m_height = 0;
	m_cells = NULL;
	delete m_refCount;
	m_refCount = NULL;
}

KCellData* KMapData::getCell(int cx, int cy)
{
	if(cx<0 || cx>=m_width) return NULL;
	if(cy<0 && cy>=m_height) return NULL;
	int idx = cy*m_width+cx;
	return &m_cells[idx];
}

KRegionData* KMapData::getRegion(int rx, int ry)
{
	int x = rx<<REGION_WIDTH_BITS;
	int y = ry<<REGION_HEIGHT_BITS;
	KCellData* pCell = this->getCell(x, y);
	if(!pCell) return NULL;
	return (KRegionData*)pCell;
}

const KCellData* KMapData::getCell(int cx, int cy) const
{
	if(cx<0 || cx>=m_width) return NULL;
	if(cy<0 && cy>=m_height) return NULL;
	int idx = cy*m_width+cx;
	return &m_cells[idx];
}

const KRegionData* KMapData::getRegion(int rx, int ry) const
{
	int x = rx<<REGION_WIDTH_BITS;
	int y = ry<<REGION_HEIGHT_BITS;
	const KCellData* pCell = this->getCell(x, y);
	if(!pCell) return NULL;
	return (const KRegionData*)pCell;
}

bool KMapData::save(StreamInterface& fo)
{
	DWORD width = m_width;
	DWORD height = m_height;
	
	if(fo.WriteData(&width, sizeof(width)) != sizeof(width))
		return false;
	
	if(fo.WriteData(&height, sizeof(height)) != sizeof(height))
		return false;

	int len = m_width*m_height*sizeof(KCellData);
	if(fo.WriteData(m_cells, len) != len)
		return false;
	
	fo.flush();
	return true;
}

/// KMapDataManager ///

KMapDataManager::KMapDataManager(const char* mapDir)
{
	char path[512];
	ASSERT(mapDir && mapDir[0]);
	getfullpath(path, mapDir, sizeof(path));
	m_mapDir = path;
}

KMapDataManager::~KMapDataManager()
{

}

bool KMapDataManager::openMap(const char* mapName)
{
	KMapData* mapData = this->getMapData(mapName);
	if(mapData) return true;

	JG_C::KString<512> filepath = this->_getFilePath(mapName);

	KInputFileStream fi;
	if(!fi.Open(filepath.c_str()))
		return false;

	KMapData mdt;
	if(!mdt.initialize(fi))
		return false;

	MapDesc mapDesc;
	mapDesc.mapName = KBuffer64k::WriteData(mapName, strlen(mapName)+1);
	mapDesc.mapData = mdt;
	m_mapByName[mapDesc.mapName] = mapDesc;

	return true;
}

bool KMapDataManager::createMap(const char* mapName, int width, int height)
{
	KMapData* mapData = this->getMapData(mapName);
	if(mapData) return false;

	JG_C::KString<512> filepath = this->_getFilePath(mapName);
	FILE* f = JG_F::KFileUtil::OpenFileForce(filepath.c_str(), "r+b");
	if(!f) return false;

	KOutputFileStream fo(f);

	KMapData mdt;
	if(!mdt.initialize(width, height))
	{
		fclose(f);
		return false;
	}

	if(!mdt.save(fo))
	{
		fclose(f);
		return false;
	}

	DWORD size = fo.tell();
	JG_F::KFileUtil::SetFileSize(f, size);
	fclose(f);

	MapDesc desc;
	desc.mapName = KBuffer64k::WriteData(mapName, strlen(mapName)+1);
	desc.mapData = mdt;
	m_mapByName[desc.mapName] = desc;

	return true;
}

bool KMapDataManager::openOrCreate(const char* mapName, int width, int height)
{
	if(this->openMap(mapName))
	{
		KMapData* mdt = this->getMapData(mapName); ASSERT(mdt);
		return (mdt->m_width == width && mdt->m_height == height);
	}
	return this->createMap(mapName, width, height);
}

bool KMapDataManager::closeMap(const char* mapName)
{
	MapByName::iterator it = m_mapByName.find(mapName);
	if(it == m_mapByName.end()) return true;
	m_mapByName.erase(it);
	return true;
}

bool KMapDataManager::deleteMap(const char* mapName)
{
	this->closeMap(mapName);
	JG_C::KString<512> filepath = this->_getFilePath(mapName);
	JG_F::KFileUtil::deleteFile(filepath.c_str());
	return true;
}

KMapData* KMapDataManager::getMapData(const char* mapName)
{
	MapByName::iterator it = m_mapByName.find(mapName);
	if(it == m_mapByName.end()) return NULL;
	return &it->second.mapData;
}

JG_C::KString<512> KMapDataManager::_getFilePath(const char* mapName) const
{
	JG_C::KString<512> filepath(m_mapDir);
	filepath.append('/').append(mapName);
	filepath.append('/').append("LogicMap");
	filepath.append('/').append(mapName).append(".mdt");
	return filepath;
}

/// KLogicMap 

KLogicMap::KLogicMap()
{

}

KLogicMap::KLogicMap(KMapData& mapData):m_mapData(mapData)
{

}

KLogicMap::~KLogicMap()
{

}

KLogicMap& KLogicMap::operator = (KMapData& mapData)
{
	if(&m_mapData == &mapData) return *this;
	m_mapData = mapData;
	return *this;
}

int KLogicMap::cellWidth() const
{
	return m_mapData.m_width;
}

int KLogicMap::cellHeight() const
{
	return m_mapData.m_height;
}

int KLogicMap::regionWidth() const
{
	return this->cellWidth() >> REGION_WIDTH_BITS;
}

int KLogicMap::regionHeight() const
{
	return this->cellHeight() >> REGION_HEIGHT_BITS;
}

bool KLogicMap::has(float fx, float fy) const
{
	return this->getCell(fx, fy) != NULL;
}

KCellData* KLogicMap::getCell(float fx, float fy)
{
	int cx = (int)(fx * METER_CELLS);
	int cy = (int)(fy * METER_CELLS);
	return m_mapData.getCell(cx, cy);
}

const KCellData* KLogicMap::getCell(float fx, float fy) const
{
	int cx = (int)(fx * METER_CELLS);
	int cy = (int)(fy * METER_CELLS);
	return m_mapData.getCell(cx, cy);
}

bool KLogicMap::setCell(float fx, float fy, const KCellData& cell)
{
	int cx = (int)(fx * METER_CELLS);
	int cy = (int)(fy * METER_CELLS);
	KCellData* pcell = m_mapData.getCell(cx, cy);
	if(!pcell) return false;
	*pcell = cell;
	return true;
}

bool KLogicMap::setNpcWalk(float fx, float fy, bool val)
{
	int flagBit = cell_allow_npc_walk;
	KCellData* cell = this->getCell(fx, fy);
	if(!cell) return false;
	if(val) this->_setCellFlag(*cell, flagBit);
	else this->_clearCellFlag(*cell, flagBit);
	return true;
}

bool KLogicMap::setHeroWalk(float fx, float fy, bool val)
{
	int flagBit = cell_allow_hero_walk;
	KCellData* cell = this->getCell(fx, fy);
	if(!cell) return false;
	if(val) this->_setCellFlag(*cell, flagBit);
	else this->_clearCellFlag(*cell, flagBit);
	return true;
}

bool KLogicMap::setPutTower(float fx, float fy, bool val)
{
	int flagBit = cell_allow_put_tower;
	KCellData* cell = this->getCell(fx, fy);
	if(!cell) return false;
	if(val) this->_setCellFlag(*cell, flagBit);
	else this->_clearCellFlag(*cell, flagBit);
	return true;
}

bool KLogicMap::setBlock(float fx, float fy, bool val)
{
	int flagBit = cell_block;
	KCellData* cell = this->getCell(fx, fy);
	if(!cell) return false;
	if(val) this->_setCellFlag(*cell, flagBit);
	else this->_clearCellFlag(*cell, flagBit);
	return true;
}

bool KLogicMap::isNpcWalk(float fx, float fy) const
{
	int flagBit = cell_allow_npc_walk;
	const KCellData* cell = this->getCell(fx, fy);
	return cell ? this->_isCellFlag(*cell, flagBit) : false;
}

bool KLogicMap::isHeroWalk(float fx, float fy) const
{
	int flagBit = cell_allow_hero_walk;
	const KCellData* cell = this->getCell(fx, fy);
	return cell ? this->_isCellFlag(*cell, flagBit) : false;
}

bool KLogicMap::isPutTower(float fx, float fy) const
{
	int flagBit = cell_allow_put_tower;
	const KCellData* cell = this->getCell(fx, fy);
	return cell ? this->_isCellFlag(*cell, flagBit) : false;
}

bool KLogicMap::isBlock(float fx, float fy) const
{
	int flagBit = cell_block;
	const KCellData* cell = this->getCell(fx, fy);
	return cell ? this->_isCellFlag(*cell, flagBit) : false;
}

void KLogicMap::_setCellFlag(KCellData& cell, int flagBit)
{
	cell.flags |= 1<<flagBit;
}

void KLogicMap::_clearCellFlag(KCellData& cell, int flagBit)
{
	cell.flags &= ~(1<<flagBit);
}

bool KLogicMap::_isCellFlag(const KCellData& cell, int flagBit) const
{
	return (cell.flags & (1<<flagBit)) != 0;
}
