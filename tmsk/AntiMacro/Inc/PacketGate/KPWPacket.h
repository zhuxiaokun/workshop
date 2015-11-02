#pragma once

#include "../KCommonDefine.h"
#include "../KCommonStruct.h"
#include "KPacket_Segment_Define.h"

enum	// PassportServer -> WorldServer
{
	s_nPW_PacketStart = s_nPW_Begin,
	s_nPW_Connected = s_nPW_PacketStart,	// 当PassportServer连接上以后通知内部的消息，游戏开发商使用自己的方式，可以无视此包
	s_nPW_ConnClosed,						// 当PassportServer连接断开以后通知内部的消息，游戏开发商使用自己的方式，可以无视此包

	s_nPW_GameStart_Rsp,					// 检查OneTimePassword,通知开始游戏（玩家在于CharacterServer相连时机做的操作）
	s_nPW_GameEnd_Rsp,						// 通知结束游戏，包括踢人也用这个包
	s_nPW_KickPlayer_Req,					// 通知踢人

	s_nPW_UserTransfer_Req,					// 玩家充值
	s_nPW_WallowNotify_Req,					// 防沉迷通知

	s_nPW_GamePlaying_Rsp,					// 玩家进入GameServer

	s_nPW_PacketEnd
};

enum	// WorldServer -> PassportServer
{
	s_nWP_PacketStart = s_nWP_Begin,
	s_nWP_Connected = s_nWP_PacketStart,	// 当游戏开发商LoginServer连接上以后通知内部的消息
	s_nWP_ConnClosed,						// 当游戏开发商LoginServer连接断开后通知内部的消息

	s_nWP_ServerLogin,

	s_nWP_GameStart_Req,					// 开始游戏
	s_nWP_GameEnd_Req,						// 结束游戏

	s_nWP_UserTransfer_Rsp,					// 玩家充值

	s_nWP_GamePlaying_Req,					// 玩家进入GameServer
	s_nWP_GameNotPlaying_Req,				// 玩家离开GameServer

	s_nWP_PacketEnd,
};


#pragma pack(push,1)
struct WP_GameStart_Req
{
	enum { s_nCode = s_nWP_GameStart_Req };
	st_PassportKey m_passportKey;
	DWORD  m_OtpCode;
	int    m_charServerID;
	DWORD  m_dwIP;
	DWORD  m_dwCharacterServerIP;
};

struct PW_GameStart_Rsp 
{
	enum { s_nCode = s_nPW_GameStart_Rsp,	};
	enum
	{
		enum_GameStart_Success,
		enum_PassportID_Error,
		enum_GameStart_OtpError,		
	};
	BYTE  m_byResult;
	st_PassportKey m_passportKey;
	DWORD m_OtpCode;
	int m_charServerID;
	int m_nMasterLevel;
	DWORD m_dwIP;
	INT m_nVipLevel;
	CHAR m_achSecretKey[MAX_MD5_STRING];
};

struct WP_GameEnd_Req 
{
	enum { s_nCode = s_nWP_GameEnd_Req,	};
	st_PassportKey m_passportKey;
	DWORD m_nOnlineSeconds;
	DWORD m_WallowSec;
};

struct PW_GameEnd_Rsp 
{
	enum { s_nCode = s_nPW_GameStart_Rsp,	};
	enum { enum_GameEnd_Success, enum_GameEnd_Failed };
	BYTE m_Result;
	st_PassportKey m_passportKey;
};


struct PW_KickPlayer_Req 
{
	enum { s_nCode = s_nPW_KickPlayer_Req,	};
	st_PassportKey m_passportKey;
};

struct PW_WallowNotify_Req 
{
	enum { s_nCode = s_nPW_WallowNotify_Req,};
	st_PassportKey m_passportKey;
	BYTE m_wallowType;
};

//Sdo用户充值
struct PW_UserTransfer_Req
{
	enum { s_nCode = s_nPW_UserTransfer_Req,};
	st_PassportKey m_passportKey;
	DWORD  m_nValue;
	BYTE  m_byCashType;
	char m_strOrderID[SDO_ORDER_BUFF_SIZE]; //KString<SDO_ORDER_BUFF_SIZE>;
};

struct WP_UserTransfer_Rsp 
{
	enum { s_nCode = s_nWP_UserTransfer_Rsp,	};
	enum { eUserTransfer_Success, eUserTransfer_Error };
	BYTE  m_byResult;
	st_PassportKey m_passportKey;
	DWORD  m_nValue;
	BYTE  m_byCashType;
	DWORD  m_dwGuild;
	char m_strOrderID[SDO_ORDER_BUFF_SIZE]; //KString<SDO_ORDER_BUFF_SIZE>;
};

struct WP_GamePlaying_Req
{
	enum { s_nCode = s_nWP_GamePlaying_Req,	};
	st_PassportKey m_passportKey;
};

struct PW_GamePlaying_Rsp
{
	enum { s_nCode = s_nPW_GamePlaying_Rsp,};
	enum
	{
		eGamePlaying_Success_Rsp,
		eGamePlaying_NotFind_Rsp,
		eGamePlaying_NotAdult_Rsp,
		eGamePlaying_NotAdultApply_Rsp,
	};
	BYTE m_byResult;
	st_PassportKey m_passportKey;
};

struct WP_GameNotPlaying_Req
{
	enum { s_nCode = s_nWP_GameNotPlaying_Req,	};
	st_PassportKey m_passportKey;
};

#pragma pack(pop)

