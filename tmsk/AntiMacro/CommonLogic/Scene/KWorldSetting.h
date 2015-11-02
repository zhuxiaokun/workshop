#ifndef _K_WORLD_SETTING_H_
#define _K_WORLD_SETTING_H_

#include <lua/KLuaWrap.h>
#include <System/Misc/StreamInterface.h>
#include <System/Collections/KTree.h>
#include <System/Collections/DynArray.h>
#include <System/Misc/StreamInterface.h>
#include "KMapSetting.h"

struct WorldID
{
	enum 
	{
		max_idx = 1<<14,
		max_world_type = 1<<14,
		max_line = 1<<4,
	};

	//////////////////////////////////////////////////

	DWORD m_idx			: 14;
	DWORD m_worldType	: 14;
	DWORD m_lineID		: 4;

	/////////////////////////////////////////////////

	WorldID();
	WorldID(DWORD id);
	WorldID(int lineID, int worldType, int idx);
	operator DWORD () const;
	JG_C::KString<128> toString() const;
};

class KWorldSetting : public KBaseLuaClass<KWorldSetting>
{
public:
	typedef char* cstr;
	typedef KPair<int,int> Scene;
	typedef KPair<int,cstr> Monster;
	typedef JG_C::DynArray<int> TrafficArray;
	typedef JG_C::DynArray<cstr>  cstrArray;
	//typedef JG_C::DynSortedArray<Scene> SceneArray;
	typedef JG_C::DynArray<Monster> MonsterArray;

public:
	class IVisitor
	{
	public:
		virtual ~IVisitor() { }
		virtual bool visit(KWorldSetting* ws) = 0;
	};

public:
	KWorldSetting();
	~KWorldSetting();

public:
	void clear();

public:
	int mapID() const;
	int worldType() const;
	const char* worldName() const;
	//bool hasScene(int sx, int sy) const;

public:
	int m_mapID;
	int m_worldType;
	const char* m_worldName;
	//SceneArray m_scenes;
	MonsterArray m_monsters;
	cstrArray m_tcs;
	cstrArray m_transes; // transimission
	TrafficArray m_traffics; // traffic route
	const KMapSetting* m_mapSetting;
	int m_baseIdx; // 内部使用
	const char* m_luaTable;

public:
	BeginDefLuaClassNoCon(KWorldSetting);
		DefMemberFunc(mapID);
		DefMemberFunc(worldType);
		DefMemberFunc(worldName);
		//DefMemberFunc(hasScene);
	EndDef;
};

class KWorldSettings
{
public:
	typedef System::Collections::DynArray<KWorldSetting*> WorldArray;
	typedef System::Collections::KMapByTree_pooled<int,KWorldSetting,32> WorldMap;
	typedef System::Collections::KMapByTree_pooled<int,WorldArray> Map2Types;
	typedef WorldMap::iterator iterator;
	typedef WorldMap::const_iterator const_iterator;

public:
	KWorldSettings();
	~KWorldSettings();

public:
	bool initialize(KMapSettings& mss, const char* fileName);
	bool initialize(KMapSettings& mss, StreamInterface& fi);
	void finalize();

public:
	bool foreach(KWorldSetting::IVisitor& v);
	bool foreach(int mapID, KWorldSetting::IVisitor& v);

public:
	int getWorlds(int mapID, const KWorldSetting* wss[], int count) const;

public:
	KWorldSetting* getWorldSetting(int worldType);
	const KWorldSetting* getWorldSetting(int worldType) const;
	const KWorldSetting* findWorldSetting(int mapID) const;

private:
	bool _addWorldType(KWorldSetting& ws);

public:
	WorldMap m_worlds;
	Map2Types m_map2types;

public:// lua interface
	KWorldSetting* getWorldSetting_lua(int worldType);

public:
	BeginDefLuaClassNoCon(KWorldSettings);
		DefMemberFunc2(getWorldSetting_lua,"getWorldSetting");
		//DefMemberFunc(findWorldSetting);
	EndDef;
};

DECLARE_SIMPLE_TYPE(KWorldSetting::Scene);
DECLARE_SIMPLE_TYPE(KWorldSetting::Monster);
BOOL operator < (const KWorldSetting::Scene& a, const KWorldSetting::Scene& b);
BOOL operator < (const KWorldSetting::Monster& a, const KWorldSetting::Monster& b);

luaClassPtr2Lua(KWorldSetting);
#endif
