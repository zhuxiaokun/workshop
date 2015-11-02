#pragma once

#include <System/KType.h>
#include <System/KMacro.h>
#include <lua/KLuaWrap.h>

class KPlayer;
class IKPlayer;
luaClassPtr2Lua(IKPlayer);
class IKPlayer : public KBaseLuaClass<IKPlayer>
{
public:
	virtual ~IKPlayer() { }
	virtual KPlayer* GetPlayer() { return NULL; }

public:
	virtual DWORD getID() const = 0;
	virtual const char* getName() const = 0;
	virtual int GetLevel() const = 0;
	virtual int getAttrValue(int attrId, int defVal) const = 0;
	virtual int GetMapID() const = 0;
	virtual DWORD GetCurrentVillage() const = 0;
	virtual IKPlayer* GetTeamLeader() = 0;
	virtual float DistanceToOtherPlayer(IKPlayer* o) = 0;
	virtual int getDayEnterTimes(int mapID) = 0;
	virtual int getWeekEnterTimes(int mapID) = 0;
	virtual int getMonthEnterTimes(int mapID) = 0;
	virtual int GetItemCount(int itemType) = 0;
	virtual DWORD GetTeam() const = 0;

public:
	BeginDefLuaClassNoCon(IKPlayer);
	DefMemberFunc(getID);
	DefMemberFunc(getName);
	DefMemberFunc(GetLevel);
	DefMemberFunc(getAttrValue);
	DefMemberFunc(GetMapID);
	DefMemberFunc(GetCurrentVillage);
	DefMemberFunc(GetTeamLeader);
	DefMemberFunc(DistanceToOtherPlayer);
	DefMemberFunc(getDayEnterTimes);
	DefMemberFunc(getWeekEnterTimes);
	DefMemberFunc(getMonthEnterTimes);
	DefMemberFunc(GetItemCount);
	DefMemberFunc(GetTeam);
	EndDef;
};
