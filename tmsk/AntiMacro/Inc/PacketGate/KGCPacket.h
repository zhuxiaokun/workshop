#pragma once

#include "KPacket_Segment_Define.h"
#include <KCommonStruct.h>
#include "KSyncAppear.h"
#pragma warning(disable:4200)

enum
{
	s_nCS_PacketStart = s_nCS_Begin,
	s_nCS_Connected = s_nCS_PacketStart,
	s_nCS_ConnClosed,

	s_nCS_PlayerLogin,					// 登进
	s_nCS_ClientReady,					// 客户端加载完成，准备开始跟服务器获取角色信息
	s_nCS_PlayerIn,						// 玩家进游戏 单向
	s_nCS_PlayerLogout,					// 登出
	s_nCS_BackToCharacter,				// 返回人物界面

	s_nCS_PlayerRelive,					// 复活

	s_nCS_StartMove,					// 移动
	s_nCS_StopMove,
	s_nCS_ClientSetPos,

	s_nCS_PlayerAppear_Req,					// 同步
	s_nCS_PlayerAppear_Stat,

	s_nCS_UIAction,						// ui interactive action

	s_nCS_PacketEnd
};

enum
{
	s_nSC_PacketStart = s_nSC_Begin,
	s_nSC_Connected = s_nSC_PacketStart,
	s_nSC_ConnError,
	s_nSC_ConnClosed,

	s_nSC_PlayerLogin,
	s_nSC_BackToCharacter_Rsp,		// 返回人物界面

	// 同步相关包
	s_nSC_Notify_Appear,	// 通知有玩家出现

	s_nSC_Compact_PlayerAppear,		// Appear数据
	s_nSC_Compact_NpcAppear,
	s_nSC_Compact_TcAppear,
	s_nSC_Compact_GoundItemAppear,

	s_nSC_RemoveCharacter,
	
	s_nSC_Sync_PlayerMove,			// Player移动
	s_nSC_Sync_PlayerDirection,
	s_nSC_BeforeChangeScene,
	s_nSC_ChangeScene,

	s_nSC_MessageBox,				// 在客户端显示消息框

	s_nSC_PacketEnd,
};

#pragma pack(push,1)

struct CS_PlayerLogin
{
	enum { s_nCode = s_nCS_PlayerLogin };
	st_PassportKey passport;
	DWORD	otpCode;
	DWORD	mainVersion;
	DWORD	subVersion;
};
// 客户端准备好了
struct CS_ClientReady
{
	enum { s_nCode = s_nCS_ClientReady };
	BYTE mac[6];
};
// 登录时请求玩家数据
struct CS_PlayerIn
{
	enum { s_nCode = s_nCS_PlayerIn };
};
// 玩家退出
struct CS_PlayerLogout
{
	enum { s_nCode = s_nCS_PlayerLogout };
};
// 返回人物界面
struct CS_BackToCharacter
{
	enum { s_nCode = s_nCS_BackToCharacter };
};
//玩家发出的复活指令
struct CS_PlayerRelive
{
	enum	{ s_nCode = s_nCS_PlayerRelive };
	BYTE	byReliveType;	// 复活方式
};
// 主角移动
struct CS_StartMove
{
	enum  { s_nCode = s_nCS_StartMove };
	float fromX, fromY, toX, toY;
	DWORD tick;
	BYTE  moveMode; // 滑行 ?
};
// 停止移动
struct CS_StopMove
{
	enum  { s_nCode = s_nCS_StopMove };
	float fx, fy;
	DWORD tick;
};
// 通知位置
struct CS_ClientSetPos
{
	enum  { s_nCode = s_nCS_ClientSetPos };
	float distance;
	DWORD tick;
};

// 如果客户端没有这个玩家的AppearCache，使用这个包从服务器请求
struct CS_PlayerAppear_Req
{
	enum { s_nCode = s_nCS_PlayerAppear_Req };
};
// 已有Cache数据，检查crc
struct CS_PlayerAppear_Stat
{
	enum { s_nCode = s_nCS_PlayerAppear_Stat };
	DWORD crc1;
	DWORD crc2;
	DWORD crc3;
};
// 客户端UI操作通知
struct CS_UIAction
{
	enum { s_nCode = s_nCS_UIAction };
	int targetType;
	DWORD targetID;
	char ssdata[0]; // stream session data
};

struct SC_PlayerLogin
{
	enum { s_nCode = s_nSC_PlayerLogin };
	int errCode;		// error message ID
	DWORD playerID;
	DWORD serverTick;
	KPlayerBaseDBInfo PlayerBase;
	short lineCount;
	short line;
};
struct SC_BackToCharacter_Rsp
{
	enum { s_nCode = s_nSC_BackToCharacter_Rsp };
	DWORD errCode; // error message ID
};

struct SC_Notify_Appear
{
	enum { s_nCode = s_nSC_Notify_Appear };
	DWORD dwID;
	int objType;
};

struct SC_Compact_PlayerAppear
{
	enum {s_nCode = s_nSC_Compact_PlayerAppear};

	DWORD dwID;
	DWORD dwStateTime;	// 没处理
	BYTE  dataMask;
	WORD  extLen;

	// AppearData所需要的内存，里面的数据由dataMask决定
	char dataBuffer[1024];

	K_PlayerAppear_1* GetData1()
	{
		if(dataMask & 0x01) return (K_PlayerAppear_1*)&dataBuffer;
		return NULL;
	}
	K_PlayerAppear_2* GetData2()
	{
		int offset = 0;
		if(dataMask & 0x01) offset += GetData1()->Size();
		if(dataMask & 0x02) return (K_PlayerAppear_2*)&dataBuffer[offset];
		return NULL;
	}
	K_PlayerAppear_3* GetData3()
	{
		int offset = 0;
		if(dataMask & 0x01) offset += GetData1()->Size();
		if(dataMask & 0x02) offset += GetData2()->Size();
		if(dataMask & 0x04) return (K_PlayerAppear_3*)&dataBuffer[offset];
		return NULL;
	}
	char* GetExtData()
	{
		int offset = 0;
		if(dataMask & 0x01) offset += GetData1()->Size();
		if(dataMask & 0x02) offset += GetData2()->Size();
		if(dataMask & 0x04) offset += GetData3()->Size();
		return &dataBuffer[offset];
	}
	int Size()
	{
		int c = sizeof(SC_Compact_PlayerAppear) - sizeof(dataBuffer);
		if(dataMask & 0x01) c += GetData1()->Size();
		if(dataMask & 0x02) c += GetData2()->Size();
		if(dataMask & 0x04) c += GetData3()->Size();
		c += extLen;
		return c;
	}
};

struct SC_Compact_NpcAppear
{
	enum { s_nCode = s_nSC_Compact_NpcAppear };

	DWORD dwID;
	BYTE  dataMask;
	WORD  extLen;
	char dataBuffer[1024];

	K_NpcAppear_1* GetData1()
	{
		if(dataMask & 0x01) return (K_NpcAppear_1*)&dataBuffer;
		return NULL;
	}
	K_NpcAppear_2* GetData2()
	{
		int offset = 0;
		if(dataMask & 0x01) offset += GetData1()->Size();
		if(dataMask & 0x02) return (K_NpcAppear_2*)&dataBuffer[offset];
		return NULL;
	}
	K_NpcAppear_3* GetData3()
	{
		int offset = 0;
		if(dataMask & 0x01) offset += GetData1()->Size();
		if(dataMask & 0x02) offset += GetData2()->Size();
		if(dataMask & 0x04) return (K_NpcAppear_3*)&dataBuffer[offset];
		return NULL;
	}
	char* GetExtData()
	{
		int offset = 0;
		if(dataMask & 0x01) offset += GetData1()->Size();
		if(dataMask & 0x02) offset += GetData2()->Size();
		if(dataMask & 0x04) offset += GetData3()->Size();
		return &dataBuffer[offset];
	}
	int Size()
	{
		int c = sizeof(SC_Compact_NpcAppear) - sizeof(dataBuffer);
		if(dataMask & 0x01) c += GetData1()->Size();
		if(dataMask & 0x02) c += GetData2()->Size();
		if(dataMask & 0x04) c += GetData3()->Size();
		c += extLen;
		return c;
	}
};

struct SC_Compact_TcAppear
{
	enum { s_nCode = s_nSC_Compact_TcAppear };
	DWORD dwID;
	BYTE  dataMask;
	WORD  extLen;
	char dataBuffer[1024];

	K_TreasureCaseAppear_1* GetData1()
	{
		if(dataMask & 0x01) return (K_TreasureCaseAppear_1*)&dataBuffer;
		return NULL;
	}
	K_TreasureCaseAppear_2* GetData2()
	{
		int offset = 0;
		if(dataMask & 0x01) offset += GetData1()->Size();
		if(dataMask & 0x02) return (K_TreasureCaseAppear_2*)&dataBuffer[offset];
		return NULL;
	}
	K_TreasureCaseAppear_3* GetData3()
	{
		int offset = 0;
		if(dataMask & 0x01) offset += GetData1()->Size();
		if(dataMask & 0x02) offset += GetData2()->Size();
		if(dataMask & 0x04) return (K_TreasureCaseAppear_3*)&dataBuffer[offset];
		return NULL;
	}
	char* GetExtData()
	{
		int offset = 0;
		if(dataMask & 0x01) offset += GetData1()->Size();
		if(dataMask & 0x02) offset += GetData2()->Size();
		if(dataMask & 0x04) offset += GetData3()->Size();
		return &dataBuffer[offset];
	}
	int Size()
	{
		int c = sizeof(SC_Compact_TcAppear) - sizeof(dataBuffer);
		if(dataMask & 0x01) c += GetData1()->Size();
		if(dataMask & 0x02) c += GetData2()->Size();
		if(dataMask & 0x04) c += GetData3()->Size();
		c += extLen;
		return c;
	}
};

struct SC_Compact_GroundItemAppear
{
	enum { s_nCode = s_nSC_Compact_GoundItemAppear };
	DWORD dwID;
	BYTE  dataMask;
	WORD  extLen;
	char dataBuffer[1024];

	K_GroundItemAppear_1* GetData1()
	{
		if(dataMask & 0x01) return (K_GroundItemAppear_1*)&dataBuffer;
		return NULL;
	}
	K_GroundItemAppear_2* GetData2()
	{
		int offset = 0;
		if(dataMask & 0x01) offset += GetData1()->Size();
		if(dataMask & 0x02) return (K_GroundItemAppear_2*)&dataBuffer[offset];
		return NULL;
	}
	K_GroundItemAppear_3* GetData3()
	{
		int offset = 0;
		if(dataMask & 0x01) offset += GetData1()->Size();
		if(dataMask & 0x02) offset += GetData2()->Size();
		if(dataMask & 0x04) return (K_GroundItemAppear_3*)&dataBuffer[offset];
		return NULL;
	}
	char* GetExtData()
	{
		int offset = 0;
		if(dataMask & 0x01) offset += GetData1()->Size();
		if(dataMask & 0x02) offset += GetData2()->Size();
		if(dataMask & 0x04) offset += GetData3()->Size();
		return &dataBuffer[offset];
	}
	int Size()
	{
		int c = sizeof(SC_Compact_GroundItemAppear) - sizeof(dataBuffer);
		if(dataMask & 0x01) c += GetData1()->Size();
		if(dataMask & 0x02) c += GetData2()->Size();
		if(dataMask & 0x04) c += GetData3()->Size();
		c += extLen;
		return c;
	}
};

struct SC_RemoveCharacter
{
	enum { s_nCode = s_nSC_RemoveCharacter };
	DWORD dwID;
	int objType;
};

//玩家移动
struct SC_Sync_PlayerMove
{
	enum { s_nCode = s_nSC_Sync_PlayerMove };
	DWORD playerID;
	float fx,fy,tx,ty;
};

struct SC_Sync_PlayerDirection
{
	enum { s_nCode = s_nSC_Sync_PlayerDirection };
	DWORD playerID;
	float angle;
};

struct SC_MessageBox
{
	enum { s_nCode = s_nSC_MessageBox };
	DWORD titleID;
	DWORD textID;
};

struct SC_BeforeChangeScene
{
	enum { s_nCoe = s_nSC_BeforeChangeScene };
};

struct SC_ChangeScene
{
	enum { s_nCode = s_nSC_ChangeScene };
	char serverIp[16];
	WORD serverPort;
	int mapID;
	char mapName[64];
	DWORD worldID;
	float fx,fy;
};

#pragma pack(pop)

template <int x> void _export_CS_SC_code_to_lua()
{
	export_enum_to_lua(s_nCS_PlayerLogin);	// CS
	export_enum_to_lua(s_nCS_ClientReady);
	export_enum_to_lua(s_nCS_PlayerIn);
	export_enum_to_lua(s_nCS_PlayerLogout);
	export_enum_to_lua(s_nCS_BackToCharacter);
	export_enum_to_lua(s_nCS_PlayerRelive);
	export_enum_to_lua(s_nCS_StartMove);
	export_enum_to_lua(s_nCS_StopMove);
	export_enum_to_lua(s_nCS_ClientSetPos);
	export_enum_to_lua(s_nCS_PlayerAppear_Req);
	export_enum_to_lua(s_nCS_PlayerAppear_Stat);
	export_enum_to_lua(s_nCS_UIAction);

	export_enum_to_lua(s_nSC_PlayerLogin);		// SC
	export_enum_to_lua(s_nSC_BackToCharacter_Rsp);
	export_enum_to_lua(s_nSC_Notify_Appear);
	export_enum_to_lua(s_nSC_Compact_PlayerAppear);
	export_enum_to_lua(s_nSC_RemoveCharacter);
	export_enum_to_lua(s_nSC_Sync_PlayerMove);
	export_enum_to_lua(s_nSC_Sync_PlayerDirection);
	export_enum_to_lua(s_nSC_MessageBox);
}
#define export_CS_SC_code_to_lua() _export_CS_SC_code_to_lua<0>()
