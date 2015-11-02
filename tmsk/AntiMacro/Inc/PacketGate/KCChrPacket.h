#ifndef _K_CCHR_PACKET_H_
#define _K_CCHR_PACKET_H_

#include "KPacket_Segment_Define.h"
#include "../KCommonStruct.h"

#pragma warning(disable:4200)

enum
{
	s_nCCh_PacketStart = s_nCChr_Begin,
	s_nCCh_Connected = s_nCChr_Begin,
	s_nCCh_ConnError,
	s_nCCh_ConnClosed,

	s_nCCh_AcctLogin, // 临时测试用，客户端直接发给character server进行登录

	s_nCCh_OTPCode,	// 请求Otp验证

	// 新手村选线
	s_nCCh_SelectServer,
	s_nCCh_CancelSelect,

	// 大世界选线
	s_nCCh_SelectNormalLineServer,
	s_nCCh_CancelNormalLineSelect,

	s_nCCh_CreateChar,						// 请求新建角色
	s_nCCh_DeleteChar,						// 请求删除角色
	s_nCCh_Enter,							// 请求角色登陆世界
	s_nCCh_CheckName,						// 请求名称检查
	s_nCCh_CancelCreate,					// 取消创建人物操作

	s_nCCh_PacketEnd,
};

enum
{
	s_nChC_PacketStart = s_nChrC_Begin,
	s_nChC_Connected = s_nChC_PacketStart,
	s_nChC_ConnError,
	s_nChC_ConnClosed,

	s_nChC_AcctLogin_Rsp,	// 临时测试用

	s_nChC_CharCount,			// 角色个数
	s_nChC_CharacterInfo,
	s_nChC_CreateChar_Rsp,
	s_nChC_DeleteChar_Rsp,
	s_nChC_CheckName_Rsp,
	s_nChC_Character_EnterRsp,
	s_nChC_Character_EnterNoviceVillage_Rsp,
	s_nChC_Character_EnterNormal_Rsp,

	s_nChC_Message,

	s_nChC_PacketEnd,
};

#pragma pack(push,1)
//------------------------------------------------------

// 临时测试用
struct CCh_AcctLogin
{
	enum { s_nCode = s_nCCh_AcctLogin };
	char acctName[20];
	char password[20];
};

struct CCh_OTP_Code
{
	enum { s_nCode = s_nCCh_OTPCode };
	st_PassportKey passport;
	DWORD otp;
};

struct CCh_SelectServer
{
	enum { s_nCode = s_nCCh_SelectServer };
	int serverIndex;
};

struct CCh_CancelSelect
{
	enum { s_nCode = s_nCCh_CancelSelect };
};

// 选择线路
struct CCh_SelectNormalLineServer
{
	enum { s_nCode = s_nCCh_SelectNormalLineServer };
	int serverIndex;
};

// 取消选线
struct CCh_CancelNormalLineSelect
{
	enum { s_nCode = s_nCCh_CancelNormalLineSelect };
};

struct CCh_CreateChar
{
	enum { s_nCode = s_nCCh_CreateChar };
	// other character info
	char name[32];
	BYTE charIndex;
	BYTE sex;
	BYTE Class;
};

struct CCh_DeleteChar
{
	enum { s_nCode = s_nCCh_DeleteChar};
	int charIndex;
};

struct CCh_Enter
{
	enum { s_nCode = s_nCCh_Enter };
	int charIndex;
};

struct CCh_CheckName
{
	enum { s_nCode = s_nCCh_CheckName };
	char name[0];
};

struct CCh_CancelCreate
{
	enum { s_nCode = s_nCCh_CancelCreate };
};

// 临时测试用
struct ChC_AcctLogin_Rsp
{
	enum { s_nCode = s_nChC_AcctLogin_Rsp };
	DWORD errCode;
	st_PassportKey passport;
	DWORD otpCode;
};

struct ChC_CharCount
{
	enum { s_nCode = s_nChC_CharCount };
	int errCode;
	BYTE count;
};

struct ChC_CharacterInfo
{
	enum { s_nCode = s_nChC_CharacterInfo };
	enum { success, notExist, nullName, equipError, dbError };
	BYTE charIndex;
	STRU_CHARACTER_INFO mstruCharacterInfo;
};

struct ChC_CreateChar_Rsp
{
	enum { s_nCode = s_nChC_CreateChar_Rsp };
	int errCode;
	BYTE charIndex;
	STRU_CHARACTER_INFO mstruCharacterInfo;
};

struct ChC_DeleteChar_Rsp
{
	enum { s_nCode = s_nChC_DeleteChar_Rsp };
	int errCode;
};

struct ChC_CheckName_Rsp
{
	enum { s_nCode = s_nChC_CheckName_Rsp };
	enum { success, name_length, name_exists, db_error, illegal_char, };
	int errCode;
};

struct ChC_Message
{
	enum { s_nCode = s_nChC_Message };
	char text[0];
};

struct ChC_Character_EnterRsp
{
	enum { s_nCode = s_nChC_Character_EnterRsp };
	DWORD	playerID;
	int		mapID;		// 客户端拿到这个MApID后可以预加载地图
	DWORD	worldID;
	float   fx,fy;
	DWORD   serverIp;
	WORD    serverPort;
	BYTE	firstLogin;
};

struct ChC_Character_EnterNoviceVillage_Rsp
{
	enum { s_nCode = s_nChC_Character_EnterNoviceVillage_Rsp };
	struct tagNoviceVillageInfo
	{
		CHAR	name[16];
		WORD	playerCount;
	};
	DWORD	playerID;
	BYTE	firstLogin;
	WORD	count;
	tagNoviceVillageInfo sNVI[0];
};

struct ChC_Character_EnterNormal_Rsp
{
	enum { s_nCode = s_nChC_Character_EnterNormal_Rsp };
	struct tagNormalInfo
	{
		CHAR	name[16];
		WORD	playerCount;
	};
	DWORD	playerID;
	BYTE	firstLogin;
	WORD	count;
	tagNormalInfo sNVI[0];
};

//------------------------------------------------------
#pragma pack(pop)
#endif
