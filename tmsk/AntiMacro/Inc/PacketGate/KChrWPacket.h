#ifndef _K_CHRW_PACKET_H_
#define _K_CHRW_PACKET_H_

#include "../KCommonStruct.h"
#include "KPacket_Segment_Define.h"

#pragma warning(disable:4200)

enum
{
	s_nChW_PacketStart = s_nChrW_Begin,
	s_nChW_Start = s_nChW_PacketStart,
	s_nChW_Connected = s_nChW_Start,
	s_nChW_ConnClosed,

	s_nChW_Login,				// character server login

	s_nChW_AcctLogin,			// 临时测试用

	s_nChW_Account_OTP_Check,
	s_nChW_Character_EnterGame_Check,
	s_nChW_Character_Logout,
	s_nChW_KickPlayer_Rsp,		// 踢人结果，只有不成功才发

	s_nChW_SelectServer,		// 玩家选线通知
	s_nChW_CancelSelect,		// 玩家取消选线通知

	s_nChW_SelectNormalLineServer,
	s_nChW_CancelNormalLineSelect,

	//s_nChW_NationalityInfo_Req,
	//s_nChW_Guild_Character_Delete,	// 删除一个帮众
	//s_nChW_CreatePlayer_Notice,		// 玩家创建一个角色通知
	//s_nChW_DeletePlayer_Notice,		// 玩家删除一个角色通知
	//s_nChW_UpdatePlayerName_Notice,	// 通知修改玩家名字

	s_nChW_PacketEnd,
};

enum
{
	s_nWCh_PacketStart = s_nWChr_Begin,
	s_nWCh_Connected = s_nWCh_PacketStart,
	s_nWCh_ConnError,
	s_nWCh_ConnClosed,

	s_nWCh_AcctLogin_Rsp,			// 临时测试用

	s_nWCh_Account_OTP_Check_Rsp,
	s_nWCh_Character_EnterGame_Check_Rsp,
	s_nWCh_Character_EnterNoviceVillage_Check_Rsp,
	s_nWCh_KickPlayer,
	s_nWCh_Character_EnterNormalLine_Check_Rsp,
	
	//s_nWCh_NationalityInfo_Rsp,
	//s_nWCh_UpdatePlayerInfo,
	//s_nWCh_PlayerEnterWorld,
	//s_nWCh_PlayerLeaveWorld,

	s_nWCh_PacketEnd,
};

#pragma pack(push,1)
//--------------------------------------------------------
struct ChW_Login
{
	enum { s_nCode = s_nChW_Login };
	int serverID;
};
struct ChW_AcctLogin
{
	enum { s_nCode = s_nChW_AcctLogin };
	char acctName[20];
	char password[20];
};
struct ChW_Account_OTP_Check
{
	enum { s_nCode = s_nChW_Account_OTP_Check };
	st_PassportKey passport;
	DWORD otp;
};
struct ChW_Character_EnterGame_Check
{
	enum { s_nCode = s_nChW_Character_EnterGame_Check };
	st_PassportKey passport;
	int		line,mapID,x,y,z;
	DWORD	playerID;
	char	nickName[MAX_NAME_DEF];
	BYTE	firstLogin;
};
struct ChW_Character_Logout
{
	enum { s_nCode = s_nChW_Character_Logout };	
	st_PassportKey passport;
};
struct ChW_KickPlayer_Rsp
{
	enum { s_nCode = s_nChW_KickPlayer_Rsp };
	enum { enum_fail, enum_success         };
	st_PassportKey passport;
	DWORD playerID;
	BYTE result;
};
struct ChW_SelectServer // 玩家选线通知
{
	enum { s_nCode = s_nChW_SelectServer };
	DWORD playerID;
	int mapID;
	DWORD worldID;
};
struct ChW_CancelSelect // 玩家取消选线通知
{
	enum { s_nCode = s_nChW_CancelSelect };
	DWORD playerID;
};

struct ChW_SelectNormalLineServer
{
	enum { s_nCode = s_nChW_SelectNormalLineServer };
	DWORD playerID;
	int mapID;
	DWORD worldID;
};
// 玩家取消选线通知
struct ChW_CancelNormalLineSelect
{
	enum { s_nCode = s_nChW_CancelNormalLineSelect };
	DWORD playerID;
};
//--------------------------------------------------------

struct WCh_AcctLogin_Rsp
{
	enum { s_nCode = s_nWCh_AcctLogin_Rsp };
	DWORD errCode;
	char acctName[20];
	st_PassportKey passport;
	DWORD otpCode;
};
struct WCh_Account_OTP_Check_Rsp
{
	enum { s_nCode = s_nWCh_Account_OTP_Check_Rsp };
	enum { success, invalidOtp, alreadyInGame };
	st_PassportKey passport;
	BYTE result;
};

//分线,新手村信息
struct WCh_Character_EnterNoviceVillage_Check_Rsp
{
	enum { s_nCode = s_nWCh_Character_EnterNoviceVillage_Check_Rsp };
	struct tagNoviceVillageInfo
	{
		int     gameServerID;
		char	name[16];
		int		mapID;
		DWORD	worldID;
		DWORD	serverIp;
		WORD	serverPort;
		DWORD	playerCount;
	};
	st_PassportKey passport;
	tagNoviceVillageInfo sNVI[0];
};

//大世界分线
struct WCh_Character_EnterNormalLine_Check_Rsp
{
	enum { s_nCode = s_nWCh_Character_EnterNormalLine_Check_Rsp };
	struct tagNormalLineInfo
	{
		int     gameServerID;
		char	name[16];
		int		mapID;
		DWORD	worldID;
		DWORD	serverIp;
		WORD	serverPort;
		DWORD	playerCount;
	};
	st_PassportKey passport;
	tagNormalLineInfo sNVL[0];
};

struct WCh_Character_EnterGame_Check_Rsp
{
	enum { s_nCode = s_nWCh_Character_EnterGame_Check_Rsp };
	enum { success, fail, serverUnavail };
	st_PassportKey passport;
	BYTE	result;
	int		mapID;
	DWORD	worldID;
	float	fx, fy;
	DWORD	serverIp;
	WORD	serverPort;
};

//struct WCh_NationalityInfo_Rsp
//{
//	enum	{ s_nCode = s_nWCh_NationalityInfo_Rsp };
//	DWORD	m_dwWaterNum;
//	DWORD	m_dwSnowNum;
//	DWORD	m_dwHillNum;
//	BYTE	m_byWeakNationID;
//};

//struct WCh_UpdatePlayerInfo
//{
//	enum	{ s_nCode = s_nWCh_UpdatePlayerInfo, };
//	DWORD	m_dwPlayerID;
//	DWORD	m_dwMapID;
//	INT		m_nX;
//	INT		m_nY;
//	INT		m_nZ;
//	INT		m_RDirection;
//};

struct WCh_KickPlayer
{
	enum { s_nCode = s_nWCh_KickPlayer };
	st_PassportKey passport;
	DWORD playerID;
};
//
//struct WCh_PlayerEnterWorld 
//{
//	enum { s_nCode = s_nWCh_PlayerEnterWorld };
//	st_PassportKey passport;
//};
//
//struct WCh_PlayerLeaveWorld 
//{
//	enum { s_nCode = s_nWCh_PlayerLeaveWorld };
//	st_PassportKey passport;
//	int m_VacancyCount; // 当前空闲多少个位置
//};
//----------------------------------------------------------
#pragma pack(pop)
#endif
