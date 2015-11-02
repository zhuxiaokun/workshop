#pragma once

#include "KPacket_Segment_Define.h"
#include <KCommonStruct.h>

#pragma warning(disable:4200)

enum SD_PROTOCOL
{
	s_nSD_PacketStart = s_nSD_Begin,
	s_nSD_Connected = s_nSD_PacketStart,
	s_nSD_ConnClosed,

	s_nSD_PlayerIn,
	s_nSD_PlayerOut,
	s_nSD_PlayerEmergencyOut,

	// 带状态副本
	s_nSD_LoadStateWorld,
	s_nSD_UnloadStateWorld,

	s_nSD_PacketEnd
};

enum DS_PROTOCOL
{
	s_nDS_PacketStart = s_nDS_Begin,
	s_nDS_Connected = s_nDS_PacketStart,
	s_nDS_ConnClosed,

	s_nDS_PlayerIn_Rsp,
	s_nDS_PlayerOut_Rsp,

	// 带状态副本
	s_nDS_LoadStateWorld_Rsp,
	s_nDS_UnloadStateWorld_Rsp,

	s_nDS_PacketEnd
};

#pragma pack(push,1)

struct SD_PlayerIn
{
	enum { s_nCode = s_nSD_PlayerIn };
	st_PassportKey passport;
	DWORD playerID;
	int mapID;
};
struct SD_PlayerOut
{
	enum { s_nCode = s_nSD_PlayerOut };
	DWORD playerID;
	int mapID;
};
struct SD_PlayerEmergencyOut
{
	enum { s_nCode = s_nSD_PlayerEmergencyOut };
	st_PassportKey m_passportKey;
	DWORD playerid;
	DWORD mapID;
};
struct SD_LoadStateWorld
{
	enum { s_nCode = s_nSD_LoadStateWorld };
	DWORD worldID;
	INT64 stateID;
};

struct SD_UnloadStateWorld
{
	enum { s_nCode = s_nSD_UnloadStateWorld };
	DWORD worldID;
	INT64 stateID;
};

struct DS_PlayerIn_Rsp
{
	enum { s_nCode = s_nDS_PlayerIn_Rsp };
	int errCode;
	st_PassportKey passport;
	DWORD playerID;
	PlayerDataHandlers handlers;
};

struct DS_PlayerOut_Rsp
{
	enum { s_nCode = s_nDS_PlayerOut_Rsp };
	DWORD playerID;
};

struct DS_LoadStateWorld_Rsp
{
	enum { s_nCode = s_nDS_LoadStateWorld_Rsp };
	DWORD worldID;
	HANDLE recHandle;
};

struct DS_UnloadStateWorld_Rsp
{
	enum { s_nCode = s_nDS_UnloadStateWorld_Rsp };
	DWORD worldID;
};

#pragma pack(pop)
