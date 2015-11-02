#ifndef _K_MAP_SETTING_H_
#define _K_MAP_SETTING_H_

#include <KCommonDefine.h>
#include <lua/KLuaWrap.h>
#include <System/Collections/KTree.h>
#include <System/Misc/StreamInterface.h>
#include "KLogicMap.h"

class KMapSetting : public KBaseLuaClass<KMapSetting>
{
public:
	enum { nation_count = MAX_NATION };
	typedef KBaseLuaClass<KMapSetting> base_type;
	class IVisitor
	{
	public:
		virtual ~IVisitor() { }
		virtual bool visit(const KMapSetting& ms) = 0;
	};

public:
	KMapSetting();
	~KMapSetting();

public:
	void clear();

public:
	int mapID() const;
	const char* mapName() const;
	const char* descName() const;

public: // get attribute from lua MAP_XXXX
	bool getBoolAttr(const char* name, bool defVal) const;
	int getIntAttr(const char* name, int defVal) const;
	LuaString getStringAttr(const char* name, const char* defVal) const;
	LMREG::CppTable getTableAttr(const char* name) const;

public:
	int entryPoint(int nation) const;
	int bornPoint(int nation) const;
	int relivePoint(int nation) const;
	int exitPoint(int nation) const;
	int returnPoint(int nation) const;

public:
	bool setEntryPoint(int nation, int point);
	bool setBornPoint(int nation, int point);
	bool setRelivePoint(int nation, int point);
	bool setExitPoint(int nation, int point);
	bool setReturnPoint(int nation, int point);

public:
	int  m_mapID;
	BOOL m_duplicate;
	const char* m_mapName;
	const char* m_descName;
	int  m_entryPoints[nation_count];
	int  m_bornPoints[nation_count];
	int  m_relivePoints[nation_count];
	int  m_exitPoints[nation_count];
	int  m_returnPoints[nation_count];

public: // Map_X中的一些属性
	int m_createTimeout;
	int m_idleTimeout;
	BOOL m_carePlayerEnter;

public:
	const char* m_luaTable;
	KLogicMap m_logicMapData;

public:
	BeginDefLuaClassNoCon(KMapSetting);
		DefMemberFunc(mapID);
		DefMemberFunc(mapName);
		DefMemberFunc(descName);
		DefMemberFunc(getBoolAttr);
		DefMemberFunc(getIntAttr);
		DefMemberFunc(getStringAttr);
		DefMemberFunc(getTableAttr);
		DefMemberFunc(entryPoint);
		DefMemberFunc(bornPoint);
		DefMemberFunc(relivePoint);
		DefMemberFunc(exitPoint);
		DefMemberFunc(returnPoint);
		DefMemberFunc(setEntryPoint);
		DefMemberFunc(setBornPoint);
		DefMemberFunc(setRelivePoint);
		DefMemberFunc(setExitPoint);
		DefMemberFunc(setReturnPoint);
	EndDef;
};

class KMapSettings
{
public:
	typedef System::Collections::KMapByTree_pooled<int,KMapSetting,32> Map;

public:
	KMapSettings();
	~KMapSettings();

public:
	bool initialize(const char* fileName);
	bool initialize(StreamInterface& si);
	void finalize();

public:
	bool foreach(KMapSetting::IVisitor& v);

public:
	KMapSetting* getMapSetting(int mapID);
	const KMapSetting* getMapSetting(int mapID) const;

public:
	Map m_maps;

public: // lua interface
	KMapSetting* getMapSetting_lua(int mapID);

public:
	BeginDefLuaClassNoCon(KMapSettings);
		DefMemberFunc2(getMapSetting_lua,"getMapSetting");
	EndDef;
};

luaClassPtr2Lua(KMapSetting);
#endif
