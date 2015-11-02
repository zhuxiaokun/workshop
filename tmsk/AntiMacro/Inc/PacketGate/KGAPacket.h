#pragma once

#include "KPacket_Segment_Define.h"
#pragma warning(disable:4200)

enum SA_PROTOCOL
{
	s_nSA_PacketStart = s_nSA_Begin,
	s_nSA_Connected = s_nSA_PacketStart,	// ��GameServer��������
	s_nSA_ConnClosed,						// ��GameServer�Ͽ�����

	s_nSA_SyncWorldToAI,
	s_nSA_DestroyWorld,						// GameServer֪ͨAI�ر�һ������

	s_nSA_PacketEnd,
};

enum AS_PROTOCOL
{
	s_nAS_PacketStart = s_nAS_Begin,
	s_nAS_Connected = s_nAS_PacketStart,
	s_nAS_ConnClosed,

	s_nAS_Login,
	s_nAS_DestroyWorld_Rsp,
	
	s_nAS_PacketEnd,
};

#pragma pack(push,1)

struct SA_SyncWorldToAI
{
	enum {s_nCode = s_nSA_SyncWorldToAI };
	DWORD worldID;
};

struct SA_DestroyWorld
{
	enum { s_nCode = s_nSA_DestroyWorld };
	DWORD worldID;
	int reason;
};

struct AS_Login
{
	enum { s_nCode = s_nAS_Login };	
};

struct AS_DestroyWorld_Rsp
{
	enum { s_nCode = s_nAS_DestroyWorld_Rsp };
	DWORD worldID;
	int result;
};

#pragma pack(pop)
