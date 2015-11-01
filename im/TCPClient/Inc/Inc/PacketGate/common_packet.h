#pragma once // 公用的包结构
#include <System/KType.h>
#include <KCommonDefine.h>
#include <KCommonStruct.h>

#pragma pack(push, 1)

namespace EnumAppearData
{
	enum
	{
		stopState,
		moveState,
		appearDataCount,
	};
}

struct _MoveNode
{
	float x, y, z;
};

struct PlayerChangeSceneReq
{
	KObjectID playerID;
	int mapID;
	int worldID;
	int pointID;
	_MoveNode mNewPos;
	_MoveNode mOldPos;
	USHORT mAngle;
	USHORT mAngleZ;
	bool bOnlyChangeLine;
};

struct PlayerChangeSceneRsp
{
	int    m_Result;
	int     m_msgID;
	KObjectID	m_dwPlayerID;
	DWORD	m_dwMapID;
	DWORD	m_dwDuplicateID;
	CHAR	m_strGameServerIP[16];
	DWORD	m_dwGameServerPort;
	bool	bOnlyChangeLine;
};

struct WorldPlayerInformation
{
	char name[MAX_NAME_DEF];
	KPassport passport;
	DWORD otpCode;
	DWORD mapID;
	KObjectID guildID, answerGuildID;
	DWORD teamID;
	BYTE masterLevel;
	BYTE nation;
	BYTE level;
	BYTE side;
	BYTE force;
	BYTE cls, subcls;
	BYTE sex;
	float equipMark;
	DWORD villageID;
	DWORD lastLevelUpTime;
};

struct PlayerDataHandlers
{
	HANDLE hPlayer;
	//HANDLE hHero;
	HANDLE hSkill;
	HANDLE hItem;
	HANDLE hQuest;
	//HANDLE hPeriod;
	HANDLE hFriendship;
	//HANDLE hGuide;
	//HANDLE hMonster;

	// ...
	void reset()
	{
		hPlayer = (HANDLE)0x7fffffff;
		//hHero = (HANDLE)0x7fffffff;
		hSkill = (HANDLE)0x7fffffff;
		hItem = (HANDLE)0x7fffffff;
		hQuest = (HANDLE)0x7fffffff;
		//hPeriod = (HANDLE)0x7fffffff;
		//hGuide = (HANDLE)0x7fffffff;
		//hMonster = (HANDLE)0x7fffffff;
	}
};

struct StopStateInfo
{
	DWORD tick;
	ObjectPosition pos;
};

struct MoveStateInfo
{
	DWORD tick;
	ObjectPosition start;
	ObjectPosition end;
};

struct RollStateInfo
{
	DWORD tick;
	ObjectPosition from;
	ObjectPosition direction;
};

typedef RollStateInfo RollOverStateInfo;

#pragma pack(pop)
