#pragma once
#include "KPacket_Segment_Define.h"

enum C2L_PROTOCOL
{
	C2L_Connect = s_nCL_Begin,
	C2L_ConnClosed,

	s_nCL_ConnError,
	s_nCL_LOGIN_REQ,				// 请求登陆
	//s_nCL_LOGOUT_REQ,				// 请求登出
	//s_nCL_LINK_START,				// 连接成功
	//s_nCL_LINK_OVER,				// 断开连接
	//s_nCL_WORLD_LINK_START,		// World连接成功
	s_nCL_PasswordProtect_Rsp,		// 密保结果回复
	s_nCL_PasswordProtect_Cancel,	// 取消密保输入
	s_nCL_RecommendServer,			// 玩家对推荐服务器的选择
	s_nCL_SessionKey_Req,			// 请求SessionKey

	s_nCL_End,
};

enum L2C_PROTOCOL
{
	L2C_Connect,
	L2C_Close
};
#pragma pack(push, 1)
//-------------------------------------------------------
struct CL_LoginReq
{
	enum { s_nCode = s_nCL_LOGIN_REQ };
	enum { enum_Mac_Addr_Len = 32};
	st_LoginInfo loginInfo;
	DWORD mainVersion,subVersion;
	FLOAT fVersion;
	char  macAddr[enum_Mac_Addr_Len];
};

struct CL_PasswordProtect_Rsp 
{
	enum { s_nCode = s_nCL_PasswordProtect_Rsp };
	BYTE result[3];
};

struct CL_PasswordProtect_Cancel
{
	enum { s_nCode = s_nCL_PasswordProtect_Cancel };
};

struct CL_RecommendServer
{
	enum { s_nCode = s_nCL_RecommendServer };
	BYTE mbySelectRecommendServer;
};

struct CL_SessionKey_Req
{
	enum { s_nCode = s_nCL_SessionKey_Req };
};

//struct CL_Link_Over //: public KPacket
//{
//	enum { s_nCode = s_nCL_LINK_OVER, };
//	DWORD m_dwConnectIndex;
//};

//struct CL_World_Link_Start //: public KPacket
//{
//	enum { s_nCode = s_nCL_WORLD_LINK_START, };
//	DWORD m_dwConnectIndex;
//};

//struct CL_Link_Start //: public KPacket
//{
//	enum { s_nCode = s_nCL_LINK_START, };
//	DWORD m_dwConnectIndex;
//};

//struct LC_ServerInfoRsp
//{
//	enum { s_nCode = s_nLC_SERVERINFO_RSP, };
//	STRU_SERVER_INFO m_struServerInfo;
//};

//struct LC_DiffTabFileCountRsp
//{
//	enum { s_nCode = s_nLC_DIFF_TABFILE_COUNT_RSP, };
//	int m_nFileCount;
//};

//struct LC_LAST_CHARACTERSERVER
//{
//	enum { s_nCode = s_nLC_LAST_CHARACTERSERVER, };
//	INT m_nAreaID;
//	INT	m_nServerID;
//};

//struct LC_DiffTabFileRsp
//{
//	enum { s_nCode = s_nLC_DIFF_TABFILE_RSP, };
//	PACKET_FILE m_packetFile;
//};

//struct LC_ProtectPack
//{
//	enum { s_nCode = s_nLC_ProtectPack };
//	// byte sequence of protect pack data
//};

//struct LC_PasswordProtect_Error
//{
//	enum { s_nCode = s_nLC_PasswordProtect_Error, };
//	BYTE byResult;
//};

//struct CL_LogoutReq
//{
//	enum { s_nCode = s_nCL_LOGOUT_REQ, };
//};

//struct LC_LogoutRsp
//{
//	enum { s_nCode = s_nLC_LOGOUT_RSP, };
//};




//struct LC_AreaNameAlias 
//{
//	enum { s_nCode = s_nLC_AreaNameAlias,			};
//};

//struct LC_NotifyMsg 
//{
//	enum { s_nCode = s_nLC_NotifyMsg,			};
//	LC_NotifyMsg()
//	{
//		memset(this, 0 ,sizeof(LC_NotifyMsg));
//	}
//	DWORD m_dwMsgID;
//	char m_achMsg[LC_NOTIFY_MSG_LEN];
//};

//struct LC_RecommendServer 
//{
//	enum { s_nCode = s_nLC_RecommendServer,		};
//	LC_RecommendServer()
//	{
//		memset(this, 0 , sizeof(LC_RecommendServer));
//	}
//};

//struct LC_SessionKey_Rsp 
//{
//	enum { s_nCode = s_nLC_SessionKey_Rsp,		};
//	LC_SessionKey_Rsp()
//	{
//		memset(this, 0 , sizeof(LC_RecommendServer));
//	}
//	st_OP_KEY ucPubKey;
//};



//-------------------------------------------------------
#pragma pack(pop)
