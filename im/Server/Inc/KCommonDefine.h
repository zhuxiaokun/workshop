#pragma once

#define BIT(x) (1 << (x))
#define max_cmd_bits (13)
#define MAX_CLIENT_IN_GATEWAY (20480)
#define MAX_NAME_DEF (64)
#define MAX_PLAYER_COUNT_PRE_ACCOUNT (8)
#define MAX_BUS_LINE_NUM 20
#define MAX_CHANGE_MAP_HISTORY 5 
#define _str_(x) #x
#define REGION_WIDTH_BITS 4     // (1<<4) = 16��
#define REGION_WIDTH (1 << REGION_WIDTH_BITS)
#define SYNC_RADIUS_DEFAULT 3   // Ĭ��ͬ���뾶
#define NORMAL_MAP_REGIONS	(16*16)
#define NPC_ID_BASE_START			(10000LL*10000LL*100LL)
#define NPC_ID_LIMITED				(10000LL*10000LL*10LL)
#define TRIGGER_ID_BASE_START		(10000LL*10000LL*100LL)
#define TRIGGER_ID_LIMITED			(10000LL*10000LL*10LL)
#define GROUNDITEM_ID_BASE_START	(10000LL*10000LL*110LL)
#define GROUND_ID_LIMITED			(10000LL*10000LL*10LL)

#define MAX_SCENE_XML_NAME			64

#define MAX_PLAYER_CONTROLED_OBJECT	20

/**
 * ʮ����:aabbbcccccccccccccc
 * aa:type id
 * bbb:server id
 * cccccccccccccc:14λ����ID
 */
#define TYPE_BASE		100000000000000000ll
#define SERVER_ID_BASE	100000000000000ll
#define GENERATE_OBJ_ID(id, _typeid, _serverid) (id + _typeid * TYPE_BASE + _serverid * SERVER_ID_BASE)
#define GET_OBJ_TYPE(id) (id / TYPE_BASE)
#define GET_SERVER_ID(id) ((id / SERVER_ID_BASE) % 1000)
#define GET_OBJ_ID(id) (id % SERVER_ID_BASE)

const int defObjDirctionCount = 65536;
const int defObjDefaultDirction = 16384;

// ������ұ���������ʱ��(second)
const int DROPPED_KEEP_SECONDS = 5*60;

enum EnumServerIndicator
{
	enum_world_server_flag		= 1,
	enum_character_server_flag	= 2,
	enum_game_server_flag		= 3,
	enum_chat_server_flag		= 4,

	enum_world_server_indp		= (1 << max_cmd_bits),
	enum_character_server_indp	= (2 << max_cmd_bits),
	enum_game_server_indp		= (3 << max_cmd_bits),
	enum_chat_server_indp		= (4 << max_cmd_bits),
};

enum EChSceneBackType
{
	ECSBT_ALLOK,	// ������
	ECSBT_Normal,   // ������
	ECSBT_Dup,		// ����
	ECSBT_MAX
};

enum eWorldObjectKind
{
	wokWorldObj			= 0,
	wokCharacter		= 1,
	wokPlayer			= 2,
	wokNpc				= 3,
	wokGroundItem		= 4,
	wokTrigger			= 5,
	wokCount,	//�뱣�������һ��
};

enum eSyncType
{
	eSyncNone,					// ��ͬ��
	
	eSyncMyself,				// ͬ�����Լ�
	eSyncAllSightPlayer,		// ͬ����������Ұ��Χ�ڵ���ң������Լ���
	eSyncAllSightPlayerButMe,	// ͬ����������Ұ��Χ�ڵ���ң��������Լ���
	eSyncNearby,				// ͬ���������������Լ���
	eSyncNearbyExceptMe,		// ͬ�����������������Լ���
	eSyncArea,					// ͬ�������򣨰����Լ���
	eSyncAreaExceptMe,			// ͬ�������򣨲������Լ���
	eSyncToAI,					// ͬ����������AIServer

	eSyncToCurrentScene,		// ����ǰ����ͬ��
	eSyncToCurrentGame,			// ����ǰgameserverͬ��
	e_SyncToSyncServer,
};

enum
{
	REGION_LEFT,
	REGION_TOP,
	REGION_RIGHT,
	REGION_BOTTOM,
};

namespace ObjectKind
{
	enum
	{
		invalid,
		player,
		npc,
		ground_item,
		count,
	};
}

namespace MapObjAttr
{
	enum
	{
		POS = BIT(0),
	};
}

namespace AppearFlagSet
{
	enum
	{
		login = BIT(0),
	};
}

//namespace ChatMessageType
//{
//	enum
//	{
//		TEXT  = 1,
//		IMAGE = 2,
//		AUDIO = 3,
//		VIDEO = 4,
//	};
//};
//
//namespace ChatMessageTarget
//{
//	enum
//	{
//		PERSON = 1,
//		GROUP  = 2,
//	};
//};
