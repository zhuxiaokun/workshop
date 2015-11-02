#pragma once
#include <System/KType.h>
#include <System/KMacro.h>
#include "KPacket_Segment_Define.h"

enum
{
	s_nDW_PacketStart = s_nDW_Begin,
	s_nDW_Connected = s_nDW_PacketStart,
	s_nDW_ConnClosed,

	s_nDW_Login,
	s_nDW_PlayerLoaded,		// ��������Ѽ���
	s_nDW_PlayerUnloaded,	// ���������ж��

	s_nDW_PacketEnd
};

enum
{
	s_nWD_PacketStart = s_nWD_Begin,

	s_nWD_Connected = s_nWD_PacketStart,
	s_nWD_ConnClosed,

	s_nWD_PacketEnd
};

#pragma pack(push,1)

struct DW_Login
{
	enum { s_nCode = s_nDW_Login };
};

// ��������Ѽ���
struct DW_PlayerLoaded
{
	enum { s_nCode = s_nDW_PlayerLoaded };
	DWORD playerID;
	BYTE  success;
};

// ���������ж��
struct DW_PlayerUnloaded
{
	enum { s_nCode = s_nDW_PlayerUnloaded };
	DWORD playerID;
	BYTE  success;
};

#pragma pack(pop)
