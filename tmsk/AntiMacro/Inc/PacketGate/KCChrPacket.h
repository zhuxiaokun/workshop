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

	s_nCCh_AcctLogin, // ��ʱ�����ã��ͻ���ֱ�ӷ���character server���е�¼

	s_nCCh_OTPCode,	// ����Otp��֤

	// ���ִ�ѡ��
	s_nCCh_SelectServer,
	s_nCCh_CancelSelect,

	// ������ѡ��
	s_nCCh_SelectNormalLineServer,
	s_nCCh_CancelNormalLineSelect,

	s_nCCh_CreateChar,						// �����½���ɫ
	s_nCCh_DeleteChar,						// ����ɾ����ɫ
	s_nCCh_Enter,							// �����ɫ��½����
	s_nCCh_CheckName,						// �������Ƽ��
	s_nCCh_CancelCreate,					// ȡ�������������

	s_nCCh_PacketEnd,
};

enum
{
	s_nChC_PacketStart = s_nChrC_Begin,
	s_nChC_Connected = s_nChC_PacketStart,
	s_nChC_ConnError,
	s_nChC_ConnClosed,

	s_nChC_AcctLogin_Rsp,	// ��ʱ������

	s_nChC_CharCount,			// ��ɫ����
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

// ��ʱ������
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

// ѡ����·
struct CCh_SelectNormalLineServer
{
	enum { s_nCode = s_nCCh_SelectNormalLineServer };
	int serverIndex;
};

// ȡ��ѡ��
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

// ��ʱ������
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
	int		mapID;		// �ͻ����õ����MApID�����Ԥ���ص�ͼ
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
