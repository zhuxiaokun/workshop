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

	purpose:	�߼����ͻ��ˡ����������õ�һЩ����
    comment:    
*********************************************************************/

#pragma once
#pragma warning(disable:4305)

#include "KPassportDefine.h"

#define MAX_NATION			3					// ���Ҹ���
#define CELL_VILLAGE_BITS	10
#define MAX_MAP_VILLAGE		(1<<CELL_VILLAGE_BITS) // һ����ͼ������ׯ����

//  ��ģ��ͨ�õĺ궨��
#define DEF_TO_STRING(x)	#x
#define	MAX_NAME			20					//  ��ɫ���ֳ���
#define MAX_PLAYER_NAME		20
#define MAX_ENABLE_NAME_LENGTH	(14)			//	��ɫ����������󳤶�
#define MAX_MB_NAME			12					// ���������12�ֽ�
#define	MAX_ACCOUNT			20					//  ��Ϸ�ʺų���
#define	MAX_NAME_DEF		(MAX_NAME + 2)		//  ����ṹ��ʱ����������ֵĺ�������2���ֽڣ������Ϳ��Ա���ʹ��strcpy��
#define	MAX_MODEL_NAME_DEF	(MAX_PATH)			//  ����ṹ��ʱ����������ֵĺ�������2���ֽڣ������Ϳ��Ա���ʹ��strcpy��
												//  ������ֱ��ʹ��memcpy( , , MAX_NAME)
#define MAX_SIGNATURE		(20)				// ����ǩ����������
#define MAX_SIGNATURE_DEF	(32)				// ����ǩ��������� 12������
#define DEF_MAX_STRENGTH_LV	(10)				// ���ǿ���ȼ�

#define MAX_PLAYER_CREATE_INDEX_PRE_ACCOUNT	(6)			// ÿ���˻��½�������ɫ����
#define MAX_PLAYER_COUNT_PRE_ACCOUNT	(16)			// ÿ���˻�����ʹ�õ�����ɫ��

#define MAX_CHOOSE_QUESTREWARDITEM (6)			// �����Ʒ������ѡ����Ŀ

#define	MAX_ACCOUNT_DEF		(MAX_ACCOUNT + 2)		//  ͬ��
#define DEF_DIS_INTERACTIVE	(6.0f)
#define DEF_USE_SERVER_TEXT	(0xFFFFFFFF)
#define DEF_One_Minute	(60)
#define DEF_One_Hour	(60 * DEF_One_Minute)
#define DEF_One_Day		(24 * DEF_One_Hour)
#define DEF_PLAYER_PROTECT1_TIME	(7 * DEF_One_Day)
#define DEF_PLAYER_PROTECT2_TIME	(40 * DEF_One_Day)

static const float DEF_GRAVITY_ACCELERATION = -0.1f;  // �������ٶ�
static const float DEF_JUMP_INIT_VELOCITY = 1;

static const	int		CFG_MAX_BAG_EQUIP_ITEM_DEFAULT_COUNT		= 20;			// ������װ������Ĭ�Ͽ�������
static const	int		CFG_MAX_BAG_EQUIP_ITEM_COUNT				= 20;			// ������װ����������

static const	int		CFG_MAX_BAG_NORMAL_ITEM_DEFAULT_COUNT		= 60;			// ��������ͨ����Ĭ�Ͽ�������
static const	int		CFG_MAX_BAG_NORMAL_ITEM_COUNT				= 60;			// ��������ͨ��������
static const	int		CFG_MAX_BAG_EXTCOUNT						= 10;			// ÿ����չ����

static const	int		CFG_MAX_BAG_MATERIAL_ITEM_DEFAULT_COUNT		= 60;			// �����в��ϵ���Ĭ�Ͽ�������
static const	int		CFG_MAX_BAG_MATERIAL_ITEM_COUNT				= 60;			// �����в��ϵ�������

static const	int		CFG_MAX_BAG_QUEST_ITEM_DEFAULT_COUNT		= 60;			// �������������Ĭ�Ͽ�������
static const	int		CFG_MAX_BAG_QUEST_ITEM_COUNT				= 60;			// �����������������

static const	int		CFG_MAX_BAG_PET_ITEM_DEFAULT_COUNT			= 3;			// �����г���Ĭ�Ͽ�������
static const	int		CFG_MAX_BAG_PET_ITEM_COUNT					= 7;			// �����г�������

static const	int		CFG_MAX_BANK_NORMAL_ITEM_DEFAULT_COUNT		= 100;			// �ֿ�����ͨ����Ĭ�Ͽ�������
static const	int		CFG_MAX_BANK_NORMAL_ITEM_COUNT				= 100;			// �ֿ�����ͨ��������
static const	int		CFG_MAX_BANK_EXTCOUNT						= 10;			// �ֿ�ÿ����չ����

static const	int		CFG_MAX_BANK_MATERIAL_ITEM_DEFAULT_COUNT	= 49;			// �ֿ��в��ϵ���Ĭ�Ͽ�������
static const	int		CFG_MAX_BANK_MATERIAL_ITEM_COUNT			= 49;			// �ֿ��в��ϵ�������

static const    int     CFG_MAX_BAG_SOLD_ITEM_DEFAULT_COUNT         = 10;            // ������Ʒ�۵�Ĭ�ϸ���
static const    int     CFG_MAX_BAG_SOLD_ITEM_COUNT					= 10;            // ������Ʒ�۵�������

#define MAX_HOLD_SOLDITEM_TIME    60

#define MAX_CHATMESSAGE_LEN		256

#define GUILD_NAME_MAX			32
#define MAX_LEVELINFO_COUNT		(120)

// ����������
#define MAX_LIFESKILL_TYPE	8
// ������еĲɼ�&���켶������
#define MAX_LIFESKILL_LEVELBYTES 8

#define MAX_LIFESKILL_EXPERTISE_TYPE 8

// ���ǵ�������
#define SOUL_TYPE_COUNT 5

#define MB_MAX_GROUP_COUNT 4


// �����̵�
#define NPC_GUILD_STORE_ID		29
// �����̵�
#define NPC_HONOUR_STORE_ID		30
#define NPC_SPCAIL_STORE_ID		33 // ��ʱ����
// ��Ʒ�̵�
#define NPC_MATERIAL_STORE_ID	32
// �����̵ֻ꣬������
#define NPC_QUEST_BUY_STORE_ID	34
// �����̵ֻ꣬������
#define NPC_QUEST_SELL_STORE_ID	35

// ����ֵ�̵�, ֻ������
#define NPC_AFFECTION_STORE_ID 40

#define Max_Mentor_Dit_Length		256
#define Max_Mentor_Info_Exist_Delay 10 * 60 * 60
#define Max_Mentor_Pupil_sum		2
#define Max_Mentor_IntimacyVal		1000

//ѡ��ϵͳ��ѡ����, ������20
#define Max_Enroll_Reason_Length	40

// ����ע���û����͹�����������˵ķǷ��ַ�
#define NO_REG_INVALID_CHARS "`~!@#$%^&*()-=\\[];',./_+|{}:\"<>?"

// �߶��ϰ������޶ȣ���λ��1/32�ף�
#define HEIGHT_OBSTACLE_TOLERANT_RANGE 6

// ��ɫ״̬����״̬
enum CHARACTER_STATE
{
	csInvalid = 0,				// ��Ч״̬

	csOnStand,					// վ��״̬
	csOnSit,					// ����״̬

	csOnMove,					// �ƶ�״̬�������ߡ��ܡ��Զ����ߡ����е�

	csOnCharge,					// ����״̬
	csOnPrepare,				// ����״̬
	csOnShoot,					// ����״̬
	csOnChannel,				// ����״̬
	csOnRecover,				// ����״̬

	csOnDeath,					// ����״̬
	csOnTrade,					// ����״̬

	csOnJumpRise,				//  ����������
	csOnFall,					//  ׹��
	csOnLand,					//  ��½

	csOnUnFight,				// ����״̬
	csOnAutoFly,				// �Զ�����
	csOnByBus,					// ����bus״̬

	csOnBeHeld,					// ������״̬

	csOnFreeze,					// ������
	csTotal
};

enum CHARACTER_EVENT
{
	ceInvalid = 0,				// ��Ч�¼�

	ceOnHit,					// �����¼�
	ceOnDuck,					// �����¼�
	ceOnExpUpdate,				// �����¼�
	ceOnLand,					// ��½�¼�
	ceOnLandRun,				// �ܶ���½�¼�
	ceOnHonor,					// �������ֵ
	ceOnFaithExpUpdate,			// �����Ϊֵ�¼�

	ceTotal
};

enum CHARACTER_CONDITIONEVENT
{
	cceInvalid = 0,

	cceQuestInvalid = 0,		// 0.	���������¼� --- ����������
	cceQuestCondition = 1,		// 1.	���������¼� --- �����������������
	cceQuestAccept = 2,			// 2.	���������¼� --- �ѽ��ܵ���δ�������
	cceQuestUnaccept = 3,		// 3.	���������¼� --- ���㵫��δ��������
	cceQuestFinish = 4,			// 4.	���������¼� --- �Ӳ�������񵫻�Ϊ������

	cc3de_onengage = 5,			// 5.	���������¼� --- ����ս��
	cc3de_ontruce = 6,			// 6.	���������¼� --- �˳�ս��

	cceTotal,
};

//  NPC����״̬
enum NPC_BASE_STATE
{
	nbsInvalid = 0,				// ��Ч״̬

	nbsGod      ,				// �޵�״̬
	nbsAuto     ,				// �Զ�״̬
	nbsFight    ,				// ս��״̬
	nbsPassivity,				// ����״̬
	nbsMax		,
};

//  NPC��չ״̬
enum NPC_STATE
{
	nsInvalid = 0,				// ��Ч״̬

	// �޵�
	nsDisappear,				// ��ʧ״̬
	nsWarp,						// ˲��״̬
	nsBorn,						// ����
	nsDie,						// ����״̬
	nsRelease,					// ���״̬

	// �Զ�
	nsLeisure,					// ����״̬
	nsPointMove,				// �����ƶ�״̬
	nsPointLeisure,				// ��������״̬
	nsLeisureX,					// ��������״̬
	nsFollow,					// ����״̬
	nsPickUp,					// ʰȡ״̬
	nsDamaged,					// ���״̬

	// ս��
	nsFightWait,				// ս���ȴ�
	nsChase,					// ׷��״̬
	nsFightMove,				// ս���ƶ�
	nsGiddy,					// ѣ��״̬
	nsFear,						// �־�״̬
	nsFightRunAway,				// ս������

	// ս��ֹͣ
	nsOutOfControl,				// ʧ��״̬
	nsRunAway,					// ����״̬
	nsMove,						// �ƶ�״̬
	nsSurrender,				// Ͷ��״̬

	// ����״̬
	nsBeTake,					// ��Я��

	// ׷�ӵ�״̬
	nsPointMoveX,				// ���ⶨ���ƶ�״̬
	nsMax,
};

enum MAGIC_BABY_STATE
{
	mbsInvalid = 0,	
	mbsBack,					// �ջ�
	mbsFightWait,				// ս������
	mbsFightPrepare,			// ս��Ԥ��
	mbsFight,					// ս����
	mbsFightEnd,				// ս������
	mbsEatGrass,				// �����
	mbsRide,					// ���

	mbsMax,

};

enum EVENT_ID
{
	EVENT_ENTRY = 0,	// ����
	EVENT_EXIT,			// �뿪
	EVENT_IDLE,			// ����
	EVENT_LAST,			// ��һ��״̬
	EVENT_APPOINTED,	// ָ���õ�һ��״̬
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
	ANIMSTATUS_JUMPRISE = 10,  //  ����������
	ANIMSTATUS_FALL = 11,  //  ׹��
	ANIMSTATUS_LAND = 12,  //  ��½
	ANIMSTATUS_UNFIGHT = 13, //  ������
	ANIMSTATUS_RELIVE = 14,
	ANIMSTATUS_CANCEL = 15,	//���
	ANIMSTATUS_SHOOT = 16,	//ֱ�ӹ���
	ANIMSTATUS_CHANNEL = 17,//����CHANNEL״̬
	ANIMSTATUS_AUTOFLY = 18, //�Զ�����
	ANIMSTATUS_BYBUS = 19, // ����bus

	ANIMSTATUS_BE_HELD = 20, //��������
	ANIMSTATUS_BE_PUT =21, //������

	ANIMSTATUS_FREEZE = 22, // ����
	ANIMSTATUS_CHARGE = 23, // ����
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
	ANIMSTATUS_AISTATE_FIGHT_WAIT,		// ս���ȴ�
	ANIMSTATUS_AISTATE_MOVE,			// �޴����ƶ�
	ANIMSTATUS_AISTATE_FIGHT_MOVE,		// ս�����޴����ƶ�
	ANIMSTATUS_AISTATE_POINTMOVEX ,		// �����ƶ�
	ANIMSTATUS_AISTATE_LEISUREX ,		// ��������
	ANIMSTATUS_AISTATE_FEAR,			// �־�״̬
	ANIMSTATUS_AISTATE_GIDDY,			// ѣ��״̬
	ANIMSTATUS_AISTATE_FIGHTRUNAWAY,	// ս������
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

// �ͻ����߼����״̬
enum MOUSE_STATE
{
	msInvial,			// �����״̬

	msOnNormal,			
	msOnMoveItem,		// �ƶ���Ʒ״̬
	msOnPickItem,		// ��ȡ��Ʒ״̬

	msTotal,
};

// ��궯��
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
// ���ʽ
enum RELIVE_TYPE
{
	rtInvalid = 0,
	rtOrigin,			// ԭ�ظ���
	rtGrave,			// Ĺ�ظ���
	rtPerfectOrigin,	// ԭ����������
	rtTotal
};

// ������ʽ
enum DEATH_TYPE
{
	dtInvalid = 0,
	dtNormal,
	dtCritical_1,//�����˺�
	dtCritical_2,//ˮ
	dtCritical_3,//��
	dtCritical_4,//��
	dtCritical_5,//��
	dtCritical_6,//��
	dtTotal,
};

// ����״̬
enum STAND_TYPE
{
	stInvalid = 0,
	stNormal,
	stFight,
	stTotal
};

//  �ƶ���ʽ
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

// װ��ģʽ
enum EQUIPMENT_MODE
{
	emNormal = 0,	// װ��ģʽ
	emFashion = 1,	// ʱװģʽ
	emNaked = 2,	// ����ģʽ
};

enum HELM_SHOW_MODE
{
	emHideHelm = 0,	// ����ͷ��
	emShowHelm,		// ��ʾͷ��
};


// ֪ͨ�ͻ���λ���ƶ�����
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


// ����Ŀ��
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
	enumIOR_Attribute_AllUpdate,				// ȫ������
	enumIOR_Attribute_IncreaseNum,				// ��������
	enumIOR_Attribute_SetNum,
	enumIOR_Attribute_IncreaseFrequencyNum,		// ����ʹ�ô���
	enumIOR_Attribute_SetFrequencyNum,
	enumIOR_Attribute_IncreaseStrengthenLevel,	// ����ǿ���ȼ�
	enumIOR_Attribute_SetStrengthenLevel,
	enumIOR_Attribute_IncreaseDura,				// �ı��;ö�
	enumIOR_Attribute_SetDura,
	enumIOR_Attribute_SetPrivateFlags,			// �ı�������
	enumIOR_Attribute_SetAliveTime,				// �޸Ĵ���ʱ��
	enumIOR_Attribute_Bind,						// ��Ʒ����
	enumIOR_Attribute_UnBind,					// ��Ʒ����
	enumIOR_Attribute_UpdateEquipInfo,			// ����װ����Ϣ
	enumIOR_Attribute_BuffID,					// ����
	enumIOR_Attribute_End,
};

enum NPC_SPECIAL_FUNCTION
{
	NSF_NOTHING		= 0	,
	NSF_CHECKQUEST		,		// ��Ҫ������ֵ�����ID
	NSF_REFRESHNPCID	,		// �������û�������������,��ˢһ���µ�NPC����
	NSF_LIVETIME		,		// ���ʱ��
	NSF_LIVETIME_LEAVE_FIGHT,	// ����ս������ʱ��
	NSF_RANDOMITEM		,		// �����Ʒ
	NSF_FUNC_COUNT		,
};

enum LEARN_SKILL_FLAG
{
	enumLSF_CAN_LEARN	= 0	,	// ����ѧϰ�ļ���
	enumLSF_CAN_NOT_LEARN	,	// �㲻����ѧϰ�ļ���
	enumLSF_ALREADY_LEARN	,	// �Ѿ�ѧ��ļ���
	enumLSF_ALREADY_LEARN_HIGHER,	//�Ѿ�ѧ����ߵȼ�����
	enumLSF_ALL_SKILL		,	// ȫ��
};

enum CAREER_CHOISE_LEVEL
{
	enumCCL_Least		= 0,		//
	enumCCL_Start		= 10,		//ѡְ
	enumCCL_Advanced1	= 40,		//����
	enumCCL_Advanced2	= 60,		//�ٴν���

};

//Talisman����
enum TALISMAN_BIND_STATUS
{
	enumTBS_Invalid = 0,
	enumTBS_Bind,
	enumTBS_UnBind,
	enumTBS_BigMode,
	enumTBS_SmallMode,
	enumTBS_Total
};

// ���书��
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
///* ��ҽ���                                                             */
///************************************************************************/
//// ��ҵĽ���״̬����
//enum enumPlayerTrade_State
//{
//	enumPT_STATE_NOTHING = 0,	// ��״̬
//	enumPT_STATE_LOCKED,		// ����״̬
//	enumPT_STATE_AGREEMENT,		// ���Խ���
//	enumPT_STATE_TRADING,		// ������
//};
//
//// �ͻ��������������������
//enum enumPlayerTrade_Command
//{
//	enumPT_Nothing	= 0,
//
//	enumPT_Request_PutOnItem,		// ��������������Ʒ
//	enumPT_Request_ModifyMoney,		// ��������������Ʒ
//	enumPT_Request_GetBack,			// �ӽ�����ȡ����Ʒ
//	enumPT_Request_SwapItem,		// ����������Ʒ�໥�ƶ�
//	enumPT_Request_Lock,			// ����������
//	enumPT_Request_Agree,			// ͬ�⽻��
//	enumPT_Request_Cancel,			// ȡ������
//	enumPT_Request_PutOnMB,			// ��������������Ʒ
//
//	enumPT_Response_ItemPutOn,		// ��Ʒ�������ڽ���������
//	enumPT_Response_ItemGetBack,	// ��Ʒ���û�����
//	enumPT_Response_ItemSwaped,		// ��Ʒ��������
//	enumPT_Response_StateChanged,	// ����״̬�޸�
//	enumPT_Response_MoneyModified,	// ��Ǯ�޸�
//	enumPT_Response_Successed,		// �ɹ�
//	enumPT_Response_Failed,			// ʧ��
//	enumPT_Response_Cancellation,	// ȡ��
//	enumPT_Response_PutOnMB,		// ��Ӧ������
//
//	enumPT_Count,
//};

// �������������ͻ��˵���Ϣ����
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


// ���������������Ϣ
enum ENUM_CHAT_GROUP
{
	enumECG_Invalid = 0,
	enumECG_System,					// ϵͳ����ʾ���桢ϵͳ��Ϣ�ȣ����󲿷���ʾ�����Ϸ��Ĺ������У�
	enumECG_Info,					// ��Ϣ����ʾ������Ϣ By Client��
	enumECG_BattleInfo,				// ս����Ϣ����ʾս����Ϣ By Client��
	enumECG_Private,				// ˽�ģ�һ��һ˽�� By ChatServer��
	enumECG_Team,					// ���飨�������� By ChatServer��
	enumECG_Current,				// ������ͬ����Χ�� by GameServer��
	enumECG_Map,					// ����ͬ����Χ������ͼ By GameServer��
	enumECG_Guild,					// ���ɣ��������� By ChatServer��
	enumECG_Nation,					// ���ң��������� By ChatServer��
	enumECG_World,					// ���磨������������By ChatServer��
	enumECG_BattleCry,				// �ź���������ͼ By GameServer��������ͼ��
	enumECG_BattleField,			// ս��Ƶ��������ս����ͼ By GameServer����ʵ����������ͼ��
	enumECG_MainFrameInfomation,	//��������Ϣ����ɫͷ����
	enumECG_MainFrameTopInfomation,	//�������ö���Ϣ
	enumECG_MessageBox,				// �Ի���
	enumECG_SystemAndMainFrame,		// ϵͳƵ����ͷ������ʾ
	enumECG_ShuiGuo,				// ˮ��
	enumECG_XueGuo,					// ѩ��
	enumECG_ShanGuo,				// ɽ��
	enumECG_SingletonMessageBox,	// �Ի���
	enumECG_FindTeam,				// Ѱ����飨���ϵȼ������� By ChatServer��
	enumECG_CustomChannel,			// �Զ���Ƶ����ʼ
	enumECG_IMGuild,				// ���ɣ�IM��
	enumECG_IMPrivate,				// ˽�ģ�IM��
	enumECG_Count,					// Ƶ��������
};

//// �����ϵͳ��Ϣ������ʾλ��
//enum ENUM_CHAT_SHOW_POSITION
//{
//	enumECSP_Invalid = 0,
//	enumECG_System,			//���촰��
//	enumECSP_MainFrameInfomation,	//��������Ϣ����ɫͷ����
//	enumECSP_MainFrameTopInfomation,//�������ö���Ϣ
//	enumECSP_Count,
//};

//��Ҫ��3D���ı���ActionEventö��
enum Action_Character_3DEvent
{
	c3d_onexpupdate = 0,		// 0.	����

	c3d_onhit,					// 1.	����
	c3d_onhit_shoulder,			// 2.	����(��)
	c3d_onhit_lift,				// 3.	����(��)
	c3d_onhit_resist,			// 4.	����(��)
	c3d_onhit_drag,				// 5.	����(��)
	c3d_onhit_rideride,			// 6.	����(����)
	c3d_onhit_ridesit,			// 7.	����(����)
	c3d_onhit_ridestand,		// 8.	����(��վ)
	c3d_onhit_ridehang,			// 9.	����(���)
	c3d_onhit_rideside,			// 10.	����(���)

	c3d_onbursthit,
	c3d_onbursthit_shoulder,	// 11.	����(��)
	c3d_onbursthit_lift,		// 12.	����(��)
	c3d_onbursthit_resist,		// 13.	����(��)
	c3d_onbursthit_drag,		// 14.	����(��)
	c3d_onbursthit_rideride,	// 15.	����(����)
	c3d_onbursthit_ridesit,		// 16.	����(����)
	c3d_onbursthit_ridestand,	// 17.	����(��վ)
	c3d_onbursthit_ridehang,	// 18.	����(���)
	c3d_onbursthit_rideside,	// 19.	����(���)
	c3d_onduck,					// 20.	����

	c3d_onland,					// 21.  ��½�¼�
	c3d_onland_shoulder,		// 22.	3D��½(��)
	c3d_onland_lift,			// 23.	3D��½(��)
	c3d_onland_resist,			// 24.	3D��½(��)
	c3d_onland_drag,			// 25.	3D��½(��)
	c3d_onland_petride,			// 26.	3D��½(����)
	c3d_onland_petsit,			// 27.	3D��½(����)
	c3d_onland_petstand,		// 28.	3D��½(��վ)
	c3d_onland_pethang,			// 29.	3D��½(���)
	c3d_onland_petside,			// 30.	3D��½(����)
	c3d_onland_bind,			// 31.	3D��½(���෨��)
	c3d_onland_unbind,			// 32.	3D��½(���෨��)
	c3d_onland_big,				// 33.	3D��½(���෨��)
	c3d_onland_small,			// 34.	3D��½(���෨��)
	c3d_onlandrun,				// 35.	3D��½��
	c3d_onlandrun_shoulder,		// 36.	3D��½��(��)
	c3d_onlandrun_lift,			// 37.	3D��½��(��)
	c3d_onlandrun_resist,		// 38.	3D��½��(��)
	c3d_onlandrun_drag,			// 39.	3D��½��(��)
	c3d_onlandrun_petride,		// 40.	3D��½��(����)
	c3d_onlandrun_petsit,		// 41.	3D��½��(����)
	c3d_onlandrun_petstand,		// 42.	3D��½��(��վ)
	c3d_onlandrun_pethang,		// 43.	3D��½��(���)
	c3d_onlandrun_petside,		// 44.	3D��½��(����)
	c3d_onlandrun_bind,			// 45.	3D��½��(���෨��)
	c3d_onlandrun_unbind,		// 46.	3D��½��(���෨��)
	c3d_onlandrun_big,			// 47.	3D��½��(���෨��)
	c3d_onlandrun_small,		// 48.	3D��½��(���෨��)

	c3d_onpickup,				// 49.	3Dʰȡ����
	c3d_onquest_1,				// 50.	����״̬1
	c3d_onquest_2,				// 51.	����״̬1
	c3d_onquest_3,				// 52.	����״̬1
	c3d_onquest_4,				// 53.	����״̬1
	c3d_onquest_complete,		// 54.	�������

	c3d_onborn,					// 55.	��ɫ����

	c3d_onholdweapon,				// 56.	����ս��ִе
	c3d_onhideweapon,				// 57.  �뿪ս����ִе

	c3d_Total,
};

enum Action_TreasureCase_3DEvent
{
	c3d_TC_Open,		// 0.	����򿪶���
	c3d_TC_Opened,		// 1.	�����״̬
	c3d_TC_Close,		// 2.	����رն���
	c3d_TC_Closed,		// 3.	����ر�״̬
	c3d_TC_Tote,		// 4.	����Я������
	c3d_TC_Death,		// 5.	������������
};
// ��Ҫ3D Show���������װ������
// ע�⣺����ö��˳��Ҫ�̶�����int KPlayer::GetAppearData(char* pAppearData)���õ����˳��
enum ENUM_EQUIPMENT_3DSHOW
{
	enumEE3_Weapon	= 0,										//01.	����
	enumEE3_Helm,												//02.   ͷ��
//	enumEE3_Mantle,												//03.	���������磩
	enumEE3_Clothing,											//04.	�·�
	enumEE3_Shoes,												//05.	�㲿
	enumEE3_FashionHead,										//06.	ʱװͷ��
	enumEE3_FashionCloth,										//07.	ʱװ�·�
//	enumEE3_FashionShoes,										//08.	ʱװЬ��
	enumEE3_Count,												//09.	����
};

// Ϊȡ����ɫ����ҡ���������ͨ����Ϣ
enum ENUM_CHAR_TYPE
{
	enum_CT_MainPlayer = 0,	// ���
	enum_CT_MagicBaby1,	// ����1
	enum_CT_MagicBaby2,	// ����1
	enum_CT_MagicBaby3,	// ����1
	enum_CT_MagicBaby4,	// ����1
	enum_CT_MagicBaby5,	// ����1
	enum_CT_MagicBaby6,	// ����1
	enum_CT_MagicBaby7,	// ����1
	enum_CT_MagicBaby8,	// ����1

	enum_CT_Observer_MagicBaby0,	// ����0
	enum_CT_Observer_MagicBaby1,	// ����0
	enum_CT_Observer_MagicBaby2,	// ����0
	enum_CT_Observer_MagicBaby3,	// ����0
	enum_CT_Observer_MagicBaby4,	// ����0
	enum_CT_Observer_MagicBaby5,	// ����0
	enum_CT_Observer_MagicBaby6,	// ����0
	enum_CT_Count,
};

// �����Ӳ���
enum MAGIC_BABY_OPERATION_INDEX
{
	// ����һ������
	MBOI_ADD_ONE_MAGIC_BABY,
	// uParam = (SC_AddMagicBaby*)lpBuff
	// lParam = 0

	// �޸�һ����������
	MBOI_UPDATE_ONE_MAGIC_BABY,
	// uParam = (CS_UpdateMagicBaby* || SC_UpdateMagicBaby*)lpBuff
	// lParam = 0

	// ��ɾһ����������
	MBOI_DELETE_ONE_MAGIC_BABY,
	// uParam = (CS_DeleteMagicBaby* || SC_DeleteMagicBaby*)lpBuff
	// lParam = 0	

	// ��һ������������Ƕ
	MBOI_INLAY_ONE_MAGIC_BABY,
	// uParam = (CS_InlayMagicBaby* || SC_InlayMagicBaby*)lpBuff
	// lParam = 0	

	// ��һ������������Ƕʱ������һ����Ʒ
	MBOI_INLAY_PUT_ITEM_MAGIC_BABY,
	// uParam = (CS_InlayPutItemMagicBaby* || SC_InlayPutItemMagicBaby*)lpBuff
	// lParam = 0	

	// ��һ������������Ƕʱ��ȡ��һ����ʯ
	MBOI_INLAY_GET_BACK_ITEM_MAGIC_BABY,
	// uParam = (CS_InlayGetBackItemMagicBaby* || SC_InlayGetBackItemMagicBaby*)lpBuff
	// lParam = 0	

	// ��һ������������Ƕʱ��������ʯ
	MBOI_INLAY_SWAP_ITEM_MAGIC_BABY,
	// uParam = (CS_InlaySwapItemMagicBaby* || SC_InlaySwapItemMagicBaby*)lpBuff
	// lParam = 0

	// ��һ������������Ƕʱ�������ؽ��
	MBOI_INLAY_RESULT_MAGIC_BABY,
	// uParam = (CS_InlayResultMagicBaby* || SC_InlayResultMagicBaby*)lpBuff
	// lParam = 0

	// ����һ����������Ϊ�ȼ�
	MBOI_UPDATE_MAGIC_BABY_FAITH_LEVEL,
	// uParam = (SC_MagicBaby_Faith_Level_Update*)lpBuff
	// lParam = 0

	// ����һ����������Ϊֵ
	MBOI_UPDATE_MAGIC_BABY_FAITH_EXP,
	// uParam = (SC_MagicBaby_Faith_Value_Update*)lpBuff
	// lParam = 0

	MBOI_UPDATE_MAGIC_BABY_TRADE,

	// ����һ����������������
	MBOI_UPDATE_MAGIC_BABY_ATTR,
	// uParam = (SC_MagicBaby_ATTR_Change*)lpBuff
	// lParam = 0
};

// ״̬���뿪ĳ״̬��ԭ��
enum eLeaveStateReason
{
	enumLSR_Normal = 0,	//�����뿪
	enumLSR_Passive,	//�����뿪
	enumLSR_Active,		//�����뿪
};

// Skill����ԭ��
enum eSkillFinishReason
{
	enumSFR_Success			= 0,	//�����뿪
	enumSFR_UseIllegalSkill	= 1,	//ʹ���˷Ƿ�����
	enumSFR_Interrupt		= 2,	//���
	//TODO: �����쳣�뿪ԭ��
};

enum eSkillLoadingType
{
	enumSLT_Charge			= 0,	//����
	enumSLT_Prepare			= 1,	//����
	enumSLT_Channel			= 2,	//����
	enumSLT_FormulaMade		= 3,	//�䷽����
};

// ��Ʒ�������������
enum ENUM_BLOCK_ALL_TYPES
{
	enumBlock_Inviad = 0,							// ��Ч�ĸ��ӣ�����������ʧʹ��
	enumBlock_BagNormal,							// ������ͨ��Ʒ
	enumBlock_BagQuest,								// ����������Ʒ
	enumBlock_BagMat,								// ����������Ʒ	
	enumBlock_BagEquip,								// ����װ����Ʒ
	enumBlock_BagPet,								// ���ﱳ����
	enumBlock_BankNormal,							// �ֿ���ͨ��Ʒ
	enumBlock_BankMat,								// �ֿ������Ʒ
	enumBlock_BagSoldItem,							// �����̵����Ʒ

	enumBlock_SlotCount,							// ������Ʒ�������͸���
};

enum ENUM_MONEY_DEFINE
{
	emBegin = 1,
	emMainMoney = emBegin,	// ������Ϸ��Ǯ
	emOffMoney,				// ����Ʊ
	emCash1,				// Ԫ��1
	emCash2,				// ����
	emCash3,				// ����
	emEnd,					// ����
};

//�����Ʒ��û�ʧȥ��Դλ��
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
	msNoMoneyRelive,		// ��Ǯ���������ܸ���
	msDead,					// ����
	msRelive,				// ����
	msForceSetPosition,		// ��ǿ��λ��
	msNoItemRelive,			// û�ж�Ӧ�ĸ���ʯ ��ʾ�Ƿ�Ԫ��
	msNoIBRelive,			// ԭ�ظ���ʱû�ж�Ӧ��IB
	msNoPerfectItemRelive,	// û�ж�Ӧ����������ʯ ��ʾ�Ƿ�Ԫ��
	msNoPerfectIBRelive,	// ����ԭ�ظ���ʱû�ж�Ӧ��IB
	msPerfectReliveCD,		// ����ԭ�ظ���CoolDown��
};
// ������Ʒ��ӡ�ɾ�������������
#define CFG_MAX_CHECK_QUEST_ITEM_COUNT	10

// ��ͨϵͳ
#define	MAX_TRANSPORT_CONFIG	256
#define	MAX_TRAFFIC_ROUTE_COUNT	256
#define MAX_TRAFFIC_QUEUE_COUNT	256	// ȫ����һ��256����վ������ÿ��˫��·��ʹ��һ����վ��ÿ��busʹ��n����վ

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

/// ���״̬
enum ENUM_MOUSE_STATE
{
	enumMouseNormal	=	0,						// һ��״̬
	enumMouseBuy,								// ��״̬
	enumMouseSail,								// ��״̬
	enumMouseSplit,								// ���״̬
	enumMousePick,								// ʰȡ
	enumMousePickBox,							// ��������
	enumMouseFight,								// ս����ʾ
	enumMouseOperator,							// ��������
	/// ������
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

// ���ѡ��Character�ķ�ʽ
enum eMouseCharTargetType
{
	enumMCTT_LeftClick = 1,		//���ѡ��
	enumMCTT_RightClick,		//�һ�ѡ��
	enumMCTT_MouseOver,			//������Character����ѡ��
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

//Linux���е����⣬���������������� ��KCommonStruct���棬��ͷɾ 20091024 Calvin
//#define randbignum(num) ((rand()*32768 + rand()) % (num))
//#define randsmallnum(num) (rand() % (num))

// ���ID��������ޣ�����������Բ��ܳ��������
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


	NPC_ID_BASE_START,						// �����ķ��������Ƶ�npc��ʼID
	TEMP_NPC_ID_BASE_START		=  (NPC_ID_BASE_START + NPC_ID_LIMITED - 1000),	// ֻ�пͻ��˿��Ƶ�npc��ʼID
	NPC_ID_BASE_END				=  (NPC_ID_BASE_START + NPC_ID_LIMITED),		// ����npc������ʵ��ID

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
	eColor_White,	// ����
	eColor_Green,	// �Ѻ�
	eColor_Red,		// �ж�
	eColor_Purple,	// ����
	eColor_Yellow,	// ��������
	eColor_Croci,	// ��PKģʽ�µ�ͬ��
	eColor_MainPlayer,
};

enum{
	RIDE_INVALID = 0,	// ����
	RIDE_SOMEONE,		// ���
	RIDE_NONE,			// ȡ�����
	RIDE_CHANGE_STATE,	// �������״̬
	RIDE_CHANGE_HEIGHT,	// ������˸߶�
	RIDE_CLOSE_2RIDE,	// ���˫�����
	RIDE_START_HEIGHT,	// ��ʼ����
	RIDE_STOP_HEIGHT,	// ֹͣ����
	RIDE_SYNC_HEIGHT,	// ͬ���������������Ϣ
};

/*
 * ���ָ��ŵ�Character����ʱ�������
 */
enum ENUM_MOUSE_POINTER_TYPE_TO_3D
{
	/*
	enumMPTT3D_CanNotSelect	= 0,	//�޷���ѡ��
	enumMPTT3D_Selectable	= 1,	//�ɱ�ѡ�е�
	enumMPTT3D_Openable		= 2,	//�ɱ���
	enumMPTT3D_Closable		= 3,	//�ɱ��ر�
	enumMPTT3D_Totable		= 4,	//�ɱ�Я��
	enumMPTT3D_Attackable	= 5,	//����
	enumMPTT3D_Supportable	= 6,	//�Ѿ�
	enumMPTT3D_CanNotSkill	= 7,	//����
	enumMPTT3D_Talkable		= 8,	//�ɶԻ���
	*/

	//����ö�ٸ��ⲿ�����йأ��벻Ҫ����˳��
	enumMPTT3D_CanNotSelect	= 0,	//�޷���ѡ��
	enumMPTT3D_Selectable	= 1,	//�ɱ�ѡ�е�
	enumMPTT3D_Openable		= 2,	//�ɱ���
	enumMPTT3D_Closable		= 3,	//�ɱ��ر�
	enumMPTT3D_Totable		= 4,	//�ɱ�Я��
	enumMPTT3D_ActiveAttackable	= 5,	//  ��������
	enumMPTT3D_PassivityAttackable  = 6,    // ��������
	enumMPTT3D_Supportable	= 7,	//�Ѿ�
	enumMPTT3D_CanNotSkill	= 8,	//����
	enumMPTT3D_Talkable		= 9,	//�ɶԻ���
	enumMPTT3D_HaveQuest    = 10,     //�ɽ�����
	enumMPTT3D_GroudItem     = 11,    //��ʰȡ������
	enumMPTT3D_FinishQuest	= 12,	  //�ɽ�����
	
};

enum ENUM_AUTOMOVE_AND_FIND_TARGET_TYPE
{
	enum_AAFTT_Unknow = 0,
	enum_AAFTT_ID,
	enum_AAFTT_TypeID,
};

/*
 * ��ѯĿ������Զ�Ѱ·��ʱ�������
 */
enum ENUM_TARGET_POS_TYPE_IN_AUTOMOVING
{
	enumTPTIA_Reachable		= 1,	//Ŀ��ɴ�
	enumTPTIA_Obstacle		= 2,	//Ŀ�����ϰ�
	enumTPTIA_Unknown		= 3,	//δ֪����
};

// ������صĲ�������
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

// ������ز����Ľ��
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
	ePKSwitch_Guild     = 1,	// ����
	ePKSwitch_Nation    = 2,	// ��ս
	ePKSwitch_World     = 3,	// ȫ��
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
	eItemEvent_Type_DisappearByGameTimeUpdate,	//��Ʒ��ʱ��ʧBy��ʵʱ�䣬������ʵʱ���ж�
	eItemEvent_Type_DisappearByRealTimeUpdate,	//��Ʒ��ʱ��ʧBy��Ϸʱ�䣬���ߺ󲻼���	
	eItemEvent_Type_EverydayDisappear,			//ÿ���ӳٵ�ʱ��ʧ
	eItemEvent_Type_EveryweekDisappear,			//ÿ���ӳٵ�ʱ��ʧ
	eItemEvent_Type_EverymonthDisappear,		//ÿ���ӳٵ�ʱ��ʧ
	eItemEvent_Type_End,
};



//////////////////////////////////////////////////////////////////////////
/*
<����1>������ϵ
0=������
1=��С��
2=����
3=����
4=����
5=��ż
6=ʦͽ
*/
enum ENUM_FILTER_CONDITION
{
	enum_Filter_Condition_Any = 0,
	enum_Filter_Condition_Team,			//	��С��
	enum_Filter_Condition_Tang,			//	����
	enum_Filter_Condition_Guild,		//	����
	enum_Filter_Condition_Nation,		//	����
	enum_Filter_Condition_Consort,		//	����
	enum_Filter_Condition_Master,		//	ʦͽ
	enum_Filter_Condition_GuildMaster,	//  ����
	enum_Filter_Condition_GuildSubMaster,//  ����
	enum_Filter_Condition_GuildPost,	//  ��Ա
	enum_Filter_Condition_Count,
};

// // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // // 

// �����ļ�·������׺
#ifdef _CLIENT 
#	define SETTING_DIR			"..\\..\\data\\configs\\"	// ��ǰ�ļ�Ŀ¼
#else
#	define SETTING_DIR		"../Settings/"	// �����ļ�Ŀ¼
#endif

#if defined(WIN32) || defined(WINDOWS)
#	define SERVER_DIR			"Server/"
#	define CLIENT_DIR			"Client\\"
#else
#	define SERVER_DIR			"Server/"
#	define CLIENT_DIR			"Client/"
#endif

//�������
#ifdef _CLIENT
//����������Ч���¼�id
#	define EXTRACT_GHOST_EVENT_ID	2001
#endif // _CLIENT

#define MAX_WORLD_DROP_COUNT		8		// ÿ��World�����8�����������	
#define MAX_TIME_WORLD_DROP_COUNT	16		// ÿ��Time�����8�����������	
#define MAX_DROP_GROUP_COUNT		8		// ÿ�����������8����ͨ������
#define MAX_DROP_ITEM_COUNT			4096	// ���е������������������ܺ�
#define MAX_WORLD_DROP_ITEM_COUNT	8192	// ��������������������������ܺ�

#define MAX_DROP_ITEM_SGC_COUNT				8		// SGCɸѡ�б���
#define MAX_DROP_ITEM_QUALITY_COUNT			8		// Ʒ��ɸѡ�б���

#define MIN_TELEPORT_DISTANCE		262144	// 512*512

enum{
	MagicWeaponClick,
	MagicWeaponIsInQueue,
};

enum SECURICA_RERESULT
{
	smAddSuccess,
	smAddRepeat,	// �ظ�
	smAddFailed,	// ʧ��
	smAddLevelLimit,// �ȼ�����
	smAddNoMoney,	// Ǯ����
	smBeBroken,		// �ڳ���
	smAway,			// Զ��
	smNear,			// �һ�
	smLost,			// ��ʧ
	smAddToClient,	// ֪ͨ�ͻ��ˣ��ڳ��쵽��
	smRemoveFromClient,	// ֪ͨ�ͻ��ˣ��ڳ�ɾ����
	smNoRide,		// ���������
};

// ��̯
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
	enumESSS_Maintenance = 0,	// ά�����޷���½
	enumESSS_Fluency,			// ����
	enumESSS_Busy,				// ��æ
	enumESSS_Full,				// ����
	enumESSS_NoLogin,			// �������޷���½
};


//--------------- ��������
// ÿ�����������2ֻ����
#define MAX_EGGS_PER_PLAYER		1
// ÿҳֻ��ʾ7��������
#define MAX_EGGS_PER_PAGE		7
// ��������ʱ��
#define MIN_EGG_ADULT_TIME		1800
// �����¶�ʱ�� 6��
#define MIN_EGG_FREE_TIME		(6*24*3600)
// ��������ʱ�� 7��
#define MIN_EGG_LOST_TIME		(7*24*3600)

enum MAGIC_BABY_EGG_ERROR
{
	mbeSuccess,
	mbeError,		// δָ������
	mbeMBFull,		// �����Ѿ�����
	mbeNotYours,	// �������
	mbeNotAdult,	// û�з�����

	mbeNoEgg,		// û�ж�Ӧ�ĵ�
	mbeTooMuch,		// ����������������
	mbeAddFailed,	// ��ӷ���ʧ��
	mbeAddSuccess,	// ��ӳɹ�
	mbeGetList,		// ��ȡ�б�
	mbeMyList,		// ��ȡ�Լ��б�
	mbePutEgg,
	mbeGetBackEgg,
	mbeGetBackSuccess,
	mbeAllList,		// �����б�
	mbeOnPageChanged,
	mbeAdult,		// �����ɹ�
	mbeMyEggList,   // ��ȡ���������Լ��б�
	mbeGetBackMySuccess, //�Ǳ��˵ķ����ɹ�

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
	// �Ի�����

	MB_NONE_L = 0,		// ����ʾ�κΰ�ť
	MB_OK_L,			// ��ʾ�м䰴ť�����OK��ť����MBRE_CENTER
	MB_CANCEL_L,		// ��ʾ�м䰴ť�����OK��ť����MBRE_CENTER
	MB_OKCANCEL_L,		// ��ʾ���Ұ�ť�����OK��ť����MBRE_LEFT	���Cancel����MBRE_RIGHT
	MB_OKCANCELRETRY_L,	// ��ʾ�����Ұ�ť�����OK��ť����MBRE_LEFT	���Cancel����MBRE_CENTER	���Retry����MBRE_RIGHT
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
	eMagicBabyBar,		// ��������������
	eMagicBabyExp,		// �������߾���
};

// ��ͨϵͳ
#define	TRANSPORT_MAX_PASSENGER		20

#define  MAGIC_BABY_FUSE_VALUE	65			//�����ں�ֵ
#define  MAGIC_BABY_FUSE_VALUE_CAL	66		//��������ֵ

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
//ע�����ʹ�����ű����������
#define GET_FUSE_LIMIT(level) ((level) * (level) * KGlobalParameter::Instance()->Get(MB_FUSE_LIMIT))

//װ�����ܵĸ��Ӻ�
#define EQUIP_SKILL_POS 100

// ���ɷ�������
#define BRANCH_COUNT_MAX			20
#define OTHER_EMPLOYEE				10

// ����ʱ���˵�ICON
#define TRADE_OTHER_ICON_ID			8

// ����������������
#define MB_SKILLGROUP_MAX			2

#define MB_TALENT_MAX				6

#define MB_TALENTNAME_MAX			16

#define PET_ICON_MAX			64

#define MB_TALENTATTR_MAX			5

#define MB_TALENTDES_MAX			128

// ��ͨ���츳����
#define MB_RELATION_OPENGENIUS		200

#define MB_RELATION_ZERO			1

#define MB_RELATION_LOWEST			0

#define MB_TALENT_ABILITY			128

enum E_SAVE_EVENTS
{
	E_SAVE_EVENT_BEGIN = 0,
	E_SAVE_EVENT_TELEPORT = E_SAVE_EVENT_BEGIN,			// �سǱ��������
	E_SAVE_EVENT_END,
};

#define MAIL_INVALID_RECEIVER		-1

///////////////////////////////�ʼ�ϵͳö��//////////////////////////
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
	errM_Net_Error,				// �������
	errM_Db_Error,				// ���ݿ����
	errM_Query_In_Part,			// ���������ʼ�δ����
	errM_Query_Accomplish,		// ���������ʼ�δ����
	errM_Send_No_Space,			// �ռ�����������
	errM_Send_No_Space_RTS,		// �ռ����������������˸�������
	errM_Send_No_Receiver,		// �ռ��˲�����
	errM_Send_No_Receiver_RTS,	// �ռ��˲����ڣ����˸�������
	errM_Send_No_Enough_Money,	// �޷��۳��㹻�Ľ�Ǯ
	errM_Send_Attachment_Oper,	// ������Ʒ��������
	errM_Delete_Fee_Nonzero,	// ���и���������Ҫ��ȡ���õ��ʼ�������ֱ��ɾ��
	errM_Fetch_Bag_No_Space,	// �����������޿ռ�
	errM_Fetch_No_Enough_Money,	// �޷��۳��㹻����ȡ����
	errM_RTS_Type_Unmatch,		// �����ʼ���֧������
	errM_RTS_Type_No_response,	// �����ʼ������Ӧ
	errM_RTS_OK,				// ���ʼ�������
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

//�츳��ĵ�ǰ״ֵ̬
enum TALENT_POINT_STATE
{
	enumTPS_Normal	= 0,		//����
	enumTPS_Invalid,			//��Ч
	enumTPS_NoPointLeft,		//��ʣ���츳��
	enumTPS_Full,				//������
	enumTPS_NeedParent,			//��Ҫ�Ӹ��츳�ſ���
	enumTPS_NotEnoughPtInSub,	//��ǰͶ����츳����
	enumTPS_RealmRestrict,		//���治��
};


enum ENUM_BILLING_ITEM_SCHOOL
{
	//��������ڽű���ֱ���õ����֣�������Ҫ���ʱ����������
	enum_Billing_Item_Free = 0,		// �����ȡ
	enum_Billing_Item_1,			// �̳ǽ����8������
	enum_Billing_Item_2,			
	enum_Billing_Item_3,			
	enum_Billing_Item_4,			
	enum_Billing_Item_5,			
	enum_Billing_Item_6,			
	enum_Billing_Item_7,			
	enum_Billing_Item_8,			
	enum_Billing_Item_Hot,			// ����
	enum_Billing_Item_Search,		// ͨ��������ӳ�����
	enum_Billing_Item_BagHot,		// ��������
	enum_Billing_Item_MagicBabyHot,	// ��������
	enum_Billing_item_StrengthenHot,// ǿ������
	enum_Billing_Item_InlayHot,		// ��Ƕ����
	enum_Billing_Item_OpenHot,		// ��������
	enum_Billing_Item_DestroyHot,	// �������
	enum_Billing_Item_StoneCompose,	// ��ʯ�ϳ�
	enum_Billing_Item_CharacterHot,	// ��ɫ����

};

// ǰ����ʱ�����һ����3��ʱ�䣨�룩
#define MAX_TIME_A_BIG_STEP		4
// �������ߵ�ʱ����ǰ��ø��߷������µ�Ŀ��㣨���룩
#define PRE_TIME_NOTIFY			300
// �ͻ���ÿ��n��֪ͨ������λ��
#define POS_TIME_NOTIFY			300
// �ٻ�Ĭ�ϵ�����ʱ��	30��
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
	enum_WISH_TYPE_NORMAL = 0,	//��ͨԸ��
	enum_WISH_TYPE_HIGH,		//�߼�Ը��
};

enum WISH_RESULT
{
	enum_WISH_RESULT_SUCCESS = 0,
	enum_WISH_RESULT_NET_ERROR,				// �������
	enum_WISH_RESULT_DB_ERROR,				// ���ݿ����
	enum_WISH_RESULT_ADD_ERROR,
	enum_WISH_RESULT_OVERTIME,
	enum_WISH_RESULT_NO_ENOUGH_PAPER,
	enum_WISH_RESULT_ADDWISH_RETURN,
};

enum WISH_SUPPORT_TYPE
{
	enum_WISH_SUPPORT = 0,		//��
	enum_WISH_UNSUPPORT,		//��
};

enum WISH_SEARCH_DIRECTION
{
	enum_WISH_FIRST_TIME = 0,	//���ҵ�һ��
	enum_WISH_LAST,				//���ϲ���
	enum_WISH_NEXT,				//���²���
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

// ��ս
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
	enum_Guild_Rewards_ID = 5104,			// ���ɽ���
	enum_Guild_Dungeon_ID,					// ���ɸ���

};

#define GUILD_EVENT_MAX		32

enum CYC_TIME_EVENT_TYPE
{
	enumCycEvent_VIPToItem1 = 20000,		// ����ȡ����
	enumCycEvent_VIPToItem2 = 20001,		// ������ȡ����
	enumCycEvent_Count,
};

enum ENUM_MB_ABSORB_TYPE
{
	enumMBAT_Hide = 0,				// ����
	enumMBAT_Show,					// ��ʾ

	enumMBAT_UpdateMaxValue1,		// �������ֵ
	enumMBAT_UpdateCurrentValue1,	// ������ǰֵ

	enumMBAT_UpdateMaxValue2,		// �������ֵ
	enumMBAT_UpdateCurrentValue2,	// ���ܵ�ǰֵ

	enumMBAT_UpdateMaxValue3,		// ���йһ��������ֵ
	enumMBAT_UpdateCurrentValue3,	// ���йһ����鵱ǰֵ
};

enum ENUM_MB_ABSORB_UI_TYPE
{
	enumMBAUT_1 = 0,	// ����������
	enumMBAUT_2,		// ����������
	enumMBAUT_3,		// ���йһ��Ӿ���
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
//	enum_guildmember_levelup	= 1,			//ְ������
//	enum_guildmember_leveldown	= 2,			//ְ�񽵼�
//	enum_guildmember_recruit	= 4,			//���ճ�Ա
//	enum_guildmember_kickout	= 8,			//������Ա
//	enum_guildmember_event		= 16,			//���ɴ���¹���
//	enum_guildmember_talent		= 32,			//���������츳
//	enum_guildmember_dungeon	= 64,			//�������ɸ���
//	enum_guildmember_callmember = 128,			//�ټ����ɳ�Ա
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

//����ϵͳ
#define	MAX_JACKPOT_VAL				10000000 // 1000��
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


//��Ϊ.����
#define MAX_CHIEFVEIN_COUNT	128
#define MAX_MINORVEIN_COUNT	128

enum enumMeridianType
{
	enumMeridianType_Invalid,
	enumMeridianType_ChiefVein,		//����
	enumMeridianType_MinorVein,		//����
	enumMeridianType_Count,
};

enum eSyncType
{
	eSyncNone,					// ��ͬ��
	eSyncMyself,				// ͬ�����Լ�
	eSyncAllSightPlayer,		// ͬ����������Ұ��Χ�ڵ���ң������Լ���
	eSyncAllSightPlayerButMe,	// ͬ����������Ұ��Χ�ڵ���ң��������Լ���
	eSyncToAI,					// ͬ����������AIServer

	eSyncToCurrentScene,		// ����ǰ����ͬ��
	eSyncToCurrentGame,		// ����ǰgameserverͬ��
	e_SyncToSyncServer,
};

enum enumLearnSkillCheck
{
	enumLSC_Money		= 1,			//������Ǯ
	enumLSC_Front		= 2,			//�����ǰ�ü���
	enumLSC_Class		= 4,			//��������ְҵ
	enumLSC_SubClass	= 8,			//��������ְҵ
	enumLSC_Nation		= 16,			//��������
	enumLSC_PlayerLevel	= 32,			//�������ҵȼ�
	enumLSC_FaithLevel	= 64,			//��������������
	enumLSC_Faith		= 128,			//������������ֵ
};

enum enumLearnSkillCheckResult
{
	enumLSCR_Success,					//���ͨ��
	enumLSCR_Faild,						//���δͨ��
	enumLSCR_Already,					//�Ѿ�ѧ��
	enumLSCR_CanNotStudy,				//����ѧϰ
	enumLSCR_Nation,					//����
	enumLSCR_Class,						//����ְҵ
	enumLSCR_SubClass,					//����ְҵ
	enumLSCR_PlayerLevel,				//�ȼ�
	enumLSCR_Money,						//��Ǯ
	enumLSCR_FaithLevel,				//������
	enumLSCR_Faith,						//����ֵ
	enumLSCR_FrontSkill,				//ǰ�ü���δѧ��
	enumLSCR_High,						//�Ѿ�ѧ����߼��ļ���
	enumLSCR_Top,						//�Ѿ�ѧ��,���Ǽ�������ߵȼ�����
};

enum enumNakedModeCategory
{
	enumNMC_None	= 0,					//���㱼ģʽ
	enumNMC_Paopao,							//����ģʽ

	enumNMC_Count,
};

enum enumPlayerAttrType
{
	enumPAT_LabourUnion,				//�ɹ�Э��
	enumPAT_ProLevel,					//�ɹ�Э��ȼ�
	enumPAT_ProLevelState,				//�ɹ�Э����ʾ��־
	enumPAT_ProExp,						//�ɹ�Э��רҵ����
	enumPAT_ProDesignedPrecision,		//�ɹ�Э��רҵר����
	enumPAT_ProTire,					//�ɹ�Э��ƣ�Ͷ�
};
