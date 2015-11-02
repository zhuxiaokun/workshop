#pragma once

#include "KGCPacket.h"
#include "../KCommonStruct.h"
#include "KPacket_Segment_Define.h"

#pragma warning(disable:4200)

enum SW_PROTOCOL
{
	s_nSW_PacketStart = s_nSW_Begin,

	s_nSW_Connected = s_nSW_PacketStart,
	s_nSW_ConnClosed,
	s_nSW_Login,				// game server login

	s_nSW_CloseServer,
	s_nSW_OpenServer,

	s_nSW_CreateWorld_Rsp,		// 动态创建，销毁副本，副本管理
	s_nSW_DestroyWorld_Rsp,
	s_nSW_DestroyWorld_Req,
	s_nSW_DestroyWorld_Ack,
	s_nSW_WorldStateVersion,
	s_nSW_WorldStateExpire,

	s_nSW_ActivityNotice,		// GameServer通知WorldServer活动相关信息，扩展用

	s_nSW_GlobalSession_CreateRecord,	// 全局Session
	s_nSW_GlobalSession_LoadRecord,
	s_nSW_GlobalSession_UnloadRecord,
	s_nSW_GlobalSession_RemoveRecord,
	s_nSW_GlobalSession_SetData,
	s_nSW_GlobalSession_RemoveData,
	s_nSW_GlobalSession_ClearData,

	s_nSW_OTPCheck,				// game server ask otp check
	s_nSW_PlayerLogin,			// player login
	s_nSW_PlayerFirstLoginToday,// player first at one day
	s_nSW_PlayerLogout,			// player logout
	s_nSW_PlayerChangeScene,	// player ask change map
	s_nSW_BackToCharacter,		// 返回人物界面
	s_nSW_KickPlayerByPlayerID,	// 通过ID踢玩家
	
	s_nSW_OnPlayerLevelup,		// 玩家升级通知
	s_nSW_PlayerVillageChange,

	s_nSW_PacketEnd
};

enum WS_PROTOCOL
{
	s_nWS_PacketStart = s_nWS_Begin,
	s_nWS_Connected = s_nWS_PacketStart,// 和WorldServer建立连接成功
	s_nWS_ConnClosed,

	s_nWS_OpenServer,
	s_nWS_ShutdownServer,
	
	s_nWS_SyncWorldsInfoToGame,
	s_nWS_CreateWorld,			// 副本管理
	s_nWS_DestroyWorld,
	s_nWS_DestroyWorld_Ack,
	s_nWS_DestroyingWorldIdle,	// 关闭中的副本进入Idle状态了

	s_nWS_SyncActivityState,	// 活动相关 (activity about)
	s_nWS_StartActivity,
	s_nWS_StopActivity,
	s_nWS_ActivityInvite,
	s_nWS_ActivityNotice, // 各种通知，扩展用

	s_nWS_OTPCheck_Rsp,
	s_nWS_PlayerChangeSceneResponse,
	s_nWS_BackToCharacter_Rsp,		// 返回人物界面
	s_nWS_KickPlayerByPlayerID,		// 通过玩家ID踢下线
	s_nWS_TeleportTo,				// World强制让玩家传送到某处

	s_nWS_BroardCastToAllPlayer,	// 广播一块内存到所有客户端
	s_nWS_ForwardToPlayer,			// 广播一块内存到指定客户端
	
	s_nWS_GlobalSession_AddData,	// Global Session
	s_nWS_GlobalSession_ChangeData,
	s_nWS_GlobalSession_RemoveData,
	s_nWS_GlobalSession_SyncRecord,
	s_nWS_GlobalSession_RemoveRecord,
	s_nWS_GlobalSession_ClearData,

	s_nWS_PacketEnd
};

#pragma pack(push,1)
struct SW_OTPCheck
{
	enum	{ s_nCode = s_nSW_OTPCheck };
	st_PassportKey m_passportKey;
	DWORD	m_dwOTPCode;
	DWORD	m_dwMainVersion;
	DWORD	m_dwSubVersion;
};

struct SW_Login
{
	enum	{ s_nCode = s_nSW_Login };	
	DWORD	m_dwGameServerID;
	CHAR	m_strServerIP[16];
	DWORD	m_dwServerPort;
};

struct SW_FirstLoginToday
{
	enum { s_nCode = s_nSW_PlayerFirstLoginToday };
	DWORD m_dwPlayerID;
};

struct SW_PlayerLogin
{
	enum { s_nCode = s_nSW_PlayerLogin, };	
	st_PassportKey m_passportKey;
	DWORD	m_dwPlayerID;
	WORD	m_Level;
	DWORD	m_NationID;
	DWORD	m_dwMapID;
	DWORD	m_dwDuplicateID;
	DWORD	m_GuildID;
	BYTE	m_Sex;
	BYTE	m_Class;
	BYTE	m_SubClass;
	FLOAT	m_equipMark;
	DWORD	m_dwLastLevelUpTime;
	DWORD	m_dwExp;
};

struct SW_PlayerLogout
{
	enum { s_nCode = s_nSW_PlayerLogout, };	
	DWORD m_dwID;
	FLOAT m_fX;
	FLOAT m_fY;
};

struct SW_CloseServer 
{
	enum { s_nCode = s_nSW_CloseServer, };
	INT	m_LeftTime;
};
struct SW_OpenServer 
{
	enum { s_nCode = s_nSW_OpenServer, };
	INT	m_LeftTime;
};

struct SW_PlayerChangeScene		// 玩家切换地图
{
	enum {	s_nCode = s_nSW_PlayerChangeScene };
	DWORD playerID;
	DWORD mapID;
	DWORD worldID;
	float x, y, z;
	// stream session/s
};

struct WS_PlayerChangeSceneResponse		// 玩家切换地图结果
{
	enum {	s_nCode = s_nWS_PlayerChangeSceneResponse };
	enum
	{
		enum_success,
		enum_state_refuse,
		enum_target_not_found,
		enum_no_available,
	};

	BYTE    m_Result;
	int     m_msgID;
	DWORD	m_dwPlayerID;
	DWORD	m_dwMapID;
	float   fx,fy;
	DWORD	m_dwDuplicateID;
	CHAR	m_strGameServerIP[16];
	DWORD	m_dwGameServerPort;
};

struct WS_OTPCheck_Rsp
{
	enum { s_nCode = s_nWS_OTPCheck_Rsp };
	enum { SUCCESS, FAIL_LOGINED };

	BYTE		m_byResult;
	st_PassportKey m_passportKey;
	DWORD		m_dwPlayerID;
	char        m_charName[MAX_NAME];
	DWORD		m_dwMapID;
	DWORD		m_dwDuplicateID;
	DWORD		m_pointID;
	DWORD		m_dwLineID;
	INT			m_nSide;
	INT			m_nMasterLevel;
	INT			m_nVipLevel;
	DWORD		m_dwTeamID;
	BOOL		m_bIsLogin;

	// stream sessions
	// ...
};

struct WS_KickAllPlayer 
{
	enum { s_nCode = s_nWS_ShutdownServer };
};

struct WS_OpenOrCloseServer 
{
	enum { s_nCode = s_nWS_OpenServer };
	BOOL m_OpenOrClose;
};

struct WS_SyncWorldsInfoToGame
{
	enum { s_nCode = s_nWS_SyncWorldsInfoToGame };
	struct World
	{
		short line;
		short worldType;
		short baseIdx;
		short count;
	};
	World worlds[0];
};

struct SW_KickPlayerByPlayerID 
{
	enum { s_nCode = s_nSW_KickPlayerByPlayerID };
	st_PassportKey m_passportKey;
	DWORD	dwPlayerID;
	BYTE	byResult;
};

struct WS_KickPlayerByPlayerID
{
	enum { s_nCode = s_nWS_KickPlayerByPlayerID };
	st_PassportKey m_passportKey;
	DWORD	dwPlayerID;
	DWORD	dwDelaySec;
};

struct WS_TeleportTo 
{
	enum	{ s_nCode = s_nWS_TeleportTo };
	DWORD	dwPlayerID;
	int		mapID;
	int		fX;
	int		fY;
	int		fZ;
	int		WorldID;
};

struct SW_OnPlayerLevelup 
{
	enum	{ s_nCode = s_nSW_OnPlayerLevelup };
	DWORD	dwPlayerID;
	WORD	wLevel;
};

struct SW_PlayerVillageChange 
{
	enum {s_nCode = s_nSW_PlayerVillageChange };
	DWORD dwPlayerID;
	INT   nVillageID;
};

struct SW_BackToCharacter
{
	enum { s_nCode = s_nSW_BackToCharacter };
	DWORD playerID;
};

struct WS_BackToCharacter_Rsp
{
	enum { s_nCode = s_nWS_BackToCharacter_Rsp };
	enum {Success, StateRefuse };
	DWORD playerID;
	BYTE result;
};

struct WS_BroardCastToAllPlayer
{
	enum { s_nCode = s_nWS_BroardCastToAllPlayer };
	int scCode;
	char data[0];
};

struct WS_ForwardToPlayer
{
	enum { s_nCode = s_nWS_ForwardToPlayer };
	DWORD playerId;
	int scCode;
	char data[0];
};

struct WS_GlobalSession_AddData
{
	enum { s_nCode = s_nWS_GlobalSession_AddData };
	WORD sid;
	WORD key, len;
	char val[0];
};
struct WS_GlobalSession_ChangeData
{
	enum { s_nCode = s_nWS_GlobalSession_ChangeData };
	WORD sid;
	WORD key, len;
	char newVal[0];
};
struct WS_GlobalSession_RemoveData
{
	enum { s_nCode = s_nWS_GlobalSession_RemoveData };
	WORD sid;
	WORD key;
};

struct WS_GlobalSession_SyncRecord
{
	enum { s_nCode = s_nWS_GlobalSession_SyncRecord };
	WORD sid;
	// string of name
	// short of binary data length
	// binary of session data
};
struct WS_GlobalSession_RemoveRecord
{
	enum { s_nCode = s_nWS_GlobalSession_RemoveRecord };
	WORD sid;
};

struct SW_GlobalSession_CreateRecord
{
	enum { s_nCode = s_nSW_GlobalSession_CreateRecord };
	WORD sid;
	char name[64];
};

struct SW_GlobalSession_LoadRecord
{
	enum { s_nCode = s_nSW_GlobalSession_LoadRecord };
	WORD sid;
};

struct SW_GlobalSession_UnloadRecord
{
	enum { s_nCode = s_nSW_GlobalSession_UnloadRecord };
	WORD sid;
};

struct SW_GlobalSession_RemoveRecord
{
	enum { s_nCode = s_nSW_GlobalSession_RemoveRecord };
	WORD sid;
};

struct SW_GlobalSession_SetData
{
	enum { s_nCode = s_nSW_GlobalSession_SetData };
	WORD sid;
	WORD key;
	WORD len;
	// sequence of val
};

struct SW_GlobalSession_RemoveData
{
	enum { s_nCode = s_nSW_GlobalSession_RemoveData };
	WORD sid;
	WORD key;
};

struct SW_GlobalSession_ClearData
{
	enum { s_nCode = s_nSW_GlobalSession_ClearData };
	WORD sid;
};

struct WS_GlobalSession_ClearData
{
	enum { s_nCode = s_nWS_GlobalSession_ClearData };
	WORD sid;
};

struct WS_CreateWorld
{
	enum { s_nCode = s_nWS_CreateWorld };
	DWORD worldID;
	INT64 stateID; // 副本状态记录
};

struct WS_DestroyWorld
{
	enum { s_nCode = s_nWS_DestroyWorld };
	DWORD worldID;
};

struct WS_DestroyWorld_Ack
{
	enum { s_nCode = s_nWS_DestroyWorld_Ack };
	DWORD worldID;
};
struct WS_DestroyingWorldIdle
{
	enum { s_nCode = s_nWS_DestroyingWorldIdle };
	DWORD worldID;
};

struct SW_CreateWorld_Rsp
{
	enum { s_nCode = s_nSW_CreateWorld_Rsp };
	enum // error codes
	{
		err_success,
		err_memory_out,
		err_init_world,
		err_create_monitor,
		err_already_exist,
		err_dbagent_unavail,
		err_load_state,
	};
	int errCode;
	DWORD worldID;
	INT64 stateID;
	TIME_T expireTime;
	short stateVer;
};

struct SW_DestroyWorld_Rsp
{
	enum { s_nCode = s_nSW_DestroyWorld_Rsp };
	enum { err_success, err_no_world };
	DWORD worldID;
	int errCode;
};

struct SW_DestroyWorld_Req
{
	enum { s_nCode = s_nSW_DestroyWorld_Req };
	enum { unknown,finished,create,error,worldServerDown };
	DWORD worldID;
	int reason;
};

struct SW_DestroyWorld_Ack
{
	enum { s_nCode = s_nSW_DestroyWorld_Ack };
	DWORD worldID;
};

struct SW_WorldStateVersion
{
	enum { s_nCode = s_nSW_WorldStateVersion };
	DWORD worldID;
	INT64 stateID;
	short version;
};

struct SW_WorldStateExpire
{
	enum { s_nCode = s_nSW_WorldStateExpire };
	DWORD  worldID;
	INT64  stateID;
	TIME_T expire;
};

struct SW_ActivityNotice
{
	enum { s_nCode = s_nSW_ActivityNotice };
	int activityID;
	int noticeType;
	// stream session data
};

struct WS_SyncActivityState
{
	enum { s_nCode = s_nWS_SyncActivityState };
	int activityID;
	int state;
	TIME_T beginTime,endTime;
};

struct WS_StartActivity
{
	enum { s_nCode = s_nWS_StartActivity };
	int activityID;
	TIME_T beginTime,endTime;
};

struct WS_StopActivity
{
	enum { s_nCode = s_nWS_StopActivity };
	int activityID;
};

struct WS_ActivityInvite
{
	enum { s_nCode = s_nWS_ActivityInvite };
	int activityID;
	DWORD playerID;
};

struct WS_ActivityNotice
{
	enum { s_nCode = s_nWS_ActivityNotice };
	int activityID;
	int noticeType;
	// stream session data
};

#pragma pack(pop)

template <int x> void _export_SW_WS_code_to_lua()
{
	// WS
	export_enum_to_lua(s_nWS_ConnClosed);
	export_enum_to_lua(s_nWS_OTPCheck_Rsp);
	export_enum_to_lua(s_nWS_PlayerChangeSceneResponse);
	export_enum_to_lua(s_nWS_ShutdownServer);
	export_enum_to_lua(s_nWS_OpenServer);
	export_enum_to_lua(s_nWS_SyncWorldsInfoToGame);
	export_enum_to_lua(s_nWS_KickPlayerByPlayerID);
	export_enum_to_lua(s_nWS_TeleportTo);
	export_enum_to_lua(s_nWS_Connected);
	export_enum_to_lua(s_nWS_BackToCharacter_Rsp);
	export_enum_to_lua(s_nWS_BroardCastToAllPlayer);
	export_enum_to_lua(s_nWS_ForwardToPlayer);
	export_enum_to_lua(s_nWS_GlobalSession_AddData);
	export_enum_to_lua(s_nWS_GlobalSession_ChangeData);
	export_enum_to_lua(s_nWS_GlobalSession_RemoveData);
	export_enum_to_lua(s_nWS_GlobalSession_SyncRecord);
	export_enum_to_lua(s_nWS_GlobalSession_RemoveRecord);
	export_enum_to_lua(s_nWS_GlobalSession_ClearData);
	export_enum_to_lua(s_nWS_CreateWorld);
	export_enum_to_lua(s_nWS_DestroyWorld);
	export_enum_to_lua(s_nWS_DestroyWorld_Ack);
	export_enum_to_lua(s_nWS_DestroyingWorldIdle);
	export_enum_to_lua(s_nWS_SyncActivityState);
	export_enum_to_lua(s_nWS_StartActivity);
	export_enum_to_lua(s_nWS_StopActivity);
	export_enum_to_lua(s_nWS_ActivityInvite);
	export_enum_to_lua(s_nWS_ActivityNotice);

	// SW
	export_enum_to_lua(s_nSW_Connected);
	export_enum_to_lua(s_nSW_ConnClosed);
	export_enum_to_lua(s_nSW_OTPCheck);
	export_enum_to_lua(s_nSW_Login);
	export_enum_to_lua(s_nSW_PlayerLogin);
	export_enum_to_lua(s_nSW_PlayerLogout);
	export_enum_to_lua(s_nSW_CloseServer);
	export_enum_to_lua(s_nSW_OpenServer);
	export_enum_to_lua(s_nSW_PlayerChangeScene);
	export_enum_to_lua(s_nSW_KickPlayerByPlayerID);
	export_enum_to_lua(s_nSW_OnPlayerLevelup);
	export_enum_to_lua(s_nSW_PlayerVillageChange);
	export_enum_to_lua(s_nSW_BackToCharacter);
	export_enum_to_lua(s_nSW_GlobalSession_CreateRecord);
	export_enum_to_lua(s_nSW_GlobalSession_LoadRecord);
	export_enum_to_lua(s_nSW_GlobalSession_UnloadRecord);
	export_enum_to_lua(s_nSW_GlobalSession_RemoveRecord);
	export_enum_to_lua(s_nSW_GlobalSession_SetData);
	export_enum_to_lua(s_nSW_GlobalSession_RemoveData);
	export_enum_to_lua(s_nSW_GlobalSession_ClearData);
	export_enum_to_lua(s_nSW_CreateWorld_Rsp);
	export_enum_to_lua(s_nSW_DestroyWorld_Rsp);
	export_enum_to_lua(s_nSW_DestroyWorld_Req);
	export_enum_to_lua(s_nSW_DestroyWorld_Ack);
	export_enum_to_lua(s_nSW_WorldStateVersion);
	export_enum_to_lua(s_nSW_WorldStateExpire);
	export_enum_to_lua(s_nSW_ActivityNotice);
}
#define export_SW_WS_code_to_lua() _export_SW_WS_code_to_lua<0>()
