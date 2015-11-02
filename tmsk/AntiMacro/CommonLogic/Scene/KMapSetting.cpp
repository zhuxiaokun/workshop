#include "KMapSetting.h"
#include <System/tinyxml/xmldom.h>
#include <System/Log/log.h>
#include <System/Memory/KBuffer64k.h>

/// KMapSetting /////////////////////////////

KMapSetting::KMapSetting()
{
	this->clear();
}

KMapSetting::~KMapSetting()
{

}

void KMapSetting::clear()
{
	m_mapID = 0;
	m_duplicate = FALSE;
	m_mapName = NULL;
	m_descName = NULL;
	memset(&m_entryPoints, 0, sizeof(m_entryPoints));
	memset(&m_bornPoints, 0, sizeof(m_bornPoints));
	memset(&m_relivePoints, 0, sizeof(m_relivePoints));
	memset(&m_exitPoints, 0, sizeof(m_exitPoints));
	memset(&m_returnPoints, 0, sizeof(m_returnPoints));
	m_createTimeout = 0;
	m_idleTimeout = 0;
	m_carePlayerEnter = FALSE;
	m_luaTable = NULL;
	m_logicMapData.clear();
}

int KMapSetting::mapID() const
{
	return m_mapID;
}

const char* KMapSetting::mapName() const
{
	return m_mapName;
}

const char* KMapSetting::descName() const
{
	return m_descName;
}

bool KMapSetting::getBoolAttr(const char* name, bool defVal) const
{
	lua_State* L = LuaWraper.m_l;
	KLuaStackRecover recover(L);

	lua_getfield(L, LUA_GLOBALSINDEX, m_luaTable);
	if(lua_isnil(L, -1)) return defVal;

	lua_pushstring(L, name);
	lua_gettable(L, -2);

	if(lua_isboolean(L, -1))
		return lua_toboolean(L, -1) != 0;
	else
		return defVal;
}

int KMapSetting::getIntAttr(const char* name, int defVal) const
{
	lua_State* L = LuaWraper.m_l;
	KLuaStackRecover recover(L);

	lua_getfield(L, LUA_GLOBALSINDEX, m_luaTable);
	if(lua_isnil(L, -1)) return defVal;

	lua_pushstring(L, name);
	lua_gettable(L, -2);
	
	if(lua_isnumber(L, -1))
		return (int)lua_tonumber(L, -1);
	else
		return defVal;
}

LuaString KMapSetting::getStringAttr(const char* name, const char* defVal) const
{
	lua_State* L = LuaWraper.m_l;
	KLuaStackRecover recover(L);

	lua_getfield(L, LUA_GLOBALSINDEX, m_luaTable);
	if(lua_isnil(L, -1)) return defVal;

	lua_pushstring(L, name);
	lua_gettable(L, -2);

	if(lua_isstring(L, -1))
	{
		size_t l;
		const char* s = lua_tolstring(L, -1, &l);
		return LuaString(s, l);
	}
	else
		return defVal;

}

LMREG::CppTable KMapSetting::getTableAttr(const char* name) const
{
	lua_State* L = LuaWraper.m_l;
	KLuaStackRecover recover(L);

	lua_getfield(L, LUA_GLOBALSINDEX, m_luaTable);
	if(lua_isnil(L, -1)) return LMREG::CppTable();

	lua_pushstring(L, name);
	lua_gettable(L, -2);

	if(lua_isstring(L, -1))
		return LMREG::read<LMREG::CppTable>(L,-1);
	else
		return LMREG::CppTable();
}

int KMapSetting::entryPoint(int nation) const
{
	ASSERT(nation >= 0 && nation < nation_count);
	return m_entryPoints[nation];
}

int KMapSetting::bornPoint(int nation) const
{
	ASSERT(nation >= 0 && nation < nation_count);
	return m_bornPoints[nation];
}

int KMapSetting::relivePoint(int nation) const
{
	ASSERT(nation >= 0 && nation < nation_count);
	return m_relivePoints[nation];
}

int KMapSetting::exitPoint(int nation) const
{
	ASSERT(nation >= 0 && nation < nation_count);
	return m_exitPoints[nation];
}

int KMapSetting::returnPoint(int nation) const
{
	ASSERT(nation >= 0 && nation < nation_count);
	return m_returnPoints[nation];
}

bool KMapSetting::setEntryPoint(int nation, int point)
{
	if(nation < 0 || nation >= nation_count)
		return false;
	m_entryPoints[nation] = point;;
	return true;
}

bool KMapSetting::setBornPoint(int nation, int point)
{
	if(nation < 0 || nation >= nation_count)
		return false;
	m_bornPoints[nation] = point;;
	return true;
}

bool KMapSetting::setRelivePoint(int nation, int point)
{
	if(nation < 0 || nation >= nation_count)
		return false;
	m_relivePoints[nation] = point;;
	return true;
}

bool KMapSetting::setExitPoint(int nation, int point)
{
	if(nation < 0 || nation >= nation_count)
		return false;
	m_exitPoints[nation] = point;;
	return true;
}

bool KMapSetting::setReturnPoint(int nation, int point)
{
	if(nation < 0 || nation >= nation_count)
		return false;
	m_returnPoints[nation] = point;;
	return true;
}

/// KMapSettings /////////////////////////////////

KMapSettings::KMapSettings()
{

}

KMapSettings::~KMapSettings()
{

}

bool KMapSettings::initialize(const char* fileName)
{
	KInputFileStream fi;
	if(!fi.Open(fileName))
	{
		Log(LOG_ERROR, "error: KMapSettings::initialize(%s), open.", fileName);
		return false;
	}
	return this->initialize(fi);
}

bool KMapSettings::initialize(StreamInterface& si)
{
	char buf[64];

	XMLDomParser parser;
	if(!parser.Initialize())
	{
		Log(LOG_ERROR, "error: KMapSettings::initialize, init xml parser");
		return false;
	}
	
	XMLElementNode xmlRoot;
	if(!parser.Parse(si, xmlRoot))
	{
		Log(LOG_ERROR, "error: KMapSettings::initialize, parse xml, %s", parser.GetErrorMsg());
		return false;
	}

	KMapSetting ms;
	XMLNodeColl coll = xmlRoot.QueryChildren2("MapSettings/Map");
	int n = coll.Size();
	for(int i=0; i<n; i++)
	{
		ms.clear();

		XMLElementNode* mapXml = (XMLElementNode*)coll.GetNode(i);
		const char* mapName = mapXml->QueryAtt("file")->c_str();
		const char* descName = mapXml->QueryAtt("name")->c_str();

		ms.m_mapID = mapXml->QueryAtt("id")->Integer();
		if(mapXml->QueryAtt("dup"))
		{
			ms.m_duplicate = mapXml->QueryAtt("dup")->Integer() ? 1:0;
		}
		ms.m_mapName = KBuffer64k::WriteData(mapName, strlen(mapName)+1);
		ms.m_descName = KBuffer64k::WriteData(descName, strlen(descName)+1);

		if(mapXml->QueryAtt("EntryPoint.value")) ms.setEntryPoint(0, mapXml->QueryAtt("EntryPoint.value")->Integer());
		if(mapXml->QueryAtt("BornPoint.value")) ms.setBornPoint(0, mapXml->QueryAtt("BornPoint.value")->Integer());
		if(mapXml->QueryAtt("RelivePoint.value")) ms.setRelivePoint(0, mapXml->QueryAtt("RelivePoint.value")->Integer());
		if(mapXml->QueryAtt("ExitPoint.value")) ms.setExitPoint(0, mapXml->QueryAtt("ExitPoint.value")->Integer());
		if(mapXml->QueryAtt("ReturnPoint.value")) ms.setReturnPoint(0, mapXml->QueryAtt("ReturnPoint.value")->Integer());

		int len = sprintf_k(buf, sizeof(buf), "Map_%d", ms.m_mapID);
		ms.m_luaTable = KBuffer64k::WriteData(buf, len+1);

		ms.m_createTimeout = ms.getIntAttr("createTimeout", 2*60);
		ms.m_idleTimeout = ms.getIntAttr("idleTimeout", ms.m_duplicate?5*60:-1);
		ms.m_carePlayerEnter = ms.getBoolAttr("carePlayerEnter", false) ? TRUE : FALSE;

		m_maps[ms.m_mapID] = ms;
	}
	return true;
}

void KMapSettings::finalize()
{

}

bool KMapSettings::foreach(KMapSetting::IVisitor& v)
{
	Map::iterator it = m_maps.begin();
	for(; it!=m_maps.end(); it++)
	{
		if(!v.visit(it->second))
			return false;
	}
	return true;
}

KMapSetting* KMapSettings::getMapSetting(int mapID)
{
	Map::iterator it = m_maps.find(mapID);
	if(it == m_maps.end()) return NULL;
	return &it->second;
}

const KMapSetting* KMapSettings::getMapSetting(int mapID) const
{
	Map::const_iterator it = m_maps.find(mapID);
	if(it == m_maps.end()) return NULL;
	return &it->second;
}

KMapSetting* KMapSettings::getMapSetting_lua(int mapID)
{
	return this->getMapSetting(mapID);
}
