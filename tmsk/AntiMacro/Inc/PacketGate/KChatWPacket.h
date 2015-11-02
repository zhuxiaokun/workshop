#pragma once

#include "KPacket_Segment_Define.h"
#include "../KCommonDefine.h"
#include "../KCommonStruct.h"

enum // ChatServer to WorldServer
{
	s_nChatWorld_PacketStart = s_nChatW_Begin,
	s_nChatWorld_Connected = s_nChatWorld_PacketStart,
	s_nChatWorld_ConnClosed,

	s_nChatWorld_ChatServerLogin,			//�����������½��
	s_nChatWorld_PlayerLoginRsq,
	
	s_nChatWorld_PacketEnd,
};


enum // WorldServer to ChatServer
{
	s_nWorldChat_PacketStart = s_nWChat_Begin,
	s_nWorldChat_Connected = s_nWorldChat_PacketStart,
	s_nWorldChat_Closed,

	s_nWorldChat_PlayerLoginRsp,
	s_nWorldChat_PlayerLogoutRsp,
	s_nWorldChat_PlayerJoinGroup,			//���������֪ͨChatServer�����Ϣ���
	s_nWorldChat_PlayerExitGroup,

	s_nWorldChat_Syncpacket,                // World Server ͨ�� Chat Server �㲥�����ͻ���

	s_nWorldChat_AttrChanged,				// ������Ա��
	s_nWorldChat_TeamPopedomChanged,		// ������Ȩ�ޱ��
	s_nWorldChat_GuildPopedomChanged,		// ��Ұ���Ȩ�ޱ��
	
	s_nWorldChat_PacketEnd,
};

//////////////////////////////////////////////////////////////////////////
/// WorldServer -> ChatServer
//////////////////////////////////////////////////////////////////////////

#pragma pack(push, 1)
struct ChatWorld_ChatServerLogin //: public KPacket
{
	enum	{ s_nCode = s_nChatWorld_ChatServerLogin, };
};

struct ChatWorld_PlayerLoginRsq
{
	enum	{ s_nCode = s_nChatWorld_PlayerLoginRsq };
	st_PassportKey m_passportKey;
	DWORD		m_dwOTPCode;
	DWORD		m_dwConnIndex;
};

struct WorldChat_PlayerLoginRsp
{
	enum { s_nCode = s_nWorldChat_PlayerLoginRsp };
	enum  { Check_Succeed, Check_Error };

	BYTE		m_byResult;
	st_PassportKey m_passportKey;
	DWORD		m_dwPlayerID;
	DWORD		m_dwConnIndex;
	CHAR		m_szPlayerName[MAX_NAME_DEF];
	BYTE		m_Level;
	BYTE		m_dwMasterLevel;
	BYTE		m_byTeamFunc;
	BYTE		m_byGuildFunc;
	DWORD		m_NationID;
	DWORD		m_TeamID;
	DWORD		m_GuildID;
	DWORD		m_Faculty;
	BYTE		m_byClass;		// ��ְҵ
	BYTE		m_bySubClass;	// ְҵ��֧
};

struct WorldChat_PlayerLogoutRsp
{
	enum	{ s_nCode = s_nWorldChat_PlayerLogoutRsp };
	st_PassportKey m_passportKey;
	DWORD	m_dwPlayerID;
};

struct WorldChat_PlayerJoinGroup //: public KPacket
{
	enum	{ s_nCode = s_nWorldChat_PlayerJoinGroup };
	DWORD	dwPlayerID;			// ���ID
	DWORD	dwGroupID;			// ��ID
	DWORD	dwGroupType;		// ������
	DWORD	dwCustomID;			// 
};

struct WorldChat_PlayerExitGroup //: public KPacket
{
	enum	{ s_nCode = s_nWorldChat_PlayerExitGroup };
	DWORD	dwPlayerID;			// ���ID
	DWORD	dwGroupID;			// ��ID������ID,����ID��
	DWORD	dwGroupType;		// ������ ,��ӡ����ҵ�
};

struct WorldChat_SyncPacket
{
	enum { s_nCode = s_nWorldChat_Syncpacket };
	BYTE	channelID;
	DWORD	groupID;
	WORD	packetCmd;
	WORD	packetLength;
	// BYTE sequence of original packet
};

struct WorldChat_AttrChanged
{
	enum { s_nCode = s_nWorldChat_AttrChanged };
	enum WorldPlayerAttrKeyType
	{
		WPAKT_Level = 0,
		WPAKT_Class,
		WPAKT_SubClass,
		WPAKT_TeamPopedom,
		WPAKT_GuildPopedom,
		WPAKT_PostID,
	};
	int mPlayerID;
	int mAttrKey;
	int mAttrValue1;
	int mAttrValue2;
};

#pragma pack(pop)
