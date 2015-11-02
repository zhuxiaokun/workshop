/********************************************************************
	created:	2007/11/29
	created:	29:11:2007   18:05
	filename: 	Athena\Main\Src\Common\LogicWorld\Src\KCommonStruct.h
	file path:	Athena\Main\Src\Common\LogicWorld\Src
	file base:	KCommonStruct
	file ext:	h
	author:		xueyan
	
	purpose:	想在这里定义或引用一些通用的类和结构
*********************************************************************/

#pragma once

#include <System/KType.h>
#include <System/KMacro.h>
#include <System/Misc/rand.h>
#include <System/Misc/KDatetime.h>
#include <System/Collections/KString.h>
#include <System/Collections/KVector.h>
#include <System/Collections/DynArray.h>
#include <System/Memory/KStepObjectPool.h>
#include "KPoint.h"
#include "KCommonDefine.h"

#define CFG_ITEM_SPECIAL_TABLE_SPLIT	100000000	// 数据库中表分割数

#define CFG_MAX_TITLE_COUNT				(20*20)		// 称号的个数
#define	CFG_MAX_ITEMBASE_SKILL			8			// 最大技能数量
#define CFG_MAX_INLAY_FORMULA_PARM		(11)		// 最大镶嵌公式参数个数
#define CFG_MAX_ITEM_ATTACH_PARM_COUNT	(10)		// 物品附加属性最大个数

#define	CFG_MAX_ITEM_TRADE				6			// 玩家最大交易数量
#define CFG_MAX_ITEM_STRENGTH			5			// 强化界面里放物品的最大数量
#define CFG_MAX_ITEM_INLAY				5			// 镶嵌界面里放物品的最大数量
#define CFG_MAX_ITEM_MB_INLAY			3			// 法宝镶嵌界面里放物品的最大数量
#define CFG_MAX_ITEM_RABBET				2			// 开槽界面里放物品的最大数量
#define CFG_MAX_ITEM_BREAK				2			// 销槽界面里放物品的最大数量
#define CFG_MAX_ITEM_SPAR				7			// 宝石合成界面里放物品的最大数量
#define CFG_MAX_ITEM_COMPOSE			5			// 装备合成界面里放物品的最大数量
#define CFG_MAX_SHORTCUT				128			// 玩家快捷栏最多个
#define CFG_MAX_ITEM_MOVABLESHOP		30			// 玩家摆摊最多物品数量
#define CFG_MAX_TEMP_SHORTCUT			6			// 玩家临时快捷栏

#define CFG_MAX_SYNC_MULITITEMS			10			// 最大物品同步数量

#define MAX_ITEM_PACK					2

// 法宝相关
#define MAX_MAGIC_BABY_COUNT_PRE_PLAYER			6
#define MAX_MAGIC_BABY_SKILL_COUNT				20
#define MAX_MB_INLAY_STONE_COUNT				3
#define MAX_MB_WUXING_SKILL_COUNT				9	// 法宝的五行技能数

// 装备技能的最大缓冲大小
#define MAX_MB_EQUIP_SKILL_INFO_BUFFER_SIZE		100
#if MAX_MB_EQUIP_SKILL_INFO_BUFFER_SIZE > 255
#	error "MAX_MB_EQUIP_SKILL_INFO_BUFFER_SIZE bigger than 255 and can not store in one BYTE!!"
#endif

// ItemCreateInfo 需要使用的宏
#define MAX_ITEM_NAME_LEN				50
#define MAX_ITEM_DESC_LEN				50
#define	MAX_ITEM_SLOT_COUNT				6
#define MAX_PASSIVESKILL_COUNT			5
#define	MAX_ITEM_SUBATTR_COUNT			8
#define	MAX_ITEM_BASEATTR_COUNT			6
// #define	MAX_ITEM_WORLD_DROP_COUNT		12		// 2009-05-31 世界掉落组数量从4个增加到12个 冗余定义，应使用 MAX_WORLD_DROP_COUNT
#define	MAX_STORE_COUNT					256
#define	MAX_MOBILE_TRADE_COUNT			256
#define	MAX_CHAR_SKILL_COUNT			256
#define	MAX_CHAR_COMBATATTR_COUNT		256
#define	MAX_CHAR_BUFF_COUNT				32
// #define	MAX_MB_SKILL_COUNT				(6)
#define MAX_QUEST_COUNT_PER_PLAYER		20
#define MAX_QUEST_STATE_SIZE			20*1024
#define MAX_QUEST_DATA_SIZE				256
#define MAX_SKILL_CD_SIZE				1024
#define MAX_FORMULA_PER_PLAYER_SIZE		2048
#define MAX_SDOLFORMULA_DBBUFF_SIZE		2048


#define CFG_MAX_LIMIT_EVENT_RECORD		100		// 玩家使用某种东西或者技能最大记录条数
#define CFG_MAX_ACTION_SAVE_RECORD		100		// 玩家使用某种东西或者技能最大记录条数
#define CFG_MAX_CUSTOM_EVENTS_RECORD	128		// 玩家保存某些事件的最大数量
#define CFG_MAX_SETID_EVENT_RECORD		256		// 总套装最大数量
#define CFG_MAX_DUP_EVENT_RECORD		100
#define CFG_MAX_TALENT_POINT_BYTE_SIZE	60		// 玩家天赋点的最大值

// ------------------- 交通线路 --------------------------------------------------------------
// 

#define TRAFFIC_DEFAULT					0x0000
#define TRAFFIC_FLYMODEL1				0x0001
#define TRAFFIC_FLYMODEL2				0x0002
#define TRAFFIC_FLYMODEL_DEFAULT		TRAFFIC_FLYMODEL1
#define TRAFFIC_FLYMODEL				(TRAFFIC_FLYMODEL1|TRAFFIC_FLYMODEL2)
#define TRAFFIC_STATION					0x0004

// ------------------- 主角 --------------------------------------------------------------
// 数据库相关，与共享内存共用，修改时请注意，一旦变更，则GameServer和DBAgent都要重新编译

#define MOSAIC_ITEM(item)			((item).byState	= enum_ItemState_MoSaic)
#define FREE_ITEM(item)				((item).byState	= enum_ItemState_Free)
#define	IS_ITEM_FREE(item)          ((item).byState == enum_ItemState_Free)
#define	IS_ITEM_MOSAIC(item)        (((item).byState & enum_ItemState_MoSaic) != 0)

#define DEF_MAX_SEND_CHAR_COUNT		(4)
#define DEF_MAX_SEND_SERVER_COUNT	(20)

#define DEF_PLAYER_MAX_MONEY	20*10000*10000		// 20万锭

class KCharacter;

#pragma pack(push,1)

struct st_Aurora_Login
{
	//st_Aurora_Login(){Init();}
	void Init()
	{
		memset(account, 0, sizeof(account)); 
		memset(password, 0, sizeof(password));
	}
	char account[MAX_MD5_STRING];
	char password[MAX_SESSION_STRING];
};

struct st_SDO_Login
{
	//st_SDO_Login(){Init();}
	void Init()
	{
		memset(session, 0, sizeof(session)); 
	}
	char session[SDOA_MAX_SESSION_DEF];
};

struct st_LoginInfo
{
	st_LoginInfo()
	{
		Init();
	}

	void Init()
	{
		memset(this, 0, sizeof(st_LoginInfo));
	}

	DWORD dwType;
	union {
		st_Aurora_Login auroraLogin;
		st_SDO_Login sdoLogin;
	};
};

struct st_PassportKey
{
	st_PassportKey() { Init(); }
	void Init()
	{
		m_dwPassportID = 0;
		m_dwUserType = 0;
	}
	BOOL IsValid() const
	{
		return (m_dwPassportID > 0);
	}
	// 重载针对INT64的强制类型转换
	inline operator INT64() const
	{
		return (((INT64)m_dwUserType)<<32) + m_dwPassportID;
	}
	// 重载针对INT64的强制类型转换
	inline operator UINT64() const
	{
		return (((UINT64)m_dwUserType)<<32) + m_dwPassportID;
	}
	// 重载针对DWORD的强制类型转换
	inline operator DWORD() const
	{
		return m_dwPassportID;
	}
	inline BOOL operator ==(const st_PassportKey& other) const
	{
		return (m_dwPassportID == other.m_dwPassportID && m_dwUserType == other.m_dwUserType);
	}
	DWORD userType() const
	{
		return m_dwUserType;
	}
	DWORD accountId() const
	{
		return m_dwPassportID;
	}
	JG_C::KString<64> str() const
	{
		JG_C::KString<64> s;
		s.Printf("type:%u acct:%u", m_dwUserType, m_dwPassportID);
		return s;
	}

public:
	DWORD m_dwUserType;
	DWORD m_dwPassportID;
};

struct st_AreaGroup
{
	INT16 n16Area;
	INT16 n16Group;

	inline operator DWORD() // 重载针对DWORD的强制类型转换
	{
		DWORD dwTemp = n16Area;
		dwTemp = dwTemp * 0xFFFF + n16Group;
		return dwTemp;
	}
};

struct st_ServerAlias
{
	st_AreaGroup agValue;
	DWORD m_dwIp;
	//KString<MAX_IP_BUFF_SIZE> strIp;
};
#pragma pack(pop)


#pragma pack(push,1)
/************************************************************************/
/* 物品属性信息 KItemBase                                             */
/************************************************************************/

//*******************************************************************************************/	//物品
struct	KItemFlags
{
public:
	WORD GetValue() const
	{
		WORD wValue = 0;
		memcpy(&wValue, this, sizeof(WORD));
		return wValue;
	}

	KItemFlags()
	{
		memset( this, 0, sizeof(KItemFlags) );
	}
	KItemFlags(WORD wVal)
	{
		*this = wVal;
	}
	KItemFlags& operator=(WORD val)
	{
		memcpy(this, &val, sizeof(WORD));
		return *this;
	}

	WORD mFlag_Bind : 1;			// 绑定
	WORD mFlag_Preserving : 15;
};

//********************************************/
struct KItemBase
{
	enum ENUM_ITEM_TYPE
	{
		enumItemBase = 0,
		enumSimpleItem,
		enumNormalItem,
		enumPetItem,
	};

	BYTE					byType;					// 物品类型
	BYTE					byLen;					// 长度
	BYTE					byPos;					// 物品位置
	WORD					wItemID;				// 物品类别ID
	WORD					wItemNum;				// 物品堆叠数量
	INT128					nSerialCode;			// 物品唯一ID
	KItemFlags				mPrivateFlags;			// 物品特殊功能标记

	KItemBase()
		: wItemID(0)
		, wItemNum(0)
		, byPos(-1)
		, mPrivateFlags(0)
		, byType(enumItemBase)
		, byLen(sizeof(KItemBase))
	{
	}

	KItemBase(int itemPos)
		: wItemID(0)
		, wItemNum(0)
		, mPrivateFlags(0)
		, byPos(itemPos)
		, byType(enumItemBase)
		, byLen(sizeof(KItemBase))
	{
	}

	bool IsKindof(int itemtype) const { return (byType == itemtype); }
};
typedef KItemBase const * KConstItemBasePtr;

#define MULTIITEM_MAXCOUNT 16
struct KMultiItemData
{
	KMultiItemData()
	{
		memset(this , 0 , sizeof(KMultiItemData));
	}
	BOOL	AddItemData(INT dwItemID , INT dwItemNum)
	{
		if (ItemCount + 1 >= MULTIITEM_MAXCOUNT)
		{
			return FALSE;
		}
		for (int i = 0; i < ItemCount; i++)
		{
			if (ItemID[i] == dwItemID)
			{
				ItemNum[i] += dwItemNum;
				return TRUE;
			}
		}
		ItemID[ItemCount] = dwItemID;
		ItemNum[ItemCount]= dwItemNum;
		ItemCount++;
		return TRUE;
	}
	int	ItemCount;
	INT	ItemID[MULTIITEM_MAXCOUNT];
	INT	ItemNum[MULTIITEM_MAXCOUNT];
};

#define DEF_NEW_SITEM_KEEP_SIZE	(10)
struct	KDBSimpleItem : public KItemBase, public System::Memory::KPortableStepPool<KDBSimpleItem,16>
{
	INT8					nFrequency;				// 物品当前使用次数
	INT32					nAliveSeconds;			// 道具剩余生存时间
	
	union
	{
		INT8				nReserve[DEF_NEW_SITEM_KEEP_SIZE];// 保留长度用
	};

	KDBSimpleItem()
		: KItemBase()
		, nFrequency(0)
		, nAliveSeconds(0)
	{
		byType = enumSimpleItem;
		byLen = sizeof(KDBSimpleItem);
		memset( nReserve, 0, sizeof(nReserve) );
	}

	KDBSimpleItem(int itemPos)
		: KItemBase()
		, nFrequency(0)
		, nAliveSeconds(0)
	{
		byPos = itemPos;
		byType = enumSimpleItem;
		byLen = sizeof(KDBSimpleItem);
		memset( nReserve, 0, sizeof(nReserve) );
	}

	void Clear()
	{
		new (this) KDBSimpleItem();
	}
};
DECLARE_SIMPLE_TYPE(KDBSimpleItem);
typedef KDBSimpleItem const * KConstSimpleItemBasePtr;

struct KDBItemEquip
{
	INT16					nSkillID;					// 物品技能ID
	INT16					nCurrentDur;				// 当前耐久度
	INT8					nStrengthLevel;				// 物品强化等级
	CHAR					szMakerName[MAX_NAME_DEF];	// 制作者名字

	struct KAttrBase
	{
		INT16 mIndex;
		INT8 mValue;
	};

#define DEF_EQUIP_MAX_BASE_ATTR	(2)
#define DEF_EQUIP_MAX_BASE_ATTR_VALUE1	(31)
#define DEF_EQUIP_MAX_BASE_ATTR_VALUE2	(32)
	// 装备基础属性
	struct KEquipBaseAttr
	{
		INT8 mAttr[DEF_EQUIP_MAX_BASE_ATTR];
	} mEquipBaseAttr;

#define DEF_EQUIP_MAX_ATTR_EX	(4)
#define DEF_EQUIP_MAX_ATTR_EX_VALUE1	(33)
#define DEF_EQUIP_MAX_ATTR_EX_VALUE2	(34)
	// 装备辅助属性
	struct KEquipAttrEx
	{
		INT8 mAttr[DEF_EQUIP_MAX_ATTR_EX];
	} mEquipAttrEx;

#define DEF_EQUIP_MAX_SANCTIFICATION	(5)

	struct	KEquipSanctificationSlot
	{
	public:
		KEquipSanctificationSlot& operator=(INT8 val)
		{
			memcpy(this, &val, sizeof(BYTE));
			return *this;
		}
		INT8 GetValue() const
		{
			INT8 nValue = 0;
			memcpy(&nValue, this, sizeof(INT8));
			return nValue;
		}
		INT8 mSlot1 : 1;
		INT8 mSlot2 : 1;
		INT8 mSlot3 : 1;
		INT8 mSlot4 : 1;
		INT8 mSlot5 : 1;
		INT8 byReserve : 3;
	};

	// 物品灵性属性
	struct KEquipSanctification
	{
	public:
		struct KAttrSanctification
		{
			INT8 mIndex;
			INT16 mValue;
		};

		KEquipSanctificationSlot mSlot1;
		KAttrSanctification mAttr[DEF_EQUIP_MAX_SANCTIFICATION];
	} mEquipSanctification;

#define DEF_EQUIP_MAX_DEVELOPS	(2)
	struct KEquipDevelops
	{
		// 装备养成属性
		struct KEquipDevelop
		{
			INT16 mIndex;
			INT32 mExp;
		} mDevelop[DEF_EQUIP_MAX_DEVELOPS];
	} mEquipDevelops;
};

struct KDBShenXingFu
{
	INT32 mnMapID;
	INT32 mnVillageID;
	float mfX;
	float mfY;
	CHAR szRemark[MAX_NAME_DEF];
};

struct KExpJar
{
	INT32 mnExp;
};

#define DEF_NEW_ITEM_KEEP_SIZE	(98)
struct KDBNormalItem : public KItemBase, public System::Memory::KPortableStepPool<KDBNormalItem,16>
{
	INT8					nFrequency;			// 物品当前使用次数
	INT32					nAliveSeconds;		// 道具剩余生存时间

	union
	{
		KDBItemEquip		mEquipInfo;						// 装备物品信息
		KDBShenXingFu		mShenXingFu;					// 神行符
		KExpJar				mExpJar;						// 经验罐
		INT8				mReserve[DEF_NEW_ITEM_KEEP_SIZE];// 保留长度用
	};

	KDBNormalItem()
		: KItemBase()
		, nFrequency(0)
		, nAliveSeconds(0)
	{
		byType = enumNormalItem;
		byLen = sizeof(KDBNormalItem);
		memset( mReserve, 0, sizeof(mReserve) );
	}

	KDBNormalItem(int itemPos)
		: KItemBase(itemPos)
		, nFrequency(0)
		, nAliveSeconds(0)
	{
		byType = enumNormalItem;
		byLen = sizeof(KDBNormalItem);
		memset( mReserve, 0, sizeof(mReserve) );
	}

	void Clear()
	{
		new (this) KDBNormalItem();
	}
};
DECLARE_SIMPLE_TYPE(KDBNormalItem);
typedef KDBNormalItem const * KConstNormalItemPtr;


// 宠物数据库结构定义
struct KDBPetItem : public KItemBase, public System::Memory::KPortableStepPool<KDBPetItem,3>
{
	INT32	nAliveSeconds;			// 道具剩余生存时间

	BYTE	byVer;						// 版本信息
	// WORD	wType;						// 类型 wItemID
	BYTE	byElement;					// 法宝五行
	UINT64  uiGUID;						//法宝唯一标识
	DWORD	dwCreateTime;				// 法宝创建时间
	char	strName[MAX_NAME_DEF];		// 名字

	WORD	wAura;		// 灵力
	WORD	wQualityAtk;
	WORD	wQualityDef;
	WORD	wQualityHp;
	INT		nHp;
	INT		nMp;
	int		nn;			// 当前饱食度
	BYTE	byColor : 4;		// 16种颜色
	BYTE	byLevel;
	BYTE	byll;		// 进化阶段
	BYTE	bya;		// 满意度
	BYTE	byba;		// 不满意度
	BYTE	byIsShow;	// 法宝是否出战
	DWORD	dwExp;

	BYTE	byEquipLevel;
	BYTE	byAuraLevel;	// 当前品质等级

	BYTE	byEndPos;

	KDBPetItem()
		: KItemBase()
		, nAliveSeconds(0)
	{
		byType = enumPetItem;
		byLen = sizeof(KDBPetItem);
	}

	KDBPetItem(int itemPos)
		: KItemBase(itemPos)
		, nAliveSeconds(0)
	{
		byType = enumPetItem;
		byLen = sizeof(KDBPetItem);
	}

	void InitPetData()
	{
		for (BYTE* p = &byVer; p <= &byEndPos; p++)
		{
			*p = 0;
		}
		//memset(&byVer, (int)&byEndPos - (int)&byVer, 0);
	}
	void Clear()
	{
		new (this) KDBPetItem();
	}
};
DECLARE_SIMPLE_TYPE(KDBPetItem);
typedef KDBPetItem const * KConstPetItemPtr;

/************************************************************************/
// 玩家物品交易物品存储的存储结构                                       */
/************************************************************************/
struct KTradeItemInfo_S 
{
	int		m_Money;
	int		m_ConfirmLevel;
	BYTE	m_PosType[CFG_MAX_ITEM_TRADE];
	BYTE	m_Pos[CFG_MAX_ITEM_TRADE];
	WORD	m_CheckSum[CFG_MAX_ITEM_TRADE];
	void Clear()
	{
		m_Money = 0;
		m_ConfirmLevel = 0;
		memset(m_PosType, 0, sizeof(m_PosType));
		memset(m_Pos, 0, sizeof(m_Pos));
		memset(m_CheckSum, 0, sizeof(m_CheckSum));
	}
	BOOL SetData(int index, int bagtype, int pos, WORD checksum = NULL)
	{
		if (index < 0 || index >= CFG_MAX_ITEM_TRADE)
			return FALSE;

		m_PosType[index] = bagtype;
		m_Pos[index] = pos;
		m_CheckSum[index] = checksum;
		return TRUE;
	}
	BYTE GetPosType(int index)
	{
		if (index < 0 || index >= CFG_MAX_ITEM_TRADE)
			return FALSE;

		return m_PosType[index];
	}

	BYTE GetPos(int index)
	{
		if (index < 0 || index >= CFG_MAX_ITEM_TRADE)
			return FALSE;

		return m_Pos[index];
	}

	BOOL IsSame(int index, WORD checksum)
	{
		if (index < 0 || index >= CFG_MAX_ITEM_TRADE)
			return FALSE;

		if (checksum != m_CheckSum[index])
			return FALSE;
		return TRUE;
	}

	BOOL IsEmpty(int index)
	{
		if (index < 0 || index >= CFG_MAX_ITEM_TRADE)
			return TRUE;

		return m_PosType[index] == enumBlock_Inviad;
	}
};

struct ITEM_INTERACTIVE_INFO
{
	ITEM_INTERACTIVE_INFO() { Init(); }
	void Init() { m_byBagType = 0; m_byBagPos = 0; }
	BYTE m_byBagType;	// 背包页类型
	BYTE m_byBagPos;	// 背包对应页格子号
};

struct KTitle_Data
{
	KTitle_Data() { memset(this , 0 , sizeof(KTitle_Data)); }
	BYTE m_byTitleClassID;
	BYTE m_byTitleID;
	BYTE m_byTitleLevel;
	DWORD m_dwRemainTime;
};

// 物品统计信息
struct KItemStatisticsInfo
{
	short mStaticMaxCount;
	short mDynamicMaxCount;
	short mCurrentCount;
	short mNextEmptyIndex;
};

// 物品格子
template<typename T, int count>
struct KNewItemSlot
{
	KNewItemSlot() {memset(this , 0 , sizeof(KNewItemSlot));}
	KItemStatisticsInfo mInfo;
	T m_Items[count];
};

struct KShortcutData
{
	KShortcutData()
	{
		memset(this , 0 , sizeof(KShortcutData));
	}

	enum enumShortcutType
	{
		enumShortcutType_Nothing = 0,
		enumShortcutType_Item,
		enumShortcutType_Skill,
		enumShortcutType_SkillGroup,
		enumShortcutType_AuroraRunSkill,
		enumShortcutType_Count,
	};

	BYTE		byShortcutType;
	WORD		wID;

	BOOL IsItemShortcut() const			{ return byShortcutType == enumShortcutType_Item && wID != 0;	}
	BOOL IsSkillShortcut() const		{ return byShortcutType == enumShortcutType_Skill && wID != 0;	}
	BOOL IsEmpty() const				{ return byShortcutType == enumShortcutType_Nothing || wID == 0;}
	BOOL IsSkillGroupShortcut() const	{ return byShortcutType == enumShortcutType_SkillGroup && wID != 0;	}
	WORD GetID() const			{ return (WORD)wID;}	
};
DECLARE_SIMPLE_TYPE(KShortcutData)

struct KItemHandles
{
	KItemHandles()
	{
		memset(this, 0  , sizeof(KItemHandles));
	}
	DWORD		m_pBagNormalItems[CFG_MAX_BAG_NORMAL_ITEM_COUNT];
	DWORD		m_pBagMatItems[CFG_MAX_BAG_MATERIAL_ITEM_COUNT];
	DWORD		m_pBagQuestItems[CFG_MAX_BAG_QUEST_ITEM_COUNT];
	DWORD		m_pBagEquipItems[CFG_MAX_BAG_EQUIP_ITEM_COUNT];
	DWORD		m_pBankNormalItems[CFG_MAX_BANK_NORMAL_ITEM_COUNT];
	DWORD		m_pBankMatItems[CFG_MAX_BANK_MATERIAL_ITEM_COUNT];
	DWORD		m_pTradeItems[CFG_MAX_ITEM_TRADE];
	DWORD		m_pStrengthItems[CFG_MAX_ITEM_STRENGTH];
	DWORD		m_pInlayItems[CFG_MAX_ITEM_INLAY];
	DWORD		m_pMBInlayItems[CFG_MAX_ITEM_MB_INLAY];
};


// 限制玩家每天或者一定时间内只能执行几次的事件记录，需要保存到数据库
struct KLimitEventRecord 
{
	WORD		m_LimitEventID;
	WORD		m_LimitEventCount;
	DWORD		m_LastRefreshTime;
};

struct KQuestIDDBInfo 
{
	DWORD	dwQuestID;
};

struct KQuestStateDBInfo
{
	DWORD	dwQuestID;
	DWORD	dwQuestState;
};

struct KActionDataRecord 
{
	enum
	{
		enum_Activity_Random = 0,
		enum_Activity_Quality = 1,
		enum_Activity_Count = 2,
	};
	DWORD		m_ActionTime;
	DWORD		m_ActionID;
	BYTE		m_byType;
	BYTE		m_byReserver;
	SHORT		m_ActionCount;
};

struct KPrestigeRecord
{
	DWORD		m_PrestigeID;
	DWORD		m_FinishTime;
};

struct KCustomEventRecord
{
	DWORD		m_EventID;
	DWORD		m_EventValue;
};

struct KSetIDEventRecord
{
	DWORD		m_SetID;
	DWORD		m_SetIDValue;
};

struct KDupEventRecord 
{
	DWORD		m_MapID;
	DWORD		m_EventID;
	DWORD		m_Value;
};

#pragma pack(pop)


// NPC商店出售物品
struct ITEM_STORE
{
	WORD			wItemID;								// 商品ID
	BYTE			byCount;								// 限量商品数量，0表示不限量
	BYTE			byRefreshTime;							// 刷新时间：单位小时
};

/************************************************************************/
/* 物品初始化属性信息        ItemCreateInfo                             */
/************************************************************************/
struct ItemSubAttr					// For ItemCreateInfo
{
	int				Attr_P1;	// attr id
	int				Attr_P2;
	int				Attr_P3;
	int				Attr_P4;
};

#pragma pack(push,1)
struct KPlayerSkillDBInfo
{
	WORD	wSkillID;
};
struct KPlayerBuffDBInfo
{
	WORD	wBuffID;
	DWORD	dwRemainTime;	//单位ms
	DWORD	dwSaveTime;		//单位s，系统时钟(time(0))
	BYTE	byStacks;		//叠加层数
	BYTE	byTriggerTimes[3];	//触发器触发次数计数
};
#pragma pack(pop)



const int MAX_SKILL_CLASS            = 15;
const int NONFIGHT_SKILL_CLASS_ID    = (MAX_SKILL_CLASS-1);

const int MAX_SKILL_TERM             = 5;
const int MAX_OCCUPATION_SKILL_CLASS = 4;
const int MAX_SKILL_GROUP_CAPACITY   = 8;	// 每组可容纳的技能上限
const int MAX_SKILL_GROUP_NUM        = 10; // 每个玩家可拥有的技能组数
const int UI_PAGE_SKILL_CAPACITY     = 12; // 每页显示多少技能
const int INVALID_SKILL_ID           = 0; // 无效的技能ID

#pragma pack(push,1)
struct KPlayerBaseDBInfo
{
	DWORD	CT_ID;
	int		CT_ACCID;
	int		CT_USERTYPE;

	char	CT_NAME[MAX_NAME_DEF];
	char	CT_SEX;
	BYTE	CT_CLASS;				// 职业

	BYTE	CT_ORIENTATION;			// 分支
	BYTE	CT_NATIONALITY;			// 国籍
	BYTE	CT_MODEL;				// 模型
	BYTE	CT_HAIRSTYLE;			// 发型
	BYTE	CT_COMPLEXION;			// 肤色
	BYTE	CT_HAIRCOLOR;			// 发色
	BYTE	CT_FACESTYLE;			// 脸型
	BYTE	CT_PRINK;				// 化妆


	DWORD	CT_EXP;
	DWORD	CT_REALM_EXP;		// 修真经验
	INT64	CT_MAINMONEY;
	INT64	CT_OFFMONEY;

	int		CT_X;
	int		CT_Y;
	int		CT_Z;

	BYTE	CT_LEVEL;			// 等级
	BYTE	CT_REALM_LEVEL;		// 修真等级
// 	BYTE	CT_TALENT;			// 根骨
// 	BYTE	CT_INTELLIGENCE;	// 灵性
	INT		CT_RP;				// 第三能量
	INT		CT_HP;				// 当前生命值
	INT		CT_MP;				// 当前法力值
	DWORD	CT_PORTALID;		// 
	DWORD	CT_MASTERLEVEL;		// 玩家权限
	INT		CT_State;			// 保存玩家状态
	DWORD	CT_VIPLEVEL;		// 玩家VIP级别
	BYTE	CT_Energy;			// 精力(用于世界频道聊天)
	INT		CT_LastEnergyRestoreTime;
	BYTE	CT_ShowHelm;		// 显示帽子
	DWORD	CT_ECP;				// 当前奇经点数
//	DWORD	CT_FaithLevel;		// 修真等级
	BYTE	CT_ShowFanshion;	// 显示时装
	DWORD	CT_DeathPunishmentPlan;	// 本次死亡惩罚
	DWORD	CT_dwTireVal;			// 疲劳值
	INT		CT_CanCostCultivation;		//修为值(可消耗)
	INT		CT_CanNotCostCultivation;	//修为值(无消耗)
	BYTE	CT_RPActive;				//第三能量系统是否激活
};
#pragma pack(pop)

#pragma pack(push,1)
struct KMagicBabyDBInfo
{
	DWORD	dwID;
	DWORD	dwTypeID;
	char	strName[MAX_NAME_DEF];
	WORD	wPos;

	SHORT	shLevel;
	SHORT	shFaithLevel;				// 修为等级
	DWORD	dwCurExp;					//当前经验值
	DWORD	dwCurFaith;					// 修为

	INT		nHP;						// 当前生命值--*
	INT		nMP;						// 当前法力值
	INT		nRP;						// 当前怒气，也就是灵力值

	INT		nNimbusPower;				// 灵力初值
	INT		nNimbusPowerAddGene;		// 灵力资质 enumMBBI_MB_NIMBUS_POWER_ADD_GENE
	INT		nNimbusPowerLevel;			// 灵力资质星级--*
	INT		nAffinity;					// 亲密当前值

	BOOL	bIsShow;					// 是否处于出战状态

// 	// U代表技能格中的技能ID
// 	DWORD	dwUSkillID[MAX_MAGIC_BABY_SKILL_COUNT];
// 	// L代表学会的技能ID
// 	DWORD	dwLSkillID[MAX_MAGIC_BABY_SKILL_COUNT];
// 	// 目前1技能格代表打开，0代表关闭
// 	BYTE	byteSkillCell[MAX_MB_WUXING_SKILL_COUNT];

	INT		nInlayStone[MAX_MB_INLAY_STONE_COUNT]; //--*

	INT		nElementVal[SOUL_TYPE_COUNT];	//吸魂的五星魂魄值

	BYTE	byEquipInfo[MAX_MB_EQUIP_SKILL_INFO_BUFFER_SIZE];	//法宝技能相关，使用的是如下的结构体内数据
	//typedef struct _MB_SKILL	//2009/12/30定义，可能游戏内部会重新构建这个结构体
	//{
	//	u_char		s_nSize;				//有效数据的长度
	//	u_int		s_nNormalAttack;		//普通攻击的技能
	//	u_int		s_nSkill[4];			//正在使用的技能
	//	u_int		s_nSkillLearn[9];		//已经学会的技能
	//	u_int		s_nSkillUnique;			//大招，传说中的大招
	//	u_int		s_nSkillPassive[3];		//被动技能
	//	u_char		s_chReserve[27];		//留待扩展
	//	_MB_SKILL()
	//	{
	//		memset( this, 0, sizeof(_MB_SKILL) );
	//	}
	//} MB_SKILL, *P_MB_SKILL;
	DWORD	dwLastAbsorbTime;			// 上一次吸灵的时间
	DWORD	dwAbsorbToday;				// 今天一共吸的值

	DWORD	dwLastReceiveTime;			// 上一次收妖的时间
	DWORD	dwReceiveToday;				// 今天一共收妖收的值

	int		nElement;		// 法宝五行
	int		nSavvy;			// 法宝悟性
	int		nRelation;		// 法宝亲密度
	int		nSpecialFace;	// 双A法宝
	int		nFuse;			//融魄值（当前值）
	DWORD	dwCreateTime;	// 法宝创建时间
	UINT64  uiGUID;			//法宝唯一标识
	int		nSkillGroup[MB_SKILLGROUP_MAX];	//法宝技能组
	int		nSkillGroupPoints;	//技能点（当前值）
	BYTE	byMBSpecialZZ;		//资质品质（同装备的区分等级：蓝装、紫装等）
	BYTE	byMBSpecialLL;		//灵力品质
	WORD	wFire;				//狂舞值
	int		nTalent[MB_TALENT_MAX];	//天赋值
	int		nTalentGrid;	//天赋值
	int		nAbility;
	int		nFirstValue;
	int		nSkillValue;
	int		nFightValue;
	int		nEvalue;
	int		nBind;
	char	skillCDInfo[52];	// 存储时间(4byte) + 4组CD时间(每组12byte)
	BYTE	byAutoSkillQueueInfo;	// 自动施放技能的设定
	int		nCurSoulByDay;
	BYTE	byEatCount;			// 每日化灵次数
	BYTE	byEatValue;			// 增加的灵力值
	BYTE	byResv[2];
	int		nEatTime;			// 化灵时间
	int		nEatTotalTime;		// 化灵最大时间
	WORD	wMakerID;
	WORD	wResv;


	VOID	MBDBInit()
	{
		memset(this , 0 ,sizeof(KMagicBabyDBInfo));
	}
	KMagicBabyDBInfo()
	{
		MBDBInit();
	}

};
#pragma pack(pop)

struct KMagicBabyGroupDBInfo
{
	KMagicBabyDBInfo	MagicBabyGroupData[MAX_MAGIC_BABY_COUNT_PRE_PLAYER];
	// 这组ID为玩家登陆时最初始的法宝栏中的法宝ID，用来退出时，进行比较删除的。
	DWORD				MagicBabyOldID[MAX_MAGIC_BABY_COUNT_PRE_PLAYER];

	VOID	MBGroupInit()
	{
		memset(this , 0 ,sizeof(KMagicBabyGroupDBInfo));
	}
	KMagicBabyGroupDBInfo()
	{
		MBGroupInit();
	}

	BOOL DeleteCheck(DWORD dwCheckID)
	{
		// 为真，就是要删除了。
		BOOL bRet = TRUE;
		for( int i = 0; i < MAX_MAGIC_BABY_COUNT_PRE_PLAYER; i++ )
		{
			if ( dwCheckID == MagicBabyGroupData[i].dwID )
			{
				bRet = FALSE;
				break;
			} // end if
		} // end for i
		return bRet;
	}

};

struct STRU_BUFF_INFO_FOR_DISPLAY
{
	INT m_nBuffID;		// BuffID
	INT m_nSrcID;		// srcID
	INT	m_nStacks;		// 叠加层数
	INT	m_nRemainTime;	// 剩余时间
	INT	m_dwBuffDuration;	// 总时间
	INT m_TooltipTextID;// ToolTip
	char m_szBuffIcon[MAX_MODEL_NAME_DEF + 1];	// Buff图标

	INT	m_nDuraType;		// 
	DWORD m_dwExpireTime;
};

struct STRU_BUFFS_INFO_FOR_DISPLAY
{
	int m_nBuffCount;			// 现有Buff总数
	STRU_BUFF_INFO_FOR_DISPLAY m_struBuffInfo[MAX_CHAR_BUFF_COUNT];
};

struct KPlayerAttr
{
	VOID	Init() {memset(this, 0, sizeof(KPlayerAttr));}
	DWORD	dwID;						// 角色ID
	CHAR	szName[MAX_PATH];			// 角色昵称
	CHAR	szCyberCafeName[MAX_PATH];	// 角色VIP网吧名称
	CHAR	szHeadImage[MAX_MODEL_NAME_DEF];	// 角色头像图
	BYTE	byClass;				// 职业
	BYTE	byLevel;				// 等级
	BYTE	byTalent;				// 根骨
	BYTE	byIntelligence;			// 灵性
	BYTE	byNationality;			// 国籍
	BYTE	bySubClass;				// 分支
	BYTE	bySex;					// 性别
	BYTE	byPrink;				// 化妆
	BYTE	byFaceStyle;			// 脸型
	BYTE	byHairStyle;			// 发型
	BYTE	byHairColor;			// 发色
	BYTE	byTeamPowerLtd;			// 队伍权限
	DWORD	dwTeamID;				// 队伍ID
	int		nHP;
	int		nMP;
	int		nRP;
	int		nMaxHP;
	int		nMaxMP;
	int		nMaxRP;
	int		nExp;					// 当前经验
	int		nNextExp;				// 下级别经验
	float	fX;
	float	fY;
	float	fZ;
	int		nAttr[6];				//属性A~E
	int		HPR;					//生命恢复率
	int		MPR;					//法力恢复率
	int		DHR;					//致命率
	int		CHR;					//会心率
	int		P_DMin;					//物攻Min
	int		P_DMax;					//物攻Max
	int		I_P_AC;					//护甲穿透
	int		P_AC;					//护甲
	int		P_DR;					//闪避率
	int		M_DMin;					//法攻Min
	int		M_DMax;					//法攻Max
	int		I_M_AC;					//抗性穿透
	int		M_AC;					//抗性
	int		M_DR;					//抵抗率
	DWORD	dwMainMoney;			// 非绑定金钱
	DWORD	dwOffMoney;				// 绑定金钱
	DWORD	dwTypeID;				// 角色类型ID
	DWORD	dwGmLevel;				// GM等级

	//法宝专用
	BYTE	byteMBPos;				// 法宝位置
	char	strMBIcon[MAX_PATH];	// 法宝图标
	char	strMBElementDis[MAX_NAME_DEF];	// 法宝元素显示

	char	strCLIcon[MAX_PATH];	// 法宝图标
	char	strRLIcon[MAX_PATH];	// 法宝图标
	char	strSLIcon[MAX_PATH];	// 法宝图标

	BYTE	byteMBPowerLevel;		// 法宝资质等级
	INT		nMBPowerAddGene;		// 法宝资质
	INT		nMBUseLevel;			// 法宝使用等级
	INT		nMBNimbusPower;			// 法宝灵力
	INT		nMBBaseNimbusPower;		// 法宝基础灵力
	INT		nMBIncreaseNimbusPower;	// 法宝成长灵力
	BOOL	bMBIsShow;				// 法宝是否出战

	WORD	mw_FaithCurrentExp;
	WORD	mw_FaithNextExp;
	BYTE	mby_FaithLevel;
	BYTE	mby_CanRealCombat;		// 是否真的可战斗
	BYTE	mby_CanCombat;			// 是否可战斗
	BYTE	mby_CanAbsorb;			// 是否可吸灵

	BYTE	mby_CanRide;			// 是否可骑乘
	BYTE	mby_CanRefine;			// 是否可炼化
	BYTE	mby_CanInlay;			// 是否可镶嵌
	BYTE	mby_CanStunt;			// 是否可绝技
	BYTE	mby_CanContract;		// 是否可契约

	BYTE	mby_CanCombatLv;		// 是否可战斗Lv
	BYTE	mby_CanRideLv;			// 是否可骑乘Lv
	BYTE	mby_CanRefineLv;		// 是否可炼化Lv
	BYTE	mby_CanInlayLv;			// 是否可镶嵌Lv
	BYTE	mby_CanStuntLv;			// 是否可绝技Lv
	BYTE	mby_CanContractLv;		// 是否可契约Lv

	// npc专用
	SHORT	m_nNpcAttackType;		// npc攻击类型
	int		m_nFightState;			// 是否战斗中
	BYTE	m_nFiveElement;			// 五行属性
	BYTE	m_byEliteLevel;			// 精英等级
	BYTE	m_byAbility;			//
	BYTE	m_byIsOwner;			//

	int		m_nGuildID;
	int		m_nGuildFunc;
	int		m_nGuildLevel;

	STRU_BUFFS_INFO_FOR_DISPLAY m_struBuffInfo;
	STRU_BUFFS_INFO_FOR_DISPLAY m_struDebuffInfo;
	//学会的五行技能
	INT m_GirdSkillID[MAX_MB_WUXING_SKILL_COUNT];
	//学会的专精技能
	INT m_PassiveSkillID[MAX_MB_WUXING_SKILL_COUNT];
	//学会的绝技
	INT m_UniqueSkillID[MAX_MB_WUXING_SKILL_COUNT];
	INT m_nWuxingVal[SOUL_TYPE_COUNT];
	INT m_EquipSkillID[8];
	INT m_nReceiveVal;
	INT m_nReceiveValPerDay;
	INT m_nRelationVal;
	INT	m_nLLPZ;
	INT m_nZZPZ;
	INT m_nMBPZ;
	INT m_nFireState;
	INT m_nBind;
	INT m_nFuse;
	INT m_nMaxFuse;
	INT m_nFirstValue;
	INT m_nSkillValue;
	INT m_nFightValue;
	INT m_nEValue;
	INT m_nMBTalent[MB_TALENT_MAX];
	char strAbility[MB_TALENT_ABILITY];
	char strAbilityName[MB_TALENT_ABILITY];

	INT m_nCurrentElement[enum_COUNT];
	INT m_equipLevel;

	DWORD m_dwTireVal;
	INT m_nCurSoulByDay;
	INT m_nSoulMax;
	INT m_nTalentRate;
	DWORD m_dwTireLimit;
	DWORD mdPostID;
	INT m_nFPC;
	INT m_nFPNC;
	INT m_nFPP;
};


class KCommonStruct
{
public:
	KCommonStruct(void);
	~KCommonStruct(void);
};

struct KCharUpdateParam
{
public:
	KCharUpdateParam() :pointPos(), dwCharID(0), dwSrcID(0), m_dwCategory(0), nCurrentLife(0),
		nCurrentMana(0), nCurrentAIBaseState(0), nCurrentAIState(0) {}
	K3DPoint	pointPos;
	DWORD		dwCharID;
	DWORD		dwSrcID;
	DWORD		m_dwCategory;
	INT			nCurrentLife;
	INT			nCurrentMana;
	BYTE		nCurrentAIBaseState;
	BYTE		nCurrentAIState;
};

struct KMovePlayerParam
{
	K3DPoint	pointPos;
	int			nDirection;
};

#pragma pack(push,1)
struct Equipment_3DShow 
{
	WORD	m_EquipmentID;
	WORD	m_EquipmentPos;
	BYTE	m_byStrengthLv;
	BYTE	m_byItemQuality;
};
#pragma pack(pop)

struct KPlayerParam
{
	DWORD		dwCharID;
	char		szName[MAX_NAME];
	DWORD		dwNetID;
	K3DPoint	pointPos;
	K3DPoint	pointTargetPos;
	unsigned short nDirection;
	unsigned short nDirectionZ;
	unsigned short nState;
	ANIMSTATUS keyMoveStatus;
	BYTE		byNationality;
	BYTE		byModel;
	BYTE		byMoveType;
	BYTE		byIsFly;
	DWORD		dwStateTime;
	Equipment_3DShow	equip_3DShow[enumEE3_Count];
};

struct KPlayerExpParam
{
	DWORD		dwCharID;
	BYTE		byLevel;
	BYTE		isLevelUp;
	int			nGetExp;
	DWORD		dwExp;
	DWORD		dwNextExp;
	DWORD		dwTeamExp;
	BYTE		byIsExtra;
};

struct KPlayerReliveParam
{
	DWORD		dwCharID;
	BYTE		byReliveType;
	float		fX;
	float		fY;
	float		fZ;
};

struct KPlayerLifeParam
{
	DWORD		dwID;
	BYTE		byType;
	BYTE		byResv[3];
	int			nHp;
	int			nMaxHp;
	int			nMp;
	int			nMaxMp;
	int			nRp;
	int			nMaxRp;
};

struct KMouseMovePlayerParam
{
	ANIMSTATUS keyMoveStatus;
	K3DPoint	pointPos;
};

struct KPlayerMachineStateParam
{
	CHARACTER_STATE	CharacterState;

};

struct KMouseRotPlayerParam
{
	BOOL			bAdded;		// 为真时，下面的值为增量，否则就是绝对值
	unsigned short nDirection; //角度
	unsigned short nDirectionZ; //角度
};

////////////////////////// 物品 //////////////////////////////////////////
struct KItemInfoAttachParm
{
public:
	void clear()
	{
		m_AttrType.clear();
		m_AttrValueP2.clear();
		m_AttrValueP3.clear();
		m_AttrValueP4.clear();
	}

	INT size() { return m_AttrType.size(); }

	void AddOneAttr( DWORD adwType, DWORD adwP2 = 0, DWORD adwP3 = 0, DWORD adwP4 = 0 )
	{
		m_AttrType[m_AttrType.size()] = adwType;
		m_AttrValueP2[m_AttrValueP2.size()] = adwP2;
		m_AttrValueP3[m_AttrValueP3.size()] = adwP3;
		m_AttrValueP4[m_AttrValueP4.size()] = adwP4;
		ASSERT_I(m_AttrType.size() == m_AttrValueP2.size());
		ASSERT_I(m_AttrType.size() == m_AttrValueP3.size());
		ASSERT_I(m_AttrType.size() == m_AttrValueP4.size());
	}

	::System::Collections::DynArray<DWORD, 1, 1> m_AttrType;
	::System::Collections::DynArray<DWORD, 1, 1> m_AttrValueP2;
	::System::Collections::DynArray<DWORD, 1, 1> m_AttrValueP3;
	::System::Collections::DynArray<DWORD, 1, 1> m_AttrValueP4;
};

struct KItemPack
{
	DWORD dwItemID;
	DWORD dwItemCount;
	BYTE byPackType;
};

struct KStoreItem
{
	WORD	wItemID;
	WORD	wInitCount;		// 限售商品的初试数量，、、、、、、、、、、如果为0，表示非限售商品
	WORD	wCount;			// 
	BYTE	byRefresh;
	BYTE	bySaleCount;
	union{
		DWORD	dwBuyPrice;
		DWORD	dwBuyNpc;
	};
	union{
		DWORD   dwSellPrice;
		DWORD   dwSellNpc;
	};
	BYTE	byIfQuest;
	BYTE	byIsOpen;
	BYTE	byResv[2];
	int		nExploit;		// 功勋值
	KItemPack ItemPack[MAX_ITEM_PACK];
};

struct KGearSet 
{
	KGearSet() {Init();}
	VOID Init()
	{
		nSetID = 0;
		nSetNameID = 0;
		nSetType = 0;
		mSetMembers.clear();
		mSetBuffs.clear();
	}
	INT	nSetID;
	INT nSetNameID;
	INT	nSetType;
	::System::Collections::DynArray<DWORD> mSetMembers;
	::System::Collections::DynArray<DWORD> mSetBuffs;
};

struct KCombineInfo
{
	DWORD LPnumberID;
	DWORD BGID;
	DWORD LPProductID;
	DWORD LPMaterials[4];
	int LPQuantities[4];
	DWORD LPIBItem[3];
	DWORD LPMessageID;
};

struct  KTradeItemOperatorParam
{
	KTradeItemOperatorParam()
	{
		Init();
	}
	VOID Init()
	{
		memset(this , 0 ,sizeof(KTradeItemOperatorParam));
	}
	
	DWORD			dwPlayerID;		// 玩家ID
	BYTE			bySubCmd;		// 子命令
	WORD			wPos;			// 物品位置
	BYTE			wSrcPos;		// 源位置
	BYTE			bySrcPosType;	// 源位置类型
	BYTE			wDestPos;		// 目标位置
	BYTE			byDestPosType;  // 目标位置类型
	BYTE			byResult;		// 返回结果
	BYTE			byState;		// 状态
	DWORD			dwMoney;
};

// 地面道具出现

struct KGroundItemParam
{
	DWORD	dwItemID;
	float	fX;
	float	fY;
	float	fZ;
	float	fDropX;
	float	fDropY;
	float	fDropZ;
	DWORD	dwOwner;
	DWORD	dwTeamOwner;
	WORD	wTypeID;
	WORD	wItemCount;
	BYTE	byQuality;
	BYTE	byIsDrop;
	BYTE	byOperator;
	BYTE	byBusy;
};

// 商店系统使用的结构
#pragma pack(push,1)
struct KChainStore
{
	// DWORD	m_dwID;		//npc id，基于BaseSet，只能叫m_dwID
	WORD	wFirstIndex;
	WORD	wLastIndex;
};


struct KStoreItemEx
{
	KStoreItem item;
	int nPos;
};
#pragma pack(pop)

//////////////////////////////////////////////////////////////////////////


struct KCharacterMoveToParam
{
	K3DPoint	pointStart;
	K3DPoint	pointEnd;
};


struct KMoveNpcParam
{
	K3DPoint	pointPos;
	int			nDirection;
};

#pragma pack(push,1)
struct KNpcInifInfomation_Cyc
{
	KNpcInifInfomation_Cyc() {memset(this, 0, sizeof(KNpcInifInfomation_Cyc));}
	float	m_fX;
	float	m_fY;
	float	m_fZ;
	WORD 	m_shAngle;
	DWORD	m_dwCategory;
	DWORD	m_dwOriginCategory;
	DWORD	m_dwNpcID;
	BYTE	m_nCurrentAIBaseState;
	BYTE	m_nCurrentAIState;
	BYTE	m_bIsReset;
};

struct KNpcInifInfomation_First
{
	KNpcInifInfomation_First() {memset(this, 0, sizeof(KNpcInifInfomation_First));}
	KNpcInifInfomation_Cyc m_CycData;
	DWORD	dwOwnerID;
	DWORD	dwMasterID;
	DWORD	m_dwGrouType;
	DWORD	m_dwGroupInstanceID;
	float	m_fRelativeHeight;
	DWORD	dwPrivateOwnerID;
	DWORD	dwPrivateLevel;
	BYTE	byReason;
	DWORD	dwCustomData;
	BOOL	bReliveDirectly;
	DWORD	dwLevel;
	DWORD	dwQuestOwnerID;
	DWORD	dwQuestID;
	DWORD	dwRelatedCharID;
};
#pragma pack(pop)

struct KNpcParam
{
	DWORD		dwCharID;
	K3DPoint	pointPos;
	K3DPoint	pointTargetPos;
	INT			nCruiseSpeed;
	INT			nChaseSpeed;
	unsigned short nDirection;
	unsigned short nDirectionZ;
	unsigned short nState;
	DWORD		dwCategory;
	DWORD		dwStateTime;
	ANIMSTATUS keyMoveStatus;
	BYTE		byMoveType;
	BYTE		byIsFly;
};

struct KStartMoveNpcParam
{
	K3DPoint	pointStart;
	K3DPoint	pointEnd;
};

struct KMouseMoveNpcParam
{
	ANIMSTATUS keyMoveStatus;
	K3DPoint	pointPos;
};

struct KMouseRotNpcParam
{
	BOOL			bAdded;		// 为真时，下面的值为增量，否则就是绝对值
	unsigned short nDirection;	//角度
	unsigned short nDirectionZ;	//角度
};

struct KNpcCommandParam
{
	NPC_COMMAND npcCommand;
	DWORD dwParam1;
	DWORD dwParam2;
	DWORD dwParam3;
	DWORD dwParam4;
	DWORD dwParam5;
};


// 玩家攻击怪物时，先记录每人攻击的数量，等怪物死亡后一起算经验
struct KAttackNpcParam
{
	DWORD		dwID;		// 攻击者ID，可能是组队ID
	int			nAttackHP;	// 攻击掉的生命值
	BYTE		byIsTeam;	// 是否是组队ID
	BYTE		byIsUsed;	// 是否被使用
	WORD		wResv;		// 其他影响经验分配的数据
};

#pragma pack(push,1)
struct KTarget
{
	TARGET_TYPE		m_TargetType;
	union
	{
		struct
		{
			FLOAT		m_fX;
			FLOAT		m_fY;
			FLOAT		m_fZ;
		}m_Coordination;
		//将VOID*改成KCharacter* by Allen 20081021
		KCharacter*		m_pCharacter;
		DWORD			m_dwCharacterID;
	};
	KTarget()
	{
		m_TargetType = ttInvalid;
	}
	inline BOOL operator !=(const KTarget& other) const
	{
		return !(*this == other);
	}
	inline BOOL operator ==(const KTarget& other) const
	{
		if (m_TargetType != other.m_TargetType) return FALSE;
		if (IsInvalid())
		{
			return TRUE;
		}
		else if (IsCharID())
		{
			return m_dwCharacterID == other.m_dwCharacterID;
		}
		else if (IsCharPointer())
		{
			return m_pCharacter == other.m_pCharacter;
		}
		else if (IsCoordination())
		{
			return (m_Coordination.m_fX == other.m_Coordination.m_fX
							&& m_Coordination.m_fY == other.m_Coordination.m_fY);
		}
		ASSERT_I(FALSE);//不支持的类型比较
		return FALSE;
	}
	inline BOOL IsCharID() const
	{
		return (m_TargetType == ttPlayerID || m_TargetType == ttNpcID || m_TargetType == ttMagicBabyID || m_TargetType == ttTreasureCaseID || m_TargetType == ttTransportID);
	}
	inline BOOL IsCharPointer() const
	{
		return (m_TargetType == ttPlayerPointer || m_TargetType == ttNpcPointer || m_TargetType == ttMagicBabyPointer || m_TargetType == ttTreasureCasePointer || m_TargetType == ttTransportPointer);
	}
	inline BOOL IsCoordination() const
	{
		return (m_TargetType == ttCoordination);
	}
	inline BOOL IsInvalid() const
	{
		return (m_TargetType == ttInvalid);
	}
};
#pragma pack(pop)

enum ENUM_CURSOR_TYPE
{
	CURSOR_WINBASE = 0,

	CURSOR_NORMAL,			//普通
	CURSOR_NORMAL_DISABLE,	//普通灰
	CURSOR_ATTACK,			//攻击
	CURSOR_ATTACK_DISABLE,	//攻击灰
	CURSOR_ATTACK_SKILL,			//带技能攻击
	CURSOR_ATTACK_SKILL_ENABLE,		//带技能可攻击
	CURSOR_ATTACK_SKILL_DISABLE,	//带技能攻击灰
	CURSOR_ATTACK_SKILL_RANGE,		//带技能范围攻击
	CURSOR_QUESTFINISH,		//可交任务
	CURSOR_QUESTUNACCEPT,	//未接任务，满足条件
	CURSOR_QUESTACCEPT,		//已接未完成
	CURSOR_QUESTCONDITION,	//未接任务，不满足条件
	CURSOR_STUDYSKILL,		//学习技能 
	CURSOR_TRANS,			//传送
	CURSOR_STORE,			//商店
	CURSOR_MAIL,			//邮件
	CURSOR_STORAGE,			//仓库
	CURSOR_TALK,			//对话
	CURSOR_PICKUP,			//拾取物品
	CURSOR_PICKUP_DISABLE,	//拾取物品灰
	CURSOR_MINE,			//采矿
	CURSOR_MINE_DISABLE,	//采矿灰
	CURSOR_HERBS,			//采药
	CURSOR_HERBS_DISABLE,	//采药灰
	CURSOR_INTERACT,		//齿轮
	CURSOR_INTERACT_DISABLE,//齿轮灰
	CURSOR_REPAIR,			//修理
	CURSOR_REPAIR_DISABLE,	//修理灰
	CURSOR_HOVER,			//鼠标激活(挂接物品...)
    CURSOR_SELL,
	CURSOR_NUMBER,

};

enum MouseState_Type
{
	MouseState_NULL = 0,		//空状态
	MouseState_Default = 1,		//默认状态
	MouseState_System = 2,		//系统状态，鼠标上挂接有系统提供的技能
	MouseState_Skill = 3,		//技能状态，鼠标上挂接有玩家技能或者物品技能
	MouseState_SlotMove = 4,	//移动道具或技能图标
};

enum MouseCommand_Type
{
	MouseCommand_NULL ,		// 空命令		
	MouseCommand_Cancel,	// 退出当前鼠标命令
	MouseCommand_Select , // 选择对象
	MouseCommand_LeftSkill , // 左键技能
	MouseCommand_RightSkill, // 右键技能
	MouseCommand_TalkToNpc,	// 跟NPC对话 
	MouseCommand_TCSkill, //操作宝箱
	MouseCommand_PickUpGroundItem, //拾取掉落物
	MouseCommand_Skill,			//鼠标附带技能
	MouseCommand_ItemSkill,		//鼠标附带道具技能
	MouseCommand_AutoPath,		//自动寻路
};


enum KMouseTarget_Type
{
	KMouseTarget_NULL,
	KMouseTarget_Ground,	//地面
	KMouseTarget_Obj,		//场景物体对象，NPC、宝箱、玩家等
	KMouseTarget_Icon,		//UI中的图标格子
	KMouseTarget_NormalUI,  //普通的UI面板
};

#define MAX_OBJ_CMD_PARAM_NUM	(5)

struct KMouse_Command
{
	MouseCommand_Type m_eMouseCommandType;
	KMouseTarget_Type m_eMouseTargetType;	//希望的鼠标对象类型
	DWORD		m_pMouseDataParam[MAX_OBJ_CMD_PARAM_NUM];	//鼠标挂接数据
	DWORD		m_pTargetParam[MAX_OBJ_CMD_PARAM_NUM];		//鼠标目标数据
	BOOL		m_bHasTarget;								//是否有目标

	typedef BOOL (*Mouse_CommandCallback)(DWORD* pTargetParam,DWORD* MouseParam2);		
	Mouse_CommandCallback	m_MouseCommandFunc;

	KMouse_Command()
	{
		memset(m_pMouseDataParam, 0, sizeof(m_pMouseDataParam));
		memset(m_pTargetParam, 0, sizeof(m_pTargetParam));
		Clear();
	}
	//填充参数时一定要确认目标的有效性，尽量不要直接传对象指针
	BOOL		Excute() {if(m_MouseCommandFunc) return m_MouseCommandFunc(m_pTargetParam, m_pMouseDataParam); return FALSE;}
	void		Clear() {m_eMouseCommandType = MouseCommand_NULL; m_MouseCommandFunc = NULL; m_bHasTarget = FALSE; m_eMouseTargetType=KMouseTarget_NULL;}
	void		ClearTarget() {m_bHasTarget = FALSE;memset(m_pTargetParam, 0, sizeof(m_pTargetParam));}
	BOOL		HasTarget() {return m_bHasTarget;}

};


class KMouse_Target
{
public:
	KMouse_Target(void):m_eType(KMouseTarget_NULL) {;}
	virtual ~KMouse_Target(void) {;}

	virtual BOOL FillMouseCommand(MouseState_Type eState, KMouse_Command& leftMouseCommand, KMouse_Command& rightMouseCommand, ENUM_CURSOR_TYPE& eMouseCursor) {return FALSE;}
	KMouseTarget_Type m_eType;
	KMouseTarget_Type GetType() {return m_eType;}

};

enum KGameOperater_Type
{
	KGameOperater_NULL,
	KGameOperater_Cancel,
	KGameOperater_System,
	KGameOperater_Skill,
	KGameOperater_ItemSkill,
	KGameOperater_PickIcon,
};

struct KGame_Operater
{
	KGameOperater_Type m_eGameOperaterType;		//操作类型
	KMouseTarget_Type m_eMouseTargetType;		//需要填期望操作的目标类型，非此类型不会被选中
	DWORD		m_OperaterDataParam[MAX_OBJ_CMD_PARAM_NUM];	//游戏操作附带数据
};

// 怪物点
struct wPoint
{
	wPoint() {Init();}
	VOID Init() {memset(this, 0, sizeof(wPoint));}
	FLOAT x,y,z;
};

struct wPointWithMapID
{
	wPointWithMapID() {Init();}
	VOID Init() {mPoint.Init();mdwMapID = 0;}
	wPoint mPoint;
	DWORD mdwMapID;
};

struct siPoint : wPoint
{
	BYTE  btActionMode;		// 行为方式
	BYTE  btActionTarget;	// 行为对象
};
struct STMonster
{
	INT		iMapId;				// 地图ID
	wPoint ptLocation;			// X，Y坐标
	SHORT	siRadius;			// 刷怪半径
	INT		iCategory;			// 怪物种类
	SHORT	siNumber;			// 怪物数量
	SHORT	siBornAdjustCond;	// 刷新调整条件
	BOOL	bInOrder;			// 随机/顺序
	siPoint ptPatrolPoint[8];	// 巡逻路线1-8
	INT		iScript;			// 脚本名
};

struct PatrolPointDetail
{
	PatrolPointDetail() {Init();}
	VOID Init() {m_dwActionType = 0;m_PatrolPointParmList.clear();}
	DWORD m_dwActionType;
	typedef ::System::Collections::DynArray<DWORD, 4, 1> KPatrolPointParmArray;
	KPatrolPointParmArray m_PatrolPointParmList;
};

// 刷怪点
typedef ::System::Collections::KVector<PatrolPointDetail> PatrolPointDetailArray;
struct PatrolPoint
{
	PatrolPoint()
	{
		this->Init();
	}
	VOID Init()
	{
		m_dwID = 0;
		m_fPatrolPosX = 0.0f;
		m_fPatrolPosY = 0.0f;
		m_dwTime = 0;
		m_PatrolPointDetailArray.clear();
	}
	DWORD m_dwID;
	FLOAT m_fPatrolPosX;
	FLOAT m_fPatrolPosY;
	DWORD m_dwTime;			// 巡逻点停留时间
	PatrolPointDetailArray m_PatrolPointDetailArray;	// 巡逻点行为集
};

struct PatrolMode
{
	PatrolMode(): byTotalPoints(0), byTeam(0), byRouteOrder(0), patrolpoint(NULL)
	{
	}
	
	~PatrolMode()
	{
		if(patrolpoint)
		{
			delete [] patrolpoint;
			patrolpoint = NULL;
		}
	}

	BYTE	byTotalPoints;		// 巡逻点数量
	BYTE	byTeam;				// 巡逻模式适用的组,(缺省为0,指所有组)
	BYTE	byRouteOrder;		// 巡逻顺序 (0=不动;1=随机.....)
	PatrolPoint* patrolpoint;	// 巡逻点
};

struct KNpcBornPoint
{
	KNpcBornPoint() {}
	VOID Init() {shAngleValue = 0;shAngleValueZ = 0;}

	wPoint	wBornPoint;
	SHORT   shAngleValue;
	SHORT   shAngleValueZ;
};

struct NPCPoint
{
	NPCPoint() {Init();}
	VOID Init()
	{
		iMapId = 0;
		siRadius = 0;
		siNumber = 0;
		siBornAdjustCond = 0;
		byDefaultState = 0;
		iScript = 0;
		GroupTypeID = 0;
		GroupInstanceID = 0;
		TeamID = 0;
		PatrolPattens = 0;
		patrolmode = NULL;
		iNpcPointId = 0;
		iCategory = 0;
		ptLocations.clear();
	}

	~NPCPoint()
	{
		SAFE_DELETE_ARRAY(patrolmode);
	}

	INT		iMapId;				// 地图ID
	float	siRadius;			// 刷怪半径
	SHORT	siNumber;			// 怪物数量
	SHORT	siBornAdjustCond;	// 刷新调整条件
	BYTE	byDefaultState;		// 缺省状态值,指向休闲动作列表,0=基本站立休闲
	INT		iScript;			// 脚本名
	DWORD   GroupTypeID;		// 队类型ID(40000+)
	DWORD   GroupInstanceID;	// 队索引(1-n)
	BYTE	TeamID;				// 队内组ID(1-8)
	BYTE	PatrolPattens;		// 巡逻模式数,共有多少巡逻方式(刷一群怪时才会大于1)
	PatrolMode* patrolmode;		// 巡逻模式
	INT		iNpcPointId;		// 刷怪点ID
	INT		iCategory;			// 怪物种类
	::System::Collections::KVector<KNpcBornPoint>	ptLocations;// X，Y坐标
};

// 巡逻点
struct PatrolPoint1
{
	wPoint	ptPrtrolPoint;		// 巡逻点坐标
	SHORT	siPrtrolState;		// 巡逻点状态
	SHORT	siPatrolAction;		// 巡逻点动作
	INT		nPointTime;			// 巡逻停留时间
	BYTE	byActionPartner;	// 巡逻点行为对象			//?
};

// 巡逻模式
struct PatrolMode1
{
	PatrolMode1(){patrolpoint = NULL;}
	~PatrolMode1(){ SAFE_DELETE_ARRAY(patrolpoint) }
	BYTE	byTotalPoints;		// 巡逻点数量
	BYTE	byTeam;				// 巡逻模式适用的组,(缺省为0,指所有组)
	BYTE	byRouteOrder;		// 巡逻顺序 (0=顺序;1=随机)
	PatrolPoint* patrolpoint;	// 巡逻点
};

// 刷怪点
struct NPCPoint1
{
	NPCPoint1(){patrolmode = NULL;}
	~NPCPoint1(){SAFE_DELETE_ARRAY(patrolmode)}
	INT		iNpcPointId;		// 刷怪点ID
	INT		iMapId;				// 地图ID
	wPoint	ptLocation;			// X，Y坐标
	SHORT	siRadius;			// 刷怪半径
	SHORT	siCategory;			// 怪物种类
	SHORT	siNumber;			// 怪物数量
	SHORT	siBornAdjustCond;	// 刷新调整条件 (default = 50% * siNumber)
	INT		iScript;			// 脚本名
	BYTE	byDefaultState;		// 缺省状态值,指向休闲动作列表,0=基本站立休闲
	BYTE	PatrolPattens;		// 巡逻模式数,共有多少巡逻方式(刷一群怪时才会大于1)
	PatrolMode* patrolmode;		// 巡逻模式
};


// 聊天信息
//////////////////////// 聊天所需模块间传递信息///////////////////////////////////////////
struct KChatOperatorParam
{
	BYTE							byChannel;								//	聊天频道类型
	BYTE							bySrcNation;							//  喊话人国籍
	BYTE							bySrcMasterLevel;						//	玩家权限
	BYTE							byDestMasterLevel;						//	玩家权限
	WORD							wMessageLen;							//	聊天消息字符长度
	DWORD							dwPlayerID;								//	玩家ID编号，可能是网络连接或什么
	DWORD							dwChannelID;							//	聊天频道ID
	DWORD							dwTargetPlayerID;						//	目标玩家ID
	::System::Collections::KDString<MAX_NAME_DEF>			m_SrcNick;								//	玩家名字
	::System::Collections::KDString<MAX_NAME_DEF>			m_DestNick;								//	聊天对象名字
	::System::Collections::KDString<MAX_CHATMESSAGE_LEN>	m_Message;								//	聊天对话信息
	DWORD							dwAliveTime;							//	聊天信息消亡时间（毫秒）
	BYTE							byPostID;								//  喊话人官职(0-7)
};

DECLARE_SIMPLE_TYPE(KChatOperatorParam);


struct KFightLogOperatorParam
{
	BYTE							byCastPlayer;							// 施法者
	BYTE							byTargetPlayer;							// 被施法目标
	BYTE							byOperationType;						// 操作类型
	WORD							wMessageLen;							// 聊天消息字符长度
	::System::Collections::KDString<MAX_CHATMESSAGE_LEN>	m_Message;		// 聊天对话信息
};

struct KPlayerChatInfo
{
public:
	KPlayerChatInfo()
	{		
		Init();
	}
	void Init()
	{
		memset(this , 0 , sizeof(KPlayerChatInfo));
	}
	BOOL		CheckCanSendMessage()
	{
		if (m_LockTime == 0)
			return TRUE;
		if(time(NULL) > m_LockTime)
		{
			m_LockTime = 0;
			return TRUE;
		}
		return FALSE;
	}
	VOID	LockTime(INT nSeconds)
	{
		m_LockTime	= time(NULL) + nSeconds;
	}
	DWORD	GetLeftLockTime()
	{
		//AHEAD: INT64和DWORD相减需要处理。如果只是time(NULL)相关可以考虑使用TIME_T
		return (DWORD)(m_LockTime - time(NULL));
	}
	INT64		GetLastSendTime(INT nChannelID)
	{
		ASSERT_I(nChannelID >= enumECG_Invalid && nChannelID < enumECG_Count);
		return m_LastSendTime[nChannelID];
	}
	VOID		SetLastSendTime(INT nChannelID , INT64 msTime)
	{
		ASSERT_I(nChannelID >= enumECG_Invalid && nChannelID < enumECG_Count);
		m_LastSendTime[nChannelID]	= msTime;
	}
private:
	INT64		m_LockTime;
	INT64		m_LastSendTime[enumECG_Count];
};


/////////////////////////////////////////////////////////////////////////

struct KNpcSF
{
	struct  Value
	{
		INT					m_nValue;
		BYTE				m_byType;
	};
	SHORT	m_Type;
	DWORD	m_dwPlayerID;
	DWORD	m_dwNpcID;
	INT		m_nValueCount;
	Value	m_Value[MAX_CHAR_SKILL_COUNT];
};

struct KLearnSkillInfo
{
	INT						m_nSkillGroupID;		//技能组ID
	INT						m_nCurrSkillCount;		//当前技能等级
	INT						m_nMaxSkillCount;		//最大技能等级
	INT						m_nCurrSkillID;			//当前技能ID
	INT						m_nCurrSkillCost;		//当前技能消耗
	INT						m_nNextSkillID;			//下一等级技能等级
	INT						m_nNextSkillCost;		//下一等级技能消耗
	INT						m_nRawX;				//显示位置
	INT						m_nRawY;				//显示位置
	BYTE					m_bySkillGroupState;	//技能学习状态
};
DECLARE_SIMPLE_TYPE(KLearnSkillInfo)

struct KLearnSkillInfos
{
	KLearnSkillInfos()
	{
		Init();
	}

	VOID Init()
	{
		m_nLearnSkillInfoCount = 0;
		m_nLearnSkillInfos.clear();
	}

	INT							m_nLearnSkillInfoCount;
	::System::Collections::DynArray<KLearnSkillInfo>	m_nLearnSkillInfos;
};

struct KLearnSkillResult
{
	INT							m_nSkillID;
	BYTE						m_byResult;
	INT							m_nParam;
};

struct KUpdateStateParam
{
	UPDATESTATEPARAM_TYPE	m_TargetType;
	union
	{
		BYTE m_byMoveType;
		BYTE m_byLeisureID;
		BYTE m_byDeathType;
		BYTE m_byStandType;
	};

	float m_fMoveSpeed;
};

struct KUpdateState
{
	UINT					m_CharacterState;
	KUpdateStateParam *		m_pStateParam;
};

struct KQuestFunction
{
	struct  Quest
	{
		INT				m_nQuestID;
		CHAR			m_szQuestName[MAX_SKILLNAME_SIZE+1];
	};
	DWORD	m_dwPlayerID;
	DWORD	m_dwNpcID;
	INT		m_nQuestCount;
	Quest	m_Quest[128];
};

//////////////////////////////////////////////////////////////////////////
// 玩家互动信息数据传递的结构体
struct KPlayerInteractiveParam
{
	KPlayerInteractiveParam()
	{
		Init();
	}
	VOID Init()
	{
		memset(this , 0 , sizeof(KPlayerInteractiveParam));
	}
	BYTE		bySubCmd;			// 命令
	DWORD		dwPlayerID;			// 自己ID
	BYTE		byState;			// 当前状态
	DWORD		dwTargetID;			// 对象ID
};

struct KCharacterAttrUpdateParam
{
	DWORD dwID;
	BYTE byType;
	BYTE byResv[3];
	INT nAttrID;
	INT nAttrValue;
};

////////////////////////////////////////////////////////////////////////////
// 登陆和角色逻辑的结构体
#pragma pack(push,1)
struct STRU_KCharacterDisplayBaseInfo
{
	BYTE	CT_SEX;					// 性别
	BYTE	CT_PRINK;				// 化妆
	BYTE	CT_NATIONALITY;			// 国籍
	BYTE	CT_FACESTYLE;			// 脸型

	BYTE	CT_HAIRSTYLE;			// 发型
	BYTE	CT_HAIRCOLOR;			// 发色
	BYTE	CT_COMPLEXION;			// 肤色
	BYTE	CT_MODEL;				// 模型(角色模型类型编号)

	BYTE	CT_Class;				// 职业
	BYTE	CT_SubClass;			// 分支
	BYTE	CT_IsShowHelm;			// 是否显示帽子
	BYTE	CT_IsShowFanshion;		// 显示时装

	TIME_T	CT_ProtectTime;			// 保护时间
	TIME_T	CT_ProtectTime2;		// 保护时间

	BYTE	CT_NeedChangeName;		// 进入游戏是否需要改名
};

struct STRU_KNpcDisplayBaseInfo
{
	BYTE mbyFightType;
	BYTE mbyAttackable;
	BYTE mbyFleeType;
	BYTE mbyMNEliteLevel;

	BYTE mbyNationality;
	BYTE mbySex;
	BYTE mbyClass;
	BYTE mbySubClass;

	int nWeaponID;

	BYTE mbyRace;
	BYTE mbyElement;
};

struct KCharacterBaseInfo
{
	DWORD	CT_ID;					// 角色ID
	DWORD	CT_LAST_LOGIN_TIME;		// 上次登录时间
	DWORD	CT_LAST_LOGIN_IP;		// 上次登录地点
	BYTE	CT_LEVEL;				// 等级
	BYTE	CT_IS_ALREADY_LOGIN;	// 初次登陆
	char	CT_NAME[MAX_NAME_DEF];	// 名称
	STRU_KCharacterDisplayBaseInfo m_DisplayInfo;	// 角色显示基本信息
};

struct STRU_EQUIP_INFO
{
	WORD wItemID;
	WORD wPos;
	BYTE m_byStrengthLv;
	BYTE m_byItemQuality;
};

struct STRU_CHARACTER_INFO
{
	void Clear() 
	{
		m_nItemCount = 0;
		memset(&m_CharacterBaseInfo, 0, sizeof(m_CharacterBaseInfo));
		memset(mItem, 0, sizeof(mItem));
	}
	KCharacterBaseInfo m_CharacterBaseInfo;
	STRU_EQUIP_INFO	mItem[CFG_MAX_BAG_EQUIP_ITEM_COUNT];
	USHORT m_nItemCount;
};

struct STRU_CHARACTER_INFOS
{
	BYTE m_Count;
	BOOL m_bCharacterInfoValid[DEF_MAX_SEND_CHAR_COUNT];
	STRU_CHARACTER_INFO m_CharacterInfo[DEF_MAX_SEND_CHAR_COUNT];
};

struct STRU_SERVER_INFO
{
	STRU_SERVER_INFO()
	{
		memset(this, 0 ,sizeof(STRU_SERVER_INFO));
	}
	char m_szServerName[MAX_NAME_DEF];
	DWORD	m_ServerIP;			// 服务器IP
	WORD	m_ServerPort;		// 服务器端口
	BYTE	m_AreaID;			// 大区ID
	BYTE	m_ID;				// 服务器ID
	BYTE	m_ServerState;
	BYTE	m_byRecommendationState;	// 0 不推荐	>0 推荐 
};
DECLARE_SIMPLE_TYPE(STRU_SERVER_INFO)

struct STRU_LOGIN_INFO
{
	STRU_LOGIN_INFO()
	{
		memset(account, 0, sizeof(account)); 
		memset(password, 0, sizeof(password));
	}
	char account[MAX_MD5_STRING];
	char password[MAX_MD5_STRING];
};

// 通知逻辑ChatServer准备好，给客户端发送的消息
struct KChatServerInfo
{
	KChatServerInfo()
	{
		memset(this , 0 ,sizeof(KChatServerInfo));
	}
	VOID SetIPAddr(CHAR* IP)
	{
		strncpy(strChatServerAddr , IP , 16);
	}

	DWORD		dwPlayerID;
	CHAR		strChatServerAddr[16];
	DWORD		dwChatServerPort;
	DWORD		m_OTP;
};
#pragma pack(pop)

// 技能组相关，zhu xiaokun 2008.9.10

enum eSkillGroupOperation
{
	enumSGO_Initialize = 0,
	enumSGO_GetManager,     // 客户端专用

	enumSGO_SetGroupSkill,
	enumSGO_SetGroupSkills,
	enumSGO_SetCurrentGroup,

	enumSGO_GetOccupationSkillClass,
	enumSGO_GetClassSkills,
	enumSGO_GetSkillProperty,
	enumSGO_GetGroupSkill,
	enumSGO_IsSkillGroupEditable,
	enumSGO_IsNewSkill,
	enumSGO_ClearNewFlag,
	enumSGO_IsPlayerHasSkill,
	enumSGO_Reset,
	enumSGO_CurrentGroupUp,
	enumSGO_CurrentGroupDown,
	enumSGO_GetIdentitySkillProperty,
	enumSGO_GetCurrentGroupIndex,
	enumSGO_GetSkillGroupData,
	enumSGO_GetIdentityActionProperty,
	enumSGO_ActiveGroup,
	enumSGO_GetGroupActiveState,

	enumSGO_SwapGroupSkill,
    //add by yangbin for skillsystem
    enumSGO_GetSkillTotalTerm,
    enumSGO_GetTermSkills,
	//
	enumSGO_Count
};

struct KSkillClass
{
	enum
	{
		skill_class_normal  = 0,
		skill_class_special = 14,
		skill_class_count   = 15,
	};
	enum
	{
		ui_skill_class_profession_count = 3,
		ui_skill_class_count = 5,
	};

	int  skillClass;
	char skillClassName[32];
};

struct KSkillProperty
{
	INT  m_nSkillID;
	CHAR m_szSkillName[MAX_SKILLNAME_SIZE+1];
	char m_iconFile[MAX_SKILLICON_SIZE+1];
	INT m_SkillContent[80];
// 	INT	 m_SBI[64];								// 等级无关的基本属性
// 	INT	 m_SLI[64];								// 等级相关的基本属性
//	BOOL m_bTargetOnly;
};

#if defined(_CLIENT) || defined(_ROBOT)
struct KIdentitySkillProperty
{
	INT		m_nSkillID;			//技能id，需要UI给出
	BOOL	m_bMainPlayer;		//是否主玩家的技能
	INT		m_nHPCost;			//技能的HP消耗
	INT		m_nMPCost;			//技能的MP消耗
	INT		m_nRPCost;			//技能的RP消耗
	CHAR	m_itemName[4][256];	//技能物品消耗的物品名字
	DWORD	m_itemCount[4];		//技能物品消耗的物品数量
	DWORD	m_itemKindCount;	//消耗多少种物品
	INT		m_nMinRange;		//最小范围
	INT		m_nMaxRange;		//最大范围
	INT		m_nR;				//半径。方形区域返回0
	DWORD	m_dwPrepareTime;	//PrepareTime
	INT		m_nCDTime;			//CD时间
	BOOL	m_bHPEnough;		//HP是否足够
	BOOL	m_bMPEnough;		//MP是否足够
	BOOL	m_bRPEnough;		//RP是否足够
	BOOL	m_bItemEnough[4];	//物品是否足够
	BOOL	m_bIsEquiped;		//技能是否装备
	DWORD	m_dwCastTime;		//Shoot时间
	DWORD	m_dwChannelTime;	//Channel时间
	INT		m_nLastCDTime;		//剩余cd时间
// 	INT		m_nMissRate;		//失误率
};

struct KIdentityActionProperty
{
	DWORD actionID;
	INT prob;
	INT nBaseThreat;
	INT param[12];
};
#endif // _CLIENT

struct KNewSkillNotify
{
	int skillID;
	int classPos;             // 新增技能组所在的位置，只有当pSkillClass不为空是有效
	KSkillClass* pSkillClass; // 如果新学会的技能导致该玩家新增一个技能类，才会有不为空的值
};


// 结构指针作为uParam
struct KSkillGroupOperation
{
	eSkillGroupOperation enumOperation;
	DWORD dwPlayerID;
	union
	{
		struct
		{
			int m_currentSkillGroup;
		} initialize;
		struct
		{
			int groupIndex;
			int pos;
			int skillID;
		} setGroupSkill;
		struct
		{
			int groupIndex;
			int skillIDs[MAX_SKILL_GROUP_CAPACITY];
		} setGroupSkills;
		struct
		{
			int groupIndex;
		} setCurrentGroup;
		struct
		{
			BYTE m_bActive;
			BYTE m_id;
			BYTE m_templateID;
		} setActiveGroup;
		struct
		{
			int skillClass;
			int pageNo;
		} getClassSkills;
		struct
		{
			int skillTerm;
			int pageNo;            
		} getTermSkills;
		struct
		{
			int skillID;
		} getSkillProperty;
#if defined(_CLIENT) || defined(_ROBOT)
		struct
		{
			INT skillID;
			BOOL bMainPlayer;
		} getIdentitySkillProperty;
		struct
		{
			DWORD actionID;
			BOOL bMainPlayer;
		} getIdentityActionProperty;
#endif // _CLIENT
		struct
		{
			int groupIndex;
			int pos;
		} getGroupSkill;
		struct
		{
			int groupIndex;
		} isSkillGroupEditable;
		struct
		{
			int skillID;
		} isNewSkill;
		struct
		{
			int skillID;
		} clearNewFlag;
		struct
		{
			int skillID;
		} isPlayerHasSkill;
		struct
		{
			int groupIndex;
		} reset;
		struct
		{
			
		} getCurrentGroupIndex;
		struct
		{
			int skillGroupIndex;
		} getSkillGroupData;
		struct
		{
			int groupIndex;
			int srcPos;
			int destPos;
		} swapGroupSkill;
	} data;

	int size() const
	{
		int baseSize = sizeof(eSkillGroupOperation) + sizeof(DWORD);
		switch(enumOperation)
		{
		case enumSGO_Initialize:              return baseSize + sizeof(data.initialize);
		case enumSGO_GetManager:              return baseSize;
		case enumSGO_SetGroupSkill:           return baseSize + sizeof(data.setGroupSkill);
		case enumSGO_SetGroupSkills:          return baseSize + sizeof(data.setGroupSkills);
		case enumSGO_SetCurrentGroup:         return baseSize + sizeof(data.setCurrentGroup);
		case enumSGO_GetOccupationSkillClass: return baseSize;
		case enumSGO_GetClassSkills:          return baseSize + sizeof(data.getClassSkills);
		case enumSGO_GetSkillProperty:        return baseSize + sizeof(data.getSkillProperty);
		case enumSGO_GetGroupSkill:           return baseSize + sizeof(data.getGroupSkill);
		case enumSGO_IsSkillGroupEditable:    return baseSize + sizeof(data.isSkillGroupEditable);
		case enumSGO_IsNewSkill:              return baseSize + sizeof(data.isNewSkill);
		case enumSGO_ClearNewFlag:            return baseSize + sizeof(data.clearNewFlag);
		case enumSGO_IsPlayerHasSkill:        return baseSize + sizeof(data.isPlayerHasSkill);
		case enumSGO_Reset:                   return baseSize + sizeof(data.reset);
		case enumSGO_CurrentGroupUp:          return baseSize;
		case enumSGO_CurrentGroupDown:        return baseSize;
		case enumSGO_GetCurrentGroupIndex:    return baseSize + sizeof(data.getCurrentGroupIndex);
		case enumSGO_GetSkillGroupData:       return baseSize + sizeof(data.getSkillGroupData);
		case enumSGO_ActiveGroup:             return baseSize + sizeof(data.setActiveGroup);
		case enumSGO_GetGroupActiveState:     return baseSize;
#ifdef _CLIENT
		case enumSGO_GetIdentitySkillProperty:	return baseSize + sizeof(data.getIdentitySkillProperty);
		case enumSGO_GetIdentityActionProperty:	return baseSize + sizeof(data.getIdentityActionProperty);
#endif // _CLIENT
		case enumSGO_SwapGroupSkill:          return baseSize + sizeof(data.swapGroupSkill);
		}
		return 0;
	}
};
// 结构指针作为lParam
struct KSkillGroupOperationResult
{
	eSkillGroupOperation enumOperation;
	union
	{
		struct
		{
			void* pManager;
		} getManager;
		struct
		{
			BOOL  result;
		} setGroupSkill;
		struct
		{
			BOOL  result;
		} setGroupSkills;
		struct
		{
			BOOL  result;
		} setCurrentGroup;
		struct
		{ 
			int result;
			KSkillClass cls[MAX_OCCUPATION_SKILL_CLASS];
		} getOccupationSkillClass;
		struct
		{
			BOOL bTermFlag[MAX_SKILL_TERM];
		} getSkillTotalTerm;
		struct
		{
			BOOL result;
			int num;
			int total;
			int skillIDs[MAX_SKILL_GROUP_CAPACITY];
		} getClassSkills;
		struct
		{
			BOOL result;
			int num;
			int total;
			int skillIDs[UI_PAGE_SKILL_CAPACITY];
			int skillRelation[UI_PAGE_SKILL_CAPACITY];//用于区别（1进阶技能；2已经学习的技能；3可学习的技能）
		} getTermSkills;
		struct
		{
			BOOL result;
			KSkillProperty property;
		} getSkillProperty;
#if defined(_CLIENT) || defined(_ROBOT)
		struct
		{
			BOOL result;
			KIdentitySkillProperty property;
		} getIdentitySkillProperty;
		struct
		{
			BOOL result;
			KIdentityActionProperty property;
		} getIdentityActionProperty;
#endif // _CLIENT
		struct
		{
			int skillID;
		} getGroupSkill;
		struct
		{
			BOOL result;
		} isSkillGroupEditable;
		struct
		{
			BOOL result;
		} isNewSkill;
		struct
		{
			BOOL result;
		} isPlayerHasSkill;
		struct
		{
			BOOL result;
		} reset;
		struct
		{
			BOOL result;
			int currentSkillGroupIndex;
		} currentGroupUp;
		struct
		{
			BOOL result;
			int currentSkillGroupIndex;
		} currentGroupDown;
		struct
		{
			BOOL result;
			int currentSkillGroupIOndex;
		} getCurrentSkillGroupIndex;
		struct 
		{
			BOOL result;
		} getSkillGroupData;
		struct
		{
			BOOL nothing;
		} getSkillGroupActiveState;
		struct
		{
			BOOL result;
		} swapGroupSkill;
	} data;

	int size() const
	{
		int baseSize = sizeof(eSkillGroupOperation);
		switch(enumOperation)
		{
		case enumSGO_Initialize:              return baseSize;
		case enumSGO_GetManager:              return baseSize + sizeof(data.getManager);
		case enumSGO_SetGroupSkill:           return baseSize + sizeof(data.setGroupSkill);
		case enumSGO_SetGroupSkills:          return baseSize + sizeof(data.setGroupSkills);
		case enumSGO_SetCurrentGroup:         return baseSize + sizeof(data.setCurrentGroup);
		case enumSGO_GetOccupationSkillClass: return baseSize + sizeof(data.getOccupationSkillClass);
		case enumSGO_GetClassSkills:          return baseSize + sizeof(data.getClassSkills);
		case enumSGO_GetSkillProperty:        return baseSize + sizeof(data.getSkillProperty);
		case enumSGO_GetGroupSkill:           return baseSize + sizeof(data.getGroupSkill);
		case enumSGO_IsSkillGroupEditable:    return baseSize + sizeof(data.isSkillGroupEditable);
		case enumSGO_IsNewSkill:              return baseSize + sizeof(data.isNewSkill);
		case enumSGO_ClearNewFlag:            return baseSize;
		case enumSGO_IsPlayerHasSkill:        return baseSize + sizeof(data.isPlayerHasSkill);
		case enumSGO_Reset:                   return baseSize + sizeof(data.reset);
		case enumSGO_CurrentGroupUp:          return baseSize + sizeof(data.currentGroupUp);
		case enumSGO_CurrentGroupDown:        return baseSize + sizeof(data.currentGroupDown);
		case enumSGO_GetCurrentGroupIndex:    return baseSize + sizeof(data.getCurrentSkillGroupIndex);
		case enumSGO_GetSkillGroupData:       return baseSize + sizeof(data.getSkillGroupData);
		case enumSGO_GetGroupActiveState:     return baseSize + sizeof(data.getSkillGroupActiveState);
		case enumSGO_SwapGroupSkill:          return baseSize + sizeof(data.swapGroupSkill);
		}
		return 0;
	}
};



// GM命令模块传递所需要的数据
struct GmSystem_Command_Data 
{
	GmSystem_Command_Data()
	{
		Init();
	}
	VOID Init()
	{
		memset(this , 0 ,sizeof(GmSystem_Command_Data));
	}
	VOID SetTargetNick(const char* Nick)
	{
		strncpy(strTargetNick , Nick , sizeof(strTargetNick) / sizeof(CHAR) - 2);
	}
	BYTE	byResult;
	INT		nCommandDefine;
	DWORD	dwPlayerID;
	INT		nValue1;
	INT		nValue2;
	INT		nValue3;
	INT		nValue4;
	INT		nValue5;
	INT		nValue6;
	CHAR	strTargetNick[MAX_NAME_DEF + 200];
};


// 结构指针作为COI_MAIN_PLAYER_USE_SKILL消息的uParam
struct KSkillMainPlayerUseSkill
{
	DWORD	m_dwSrcCharID;
	DWORD	m_dwSkillID;
	KTarget m_MouseTarget;
// 	KTarget m_MouseOver;
};

// 技能CoolDown的查询结果
struct KSkillCDGroupResult
{
	//CoolDown总时间(ms)
	DWORD dwTotalTime;
	//CoolDown剩余时间(ms)
	DWORD dwLastTime;
};

// Tab文件的头和默认值
struct KTabFileTitle
{
	const char* lpszTitle;
	int			nDefaultValue;
};

#pragma pack(push,1)
struct KCombatSyncAttr
{
	INT	nAttrID;
	INT	nAttrValue;
};
#pragma pack(pop)

struct KUseSkillByItemData 
{
	KUseSkillByItemData()
	{
		Init();
	}
	VOID	Init()
	{
		memset(this , 0 , sizeof(KUseSkillByItemData));
	}
	VOID	SetData(WORD wPos , BYTE byPosType , DWORD dwItemID , DWORD dwItemNum , DWORD dwSkillID)
	{
		m_wPos		= wPos;
		m_byPosType	= byPosType;
		m_dwItemID	= dwItemID;
		m_dwItemNum	= dwItemNum;
		m_dwSkillID	= dwSkillID;
	}
	WORD	m_wPos;
	BYTE	m_byPosType;
	DWORD	m_dwItemID;
	DWORD	m_dwItemNum;
	DWORD	m_dwSkillID;
};

// 对3D来说Character的类型
enum enumCharacterTypeTo3D
{
	enumCTT3D_MainPlayer,		//主玩家
	enumCTT3D_OtherPlayer,		//其他玩家
	enumCTT3D_MainMagicBaby,	//主法宝
	enumCTT3D_OtherMagicBaby,	//其他法宝
	enumCTT3D_Monster,			//Monster
	enumCTT3D_TreasureCase,		//宝箱
	enumCTT3D_Other,			//其它
};

// action结果到3D
struct KSkillResultTo3D
{
	enumCharacterTypeTo3D m_eSrcType;
	enumCharacterTypeTo3D m_eDestType;
	enum
	{
		enumKRT_HP,					//显示血值
		enumKRT_MP,					//显示魔值
		enumKRT_Absorb,				//显示“吸收”
		enumKRT_Immunity,			//免疫
		enumKRT_Counteract,			//躲闪
		enumKRT_MagicCounteract,	//法术抵抗
		enumKRT_ReceiveHigh,		//高血量收妖成功
		enumKRT_ReceiveMid,			//中血量收妖成功
		enumKRT_ReceiveLow,			//低血量收妖成功
		enumKRT_ReceiveFail,		//收妖失败
		enumKRT_Relation,			//亲密度
	}m_eActionResultType;

	enum
	{
		enumKRT_Normal,		//正常攻击
		enumKRT_Deadliness,	//致命一击
		enumKRT_IgnoreDep,	//会心一击
	}m_eHowToActionResult;

	enum ENUM_CLASSFICATION
	{
		enumKRT_UnknownClass,	//未知分类
		enumKRT_Attack,	//攻击
		enumKRT_Heal,		//治疗
		enumKRT_Debuff,	//物理减益
		enumKRT_BeAbsorbedHP,	//被吸取HP
		enumKRT_BeAbsorbedMP,	//被吸取MP
		enumKRT_AbsorbHP,	//吸取HP
		enumKRT_AbsorbMP,	//吸取MP
		enumKRT_AbsorbFaith,	//获得修为
	}m_eActionClassification;

	BOOL m_bIsDot;

	DWORD m_dwSrcID;		//A方ID
	DWORD m_dwDestID;		//D方ID
	DWORD m_dwSkillID;		//SkillID
	BYTE m_byActionOrder;	//Action是Skill的第几个Action
	INT m_nValue;			//Action结果数值，只有m_eHurtResultType == enumKRT_HP/MP时有效

	inline BOOL IsShowHP() const
	{
		return m_eActionResultType == enumKRT_HP;
	}
	inline BOOL IsShowMP() const
	{
		return m_eActionResultType == enumKRT_MP;
	}
	inline BOOL IsAbsorb() const
	{
		return m_eActionResultType == enumKRT_Absorb;
	}
	inline BOOL IsImmunity() const
	{
		return m_eActionResultType == enumKRT_Immunity;
	}
	inline BOOL IsCounteract() const
	{
		return m_eActionResultType == enumKRT_Counteract;
	}
	inline BOOL IsMagicCounteract() const
	{
		return m_eActionResultType == enumKRT_MagicCounteract;
	}
	inline BOOL IsDeadliness() const
	{
		return m_eHowToActionResult == enumKRT_Deadliness;
	}
	inline BOOL IsIgnoreDep() const
	{
		return m_eHowToActionResult == enumKRT_IgnoreDep;
	}
	inline void Clear()
	{
		m_eSrcType = enumCTT3D_Other;
		m_eDestType = enumCTT3D_Other;
		m_eActionResultType = enumKRT_Immunity;
		m_eHowToActionResult = enumKRT_Normal;
		m_eActionClassification = enumKRT_UnknownClass;

		m_dwSrcID = 0;			//A方ID
		m_dwDestID = 0;			//D方ID
		m_dwSkillID = 0;		//SkillID
		m_byActionOrder = 0;	//Action是Skill的第几个Action
		m_nValue = 0;			//Action结果数值，只有m_eHurtResultType == enumKRT_HP/MP时有效
		m_bIsDot = 0;
	}

	//根据结果包内容得到需要输出的MsgID
	INT ParseMessageID()
	{
		bool bMainPlayer = false;
		bool bMainMagicBaby = false;	//也包括召唤兽
		if(m_eDestType == enumCTT3D_MainPlayer)
			bMainPlayer = true;
		if(m_eDestType == enumCTT3D_MainMagicBaby)
			bMainMagicBaby = true;
		if((m_eActionClassification == enumKRT_BeAbsorbedMP || m_eActionClassification == enumKRT_AbsorbMP)
			&& m_eDestType == enumCTT3D_TreasureCase) //宝箱的魔值变化不显示
		{
			return 0;
		}
		if(m_eActionResultType == enumKRT_ReceiveHigh)
		{	
			return 50562;
		}
		else if(m_eActionResultType == enumKRT_ReceiveMid)
		{
			return 50563;
		}
		else if(m_eActionResultType == enumKRT_ReceiveLow)
		{
			return 50564;
		}
		else if(m_eActionResultType == enumKRT_ReceiveFail)
		{
			return 50565;
		}
		else if(m_eActionResultType == enumKRT_Relation)
		{
			return 50592;
		}

		if(IsImmunity()) // 如果是免疫
		{
			if(bMainPlayer)
			{
				return 50526;
			}
			else if(bMainMagicBaby)
			{
				return 57036;
			}
			else
			{
				if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
				{
					return 50567;
				}
				else
				{
					return 50500;
				}
			}
		}
		else
		{
			if(m_eActionClassification == enumKRT_Attack) // 如果是攻击
			{
				if(IsCounteract())			//躲闪
				{
					if(bMainPlayer)
						return 50538;
					else if(bMainPlayer)
						return 57040;
					else
					{
						if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
						{
							return 50571;
						}
						else
						{
							return 50504;
						}
					}
				}
				else 
				{
					if(m_bIsDot)
					{
						if(bMainPlayer)
						{
							return 50595;
						}
						else if(bMainMagicBaby)
						{
							return 57041;
						}
						else
						{
							if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
							{
								return 51957;
							}
							else
							{
								return 50593;
							}
						}
					}
					else 
					{
						if(m_eHowToActionResult == enumKRT_Normal) //正常攻击
						{
							if(bMainPlayer)
								return 50535;
							else if(bMainMagicBaby)
							{
								return 57037;
							}
							else
							{
								if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
								{
									return 50568;
								}
								else
								{
									return 50501;
								}
							}
						}
						else if(m_eHowToActionResult == enumKRT_Deadliness)//致命一击
						{
							if(bMainPlayer)
								return 50536;
							else if(bMainMagicBaby)
								return 57038;
							else
							{
								if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
								{
									return 50569;
								}
								else
								{
									return 50502;
								}
							}
						}
						else if(m_eHowToActionResult == enumKRT_IgnoreDep)//会心一击
						{
							if(bMainPlayer)
								return 50537;
							else if(bMainMagicBaby)
								return 57039;
							else
							{
								if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
								{
									return 50570;
								}
								else
								{	
									return 50503;
								}
							}
						}
					}
				}
			}
			else if(m_eActionClassification == enumKRT_Heal)	//治疗
			{
				if(m_bIsDot)
				{
					if(bMainPlayer)
					{
						return 50598;
					}
					else if(bMainMagicBaby)
					{
						return 57049;
					}
					else
					{
						if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
						{
							return 51961;
						}
						else
						{
							return 50597;
						}
					}
				}
				else
				{
					if(m_eHowToActionResult == enumKRT_Normal) //正常攻击
					{
						if(bMainPlayer)
							return 50543;
						else if(bMainMagicBaby)
							return 57047;
						else
							return 50519;
					}
					else if(m_eHowToActionResult == enumKRT_Deadliness)//致命一击
					{
						if(bMainPlayer)
							return 50544;
						else if(bMainMagicBaby)
							return 57048;
						else
							return 50520;
					}
				}
			}
			else if(m_eActionClassification == enumKRT_Debuff)	//物理减益
			{
				if(IsCounteract())			//躲闪
				{
					if(bMainPlayer)
						return 50545;
					else if(bMainMagicBaby)
						return 57050;
					else
					{
						if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
						{
							return 50584;
						}
						else
						{
							return 50517;
						}
					}
				}
			}
			else if(m_eActionClassification == enumKRT_BeAbsorbedHP)  //被吸收血
			{
				if(IsMagicCounteract())			//抵抗
				{
					if(bMainPlayer)
						return 50548;
					else if(bMainMagicBaby)
						return 57053;
					else
					{
						if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
						{
							return 50589;
						}
						else
						{
							return 50522;
						}
					}
				}
				else if(m_eHowToActionResult == enumKRT_Normal)
				{
					if(m_bIsDot)
					{
						if(bMainPlayer)
						{
							return 51953;
						}
						else if(bMainMagicBaby)
						{
							return 57054;
						}
						else
						{
							if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
							{
								return 51959;
							}
							else
							{
								return 51951;
							}
						}
					}
					else
					{
						if(bMainPlayer)
							return 50547;	
						else if(bMainMagicBaby)
							return 57052;
						else
						{
							if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
							{
								return 50588;
							}
							else
							{
								return 50521;
							}
						}
					}
				}
			}
			else if(m_eActionClassification == enumKRT_BeAbsorbedMP)  //被吸收魔
			{
				if(IsMagicCounteract())			//抵抗
				{
					if(bMainPlayer)
						return 50550;
					else if(bMainMagicBaby)
						return 57056;
					else
					{
						if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
						{
							return 50591;
						}
						else
						{
							return 50524;
						}
					}
				}
				else if(m_eHowToActionResult == enumKRT_Normal)
				{
					if(m_bIsDot)
					{
						if(bMainPlayer)
						{
							return 51954;
						}
						else if(bMainMagicBaby)
						{
							return 57057;
						}
						else
						{
							if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
							{
								return 51960;
							}
							else
							{
								return 51952;
							}
						}
					}
					else
					{
						if(bMainPlayer)
							return 50549;		
						else if(bMainMagicBaby)
							return 57055;
						else
						{
							if(m_eSrcType == enumCTT3D_MainMagicBaby) //如果是主角法宝打的
							{
								return 50590;
							}
							else
							{
								return 50523;
							}
						}
					}
				}
			}
			else if(m_eActionClassification == enumKRT_AbsorbHP)
			{
				if(m_bIsDot)
				{
					if(bMainPlayer)
						return 51955;
					if(bMainMagicBaby)
						return 57059;
				}
				else
				{
					if(bMainPlayer)
						return 50551;
					if(bMainMagicBaby)
						return 57058;
				}
			}
			else if(m_eActionClassification == enumKRT_AbsorbMP)
			{
				if(m_bIsDot)
				{
					if(bMainPlayer)
						return 51956;
					else if(bMainMagicBaby)
					{
						return 57060;
					}
				}
				else 
				{
					if(bMainPlayer)
						return 50552;
					else if(bMainMagicBaby)
					{
						return 57061;
					}
					else if(m_eDestType == enumCTT3D_MainMagicBaby)
					{
						return 51963;
					}
				}
			}
		}
		return 0;
	}
};
// 任务物品添加、删除、检查结构
struct KQuestItemsDataParam
{
	KQuestItemsDataParam()
	{
		memset(this , 0 , sizeof(KQuestItemsDataParam));
	}
	BOOL	AddItemData(INT dwItemID , INT dwItemNum)
	{
		if (ItemCount + 1 >= CFG_MAX_CHECK_QUEST_ITEM_COUNT)
		{
			return FALSE;
		}
		for (int i = 0; i < ItemCount; i++)
		{
			if (ItemID[i] == dwItemID)
			{
				ItemNum[i] += dwItemNum;
				return TRUE;
			}
		}
		ItemID[ItemCount] = dwItemID;
		ItemNum[ItemCount]= dwItemNum;
		ItemCount++;
		return TRUE;
	}
	int	ItemCount;
	INT	ItemID[CFG_MAX_CHECK_QUEST_ITEM_COUNT];
	INT	ItemNum[CFG_MAX_CHECK_QUEST_ITEM_COUNT];
};


struct KPos2D
{
	float fx;
	float fy;
};
//由消息LOGIC_MAIN_PLAYER_SKILL_START_COOLING使用
struct KSkillCDInfo
{
	DWORD m_dwSkillID;	//技能ID
	DWORD m_dwSkillCDGroupID;	//技能冷却组ID
	DWORD m_nTotalTime;	//总时间(ms)
	DWORD m_nLastTime;	//剩余(ms)
	BYTE m_byType;		// 技能拥有者类型
	enum
	{
		enumCommonSkill,
		enumItemSkill,
	} m_skillType;
};

/// 通知更改鼠标表现
struct KMouseStateInfo
{
	BOOL m_idHold;		// true 悬挂 false 就是更换鼠标图片
	char szImageName[MAX_ITEM_NAME_LEN];
	int nImageID;		// 当非hold时只需告知什么类型，表现自己选择资源
};


// 鼠标状态数据结构
struct KMouseStateType
{
	KMouseStateType()
	{
		Init();
	}
	VOID Init()
	{
		memset(this , 0 , sizeof(KMouseStateType));
	}
	DWORD	dwStateID;
	// UI Block
	DWORD	dwLogicType;
	DWORD	dwLogicPos;
	// 3D
};

// 图标类型
enum enumIconType
{
	enumIT_Normal = 0,	// 正常图标
	enumIT_Brown,		// 变灰图标
	enumIT_Invalid,		// 无图标
};

// 技能图标变化通知
struct KSkillIconAnnoucement
{
	BYTE m_nWhosIcon;			//谁的图标（玩家or法宝）需要变化，对应KWorldObj里面的eWorldObjectKind类型
	DWORD m_dwSkillID;			//要改变的技能id
	enumIconType m_eIconType;	//要变成的图标类型
};

enum QuestionAnswer
{
	enum_Question_Start,
	enum_Question_GetInfo,
	enum_Question_AnswerInfo,
	enum_Question_ReturnQuestionInfo,
	enum_Question_FinishQuestionInfo,
	enum_Question_End,
};

// 游戏意见反馈和问题回答
struct KQuestionAnserData
{
	KQuestionAnserData()
	{
		memset(this , 0 , sizeof(KQuestionAnserData));
	}
	DWORD	dwSubCmd;
	INT		nQuestionID;
	CHAR	strQuestion[64];
	CHAR	strAnswer1[64];
	CHAR	strAnswer2[64];
	CHAR	strAnswer3[64];
	CHAR	strAnswer4[64];
};

//得到当前技能的显示状态
struct KSkillState
{
	BYTE m_byType;		// 技能拥有者类型,wokPlayer/wokMagicBaby
	KSkillCDGroupResult skillCDResult;	//CoolDown信息
	enumIconType iconType;				//图标信息
	KSkillState() : m_byType(0){}
};

#pragma pack(push, 1)
//Action物理伤害和魔法伤害的结果
struct KActionDamageResult
{
	void Fill(
		INT nHP
		, INT nHPRealIncrease
		, BOOL bImmunity
		, BOOL bEvasion
		, BOOL bDeadliness
		, BOOL bIgnoreDep)
	{
		m_nHP = nHP;
		m_nHPRealIncrease = nHPRealIncrease;
		m_bIsImmunity = bImmunity;
		m_bIsEvation = bEvasion;
		m_bIsDeadliness = bDeadliness;
		m_bIsIgnoreDep = bIgnoreDep;
	}

	int getHP() const {return m_nHP;}
	bool isImmunity() const {return 1==m_bIsImmunity;}
	bool isEvation() const {return 1==m_bIsEvation;}
	int getHPRealIncrease() const {return m_nHPRealIncrease;}
	bool isDeadliness() const {return 1==m_bIsDeadliness;}
	bool isIgnoreDep() const {return 1==m_bIsIgnoreDep;}

	INT		m_nHP : 30;
	// 是否免疫
	DWORD	m_bIsImmunity : 1;
	// 是否豁免
	DWORD	m_bIsEvation : 1;
	INT		m_nHPRealIncrease : 30;
	// 是否致命
	DWORD	m_bIsDeadliness : 1;
	// 是否会心
	DWORD	m_bIsIgnoreDep : 1;
};

//Action增加Buff的结果
struct KActionAddBuffResult
{
	void Fill(BYTE byteAddBuffResult, WORD wBuffID, BYTE stacks, DWORD dwSrcID, DWORD dwDuration)
	{
		m_byteAddBuffResult = byteAddBuffResult;
		m_wBuffID = wBuffID;
		m_byStacks = stacks;
		m_dwSrcID = dwSrcID;
		m_dwDuration = dwDuration;
	}
	int getAddBuffResult() const {return m_byteAddBuffResult;}
	int getStacks() const {return m_byStacks;}
	int getDuration() const {return m_dwDuration;}
	int getBuffID() const {return m_wBuffID;}
	int getSrcID() const {return m_dwSrcID;}

	/**
	 * 增加Buff的结果
	 * 0 抵抗
	 * 1 成功
	 */
	BYTE	m_byteAddBuffResult	:	1;
	// 层数
	BYTE	m_byStacks			:	7;
	// 持续时间
	DWORD	m_dwDuration;
	// 为0代表Buff相关的数据没有启用
	WORD	m_wBuffID;
	// 施放者
	DWORD	m_dwSrcID;
};

//Action一般结果
struct KActionCommonResult
{
	/*
	 * 0. 免疫
	 * 1. 成功
	 * 2. 失败
	 * 3. 低血收妖成功
	 * 4. 中血收妖成功
	 * 5. 高血收妖成功
	 */
	enum
	{
		enumACR_Immune		= 0,
		enumACR_Success		= 1,
		enumACR_Fail		= 2,
		enumACR_LowSuccess	= 3,
		enumACR_MidSuccess	= 4,
		enumACR_HighSuccess	= 5,
	};
	BYTE m_byResult;
};

//吸收Action的结果
struct KActionDrainResult
{
	void Fill(INT srcVal, BYTE srcType, INT destVal, BYTE destType)
	{
		m_nSrcVal = srcVal;
		m_bySrcValType = srcType;
		m_nDestVal = destVal;
		m_byDestValType = destType;
	}
	int getSrcVal() const {return m_nSrcVal;}
	int getSrcValType() const {return m_bySrcValType;}
	int getDestVal() const {return m_nDestVal;}
	int getDestValType() const {return m_byDestValType;}

	//吸收值
	INT		m_nSrcVal : 29;
	//吸收的东西（0 HP，1 MP，2 RP，3 灵气）
	DWORD	m_bySrcValType : 3;
	//被吸收值
	INT		m_nDestVal : 29;
	//被吸的东西（0 HP，1 MP，2 RP，3 灵气）
	DWORD	m_byDestValType : 3;
};

//回复能量Action的结果
struct KActionRestoreEnergyResult
{
	void Fill(BYTE valType, INT val)
	{
		m_byValType = valType;
		m_nVal = val;
	}
	int getVal() const {return m_nVal;}
	int getValType() const {return m_byValType;}

	//回复值
	INT		m_nVal : 29;
	//回复的东西（0 HP，1 MP，2 RP，3 灵气）
	DWORD	m_byValType : 3;
};

struct KActionReadResult
{
	enum ENUM_RESULT_TYPE
	{
		enumERT_CommonResult,
		enumERT_DamageResult,
		enumERT_AddBuffResult,
		enumERT_DrainResult,
		enumERT_RestoreEnergyResult,
	} m_eResultType;
	union
	{
		KActionCommonResult m_CommonResult;
		KActionDamageResult m_DamageResult;
		KActionDrainResult m_DrainResult;
		KActionAddBuffResult m_AddBuffResult;
		KActionRestoreEnergyResult m_RestoreEnergyResult;
	};
};

struct KActionResult
{
	int m_eResultType;
	bool m_bIsDot;			// 是否是dot
	int m_dwSrcID;			// A方ID
	int m_dwDestID;			// D方ID
	int m_dwSkillID;		// SkillID
	int m_byActionOrder;	// Action是Skill的第几个Action
	int actionType;			// Action类型
	KActionReadResult m_Result;
};

// 当character出现在视野中的时候要同步的buff信息
struct KAddBuffWhenAppearInfo
{
	DWORD	dwSrcID;
	DWORD	dwBuffID;
	INT		nRemainTime;	//buff剩余时间
	DWORD	dwElapseTime;	//buff经过时间
	BYTE	byStacks;		//叠加层数
};

//设定快捷栏栏位的内容
struct SetShortcutInfo
{
	BYTE	byPosID;	//位置ID
	KShortcutData shortcutData;
};

#pragma pack(pop)

// 技能使用过程中的信息
struct KSkillInfoTo3D
{
	// 技能id
	DWORD m_dwSkillID;
	// 技能弹道速度
	DWORD m_dwMissileSpeed;
	// 技能加速比
	FLOAT m_fActionAccRate;
	// 技能名字（以后是否使用ID？）
	const char* m_szSkillName;
	// 总时间
	DWORD m_dwTotalTime;
	// 额外要显示的文字
	const char* m_szExtraInfo;
	KSkillInfoTo3D()
		: m_dwSkillID(0)
		, m_dwMissileSpeed(0)
		, m_fActionAccRate(0.0f)
		, m_szSkillName("")
		, m_dwTotalTime(0)
		, m_szExtraInfo(""){}
};

// 技能使用过程中的触发的事件(收妖)
struct KSkillEventTo3D
{
	// 技能event id
	DWORD m_dwEventID;
	// 技能弹道速度
	DWORD m_dwMissileSpeed;
};

// 3D要求延时播放SkillResult
struct KSkillResultRequireBy3D
{
	// 源ID
	DWORD	dwSrcID;
	// 从Shoot开始的延时
	DWORD	dwDelayTime;
	// Action的序号
	BYTE	byActionOrder;
	// 技能ID 
	DWORD	dwSkillID;
	KTarget	target;

	DWORD uParam;	// ACTION POINT
	DWORD lParam;	// ACTION INDEX
};

struct KDynamicNpcOperationParm
{
	enum ENUM_DynamicNpcOperation_Type
	{
		enum_DynamicNpcOperation_Unknow = 0,
		enum_DynamicNpcOperation_Add,
		enum_DynamicNpcOperation_Del,

		enum_DynamicNpcOperation_Count,
	};

	KDynamicNpcOperationParm() {memset(this, 0, sizeof(KDynamicNpcOperationParm));}
	ENUM_DynamicNpcOperation_Type m_dwOpType;
	DWORD m_dwRandomGroupID;
	DWORD m_dwTypeID;
	DWORD m_dwPointID;
	DWORD m_dwCharacterID;
	DWORD m_dwQuestOwnerID;
	DWORD m_dwQuestID;
	DWORD m_dwLevel;
	DWORD m_dwCustomData;
	DWORD m_dwExistTime;
	DWORD m_dwExistTimeOnLeaveFight;
	DWORD	dwMasterID;
	DWORD	dwStopDistance;
	DWORD	dwFollowDistance;
	DWORD	dwWaitDistance;
	INT		m_nHateValue;
	FLOAT	m_dwPosX;
	FLOAT	m_dwPosY;
	FLOAT	m_dwPosZ;
	WORD	m_wPatrolPointID;
	BYTE	m_byPatrolType;
	BYTE	m_bCanRelive;
	BYTE	byIsPrivate;
	BYTE	byWaitOrForce;
	BYTE	byReason;
	DWORD	dwPrivateOwnerID;
	DWORD	dwPrivateLevel;
	USHORT	shAngle;
	BOOL	bReliveDirectly;
	BOOL	bForceBreathe;	// 是否强制呼吸
	BOOL	bTalkSwitch;	// 是否允许互动
	DWORD	m_dwRelatedCharID;	// 关联角色id
	DWORD   GroupTypeID;		// 队类型ID(40000+)
	DWORD   GroupInstanceID;	// 队索引(1-n)
	BYTE	TeamID;				// 队内组ID(1-8)
};

struct KDynamicNpcResult
{
	KDynamicNpcResult() {memset(this, 0, sizeof(KDynamicNpcResult));}
	DWORD dwOwnerID;
	DWORD dwMasterID;
	DWORD dwPrivateOwnerID;
	DWORD dwPrivateLevel;
	BYTE  byReason;
	BOOL  bReliveDirectly;
	DWORD dwLevel;
	DWORD m_dwQuestOwnerID;
	DWORD m_dwQuestID;
	DWORD	dwRelatedCharID;
};

struct KEquipItemParm
{
	DWORD m_wPos;
	DWORD m_wItemID;
	DWORD m_wDisplayID;
	BYTE m_byStrengthLv;
	BYTE m_byItemQuality;
};

//struct KWorldConfigInfo
//{
//	struct KSceneConfigInfo
//	{
//		KSceneConfigInfo() :m_dwSceneIndexX(0), m_dwSceneIndexY(0) {}
//		DWORD m_dwSceneIndexX;
//		DWORD m_dwSceneIndexY;
//	};
//
//	struct KFileConfigInfo
//	{
//		KFileConfigInfo() {Init();}
//		VOID Init() {memset(this, 0, sizeof(KFileConfigInfo));}
//		DWORD m_dwID;
//		CHAR m_szFile[MAX_PATH];
//	};
//
//	struct KDuplicateConfigInfo
//	{
//		KDuplicateConfigInfo()
//		{
//			m_dwMapID = 0;
//			m_dwMapCount = 0;
//			m_dwStartSceneX = 0;
//			m_dwStartSceneY = 0;
//			m_dwLength = 0;
//			m_dwHeight = 0;
//			m_dwBornX = 0.0f;
//			m_dwBornY = 0.0f;
//			m_dwBornZ = 0.0f;
//			m_dwReliveX = 0.0f;
//			m_dwReliveY = 0.0f;
//			m_dwReliveZ = 0.0f;
//			m_szMax_Scene_Count = 0;
//			memset(&m_szWorld_File, 0, sizeof(m_szWorld_File));
//		}
//		DWORD m_dwSceneType;
//		DWORD m_dwMapID;
//		DWORD m_dwMapCount;
//		DWORD m_dwStartSceneX;
//		DWORD m_dwStartSceneY;
//		DWORD m_dwOffsetSceneX;
//		DWORD m_dwOffsetSceneY;
//		DWORD m_dwLength;
//		DWORD m_dwHeight;
//		FLOAT m_dwBornX;
//		FLOAT m_dwBornY;
//		FLOAT m_dwBornZ;
//		USHORT m_shBornDirection;
//		FLOAT m_dwReliveX;
//		FLOAT m_dwReliveY;
//		FLOAT m_dwReliveZ;
//		USHORT m_shReliveDirection;
//		DWORD m_dwExitMapID;
//		FLOAT m_dwExitX;
//		FLOAT m_dwExitY;
//		FLOAT m_dwExitZ;
//		USHORT m_shExitDirection;
//		DWORD m_szMax_Scene_Count;
//		::System::Collections::KVector<KSceneConfigInfo> m_Scene_Info;
//		CHAR m_szWorld_File[MAX_PATH];
//		::System::Collections::KVector< KFileConfigInfo > m_Monster_Files;
//		::System::Collections::KVector< KFileConfigInfo > m_TreasureCase_Files;
//	};

	//struct KClientConfigInfo
	//{
	//	KClientConfigInfo() { memset(this, 0, sizeof(KClientConfigInfo)); }
	//	DWORD m_dwID;
	//	DWORD m_dwWidthCount;
	//	DWORD m_dwHeightCount;
	//	CHAR m_szWorld_File[MAX_PATH];
	//	CHAR m_szMap_File[MAX_PATH];
	//	CHAR m_szMonster_File[MAX_PATH];
	//};

//	KWorldConfigInfo()
//	{
//		m_dwMax_Player_Count = 0;
//		m_dwMax_Npc_Count = 0;
//		m_dwMax_MagicBaby_Count = 0;
//		m_dwMax_Static_MagicBaby_Count = 0;
//		m_dwMax_Item_Count = 0;
//		m_dwMax_Ground_Item_Count = 0;
//		m_dwMax_Attribute_Count = 0;
//		m_dwMax_Skill_Count = 0;
//		m_dwMax_Skill_Event_Count = 0;
//		m_dwMax_Buff_State_Count = 0;
//		m_dwMax_Transport_Count = 0;
//		m_dwMax_TreasureCase_Count = 0;
//		m_dwMax_Quest_Count = 0;
//		m_dwMax_QuestEvents_Count = 0;
//#ifdef _SERVER
//		m_dwMax_Active_Region_Rate = 0;
//		memset(m_szWorld_File, 0, sizeof(m_szWorld_File));
//		m_szMax_Scene_Count = 0;
//
//#ifdef _DUPLICATESERVER
//		m_dwDuplicateTypeCount = 0;
//#endif // DUPLICATESERVER
//#endif // _SERVER
//
//#ifdef _CLIENT
//		m_dwMax_MapCount = 0;
//#endif // _CLIENT
//	}

//	::System::Collections::KVector< KFileConfigInfo > m_Monster_Files;
//	::System::Collections::KVector< KFileConfigInfo > m_TransmissionPoint_Files;
//	DWORD m_dwMax_Player_Count;
//	DWORD m_dwMax_Npc_Count;
//	DWORD m_dwMax_MagicBaby_Count;
//	DWORD m_dwMax_Static_MagicBaby_Count;
//	DWORD m_dwMax_Item_Count;
//	DWORD m_dwMax_Ground_Item_Count;
//	DWORD m_dwMax_Attribute_Count;
//	DWORD m_dwMax_Skill_Count;
//	DWORD m_dwMax_Skill_Event_Count;
//	DWORD m_dwMax_Buff_State_Count;
//	DWORD m_dwMax_Transport_Count;
//	DWORD m_dwMax_TreasureCase_Count;
//	DWORD m_dwMax_Quest_Count;
//	DWORD m_dwMax_QuestEvents_Count;
//
//#ifdef _SERVER
//	DWORD m_dwMax_Active_Region_Rate;
//	CHAR m_szWorld_File[MAX_PATH];
//
//	DWORD m_szMax_Scene_Count;
//	KVector<KSceneConfigInfo> m_Scene_Info;
//
//#ifdef _DUPLICATESERVER
//	DWORD m_dwDuplicateTypeCount;
//	KVector<KDuplicateConfigInfo> m_Duplicate_Info;
//#endif // DUPLICATESERVER
//
//#endif // _SERVER
//
//#ifdef _CLIENT
//	DWORD m_dwMax_MapCount;
//	KVector<KClientConfigInfo> m_ClientMap_Info;
//#endif // _CLIENT
//};

/**
 * Warning: 出于安全因素的考虑，绝对不允许Client发出这样的包
 *
 * 通用变长包
 *
 * 例1，发送方写入，同步:
 * KCommonPacket cp;
 * cp << (INT)123 << (INT)456;	//发送方写入INT，最好指定写入数据的类型
 * cp.WriteString("Hello!");	//发送方写入string
 * pDestChar->SyncData(s_nSomePacketCode, cp, cp.GetSendLength(), eSyncAllSightPlayer);
 *
 * 例2，同步，接收方:
 * //得到包内容
 * KCommonPacket cp(pPacket, nPacketLength);
 * //然后把cp传给处理的地方，如直接读出
 * INT m, n;
 * cp >> m >> n;				//接收方读出INT
 * char buff[1000];
 * cp.ReadString(buff, 1000);	//接收方直接读出string
 */
#define COMMON_PACKET_USE_DYNAMIC_MEMORY
struct KCommonPacket	//通用变长包
{
	//最大包长
	static const DWORD MAX_PACKET_LENGTH = 10 * (1 << 10);
	//包头长
	static const DWORD HEAD_SIZE = sizeof(DWORD);
	//BUFFER长
	static const DWORD BUFF_SIZE = MAX_PACKET_LENGTH - HEAD_SIZE;
	CHAR*	m_chBuff;						//缓冲指针
	DWORD	m_dwBuffSize;					//缓冲大小
	DWORD	m_dwHeadPos;					//缓冲区首指针
#ifndef COMMON_PACKET_USE_DYNAMIC_MEMORY
	static CHAR staticBuff[MAX_PACKET_LENGTH];
#else
	CHAR	selfBuff[MAX_PACKET_LENGTH];
#endif

private:
	BOOL	m_bNeedDelete;					//是否由KCommonPacket管理缓冲区的创建、释放
public:
	KCommonPacket() : m_dwBuffSize(0), m_dwHeadPos(0), m_bNeedDelete(FALSE)
	{
#ifdef COMMON_PACKET_USE_DYNAMIC_MEMORY
		//m_bNeedDelete = TRUE;
		//m_chBuff = new char[MAX_PACKET_LENGTH];
		m_bNeedDelete = FALSE;
		m_chBuff = selfBuff;
#else
		m_bNeedDelete = FALSE;
		m_chBuff = staticBuff;
#endif // COMMON_PACKET_USE_DYNAMIC_MEMORY
		memcpy(m_chBuff, &m_dwBuffSize, sizeof(m_dwBuffSize));
		m_chBuff += HEAD_SIZE;
	}
	~KCommonPacket()
	{
		if (m_bNeedDelete)
		{
			m_chBuff -= HEAD_SIZE;
			delete m_chBuff;
		}
	}
	//从const void*初始化一个包（接收方用）
	KCommonPacket(const void* pPacket, DWORD packLength)
	{
		m_bNeedDelete = FALSE;
		m_dwHeadPos = 0;
		if (packLength < HEAD_SIZE)
		{
			ASSERT_I(FALSE);//包过短
			m_dwBuffSize = 0;
			return;
		}
		memcpy(&m_dwBuffSize, pPacket, HEAD_SIZE);
		if (m_dwBuffSize > packLength - HEAD_SIZE || m_dwBuffSize > BUFF_SIZE)
		{
			ASSERT_I(FALSE);//包错误，长度过长
			m_dwBuffSize = 0;
			return;
		}
		m_chBuff = (char*)((char*)pPacket + HEAD_SIZE);
	}

	//流中是否有数据
	inline BOOL IsEof() const
	{
		return m_dwHeadPos >= m_dwBuffSize;
	}
	//流复位
	inline void Reset()
	{
		m_dwBuffSize = 0;
		m_dwHeadPos = 0;
		memcpy(m_chBuff - HEAD_SIZE, &m_dwBuffSize, sizeof(m_dwBuffSize));
	}
	//得到发送长度
	inline DWORD GetSendLength() const
	{
		if (m_dwBuffSize == 0)
		{
			return 0;
		}
		return (HEAD_SIZE + m_dwBuffSize);
	}
	//得到Buff的长度
	inline DWORD GetBuffLength() const
	{
		return m_dwBuffSize;
	}
	//得到还没有处理的数据的长度
	inline DWORD GetDataLength() const
	{
		return m_dwBuffSize - m_dwHeadPos;
	}
	//得到还能写入的剩余长度
	inline DWORD GetRemainSize() const
	{
		return MAX_PACKET_LENGTH - GetSendLength();
	}
	inline operator CHAR*()
	{
		return (m_chBuff - HEAD_SIZE);
	}
	inline BOOL Write(const void* buff, DWORD dwSize)
	{
		if (m_dwBuffSize + dwSize >= BUFF_SIZE)
		{
			return FALSE;
		}
		memcpy(&(m_chBuff[m_dwBuffSize]), buff, dwSize);
		m_dwBuffSize += dwSize;
		memcpy((m_chBuff - KCommonPacket::HEAD_SIZE), &m_dwBuffSize, sizeof(m_dwBuffSize));
		return TRUE;
	}
	inline BOOL Read(void* buff, DWORD dwReadSize)
	{
		if (m_dwHeadPos + dwReadSize > m_dwBuffSize)
		{
			return FALSE;
		}
		memcpy(buff, &(m_chBuff[m_dwHeadPos]), dwReadSize);
		m_dwHeadPos += dwReadSize;
		return TRUE;
	}
	inline BOOL WriteString(const char* buff)
	{
		DWORD dwSize = strlen(buff) + 1;
		return Write(buff, dwSize);
	}
	inline BOOL ReadString(char* buff, DWORD buffSize)
	{
		DWORD dwReadSize = strlen(&(m_chBuff[m_dwHeadPos])) + 1;
		if (buffSize < dwReadSize) return FALSE;
		return Read(buff, dwReadSize);
	}
};

//内存copy输出流操作的定义 for KCommonPacket
template<typename TYPE>
KCommonPacket& operator<<(KCommonPacket &aStream, const TYPE& obj)
{
	if (aStream.m_dwBuffSize + sizeof(TYPE) >= aStream.BUFF_SIZE)
	{
		ASSERT_I(FALSE);
		return aStream;
	}
	memcpy(&(aStream.m_chBuff[aStream.m_dwBuffSize]), &obj, sizeof(TYPE));
	aStream.m_dwBuffSize += sizeof(TYPE);
	memcpy((aStream.m_chBuff - KCommonPacket::HEAD_SIZE), &aStream.m_dwBuffSize, sizeof(aStream.m_dwBuffSize));
	return aStream;
}

//内存copy输入流操作的定义 for KCommonPacket
template<typename TYPE>
KCommonPacket& operator>>(KCommonPacket &aStream, TYPE& obj)
{
	if (aStream.m_dwHeadPos + sizeof(TYPE) > aStream.m_dwBuffSize)
	{
		ASSERT_I(FALSE);/*读过界检测*/
		aStream.m_dwHeadPos = aStream.m_dwBuffSize;
		return aStream;
	}
	memcpy(&obj, &(aStream.m_chBuff[aStream.m_dwHeadPos]), sizeof(TYPE));
	aStream.m_dwHeadPos += sizeof(TYPE);
	return aStream;
}

//struct KNpc_Talk_Rsp
//{
//	#define MAX_NPC_TALK_OPTION_COUNT (128)
//	DWORD m_dwRspType;
//	DWORD m_dwPlayerID;
//	DWORD m_dwTargetID;
//	DWORD m_dwParm1;
//	DWORD m_dwParm2;
//	DWORD m_dwParm3;
//	DWORD m_dwParm4;
//
//	DWORD m_dwOptionCount;
//	DWORD m_dwFunctionCount;
//	DWORD m_dwQuestCount;
//	DWORD m_dwParms1[MAX_NPC_TALK_OPTION_COUNT];
//};

struct KNpc_AI_Command
{
	enum ENUM_COMMAND_TYPE
	{
		enum_Command_Unknow = 0,
		enum_Command_Relive,
		enum_Command_Relive_ok,
		enum_Command_Die,
		enum_Command_Disappear,
		enum_Command_SpecialAction,
		enum_Command_ChangeAngle,
		enum_Command_ChangeSpeed,
		enum_Command_ChangeMonsterType,
		enum_Command_Event,
		enum_Command_QuestOpeartion,
		enum_Command_CloseAllInterActiveObj,
		enum_Command_ChangePosition,
		enum_Command_TalkSwitch,
		enum_Command_Stop,
		enum_Command_Restore,
		enum_Command_ReleaseAllBuffs,
		enum_Command_RunEvent,
		enum_Command_EnterOrExitInvincibility,
		enum_Command_EnterOrExitFight,
		enum_Command_ChangeLevel,
		enum_Command_ChangeForceBreathe,
		enum_Command_Count,
	};

	DWORD	m_dwNpcID;
	DWORD	m_dwNpcCurrentTypeID;
	DWORD	m_dwNpcOriginTypeID;
	DWORD	m_dwNpcAI_Command;
	INT		m_nParm01;
	INT		m_nParm02;
	FLOAT	m_fParm03;
	FLOAT	m_fParm04;
	DWORD	m_dwParm05;
	::System::Collections::KString<MAX_NAME_DEF> m_strText;
	VOID Init() { m_dwNpcID = 0; m_dwNpcAI_Command = enum_Command_Unknow; m_dwNpcCurrentTypeID = 0; m_dwNpcOriginTypeID = 0;
	m_nParm01 =0; m_nParm02 = 0; m_fParm03 = 0.0f; m_fParm04 = 0.0f; m_dwParm05 = 0;m_strText.clear();}
};

struct KNpc_AI_Commands
{
	KNpc_AI_Command m_Commands;
	VOID Init() { m_Commands.Init(); }
};

struct KNpc_AI_Command_Result
{
	enum ENUM_COMMAND_RESULT_TYPE
	{
		enum_Command_Result_Faild = 0,
		enum_Command_Result_Relive_Succeed,
		enum_Command_Result_Die_Succeed,
		enum_Command_Result_Disappear_Succeed,
		enum_Command_Result_SpecialAction_Succeed,
		enum_Command_Result_ChangeAngle_Succeed,
		enum_Command_Result_ChangeSpeed_Succeed,
		enum_Command_Result_ChangeMonsterType_Succeed,
		enum_Command_Result_ChangePosition_Succeed,
		enum_Command_Result_Stop_Succeed,
		enum_Command_Result_ChangeExistTime_Succeed,
		enum_Command_Result_ChangeExistTimeOnLeaveFight_Succeed,
		enum_Command_Result_ChangeFollowDistance,
		enum_Command_Result_ChangeFightMode,
		enum_Command_Result_ChangeLevel,
		enum_Command_Result_ChangeForceBreathe,
		enum_Command_Result_ClearHateList,
		enum_Command_Result_Count,
	};
	DWORD	m_dwNpcID;
	DWORD	m_dwParm1;
	DWORD	m_dwParm2;
	DWORD	m_dwParm3;
	DWORD	m_dwParm4;
	DWORD	m_dwNpcAI_Command_Result;
	VOID Init() { m_dwNpcID = 0; m_dwNpcAI_Command_Result = enum_Command_Result_Faild; m_dwParm1 = 0; m_dwParm2 = 0, m_dwParm3 = 0; m_dwParm4 = 0;}
};

struct KNpc_AI_Command_Results
{
	KNpc_AI_Command_Result m_CommandResults;
	VOID Init() { m_CommandResults.Init(); }
};

struct KMapCellChanged
{
	DWORD	x;
	DWORD	y;
	BYTE	byObsType	: 4;
	BYTE	byObstacle	: 4;
	DWORD	dwHeight;
};


struct KWorldMapTeamMemeberInfo 
{
	float m_fX;
	float m_fY;
	float m_fZ;
	DWORD m_dwMapID;
	char m_szName[MAX_NAME_DEF];
};

struct KCharacterRelationshipReq
{
	DWORD dwID1;
	DWORD dwID2;
};

struct KCharacterRelationshipRsp
{
	DWORD dwRelationship;
	BOOL  bInSameTeam;
};

/**
 * 根据三个点计算向量1、2的方向，向量1 = (x1, y1)->(x0, y0)，向量2 = (x2, y2)->(x0, y0)
 * Param:
 *		in		x0, y0:	共同起点坐标
 *		in		x1, y1:	向量1的终点坐标
 *		in		x2, y2:	向量2的终点坐标
 * Return:
 *		如果向量1在向量2的右侧（包括方向相同和相反），返回TRUE，否则FALSE
 */
BOOL _Side(FLOAT x0, FLOAT y0, FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2);

/**
 * 判断一个点是否在一个矩形中
 * Param:
 *		in		testPtX, testPtY:	测试点坐标
 *		in		fx, fy:				测试矩形顶点坐标，要求点按顺时针保存
 * Return:
 *		如果testPt在矩形中（包括边界上）返回TRUE，否则FALSE
 */
BOOL _IsPtInRectangle(FLOAT testPtX, FLOAT testPtY, const FLOAT fx[4], const FLOAT fy[4]);

struct KBuffInfoToUI
{
	DWORD			m_dwBuffID;
	CHAR			m_szBuffName[32];
	CHAR			m_szBuffIcon[MAX_PATH];
// 	INT				m_nBuffLevel;		// Buff等级
	INT				m_nBuffPriority;	// Buff优先级
	DWORD			m_dwBuffDuration;	// Buff默认持续时间
	BYTE			m_byBuffType;		// Buff的类型 0 Good, 1 Bad, 2 Hidden

	INT				m_nDotInterval;		// DOT间隔时间
	INT				m_nAntiDispelRate;	// Buff防驱散率
	INT				m_nBuffClass;		// Buff类型
	INT				m_nBuffCurrentStacks;		// buff叠加层数
	INT				m_nBuffStacks;		// buff叠加层数上限
	INT				m_nBuffToolTipID;	// Buff的ToolTip
	DWORD			m_dwStatus;			// 标识Buff异常状态
	INT				m_nShowLastTime;	// 是否显示剩余时间
	struct 
	{
		DWORD		m_dwValueInfoCount; // 填充的Action的个数
		INT			m_nValue[20];
	} m_actionInfo;

	// DynInfo
	CHAR			m_szBuffCasterName[MAX_NAME_DEF];
	INT				m_nRemainTime;

	INT				m_nDuraType;		// 
	DWORD			m_dwExpireTime;

	BOOL			mbShowCaster;
};

struct KBuffDynamicInfoReq
{
	KBuffDynamicInfoReq():m_dwBuffID(0), m_dwCharacterID(0), m_dwSrcID(0) {}
	DWORD m_dwBuffID;
	DWORD m_dwCharacterID;
	DWORD m_dwSrcID;
};

template <typename _T>
void Swap(_T& a, _T& b)
{
	_T temp;
	temp = a;
	a = b;
	b = temp;
}

//LogicWorld设定的Lua回调接口信息
struct KScriptCallbackFuncFormat
{
	const char* funcName;	//脚本函数名
	void* funcPtr;			//函数地址
	enum ENUM_FUNC_TYPE
	{
		enumFT_INT__void,
		enumFT_INT__1_DWORD,
		enumFT_INT__2_DWORD,
		enumFT_INT__3_DWORD,
		enumFT_STR__void,
		enumFT_STR__1_DWORD,
		enumFT_STR__2_DWORD,
		enumFT_void__void,
		enumFT_void__1_INT,
		enumFT_void__2_INT,
		enumFT_void__3_INT,
		enumFT_void__4_INT,
		enumFT_void__5_INT,
		enumFT_bool__void,
		enumFT_bool__1_INT,
		enumFT_bool__2_INT,
		enumFT_bool__3_INT,
		enumFT_bool__4_INT,
		enumFT_bool__5_INT,
		enumFT_void__1_bool,
		enumFT_float__void,
		enumFT_float__1_INT,
		enumFT_float__2_INT,
		enumFT_float__3_INT,
		enumFT_float__4_INT,
		enumFT_float__5_INT,
	} funcType;

	KScriptCallbackFuncFormat()
	{
		funcName = "";
		funcPtr = NULL;
		funcType = enumFT_INT__void;
	}
};

//好友系统用的数据（UI到逻辑）
struct KFriendMsgDataToLogic
{
	//好友系统消息枚举(逻辑与UI通信用)
	enum ENUM_FRIEND_SYSTEM_MSG
	{
		enumFSMA_ReqRefresh		= 0,	//刷新相识列表
		enumFSMA_ReqAdd			= 1,	//相识增加新人
		enumFSMA_ReqDelete		= 2,	//相识删除
		enumFSMA_ReqModify		= 3,	//相识修改
		enumFSMA_SearchPlayer	= 4,	//查找Player
		enumFSMA_GetCount		= 5,	//得到列表条目数

		enumFSMF_ReqRefresh		= 100,	//刷新好友列表
		enumFSMF_ReqAdd			= 101,	//好友增加新人
		enumFSMF_ReqDelete		= 102,	//好友删除
//		enumFSMF_ReqModify		= 103,	//好友修改
		enumFSMF_SearchPlayer	= 104,	//查找Player
		enumFSMF_ConfirmAdd		= 105,	//确认删除的回复
		enumFSMF_GetCount		= 106,	//得到列表条目数
		enumFSMF_GetPlayer		= 107,
        enumFSMF_SetAutoRefuse,// 自动拒绝好友申请
        enumFSMF_GetAutoRefuse,// 自动拒绝好友申请

		enumFSMB_ReqRefresh		= 200,	//刷新黑名单列表
		enumFSMB_ReqAdd			= 201,	//黑名单增加新人
		enumFSMB_ReqDelete		= 202,	//黑名单删除
//		enumFSMB_ReqModify		= 203,	//黑名单修改
		enumFSMB_SearchPlayer	= 204,	//查找Player
		enumFSMB_GetCount		= 205,	//得到列表条目数

		enumFSME_ReqRefresh		= 300,	//刷新仇人列表
		enumFSME_ReqDelete		= 302,	//仇人删除
		enumFSME_GetCount		= 305,	//得到列表条目数
        
	} m_msgID;
	inline BOOL IsAcqMsg() const
	{
		return m_msgID >= enumFSMA_ReqRefresh && m_msgID < enumFSMF_ReqRefresh;
	}
	inline BOOL IsFriendMsg() const
	{
		return m_msgID >= enumFSMF_ReqRefresh && m_msgID < enumFSMB_ReqRefresh;
	}
	inline BOOL IsBlackListMsg() const
	{
		return m_msgID >= enumFSMB_ReqRefresh && m_msgID < enumFSME_ReqRefresh;
	}
	inline BOOL IsEnemyMsg() const
	{
		return m_msgID >= enumFSME_ReqRefresh;
	}
	union
	{
		struct
		{
			bool state;
		}m_AutoRefuseState;
		struct
		{
			const char* m_szName;
			const char* m_szNote;
		} m_AReqAddParam;
		struct
		{
			DWORD dwID;
		} m_AReqDeleteParam;
		struct
		{
			DWORD dwID;
			const char* m_szModifyTo;
		} m_AReqModifyParam;
		struct
		{
			char buff[200][MAX_NAME];
			DWORD count;
			const static DWORD MAX_SIZE = 200;
		} m_ASearchParam;

		struct
		{
			const char* m_szName;
		} m_FReqAddParam;
		struct
		{
			DWORD dwID;
		} m_FReqDeleteParam;
		struct
		{
			char buff[200][MAX_NAME];
			DWORD count;
			const static DWORD MAX_SIZE = 200;
		} m_FSearchParam;
		struct
		{
			BOOL m_yes;
			DWORD m_srcID;
		} m_FConfirmAdd;
		struct
		{
			DWORD m_destID;
		} m_FJoinInTeam;

		struct
		{
			DWORD dwID;
		} m_EReqDeleteParam;

		struct
		{
			const char* m_szName;
		} m_BReqAddParam;
		struct
		{
			DWORD dwID;
		} m_BReqDeleteParam;
		struct
		{
			char buff[200][MAX_NAME];
			DWORD count;
			const static DWORD MAX_SIZE = 200;
		} m_BSearchParam;
		struct
		{
			DWORD	count;
			DWORD	maxCount;
		} m_countParam;
		struct
		{
			DWORD m_playerID;
			char m_playerName[MAX_NAME];	// 20
			INT m_nationality;				// 1
			INT m_sex;						// 1
			INT m_level;					// 1, 等级
			INT m_career;					// 职业
			INT m_subCareer;				// 副职
			INT m_friendRank;				// 1, 好友品级，称号，从Rank映射
			DWORD m_joinTime;				// 4, 加入好友的时间
			BOOL m_onlineState;				// 在线状态
		} m_getPlayer;
	};
};

//好友系统用的数据（逻辑到UI）
struct KFriendMsgDataToUI
{
	//好友系统消息枚举(逻辑与UI通信用)
	enum ENUM_FRIEND_SYSTEM_MSG
	{
		enumFSMA_Clear		= 0,	//清空相识列表
		enumFSMA_Add		= 1,	//相识增加新人
		enumFSMA_Delete		= 2,	//相识删除
		enumFSMA_Modify		= 3,	//相识修改

		enumFSMF_Clear		= 100,	//清空好友列表
		enumFSMF_Add		= 101,	//好友增加新人
		enumFSMF_Delete		= 102,	//好友删除
		enumFSMF_Modify		= 103,	//好友修改
		enumFSMF_AskConfirmAdd	= 104,	//确认是否增加好友

		enumFSMB_Clear		= 200,	//清空黑名单列表
		enumFSMB_Add		= 201,	//黑名单增加新人
		enumFSMB_Delete		= 202,	//黑名单删除
		enumFSMB_Modify		= 203,	//黑名单修改

		enumFSME_Clear		= 300,	//清空仇人列表
		enumFSME_Add		= 301,	//仇人增加新人
		enumFSME_Delete		= 302,	//仇人删除
		enumFSME_Modify		= 303,	//仇人修改

		enumFSMA_RefreshNum	= 1000,	//刷新数字显示
		enumFSMF_RefreshNum,	//刷新数字显示
		enumFSMB_RefreshNum,	//刷新数字显示
		enumFSME_RefreshNum,	//刷新数字显示
	} m_msgID;
	union
	{
		struct
		{
			INT m_FriendOnlineNum;
	        INT m_BlackOnlineNum;
	        INT m_EnemyOnlineNum;
	        INT m_FriendNum;
	        INT m_BlackNum;
	        INT m_EnemyNum;
		}m_OnlineNum;
		struct
		{
			DWORD m_playerID;
			BOOL m_bOnline;
			const char* m_playerName;
			BYTE m_nationality;
			BYTE m_sex;
			TIME_T m_joinTime;
			const char* m_note;
			BYTE m_friendLevel;
		} m_AcqParam;
		struct
		{
			DWORD m_playerID;
			BOOL m_bOnline;
			const char* m_playerName;
			BYTE m_nationality;
			BYTE m_sex;
			INT m_level;
			INT m_class;
			INT m_subclass;
 			BYTE m_friendLevel;
// 			BYTE m_rank;
		} m_FriendParam;
		struct
		{
			DWORD m_playerID;
			const char* m_playerName;
		} m_AskAddParam;
		struct
		{
			DWORD m_playerID;
			BOOL m_bOnline;
			const char* m_playerName;
			BYTE m_nationality;
			BYTE m_sex;
			TIME_T m_joinTime;
		} m_BlackListParam;
		struct
		{
			DWORD m_playerID;
			BOOL m_bOnline;
			const char* m_playerName;
			BYTE m_nationality;
			BYTE m_sex;
			TIME_T m_lastAttackTime;
		} m_EnemyParam;
		struct
		{
			DWORD	count;
			DWORD	maxCount;
		} m_countParam;

	};
};

class KBasicNumInterpreter
{
public:
	static const char* NationalityToString(DWORD id);
	static const char* SexToString(DWORD id);
};

struct KServerInfo 
{
	DWORD m_dwServerID;
	CHAR  m_strServerIP[16];
	DWORD m_dwServerPort;
};

struct KPlayerSocietyProperty
{
	INT64 dayHonor;
	INT64 dayHonorLimit;
	INT64 totalHonor;
	INT64 totalHonorLimit;
	int   maxContinueKillNum;
	int   evilValue;
	int   jailTimes;
	INT64 nationContribute;
	INT64 nationContributeLimit;
	BYTE  byEnergy;
	int   hourHonor;
	int   hourHonorLimit;
	int   hourKill;
	int   hourKillLimit;
	INT   nPlayerExploit;
	INT   nPlayerExploitByNextLevel;
	INT   nPlayerExploitLevel;
};

struct KGradualChangeScaleData
{
	INT x;
	INT y;
	INT z;
	INT xx;
	INT yy;
	INT zz;
	DWORD duration;
	KGradualChangeScaleData()
	{
		x = 0;
		y = 0;
		z = 0;
		xx = 0;
		yy = 0;
		zz = 0;
		duration = 1000;
	}
};

//struct KSceneInfoInWorld
//{
//	DWORD	m_dwSceneX;
//	DWORD	m_dwSceneY;
//};

//struct stMonsterFileNode 
//{
//	DWORD											m_dwMonsterFileID;
//	KSTRING											m_strMonsterFileFile;
//};
//
//struct KWorldInfoInGame
//{
//	DWORD											m_dwMapID;							// 地图 ID
//	KSTRING											m_strName;							// 地图名称
//	KSTRING											m_strWorldFile;						// 逻辑地图文件路径
//	KVector<stMonsterFileNode>						m_VectorByMonstrFile;
//
//#ifdef _SERVER
//	DWORD											m_dwWorldInfoID;
//	KVector<KSTRING>								m_VectorByTreasureCaseFile;
//	KVector<KSTRING>								m_VectorByTransmissionPosionFile;
//	KVector<KSceneInfoInWorld>						m_VectorByScene;
//	KVector<int>									m_VectorByTrafficLine;
//#endif
//
//#ifdef _CLIENT
//	DWORD											m_dwSceneWidth;
//	DWORD											m_dwSceneHeight;
//	KSTRING											m_strL2MapFile;						//寻路用的二级地图路径
//#endif
//};
//
//struct KWorldInfoForGame
//{
//	DWORD m_dwWorldInfoID;	// 指向 KWorldInfoInGame 对象
//	DWORD m_dwCount;		// 这个类型的 World 有多少个 ?
//	KVector<DWORD>									m_VectorByWorldID;
//};
//
//struct KWorldsInGameServer
//{
//	DWORD											m_dwGameServerID;
//	DWORD											m_dwLineCount;
//	KMapByVector<DWORD,KWorldInfoForGame>			m_Worlds;
//	KMapByVector<DWORD,KWorldInfoForGame>::iterator	WorldsInfoIterator;
//};


//战场相关
struct KBattleFieldInfo
{
	CHAR		m_strBFName[256];		//战场名称
	DWORD		m_dwMapID;				//战场对应地图ID
	DWORD		m_dwWorldID;			//战场对应WorldID
	DWORD		m_dwBattleFieldTypeID;	//战场类型
	BOOL		m_bOpen;				//战场是否开放
	DWORD		m_dwMaxPlayer;			//战场最大人数上限
	DWORD		m_dwCurrPlayer;			//当前战场人数
};
DECLARE_SIMPLE_TYPE(KBattleFieldInfo)

struct KModelSizeInfoTo3D
{
	FLOAT sizeX;
	FLOAT sizeY;
	FLOAT sizeZ;
};

struct KTalismanParam
{
	DWORD dwTalismanID;
	BOOL bTalismanSize;
};

struct KLifeSkillInfoToLogic
{
	enum ENUM_LSFI_TO_LOGIC
	{
		//配方处理相关
		enumLTL_QueryFormulaBoardUpdateFormulaTree,
		enumLTL_QueryFormulaBoardUpdateCurrentFormulaInfo,
		enumLTL_GetTotalItemCount,

		enumLTL_EnhanceFormula,
		enumLTL_ForgetFormula,

		//炼炉处理相关
		enumLTL_QueryHoldFurnace,
		enumLTL_QueryStartWorking,
		enumLTL_WorkCommand,

		//生活技能数据刷新
		enumLTL_QueryUpdateLifeSkillInfo,
		enumLTL_CanEnhanceFormulaCheck,
		enumLTL_QueryAssist,

		//精力活力转换
		enumLTL_EnergyTransferVigour,
		enumLTL_ApplyExpertis,
		enumLTL_RegisterLifeSkillSkill,

	} m_msgID;

	union
	{
		struct
		{
			DWORD m_dwFormulaID;
		} m_queryFormulaBoardUpdateCurrentFormulaInfo;

		struct
		{
			DWORD m_dwItemID;
			DWORD m_dwItemCount;
		} m_totalItemCount;

		struct
		{
			DWORD m_dwFormulaID;
			DWORD m_dwItemID[3];
			DWORD m_dwItemCount[3];
		} m_enhanceFormula;

		struct
		{
			DWORD m_dwFurnaceID;
		} m_queryHoldFurnace;

		struct
		{
			DWORD m_dwFormulaID;
		} m_queryStartWorking;
		struct
		{
			DWORD m_dwCommand;
		} m_workCommand;
		struct 
		{
			DWORD m_dwFormulaID;
		} m_forgetFormula;
		struct
		{
			DWORD m_dwFormulaID;
			BOOL m_result;
		} m_canEnhanceFormulaCheck;
		struct
		{
			INT m_dwBit;
		} m_EnergyTranVigourBit;
		struct  
		{
			INT m_expertiseNum;
			BOOL m_applyType;
		} m_ApplyExpertise;

	};
};

struct KLifeSkillInfoToUI
{
	enum ENUM_LSI_TO_UI
	{
		//配方处理相关
		enumLTU_FormulaBoardAddFormula,
		enumLTU_FormulaBoardClearFormula,
		enumLTU_FormulaBoardRemoveFormula,
		enumLTU_FormulaBoardUpdateCurrentFormulaInfo,
		enumLTU_FormulaBoardEnhanceError,

		enumLTU_OpenFormulaEnhanceWindow,
		enumLTU_CloseFormulaEnhanceWindow,
		enumLTU_OpenMakingWindow,
		enumLTU_CloseMakingWindow,
		enumLTU_OpenFurnaceWindow,
		enumLTU_CloseFurnaceWindow,

		enumLTU_WorkingOperationCoolDown,

		enumLTU_UpdateLifeSkillInfo,
		enumLTU_ChangeMakingProgressBar,

		enumLTU_AskAssistMaking,
		enumLTU_EnableViewFormula,
		enumLTU_DisableViewFormula,
		enumLTU_RegisterLifeSkillSkill,
		enumLTU_LetChooseExpertise,
	} m_msgID;

	union
	{
		struct
		{
			DWORD m_formulaID;
			DWORD m_formulaType;
			char m_name[MAX_NAME];
			const char* m_icon;
			INT m_quality;
			INT m_level;
			INT m_PCReq;
			int m_PLSReq;
		} m_formulaBoardAddFormulaInfo;
		struct
		{
			DWORD m_formulaID;
			DWORD m_formulaType;
		} m_formulaBoardRemoveFormulaInfo;
		struct
		{
			DWORD m_currentFormula;
			char m_name[MAX_NAME];
			DWORD m_itemID[3];
			DWORD m_itemCount[3];
			DWORD m_improveFormula;
			DWORD m_description;
			INT m_tempLow;
			INT m_tempHigh;
			INT m_energyCost;
			INT m_timeCost;
			INT m_levelReq;
			const char* m_icon;
			INT m_quality;
			INT m_PLReq;
			INT m_PCReq;


			INT m_currentEnergy;
		} m_currentFormulaInfo;
		struct
		{
			char m_errorMsg[1000];
		} m_formulaEnhanceError;
		struct
		{
			DWORD m_levels[MAX_LIFESKILL_TYPE];
			INT m_exps[MAX_LIFESKILL_TYPE];
			INT m_expsLimit[MAX_LIFESKILL_TYPE];
			BYTE m_levelBytes[MAX_LIFESKILL_LEVELBYTES];
			INT m_energy;
			INT m_maxEnergy;
			INT m_vigour;
			INT m_maxVigour;
			INT m_expertise;
		} m_lifeSkillInfo;
		struct
		{
			DWORD m_currentPos;
			DWORD m_totalSize;
		} m_changeMakingProgressBarInfo;
		struct
		{
			const char* m_name;
			DWORD m_dwType;
		} m_openMakingWindow;
		struct
		{
			BOOL m_filter[4];
		} m_openFurnaceWindow;
		struct
		{
			char m_askMessage[1000];
		} m_askAssistMaking;
		struct  
		{
			DWORD m_skillID;
			char m_SkillIcon[1024];
			char m_SkillName[1024];
			DWORD m_SkillLevel;
		} m_RegisterSkill;
		struct  
		{
			DWORD expertise;
			DWORD canChoose;
		} m_ChooseExpert;
	};
};

// 3D tooltip 结构
struct K3DItemTooltip
{
	K3DItemTooltip() {memset(this, 0, sizeof(K3DItemTooltip));}
	INT		nItemID;
	INT		nItemNameID;
	INT		nItemMaterialID;
	INT		nItemLevel;
	INT		nItemQuality;
	INT		nItemDisplayID;
};

struct K3DPlayerTooltip
{
	K3DPlayerTooltip() {memset(this, 0, sizeof(K3DPlayerTooltip));}
	DWORD	dwPlayerID;
	CHAR	szPlayerName[MAX_NAME];
	CHAR	szCyberCafeName[MAX_PATH];
	CHAR	szGuildName[MAX_NAME];
	BYTE	byLevel;
	BYTE	byClass;
	BYTE	bySubClass;
	BYTE	byNationality;
	CHAR	strMeta[MAX_NAME];
};

struct K3DNpcTooltip
{
	K3DNpcTooltip() {memset(this, 0, sizeof(K3DNpcTooltip));}
	int	nNpcID;		// npc实例ID
	int	nNpcTypeID;	// npc类型ID
	int nLevel;		// npc等级
	int nNameID;	// npc名称字符串id
	int nTitleID;	// npc称号字符串id
	int nRaceID;	// npc种族字符串id
	int nELevelID;	// npc精英等级字符串id
	int nOwnForceID;// npc势力字符串id
	int nDisplayID;	// npc显示信息
};

struct K3DTreasureCaseTooltip
{
	K3DTreasureCaseTooltip() {memset(this, 0, sizeof(K3DTreasureCaseTooltip));}
	DWORD	dwTreasureCaseID;
	DWORD	dwTreasureCaseTypeID;
	int		nNameID;
	int		nTypeID;
	int		nNeedLevel;
	int		nNeedItemID;
	int		nDisplayID;
};

enum ENUM_FIGHT_LOG_EVENT_TYPE
{
	enumFLT_Unknown = 0,
	enumFLT_OnEffect,
	enumFLT_OnPrepare,
	enumFLT_OnChannel,
	enumFLT_GainBuff,
	enumFLT_LostBuff,
	enumFLT_OnKill,
	enumFLT_OnLevelUp,
	enumFLT_EnterFight,
	enumFLT_LeaveFight,
	enumFLT_OnGainExp,
	enumFLT_OnGainHonor,
	enumFLT_OnGainGold,
	enumFLT_OnGainItem,
	enumFLT_OnGainRealmExp,
	enumFLT_OnGainRealmTip,
};

// 对FightLog来说Character的类型
enum ENUM_CHAR_TYPE_TO_FIGHT_LOG
{
	// 0.	未知
	enumCTTFL_Unknown = 0,
	// 1.	自己
	enumCTTFL_MainPlayer,
	// 2.	宠物
	enumCTTFL_MainMagicBaby,
	// 3.	队友
	enumCTTFL_TeamMate,
	// 4.	敌对NPC
	enumCTTFL_EnemyNPC,
	// 5.	敌对玩家
	enumCTTFL_EnemyPlayer,
	// 6.	其它
	enumCTTFL_Other,
};
struct KChangeSceneParam
{
	int		nX;
	int		nY;
	int		nZ;
	int		nBusLineID;
	int		dwCoupleID;
	int		dwCarrieeTypeID;
	DWORD	dwSummonMonsterID;
	int		nSummonMonsterLevel;
	int		nSummonMonsterHp;
	int		nSummonMonsterMp;
};

struct KTrafficTempStation
{
	int		nPlayer1;
	int		nPlayer2;
	int		nBusLine;
	DWORD	dwFirstTick;
	BYTE	byPassengerCount;
};

struct KCompressPacket
{
	WORD wCommand;
	WORD wLength;
	char szBuffer[4];
};

//法宝学习技能的一条信息
struct KMBLearnSkillInfo
{
	DWORD m_skillID;
	const char* m_icon;
	const char* m_name;
	const char* m_desc;
	const char* m_desc2;
	INT m_group;
	const char* m_groupName;
	const char* m_totalDescription;
	INT m_canLearn;
	INT m_wuxingCost[SOUL_TYPE_COUNT];
	INT m_levelNeeded;
// 	INT m_faithGiven;
	DWORD m_level;

	void Reset()
	{
		m_skillID = 0;
		m_icon = "";
		m_name = "";
		m_desc = "";
		m_desc2 = "";
		m_canLearn = 0;
		m_group = 0;
		m_groupName = "";
		for (INT i = 0; i < SOUL_TYPE_COUNT; i++)
		{
			m_wuxingCost[i] = 0;
		}
		m_levelNeeded = 0;
// 		m_faithGiven = 0;
		m_level = 0;
	}
};

struct KMBCmdToLogic
{
	enum ENUM_CMD_TO_LOGIC
	{
		enumCTL_Unknown,
		enumCTL_UseSkill,
		enumCTL_DragSkill,
		enumCTL_LearnSkill,

		enumCTL_GetLearnSkillInfo,
		enumCTL_GetWuxingInfo,
		enumCTL_GetCurrentName,
		enumCTL_GetCurrentAttr,
		enumCTL_GetCurrentAttrEx,

		enumCTL_GetName,
		enumCTL_GetAttr,
		enumCTL_GetAttrEx,

	} m_msgID;

	enum ENUM_SLOT_TYPE
	{
		enum_Unknown,
		enum_LearnBar,	//已学会的栏位
		enum_EquipBar,	//装备栏位
		enum_UniqueBar,	//专精技能栏位
	};

	struct
	{
		KMBLearnSkillInfo m_infos[MAX_MB_WUXING_SKILL_COUNT];
		::System::Collections::KVector<KMBLearnSkillInfo> m_info;
		INT m_MBPos;
	} m_learnSkillInfo;

	union
	{
		struct
		{
			INT m_nBlockID;
		} m_useSkillInfo;

		struct
		{
			ENUM_SLOT_TYPE m_from;
			ENUM_SLOT_TYPE m_to;
			INT m_fromPos;
			INT m_toPos;
		} m_dragSkillInfo;

		struct 
		{
			DWORD m_skillID;
			INT m_MBPos;
		} m_learnSkill;

		struct
		{
			INT m_wuxingElem[SOUL_TYPE_COUNT];
			INT m_MBPos;
		} m_wuxingInfo;

		struct
		{
			const CHAR *m_pName;
		} m_CurrentNameInfo;

		struct
		{
			INT m_nAttrKey;
			INT m_nAttrValue;
		} m_CurrentAttrInfo, m_CurrentAttrExInfo;

		struct
		{
			INT m_MBPos;
			const CHAR *m_pName;
		} m_NameInfo;

		struct
		{
			INT m_MBPos;
			INT m_nAttrKey;
			INT m_nAttrValue;
		} m_AttrInfo, m_AttrExInfo;
	};

	KMBCmdToLogic()
	{
		m_msgID = enumCTL_Unknown;
	}
};

struct KMBCmdFromServer
{
	enum ENUM_CMD_FROM_SERVER
	{
		enumCFS_Unknown,
		enumCFS_InitSkill,
		enumCFS_SetSkill,
	} m_msgID;

	DWORD m_mbID;

	union
	{
		struct
		{
			const void* m_buff;
			DWORD m_buffLen;
		} m_initSkillInfo;
		struct
		{
			INT m_pos;
			DWORD m_skillID;
		} m_setSkillInfo;
	};
	KMBCmdFromServer()
	{
		m_msgID = enumCFS_Unknown;
	}
};


struct KBagPositionInfo 
{
	BOOL Initialize(int pos, int bagtype, int itemID, int itemNum)
	{
		m_Pos = pos;
		m_Bagtype = bagtype;
		m_ItemID = itemID;
		m_ItemNum = itemNum;
		return TRUE;
	}
	int m_Pos;
	int m_Bagtype;
	int m_ItemID;
	int m_ItemNum;
};
DECLARE_SIMPLE_TYPE(KBagPositionInfo)


struct KAliveTimeItemInfo 
{
	INT128 m_ID;
	int m_AliveType;
	bool operator < (const KAliveTimeItemInfo& o) const;
};
DECLARE_SIMPLE_TYPE(KAliveTimeItemInfo)

// 商城道具
struct KBillingItemInfo 
{
	KBillingItemInfo()
	{
		memset(this, 0 ,sizeof(KBillingItemInfo));
	}

	int  nIndex;			// 序号
	BYTE bySchoolType;		// 大类型
	BYTE byClassType;		// 小类型
	WORD wItemID;			// 物品ID
	WORD wItemNum;			// 数量
	WORD wPrice;			// 价格
	BYTE byAgio;			// 折扣价格 90代表9折 85代表85折
	ENUM_MONEY_DEFINE nMoneyType;		// 销售货币单位

	enum ENUM_ITEM_FLAG
	{
		enum_Nothing	= 0,
		enum_Want_To_Become_Strong		= 0x0001,		// 我要变强大
		enum_Want_To_Become_Fashion		= 0x0002,		// 我要无限炫
		enum_Want_To_Become_MagicBaby	= 0x0004,		// 我要好法宝
		enum_Want_To_Become_LevelUp		= 0x0008,		// 我要快升级
	};
	BOOL EqualFlag(ENUM_ITEM_FLAG eFlag)
	{
		if (eFlag & nItemFlag)	return TRUE;
		return FALSE;
	}
	int nItemFlag;			// 物品的标志位，特殊意义
};
DECLARE_SIMPLE_TYPE(KBillingItemInfo)

struct KBillingFreeItem
{
	KBillingFreeItem()
	{
		memset(this, 0 , sizeof(KBillingFreeItem));
	}
	int  nIndex;			// 序号
	WORD wMaleItemID;		// 男性免费道具
	WORD wMaleItemNum;		// 男性免费道具数量
	WORD wFemaleItemID;		// 女性免费道具
	WORD wFemaleItemNum;	// 女性免费道具数量
};
DECLARE_SIMPLE_TYPE(KBillingFreeItem)

// 充值记录
struct KPrepaidRecordInfo 
{
	int nIndex;
	//DWORD dwAccountID;
	st_PassportKey passportKey;
	BYTE byCashType;
	int nCashValue;
};
DECLARE_SIMPLE_TYPE(KPrepaidRecordInfo)

// TEAM
struct KTeamItem
{
	void Initialize(DWORD dwPlayerID)
	{
		memset(this, 0 ,sizeof(KTeamItem));
		m_dwPlayerID = dwPlayerID;
		m_bOnline = TRUE;
	}
	DWORD m_dwMapID;
	DWORD m_dwDuplicateID;
	DWORD m_dwWorldLine;
	DWORD m_dwPlayerID;
	FLOAT m_fExitX;
	FLOAT m_fExitY;
	DWORD m_dwPopedom;	
	BOOL m_bOnline;
	BYTE m_byOffLineTime;
	INT m_fX , m_fY;		// 这里用INT就足够了
	char m_szName[MAX_NAME_DEF];
	BYTE m_byLevel;
	int m_nHP, m_nMP, m_nMaxHP, m_nMaxMP;
	BYTE m_bySex;
	BYTE m_FriendCount;
	BYTE m_Class;
	BYTE m_SubClass;
	FLOAT m_equipMark;
	DWORD m_dwVillageID;
};
DECLARE_SIMPLE_TYPE(KTeamItem)

struct KTeamRule
{
public:
	BOOL bFreeExp;		// 是否平均分配经验
	BOOL bFreeItem;		// 是否自由分配物品
	BOOL bcAdd;			// 是否允许加入
	BOOL bcUnit;		// 是否允许合并
	INT	 nDiceLevel;	// 筛子级别
	BOOL bOccupation;	// 职业优先
public:
	KTeamRule(BOOL fexp = 0, BOOL fitem = 1, BOOL cadd = 1, BOOL cunit = 1, BOOL occup = 1)
		: bFreeExp(fexp)
		, bFreeItem(fitem)
		, bcAdd(cadd)
		, bcUnit(cunit)
		, nDiceLevel(3)
		, bOccupation(occup)
	{
	}
	KTeamRule& operator = (const KTeamRule& v)
	{
		if(this == &v) return *this;
		memcpy(this, &v, sizeof(KTeamRule));
		return *this;
	}
	void Init()
	{
		bFreeExp = 1;
		bFreeItem = 0;
		bcAdd = 1;
		bcUnit = 1;
		nDiceLevel = 3;
		bOccupation = 1;
	}
};

struct KEquipmentAskInfo 
{
	BYTE byPos;
	BYTE byPosType;
	BYTE byDestPos;
};

struct KOpenQuestParm
{
	KOpenQuestParm() {memset(this, 0, sizeof(KOpenQuestParm));}
	DWORD m_dwQuestID;
	DWORD m_dwNpcTypeID;
	FLOAT x;
	FLOAT y;
};

struct KRepairTooltipPram
{
#define DEF_TOOLTIP_MAX_POS	(6)
	KRepairTooltipPram() {memset(this, 0, sizeof(KRepairTooltipPram));}
	struct KRepairTooltipBlock
	{
		KRepairTooltipBlock() {memset(this, 0, sizeof(KRepairTooltipBlock));}
		DWORD mdwItemID;
		DWORD mdwCurrentDura;
		DWORD mdwCurrentMaxDura;
	};
	KRepairTooltipBlock mItemTooltips[DEF_TOOLTIP_MAX_POS];
};

//--------------- 法宝孵化

struct KMagicBabyEgg
{
	DWORD dwEggID;			// ID
	DWORD dwOwnerID;		// 所有者ID
	TIME_T nIncubateTime;	// 开始孵化时间
	long  nType;			// 类型ID
	DWORD dwRemineTime;		// 剩余时间
	char  szOwnerName[MAX_NAME_DEF - 1];
	BYTE  byIsAdult;		// 是否孵化完成
	BYTE  byNation;			// 国家
	TIME_T freeTime;			// 释放时间
	TIME_T durTime;			// 孵化时间 
	TIME_T lostTime;		// 过期时间
};

struct KMagicBabyEggEx : public KMagicBabyEgg
{
	DWORD dwCurTime;
};

struct KUIMagicBabyEgg
{
	int nID;
	int nLevel;
	int nIndex;
	long time;
	char szIconName[64];
	char szStates[16];
	char szItemName[256];
	char  szOwnerName[MAX_NAME_DEF - 1];
};

struct KUIInitEggsList
{
	int m_nCurPage;
	int m_nMaxPage;
};

struct KStaticTabSreachParam
{
	KStaticTabSreachParam() {Init();}
	VOID Init()
	{
		m_pTabFileName = NULL;
		m_pSreachKeyWord = NULL;
		m_pSreachKeyWordValue = NULL;
		m_vecTargetWords.clear();
		m_vecTargetValues.clear();
	}
	char * m_pTabFileName;
	char * m_pSreachKeyWord;
	char * m_pSreachKeyWordValue;

	typedef ::System::Collections::KVector<char *> KStaticTabSreachParamWords;
	KStaticTabSreachParamWords m_vecTargetWords;

	typedef ::System::Collections::KVector< ::System::Collections::KString<MAX_PATH> > KStaticTabSreachParamValues;
	KStaticTabSreachParamValues m_vecTargetValues;
};

int randbignum(int num);
int randsmallnum(int num);
int randrange(int beg, int end);

#define	MAX_TRAFFIC_ROUTE_NAME		16
// 一条路线上最多能有1024个节点，该数字仅在编辑器和逻辑交互时受限，可随时调整数值，扩大数值仅对传递的临时变量有影响，
// 服务器存储结构为KVector，不受这个影响。
#define	MAX_TRAFFIC_WAYPOINT_COUNT	1024

struct KTransportInfo
{
	//DWORD	dwDriverID;			// 司机ID，如果=TRANSPORT_INVALID_DRIVER, 则表示是无人驾驶的车
	INT		nTypeID;
	INT		nRouteIndex;		// 路线ID，无人jia驶时使用
	INT		nMaxPassenger;
	DWORD	dwPassenger[TRANSPORT_MAX_PASSENGER];	//乘客ID

};

struct KTrafficWaypoint
{
	float fX;
	float fY;
	float fZ;
	float fSpeed;
	float fAngle[4];
	float fTime;	// 从起始点到该点的偏移时间
	WORD  wEventID;
	WORD  wStationID;
	BYTE  byFlyModelAndStation;	// 飞行模式标识位 | 车站站点标识位
	BYTE  byCanGetOn : 1;			// 是否可以在这段上车
	BYTE  byCanGetOff : 1;			// 是否可以在这段上车
	BYTE  byResv : 6;			// 是否可以在这段上车
	BYTE  byReserve[2];			// 保留位
};

struct KTrafficWaypointNew
{
	wPoint mPoint;
	float fSpeed;
	const char * szTag;
};

struct KRouteBaseInfo
{
	char szName[MAX_TRAFFIC_ROUTE_NAME];	// 路线名称
	WORD nBusTypeID;	// 交通工具类型
	BYTE byIsWalk;		// 是否是行走路线
	BYTE byIsStraightaway; // 是否走直线线路
	WORD nSeatCount;	// 座位数量
	BYTE byCycleLine;	// 是否是循环线路
	BYTE byMaxCount;	// 发车的最大数量
	WORD wInterval;		// 发车间隔,如果是环线，则可以根据线路长度和最大车数来计算
	WORD wTotalInterval;// 总时间间隔：每隔wTotalInterval将发出byMaxCount辆车，这些车不是一瞬间发出去的，而是两两间隔wInterval发出去
	int	nWaypointCount;	// 节点数量
	float fTotalTime;	// 走完这条路线的总时间
};

struct KTrafficRouteInfo
{
	KRouteBaseInfo		Info;		// 路线的基本信息
	KTrafficWaypoint	Waypoints[MAX_TRAFFIC_WAYPOINT_COUNT];	// 节点数组
};

struct KTrafficRouteOperator
{
	int nRouteIndex;
	char szName[MAX_TRAFFIC_ROUTE_NAME];
	KRouteBaseInfo		Info;		// 路线的基本信息
};


struct KTrafficWaypointOperator
{
	int nRouteIndex;
	int nWaypointIndex;
	KTrafficWaypoint wp;
};

struct KTransportInitInfo
{
	float fX;
	float fY;
	float fZ;
	float fSpeed;
	float fAngle;		// 左右倾斜角度
};


struct KTrafficDeparture
{
	WORD	nCount;					// 已发的车数
	DWORD	dwLastTime;				// 最后一次发车时间
	DWORD	dwFirstTime;			// 发第一辆车的时间
};

typedef System::Collections::KVector<KTrafficWaypoint> KWaypointsVector;
typedef System::Collections::KVector<int> KTransportVector;

struct KTrafficRoute
{
	KRouteBaseInfo		Info;		// 路线基本信息
	KWaypointsVector	Waypoints;	// 节点数组
};



struct KTimerLogData
{
	KTimerLogData()
	{
		Reset();
	}
	void Reset()
	{
		memset(this, 0 ,sizeof(KTimerLogData));
	}
	//////////////////////////////////////////////////////////////////////////
	//		登陆系统
	//////////////////////////////////////////////////////////////////////////
	DWORD	m_LoginCount;			// 登陆数量
	DWORD	m_LogoutCount;			// 登出数量
	DWORD	m_DisconnectCount;		// 断线数量
	DWORD	m_ErrorAccountCount;	// 帐号错误次数
	DWORD	m_ErrorPasswordCount;	// 密码错误次数

	//////////////////////////////////////////////////////////////////////////
	//		角色创建
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		战斗系统
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		经济系统
	//////////////////////////////////////////////////////////////////////////
	INT64	m_AddMoney;				// 游戏币增加
	INT64	m_CostMoney;			// 花费游戏币

	//////////////////////////////////////////////////////////////////////////
	//		交通工具
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		互动系统
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		副本系统
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		物品系统
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		法宝系统
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		任务系统
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		邮件系统
	//////////////////////////////////////////////////////////////////////////
	INT		Mail_SendOK;		// 邮件发送成功次数
	INT		Mail_SendFail;		// 邮件发送失败次数
	INT		Mail_FetchOK;		// 邮件提取成功次数
	INT		Mail_FetchFail;		// 邮件提取失败次数
	INT		Mail_SendGuild;		// 邮件发送帮派邮件
};

struct KWuXing
{
	int nGold;
	int nWood;
	int nWater;
	int nFire;
	int nEarth;
	int nSign;
};

struct KMagicBabyTalentInfo
{
	CHAR MBTalentIcon[PET_ICON_MAX];
	CHAR MBTalentTooltip[MB_TALENTDES_MAX];
	CHAR MBTalentIcon1[PET_ICON_MAX];
	CHAR MBTalentTooltip1[MB_TALENTDES_MAX];
	CHAR MBTalentIcon2[PET_ICON_MAX];
	CHAR MBTalentTooltip2[MB_TALENTDES_MAX];
	CHAR MBTalentIcon3[PET_ICON_MAX];
	CHAR MBTalentTooltip3[MB_TALENTDES_MAX];
	CHAR MBTalentIcon4[PET_ICON_MAX];
	CHAR MBTalentTooltip4[MB_TALENTDES_MAX];
	CHAR MBTalentIcon5[PET_ICON_MAX];
	CHAR MBTalentTooltip5[MB_TALENTDES_MAX];
	bool MBTalentActive;
	bool MBTalentID;
};


struct KGetItemPositionByIDData 
{
	BYTE byPosType;
	WORD wPos;
};

///////////////////////////////////邮件相关/////////////////////////////////
const int 	MAX_QUERY_MAIL_COUNT	=	10;
const int	MAIL_HEAD_LEN			=	20;
const int	MAIL_BODY_LEN			=	1000;
const int	MAIL_ATTACHMENT_LEN		=	256;
const int	MAX_BUFF_MAIL_HEAD		=	MAIL_HEAD_LEN + 1;
const int	MAX_BUFF_MAIL_BODY		=	MAIL_BODY_LEN + 1;
const int	MAX_AFFICHE_MAIL_COUNT	=	50;
const int	MAX_GUILD_MEMBER_COUNT	=	210;
const int	MAIL_PLAYER_NAME		=	20;
#pragma pack(push,1)

struct Mail_Attach_ItemID
{
	DWORD	ItemID;
	int		ItemCount;
};

struct Mail_Packet
{
	enum		{FIX_PART_LEN = 90,};					//MP_BUFF之前的长度
	INT64		MP_MailID;								//邮件ID				BIGINT(8)
	DWORD		MP_SenderID;							//发件人ID				INT (4)
	char		MP_SenderName[MAIL_PLAYER_NAME + 1];	//发件人名字			CHAR(20)
	DWORD		MP_ReceiverID;							//收件人ID				INT(4)
	char		MP_ReceiverName[MAIL_PLAYER_NAME + 1];	//收件人名字			CHAR(20)
	INT64		MP_ReceiveTime;							//时间日期				TIMESTAMP
	BYTE		MP_IsRead;								//邮件读取状态(已读/未读)	TINYINT(1)
	BYTE		MP_IsFetch;								//已操作				TINYINT(1)
	BYTE		MP_MailType;							//邮件类型(普通/帮派/物品/系统)	TINYINT(1)
	BYTE		MP_AttachItemType;						//物品类型(ItemBase/ItemID)		TINYINT(1)
	DWORD		MP_FeeAmount;							//付费数量				INT(4)
	DWORD		MP_AttachMoney;							//附加金钱数量			INT(4)
	int			MP_HeadLen;								//邮件主题				VARCHAR(20)
	int			MP_BodyLen;								//邮件信息				TEXT
	int			MP_AttachmentLen;						//邮件附加物品		BIGINT(8)
	//实际Buffer大小根据三块数据的实际长度得出。
	//|Head| + '\0' + |Body| + '\0' + |Attachment|
	//在放置数据时，也必须保证按照Head, Body, Attachment的顺序依次放入
	char		MP_BUFF[MAX_BUFF_MAIL_HEAD + MAX_BUFF_MAIL_BODY + MAIL_ATTACHMENT_LEN];
	static int GetUsefulLen(const Mail_Packet& packet)
	{
		int nSize = sizeof(Mail_Packet);
		return (FIX_PART_LEN + packet.MP_HeadLen + packet.MP_BodyLen + packet.MP_AttachmentLen);
	}
	//获取地址
	static char * GetMailHeadAddr(const Mail_Packet& packet)
	{
		return (char *)(&packet.MP_BUFF[0]);
	}
	static char * GetMailBodyAddr(const Mail_Packet& packet)
	{
		return (char *)(&packet.MP_BUFF[packet.MP_HeadLen]);
	}
	static char * GetMailAttachementAddr(const Mail_Packet& packet)
	{
		return (char *)(&packet.MP_BUFF[packet.MP_HeadLen + packet.MP_BodyLen]);
	}
	//存入数据
	static bool SetMailHead(Mail_Packet& packet, const char * pszHead)
	{
		packet.MP_HeadLen = strlen(pszHead) + 1;
		if (packet.MP_HeadLen > (MAIL_HEAD_LEN + 1))
		{
			packet.MP_HeadLen = 0;
			return false;
		}
		strcpy_k(GetMailHeadAddr(packet), sizeof(packet.MP_BUFF), pszHead);
		return true;
	}
	static bool SetMailBody(Mail_Packet& packet, const char * pszBody)
	{
		packet.MP_BodyLen = strlen(pszBody) + 1;
		if (packet.MP_BodyLen > (MAIL_BODY_LEN + 1))
		{
			packet.MP_BodyLen = 0;
			return false;
		}
		strcpy_k(GetMailBodyAddr(packet), sizeof(packet.MP_BUFF) - packet.MP_HeadLen, pszBody);
		return true;
	}
	static bool SetMailAttachement(Mail_Packet& packet, const char * pAttachment, int nAttachmentLen)
	{
		packet.MP_AttachmentLen = nAttachmentLen;
		if (packet.MP_AttachmentLen > MAIL_ATTACHMENT_LEN)
		{
			packet.MP_BodyLen = 0;
			return false;
		}
		memcpy(GetMailBodyAddr(packet), pAttachment, nAttachmentLen);
		return true;
	}
	//检查数据是否可能遭到破坏
	bool static CheckIntact(const Mail_Packet& packet)
	{
		//检查长度是否越界，并且mail和body的字符串是否以0结尾
		return	(	packet.MP_HeadLen <= (MAIL_HEAD_LEN + 1) &&
			packet.MP_BodyLen <= (MAIL_BODY_LEN + 1) && 
			packet.MP_AttachmentLen <= MAIL_ATTACHMENT_LEN &&
			packet.MP_BUFF[packet.MP_HeadLen - 1] == '\0' &&
			packet.MP_BUFF[packet.MP_HeadLen + packet.MP_BodyLen - 1] == '\0'
			);
	}

};

struct Mail_Info_Rsp
{
	INT64 mailID;
	char senderName[MAIL_PLAYER_NAME + 1];
	INT64 receiveTime;
	BYTE isRead;
	BYTE mailType;
	char mailHead[MAIL_HEAD_LEN + 1];
};

struct Mail_Attach
{
	INT64 mailID;
	int attachItemType;							//附加物品类型(0:KItemBase/1:Mail_Attach_ItemID)
	DWORD feeAmount;							//付费数量	
	DWORD attachMoney;							//附加金钱数量
	int attachmentLen;							//附加物品长度
	char attachment[MAIL_ATTACHMENT_LEN];		//附加物品
};

struct Affiche_Condition_Rsp
{
	INT64 mailID;
	int conditionID;	
};
///////////////////////////Public struct/////////////////////
struct Mail_Query_Req
{
	DWORD	playerID;
	INT64	mailID;
};

struct Mail_Receive_Req
{
	DWORD	playerID;
	INT64	mailID;
};

struct Mail_Send_Req
{
	DWORD	playerID;
	int		hasAttachment;
	Mail_Packet mailPacket;
	int static GetUsefulLen(const Mail_Send_Req& req)
	{
		return (sizeof(req.playerID) +
			sizeof(req.hasAttachment) +
			Mail_Packet::GetUsefulLen(req.mailPacket));
	}
};

struct Mail_Delete_Req
{
	DWORD	playerID;
	INT64	mailID;
	int		deleteOption;
};

struct Mail_Set_Readed_Req
{
	DWORD	playerID;
	INT64	mailID;
};

struct Mail_Fetch_Attachment_Req
{
	DWORD	playerID;
	INT64	mailID;
	DWORD	feeAmount;
};

struct Mail_ReturnToSender_Req
{
	DWORD	playerID;
	INT64	mailID;
};

struct Mail_Query_Rsp
{
	//enum { 
	//	error_Succeed = 0,
	//	error_Net_Error,		// 网络出错
	//	error_Db_Error,			// 数据库出错
	//	error_Sync_In_Part,		// 后续还有数据
	//	error_Sync_Accomplish,	// 已经同步数据
	//};
	BYTE	result;
	DWORD	playerID;
	int		mailsCount;
	Mail_Info_Rsp mailInfo[MAX_QUERY_MAIL_COUNT];
	int static GetUsefulLen(const Mail_Query_Rsp& rsp)
	{
		return (sizeof(rsp.result) +
			sizeof(rsp.playerID) +
			sizeof(rsp.mailsCount) +
			sizeof(Mail_Info_Rsp) * rsp.mailsCount);
	}

};

struct Mail_Receive_Rsp
{
	//enum { 
	//	error_Succeed = 0,
	//	error_Net_Error,		// 网络出错
	//	error_Db_Error,			// 数据库出错
	//};
	BYTE	result;
	DWORD	playerID;
	Mail_Packet mailPacket;
	int static GetUsefulLen(const Mail_Receive_Rsp& rsp)
	{
		return (sizeof(rsp.result) +
			sizeof(rsp.playerID) +
			Mail_Packet::GetUsefulLen(rsp.mailPacket));
	}
};

struct Mail_Send_Rsp
{
	//enum { 
	//	error_Succeed = 0,
	//	error_Net_Error,		// 网络出错
	//	error_Db_Error,			// 数据库出错
	//	error_Receiver_No_Space,// 收件人信箱已满
	//	error_No_Receiver,		// 收件人不存在
	//	error_No_Enough_Money,	// 无法扣除足够的金钱
	//	error_Attachment_Error,	// 附件物品操作出错
	//};
	BYTE	result;
	DWORD	playerID;
	char	receiverName[MAIL_PLAYER_NAME];

};

struct Mail_Delete_Rsp
{
	//enum { 
	//	error_Succeed = 0,
	//	error_Net_Error,		// 网络出错
	//	error_Db_Error,			// 数据库出错
	//	error_Fee_Nonzero,		// 需要提取费用，不能直接删除
	//};
	BYTE	result;
	DWORD	playerID;
	INT64	mailID;
	int		deleteOption;
};

struct Mail_Set_Readed_Rsp
{
	DWORD	playerID;
	INT64	mailID;
};

struct Mail_Fetch_Attachment_Rsp
{
	//enum { 
	//	error_Succeed = 0,
	//	error_Net_Error,		// 网络出错
	//	error_Db_Error,			// 数据库出错
	//	error_Bag_No_Space,		// 背包已满，无空间
	//	error_No_Enough_Money,	// 无法扣除足够的金钱
	//};
	BYTE	result;
	DWORD	playerID;
	INT64	mailID;
};

struct Mail_ReturnToSender_Rsp
{
	//enum { 
	//	error_Succeed = 0,
	//	error_Net_Error,		// 网络出错
	//	error_Db_Error,			// 数据库出错
	//	error_Type_Unmatch,		// 此类邮件不支持退信
	//};
	BYTE	result;
	DWORD	playerID;
	INT64	mailID;
};

struct KConvokeCahce
{
	KConvokeCahce() {Clear();}
	VOID Clear() {memset(this, 0, sizeof(KConvokeCahce));}
	VOID StartConvoke(DWORD adwX, DWORD adwY, DWORD adwMapID, DWORD adwWorldID, INT anRemainTime)
	{
		m_dwPosX = adwX;
		m_dwPosY = adwY;
		m_dwMapID = adwMapID;
		m_dwWorldID = adwWorldID;
		m_nStartTime = static_cast<TIME_T>( time(0) );
		m_nRemainTime = anRemainTime;
	}
	DWORD m_dwPosX;
	DWORD m_dwPosY;
	DWORD m_dwMapID;
	DWORD m_dwWorldID;
	TIME_T m_nStartTime;
	INT m_nRemainTime;
};

struct KPracticeCreateInfo
{
	KPracticeCreateInfo() {Init();}
	VOID Init() {memset(this, 0, sizeof(KPracticeCreateInfo));}
	BYTE m_byLevel;
	DWORD m_dwMaxExp;
	DWORD m_dwClassNameID;
	DWORD m_dwSchoolNameID;
	DWORD m_dwTipID1;
	DWORD m_dwTipID2;
	DWORD m_dwTipID3;
	DWORD m_dwClassToolTips;
	DWORD m_dwSchoolToolTips;
	DWORD m_dwCurrentToolTips;
	DWORD m_dwPraAbility;
	DWORD m_dwPraSkillXS;
	DWORD m_dwPraSkillFS;
	DWORD m_dwPraSkillDS;
	DWORD m_dwPraSkillWS;

	DWORD m_dwToolTipA;
	DWORD m_dwToolTipB;
	DWORD m_dwToolTipC;
	DWORD m_dwToolTipD;
	DWORD m_dwToolTipE;
	DWORD m_dwToolTipF;
	DWORD m_dwToolTipG;
	DWORD m_dwToolTipH;
};

#pragma pack(pop)

struct KTalentCmdFromUI
{
	enum
	{
		//获得值
		enumTCFUI_GetVal = 0,
		//获得状态，返回值见TALENT_POINT_STATE
		enumTCFUI_GetStatus,
		//获得天赋总点数
		enumTCFUI_GetTotalPoint,
		//获得天赋已用点数
		enumTCFUI_GetUsedPoint,
		//获得天赋剩余点数
		enumTCFUI_GetRemainPoint,
		//获得天赋图标串
		enumTCFUI_GetIcon,

		//学习某天赋
		enumTCFUI_Learn,
		//洗点
		enumTCFUI_Clear,

		//得到TalentData
		enumTCFUI_GetTalentData,
		//获得全部状态，返回值为2^TALENT_POINT_STATE的组合
		enumTCFUI_GetAllStatus,
		//询问洗点
		enumTCFUI_AskClear,
	};
	union
	{
		struct
		{
			INT m_nClass;
			DWORD m_dwKey;
		} m_getTalentData;
	};
};

struct KSkillGrasp
{
	int m_skillGraspPointCount;							//当前剩余领悟点数
	int m_graspList[MB_SKILLGROUP_MAX];					//领悟信息列表
};

struct KMBTalentInfo
{
	int m_talentGrid;
	int m_talent[MB_TALENT_MAX];
};

struct KMBSkillFightValue
{
	int m_skillID;
	int m_fightValue;
};

struct KTalentEventToUI
{
	enum
	{
		//天赋点初始化完成
		enumTETUI_Init = 0,
		//某天赋点学习成功，参数为学习成功的天赋点
		enumTETUI_Learn,
		//天赋点总数变化了
		enumTETUI_TotalPointChange,
		//天赋点清洗了
		enumTETUI_Clear,
		//转职
		enumTETUI_ChangeClass,
		//二转
		enumTETUI_ChangeSubClass,
		//强制刷新
		enumTETUI_Refresh,
		//询问确认天赋点是否要清除，参数为需要花费的钱
		enumTETUI_AskClear,
	};
};

struct PACKET_FILE
{
	enum		{PF_PATH_RELATIVE = 0, PF_PATH_POSITIVE};		//MP_BUFF之前的长度
	int			PF_FileNameLen;									//文件名字长度
	int			PF_FileStreamLen;								//文件数据流长度
	//实际Buffer大小根据两块数据的实际长度得出。
	//|FileName| + |FileStream|
	BYTE		PF_BUFF[1024 * 9];
	static int GetUsefulLen(const PACKET_FILE& packet)
	{
		return (sizeof(packet.PF_FileNameLen) + sizeof(packet.PF_FileStreamLen) +
				packet.PF_FileNameLen + packet.PF_FileStreamLen);
	}

	static BYTE * GetStreamAddr(const PACKET_FILE& packet)
	{
		return (BYTE *)(&packet.PF_BUFF[packet.PF_FileNameLen]);
	}

	//存入数据
	static bool SetData(PACKET_FILE& packet, const char * pFileName, const BYTE * pData, int nDataLen)
	{
		packet.PF_FileNameLen = strlen(pFileName) + 1;
		packet.PF_FileStreamLen = nDataLen;
		if (packet.PF_FileNameLen + packet.PF_FileStreamLen > 1024 * 9)
			return false;

		strcpy_k((char *)(&packet.PF_BUFF[0]), sizeof(packet.PF_BUFF), pFileName);
		memcpy(&packet.PF_BUFF[packet.PF_FileNameLen], pData, nDataLen);
		return true;
	}

	//检查数据是否可能遭到破坏
	bool static CheckIntact(const PACKET_FILE& packet, int nLen)
	{
		//检查长度是否符合
		return	( GetUsefulLen(packet) == nLen );
	}

};

struct KAttrChangeEventParam
{
	enum
	{
		enumACEP_PlayerTalent = 1,	//玩家天赋
		enumACEP_Vigour,			//精力
		enumACEP_Energy,			//活力
		enumACEP_Effort,			//气力
		enumACEP_HPNotFull,			//hp从满变到不满
		enumACEP_HPLess20,			//hp变到低于20%
		enumACEP_HPLess50,			//hp变到低于50%
		enumACEP_ExerciseMore10,	//修炼值变道高于10%
		enumACEP_EquipOneStar,		//装备等级一颗星，oldValue和currentValue一样，都是当前值
		enumACEP_EquipTwoStars,		//装备等级两颗星，oldValue和currentValue一样，都是当前值
		enumACEP_EquipThreeStars,	//装备等级三颗星，oldValue和currentValue一样，都是当前值
	} m_valueType;
	INT m_nOldValue;
	INT m_nCurrentValue;
};

struct KConvokeSrcInfo
{
	KConvokeSrcInfo() {Init();}
	VOID Init() {memset(this, 0, sizeof(KConvokeSrcInfo));}
	DWORD mdwSrcID;
	DWORD mdwPosX;
	DWORD mdwPosY;
	DWORD mdwMapID;
	DWORD mdwWorldID;
	DWORD mdwSceneID;
	DWORD mdwRegionID;
	DWORD mdwTeamID;
	DWORD mdwGuildID;
	DWORD mdwTangID;
	DWORD mdwNatID;
	BYTE mbyConvokeID;
};

/// Wishing Tree
#pragma pack(push,1)
struct KWish
{
	DWORD	dwWishID;
	TIME_T	tBegin;
	TIME_T	tEnd;
	TIME_T	tSend;
	int		nSupport;
	int		nUnSupport;
	DWORD	dwPlayerID;
	BYTE	byWishType;
	CHAR	achPlayerName[22];
	CHAR	achContent[42];

};

struct WT_ShowTree_Req
{
	DWORD	dwPlayerID;
	int		nNationID;
};

struct WT_ShowTree_Rsp
{
	DWORD	dwPlayerID;
	BYTE	byResult;
	DWORD	adwTasks[5];
	BYTE	abyTaskResults[5];
	int		nNormalCount;
	int		nHighCount;
	KWish	aWish[60];
	int static GetUsefulLen(const WT_ShowTree_Rsp& rsp)
	{
		return (sizeof(DWORD) +
			sizeof(BYTE) +
			sizeof(DWORD) * 5 +
			sizeof(BYTE) * 5 +
			sizeof(int) * 2 +
			sizeof(KWish) * (rsp.nNormalCount + rsp.nHighCount));
	}
};

struct WT_AddWish_Req
{
	DWORD	dwPlayerID;
	int		nNationID;
	KWish	wish;
};


struct WT_AddWish_Rsp
{
	DWORD	dwPlayerID;
	BYTE	byResult;
	KWish	wish;
};

struct WT_Search_Req
{
	DWORD	dwPlayerID;
	int		nNationID;
	BYTE	byDirection;
	int		nRequireCount;
	DWORD	dwBoudaryWishID;
};

struct WT_Search_Rsp
{
	DWORD	dwPlayerID;
	BYTE	byResult;
	BYTE	byDirection;
	int		nCount;
	KWish	aWish[60];
	int static GetUsefulLen(const WT_Search_Rsp& rsp)
	{
		return (sizeof(DWORD) +
			sizeof(BYTE) +
			sizeof(BYTE) +
			sizeof(int) +
			sizeof(KWish) * rsp.nCount);
	}
};

struct WT_Support_Req
{
	DWORD	dwPlayerID;
	int		nNationID;
	BYTE	bySupport;
	KWish	wish;
};

struct WT_Support_Rsp
{
	DWORD	dwPlayerID;
	BYTE	byResult;
	BYTE	bySupport;
	KWish	wish;
};

// 许愿树操作
struct KWTOperatorParam
{
	KWTOperatorParam()
	{
		Init();
	}

	VOID Init()
	{
		dwWishID = 0;
		memset(&wish, 0, sizeof(wish));
	}

	BYTE			bySubCmd;
	BYTE			byResult;		// 1: success 2 :failed
	BYTE			bySupport;		// 0: support 1 :unsupport
	BYTE			byQuestResult;	// 任务完成结果
	DWORD			dwQuestID;		// 任务ID
	BYTE			byGetWishType;	// 获取愿望类型 0:Tree, 1:Research
	BYTE			byTreeIndex;	// 许愿树的索引位置0-58
	DWORD			dwWishID;		// 愿望ID
	KWish			wish;
};

#pragma pack(pop)

struct K3DObjectCreateParam
{
	BOOL bMorph;
	DWORD dwMorphID;
	DWORD dwOriginalID;
};

// 战斗统计信息输出
struct KFightStatisticsLogInfo
{
	enum
	{
		enumFSLI_Damage,	//伤害输出，对应m_damage
		enumFSLI_Heal,		//治疗输出，对应m_heal
		enumFSLI_FightStateChange,	//战斗状态转变
	} m_logType;
	union{
		struct{
			const char* pSrcName;			//源的名字
			const char* pSrcMasterName;		//源的主控玩家的名字（如法宝的主人，召唤兽的主人等，下同）
			const char* pDestName;			//目标的名字
			const char* pDestMasterName;	//目标的主控玩家的名字
			const char* pSkillName;			//技能名
			int nVal;						//治疗/伤害值
		}m_damage, m_heal;
		struct{
			BOOL enterFight;				//为TRUE为进入战斗，离开为FALSE
			const char* pName;				//进入战斗的人的名字
		}m_fightState;
	};
};

//////////////////////////////////////////////////////////////////////////

struct KPasswordProtect 
{
	KPasswordProtect()
	{
		Clear();
	}
	void Clear()
	{
		memset(this, 0 , sizeof(KPasswordProtect));
	}

	enum
	{
		max_x = 10,			// 行数 对应字母ABCDEFGH
		max_y = 8,			// 列数 对应数字12345678910
	};
	BYTE m_Key[max_y][max_x];
};


struct KProtectAskInfo
{
	static const int m_ProtectResultCount = 3;

	BOOL CheckPos(int pos)
	{
		if (pos < 0 || pos >= m_ProtectResultCount)
			return FALSE;
		return TRUE;
	}

	BOOL SetValue_X(int pos, int value)
	{
		if (!CheckPos(pos)) return FALSE;
		m_X[pos] = value;
		return TRUE;
	}

	BOOL SetValue_Y(int pos, int value)
	{
		if (!CheckPos(pos)) return FALSE;
		m_Y[pos] = value;
		return TRUE;
	}

	int GetValue_X(int pos)
	{
		if (!CheckPos(pos)) return FALSE;
		return m_X[pos];
	}

	int GetValue_Y(int pos)
	{
		if (!CheckPos(pos)) return FALSE;
		return m_Y[pos];
	}

	BYTE	m_X[m_ProtectResultCount];
	BYTE	m_Y[m_ProtectResultCount];
};

struct KMovePtDesc
{
	FLOAT x;
	FLOAT y;
	INT mapID;
	KMovePtDesc()
		: x(0.0f)
		, y(0.0f)
		, mapID(0){}
};


struct KVillageInfo
{
	DWORD dwID;
	char name[64];
};

// 元婴修炼
struct KExerciseInfo
{
	int nStatus;
	int nTotalExp;
	int nTrainedExp;
	int nUsableExp;
	int nCurProgress;
	int nMaxProgress;
	int nMonsterCount;
	DWORD nMonsterID[64];
	int nMonsterLevel[64];
	char nMonsterName[64][20];
};

struct KEnrollParty
{
	int remainSec;
	char partyName[64];
	int activityDesID;
};


// 休闲挂机的阵型信息
struct FormationCreateInfo : public System::Memory::KPortableStepPool<FormationCreateInfo,32>
{
public:
	FormationCreateInfo() {Init();}
	VOID Init()
	{
		mdwFormationID = 0;
		mdwMinMember = 0;
		mdwMaxMember = 0;
		mdwMutuRange = 0;
		mdwFormationRange = 0;
		mdwFormationDelay = 0;
		mdwFormationTotalTime = 0;
		mfNormalExpRate = 0.0f;
		mfPerfectExpRate = 0.0f;
		mfPerfect1ExpRate = 0.0f;
		mfReductionExpRate = 0.0f;
		mdwFormationTC = 0;
		mdwFormationExpTime = 0;
		mPerfectConditionIDs.clear();

		memset(mszFormationName, 0, sizeof(mszFormationName));
		memset(mszFormationImage, 0, sizeof(mszFormationImage));

#ifdef _CLIENT
		mdwTooltip1 = 0;
		mdwTooltip2 = 0;
#endif // _CLIENT
	}

	DWORD mdwFormationID;		// 阵型ID
	DWORD mdwMinMember;			// 最少需要人数
	DWORD mdwMaxMember;			// 最多支持人数
	DWORD mdwMutuRange;			// 互斥范围 同时是人数检查范围
	DWORD mdwFormationRange;	// 阵型范围
	DWORD mdwFormationDelay;	// 阵型启动延迟
	DWORD mdwFormationTotalTime;// 阵型总时间
	FLOAT mfNormalExpRate;		// 普通经验值获得速率
	FLOAT mfPerfectExpRate;		// 完美阵型时经验值获得速率
	FLOAT mfPerfect1ExpRate;	// 准完美阵型时经验值获得速率
	FLOAT mfReductionExpRate;	// 每少一个人的经验值递减速率
	DWORD mdwFormationTC;		// 阵型特效宝箱ID
	DWORD mdwFormationExpTime;	// 普通状态下阵型加满经验值需要的时间0为不加经验
	::System::Collections::DynArray<DWORD> mPerfectConditionIDs;	// 完美阵型的条件

	CHAR mszFormationName[MAX_NAME_DEF];
	CHAR mszFormationImage[MAX_PATH];

#ifdef _CLIENT
	DWORD mdwTooltip1;
	DWORD mdwTooltip2;
#endif // _CLIENT
};

struct KRecommendationInfo
{
	enum ENUM_SERVER_TYPE
	{
		enumNormal = 0,
		enumRecommendation,
		enumNew,
	};

	KRecommendationInfo() {Init();}
	VOID Init() {mbyRecommendationState = enumNormal;mAwardBuffID = 0;}
	BYTE mbyRecommendationState;	// ENUM_SERVER_TYPE
	DWORD mAwardBuffID;				// 奖励buff
};

const int _WORLD_VALUE_NOTE1_LEN = 24;
const int _WORLD_VALUE_NOTE2_LEN = 48;
//////////////////////////////////////////////////////////////////////////
#pragma pack(push,1)

struct KGuildBFSummary
{
	BYTE					byState;
	BYTE					byRound;
	BYTE					byTimeWeek;
	KLocalTime				timeStart;
	KLocalTime				timeEnd;
	BOOL					bOrdered;
};

struct KGuildBFInfo
{
	DWORD					dwWorldID;
	BYTE					byWeek;
	BYTE					byRound;
	BYTE					byGroup;
	DWORD					adwGuildID[2];
	INT64					an64Combat[2];
	::System::Collections::KString<GUILD_NAME_MAX>	astrGuildName[2];
	INT						anGuildIntegral[2];
};

struct KGuildBFApplyInfo
{
	BYTE					byOrder;
	DWORD					adwGuildID;
	::System::Collections::KString<GUILD_NAME_MAX>	astrGuildName;
};

struct KAreaNameAlias
{
	int m_AreaId;
	BYTE m_ColorR;
	BYTE m_ColorG;
	BYTE m_ColorB;
	FLOAT m_MinVer;
	FLOAT m_MaxVer;
	char m_AreaName[MAX_MD5_STRING];
	char m_AreaState[MAX_MD5_STRING];
};

struct KRecommendServerInfo
{
public:
	KRecommendServerInfo() {Init();}
	KRecommendServerInfo(const KRecommendServerInfo &aData)
	{
		m_RecommendAreaID = aData.m_RecommendAreaID;
		m_RecommendSvrID = aData.m_RecommendSvrID;
		m_RecommendMessageTitle = aData.m_RecommendMessageTitle;
		m_RecommendMessage1 = aData.m_RecommendMessage1;
		m_RecommendMessage2 = aData.m_RecommendMessage2;
	}

	void Init()
	{
		m_RecommendAreaID = 0;
		m_RecommendSvrID = 0;
		m_RecommendMessageTitle = "";
		m_RecommendMessage1 = "";
		m_RecommendMessage2 = "";
	}
	BYTE m_RecommendAreaID;
	BYTE m_RecommendSvrID;
	::System::Collections::KDString<MAX_PATH> m_RecommendMessageTitle;
	::System::Collections::KDString<MAX_PATH> m_RecommendMessage1;
	::System::Collections::KDString<MAX_PATH> m_RecommendMessage2;
};
DECLARE_SIMPLE_TYPE(KRecommendServerInfo);

// 掉落组
struct KDropGroupParam
{
	VOID	Init()
	{
		wGroupID = 0;
		byQuantity = 0;
		dwProbability = 0;
		dwInfluenceDegree = 0;
	}
	WORD	wGroupID;				// 掉落组ID
	BYTE	byQuantity;				// 抽取次数
	DWORD	dwProbability;			// 抽取概率dwProbability/1000000;百万分之n
	DWORD   dwInfluenceDegree;		//
};
 
struct KWorldDropParam
{
	VOID	Init()
	{
		byID = 0;			
		byQuantity = 0;			
		dwProbability = 0;					
		byLevelType = 0;		
		shMinTC = 0;			
		shMaxTC = 0;		
		memset( aSGC,0,sizeof(aSGC));
		memset( aQuality,0,sizeof(aQuality));
	}
	WORD	byID;				// 世界掉落ID
	WORD	wSGCCount;
	POINT	aSGC[MAX_DROP_ITEM_SGC_COUNT];	// 掉落组
	BYTE	byQuantity;				// 抽取次数
	DWORD	dwProbability;			// 抽取概率dwProbability/1000000;百万分之n
	WORD	wQualityCount;
	POINT   aQuality[MAX_DROP_ITEM_QUALITY_COUNT];// 品质筛选
	BYTE	byLevelType;			// 等级筛选类型
	short	shMinTC;				// 最小等级
	short	shMaxTC;				// 最大等级
};

struct KIdentityBuffTootipInfo
{
	DWORD m_nBuffID;		// BuffID
	DWORD m_nSrcID;			// srcID
	DWORD m_nStacks;		// 叠加层数
	DWORD m_nRemainTime;	// 剩余时间
	DWORD m_dwExpireTime;	// 消失时间 仅限于以自然时间为倒计时的时间中
};

struct KBuffRelatedCharID
{
	DWORD m_dwSrcID;
	DWORD m_dwTargetID;
};

struct KTopMessage
{
	char *szMainTitle;
	char *szSubTitle;
	KTopMessage():szMainTitle(NULL), szSubTitle(NULL){}
};

#define AuroraPaoPaoSkillCount		3
struct KTempSkillSlotInfo
{
	BYTE pos;
	DWORD skillID;
};

struct KAttrUpdateVal
{
	INT nDeltaVal;
	INT nNewVal;
};

#define MAX_TRAFFICNAME_LEN 16

struct KTrafficStation
{
	char name[MAX_TRAFFICNAME_LEN];
	int nID;
	float x,y,z;
};
DECLARE_SIMPLE_TYPE(KTrafficStation);

struct KTrafficPath
{
	KTrafficStation station;
	::System::Collections::DynArray<KTrafficStation> stations;	
};

enum ChargeLandScape
{
	enumCLS_None = 0,
	enumCLS_Line,
	enumCLS_Circle,
	enumCLS_Sector,//扇形
	enumCLS_Rectangle,
	
	enumCLS_Count,
};

struct KChargeLandScape
{
	BYTE byCLSType;
	DWORD param1;
	DWORD param2;
};
#pragma pack(pop)


struct KChangeMap_Param
{
	int mMapID;
	BOOL mbduplicate;
	char mszMapPath[MAX_PATH];
};

#pragma pack(push,1)
struct PlayerDataHandlers
{
	HANDLE hPlayer;

	// ...
	void reset()
	{
		hPlayer = (HANDLE)0x7fffffff;
	}
};
#pragma pack(pop)
