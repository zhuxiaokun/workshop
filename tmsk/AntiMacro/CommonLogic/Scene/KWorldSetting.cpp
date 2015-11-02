#include "KWorldSetting.h"
#include <System/tinyxml/xmldom.h>
#include <System/Memory/KBuffer64k.h>
#include "KLogicMap.h"

/// WorldID ////////////////////////////////////

WorldID::WorldID()
	: m_idx(0)
	, m_worldType(0)
	, m_lineID(0)
{

}

WorldID::WorldID(DWORD id)
{
	*(DWORD*)this = id;
}

WorldID::WorldID(int lineID, int worldType, int idx)
	: m_lineID(lineID)
	, m_worldType(worldType)
	, m_idx(idx)
{

}

WorldID::operator DWORD () const
{
	return *(DWORD*)this;
}

JG_C::KString<128> WorldID::toString() const
{
	char buf[128];
	int len = sprintf_k(buf, sizeof(buf), "%u(%d:%d:%d)", (DWORD)*this, m_lineID, m_worldType, m_idx);
	return JG_C::KString<128>(buf, len);
}

/// KWorldSetting //////////////////////

KWorldSetting::KWorldSetting()
{
	this->clear();
}

KWorldSetting::~KWorldSetting()
{

}

void KWorldSetting::clear()
{
	m_mapID = 0;
	m_worldType = 0;
	m_worldName = NULL;
	//m_scenes.clear();
	m_monsters.clear();
	m_tcs.clear();
	m_mapSetting = NULL;
	m_baseIdx = 0;
	m_luaTable = NULL;
}

int KWorldSetting::mapID() const
{
	return m_mapID;
}

int KWorldSetting::worldType() const
{
	return m_worldType;
}

const char* KWorldSetting::worldName() const
{
	return m_worldName;
}

//bool KWorldSetting::hasScene(int sx, int sy) const
//{
//	Scene scene(sx,sy);
//	return m_scenes.find(scene) >= 0;
//}

/// KWorldSettings ///////////////////////////////

KWorldSettings::KWorldSettings()
{

}

KWorldSettings::~KWorldSettings()
{

}

bool KWorldSettings::initialize(KMapSettings& mss, const char* fileName)
{
	KInputFileStream fi;
	if(!fi.Open(fileName))
	{
		Log(LOG_ERROR, "error: KWorldSettings::initialize(%s), open", fileName);
		return false;
	}
	return this->initialize(mss, fi);
}

bool KWorldSettings::initialize(KMapSettings& mss, StreamInterface& fi)
{
	char buf[128];

	XMLDomParser parser;
	if(!parser.Initialize())
	{
		Log(LOG_ERROR, "error: KWorldSettings::initialize, init xml parser");
		return false;
	}

	XMLElementNode xmlRoot;
	if(!parser.Parse(fi, xmlRoot))
	{
		Log(LOG_ERROR, "error: KWorldSettings::initialize, parse xml, %s", parser.GetErrorMsg());
		return false;
	}

	KWorldSetting ws;

	XMLNodeColl coll2; int c;
	XMLNodeColl coll = xmlRoot.QueryChildren2("WorldSettings/World");

	int n = coll.Size();
	for(int i=0; i<n; i++)
	{
		ws.clear();

		XMLElementNode* worldXml = (XMLElementNode*)coll.GetNode(i);
		const char* worldName = worldXml->QueryAtt("name")->c_str();

		ws.m_mapID = worldXml->QueryAtt("map")->Integer();
		ws.m_mapSetting = mss.getMapSetting(ws.m_mapID);
		if(!ws.m_mapSetting)
		{
			Log(LOG_ERROR, "error: KWorldSettings::initialize, map %d not found", ws.m_mapID);
			return false;
		}

		ws.m_worldType = worldXml->QueryAtt("type")->Integer();
		ws.m_worldName = KBuffer64k::WriteData(worldName, strlen(worldName)+1);

		int len = sprintf_k(buf, sizeof(buf), "WorldMonitor_%d", ws.m_worldType);
		ws.m_luaTable = KBuffer64k::WriteData(buf, len+1);

		coll2 = worldXml->QueryChildren2("Monster"); c = coll2.Size();
		for(int k=0; k<c; k++)
		{
			KWorldSetting::Monster monster;
			XMLElementNode* elemXml = (XMLElementNode*)coll2.GetNode(k);
			const char* fileName = elemXml->QueryAtt("value")->c_str();
			monster.first = elemXml->QueryAtt("id")->Integer();
			monster.second = KBuffer64k::WriteData(fileName, strlen(fileName)+1);
			ws.m_monsters.push_back(monster);
		}

		coll2 = worldXml->QueryChildren2("TreasureCase"); c = coll2.Size();
		for(int k=0; k<c; k++)
		{
			XMLElementNode* elemXml = (XMLElementNode*)coll2.GetNode(k);
			const char* fileName = elemXml->QueryAtt("value")->c_str();
			ws.m_tcs.push_back(KBuffer64k::WriteData(fileName, strlen(fileName)+1));
		}

		coll2 = worldXml->QueryChildren2("Transimission"); c = coll2.Size();
		for(int k=0; k<c; k++)
		{
			XMLElementNode* elemXml = (XMLElementNode*)coll2.GetNode(k);
			const char* fileName = elemXml->QueryAtt("value")->c_str();
			ws.m_transes.push_back(KBuffer64k::WriteData(fileName, strlen(fileName)+1));
		}

		coll2 = worldXml->QueryChildren2("Traffic"); c = coll2.Size();
		for(int k=0; k<c; k++)
		{
			XMLElementNode* elemXml = (XMLElementNode*)coll2.GetNode(k);
			int trafficRoute = elemXml->QueryAtt("value")->Integer();
			ws.m_traffics.push_back(trafficRoute);
		}

		//coll2 = worldXml->QueryChildren2("Scene"); c = coll2.Size();
		//for(int k=0; k<c; k++)
		//{
		//	KWorldSetting::Scene scene;
		//	XMLElementNode* elemXml = (XMLElementNode*)coll2.GetNode(k);
		//	scene.first = elemXml->QueryAtt("x")->Integer();
		//	scene.second = elemXml->QueryAtt("y")->Integer();
		//	ws.m_scenes.insert_unique(scene);
		//}

		this->_addWorldType(ws);
	}

	return true;
}

void KWorldSettings::finalize()
{
	return;
}

bool KWorldSettings::foreach(KWorldSetting::IVisitor& v)
{
	WorldMap::iterator it = m_worlds.begin();
	WorldMap::iterator ite = m_worlds.end();
	while(it != ite)
	{
		KWorldSetting* ws = &it->second;
		if(!v.visit(ws)) return false;
		it++;
	}
	return true;
}

bool KWorldSettings::foreach(int mapID, KWorldSetting::IVisitor& v)
{
	Map2Types::iterator it = m_map2types.find(mapID);
	if(it == m_map2types.end()) return true;
	WorldArray& worldArr = it->second;
	int n = worldArr.size();
	for(int i=0; i<n; i++)
	{
		KWorldSetting* ws = worldArr[i];
		if(!v.visit(ws)) return false;
	}
	return true;
}

int KWorldSettings::getWorlds(int mapID, const KWorldSetting* wss[], int count) const
{
	Map2Types::const_iterator it = m_map2types.find(mapID);
	if(it == m_map2types.end()) return 0;

	const WorldArray& arr = it->second;
	int c = arr.size()<count ? arr.size():count;
	for(int i=0; i<c; i++)
	{
		const KWorldSetting* ws = arr.at(i);
		wss[i] = ws;
	}
	return c;
}

KWorldSetting* KWorldSettings::getWorldSetting(int worldType)
{
	WorldMap::iterator it = m_worlds.find(worldType);
	if(it == m_worlds.end()) return NULL;
	return &it->second;
}

const KWorldSetting* KWorldSettings::getWorldSetting(int worldType) const
{
	WorldMap::const_iterator it = m_worlds.find(worldType);
	if(it == m_worlds.end()) return NULL;
	return &it->second;
}

const KWorldSetting* KWorldSettings::findWorldSetting(int mapID) const
{
	Map2Types::const_iterator it = m_map2types.find(mapID);
	if(it == m_map2types.end()) return NULL;
	const WorldArray& arr = it->second;
	return arr[0];
}

bool KWorldSettings::_addWorldType(KWorldSetting& ws)
{
	int wtype = ws.m_worldType;
	m_worlds[wtype] = ws;
	m_map2types[wtype].push_back(&m_worlds[wtype]);
	return true;
}

KWorldSetting* KWorldSettings::getWorldSetting_lua(int worldType)
{
	return this->getWorldSetting(worldType);
}

BOOL operator < (const KWorldSetting::Scene& a, const KWorldSetting::Scene& b)
{
	return memcmp(&a, &b, sizeof(a)) < 0;
}

BOOL operator < (const KWorldSetting::Monster& a, const KWorldSetting::Monster& b)
{
	return memcmp(&a, &b, sizeof(a)) < 0;
}