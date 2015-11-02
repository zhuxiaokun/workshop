/********************************************************************
	history:
    created:	2006/03/07
	created:	7:3:2006   14:38
	
    filename: 	\SanGuo\SanGuoMain\Include\CommonDefine.h
	file path:	\SanGuo\SanGuoMain\Include
	file base:	CommonDefine
	file ext:	h

	author:		xueyan
	copyright:  KingSoft 2005-2007

	purpose:	逻辑、客户端、服务器共用的一些定义
    comment:    
*********************************************************************/

#pragma once
#pragma warning(disable:4305)

#include "KPassportDefine.h"

#define MAX_NATION			3					// 国家个数
#define CELL_VILLAGE_BITS	10
#define MAX_MAP_VILLAGE		(1<<CELL_VILLAGE_BITS) // 一个地图中最大村庄个数

//  各模块通用的宏定义
#define DEF_TO_STRING(x)	#x
#define	MAX_NAME			20					//  角色名字长度
#define MAX_PLAYER_NAME		20
#define MAX_ENABLE_NAME_LENGTH	(14)			//	角色名称允许最大长度
#define MAX_MB_NAME			12					// 法宝名字最长12字节
#define	MAX_ACCOUNT			20					//  游戏帐号长度
#define	MAX_NAME_DEF		(MAX_NAME + 2)		//  定义结构的时候，最好在名字的后面冗余2个字节，这样就可以避免使用strcpy，
#define	MAX_MODEL_NAME_DEF	(MAX_PATH)			//  定义结构的时候，最好在名字的后面冗余2个字节，这样就可以避免使用strcpy，
												//  而可以直接使用memcpy( , , MAX_NAME)
#define MAX_SIGNATURE		(20)				// 个人签名允许字数
#define MAX_SIGNATURE_DEF	(32)				// 个人签名最大字数 12个保留
#define DEF_MAX_STRENGTH_LV	(10)				// 最大强化等级

#define MAX_PLAYER_CREATE_INDEX_PRE_ACCOUNT	(6)			// 每个账户新建的最大角色索引
#define MAX_PLAYER_COUNT_PRE_ACCOUNT	(16)			// 每个账户可以使用的最大角色数

#define MAX_CHOOSE_QUESTREWARDITEM (6)			// 最大物品奖励可选择数目

#define	MAX_ACCOUNT_DEF		(MAX_ACCOUNT + 2)		//  同上
#define DEF_DIS_INTERACTIVE	(6.0f)
#define DEF_USE_SERVER_TEXT	(0xFFFFFFFF)
#define DEF_One_Minute	(60)
#define DEF_One_Hour	(60 * DEF_One_Minute)
#define DEF_One_Day		(24 * DEF_One_Hour)
#define DEF_PLAYER_PROTECT1_TIME	(7 * DEF_One_Day)
#define DEF_PLAYER_PROTECT2_TIME	(40 * DEF_One_Day)

static const float DEF_GRAVITY_ACCELERATION = -0.1f;  // 重力加速度
static const float DEF_JUMP_INIT_VELOCITY = 1;

static const	int		CFG_MAX_BAG_EQUIP_ITEM_DEFAULT_COUNT		= 20;			// 背包中装备道具默认开启数量
static const	int		CFG_MAX_BAG_EQUIP_ITEM_COUNT				= 20;			// 背包中装备道具数量

static const	int		CFG_MAX_BAG_NORMAL_ITEM_DEFAULT_COUNT		= 60;			// 背包中普通道具默认开启数量
static const	int		CFG_MAX_BAG_NORMAL_ITEM_COUNT				= 60;			// 背包中普通道具数量
static const	int		CFG_MAX_BAG_EXTCOUNT						= 10;			// 每次扩展数量

static const	int		CFG_MAX_BAG_MATERIAL_ITEM_DEFAULT_COUNT		= 60;			// 背包中材料道具默认开启数量
static const	int		CFG_MAX_BAG_MATERIAL_ITEM_COUNT				= 60;			// 背包中材料道具数量

static const	int		CFG_MAX_BAG_QUEST_ITEM_DEFAULT_COUNT		= 60;			// 背包中任务道具默认开启数量
static const	int		CFG_MAX_BAG_QUEST_ITEM_COUNT				= 60;			// 背包中任务道具数量

static const	int		CFG_MAX_BAG_PET_ITEM_DEFAULT_COUNT			= 3;			// 背包中宠物默认开启数量
static const	int		CFG_MAX_BAG_PET_ITEM_COUNT					= 7;			// 背包中宠物数量

static const	int		CFG_MAX_BANK_NORMAL_ITEM_DEFAULT_COUNT		= 100;			// 仓库中普通道具默认开启数量
static const	int		CFG_MAX_BANK_NORMAL_ITEM_COUNT				= 100;			// 仓库中普通道具数量
static const	int		CFG_MAX_BANK_EXTCOUNT						= 10;			// 仓库每次扩展数量

static const	int		CFG_MAX_BANK_MATERIAL_ITEM_DEFAULT_COUNT	= 49;			// 仓库中材料道具默认开启数量
static const	int		CFG_MAX_BANK_MATERIAL_ITEM_COUNT			= 49;			// 仓库中材料道具数量

static const    int     CFG_MAX_BAG_SOLD_ITEM_DEFAULT_COUNT         = 10;            // 卖出物品槽的默认格数
static const    int     CFG_MAX_BAG_SOLD_ITEM_COUNT					= 10;            // 卖出物品槽的最大格数

#define MAX_HOLD_SOLDITEM_TIME    60

#define MAX_CHATMESSAGE_LEN		256

#define GUILD_NAME_MAX			32
#define MAX_LEVELINFO_COUNT		(120)

// 最大生活技能数
#define MAX_LIFESKILL_TYPE	8
// 生活技能中的采集&制造级别数量
#define MAX_LIFESKILL_LEVELBYTES 8

#define MAX_LIFESKILL_EXPERTISE_TYPE 8

// 魂魄的种类数
#define SOUL_TYPE_COUNT 5

#define MB_MAX_GROUP_COUNT 4


// 帮派商店
#define NPC_GUILD_STORE_ID		29
// 荣誉商店
#define NPC_HONOUR_STORE_ID		30
#define NPC_SPCAIL_STORE_ID		33 // 暂时不用
// 物品商店
#define NPC_MATERIAL_STORE_ID	32
// 跑商商店，只能买入
#define NPC_QUEST_BUY_STORE_ID	34
// 跑商商店，只能卖出
#define NPC_QUEST_SELL_STORE_ID	35

// 恩情值商店, 只能卖出
#define NPC_AFFECTION_STORE_ID 40

#define Max_Mentor_Dit_Length		256
#define Max_Mentor_Info_Exist_Delay 10 * 60 * 60
#define Max_Mentor_Pupil_sum		2
#define Max_Mentor_IntimacyVal		1000

//选举系统入选理由, 不超过20
#define Max_Enroll_Reason_Length	40

// 设置注册用户名和公会名必须过滤的非法字符
#define NO_REG_INVALID_CHARS "`~!@#$%^&*()-=\\[];',./_+|{}:\"<>?"

// 高度障碍容忍限度（单位：1/32米）
#define HEIGHT_OBSTACLE_TOLERANT_RANGE 6

// 角色状态机的状态
enum CHARACTER_STATE
{
	csInvalid = 0,				// 无效状态

	csOnStand,					// 站立状态
	csOnSit,					// 打坐状态

	csOnMove,					// 移动状态，包括走、跑、自动行走、飞行等

	csOnCharge,					// 蓄力状态
	csOnPrepare,				// 吟唱状态
	csOnShoot,					// 发招状态
	csOnChannel,				// 引导状态
	csOnRecover,				// 收招状态

	csOnDeath,					// 死亡状态
	csOnTrade,					// 交易状态

	csOnJumpRise,				//  起跳及上升
	csOnFall,					//  坠落
	csOnLand,					//  着陆

	csOnUnFight,				// 被击状态
	csOnAutoFly,				// 自动飞行
	csOnByBus,					// 乘坐bus状态

	csOnBeHeld,					// 被人拖状态

	csOnFreeze,					// 被冻结
	csTotal
};

enum CHARACTER_EVENT
{
	ceInvalid = 0,				// 无效事件

	ceOnHit,					// 被击事件
	ceOnDuck,					// 闪避事件
	ceOnExpUpdate,				// 升级事件
	ceOnLand,					// 着陆事件
	ceOnLandRun,				// 跑动着陆事件
	ceOnHonor,					// 获得荣誉值
	ceOnFaithExpUpdate,			// 获得修为值事件

	ceTotal
};

enum CHARACTER_CONDITIONEVENT
{
	cceInvalid = 0,

	cceQuestInvalid = 0,		// 0.	任务条件事件 --- 不存在任务
	cceQuestCondition = 1,		// 1.	任务条件事件 --- 不满足接受任务条件
	cceQuestAccept = 2,			// 2.	任务条件事件 --- 已接受但还未完成任务
	cceQuestUnaccept = 3,		// 3.	任务条件事件 --- 满足但还未接受任务
	cceQuestFinish = 4,			// 4.	任务条件事件 --- 接并完成任务但还为交任务

	cc3de_onengage = 5,			// 5.	法宝条件事件 --- 进入战斗
	cc3de_ontruce = 6,			// 6.	法宝条件事件 --- 退出战斗

	cceTotal,
};

//  NPC基本状态
enum NPC_BASE_STATE
{
	nbsInvalid = 0,				// 无效状态

	nbsGod      ,				// 无敌状态
	nbsAuto     ,				// 自动状态
	nbsFight    ,				// 战斗状态
	nbsPassivity,				// 被动状态
	nbsMax		,
};

//  NPC扩展状态
enum NPC_STATE
{
	nsInvalid = 0,				// 无效状态

	// 无敌
	nsDisappear,				// 消失状态
	nsWarp,						// 瞬移状态
	nsBorn,						// 出生
	nsDie,						// 死亡状态
	nsRelease,					// 解除状态

	// 自动
	nsLeisure,					// 休闲状态
	nsPointMove,				// 定点移动状态
	nsPointLeisure,				// 定点休闲状态
	nsLeisureX,					// 特殊休闲状态
	nsFollow,					// 跟随状态
	nsPickUp,					// 拾取状态
	nsDamaged,					// 损毁状态

	// 战斗
	nsFightWait,				// 战斗等待
	nsChase,					// 追击状态
	nsFightMove,				// 战斗移动
	nsGiddy,					// 眩晕状态
	nsFear,						// 恐惧状态
	nsFightRunAway,				// 战斗逃跑

	// 战斗停止
	nsOutOfControl,				// 失控状态
	nsRunAway,					// 逃跑状态
	nsMove,						// 移动状态
	nsSurrender,				// 投降状态

	// 被动状态
	nsBeTake,					// 被携带

	// 追加的状态
	nsPointMoveX,				// 特殊定点移动状态
	nsMax,
};

enum MAGIC_BABY_STATE
{
	mbsInvalid = 0,	
	mbsBack,					// 收回
	mbsFightWait,				// 战斗待机
	mbsFightPrepare,			// 战斗预备
	mbsFight,					// 战斗中
	mbsFightEnd,				// 战斗结束
	mbsEatGrass,				// 吸灵草
	mbsRide,					// 骑乘

	mbsMax,

};

enum EVENT_ID
{
	EVENT_ENTRY = 0,	// 进入
	EVENT_EXIT,			// 离开
	EVENT_IDLE,			// 空闲
	EVENT_LAST,			// 上一个状态
	EVENT_APPOINTED,	// 指定好的一个状态
	EVENT_MAX_COUNT,
};

enum ANIMSTATUS
{
	// EVENT_ENTRY,
	// EVENT_EXIT,
	// EVENT_IDEL,
	ANIMSTATUS_INVALID = 0,
	ANIMSTATUS_CHANGE_DIRECTION = 1,
	ANIMSTATUS_STOP = 3,
	ANIMSTATUS_FORWARD = 4,
	ANIMSTATUS_FLY_CHANGE = 5,
	ANIMSTATUS_PREPARE = 6,
	ANIMSTATUS_STATETIMEEND = 7,
	ANIMSTATUS_AUTOMOVE = 8,
	ANIMSTATUS_DEATH = 9,
	ANIMSTATUS_JUMPRISE = 10,  //  起跳及上升
	ANIMSTATUS_FALL = 11,  //  坠落
	ANIMSTATUS_LAND = 12,  //  着陆
	ANIMSTATUS_UNFIGHT = 13, //  被击中
	ANIMSTATUS_RELIVE = 14,
	ANIMSTATUS_CANCEL = 15,	//打断
	ANIMSTATUS_SHOOT = 16,	//直接攻击
	ANIMSTATUS_CHANNEL = 17,//进入CHANNEL状态
	ANIMSTATUS_AUTOFLY = 18, //自动飞行
	ANIMSTATUS_BYBUS = 19, // 乘坐bus

	ANIMSTATUS_BE_HELD = 20, //被人拖上
	ANIMSTATUS_BE_PUT =21, //被放下

	ANIMSTATUS_FREEZE = 22, // 冻结
	ANIMSTATUS_CHARGE = 23, // 蓄力
	ANIMSTATUS_MAX
};

enum AIANIMSTATUS
{
	ANIMSTATUS_AISTATE_TIMEEND = EVENT_MAX_COUNT,
	ANIMSTATUS_AISTATE_POINTMOVE ,
	ANIMSTATUS_AISTATE_POINTLEISURE ,
	ANIMSTATUS_AISTATE_LEISURE ,
	ANIMSTATUS_AISTATE_FINDENEMY ,
	ANIMSTATUS_AISTATE_WARP ,
	ANIMSTATUS_AISTATE_DEATH ,
	ANIMSTATUS_AISTATE_FOLLOW ,
	ANIMSTATUS_AISTATE_BORN,
	ANIMSTATUS_AISTATE_DISAPPEAR,
	ANIMSTATUS_AISTATE_RELEASE,
	ANIMSTATUS_AISTATE_RUNAWAY,
	ANIMSTATUS_AISTATE_SURRENDER,
	ANIMSTATUS_AISTATE_FIGHT_WAIT,		// 战斗等待
	ANIMSTATUS_AISTATE_MOVE,			// 无打扰移动
	ANIMSTATUS_AISTATE_FIGHT_MOVE,		// 战斗中无打扰移动
	ANIMSTATUS_AISTATE_POINTMOVEX ,		// 特殊移动
	ANIMSTATUS_AISTATE_LEISUREX ,		// 特殊休闲
	ANIMSTATUS_AISTATE_FEAR,			// 恐惧状态
	ANIMSTATUS_AISTATE_GIDDY,			// 眩晕状态
	ANIMSTATUS_AISTATE_FIGHTRUNAWAY,	// 战斗逃跑
	ANIMSTATUS_AISTATE_MAX ,
};

enum MBANIMSTATUS
{
	ANIMSTATUS_MBSTATE_BACK = EVENT_MAX_COUNT,
	ANIMSTATUS_MBSTATE_FIGHT_WAIT ,
	ANIMSTATUS_MBSTATE_FIGHT_PREPARE ,
	ANIMSTATUS_MBSTATE_FIGHT ,
	ANIMSTATUS_MBSTATE_FIGHT_END ,
	ANIMSTATUS_MBSTATE_EAT_GRASS ,
	ANIMSTATUS_MBSTATE_RIDE ,

	ANIMSTATUS_MBSTATE_MAX ,
};

// 客户端逻辑鼠标状态
enum MOUSE_STATE
{
	msInvial,			// 鼠标无状态

	msOnNormal,			
	msOnMoveItem,		// 移动物品状态
	msOnPickItem,		// 捡取物品状态

	msTotal,
};

// 鼠标动作
enum MOUSEACTION
{
	MOUSEACTION_INVILAD = 0,

	MOUSEACTION_LEFTDOWN,
	MOUSEACTION_LEFTUP,
	MOUSEACTION_LEFTCLICK,
	MOUSEACTION_RIGHTDOWN,
	MOUSEACTION_RIGHTUP,
	MOUSEACTION_RIGHTCLICK,

	MOUSEACTION_TOTAL,
};

enum NPC_COMMAND
{
	NONE_CMD            = 0,
	MOVE_CMD            = 1,
	STOP_CMD            = 2,
	ATTACK_OBJECT_CMD   = 3,
	ATTACK_AREA_CMD     = 4,
	PATROL_CMD          = 5,
	HOLD_CMD            = 6,
	SKILL_OBJECT_CMD    = 7,
	SKILL_AREA_CMD      = 8,
	FOLLOW_CMD          = 9,
	RELIVE_CMD          = 10,
};

// Character appear
enum ENUM_CHARACTER_TYPE
{
	ct_Player = 1,
	ct_Npc,
	ct_Transport,
	ct_TreasureCase,
	ct_MovableShop,
};
// 复活方式
enum RELIVE_TYPE
{
	rtInvalid = 0,
	rtOrigin,			// 原地复活
	rtGrave,			// 墓地复活
	rtPerfectOrigin,	// 原地完美复活
	rtTotal
};

// 死亡方式
enum DEATH_TYPE
{
	dtInvalid = 0,
	dtNormal,
	dtCritical_1,//物理伤害
	dtCritical_2,//水
	dtCritical_3,//火
	dtCritical_4,//风
	dtCritical_5,//雷
	dtCritical_6,//土
	dtTotal,
};

// 休闲状态
enum STAND_TYPE
{
	stInvalid = 0,
	stNormal,
	stFight,
	stTotal
};

//  移动方式
enum MOVE_TYPE
{
	mtInvalid = 0,
	mtAutoChange,
	mtWalk,
	mtRun,
	mtFly,
	mtFight,
	mtStop,
	mtWarp,
	mtRunaway,
	mtTotal
};

// 装备模式
enum EQUIPMENT_MODE
{
	emNormal = 0,	// 装备模式
	emFashion = 1,	// 时装模式
	emNaked = 2,	// 裸体模式
};

enum HELM_SHOW_MODE
{
	emHideHelm = 0,	// 隐藏头盔
	emShowHelm,		// 显示头盔
};


// 通知客户端位置移动类型
enum SET_POSITION_TYPE
{
	e_Relive = 0,
	e_Item,
	e_Skill,
	e_GmCommand,
	e_Transport,
	e_ConvokeCommand,
};
//  for skill // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // /
const int	MAX_SKILLNAME_SIZE				= 32;
#define MAX_SKILLICON_SIZE					200
#define MAX_3DACTION_NAME_SIZE				200


// 攻击目标
enum TARGET_TYPE
{
	ttInvalid = 0,

	ttCoordination,
	ttPlayerPointer,
	ttNpcPointer,
	ttMagicBabyPointer,
	ttPlayerID,
	ttNpcID,
	ttMagicBabyID,
	ttTreasureCaseID,
	ttTreasureCasePointer,
	ttTransportID,
	ttTransportPointer,

	ttTotal
};

enum enumTargetClass
{
	tcInvalid = 0,
	tcAttackTarget,
	tcSelectedTarget,
	tcMouseTarget,
	tcMouseOver,
};

enum UPDATESTATE_TYPE
{
	enumUST_STATE = 0,
	enumUST_TIME,
	enumUST_SPEED,
};

enum UPDATESTATEPARAM_TYPE
{
	utInvalid = 0,
	utMoveType,
	utLeisureID,
	utDeathType,
	utStandType,
};

enum eItem_Operation_Attribute_Update
{
	enumIOR_Attribute_Begin = 0,
	enumIOR_Attribute_AllUpdate,				// 全部更新
	enumIOR_Attribute_IncreaseNum,				// 增加数量
	enumIOR_Attribute_SetNum,
	enumIOR_Attribute_IncreaseFrequencyNum,		// 增加使用次数
	enumIOR_Attribute_SetFrequencyNum,
	enumIOR_Attribute_IncreaseStrengthenLevel,	// 设置强化等级
	enumIOR_Attribute_SetStrengthenLevel,
	enumIOR_Attribute_IncreaseDura,				// 改变耐久度
	enumIOR_Attribute_SetDura,
	enumIOR_Attribute_SetPrivateFlags,			// 改变特殊标记
	enumIOR_Attribute_SetAliveTime,				// 修改存亡时间
	enumIOR_Attribute_Bind,						// 物品绑定者
	enumIOR_Attribute_UnBind,					// 物品绑定者
	enumIOR_Attribute_UpdateEquipInfo,			// 更新装备信息
	enumIOR_Attribute_BuffID,					// 入灵
	enumIOR_Attribute_End,
};

enum NPC_SPECIAL_FUNCTION
{
	NSF_NOTHING		= 0	,
	NSF_CHECKQUEST		,		// 需要检查凶手的任务ID
	NSF_REFRESHNPCID	,		// 如果凶手没有完成以上任务,则刷一个新的NPC出来
	NSF_LIVETIME		,		// 存活时间
	NSF_LIVETIME_LEAVE_FIGHT,	// 脱离战斗后存活时间
	NSF_RANDOMITEM		,		// 随机物品
	NSF_FUNC_COUNT		,
};

enum LEARN_SKILL_FLAG
{
	enumLSF_CAN_LEARN	= 0	,	// 可以学习的技能
	enumLSF_CAN_NOT_LEARN	,	// 你不可以学习的技能
	enumLSF_ALREADY_LEARN	,	// 已经学会的技能
	enumLSF_ALREADY_LEARN_HIGHER,	//已经学会更高等级的了
	enumLSF_ALL_SKILL		,	// 全部
};

enum CAREER_CHOISE_LEVEL
{
	enumCCL_Least		= 0,		//
	enumCCL_Start		= 10,		//选职
	enumCCL_Advanced1	= 40,		//进阶
	enumCCL_Advanced2	= 60,		//再次进阶

};

//Talisman功能
enum TALISMAN_BIND_STATUS
{
	enumTBS_Invalid = 0,
	enumTBS_Bind,
	enumTBS_UnBind,
	enumTBS_BigMode,
	enumTBS_SmallMode,
	enumTBS_Total
};

// 宝箱功能
enum TREASURECASE_DRAG_STATUS
{
	enumTDS_Invalid = 0,
	enumTDS_Drag,
	enumTDS_UnDrag,
	enumTDS_Total
};

enum ENUM_PLAYER_CLASS_TYPE
{
	enumPC_NOCLASS = 0,
	enumPC_XIASHI,
	enumPC_FANGSHI,
	enumPC_YUESHI,
	enumPC_WUSHI,
	enumPC_CIKE,
	enumPC_GONGJIAN,
	enumPC_MAXCLASS,
};

///************************************************************************/
///* 玩家交易                                                             */
///************************************************************************/
//// 玩家的交易状态类型
//enum enumPlayerTrade_State
//{
//	enumPT_STATE_NOTHING = 0,	// 无状态
//	enumPT_STATE_LOCKED,		// 锁定状态
//	enumPT_STATE_AGREEMENT,		// 可以交易
//	enumPT_STATE_TRADING,		// 交易中
//};
//
//// 客户端请求给服务器的命令
//enum enumPlayerTrade_Command
//{
//	enumPT_Nothing	= 0,
//
//	enumPT_Request_PutOnItem,		// 往交易栏放置物品
//	enumPT_Request_ModifyMoney,		// 往交易栏放置物品
//	enumPT_Request_GetBack,			// 从交易栏取回物品
//	enumPT_Request_SwapItem,		// 交易栏中物品相互移动
//	enumPT_Request_Lock,			// 锁定交易栏
//	enumPT_Request_Agree,			// 同意交易
//	enumPT_Request_Cancel,			// 取消交易
//	enumPT_Request_PutOnMB,			// 往交易栏放置物品
//
//	enumPT_Response_ItemPutOn,		// 物品被放置在交易栏上了
//	enumPT_Response_ItemGetBack,	// 物品被拿回来了
//	enumPT_Response_ItemSwaped,		// 物品被交换了
//	enumPT_Response_StateChanged,	// 交易状态修改
//	enumPT_Response_MoneyModified,	// 金钱修改
//	enumPT_Response_Successed,		// 成功
//	enumPT_Response_Failed,			// 失败
//	enumPT_Response_Cancellation,	// 取消
//	enumPT_Response_PutOnMB,		// 回应方法宝
//
//	enumPT_Count,
//};

// 服务器反馈给客户端的消息类型
enum enumPlayerTrade_Reponse
{
	enumPT_REP_NOTHING = 0,

};

enum CHARACTER_ATTR_ISSYNC
{
	enum_NOSYNC_ATTR,
	enum_SYNC_ATTR_TO_SELF_ONLY,
	enum_SYNC_ATTR_TO_ALL_BUT_ME,
	enum_SYNC_ATTR_TO_ALL,
};


// 聊天服务器分组信息
enum ENUM_CHAT_GROUP
{
	enumECG_Invalid = 0,
	enumECG_System,					// 系统（显示公告、系统信息等，绝大部分显示在最上方的滚动条中）
	enumECG_Info,					// 信息（显示个人信息 By Client）
	enumECG_BattleInfo,				// 战斗信息（显示战斗信息 By Client）
	enumECG_Private,				// 私聊（一对一私聊 By ChatServer）
	enumECG_Team,					// 队伍（整个队伍 By ChatServer）
	enumECG_Current,				// 附近（同步范围内 by GameServer）
	enumECG_Map,					// 区域（同步范围整个地图 By GameServer）
	enumECG_Guild,					// 帮派（整个帮派 By ChatServer）
	enumECG_Nation,					// 国家（整个国家 By ChatServer）
	enumECG_World,					// 世界（整个服务器，By ChatServer）
	enumECG_BattleCry,				// 呐喊（整个地图 By GameServer，整个地图）
	enumECG_BattleField,			// 战场频道（整个战场地图 By GameServer，其实就是整个地图）
	enumECG_MainFrameInfomation,	//主界面信息（角色头顶）
	enumECG_MainFrameTopInfomation,	//主界面置顶信息
	enumECG_MessageBox,				// 对话框
	enumECG_SystemAndMainFrame,		// 系统频道和头顶都显示
	enumECG_ShuiGuo,				// 水国
	enumECG_XueGuo,					// 雪国
	enumECG_ShanGuo,				// 山国
	enumECG_SingletonMessageBox,	// 对话框
	enumECG_FindTeam,				// 寻求队伍（符合等级条件的 By ChatServer）
	enumECG_CustomChannel,			// 自定义频道开始
	enumECG_IMGuild,				// 帮派（IM）
	enumECG_IMPrivate,				// 私聊（IM）
	enumECG_Count,					// 频道总数量
};

//// 聊天或系统消息界面显示位置
//enum ENUM_CHAT_SHOW_POSITION
//{
//	enumECSP_Invalid = 0,
//	enumECG_System,			//聊天窗口
//	enumECSP_MainFrameInfomation,	//主界面信息（角色头顶）
//	enumECSP_MainFrameTopInfomation,//主界面置顶信息
//	enumECSP_Count,
//};

//需要给3D发的表现ActionEvent枚举
enum Action_Character_3DEvent
{
	c3d_onexpupdate = 0,		// 0.	升级

	c3d_onhit,					// 1.	被击
	c3d_onhit_shoulder,			// 2.	被击(背)
	c3d_onhit_lift,				// 3.	被击(举)
	c3d_onhit_resist,			// 4.	被击(抗)
	c3d_onhit_drag,				// 5.	被击(拖)
	c3d_onhit_rideride,			// 6.	被击(骑骑)
	c3d_onhit_ridesit,			// 7.	被击(骑坐)
	c3d_onhit_ridestand,		// 8.	被击(骑站)
	c3d_onhit_ridehang,			// 9.	被击(骑挂)
	c3d_onhit_rideside,			// 10.	被击(骑侧)

	c3d_onbursthit,
	c3d_onbursthit_shoulder,	// 11.	被击(背)
	c3d_onbursthit_lift,		// 12.	被击(举)
	c3d_onbursthit_resist,		// 13.	被击(抗)
	c3d_onbursthit_drag,		// 14.	被击(拖)
	c3d_onbursthit_rideride,	// 15.	被击(骑骑)
	c3d_onbursthit_ridesit,		// 16.	被击(骑坐)
	c3d_onbursthit_ridestand,	// 17.	被击(骑站)
	c3d_onbursthit_ridehang,	// 18.	被击(骑挂)
	c3d_onbursthit_rideside,	// 19.	被击(骑侧)
	c3d_onduck,					// 20.	闪避

	c3d_onland,					// 21.  着陆事件
	c3d_onland_shoulder,		// 22.	3D着陆(背)
	c3d_onland_lift,			// 23.	3D着陆(举)
	c3d_onland_resist,			// 24.	3D着陆(抗)
	c3d_onland_drag,			// 25.	3D着陆(拖)
	c3d_onland_petride,			// 26.	3D着陆(骑骑)
	c3d_onland_petsit,			// 27.	3D着陆(骑坐)
	c3d_onland_petstand,		// 28.	3D着陆(骑站)
	c3d_onland_pethang,			// 29.	3D着陆(骑挂)
	c3d_onland_petside,			// 30.	3D着陆(侧骑)
	c3d_onland_bind,			// 31.	3D着陆(绑定类法宝)
	c3d_onland_unbind,			// 32.	3D着陆(绑定类法宝)
	c3d_onland_big,				// 33.	3D着陆(绑定类法宝)
	c3d_onland_small,			// 34.	3D着陆(绑定类法宝)
	c3d_onlandrun,				// 35.	3D着陆跑
	c3d_onlandrun_shoulder,		// 36.	3D着陆跑(背)
	c3d_onlandrun_lift,			// 37.	3D着陆跑(举)
	c3d_onlandrun_resist,		// 38.	3D着陆跑(抗)
	c3d_onlandrun_drag,			// 39.	3D着陆跑(拖)
	c3d_onlandrun_petride,		// 40.	3D着陆跑(骑骑)
	c3d_onlandrun_petsit,		// 41.	3D着陆跑(骑坐)
	c3d_onlandrun_petstand,		// 42.	3D着陆跑(骑站)
	c3d_onlandrun_pethang,		// 43.	3D着陆跑(骑挂)
	c3d_onlandrun_petside,		// 44.	3D着陆跑(侧骑)
	c3d_onlandrun_bind,			// 45.	3D着陆跑(绑定类法宝)
	c3d_onlandrun_unbind,		// 46.	3D着陆跑(绑定类法宝)
	c3d_onlandrun_big,			// 47.	3D着陆跑(绑定类法宝)
	c3d_onlandrun_small,		// 48.	3D着陆跑(绑定类法宝)

	c3d_onpickup,				// 49.	3D拾取动作
	c3d_onquest_1,				// 50.	任务状态1
	c3d_onquest_2,				// 51.	任务状态1
	c3d_onquest_3,				// 52.	任务状态1
	c3d_onquest_4,				// 53.	任务状态1
	c3d_onquest_complete,		// 54.	任务完成

	c3d_onborn,					// 55.	角色出生

	c3d_onholdweapon,				// 56.	进入战斗执械
	c3d_onhideweapon,				// 57.  离开战斗不执械

	c3d_Total,
};

enum Action_TreasureCase_3DEvent
{
	c3d_TC_Open,		// 0.	宝箱打开动作
	c3d_TC_Opened,		// 1.	宝箱打开状态
	c3d_TC_Close,		// 2.	宝箱关闭动作
	c3d_TC_Closed,		// 3.	宝箱关闭状态
	c3d_TC_Tote,		// 4.	宝箱携带动作
	c3d_TC_Death,		// 5.	宝箱死亡动作
};
// 需要3D Show出来的玩家装备类型
// 注意：下面枚举顺序要固定，在int KPlayer::GetAppearData(char* pAppearData)会用到这个顺序
enum ENUM_EQUIPMENT_3DSHOW
{
	enumEE3_Weapon	= 0,										//01.	武器
	enumEE3_Helm,												//02.   头部
//	enumEE3_Mantle,												//03.	背部（披风）
	enumEE3_Clothing,											//04.	衣服
	enumEE3_Shoes,												//05.	足部
	enumEE3_FashionHead,										//06.	时装头饰
	enumEE3_FashionCloth,										//07.	时装衣服
//	enumEE3_FashionShoes,										//08.	时装鞋子
	enumEE3_Count,												//09.	数量
};

// 为取出角色（玩家、法宝）的通用信息
enum ENUM_CHAR_TYPE
{
	enum_CT_MainPlayer = 0,	// 玩家
	enum_CT_MagicBaby1,	// 法宝1
	enum_CT_MagicBaby2,	// 法宝1
	enum_CT_MagicBaby3,	// 法宝1
	enum_CT_MagicBaby4,	// 法宝1
	enum_CT_MagicBaby5,	// 法宝1
	enum_CT_MagicBaby6,	// 法宝1
	enum_CT_MagicBaby7,	// 法宝1
	enum_CT_MagicBaby8,	// 法宝1

	enum_CT_Observer_MagicBaby0,	// 法宝0
	enum_CT_Observer_MagicBaby1,	// 法宝0
	enum_CT_Observer_MagicBaby2,	// 法宝0
	enum_CT_Observer_MagicBaby3,	// 法宝0
	enum_CT_Observer_MagicBaby4,	// 法宝0
	enum_CT_Observer_MagicBaby5,	// 法宝0
	enum_CT_Observer_MagicBaby6,	// 法宝0
	enum_CT_Count,
};

// 法宝子操作
enum MAGIC_BABY_OPERATION_INDEX
{
	// 增加一个法宝
	MBOI_ADD_ONE_MAGIC_BABY,
	// uParam = (SC_AddMagicBaby*)lpBuff
	// lParam = 0

	// 修改一个法宝属性
	MBOI_UPDATE_ONE_MAGIC_BABY,
	// uParam = (CS_UpdateMagicBaby* || SC_UpdateMagicBaby*)lpBuff
	// lParam = 0

	// 假删一个法宝属性
	MBOI_DELETE_ONE_MAGIC_BABY,
	// uParam = (CS_DeleteMagicBaby* || SC_DeleteMagicBaby*)lpBuff
	// lParam = 0	

	// 对一个法宝进行镶嵌
	MBOI_INLAY_ONE_MAGIC_BABY,
	// uParam = (CS_InlayMagicBaby* || SC_InlayMagicBaby*)lpBuff
	// lParam = 0	

	// 对一个法宝进行镶嵌时，放入一个物品
	MBOI_INLAY_PUT_ITEM_MAGIC_BABY,
	// uParam = (CS_InlayPutItemMagicBaby* || SC_InlayPutItemMagicBaby*)lpBuff
	// lParam = 0	

	// 对一个法宝进行镶嵌时，取出一个宝石
	MBOI_INLAY_GET_BACK_ITEM_MAGIC_BABY,
	// uParam = (CS_InlayGetBackItemMagicBaby* || SC_InlayGetBackItemMagicBaby*)lpBuff
	// lParam = 0	

	// 对一个法宝进行镶嵌时，交换宝石
	MBOI_INLAY_SWAP_ITEM_MAGIC_BABY,
	// uParam = (CS_InlaySwapItemMagicBaby* || SC_InlaySwapItemMagicBaby*)lpBuff
	// lParam = 0

	// 对一个法宝进行镶嵌时，各返回结果
	MBOI_INLAY_RESULT_MAGIC_BABY,
	// uParam = (CS_InlayResultMagicBaby* || SC_InlayResultMagicBaby*)lpBuff
	// lParam = 0

	// 更新一个法宝的修为等级
	MBOI_UPDATE_MAGIC_BABY_FAITH_LEVEL,
	// uParam = (SC_MagicBaby_Faith_Level_Update*)lpBuff
	// lParam = 0

	// 更新一个法宝的修为值
	MBOI_UPDATE_MAGIC_BABY_FAITH_EXP,
	// uParam = (SC_MagicBaby_Faith_Value_Update*)lpBuff
	// lParam = 0

	MBOI_UPDATE_MAGIC_BABY_TRADE,

	// 更新一个法宝的特有属性
	MBOI_UPDATE_MAGIC_BABY_ATTR,
	// uParam = (SC_MagicBaby_ATTR_Change*)lpBuff
	// lParam = 0
};

// 状态机离开某状态的原因
enum eLeaveStateReason
{
	enumLSR_Normal = 0,	//正常离开
	enumLSR_Passive,	//被动离开
	enumLSR_Active,		//主动离开
};

// Skill结束原因
enum eSkillFinishReason
{
	enumSFR_Success			= 0,	//正常离开
	enumSFR_UseIllegalSkill	= 1,	//使用了非法技能
	enumSFR_Interrupt		= 2,	//打断
	//TODO: 增加异常离开原因
};

enum eSkillLoadingType
{
	enumSLT_Charge			= 0,	//蓄力
	enumSLT_Prepare			= 1,	//吟唱
	enumSLT_Channel			= 2,	//引导
	enumSLT_FormulaMade		= 3,	//配方制作
};

// 物品相关所有组类型
enum ENUM_BLOCK_ALL_TYPES
{
	enumBlock_Inviad = 0,							// 无效的格子，可能用做丢失使用
	enumBlock_BagNormal,							// 背包普通物品
	enumBlock_BagQuest,								// 背包任务物品
	enumBlock_BagMat,								// 背包材料物品	
	enumBlock_BagEquip,								// 背包装备物品
	enumBlock_BagPet,								// 宠物背包栏
	enumBlock_BankNormal,							// 仓库普通物品
	enumBlock_BankMat,								// 仓库材料物品
	enumBlock_BagSoldItem,							// 卖到商店的物品

	enumBlock_SlotCount,							// 存贮物品格子类型个数
};

enum ENUM_MONEY_DEFINE
{
	emBegin = 1,
	emMainMoney = emBegin,	// 身上游戏金钱
	emOffMoney,				// 绑定银票
	emCash1,				// 元宝1
	emCash2,				// 赠送
	emCash3,				// 积分
	emEnd,					// 总数
};

//玩家物品获得或失去来源位置
enum ENUM_ITEM_MODE
{
	em_Item_Inviald = 0,
	em_Item_GetByPickup,
	em_Item_GetByMoveItem,
	em_Item_Delete,
	em_Item_GetByQuest,
	em_Item_GetByPlayerInit,
	em_Item_GetByTrade,
	em_Item_Count,
};
enum ENUM_MOVESTATE_DEFINE
{
	msSuccess = 0,
	msNoMoneyRelive,		// 金钱不够，不能复活
	msDead,					// 死亡
	msRelive,				// 复活
	msForceSetPosition,		// 仅强设位置
	msNoItemRelive,			// 没有对应的复活石 提示是否花元宝
	msNoIBRelive,			// 原地复活时没有对应的IB
	msNoPerfectItemRelive,	// 没有对应的完美复活石 提示是否花元宝
	msNoPerfectIBRelive,	// 完美原地复活时没有对应的IB
	msPerfectReliveCD,		// 完美原地复活CoolDown中
};
// 任务物品添加、删除、检查数量宏
#define CFG_MAX_CHECK_QUEST_ITEM_COUNT	10

// 交通系统
#define	MAX_TRANSPORT_CONFIG	256
#define	MAX_TRAFFIC_ROUTE_COUNT	256
#define MAX_TRAFFIC_QUEUE_COUNT	256	// 全世界一共256个车站，包括每条双飞路线使用一个车站，每个bus使用n个车站

enum ENUM_TRAFFIC_OPERATOR
{
	etGetList = 1,
	etLoadFile,
	etSaveFile,
	etAddRoute,
	etRemoveRoute,
	etGetRouteInfo,
	etUpdateRouteInfo,
	etAddWaypoint,
	etGetWaypoint,
	etUpdateWaypoint,
	etRemoveWaypoint,
	etGetRouteCount,
};

/// 鼠标状态
enum ENUM_MOUSE_STATE
{
	enumMouseNormal	=	0,						// 一般状态
	enumMouseBuy,								// 买状态
	enumMouseSail,								// 卖状态
	enumMouseSplit,								// 拆分状态
	enumMousePick,								// 拾取
	enumMousePickBox,							// 开启宝箱
	enumMouseFight,								// 战斗表示
	enumMouseOperator,							// 开启机关
	/// 待补充
};

enum ENUM_HATE_TO_LIMIT_TYPE
{
	enum_NotToLimit = 0,
	enum_To_Max,
	enum_To_Min,

	enum_To_Limit_Type_Count,
};
enum ENUM_MESSAGE_TYPE
{
	emChat = 1,
	emSystem,
	emHead,
	emUI,
	emOther
};

// 鼠标选中Character的方式
enum eMouseCharTargetType
{
	enumMCTT_LeftClick = 1,		//左击选中
	enumMCTT_RightClick,		//右击选中
	enumMCTT_MouseOver,			//鼠标放在Character身上选中
};

enum ENUM_3D_NAME_STATE_TYPE
{
	enumPassivenessNotInFight = 0,
	enumPassivenessInFight,
	enumInitiative,

	enum_NameStateTypeCount,
};

enum ENUM_AUTOMOVE_ERROR_CODE
{
	enum_AM_Succeed = 1,
	enum_AM_Error_CharacterIsNull = 0,
	enum_AM_Error_CharacterIsDead = -1,
	enum_AM_Error_PointIsObstacle = -2,
	enum_AM_Error_StateTransFault = -3,
};

enum ENUM_SETCLIENT_INFO_TYPE
{
	enum_SCIT_Invilad = 0,
	enum_SCIT_VisibleRangeSquared,
	enum_SCIT_Count,
};

enum ENUM_CUSTON_WIN_TYPE
{
	enum_CW_Default = 0,
	enum_CW_Dialog,
	enum_CW_Singleton_Tootip,
	enum_CW_Singleton_Click_Tooltip,
	enum_CW_Singleton_TootipCompare1,
	enum_CW_Singleton_TootipCompare2,
	enum_CW_CustomParent,
	enum_CW_Count,
};

enum ENUM_DIALOG_INTERACTIVE_TYPE
{
	enum_IT_UnKnow = 0,
	enum_IT_InterActive_Change,
	enum_IT_ChangePos,
	enum_IT_ChangeState,
	enum_IT_ChangeModel,
	enum_IT_ADD_Character,
	enum_IT_Del_Character,
	enum_IT_QuestEvent,
	enum_IT_Act_Client,
	enum_IT_Act_Server,
	enum_IT_Bus,
	enum_IT_TreasureCaseOp,
	enum_IT_ChangeToTreasureCase,
	enum_IT_SpecialEvent,
	enum_IT_Trans,
	enum_IT_Talk,
	enum_IT_InterActive_Event,

	enum_IT_Count,
};

//Linux下有点问题，更换成其他函数了 在KCommonStruct里面，回头删 20091024 Calvin
//#define randbignum(num) ((rand()*32768 + rand()) % (num))
//#define randsmallnum(num) (rand() % (num))

// 玩家ID的最大上限，玩家数量绝对不能超过这个数
#define PLAYER_ID_LIMITED			50000000
#define NPC_ID_LIMITED				10000000
#define GROUND_ID_LIMITED			200000000
#define TRANSPORT_ID_LIMITED		100000000
#define TREASURECASE_ID_LIMITED		20000000

enum
{
	PLAYER_ID_BASE_START		=   1,
	PLAYER_ID_BASE_END			=	PLAYER_ID_LIMITED,

	MOVABLESHOP_ID_BASE_START,
	MOVABLESHOP_ID_BASE_END		=	2*PLAYER_ID_LIMITED,

	PET_ID_BASE_START,
	PET_ID_BASE_END				=	3*PLAYER_ID_LIMITED,


	NPC_ID_BASE_START,						// 正常的服务器控制的npc开始ID
	TEMP_NPC_ID_BASE_START		=  (NPC_ID_BASE_START + NPC_ID_LIMITED - 1000),	// 只有客户端控制的npc开始ID
	NPC_ID_BASE_END				=  (NPC_ID_BASE_START + NPC_ID_LIMITED),		// 所有npc结束的实例ID

	GROUNDITEM_ID_BASE_START,
	GROUNDITEM_ID_BASE_END		= (GROUNDITEM_ID_BASE_START + GROUND_ID_LIMITED),

	TRANSPORT_ID_BASE_START,
	TRANSPORT_ID_BASE_END		= (TRANSPORT_ID_BASE_START + TRANSPORT_ID_LIMITED),

	TREASURE_CASE_ID_BASE_START,
	TREASURE_CASE_ID_BASE_END	= (TREASURE_CASE_ID_BASE_START + TREASURECASE_ID_LIMITED),

};
#define	FROM_NET		10
#define	FLY_MIN_HEIGHT	0.3f

const int C_MIN_PK_LEVEL = 10;
enum enumNameColor
{
	eColor_Unknown,
	eColor_White,	// 中立
	eColor_Green,	// 友好
	eColor_Red,		// 敌对
	eColor_Purple,	// 紫名
	eColor_Yellow,	// 被动怪物
	eColor_Croci,	// 开PK模式下的同国
	eColor_MainPlayer,
};

enum{
	RIDE_INVALID = 0,	// 错误
	RIDE_SOMEONE,		// 骑乘
	RIDE_NONE,			// 取消骑乘
	RIDE_CHANGE_STATE,	// 更改骑乘状态
	RIDE_CHANGE_HEIGHT,	// 更改骑乘高度
	RIDE_CLOSE_2RIDE,	// 解除双人骑乘
	RIDE_START_HEIGHT,	// 开始升空
	RIDE_STOP_HEIGHT,	// 停止升空
	RIDE_SYNC_HEIGHT,	// 同步其他玩家升空信息
};

/*
 * 鼠标指针放到Character身上时候的类型
 */
enum ENUM_MOUSE_POINTER_TYPE_TO_3D
{
	/*
	enumMPTT3D_CanNotSelect	= 0,	//无法被选中
	enumMPTT3D_Selectable	= 1,	//可被选中的
	enumMPTT3D_Openable		= 2,	//可被打开
	enumMPTT3D_Closable		= 3,	//可被关闭
	enumMPTT3D_Totable		= 4,	//可被携带
	enumMPTT3D_Attackable	= 5,	//敌人
	enumMPTT3D_Supportable	= 6,	//友军
	enumMPTT3D_CanNotSkill	= 7,	//中立
	enumMPTT3D_Talkable		= 8,	//可对话的
	*/

	//以下枚举跟外部数据有关，请不要更改顺序
	enumMPTT3D_CanNotSelect	= 0,	//无法被选中
	enumMPTT3D_Selectable	= 1,	//可被选中的
	enumMPTT3D_Openable		= 2,	//可被打开
	enumMPTT3D_Closable		= 3,	//可被关闭
	enumMPTT3D_Totable		= 4,	//可被携带
	enumMPTT3D_ActiveAttackable	= 5,	//  主动敌人
	enumMPTT3D_PassivityAttackable  = 6,    // 被动敌人
	enumMPTT3D_Supportable	= 7,	//友军
	enumMPTT3D_CanNotSkill	= 8,	//中立
	enumMPTT3D_Talkable		= 9,	//可对话的
	enumMPTT3D_HaveQuest    = 10,     //可接任务
	enumMPTT3D_GroudItem     = 11,    //可拾取掉落物
	enumMPTT3D_FinishQuest	= 12,	  //可交任务
	
};

enum ENUM_AUTOMOVE_AND_FIND_TARGET_TYPE
{
	enum_AAFTT_Unknow = 0,
	enum_AAFTT_ID,
	enum_AAFTT_TypeID,
};

/*
 * 查询目标点在自动寻路的时候的类型
 */
enum ENUM_TARGET_POS_TYPE_IN_AUTOMOVING
{
	enumTPTIA_Reachable		= 1,	//目标可达
	enumTPTIA_Obstacle		= 2,	//目标是障碍
	enumTPTIA_Unknown		= 3,	//未知区域
};

// 好友相关的操作类型
enum eFsOpera
{
	enum_FSO_AddAcq,
	enum_FSO_ModifyAcq_Notes,
	enum_FSO_AddFriend,
	enum_FSO_AddEnemy,
	enum_FSO_AddRefuse,
	enum_FSO_RemoveAcq,
	enum_FSO_RemoveFriend,
	enum_FSO_RemoveEnemy,
	enum_FSO_RemoveRefuse,
	enum_FSO_Query_Equip,
	enum_FSO_UpdateSignature,
};

// 好友相关操作的结果
enum eFsOperaResult
{
	enum_FSOR_Success,
	enum_FSOR_Busy,
	enum_FSOR_Already_Exists,
	enum_FSOR_No_Player,
	enum_FSOR_NoPlayer_Or_NotOnline,
	enum_FSOR_Peer_Refuse,
	enum_FSOR_Acq_Full,
	enum_FSOR_Friend_Full,
	enum_FSOR_Enemy_Full,
	enum_FSOR_Refuse_Full,
	enum_FSOR_Try_Later,
	enum_FSOR_Is_Self,
	enum_FSOR_Other,
};

enum enumPKSwitch
{
	ePKSwitch_Invalid   = 0,
	ePKSwitch_Guild     = 1,	// 帮派
	ePKSwitch_Nation    = 2,	// 国战
	ePKSwitch_World     = 3,	// 全体
};

enum ENUM_FORCE_RELATIONSHIP
{
	enumFR_Friend		= 0,
	enumFR_Enemy		= 1,
	enumFR_Neutrality	= 2,
};

enum eNationarity
{
	enumNation_None	= 0,
	enumNation_ShuiGuo = 1,
	enumNation_XueGuo  = 2,
	enumNation_ShanGuo = 3,
	enumNation_LingGuo = 4,
	enumNation_GuiGuo  = 5,
	enumNation_MoYu    = 6,
	enumNation_Max
};


enum ITEM_EVENT_TYPE
{
	eItemEvent_Type_Start = 0,
	eItemEvent_Type_DisappearByGameTimeUpdate,	//物品定时消失By现实时间，根据真实时间判断
	eItemEvent_Type_DisappearByRealTimeUpdate,	//物品定时消失By游戏时间，下线后不计算	
	eItemEvent_Type_EverydayDisappear,			//每日延迟到时消失
	eItemEvent_Type_EveryweekDisappear,			//每周延迟到时消失
	eItemEvent_Type_EverymonthDisappear,		//每月延迟到时消失
	eItemEvent_Type_End,
};



//////////////////////////////////////////////////////////////////////////
/*
<参数1>：社会关系
0=无限制
1=本小队
2=本堂
3=本帮
4=本国
5=配偶
6=师徒
*/
enum ENUM_FILTER_CONDITION
{
	enum_Filter_Condition_Any = 0,
	enum_Filter_Condition_Team,			//	本小队
	enum_Filter_Condition_Tang,			//	本堂
	enum_Filter_Condition_Guild,		//	帮派
	enum_Filter_Condition_Nation,		//	国家
	enum_Filter_Condition_Consort,		//	夫妻
	enum_Filter_Condition_Master,		//	师徒
	enum_Filter_Condition_GuildMaster,	//  帮主
	enum_Filter_Condition_GuildSubMaster,//  唐主
	enum_Filter_Condition_GuildPost,	//  官员
	enum_Filter_Condition_Count,
};

// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 

// 配置文件路径及后缀
#ifdef _CLIENT 
#	define SETTING_DIR			"..\\..\\data\\configs\\"	// 当前文件目录
#else
#	define SETTING_DIR		"../Settings/"	// 配置文件目录
#endif

#if defined(WIN32) || defined(WINDOWS)
#	define SERVER_DIR			"Server/"
#	define CLIENT_DIR			"Client\\"
#else
#	define SERVER_DIR			"Server/"
#	define CLIENT_DIR			"Client/"
#endif

//表现相关
#ifdef _CLIENT
//收妖特殊特效的事件id
#	define EXTRACT_GHOST_EVENT_ID	2001
#endif // _CLIENT

#define MAX_WORLD_DROP_COUNT		8		// 每个World最多有8个世界掉落组	
#define MAX_TIME_WORLD_DROP_COUNT	16		// 每个Time最多有8个世界掉落组	
#define MAX_DROP_GROUP_COUNT		8		// 每个怪物最多有8个普通掉落组
#define MAX_DROP_ITEM_COUNT			4096	// 所有掉落组中物体数量的总和
#define MAX_WORLD_DROP_ITEM_COUNT	8192	// 所有世界掉落组中物体数量的总和

#define MAX_DROP_ITEM_SGC_COUNT				8		// SGC筛选列表长度
#define MAX_DROP_ITEM_QUALITY_COUNT			8		// 品质筛选列表长度

#define MIN_TELEPORT_DISTANCE		262144	// 512*512

enum{
	MagicWeaponClick,
	MagicWeaponIsInQueue,
};

enum SECURICA_RERESULT
{
	smAddSuccess,
	smAddRepeat,	// 重复
	smAddFailed,	// 失败
	smAddLevelLimit,// 等级不够
	smAddNoMoney,	// 钱不够
	smBeBroken,		// 镖车坏
	smAway,			// 远离
	smNear,			// 找回
	smLost,			// 丢失
	smAddToClient,	// 通知客户端，镖车领到了
	smRemoveFromClient,	// 通知客户端，镖车删除了
	smNoRide,		// 不可以骑乘
};

// 摆摊
enum MovableShop_State
{
	enum_MovableShop_Close_State = 0,
	enum_MovableShop_Prepare_State,
	enum_MovableShop_Open_State,
};

enum MovableShop_Type
{
	enum_MovableShop_Buy_Type = 0,
	enum_MovableShop_Sell_Type, 
};

//enum GuildPplType
//{
//	enumNGP_Common,
//	enumNGP_SubMaster,
//	eGuildKeeper,
//	eGuildMascot,
//	enumNGP_Master,
//
//};

#define DEF_DEFAULT_DIALOG_FRAME_ID		(899999)
#define DEF_DEFAULT_ITEM_TOOLTIP		(900000)
#define DEF_DEFAULT_SKILL_TOOLTIP		(900002)
#define DEF_DEFAULT_BUFF_TOOLTIP		(900003)
#define DEF_DEFAULT_DROPITEM_TOOLTIP	(900004)
#define DEF_DEFAULT_PLAYER_TOOLTIP		(900005)
#define DEF_DEFAULT_NPC_TOOLTIP			(900006)
#define DEF_DEFAULT_TREASURE_TOOLTIP	(900007)
#define DEF_DEFAULT_SKILLRESULT			(900008)
#define DEF_DEFAULT_PLAYEREXP			(900009)
#define DEF_DEFAULT_PLAYERHONOR			(900010)
#define DEF_DEFAULT_TALENTS				(900011)
#define DEF_DEFAULT_PLAYERFAITHEXP		(900012)
#define DEF_DEFAULT_PRACTICE			(900013)
#define DEF_DEFAULT_MB_FIGHT			(900014)
#define DEF_DEFAULT_ACTION_TOOLTIP		(900015)

#define KTEAM_MEMBER_MAX 6

enum enumKTeamError
{
	enumKTeam_Ok = 0,
	enumKTeam_Full,
	enumKTeam_NoPopedom,
	enumKTeam_TeamExist,
	enumKTeam_MemberExist,
	enumKTeam_TeamInExist,
};

enum KTeamPopedom
{
	enumKTeam_Member = 0,
	enumKTeam_MemberEx,
	enumKTeam_Captain
};

enum KTeamStat
{
	enumKTeamStat_Normal = 0,
	enumKTeamStat_CreateIng,
	enumKTeamStat_Locked,
};

enum enumKTeam_Member_State
{
	enum_Team_Member_Outsight = 0,
	enum_Team_Member_Near,
	enum_Team_Member_Offline,
};

enum TEAM_RULE_NAME
{
	TR_EXP = 0,
	TR_ITEM,
	TR_ADD,
	TR_UNIT,
	TR_DICELEVEL,
	TR_OCCUPATION,
};

enum ENUM_SERVERLIST_SHOW_STATE
{
	enumESSS_Maintenance = 0,	// 维护，无法登陆
	enumESSS_Fluency,			// 流畅
	enumESSS_Busy,				// 繁忙
	enumESSS_Full,				// 爆满
	enumESSS_NoLogin,			// 爆满，无法登陆
};


//--------------- 法宝孵化
// 每个玩家最多孵化2只法宝
#define MAX_EGGS_PER_PLAYER		1
// 每页只显示7个法宝蛋
#define MAX_EGGS_PER_PAGE		7
// 法宝孵化时间
#define MIN_EGG_ADULT_TIME		1800
// 法宝孤儿时间 6天
#define MIN_EGG_FREE_TIME		(6*24*3600)
// 法宝丢弃时间 7天
#define MIN_EGG_LOST_TIME		(7*24*3600)

enum MAGIC_BABY_EGG_ERROR
{
	mbeSuccess,
	mbeError,		// 未指定错误
	mbeMBFull,		// 法宝已经满了
	mbeNotYours,	// 不是你的
	mbeNotAdult,	// 没有孵化好

	mbeNoEgg,		// 没有对应的蛋
	mbeTooMuch,		// 孵化数量超过上限
	mbeAddFailed,	// 添加孵化失败
	mbeAddSuccess,	// 添加成功
	mbeGetList,		// 获取列表
	mbeMyList,		// 获取自己列表
	mbePutEgg,
	mbeGetBackEgg,
	mbeGetBackSuccess,
	mbeAllList,		// 所有列表
	mbeOnPageChanged,
	mbeAdult,		// 孵化成功
	mbeMyEggList,   // 获取服务器上自己列表
	mbeGetBackMySuccess, //那别人的法宝成功

	mbMagicEggOpen,
	mbMagicEggGetBack,
	mbMagicEggGetOthers,
};

enum MAIL_OPERATOR
{
	s_nMOPER_Begin = 0,

	s_nMOPER_Show_MailSystem,
	s_nMOPER_Read,
	s_nMOPER_Write,
	s_nMOPER_PageUp,
	s_nMOPER_PageDown,
	s_nMOPER_Del_Curr,		//by row
	s_nMOPER_Del_Readed,	//by read state
	s_nMOPER_Del_Reading,	//by mail id
	s_nMOPER_WriteBack,
	s_nMOPER_Fetch_Attachment,
	s_nMOPER_ReturnToSender,
	s_nMOPER_Show_FriendList,
	s_nMOPER_Append_Attachment,
	s_nMOPER_Close_MailSystem,

	s_nMOPER_End,
};

enum MessageBoxStyleEnumLogic
{
	// 对话框风格

	MB_NONE_L = 0,		// 不显示任何按钮
	MB_OK_L,			// 显示中间按钮，点击OK按钮返回MBRE_CENTER
	MB_CANCEL_L,		// 显示中间按钮，点击OK按钮返回MBRE_CENTER
	MB_OKCANCEL_L,		// 显示左右按钮，点击OK按钮返回MBRE_LEFT	点击Cancel返回MBRE_RIGHT
	MB_OKCANCELRETRY_L,	// 显示左中右按钮，点击OK按钮返回MBRE_LEFT	点击Cancel返回MBRE_CENTER	点击Retry返回MBRE_RIGHT
};

enum NpcOwnerService
{
	nosNpc,
	nosSecuricar,
};

enum MagicBabyEvent
{
	eGetMagicBaby,
	eDecFriendiness,
	eFight,
	eFightActive,
};

enum MagicBabyItem
{
	eMagicBabyBar,		// 法宝道具栏道具
	eMagicBabyExp,		// 法宝道具经验
};

// 交通系统
#define	TRANSPORT_MAX_PASSENGER		20

#define  MAGIC_BABY_FUSE_VALUE	65			//法宝融合值
#define  MAGIC_BABY_FUSE_VALUE_CAL	66		//法宝计算值

#define	MAGIC_BABY_BAR_BASE		3
#define MAGICBABY_BAR_EX		4

enum CharacterMoveParam
{
	ecMoveMainPlayer,
	ecMoveFromNet,
};

#define MB_FUSE_DE_LEVEL		64
#define MB_FUSE_DE_FUSE			15 / 100
#define MB_FUSE_DE_WX			62
#define MB_FUSE_MONEY			68
#define MB_FUSE_INCREASE		66
#define MB_FUSE_LIMIT			65

enum MB_WX
{
	enum_GOLD,
	enum_WOOD,
	enum_WATER,
	enum_FIRE,
	enum_EARTH,
	enum_COUNT,
};

enum MB_ITEM_FUSE
{
	enum_ITEM_FUSE_BEGIN  = 10560,
	enum_ITEM_FUSE_1 = enum_ITEM_FUSE_BEGIN,
	enum_ITEM_FUSE_2,
	enum_ITEM_FUSE_3,
	enum_ITEM_FUSE_4,
	enum_ITEM_FUSE_5,
	enum_ITEM_FUSE_6,
	enum_ITEM_FUSE_7,
	enum_ITEM_FUSE_8,
	enum_ITEM_FUSE_END,
};

enum MB_FUSE_OPERATION
{
	enum_NEED_MONEY,
	enum_NEED_UPDATE,
	enum_NOTHING_CREATE,
	enum_BAG_FULL,
	enum_CURRENT_MB,
	enum_NEED_COUNT,
};
//注意宏中使用括号避免产生歧义
#define GET_FUSE_LIMIT(level) ((level) * (level) * KGlobalParameter::Instance()->Get(MB_FUSE_LIMIT))

//装备技能的格子号
#define EQUIP_SKILL_POS 100

// 帮派分堂人数
#define BRANCH_COUNT_MAX			20
#define OTHER_EMPLOYEE				10

// 交易时别人的ICON
#define TRADE_OTHER_ICON_ID			8

// 法宝技能组的最大数
#define MB_SKILLGROUP_MAX			2

#define MB_TALENT_MAX				6

#define MB_TALENTNAME_MAX			16

#define PET_ICON_MAX			64

#define MB_TALENTATTR_MAX			5

#define MB_TALENTDES_MAX			128

// 灵通开天赋上线
#define MB_RELATION_OPENGENIUS		200

#define MB_RELATION_ZERO			1

#define MB_RELATION_LOWEST			0

#define MB_TALENT_ABILITY			128

enum E_SAVE_EVENTS
{
	E_SAVE_EVENT_BEGIN = 0,
	E_SAVE_EVENT_TELEPORT = E_SAVE_EVENT_BEGIN,			// 回城保存的数据
	E_SAVE_EVENT_END,
};

#define MAIL_INVALID_RECEIVER		-1

///////////////////////////////邮件系统枚举//////////////////////////
enum MAIL_READ_STATE
{
	MAIL_READ_STATE_UNREAD = 0,
	MAIL_READ_STATE_READED,
};

enum MAIL_RETURN_TYPE
{
	MAIL_RETURN_TYPE_NORMAL = 0,
	MAIL_RETURN_TYPE_OVERTIME,
};

enum MAIL_TYPE
{
	MAIL_TYPE_NORMAL = 0,
	MAIL_TYPE_GUILD,
	MAIL_TYPE_ATTACHMENT,
	MAIL_TYPE_SYSTEM,
};

enum MAIL_DELETE_OPTION
{
	MAIL_DELETE_CURR = 0,
	MAIL_DELETE_READED,
};

enum MAIL_ATTACH_ITEM_TYPE
{
	MAIL_ATTACH_KITEMBASE = 0,
	MAIL_ATTACH_ITEMID,
};

enum MAIL_RSP_RESULT_TYPE
{
	errM_Succeed = 0,
	errM_Net_Error,				// 网络出错
	errM_Db_Error,				// 数据库出错
	errM_Query_In_Part,			// 后续还有邮件未传送
	errM_Query_Accomplish,		// 后续再无邮件未传送
	errM_Send_No_Space,			// 收件人信箱已满
	errM_Send_No_Space_RTS,		// 收件人信箱已满，已退给发件人
	errM_Send_No_Receiver,		// 收件人不存在
	errM_Send_No_Receiver_RTS,	// 收件人不存在，已退给发件人
	errM_Send_No_Enough_Money,	// 无法扣除足够的金钱
	errM_Send_Attachment_Oper,	// 附件物品操作出错
	errM_Delete_Fee_Nonzero,	// 带有附件并且需要提取费用的邮件，不能直接删除
	errM_Fetch_Bag_No_Space,	// 背包已满，无空间
	errM_Fetch_No_Enough_Money,	// 无法扣除足够的提取费用
	errM_RTS_Type_Unmatch,		// 此类邮件不支持退信
	errM_RTS_Type_No_response,	// 此类邮件无需回应
	errM_RTS_OK,				// 该邮件已退信
};

enum ENUM_CONVOKE_OP
{
	enum_COPT_Unknow,
	enum_COPT_Convoke,
	enum_COPT_CancelConvoke,
	enum_COPT_ReturnBack,
	enum_COPT_CancelReturnBack,
	enum_COPT_Count,
};

enum ENUM_CONVOKE_RESULT
{
	enumCR_Unknow = 0,
	enumCR_Succeed,
	enumCR_Succeed_Back,
	enumCR_ConvokeInfoNotFound,
	enumCR_Obstacle,
};


enum eGearSetBuff
{
	enum_GearSet_Buff1 = 0,	
	enum_GearSet_Buff2,		
	enum_GearSet_Buff3,		
	enum_GearSet_Buff4,
	enum_GearSet_Buff5,		
	enum_GearSet_Buff6,		
	enum_GearSet_Buff7,		
	enum_GearSet_Buff8,
	enum_GearSet_Buff9,		
	enum_GearSet_Buff10,	
	enum_GearSet_Buff11,	
	enum_GearSet_Buff12,
	enum_GearSet_Buff13,	
	enum_GearSet_Count
};

enum eWorldObjectKind
{
	wokWorldObj		= 1<<0,
	wokGroundItem	= 1<<1,
	wokMovableShop	= 1<<2,
	wokCharacter	= 1<<3,
	wokNpc			= 1<<4,
	wokPlayer		= 1<<5,
	wokTreasureCase	= 1<<6,
	wokTransport	= 1<<7,
	wokPet			= 1<<8,
};


enum ENUM_MB_SKILL_GROUP
{
	eMBLearnSkillGroupSuccess,
};

enum RideState
{
	RideNone,
	RideDriver,
	RidePassenger
};

enum ENUM_MB_LL_PZ
{
	eLLWhite,
	eLLGreen,
	eLLBlue,
	eLLPurple,
};

//天赋点的当前状态值
enum TALENT_POINT_STATE
{
	enumTPS_Normal	= 0,		//可用
	enumTPS_Invalid,			//无效
	enumTPS_NoPointLeft,		//无剩余天赋点
	enumTPS_Full,				//加满了
	enumTPS_NeedParent,			//需要加父天赋才可用
	enumTPS_NotEnoughPtInSub,	//当前投入的天赋不足
	enumTPS_RealmRestrict,		//修真不足
};


enum ENUM_BILLING_ITEM_SCHOOL
{
	//这个定义在脚本里直接用的数字，所以需要添加时请在最后添加
	enum_Billing_Item_Free = 0,		// 免费领取
	enum_Billing_Item_1,			// 商城界面的8个分类
	enum_Billing_Item_2,			
	enum_Billing_Item_3,			
	enum_Billing_Item_4,			
	enum_Billing_Item_5,			
	enum_Billing_Item_6,			
	enum_Billing_Item_7,			
	enum_Billing_Item_8,			
	enum_Billing_Item_Hot,			// 热销
	enum_Billing_Item_Search,		// 通过查找添加出来的
	enum_Billing_Item_BagHot,		// 背包热销
	enum_Billing_Item_MagicBabyHot,	// 法宝热销
	enum_Billing_item_StrengthenHot,// 强化热销
	enum_Billing_Item_InlayHot,		// 镶嵌热销
	enum_Billing_Item_OpenHot,		// 开槽热销
	enum_Billing_Item_DestroyHot,	// 拆除热销
	enum_Billing_Item_StoneCompose,	// 宝石合成
	enum_Billing_Item_CharacterHot,	// 角色热销

};

// 前进的时候，最大一步走3秒时间（秒）
#define MAX_TIME_A_BIG_STEP		4
// 键盘行走的时候，提前多久告诉服务器新的目标点（毫秒）
#define PRE_TIME_NOTIFY			300
// 客户端每隔n秒通知服务器位置
#define POS_TIME_NOTIFY			300
// 召唤默认的限制时间	30秒
#define DEF_DEFAULT_CONVOKE_TIME	(30)

enum
{
	enum_Move_None,
	enum_Move_MainPlayer,
	enum_Move_MagicBaby,
};

/// Wishing Tree
enum WISH_TYPE
{
	enum_WISH_TYPE_NORMAL = 0,	//普通愿望
	enum_WISH_TYPE_HIGH,		//高级愿望
};

enum WISH_RESULT
{
	enum_WISH_RESULT_SUCCESS = 0,
	enum_WISH_RESULT_NET_ERROR,				// 网络出错
	enum_WISH_RESULT_DB_ERROR,				// 数据库出错
	enum_WISH_RESULT_ADD_ERROR,
	enum_WISH_RESULT_OVERTIME,
	enum_WISH_RESULT_NO_ENOUGH_PAPER,
	enum_WISH_RESULT_ADDWISH_RETURN,
};

enum WISH_SUPPORT_TYPE
{
	enum_WISH_SUPPORT = 0,		//顶
	enum_WISH_UNSUPPORT,		//踩
};

enum WISH_SEARCH_DIRECTION
{
	enum_WISH_FIRST_TIME = 0,	//查找第一次
	enum_WISH_LAST,				//向上查找
	enum_WISH_NEXT,				//向下查找
};


enum ENUM_FIRE_SKILL
{
	enum_Fire_Skill_E1 = 8005,
	enum_Fire_Skill_E2,
	enum_Fire_Skill_E3,
	enum_Fire_Skill_E4,
	enum_Fire_Skill_E5,
};

enum ENUM_FIRE_SKILL_FAR
{
	enum_Fire_Skill_E1_Far = 8010,
	enum_Fire_Skill_E2_Far,
	enum_Fire_Skill_E3_Far,
	enum_Fire_Skill_E4_Far,
	enum_Fire_Skill_E5_Far,
};

#define MAGICBABY_CHOOSE_ENEMY_LEVEL		1
#define MAGICBABY_CHOOSE_ENEMY_DISTANCE		1
#define MAGICBABY_CHOOSE_ENEMY_ANGLE		1	

#define MAGICBABY_NORMAIL_SKILL_ID			8001
#define MAGICBABY_NORMAIL_FAR_SKILL_ID		8002

#define MAGICBABY_GRID_MAX					6

enum ENUM_OPER_WISHING_TREE
{
	s_nWT_OPER_Begin = 0,

	s_nWT_OPER_ShowTree,
	s_nWT_OPER_AddWish,
	s_nWT_OPER_Search,
	s_nWT_OPER_Support,
	s_nWT_OPER_PageUp,
	s_nWT_OPER_PageDown,
	s_nWT_OPER_AddQuest,
	s_nWT_OPER_GetWish,

	s_nWT_OPER_End,
};

enum ENUM_WT_STATUS
{
	s_nWT_STATUS_None = 0,
	s_nWT_STATUS_Request,
};

#define DEF_GAMBING_OUTSIDE_COUNT	(16)
#define DEF_GAMBING_INSIDE_COUNT	(4)

enum ENUM_GAMBING_EVENT
{
	enumGambingEvent_Unknow = 0,
	enumGambingEvent_Init,
	enumGambingEvent_Begin,
	enumGambingEvent_End,
	enumGambingEvent_Count,
};


struct DynamicActivityOperation
{
	int nBuffID;
	int nActivityType;
	int nParam;
	int nPlayerID;
	int nItemID;
};

#define DEF_DYNAMIC_ACTIVITY	30000
#define DEF_MB_TRADE_OTHER		8


enum ENUM_GAMBING_ITEM_TYPE
{
	enumGT_Unknow = 0,
	enumGT_Item,
	enumGT_Event,
};
enum ENUM_GAMBING_EVENT_TYPE
{
	enumEUnknow = 0,
	enumEPointTriple,
	enumEFreeChoice,
	enumEGetAll,
	enumEInside,
};

enum ENUM_GLOBAL_EVENT_TYPE
{
	enumGET_Unknow = 0,
	enumGET_Run,
	enumGET_Idle,
	enumGET_Stop,

	enumGET_OnDay,
	enumGET_OnHour,
	enumGET_OnMinute,
	enumGET_OnSecond,

	enumGET_OnEnterGame,
	enumGET_OnLeaveGame,

	enumGET_OnEnterWorld,
	enumGET_OnLeaveWorld,

	enumGET_OnNpcRelive,
	enumGET_OnNpcDie,
	enumGET_OnNpcDisappear,
	enumGET_OnNpcRemoveAllBuff,

	enumGET_Count,
};

enum ENUM_TALENT_OPERATION
{
	enum_mb_talent_error,
	enum_mb_talent_success,
	enum_mb_talent_replace,
	enum_mb_talent_replace_type,
	enum_mb_talent_limit,
	enum_mb_talent_level,
};

enum ENUM_FALLOW_CHESS
{
	enum_fallow_chess_start,
	enum_fallow_chess_objPos,
	enum_fallow_chess_lottery,
	enum_fallow_chess_npcPos,

	enum_fc_apply_npc_dicing,
	enum_fc_apply_dicing,
	enum_fc_apply_use_card,
	enum_fc_apply_process_forth_card,
	enum_fc_apply_custom_step,
	enum_fc_apply_lottery,
	enum_fc_apply_custom_over,
};

enum ENUM_PLUG_QUIZ
{
	enum_plug_quiz_start,
	// GameServer -> HackServer
	enum_plug_quiz_monster_kill,
	enum_plug_quiz_finish_quest,
	enum_plug_quiz_enter_game,
	enum_plug_quiz_leave_game,
	enum_plug_quiz_enter_field,
	
	
	// HackServer -> GameServer
	enum_plug_quiz_reload_config,
	enum_plug_quiz_require_pq_info,
	enum_plug_quiz_update_pq_info,
	enum_plug_quiz_answer_info,

	// HackServer -> GameServer -> Client
	enum_plug_quiz_one_question,

	// Client -> GameServer -> HackServer
	enum_plug_quiz_change_question,
	enum_plug_quiz_answer,

	// GameServer -> HackServer
	enum_plug_quiz_update_info,

	enum_plug_quiz_end,
};

enum ENUM_MB_TALENT_GRID_TYPE
{
	MBTalent_Grid1 = 1,
	MBTalent_Grid2,
	MBTalent_Grid3,
	MBTalent_Grid4,
	MBTalent_Grid5,
	MBTalent_Grid6,
};

#define MB_TALENT_OPENGRID_ITEM 55617
#define MB_TALENT_FORGET_ITEM 55618

#define MAX_IP_BUFF_SIZE		24

enum ENUM_WALLOW_NOTIFY_TYPE
{
	e_Wallow_Unknown = 0,
	e_Wallow_Ramain_10min,
	e_Wallow_Ramain_5min,
};

enum ENUM_DYNAMIC_ACTIVITY
{
	enum_Guild_Rewards,
	enum_Guild_Vip,
};

#define MAX_SITE_ADDR_SIZE		128

// 国战
#define MAX_NATIONBF_NUMBER			100
#define	MAX_NATIONBF_QUEUE_SIZE		500
#define INVALID_MEMBER_POS			-1

enum ENUM_APPLY_QUEUE_TYPE
{
	eAQT_None	= 0,
	eAQT_NBF_ShuiGuo = enumNation_ShuiGuo,
	eAQT_NBF_XueGuo  = enumNation_XueGuo,
	eAQT_NBF_ShanGuo = enumNation_ShanGuo,
};

enum ENUM_NATIONBF_APPLY_CMD
{
	eNBF_SelfApply = 0,
	eNBF_TeamApply_Alert,
	eNBF_TeamApply_Force,
};

enum ENUM_NATIONBF_APPLY_RESULT
{	
	eResult_Success = 0,
	eResult_CheckInfoFail,
	eResult_QueueFull,
	eResult_TeamAtEdge, 
	eResult_OutTime,
};
#define MAX_SITE_ADDR_SIZE		128

#define DUMP_CORE_ONLY_ONCE_BEGIN	\
	{	\
		static BOOL recordLog = FALSE;	\
		if (!recordLog)	\
		{	\
			create_dump();	\
			recordLog = TRUE;

#define DUMP_CORE_ONLY_ONCE_END	\
		}	\
	}

#define DUMP_CORE_ONLY_ONCE	\
	DUMP_CORE_ONLY_ONCE_BEGIN	\
	DUMP_CORE_ONLY_ONCE_END

enum ENUM_TEAM_TYPE
{
	enum_All_Team,
	enum_Dungeon_BaoDian,
	enum_Dungeon_XuanShang,
	enum_Dungeon_TuLong,
	enum_Dungeon_DiGong,
	enum_Dungeon_MoCun,
	enum_Dungeon_MuXue,
	enum_Dungeon_QiFeng,
	enum_Achievement,
	enum_Dungeon_ShiLian,
	enum_Dungeon_PVP,
	enum_Other,
	enum_Team_Type_Count,
};

#define MAX_SITE_ADDR_SIZE		128

enum ENUM_GUILD_EVENT
{
	enum_Guild_Rewards_ID = 5104,			// 帮派奖励
	enum_Guild_Dungeon_ID,					// 帮派副本

};

#define GUILD_EVENT_MAX		32

enum CYC_TIME_EVENT_TYPE
{
	enumCycEvent_VIPToItem1 = 20000,		// 金卡领取道具
	enumCycEvent_VIPToItem2 = 20001,		// 银卡领取道具
	enumCycEvent_Count,
};

enum ENUM_MB_ABSORB_TYPE
{
	enumMBAT_Hide = 0,				// 隐藏
	enumMBAT_Show,					// 显示

	enumMBAT_UpdateMaxValue1,		// 灵力最大值
	enumMBAT_UpdateCurrentValue1,	// 灵力当前值

	enumMBAT_UpdateMaxValue2,		// 亲密最大值
	enumMBAT_UpdateCurrentValue2,	// 亲密当前值

	enumMBAT_UpdateMaxValue3,		// 休闲挂机经验最大值
	enumMBAT_UpdateCurrentValue3,	// 休闲挂机经验当前值
};

enum ENUM_MB_ABSORB_UI_TYPE
{
	enumMBAUT_1 = 0,	// 法宝吸灵力
	enumMBAUT_2,		// 法宝吸亲密
	enumMBAUT_3,		// 休闲挂机加经验
};

#define DEF_FORMATION_MAX_EXP_PRE_DAY(x)	((DWORD)((45.0f + x * 5.0f) * 0.5f * 4.0f * 120.0f))
#define DEF_FORMATION_MAX_EXP_PRE_DAY32(x)	((WORD)(DEF_FORMATION_MAX_EXP_PRE_DAY(x)>>16))
#define DEF_FORMATION_MAX_EXP_PRE_DAY16(x)	((WORD)(DEF_FORMATION_MAX_EXP_PRE_DAY(x)))

#define DEF_MB_FUSE_ITEM_MAX	64
#define DEF_MB_FUSE_ITEM_TYPE	4
#define DEF_SEEK_TEAM_TIME			10 * 1000


#define DEF_MB_FUSE_EVENT		10021

enum enumWorldValueID
{
	ewv_TopLevel = 0,
	ewv_TopPlayer1,
	ewv_TopPlayer2,
	ewv_TopPlayer3,
	ewv_TopPlayer4,
	ewv_TopPlayer5,
	ewv_TopPlayer6,
	ewv_TopPlayer7,
	ewv_TopPlayer8,
	ewv_TopPlayer9,
	ewv_TopPlayer10,
	ewv_GuildBTState,
	ewv_ElectionCfgFilePath,
	ewv_WorldValueCount,
};

#define MAX_PLAYER_MONEY			2000000000

enum enumSeekTeamOptType
{
	enum_seekteam_add,
	enum_seekteam_remove,
	enum_seekteam_join,
};


enum enumMagicBabyOptLogType
{
	enum_MB_Log_Create,
	enum_MB_Log_Destroy,
	enum_MB_Log_Trade,
	enum_MB_Log_Sale,
};

enum enumExpGhostState
{
	enum_egs_None,
	enum_egs_Clear,
	enum_egs_Level1,
	enum_egs_Level2,
	enum_egs_Level3,
	enum_egs_ExpFull,
	enum_egs_LevelFull,

};

#define DEF_FORMAT_EXP_RATE_ATTR_KEY	(7150)
#define DEF_FORMAT_CUR_EXP_PRE_DAY		(60000)
#define DEF_FORMAT_CUR_EXP_CLEAR_TIME	(60001)

//
//enum enumGuildOpt
//{
//	enum_guildmember_levelup	= 1,			//职务升级
//	enum_guildmember_leveldown	= 2,			//职务降级
//	enum_guildmember_recruit	= 4,			//招收成员
//	enum_guildmember_kickout	= 8,			//开除成员
//	enum_guildmember_event		= 16,			//帮派大记事管理
//	enum_guildmember_talent		= 32,			//升级帮派天赋
//	enum_guildmember_dungeon	= 64,			//开启帮派副本
//	enum_guildmember_callmember = 128,			//召集帮派成员
//};

#define GUILD_BBS_TITLE		32
#define GUILD_BBS_CONTENTS	128

#define GUILD_POINT_INIT	2
#define GUILD_POINT_LAST	1

#define GUILD_GLOBAL_REPUTATION_MUL	2
#define GUILD_GLOBAL_REPUTATION_DIV	2
#define MAX_KRES_DATA_LEN	1024

#define MAX_WORLDVALUE_POS	4
enum enumWorldValue_Pos{evp_Pos1 = 1, evp_Pos2 = 2, evp_Pos3 = 3, evp_Pos4 = 4};

enum enumLimitLevelState
{
	ells_UnBuild = 0,
	ells_Opening,
	ells_Building,
	ells_StandPlayer,
	ells_StandHero,
};

enum enumLimitLevelDo
{
	elld_UnDo = 0,
	elld_Done,
};

enum enumResIndexID
{
	eRII_Unknown = 0,
	eRII_TopPlayer1,
	eRII_TopPlayer2,
	eRII_TopPlayer3,
	eRII_TopPlayer4,
	eRII_TopPlayer5,
	eRII_TopPlayer6,
	eRII_TopPlayer7,
	eRII_TopPlayer8,
	eRII_TopPlayer9,
	eRII_TopPlayer10,
	eRII_RuntimeResCount,
};

#define GUILD_MEMBER_LEVEL1		0
#define GUILD_MEMBER_LEVEL2		150
#define GUILD_MEMBER_LEVEL3		300
#define GUILD_MEMBER_LEVEL4		450
#define GUILD_MEMBER_LEVEL5		600
#define GUILD_MEMBER_LEVEL6		1200
#define GUILD_MEMBER_LEVEL7		2400
#define GUILD_MEMBER_LEVEL8		4800
#define GUILD_MEMBER_LEVEL9		9600

enum enumGuildPosition
{
	eChangenumNGP_Master,
	eLevelup,
	eLeveldown,
	eKickout,
};

enum enumInviteTeamType
{
	eITT_Unknown = 0,
	eITT_I_Captain,
	eITT_You_Captain
};

enum ENUM_GAMBLING_TYPE
{
	enumGamblingT_Unknow = 0,
	enumGamblingT_Caesar,
	enumGamblingT_Count,
};

enum EXIT_GAME
{
	enum_Exit_None,
	enum_Exit_BackToCharacter,
	enum_Exit_BackToLogin,
	enum_Exit_CloseClient,
	enum_Exit_ChangeLine,
};
enum BillingItemFlag
{
	enum_BillingItemBegin,
	enum_BillingItemData,
};

enum enumGuildBtWeek{eGBW_Close = 0, eGBW_Week1, eGBW_Week2};
enum enumGuildBtRound{eGBS_Close = 0, eGBS_Apply, eGBS_Round1, eGBS_Round2, eGBS_Round3, eGBS_Round4, 
eGBS_Playoffs1, eGBS_Playoffs2, eGBS_Playoffs3, eGBS_End};

#define	MAX_FRIEND_LEVEL			5
#define MAX_KEEP_FRIENDLEVEL_SECS	604800	//7*24*3600

//奖池系统
#define	MAX_JACKPOT_VAL				10000000 // 1000万锭
enum HIDEANDSEEK_BETLEVEL
{
	BET_SCALE_LOW = 0,
	BET_SCALE_HIGH,

	BET_SCALE_TOTAL,
};
enum HIDEANDSEEK_QUERY_RESULT
{
	HAS_RESULT_ENROLL_ENABLE = 0,
	HAS_RESULT_ENROLL_DISABLE,
	HAS_RESULT_FUNC_DISABLE,
};


//修为.经脉
#define MAX_CHIEFVEIN_COUNT	128
#define MAX_MINORVEIN_COUNT	128

enum enumMeridianType
{
	enumMeridianType_Invalid,
	enumMeridianType_ChiefVein,		//正脉
	enumMeridianType_MinorVein,		//奇脉
	enumMeridianType_Count,
};

enum eSyncType
{
	eSyncNone,					// 不同步
	eSyncMyself,				// 同步给自己
	eSyncAllSightPlayer,		// 同步给所有视野范围内的玩家（包括自己）
	eSyncAllSightPlayerButMe,	// 同步给所有视野范围内的玩家（不包括自己）
	eSyncToAI,					// 同步给关联的AIServer

	eSyncToCurrentScene,		// 给当前场景同步
	eSyncToCurrentGame,		// 给当前gameserver同步
	e_SyncToSyncServer,
};

enum enumLearnSkillCheck
{
	enumLSC_Money		= 1,			//不检查金钱
	enumLSC_Front		= 2,			//不检查前置技能
	enumLSC_Class		= 4,			//不检查初级职业
	enumLSC_SubClass	= 8,			//不检查进阶职业
	enumLSC_Nation		= 16,			//不检查国籍
	enumLSC_PlayerLevel	= 32,			//不检查玩家等级
	enumLSC_FaithLevel	= 64,			//不检查玩家修真期
	enumLSC_Faith		= 128,			//不检查玩家修真值
};

enum enumLearnSkillCheckResult
{
	enumLSCR_Success,					//检查通过
	enumLSCR_Faild,						//检查未通过
	enumLSCR_Already,					//已经学会
	enumLSCR_CanNotStudy,				//不可学习
	enumLSCR_Nation,					//国籍
	enumLSCR_Class,						//初级职业
	enumLSCR_SubClass,					//进阶职业
	enumLSCR_PlayerLevel,				//等级
	enumLSCR_Money,						//金钱
	enumLSCR_FaithLevel,				//修真期
	enumLSCR_Faith,						//修真值
	enumLSCR_FrontSkill,				//前置技能未学会
	enumLSCR_High,						//已经学会更高级的技能
	enumLSCR_Top,						//已经学会,且是技能组最高等级技能
};

enum enumNakedModeCategory
{
	enumNMC_None	= 0,					//非裸奔模式
	enumNMC_Paopao,							//跑跑模式

	enumNMC_Count,
};

enum enumPlayerAttrType
{
	enumPAT_LabourUnion,				//仙工协会
	enumPAT_ProLevel,					//仙工协会等级
	enumPAT_ProLevelState,				//仙工协会显示标志
	enumPAT_ProExp,						//仙工协会专业经验
	enumPAT_ProDesignedPrecision,		//仙工协会专业专精度
	enumPAT_ProTire,					//仙工协会疲劳度
};
