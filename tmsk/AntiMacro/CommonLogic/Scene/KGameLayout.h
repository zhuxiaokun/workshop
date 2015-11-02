#ifndef _K_GAME_LAYOUT_H_
#define _K_GAME_LAYOUT_H_

#include <lua/KLuaWrap.h>
#include <System/Collections/DynArray.h>
#include <System/Misc/StreamInterface.h>
#include <System/tinyxml/xmldom.h>
#include "KWorldSetting.h"
#include <System/Memory/KStepObjectPool.h>

struct GameWorld
{
	int m_line;
	int m_gameServer;
	int m_baseIdx;
	const KWorldSetting* m_worldSetting;
	int m_count;

	//////////////////////////////////////////
	GameWorld();
	GameWorld(int gameServer, const KWorldSetting* ws, int count);
	GameWorld(const GameWorld& o);
	GameWorld& operator = (const GameWorld& o);
};

DECLARE_SIMPLE_TYPE(GameWorld);

class KGameServerSetting
	: public System::Memory::KPortableStepPool<KGameServerSetting,32>
{
friend class KGameLineSetting;
public:
	class wcmp
	{
	public:
		bool operator () (const GameWorld& a, const GameWorld& b) const
		{ return a.m_worldSetting->m_worldType < b.m_worldSetting->m_worldType; }
	};
	class IVisitor
	{
	public:
		virtual ~IVisitor() { }
		virtual bool visit(GameWorld& ws) = 0;
	};

public:
	typedef System::Collections::DynSortedArray<GameWorld,wcmp> WorldArray;

public:
	KGameServerSetting();
	~KGameServerSetting();

public:
	void clear();
	bool accept(IVisitor* visitor);

public:
	int serverID() const;
	int getWorldCount() const;
	const GameWorld* getWorld(int idx) const;
	const GameWorld* findWorld(int worldType) const;

private:
	bool _initFromXml(KWorldSettings& wss, XMLElementNode* xml);

public:
	int m_id;
	WorldArray m_worlds;

public:
	BeginDefLuaClassNoCon(KGameServerSetting);
		DefMemberFunc(serverID);
		DefMemberFunc(getWorldCount);
		DefMemberFunc(getWorld);
		DefMemberFunc(findWorld);
	EndDef;
};

class KGameLineSetting
	: public KGameServerSetting::IVisitor
	, public System::Memory::KPortableStepPool<KGameLineSetting,8>
{
friend class KGameLayout;
public:
	class gs_cmp
	{
	public:
		typedef KGameServerSetting* ptr;
		bool operator () (const ptr& a, const ptr& b) const
		{
			return a->m_id < b->m_id;
		}
	};
	typedef System::Collections::DynSortedArray<KGameServerSetting*,gs_cmp> GameServerArray;

public:
	KGameLineSetting();
	~KGameLineSetting();

public:
	void clear();
	bool accept(KGameServerSetting::IVisitor* visitor);
	bool visit(GameWorld& ws); // set line, baseIdx for GameWorld

public:
	int lineID() const;
	int getGameServerCount() const;
	const KGameServerSetting* getGameServer(int idx) const;
	const KGameServerSetting* findGameServer(int gameServerID) const;
	int getWorldCount(int worldType) const;
	int findWorlds(int worldType, GameWorld* worlds, int count) const;

private:
	bool _initFromXml(KWorldSettings& wss, XMLElementNode* xml);

public:
	int m_lineID;
	GameServerArray m_gameServers;

public:
	BeginDefLuaClassNoCon(KGameLineSetting);
		DefMemberFunc(lineID);
		DefMemberFunc(getGameServerCount);
		DefMemberFunc(getGameServer);
		DefMemberFunc(findGameServer);
		DefMemberFunc(getWorldCount);
	EndDef;
};

class KGameLayout
{
public:
	class ls_cmp
	{
	public:
		typedef KGameLineSetting* ptr;
		bool operator() (const ptr& a, const ptr& b) const
		{
			return a->m_lineID < b->m_lineID;
		}
	};
	typedef System::Collections::DynSortedArray<KGameLineSetting*,ls_cmp> LineArray;

public:
	KGameLayout();
	~KGameLayout();

public:
	bool initialize(KWorldSettings& wss, const char* fileName);
	bool initialize(KWorldSettings& wss, StreamInterface& fi);
	void finalize();

public: // 提供一个遍历所有世界的方法
	bool accept(KGameServerSetting::IVisitor* visitor);

public:
	int getLineCount() const;
	const KGameLineSetting* getLineSetting(int idx) const;
	const KGameLineSetting* findLineSetting(int lineID) const;

public:
	int findWorlds(int line, int worldType, GameWorld* worlds, int count) const;
	int findGameServerWorlds(int gameServerID, GameWorld* worlds, int count) const;

public:
	LineArray m_lines;

public:
	BeginDefLuaClassNoCon(KGameLayout);
		DefMemberFunc(getLineCount);
		DefMemberFunc(getLineSetting);
		DefMemberFunc(findLineSetting);
	EndDef;
};

#endif
