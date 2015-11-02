/********************************************************************
	created:	2007/11/29
	created:	29:11:2007   18:05
	filename: 	Athena\Main\Src\Common\LogicWorld\Src\KCommonStruct.h
	file path:	Athena\Main\Src\Common\LogicWorld\Src
	file base:	KCommonStruct
	file ext:	h
	author:		xueyan
	
	purpose:	�������ﶨ�������һЩͨ�õ���ͽṹ
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

#define CFG_ITEM_SPECIAL_TABLE_SPLIT	100000000	// ���ݿ��б�ָ���

#define CFG_MAX_TITLE_COUNT				(20*20)		// �ƺŵĸ���
#define	CFG_MAX_ITEMBASE_SKILL			8			// ���������
#define CFG_MAX_INLAY_FORMULA_PARM		(11)		// �����Ƕ��ʽ��������
#define CFG_MAX_ITEM_ATTACH_PARM_COUNT	(10)		// ��Ʒ��������������

#define	CFG_MAX_ITEM_TRADE				6			// ������������
#define CFG_MAX_ITEM_STRENGTH			5			// ǿ�����������Ʒ���������
#define CFG_MAX_ITEM_INLAY				5			// ��Ƕ���������Ʒ���������
#define CFG_MAX_ITEM_MB_INLAY			3			// ������Ƕ���������Ʒ���������
#define CFG_MAX_ITEM_RABBET				2			// ���۽��������Ʒ���������
#define CFG_MAX_ITEM_BREAK				2			// ���۽��������Ʒ���������
#define CFG_MAX_ITEM_SPAR				7			// ��ʯ�ϳɽ��������Ʒ���������
#define CFG_MAX_ITEM_COMPOSE			5			// װ���ϳɽ��������Ʒ���������
#define CFG_MAX_SHORTCUT				128			// ��ҿ��������
#define CFG_MAX_ITEM_MOVABLESHOP		30			// ��Ұ�̯�����Ʒ����
#define CFG_MAX_TEMP_SHORTCUT			6			// �����ʱ�����

#define CFG_MAX_SYNC_MULITITEMS			10			// �����Ʒͬ������

#define MAX_ITEM_PACK					2

// �������
#define MAX_MAGIC_BABY_COUNT_PRE_PLAYER			6
#define MAX_MAGIC_BABY_SKILL_COUNT				20
#define MAX_MB_INLAY_STONE_COUNT				3
#define MAX_MB_WUXING_SKILL_COUNT				9	// ���������м�����

// װ�����ܵ���󻺳��С
#define MAX_MB_EQUIP_SKILL_INFO_BUFFER_SIZE		100
#if MAX_MB_EQUIP_SKILL_INFO_BUFFER_SIZE > 255
#	error "MAX_MB_EQUIP_SKILL_INFO_BUFFER_SIZE bigger than 255 and can not store in one BYTE!!"
#endif

// ItemCreateInfo ��Ҫʹ�õĺ�
#define MAX_ITEM_NAME_LEN				50
#define MAX_ITEM_DESC_LEN				50
#define	MAX_ITEM_SLOT_COUNT				6
#define MAX_PASSIVESKILL_COUNT			5
#define	MAX_ITEM_SUBATTR_COUNT			8
#define	MAX_ITEM_BASEATTR_COUNT			6
// #define	MAX_ITEM_WORLD_DROP_COUNT		12		// 2009-05-31 ���������������4�����ӵ�12�� ���ඨ�壬Ӧʹ�� MAX_WORLD_DROP_COUNT
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


#define CFG_MAX_LIMIT_EVENT_RECORD		100		// ���ʹ��ĳ�ֶ������߼�������¼����
#define CFG_MAX_ACTION_SAVE_RECORD		100		// ���ʹ��ĳ�ֶ������߼�������¼����
#define CFG_MAX_CUSTOM_EVENTS_RECORD	128		// ��ұ���ĳЩ�¼����������
#define CFG_MAX_SETID_EVENT_RECORD		256		// ����װ�������
#define CFG_MAX_DUP_EVENT_RECORD		100
#define CFG_MAX_TALENT_POINT_BYTE_SIZE	60		// ����츳������ֵ

// ------------------- ��ͨ��· --------------------------------------------------------------
// 

#define TRAFFIC_DEFAULT					0x0000
#define TRAFFIC_FLYMODEL1				0x0001
#define TRAFFIC_FLYMODEL2				0x0002
#define TRAFFIC_FLYMODEL_DEFAULT		TRAFFIC_FLYMODEL1
#define TRAFFIC_FLYMODEL				(TRAFFIC_FLYMODEL1|TRAFFIC_FLYMODEL2)
#define TRAFFIC_STATION					0x0004

// ------------------- ���� --------------------------------------------------------------
// ���ݿ���أ��빲���ڴ湲�ã��޸�ʱ��ע�⣬һ���������GameServer��DBAgent��Ҫ���±���

#define MOSAIC_ITEM(item)			((item).byState	= enum_ItemState_MoSaic)
#define FREE_ITEM(item)				((item).byState	= enum_ItemState_Free)
#define	IS_ITEM_FREE(item)          ((item).byState == enum_ItemState_Free)
#define	IS_ITEM_MOSAIC(item)        (((item).byState & enum_ItemState_MoSaic) != 0)

#define DEF_MAX_SEND_CHAR_COUNT		(4)
#define DEF_MAX_SEND_SERVER_COUNT	(20)

#define DEF_PLAYER_MAX_MONEY	20*10000*10000		// 20��

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
	// �������INT64��ǿ������ת��
	inline operator INT64() const
	{
		return (((INT64)m_dwUserType)<<32) + m_dwPassportID;
	}
	// �������INT64��ǿ������ת��
	inline operator UINT64() const
	{
		return (((UINT64)m_dwUserType)<<32) + m_dwPassportID;
	}
	// �������DWORD��ǿ������ת��
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

	inline operator DWORD() // �������DWORD��ǿ������ת��
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
/* ��Ʒ������Ϣ KItemBase                                             */
/************************************************************************/

//*******************************************************************************************/	//��Ʒ
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

	WORD mFlag_Bind : 1;			// ��
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

	BYTE					byType;					// ��Ʒ����
	BYTE					byLen;					// ����
	BYTE					byPos;					// ��Ʒλ��
	WORD					wItemID;				// ��Ʒ���ID
	WORD					wItemNum;				// ��Ʒ�ѵ�����
	INT128					nSerialCode;			// ��ƷΨһID
	KItemFlags				mPrivateFlags;			// ��Ʒ���⹦�ܱ��

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
	INT8					nFrequency;				// ��Ʒ��ǰʹ�ô���
	INT32					nAliveSeconds;			// ����ʣ������ʱ��
	
	union
	{
		INT8				nReserve[DEF_NEW_SITEM_KEEP_SIZE];// ����������
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
	INT16					nSkillID;					// ��Ʒ����ID
	INT16					nCurrentDur;				// ��ǰ�;ö�
	INT8					nStrengthLevel;				// ��Ʒǿ���ȼ�
	CHAR					szMakerName[MAX_NAME_DEF];	// ����������

	struct KAttrBase
	{
		INT16 mIndex;
		INT8 mValue;
	};

#define DEF_EQUIP_MAX_BASE_ATTR	(2)
#define DEF_EQUIP_MAX_BASE_ATTR_VALUE1	(31)
#define DEF_EQUIP_MAX_BASE_ATTR_VALUE2	(32)
	// װ����������
	struct KEquipBaseAttr
	{
		INT8 mAttr[DEF_EQUIP_MAX_BASE_ATTR];
	} mEquipBaseAttr;

#define DEF_EQUIP_MAX_ATTR_EX	(4)
#define DEF_EQUIP_MAX_ATTR_EX_VALUE1	(33)
#define DEF_EQUIP_MAX_ATTR_EX_VALUE2	(34)
	// װ����������
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

	// ��Ʒ��������
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
		// װ����������
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
	INT8					nFrequency;			// ��Ʒ��ǰʹ�ô���
	INT32					nAliveSeconds;		// ����ʣ������ʱ��

	union
	{
		KDBItemEquip		mEquipInfo;						// װ����Ʒ��Ϣ
		KDBShenXingFu		mShenXingFu;					// ���з�
		KExpJar				mExpJar;						// �����
		INT8				mReserve[DEF_NEW_ITEM_KEEP_SIZE];// ����������
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


// �������ݿ�ṹ����
struct KDBPetItem : public KItemBase, public System::Memory::KPortableStepPool<KDBPetItem,3>
{
	INT32	nAliveSeconds;			// ����ʣ������ʱ��

	BYTE	byVer;						// �汾��Ϣ
	// WORD	wType;						// ���� wItemID
	BYTE	byElement;					// ��������
	UINT64  uiGUID;						//����Ψһ��ʶ
	DWORD	dwCreateTime;				// ��������ʱ��
	char	strName[MAX_NAME_DEF];		// ����

	WORD	wAura;		// ����
	WORD	wQualityAtk;
	WORD	wQualityDef;
	WORD	wQualityHp;
	INT		nHp;
	INT		nMp;
	int		nn;			// ��ǰ��ʳ��
	BYTE	byColor : 4;		// 16����ɫ
	BYTE	byLevel;
	BYTE	byll;		// �����׶�
	BYTE	bya;		// �����
	BYTE	byba;		// �������
	BYTE	byIsShow;	// �����Ƿ��ս
	DWORD	dwExp;

	BYTE	byEquipLevel;
	BYTE	byAuraLevel;	// ��ǰƷ�ʵȼ�

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
// �����Ʒ������Ʒ�洢�Ĵ洢�ṹ                                       */
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
	BYTE m_byBagType;	// ����ҳ����
	BYTE m_byBagPos;	// ������Ӧҳ���Ӻ�
};

struct KTitle_Data
{
	KTitle_Data() { memset(this , 0 , sizeof(KTitle_Data)); }
	BYTE m_byTitleClassID;
	BYTE m_byTitleID;
	BYTE m_byTitleLevel;
	DWORD m_dwRemainTime;
};

// ��Ʒͳ����Ϣ
struct KItemStatisticsInfo
{
	short mStaticMaxCount;
	short mDynamicMaxCount;
	short mCurrentCount;
	short mNextEmptyIndex;
};

// ��Ʒ����
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


// �������ÿ�����һ��ʱ����ֻ��ִ�м��ε��¼���¼����Ҫ���浽���ݿ�
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


// NPC�̵������Ʒ
struct ITEM_STORE
{
	WORD			wItemID;								// ��ƷID
	BYTE			byCount;								// ������Ʒ������0��ʾ������
	BYTE			byRefreshTime;							// ˢ��ʱ�䣺��λСʱ
};

/************************************************************************/
/* ��Ʒ��ʼ��������Ϣ        ItemCreateInfo                             */
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
	DWORD	dwRemainTime;	//��λms
	DWORD	dwSaveTime;		//��λs��ϵͳʱ��(time(0))
	BYTE	byStacks;		//���Ӳ���
	BYTE	byTriggerTimes[3];	//������������������
};
#pragma pack(pop)



const int MAX_SKILL_CLASS            = 15;
const int NONFIGHT_SKILL_CLASS_ID    = (MAX_SKILL_CLASS-1);

const int MAX_SKILL_TERM             = 5;
const int MAX_OCCUPATION_SKILL_CLASS = 4;
const int MAX_SKILL_GROUP_CAPACITY   = 8;	// ÿ������ɵļ�������
const int MAX_SKILL_GROUP_NUM        = 10; // ÿ����ҿ�ӵ�еļ�������
const int UI_PAGE_SKILL_CAPACITY     = 12; // ÿҳ��ʾ���ټ���
const int INVALID_SKILL_ID           = 0; // ��Ч�ļ���ID

#pragma pack(push,1)
struct KPlayerBaseDBInfo
{
	DWORD	CT_ID;
	int		CT_ACCID;
	int		CT_USERTYPE;

	char	CT_NAME[MAX_NAME_DEF];
	char	CT_SEX;
	BYTE	CT_CLASS;				// ְҵ

	BYTE	CT_ORIENTATION;			// ��֧
	BYTE	CT_NATIONALITY;			// ����
	BYTE	CT_MODEL;				// ģ��
	BYTE	CT_HAIRSTYLE;			// ����
	BYTE	CT_COMPLEXION;			// ��ɫ
	BYTE	CT_HAIRCOLOR;			// ��ɫ
	BYTE	CT_FACESTYLE;			// ����
	BYTE	CT_PRINK;				// ��ױ


	DWORD	CT_EXP;
	DWORD	CT_REALM_EXP;		// ���澭��
	INT64	CT_MAINMONEY;
	INT64	CT_OFFMONEY;

	int		CT_X;
	int		CT_Y;
	int		CT_Z;

	BYTE	CT_LEVEL;			// �ȼ�
	BYTE	CT_REALM_LEVEL;		// ����ȼ�
// 	BYTE	CT_TALENT;			// ����
// 	BYTE	CT_INTELLIGENCE;	// ����
	INT		CT_RP;				// ��������
	INT		CT_HP;				// ��ǰ����ֵ
	INT		CT_MP;				// ��ǰ����ֵ
	DWORD	CT_PORTALID;		// 
	DWORD	CT_MASTERLEVEL;		// ���Ȩ��
	INT		CT_State;			// �������״̬
	DWORD	CT_VIPLEVEL;		// ���VIP����
	BYTE	CT_Energy;			// ����(��������Ƶ������)
	INT		CT_LastEnergyRestoreTime;
	BYTE	CT_ShowHelm;		// ��ʾñ��
	DWORD	CT_ECP;				// ��ǰ�澭����
//	DWORD	CT_FaithLevel;		// ����ȼ�
	BYTE	CT_ShowFanshion;	// ��ʾʱװ
	DWORD	CT_DeathPunishmentPlan;	// ���������ͷ�
	DWORD	CT_dwTireVal;			// ƣ��ֵ
	INT		CT_CanCostCultivation;		//��Ϊֵ(������)
	INT		CT_CanNotCostCultivation;	//��Ϊֵ(������)
	BYTE	CT_RPActive;				//��������ϵͳ�Ƿ񼤻�
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
	SHORT	shFaithLevel;				// ��Ϊ�ȼ�
	DWORD	dwCurExp;					//��ǰ����ֵ
	DWORD	dwCurFaith;					// ��Ϊ

	INT		nHP;						// ��ǰ����ֵ--*
	INT		nMP;						// ��ǰ����ֵ
	INT		nRP;						// ��ǰŭ����Ҳ��������ֵ

	INT		nNimbusPower;				// ������ֵ
	INT		nNimbusPowerAddGene;		// �������� enumMBBI_MB_NIMBUS_POWER_ADD_GENE
	INT		nNimbusPowerLevel;			// ���������Ǽ�--*
	INT		nAffinity;					// ���ܵ�ǰֵ

	BOOL	bIsShow;					// �Ƿ��ڳ�ս״̬

// 	// U�����ܸ��еļ���ID
// 	DWORD	dwUSkillID[MAX_MAGIC_BABY_SKILL_COUNT];
// 	// L����ѧ��ļ���ID
// 	DWORD	dwLSkillID[MAX_MAGIC_BABY_SKILL_COUNT];
// 	// Ŀǰ1���ܸ����򿪣�0����ر�
// 	BYTE	byteSkillCell[MAX_MB_WUXING_SKILL_COUNT];

	INT		nInlayStone[MAX_MB_INLAY_STONE_COUNT]; //--*

	INT		nElementVal[SOUL_TYPE_COUNT];	//��������ǻ���ֵ

	BYTE	byEquipInfo[MAX_MB_EQUIP_SKILL_INFO_BUFFER_SIZE];	//����������أ�ʹ�õ������µĽṹ��������
	//typedef struct _MB_SKILL	//2009/12/30���壬������Ϸ�ڲ������¹�������ṹ��
	//{
	//	u_char		s_nSize;				//��Ч���ݵĳ���
	//	u_int		s_nNormalAttack;		//��ͨ�����ļ���
	//	u_int		s_nSkill[4];			//����ʹ�õļ���
	//	u_int		s_nSkillLearn[9];		//�Ѿ�ѧ��ļ���
	//	u_int		s_nSkillUnique;			//���У���˵�еĴ���
	//	u_int		s_nSkillPassive[3];		//��������
	//	u_char		s_chReserve[27];		//������չ
	//	_MB_SKILL()
	//	{
	//		memset( this, 0, sizeof(_MB_SKILL) );
	//	}
	//} MB_SKILL, *P_MB_SKILL;
	DWORD	dwLastAbsorbTime;			// ��һ�������ʱ��
	DWORD	dwAbsorbToday;				// ����һ������ֵ

	DWORD	dwLastReceiveTime;			// ��һ��������ʱ��
	DWORD	dwReceiveToday;				// ����һ�������յ�ֵ

	int		nElement;		// ��������
	int		nSavvy;			// ��������
	int		nRelation;		// �������ܶ�
	int		nSpecialFace;	// ˫A����
	int		nFuse;			//����ֵ����ǰֵ��
	DWORD	dwCreateTime;	// ��������ʱ��
	UINT64  uiGUID;			//����Ψһ��ʶ
	int		nSkillGroup[MB_SKILLGROUP_MAX];	//����������
	int		nSkillGroupPoints;	//���ܵ㣨��ǰֵ��
	BYTE	byMBSpecialZZ;		//����Ʒ�ʣ�ͬװ�������ֵȼ�����װ����װ�ȣ�
	BYTE	byMBSpecialLL;		//����Ʒ��
	WORD	wFire;				//����ֵ
	int		nTalent[MB_TALENT_MAX];	//�츳ֵ
	int		nTalentGrid;	//�츳ֵ
	int		nAbility;
	int		nFirstValue;
	int		nSkillValue;
	int		nFightValue;
	int		nEvalue;
	int		nBind;
	char	skillCDInfo[52];	// �洢ʱ��(4byte) + 4��CDʱ��(ÿ��12byte)
	BYTE	byAutoSkillQueueInfo;	// �Զ�ʩ�ż��ܵ��趨
	int		nCurSoulByDay;
	BYTE	byEatCount;			// ÿ�ջ������
	BYTE	byEatValue;			// ���ӵ�����ֵ
	BYTE	byResv[2];
	int		nEatTime;			// ����ʱ��
	int		nEatTotalTime;		// �������ʱ��
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
	// ����IDΪ��ҵ�½ʱ���ʼ�ķ������еķ���ID�������˳�ʱ�����бȽ�ɾ���ġ�
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
		// Ϊ�棬����Ҫɾ���ˡ�
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
	INT	m_nStacks;		// ���Ӳ���
	INT	m_nRemainTime;	// ʣ��ʱ��
	INT	m_dwBuffDuration;	// ��ʱ��
	INT m_TooltipTextID;// ToolTip
	char m_szBuffIcon[MAX_MODEL_NAME_DEF + 1];	// Buffͼ��

	INT	m_nDuraType;		// 
	DWORD m_dwExpireTime;
};

struct STRU_BUFFS_INFO_FOR_DISPLAY
{
	int m_nBuffCount;			// ����Buff����
	STRU_BUFF_INFO_FOR_DISPLAY m_struBuffInfo[MAX_CHAR_BUFF_COUNT];
};

struct KPlayerAttr
{
	VOID	Init() {memset(this, 0, sizeof(KPlayerAttr));}
	DWORD	dwID;						// ��ɫID
	CHAR	szName[MAX_PATH];			// ��ɫ�ǳ�
	CHAR	szCyberCafeName[MAX_PATH];	// ��ɫVIP��������
	CHAR	szHeadImage[MAX_MODEL_NAME_DEF];	// ��ɫͷ��ͼ
	BYTE	byClass;				// ְҵ
	BYTE	byLevel;				// �ȼ�
	BYTE	byTalent;				// ����
	BYTE	byIntelligence;			// ����
	BYTE	byNationality;			// ����
	BYTE	bySubClass;				// ��֧
	BYTE	bySex;					// �Ա�
	BYTE	byPrink;				// ��ױ
	BYTE	byFaceStyle;			// ����
	BYTE	byHairStyle;			// ����
	BYTE	byHairColor;			// ��ɫ
	BYTE	byTeamPowerLtd;			// ����Ȩ��
	DWORD	dwTeamID;				// ����ID
	int		nHP;
	int		nMP;
	int		nRP;
	int		nMaxHP;
	int		nMaxMP;
	int		nMaxRP;
	int		nExp;					// ��ǰ����
	int		nNextExp;				// �¼�����
	float	fX;
	float	fY;
	float	fZ;
	int		nAttr[6];				//����A~E
	int		HPR;					//�����ָ���
	int		MPR;					//�����ָ���
	int		DHR;					//������
	int		CHR;					//������
	int		P_DMin;					//�﹥Min
	int		P_DMax;					//�﹥Max
	int		I_P_AC;					//���״�͸
	int		P_AC;					//����
	int		P_DR;					//������
	int		M_DMin;					//����Min
	int		M_DMax;					//����Max
	int		I_M_AC;					//���Դ�͸
	int		M_AC;					//����
	int		M_DR;					//�ֿ���
	DWORD	dwMainMoney;			// �ǰ󶨽�Ǯ
	DWORD	dwOffMoney;				// �󶨽�Ǯ
	DWORD	dwTypeID;				// ��ɫ����ID
	DWORD	dwGmLevel;				// GM�ȼ�

	//����ר��
	BYTE	byteMBPos;				// ����λ��
	char	strMBIcon[MAX_PATH];	// ����ͼ��
	char	strMBElementDis[MAX_NAME_DEF];	// ����Ԫ����ʾ

	char	strCLIcon[MAX_PATH];	// ����ͼ��
	char	strRLIcon[MAX_PATH];	// ����ͼ��
	char	strSLIcon[MAX_PATH];	// ����ͼ��

	BYTE	byteMBPowerLevel;		// �������ʵȼ�
	INT		nMBPowerAddGene;		// ��������
	INT		nMBUseLevel;			// ����ʹ�õȼ�
	INT		nMBNimbusPower;			// ��������
	INT		nMBBaseNimbusPower;		// ������������
	INT		nMBIncreaseNimbusPower;	// �����ɳ�����
	BOOL	bMBIsShow;				// �����Ƿ��ս

	WORD	mw_FaithCurrentExp;
	WORD	mw_FaithNextExp;
	BYTE	mby_FaithLevel;
	BYTE	mby_CanRealCombat;		// �Ƿ���Ŀ�ս��
	BYTE	mby_CanCombat;			// �Ƿ��ս��
	BYTE	mby_CanAbsorb;			// �Ƿ������

	BYTE	mby_CanRide;			// �Ƿ�����
	BYTE	mby_CanRefine;			// �Ƿ������
	BYTE	mby_CanInlay;			// �Ƿ����Ƕ
	BYTE	mby_CanStunt;			// �Ƿ�ɾ���
	BYTE	mby_CanContract;		// �Ƿ����Լ

	BYTE	mby_CanCombatLv;		// �Ƿ��ս��Lv
	BYTE	mby_CanRideLv;			// �Ƿ�����Lv
	BYTE	mby_CanRefineLv;		// �Ƿ������Lv
	BYTE	mby_CanInlayLv;			// �Ƿ����ǶLv
	BYTE	mby_CanStuntLv;			// �Ƿ�ɾ���Lv
	BYTE	mby_CanContractLv;		// �Ƿ����ԼLv

	// npcר��
	SHORT	m_nNpcAttackType;		// npc��������
	int		m_nFightState;			// �Ƿ�ս����
	BYTE	m_nFiveElement;			// ��������
	BYTE	m_byEliteLevel;			// ��Ӣ�ȼ�
	BYTE	m_byAbility;			//
	BYTE	m_byIsOwner;			//

	int		m_nGuildID;
	int		m_nGuildFunc;
	int		m_nGuildLevel;

	STRU_BUFFS_INFO_FOR_DISPLAY m_struBuffInfo;
	STRU_BUFFS_INFO_FOR_DISPLAY m_struDebuffInfo;
	//ѧ������м���
	INT m_GirdSkillID[MAX_MB_WUXING_SKILL_COUNT];
	//ѧ���ר������
	INT m_PassiveSkillID[MAX_MB_WUXING_SKILL_COUNT];
	//ѧ��ľ���
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
	BOOL			bAdded;		// Ϊ��ʱ�������ֵΪ������������Ǿ���ֵ
	unsigned short nDirection; //�Ƕ�
	unsigned short nDirectionZ; //�Ƕ�
};

////////////////////////// ��Ʒ //////////////////////////////////////////
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
	WORD	wInitCount;		// ������Ʒ�ĳ��������������������������������Ϊ0����ʾ��������Ʒ
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
	int		nExploit;		// ��ѫֵ
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
	
	DWORD			dwPlayerID;		// ���ID
	BYTE			bySubCmd;		// ������
	WORD			wPos;			// ��Ʒλ��
	BYTE			wSrcPos;		// Դλ��
	BYTE			bySrcPosType;	// Դλ������
	BYTE			wDestPos;		// Ŀ��λ��
	BYTE			byDestPosType;  // Ŀ��λ������
	BYTE			byResult;		// ���ؽ��
	BYTE			byState;		// ״̬
	DWORD			dwMoney;
};

// ������߳���

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

// �̵�ϵͳʹ�õĽṹ
#pragma pack(push,1)
struct KChainStore
{
	// DWORD	m_dwID;		//npc id������BaseSet��ֻ�ܽ�m_dwID
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
	BOOL			bAdded;		// Ϊ��ʱ�������ֵΪ������������Ǿ���ֵ
	unsigned short nDirection;	//�Ƕ�
	unsigned short nDirectionZ;	//�Ƕ�
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


// ��ҹ�������ʱ���ȼ�¼ÿ�˹������������ȹ���������һ���㾭��
struct KAttackNpcParam
{
	DWORD		dwID;		// ������ID�����������ID
	int			nAttackHP;	// ������������ֵ
	BYTE		byIsTeam;	// �Ƿ������ID
	BYTE		byIsUsed;	// �Ƿ�ʹ��
	WORD		wResv;		// ����Ӱ�쾭����������
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
		//��VOID*�ĳ�KCharacter* by Allen 20081021
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
		ASSERT_I(FALSE);//��֧�ֵ����ͱȽ�
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

	CURSOR_NORMAL,			//��ͨ
	CURSOR_NORMAL_DISABLE,	//��ͨ��
	CURSOR_ATTACK,			//����
	CURSOR_ATTACK_DISABLE,	//������
	CURSOR_ATTACK_SKILL,			//�����ܹ���
	CURSOR_ATTACK_SKILL_ENABLE,		//�����ܿɹ���
	CURSOR_ATTACK_SKILL_DISABLE,	//�����ܹ�����
	CURSOR_ATTACK_SKILL_RANGE,		//�����ܷ�Χ����
	CURSOR_QUESTFINISH,		//�ɽ�����
	CURSOR_QUESTUNACCEPT,	//δ��������������
	CURSOR_QUESTACCEPT,		//�ѽ�δ���
	CURSOR_QUESTCONDITION,	//δ�����񣬲���������
	CURSOR_STUDYSKILL,		//ѧϰ���� 
	CURSOR_TRANS,			//����
	CURSOR_STORE,			//�̵�
	CURSOR_MAIL,			//�ʼ�
	CURSOR_STORAGE,			//�ֿ�
	CURSOR_TALK,			//�Ի�
	CURSOR_PICKUP,			//ʰȡ��Ʒ
	CURSOR_PICKUP_DISABLE,	//ʰȡ��Ʒ��
	CURSOR_MINE,			//�ɿ�
	CURSOR_MINE_DISABLE,	//�ɿ��
	CURSOR_HERBS,			//��ҩ
	CURSOR_HERBS_DISABLE,	//��ҩ��
	CURSOR_INTERACT,		//����
	CURSOR_INTERACT_DISABLE,//���ֻ�
	CURSOR_REPAIR,			//����
	CURSOR_REPAIR_DISABLE,	//�����
	CURSOR_HOVER,			//��꼤��(�ҽ���Ʒ...)
    CURSOR_SELL,
	CURSOR_NUMBER,

};

enum MouseState_Type
{
	MouseState_NULL = 0,		//��״̬
	MouseState_Default = 1,		//Ĭ��״̬
	MouseState_System = 2,		//ϵͳ״̬������Ϲҽ���ϵͳ�ṩ�ļ���
	MouseState_Skill = 3,		//����״̬������Ϲҽ�����Ҽ��ܻ�����Ʒ����
	MouseState_SlotMove = 4,	//�ƶ����߻���ͼ��
};

enum MouseCommand_Type
{
	MouseCommand_NULL ,		// ������		
	MouseCommand_Cancel,	// �˳���ǰ�������
	MouseCommand_Select , // ѡ�����
	MouseCommand_LeftSkill , // �������
	MouseCommand_RightSkill, // �Ҽ�����
	MouseCommand_TalkToNpc,	// ��NPC�Ի� 
	MouseCommand_TCSkill, //��������
	MouseCommand_PickUpGroundItem, //ʰȡ������
	MouseCommand_Skill,			//��긽������
	MouseCommand_ItemSkill,		//��긽�����߼���
	MouseCommand_AutoPath,		//�Զ�Ѱ·
};


enum KMouseTarget_Type
{
	KMouseTarget_NULL,
	KMouseTarget_Ground,	//����
	KMouseTarget_Obj,		//�����������NPC�����䡢��ҵ�
	KMouseTarget_Icon,		//UI�е�ͼ�����
	KMouseTarget_NormalUI,  //��ͨ��UI���
};

#define MAX_OBJ_CMD_PARAM_NUM	(5)

struct KMouse_Command
{
	MouseCommand_Type m_eMouseCommandType;
	KMouseTarget_Type m_eMouseTargetType;	//ϣ��������������
	DWORD		m_pMouseDataParam[MAX_OBJ_CMD_PARAM_NUM];	//���ҽ�����
	DWORD		m_pTargetParam[MAX_OBJ_CMD_PARAM_NUM];		//���Ŀ������
	BOOL		m_bHasTarget;								//�Ƿ���Ŀ��

	typedef BOOL (*Mouse_CommandCallback)(DWORD* pTargetParam,DWORD* MouseParam2);		
	Mouse_CommandCallback	m_MouseCommandFunc;

	KMouse_Command()
	{
		memset(m_pMouseDataParam, 0, sizeof(m_pMouseDataParam));
		memset(m_pTargetParam, 0, sizeof(m_pTargetParam));
		Clear();
	}
	//������ʱһ��Ҫȷ��Ŀ�����Ч�ԣ�������Ҫֱ�Ӵ�����ָ��
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
	KGameOperater_Type m_eGameOperaterType;		//��������
	KMouseTarget_Type m_eMouseTargetType;		//��Ҫ������������Ŀ�����ͣ��Ǵ����Ͳ��ᱻѡ��
	DWORD		m_OperaterDataParam[MAX_OBJ_CMD_PARAM_NUM];	//��Ϸ������������
};

// �����
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
	BYTE  btActionMode;		// ��Ϊ��ʽ
	BYTE  btActionTarget;	// ��Ϊ����
};
struct STMonster
{
	INT		iMapId;				// ��ͼID
	wPoint ptLocation;			// X��Y����
	SHORT	siRadius;			// ˢ�ְ뾶
	INT		iCategory;			// ��������
	SHORT	siNumber;			// ��������
	SHORT	siBornAdjustCond;	// ˢ�µ�������
	BOOL	bInOrder;			// ���/˳��
	siPoint ptPatrolPoint[8];	// Ѳ��·��1-8
	INT		iScript;			// �ű���
};

struct PatrolPointDetail
{
	PatrolPointDetail() {Init();}
	VOID Init() {m_dwActionType = 0;m_PatrolPointParmList.clear();}
	DWORD m_dwActionType;
	typedef ::System::Collections::DynArray<DWORD, 4, 1> KPatrolPointParmArray;
	KPatrolPointParmArray m_PatrolPointParmList;
};

// ˢ�ֵ�
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
	DWORD m_dwTime;			// Ѳ�ߵ�ͣ��ʱ��
	PatrolPointDetailArray m_PatrolPointDetailArray;	// Ѳ�ߵ���Ϊ��
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

	BYTE	byTotalPoints;		// Ѳ�ߵ�����
	BYTE	byTeam;				// Ѳ��ģʽ���õ���,(ȱʡΪ0,ָ������)
	BYTE	byRouteOrder;		// Ѳ��˳�� (0=����;1=���.....)
	PatrolPoint* patrolpoint;	// Ѳ�ߵ�
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

	INT		iMapId;				// ��ͼID
	float	siRadius;			// ˢ�ְ뾶
	SHORT	siNumber;			// ��������
	SHORT	siBornAdjustCond;	// ˢ�µ�������
	BYTE	byDefaultState;		// ȱʡ״ֵ̬,ָ�����ж����б�,0=����վ������
	INT		iScript;			// �ű���
	DWORD   GroupTypeID;		// ������ID(40000+)
	DWORD   GroupInstanceID;	// ������(1-n)
	BYTE	TeamID;				// ������ID(1-8)
	BYTE	PatrolPattens;		// Ѳ��ģʽ��,���ж���Ѳ�߷�ʽ(ˢһȺ��ʱ�Ż����1)
	PatrolMode* patrolmode;		// Ѳ��ģʽ
	INT		iNpcPointId;		// ˢ�ֵ�ID
	INT		iCategory;			// ��������
	::System::Collections::KVector<KNpcBornPoint>	ptLocations;// X��Y����
};

// Ѳ�ߵ�
struct PatrolPoint1
{
	wPoint	ptPrtrolPoint;		// Ѳ�ߵ�����
	SHORT	siPrtrolState;		// Ѳ�ߵ�״̬
	SHORT	siPatrolAction;		// Ѳ�ߵ㶯��
	INT		nPointTime;			// Ѳ��ͣ��ʱ��
	BYTE	byActionPartner;	// Ѳ�ߵ���Ϊ����			//?
};

// Ѳ��ģʽ
struct PatrolMode1
{
	PatrolMode1(){patrolpoint = NULL;}
	~PatrolMode1(){ SAFE_DELETE_ARRAY(patrolpoint) }
	BYTE	byTotalPoints;		// Ѳ�ߵ�����
	BYTE	byTeam;				// Ѳ��ģʽ���õ���,(ȱʡΪ0,ָ������)
	BYTE	byRouteOrder;		// Ѳ��˳�� (0=˳��;1=���)
	PatrolPoint* patrolpoint;	// Ѳ�ߵ�
};

// ˢ�ֵ�
struct NPCPoint1
{
	NPCPoint1(){patrolmode = NULL;}
	~NPCPoint1(){SAFE_DELETE_ARRAY(patrolmode)}
	INT		iNpcPointId;		// ˢ�ֵ�ID
	INT		iMapId;				// ��ͼID
	wPoint	ptLocation;			// X��Y����
	SHORT	siRadius;			// ˢ�ְ뾶
	SHORT	siCategory;			// ��������
	SHORT	siNumber;			// ��������
	SHORT	siBornAdjustCond;	// ˢ�µ������� (default = 50% * siNumber)
	INT		iScript;			// �ű���
	BYTE	byDefaultState;		// ȱʡ״ֵ̬,ָ�����ж����б�,0=����վ������
	BYTE	PatrolPattens;		// Ѳ��ģʽ��,���ж���Ѳ�߷�ʽ(ˢһȺ��ʱ�Ż����1)
	PatrolMode* patrolmode;		// Ѳ��ģʽ
};


// ������Ϣ
//////////////////////// ��������ģ��䴫����Ϣ///////////////////////////////////////////
struct KChatOperatorParam
{
	BYTE							byChannel;								//	����Ƶ������
	BYTE							bySrcNation;							//  �����˹���
	BYTE							bySrcMasterLevel;						//	���Ȩ��
	BYTE							byDestMasterLevel;						//	���Ȩ��
	WORD							wMessageLen;							//	������Ϣ�ַ�����
	DWORD							dwPlayerID;								//	���ID��ţ��������������ӻ�ʲô
	DWORD							dwChannelID;							//	����Ƶ��ID
	DWORD							dwTargetPlayerID;						//	Ŀ�����ID
	::System::Collections::KDString<MAX_NAME_DEF>			m_SrcNick;								//	�������
	::System::Collections::KDString<MAX_NAME_DEF>			m_DestNick;								//	�����������
	::System::Collections::KDString<MAX_CHATMESSAGE_LEN>	m_Message;								//	����Ի���Ϣ
	DWORD							dwAliveTime;							//	������Ϣ����ʱ�䣨���룩
	BYTE							byPostID;								//  �����˹�ְ(0-7)
};

DECLARE_SIMPLE_TYPE(KChatOperatorParam);


struct KFightLogOperatorParam
{
	BYTE							byCastPlayer;							// ʩ����
	BYTE							byTargetPlayer;							// ��ʩ��Ŀ��
	BYTE							byOperationType;						// ��������
	WORD							wMessageLen;							// ������Ϣ�ַ�����
	::System::Collections::KDString<MAX_CHATMESSAGE_LEN>	m_Message;		// ����Ի���Ϣ
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
		//AHEAD: INT64��DWORD�����Ҫ�������ֻ��time(NULL)��ؿ��Կ���ʹ��TIME_T
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
	INT						m_nSkillGroupID;		//������ID
	INT						m_nCurrSkillCount;		//��ǰ���ܵȼ�
	INT						m_nMaxSkillCount;		//����ܵȼ�
	INT						m_nCurrSkillID;			//��ǰ����ID
	INT						m_nCurrSkillCost;		//��ǰ��������
	INT						m_nNextSkillID;			//��һ�ȼ����ܵȼ�
	INT						m_nNextSkillCost;		//��һ�ȼ���������
	INT						m_nRawX;				//��ʾλ��
	INT						m_nRawY;				//��ʾλ��
	BYTE					m_bySkillGroupState;	//����ѧϰ״̬
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
// ��һ�����Ϣ���ݴ��ݵĽṹ��
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
	BYTE		bySubCmd;			// ����
	DWORD		dwPlayerID;			// �Լ�ID
	BYTE		byState;			// ��ǰ״̬
	DWORD		dwTargetID;			// ����ID
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
// ��½�ͽ�ɫ�߼��Ľṹ��
#pragma pack(push,1)
struct STRU_KCharacterDisplayBaseInfo
{
	BYTE	CT_SEX;					// �Ա�
	BYTE	CT_PRINK;				// ��ױ
	BYTE	CT_NATIONALITY;			// ����
	BYTE	CT_FACESTYLE;			// ����

	BYTE	CT_HAIRSTYLE;			// ����
	BYTE	CT_HAIRCOLOR;			// ��ɫ
	BYTE	CT_COMPLEXION;			// ��ɫ
	BYTE	CT_MODEL;				// ģ��(��ɫģ�����ͱ��)

	BYTE	CT_Class;				// ְҵ
	BYTE	CT_SubClass;			// ��֧
	BYTE	CT_IsShowHelm;			// �Ƿ���ʾñ��
	BYTE	CT_IsShowFanshion;		// ��ʾʱװ

	TIME_T	CT_ProtectTime;			// ����ʱ��
	TIME_T	CT_ProtectTime2;		// ����ʱ��

	BYTE	CT_NeedChangeName;		// ������Ϸ�Ƿ���Ҫ����
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
	DWORD	CT_ID;					// ��ɫID
	DWORD	CT_LAST_LOGIN_TIME;		// �ϴε�¼ʱ��
	DWORD	CT_LAST_LOGIN_IP;		// �ϴε�¼�ص�
	BYTE	CT_LEVEL;				// �ȼ�
	BYTE	CT_IS_ALREADY_LOGIN;	// ���ε�½
	char	CT_NAME[MAX_NAME_DEF];	// ����
	STRU_KCharacterDisplayBaseInfo m_DisplayInfo;	// ��ɫ��ʾ������Ϣ
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
	DWORD	m_ServerIP;			// ������IP
	WORD	m_ServerPort;		// �������˿�
	BYTE	m_AreaID;			// ����ID
	BYTE	m_ID;				// ������ID
	BYTE	m_ServerState;
	BYTE	m_byRecommendationState;	// 0 ���Ƽ�	>0 �Ƽ� 
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

// ֪ͨ�߼�ChatServer׼���ã����ͻ��˷��͵���Ϣ
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

// ��������أ�zhu xiaokun 2008.9.10

enum eSkillGroupOperation
{
	enumSGO_Initialize = 0,
	enumSGO_GetManager,     // �ͻ���ר��

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
// 	INT	 m_SBI[64];								// �ȼ��޹صĻ�������
// 	INT	 m_SLI[64];								// �ȼ���صĻ�������
//	BOOL m_bTargetOnly;
};

#if defined(_CLIENT) || defined(_ROBOT)
struct KIdentitySkillProperty
{
	INT		m_nSkillID;			//����id����ҪUI����
	BOOL	m_bMainPlayer;		//�Ƿ�����ҵļ���
	INT		m_nHPCost;			//���ܵ�HP����
	INT		m_nMPCost;			//���ܵ�MP����
	INT		m_nRPCost;			//���ܵ�RP����
	CHAR	m_itemName[4][256];	//������Ʒ���ĵ���Ʒ����
	DWORD	m_itemCount[4];		//������Ʒ���ĵ���Ʒ����
	DWORD	m_itemKindCount;	//���Ķ�������Ʒ
	INT		m_nMinRange;		//��С��Χ
	INT		m_nMaxRange;		//���Χ
	INT		m_nR;				//�뾶���������򷵻�0
	DWORD	m_dwPrepareTime;	//PrepareTime
	INT		m_nCDTime;			//CDʱ��
	BOOL	m_bHPEnough;		//HP�Ƿ��㹻
	BOOL	m_bMPEnough;		//MP�Ƿ��㹻
	BOOL	m_bRPEnough;		//RP�Ƿ��㹻
	BOOL	m_bItemEnough[4];	//��Ʒ�Ƿ��㹻
	BOOL	m_bIsEquiped;		//�����Ƿ�װ��
	DWORD	m_dwCastTime;		//Shootʱ��
	DWORD	m_dwChannelTime;	//Channelʱ��
	INT		m_nLastCDTime;		//ʣ��cdʱ��
// 	INT		m_nMissRate;		//ʧ����
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
	int classPos;             // �������������ڵ�λ�ã�ֻ�е�pSkillClass��Ϊ������Ч
	KSkillClass* pSkillClass; // �����ѧ��ļ��ܵ��¸��������һ�������࣬�Ż��в�Ϊ�յ�ֵ
};


// �ṹָ����ΪuParam
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
// �ṹָ����ΪlParam
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
			int skillRelation[UI_PAGE_SKILL_CAPACITY];//��������1���׼��ܣ�2�Ѿ�ѧϰ�ļ��ܣ�3��ѧϰ�ļ��ܣ�
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



// GM����ģ�鴫������Ҫ������
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


// �ṹָ����ΪCOI_MAIN_PLAYER_USE_SKILL��Ϣ��uParam
struct KSkillMainPlayerUseSkill
{
	DWORD	m_dwSrcCharID;
	DWORD	m_dwSkillID;
	KTarget m_MouseTarget;
// 	KTarget m_MouseOver;
};

// ����CoolDown�Ĳ�ѯ���
struct KSkillCDGroupResult
{
	//CoolDown��ʱ��(ms)
	DWORD dwTotalTime;
	//CoolDownʣ��ʱ��(ms)
	DWORD dwLastTime;
};

// Tab�ļ���ͷ��Ĭ��ֵ
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

// ��3D��˵Character������
enum enumCharacterTypeTo3D
{
	enumCTT3D_MainPlayer,		//�����
	enumCTT3D_OtherPlayer,		//�������
	enumCTT3D_MainMagicBaby,	//������
	enumCTT3D_OtherMagicBaby,	//��������
	enumCTT3D_Monster,			//Monster
	enumCTT3D_TreasureCase,		//����
	enumCTT3D_Other,			//����
};

// action�����3D
struct KSkillResultTo3D
{
	enumCharacterTypeTo3D m_eSrcType;
	enumCharacterTypeTo3D m_eDestType;
	enum
	{
		enumKRT_HP,					//��ʾѪֵ
		enumKRT_MP,					//��ʾħֵ
		enumKRT_Absorb,				//��ʾ�����ա�
		enumKRT_Immunity,			//����
		enumKRT_Counteract,			//����
		enumKRT_MagicCounteract,	//�����ֿ�
		enumKRT_ReceiveHigh,		//��Ѫ�������ɹ�
		enumKRT_ReceiveMid,			//��Ѫ�������ɹ�
		enumKRT_ReceiveLow,			//��Ѫ�������ɹ�
		enumKRT_ReceiveFail,		//����ʧ��
		enumKRT_Relation,			//���ܶ�
	}m_eActionResultType;

	enum
	{
		enumKRT_Normal,		//��������
		enumKRT_Deadliness,	//����һ��
		enumKRT_IgnoreDep,	//����һ��
	}m_eHowToActionResult;

	enum ENUM_CLASSFICATION
	{
		enumKRT_UnknownClass,	//δ֪����
		enumKRT_Attack,	//����
		enumKRT_Heal,		//����
		enumKRT_Debuff,	//�������
		enumKRT_BeAbsorbedHP,	//����ȡHP
		enumKRT_BeAbsorbedMP,	//����ȡMP
		enumKRT_AbsorbHP,	//��ȡHP
		enumKRT_AbsorbMP,	//��ȡMP
		enumKRT_AbsorbFaith,	//�����Ϊ
	}m_eActionClassification;

	BOOL m_bIsDot;

	DWORD m_dwSrcID;		//A��ID
	DWORD m_dwDestID;		//D��ID
	DWORD m_dwSkillID;		//SkillID
	BYTE m_byActionOrder;	//Action��Skill�ĵڼ���Action
	INT m_nValue;			//Action�����ֵ��ֻ��m_eHurtResultType == enumKRT_HP/MPʱ��Ч

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

		m_dwSrcID = 0;			//A��ID
		m_dwDestID = 0;			//D��ID
		m_dwSkillID = 0;		//SkillID
		m_byActionOrder = 0;	//Action��Skill�ĵڼ���Action
		m_nValue = 0;			//Action�����ֵ��ֻ��m_eHurtResultType == enumKRT_HP/MPʱ��Ч
		m_bIsDot = 0;
	}

	//���ݽ�������ݵõ���Ҫ�����MsgID
	INT ParseMessageID()
	{
		bool bMainPlayer = false;
		bool bMainMagicBaby = false;	//Ҳ�����ٻ���
		if(m_eDestType == enumCTT3D_MainPlayer)
			bMainPlayer = true;
		if(m_eDestType == enumCTT3D_MainMagicBaby)
			bMainMagicBaby = true;
		if((m_eActionClassification == enumKRT_BeAbsorbedMP || m_eActionClassification == enumKRT_AbsorbMP)
			&& m_eDestType == enumCTT3D_TreasureCase) //�����ħֵ�仯����ʾ
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

		if(IsImmunity()) // ���������
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
				if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
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
			if(m_eActionClassification == enumKRT_Attack) // ����ǹ���
			{
				if(IsCounteract())			//����
				{
					if(bMainPlayer)
						return 50538;
					else if(bMainPlayer)
						return 57040;
					else
					{
						if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
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
							if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
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
						if(m_eHowToActionResult == enumKRT_Normal) //��������
						{
							if(bMainPlayer)
								return 50535;
							else if(bMainMagicBaby)
							{
								return 57037;
							}
							else
							{
								if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
								{
									return 50568;
								}
								else
								{
									return 50501;
								}
							}
						}
						else if(m_eHowToActionResult == enumKRT_Deadliness)//����һ��
						{
							if(bMainPlayer)
								return 50536;
							else if(bMainMagicBaby)
								return 57038;
							else
							{
								if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
								{
									return 50569;
								}
								else
								{
									return 50502;
								}
							}
						}
						else if(m_eHowToActionResult == enumKRT_IgnoreDep)//����һ��
						{
							if(bMainPlayer)
								return 50537;
							else if(bMainMagicBaby)
								return 57039;
							else
							{
								if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
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
			else if(m_eActionClassification == enumKRT_Heal)	//����
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
						if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
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
					if(m_eHowToActionResult == enumKRT_Normal) //��������
					{
						if(bMainPlayer)
							return 50543;
						else if(bMainMagicBaby)
							return 57047;
						else
							return 50519;
					}
					else if(m_eHowToActionResult == enumKRT_Deadliness)//����һ��
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
			else if(m_eActionClassification == enumKRT_Debuff)	//�������
			{
				if(IsCounteract())			//����
				{
					if(bMainPlayer)
						return 50545;
					else if(bMainMagicBaby)
						return 57050;
					else
					{
						if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
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
			else if(m_eActionClassification == enumKRT_BeAbsorbedHP)  //������Ѫ
			{
				if(IsMagicCounteract())			//�ֿ�
				{
					if(bMainPlayer)
						return 50548;
					else if(bMainMagicBaby)
						return 57053;
					else
					{
						if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
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
							if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
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
							if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
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
			else if(m_eActionClassification == enumKRT_BeAbsorbedMP)  //������ħ
			{
				if(IsMagicCounteract())			//�ֿ�
				{
					if(bMainPlayer)
						return 50550;
					else if(bMainMagicBaby)
						return 57056;
					else
					{
						if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
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
							if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
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
							if(m_eSrcType == enumCTT3D_MainMagicBaby) //��������Ƿ������
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
// ������Ʒ��ӡ�ɾ�������ṹ
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
//����ϢLOGIC_MAIN_PLAYER_SKILL_START_COOLINGʹ��
struct KSkillCDInfo
{
	DWORD m_dwSkillID;	//����ID
	DWORD m_dwSkillCDGroupID;	//������ȴ��ID
	DWORD m_nTotalTime;	//��ʱ��(ms)
	DWORD m_nLastTime;	//ʣ��(ms)
	BYTE m_byType;		// ����ӵ��������
	enum
	{
		enumCommonSkill,
		enumItemSkill,
	} m_skillType;
};

/// ֪ͨ����������
struct KMouseStateInfo
{
	BOOL m_idHold;		// true ���� false ���Ǹ������ͼƬ
	char szImageName[MAX_ITEM_NAME_LEN];
	int nImageID;		// ����holdʱֻ���֪ʲô���ͣ������Լ�ѡ����Դ
};


// ���״̬���ݽṹ
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

// ͼ������
enum enumIconType
{
	enumIT_Normal = 0,	// ����ͼ��
	enumIT_Brown,		// ���ͼ��
	enumIT_Invalid,		// ��ͼ��
};

// ����ͼ��仯֪ͨ
struct KSkillIconAnnoucement
{
	BYTE m_nWhosIcon;			//˭��ͼ�꣨���or��������Ҫ�仯����ӦKWorldObj�����eWorldObjectKind����
	DWORD m_dwSkillID;			//Ҫ�ı�ļ���id
	enumIconType m_eIconType;	//Ҫ��ɵ�ͼ������
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

// ��Ϸ�������������ش�
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

//�õ���ǰ���ܵ���ʾ״̬
struct KSkillState
{
	BYTE m_byType;		// ����ӵ��������,wokPlayer/wokMagicBaby
	KSkillCDGroupResult skillCDResult;	//CoolDown��Ϣ
	enumIconType iconType;				//ͼ����Ϣ
	KSkillState() : m_byType(0){}
};

#pragma pack(push, 1)
//Action�����˺���ħ���˺��Ľ��
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
	// �Ƿ�����
	DWORD	m_bIsImmunity : 1;
	// �Ƿ����
	DWORD	m_bIsEvation : 1;
	INT		m_nHPRealIncrease : 30;
	// �Ƿ�����
	DWORD	m_bIsDeadliness : 1;
	// �Ƿ����
	DWORD	m_bIsIgnoreDep : 1;
};

//Action����Buff�Ľ��
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
	 * ����Buff�Ľ��
	 * 0 �ֿ�
	 * 1 �ɹ�
	 */
	BYTE	m_byteAddBuffResult	:	1;
	// ����
	BYTE	m_byStacks			:	7;
	// ����ʱ��
	DWORD	m_dwDuration;
	// Ϊ0����Buff��ص�����û������
	WORD	m_wBuffID;
	// ʩ����
	DWORD	m_dwSrcID;
};

//Actionһ����
struct KActionCommonResult
{
	/*
	 * 0. ����
	 * 1. �ɹ�
	 * 2. ʧ��
	 * 3. ��Ѫ�����ɹ�
	 * 4. ��Ѫ�����ɹ�
	 * 5. ��Ѫ�����ɹ�
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

//����Action�Ľ��
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

	//����ֵ
	INT		m_nSrcVal : 29;
	//���յĶ�����0 HP��1 MP��2 RP��3 ������
	DWORD	m_bySrcValType : 3;
	//������ֵ
	INT		m_nDestVal : 29;
	//�����Ķ�����0 HP��1 MP��2 RP��3 ������
	DWORD	m_byDestValType : 3;
};

//�ظ�����Action�Ľ��
struct KActionRestoreEnergyResult
{
	void Fill(BYTE valType, INT val)
	{
		m_byValType = valType;
		m_nVal = val;
	}
	int getVal() const {return m_nVal;}
	int getValType() const {return m_byValType;}

	//�ظ�ֵ
	INT		m_nVal : 29;
	//�ظ��Ķ�����0 HP��1 MP��2 RP��3 ������
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
	bool m_bIsDot;			// �Ƿ���dot
	int m_dwSrcID;			// A��ID
	int m_dwDestID;			// D��ID
	int m_dwSkillID;		// SkillID
	int m_byActionOrder;	// Action��Skill�ĵڼ���Action
	int actionType;			// Action����
	KActionReadResult m_Result;
};

// ��character��������Ұ�е�ʱ��Ҫͬ����buff��Ϣ
struct KAddBuffWhenAppearInfo
{
	DWORD	dwSrcID;
	DWORD	dwBuffID;
	INT		nRemainTime;	//buffʣ��ʱ��
	DWORD	dwElapseTime;	//buff����ʱ��
	BYTE	byStacks;		//���Ӳ���
};

//�趨�������λ������
struct SetShortcutInfo
{
	BYTE	byPosID;	//λ��ID
	KShortcutData shortcutData;
};

#pragma pack(pop)

// ����ʹ�ù����е���Ϣ
struct KSkillInfoTo3D
{
	// ����id
	DWORD m_dwSkillID;
	// ���ܵ����ٶ�
	DWORD m_dwMissileSpeed;
	// ���ܼ��ٱ�
	FLOAT m_fActionAccRate;
	// �������֣��Ժ��Ƿ�ʹ��ID����
	const char* m_szSkillName;
	// ��ʱ��
	DWORD m_dwTotalTime;
	// ����Ҫ��ʾ������
	const char* m_szExtraInfo;
	KSkillInfoTo3D()
		: m_dwSkillID(0)
		, m_dwMissileSpeed(0)
		, m_fActionAccRate(0.0f)
		, m_szSkillName("")
		, m_dwTotalTime(0)
		, m_szExtraInfo(""){}
};

// ����ʹ�ù����еĴ������¼�(����)
struct KSkillEventTo3D
{
	// ����event id
	DWORD m_dwEventID;
	// ���ܵ����ٶ�
	DWORD m_dwMissileSpeed;
};

// 3DҪ����ʱ����SkillResult
struct KSkillResultRequireBy3D
{
	// ԴID
	DWORD	dwSrcID;
	// ��Shoot��ʼ����ʱ
	DWORD	dwDelayTime;
	// Action�����
	BYTE	byActionOrder;
	// ����ID 
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
	BOOL	bForceBreathe;	// �Ƿ�ǿ�ƺ���
	BOOL	bTalkSwitch;	// �Ƿ�������
	DWORD	m_dwRelatedCharID;	// ������ɫid
	DWORD   GroupTypeID;		// ������ID(40000+)
	DWORD   GroupInstanceID;	// ������(1-n)
	BYTE	TeamID;				// ������ID(1-8)
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
 * Warning: ���ڰ�ȫ���صĿ��ǣ����Բ�����Client���������İ�
 *
 * ͨ�ñ䳤��
 *
 * ��1�����ͷ�д�룬ͬ��:
 * KCommonPacket cp;
 * cp << (INT)123 << (INT)456;	//���ͷ�д��INT�����ָ��д�����ݵ�����
 * cp.WriteString("Hello!");	//���ͷ�д��string
 * pDestChar->SyncData(s_nSomePacketCode, cp, cp.GetSendLength(), eSyncAllSightPlayer);
 *
 * ��2��ͬ�������շ�:
 * //�õ�������
 * KCommonPacket cp(pPacket, nPacketLength);
 * //Ȼ���cp��������ĵط�����ֱ�Ӷ���
 * INT m, n;
 * cp >> m >> n;				//���շ�����INT
 * char buff[1000];
 * cp.ReadString(buff, 1000);	//���շ�ֱ�Ӷ���string
 */
#define COMMON_PACKET_USE_DYNAMIC_MEMORY
struct KCommonPacket	//ͨ�ñ䳤��
{
	//������
	static const DWORD MAX_PACKET_LENGTH = 10 * (1 << 10);
	//��ͷ��
	static const DWORD HEAD_SIZE = sizeof(DWORD);
	//BUFFER��
	static const DWORD BUFF_SIZE = MAX_PACKET_LENGTH - HEAD_SIZE;
	CHAR*	m_chBuff;						//����ָ��
	DWORD	m_dwBuffSize;					//�����С
	DWORD	m_dwHeadPos;					//��������ָ��
#ifndef COMMON_PACKET_USE_DYNAMIC_MEMORY
	static CHAR staticBuff[MAX_PACKET_LENGTH];
#else
	CHAR	selfBuff[MAX_PACKET_LENGTH];
#endif

private:
	BOOL	m_bNeedDelete;					//�Ƿ���KCommonPacket���������Ĵ������ͷ�
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
	//��const void*��ʼ��һ���������շ��ã�
	KCommonPacket(const void* pPacket, DWORD packLength)
	{
		m_bNeedDelete = FALSE;
		m_dwHeadPos = 0;
		if (packLength < HEAD_SIZE)
		{
			ASSERT_I(FALSE);//������
			m_dwBuffSize = 0;
			return;
		}
		memcpy(&m_dwBuffSize, pPacket, HEAD_SIZE);
		if (m_dwBuffSize > packLength - HEAD_SIZE || m_dwBuffSize > BUFF_SIZE)
		{
			ASSERT_I(FALSE);//�����󣬳��ȹ���
			m_dwBuffSize = 0;
			return;
		}
		m_chBuff = (char*)((char*)pPacket + HEAD_SIZE);
	}

	//�����Ƿ�������
	inline BOOL IsEof() const
	{
		return m_dwHeadPos >= m_dwBuffSize;
	}
	//����λ
	inline void Reset()
	{
		m_dwBuffSize = 0;
		m_dwHeadPos = 0;
		memcpy(m_chBuff - HEAD_SIZE, &m_dwBuffSize, sizeof(m_dwBuffSize));
	}
	//�õ����ͳ���
	inline DWORD GetSendLength() const
	{
		if (m_dwBuffSize == 0)
		{
			return 0;
		}
		return (HEAD_SIZE + m_dwBuffSize);
	}
	//�õ�Buff�ĳ���
	inline DWORD GetBuffLength() const
	{
		return m_dwBuffSize;
	}
	//�õ���û�д�������ݵĳ���
	inline DWORD GetDataLength() const
	{
		return m_dwBuffSize - m_dwHeadPos;
	}
	//�õ�����д���ʣ�೤��
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

//�ڴ�copy����������Ķ��� for KCommonPacket
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

//�ڴ�copy�����������Ķ��� for KCommonPacket
template<typename TYPE>
KCommonPacket& operator>>(KCommonPacket &aStream, TYPE& obj)
{
	if (aStream.m_dwHeadPos + sizeof(TYPE) > aStream.m_dwBuffSize)
	{
		ASSERT_I(FALSE);/*��������*/
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
 * �����������������1��2�ķ�������1 = (x1, y1)->(x0, y0)������2 = (x2, y2)->(x0, y0)
 * Param:
 *		in		x0, y0:	��ͬ�������
 *		in		x1, y1:	����1���յ�����
 *		in		x2, y2:	����2���յ�����
 * Return:
 *		�������1������2���Ҳࣨ����������ͬ���෴��������TRUE������FALSE
 */
BOOL _Side(FLOAT x0, FLOAT y0, FLOAT x1, FLOAT y1, FLOAT x2, FLOAT y2);

/**
 * �ж�һ�����Ƿ���һ��������
 * Param:
 *		in		testPtX, testPtY:	���Ե�����
 *		in		fx, fy:				���Ծ��ζ������꣬Ҫ��㰴˳ʱ�뱣��
 * Return:
 *		���testPt�ھ����У������߽��ϣ�����TRUE������FALSE
 */
BOOL _IsPtInRectangle(FLOAT testPtX, FLOAT testPtY, const FLOAT fx[4], const FLOAT fy[4]);

struct KBuffInfoToUI
{
	DWORD			m_dwBuffID;
	CHAR			m_szBuffName[32];
	CHAR			m_szBuffIcon[MAX_PATH];
// 	INT				m_nBuffLevel;		// Buff�ȼ�
	INT				m_nBuffPriority;	// Buff���ȼ�
	DWORD			m_dwBuffDuration;	// BuffĬ�ϳ���ʱ��
	BYTE			m_byBuffType;		// Buff������ 0 Good, 1 Bad, 2 Hidden

	INT				m_nDotInterval;		// DOT���ʱ��
	INT				m_nAntiDispelRate;	// Buff����ɢ��
	INT				m_nBuffClass;		// Buff����
	INT				m_nBuffCurrentStacks;		// buff���Ӳ���
	INT				m_nBuffStacks;		// buff���Ӳ�������
	INT				m_nBuffToolTipID;	// Buff��ToolTip
	DWORD			m_dwStatus;			// ��ʶBuff�쳣״̬
	INT				m_nShowLastTime;	// �Ƿ���ʾʣ��ʱ��
	struct 
	{
		DWORD		m_dwValueInfoCount; // ����Action�ĸ���
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

//LogicWorld�趨��Lua�ص��ӿ���Ϣ
struct KScriptCallbackFuncFormat
{
	const char* funcName;	//�ű�������
	void* funcPtr;			//������ַ
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

//����ϵͳ�õ����ݣ�UI���߼���
struct KFriendMsgDataToLogic
{
	//����ϵͳ��Ϣö��(�߼���UIͨ����)
	enum ENUM_FRIEND_SYSTEM_MSG
	{
		enumFSMA_ReqRefresh		= 0,	//ˢ����ʶ�б�
		enumFSMA_ReqAdd			= 1,	//��ʶ��������
		enumFSMA_ReqDelete		= 2,	//��ʶɾ��
		enumFSMA_ReqModify		= 3,	//��ʶ�޸�
		enumFSMA_SearchPlayer	= 4,	//����Player
		enumFSMA_GetCount		= 5,	//�õ��б���Ŀ��

		enumFSMF_ReqRefresh		= 100,	//ˢ�º����б�
		enumFSMF_ReqAdd			= 101,	//������������
		enumFSMF_ReqDelete		= 102,	//����ɾ��
//		enumFSMF_ReqModify		= 103,	//�����޸�
		enumFSMF_SearchPlayer	= 104,	//����Player
		enumFSMF_ConfirmAdd		= 105,	//ȷ��ɾ���Ļظ�
		enumFSMF_GetCount		= 106,	//�õ��б���Ŀ��
		enumFSMF_GetPlayer		= 107,
        enumFSMF_SetAutoRefuse,// �Զ��ܾ���������
        enumFSMF_GetAutoRefuse,// �Զ��ܾ���������

		enumFSMB_ReqRefresh		= 200,	//ˢ�º������б�
		enumFSMB_ReqAdd			= 201,	//��������������
		enumFSMB_ReqDelete		= 202,	//������ɾ��
//		enumFSMB_ReqModify		= 203,	//�������޸�
		enumFSMB_SearchPlayer	= 204,	//����Player
		enumFSMB_GetCount		= 205,	//�õ��б���Ŀ��

		enumFSME_ReqRefresh		= 300,	//ˢ�³����б�
		enumFSME_ReqDelete		= 302,	//����ɾ��
		enumFSME_GetCount		= 305,	//�õ��б���Ŀ��
        
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
			INT m_level;					// 1, �ȼ�
			INT m_career;					// ְҵ
			INT m_subCareer;				// ��ְ
			INT m_friendRank;				// 1, ����Ʒ�����ƺţ���Rankӳ��
			DWORD m_joinTime;				// 4, ������ѵ�ʱ��
			BOOL m_onlineState;				// ����״̬
		} m_getPlayer;
	};
};

//����ϵͳ�õ����ݣ��߼���UI��
struct KFriendMsgDataToUI
{
	//����ϵͳ��Ϣö��(�߼���UIͨ����)
	enum ENUM_FRIEND_SYSTEM_MSG
	{
		enumFSMA_Clear		= 0,	//�����ʶ�б�
		enumFSMA_Add		= 1,	//��ʶ��������
		enumFSMA_Delete		= 2,	//��ʶɾ��
		enumFSMA_Modify		= 3,	//��ʶ�޸�

		enumFSMF_Clear		= 100,	//��պ����б�
		enumFSMF_Add		= 101,	//������������
		enumFSMF_Delete		= 102,	//����ɾ��
		enumFSMF_Modify		= 103,	//�����޸�
		enumFSMF_AskConfirmAdd	= 104,	//ȷ���Ƿ����Ӻ���

		enumFSMB_Clear		= 200,	//��պ������б�
		enumFSMB_Add		= 201,	//��������������
		enumFSMB_Delete		= 202,	//������ɾ��
		enumFSMB_Modify		= 203,	//�������޸�

		enumFSME_Clear		= 300,	//��ճ����б�
		enumFSME_Add		= 301,	//������������
		enumFSME_Delete		= 302,	//����ɾ��
		enumFSME_Modify		= 303,	//�����޸�

		enumFSMA_RefreshNum	= 1000,	//ˢ��������ʾ
		enumFSMF_RefreshNum,	//ˢ��������ʾ
		enumFSMB_RefreshNum,	//ˢ��������ʾ
		enumFSME_RefreshNum,	//ˢ��������ʾ
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
//	DWORD											m_dwMapID;							// ��ͼ ID
//	KSTRING											m_strName;							// ��ͼ����
//	KSTRING											m_strWorldFile;						// �߼���ͼ�ļ�·��
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
//	KSTRING											m_strL2MapFile;						//Ѱ·�õĶ�����ͼ·��
//#endif
//};
//
//struct KWorldInfoForGame
//{
//	DWORD m_dwWorldInfoID;	// ָ�� KWorldInfoInGame ����
//	DWORD m_dwCount;		// ������͵� World �ж��ٸ� ?
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


//ս�����
struct KBattleFieldInfo
{
	CHAR		m_strBFName[256];		//ս������
	DWORD		m_dwMapID;				//ս����Ӧ��ͼID
	DWORD		m_dwWorldID;			//ս����ӦWorldID
	DWORD		m_dwBattleFieldTypeID;	//ս������
	BOOL		m_bOpen;				//ս���Ƿ񿪷�
	DWORD		m_dwMaxPlayer;			//ս�������������
	DWORD		m_dwCurrPlayer;			//��ǰս������
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
		//�䷽�������
		enumLTL_QueryFormulaBoardUpdateFormulaTree,
		enumLTL_QueryFormulaBoardUpdateCurrentFormulaInfo,
		enumLTL_GetTotalItemCount,

		enumLTL_EnhanceFormula,
		enumLTL_ForgetFormula,

		//��¯�������
		enumLTL_QueryHoldFurnace,
		enumLTL_QueryStartWorking,
		enumLTL_WorkCommand,

		//���������ˢ��
		enumLTL_QueryUpdateLifeSkillInfo,
		enumLTL_CanEnhanceFormulaCheck,
		enumLTL_QueryAssist,

		//��������ת��
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
		//�䷽�������
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

// 3D tooltip �ṹ
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
	int	nNpcID;		// npcʵ��ID
	int	nNpcTypeID;	// npc����ID
	int nLevel;		// npc�ȼ�
	int nNameID;	// npc�����ַ���id
	int nTitleID;	// npc�ƺ��ַ���id
	int nRaceID;	// npc�����ַ���id
	int nELevelID;	// npc��Ӣ�ȼ��ַ���id
	int nOwnForceID;// npc�����ַ���id
	int nDisplayID;	// npc��ʾ��Ϣ
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

// ��FightLog��˵Character������
enum ENUM_CHAR_TYPE_TO_FIGHT_LOG
{
	// 0.	δ֪
	enumCTTFL_Unknown = 0,
	// 1.	�Լ�
	enumCTTFL_MainPlayer,
	// 2.	����
	enumCTTFL_MainMagicBaby,
	// 3.	����
	enumCTTFL_TeamMate,
	// 4.	�ж�NPC
	enumCTTFL_EnemyNPC,
	// 5.	�ж����
	enumCTTFL_EnemyPlayer,
	// 6.	����
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

//����ѧϰ���ܵ�һ����Ϣ
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
		enum_LearnBar,	//��ѧ�����λ
		enum_EquipBar,	//װ����λ
		enum_UniqueBar,	//ר��������λ
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

// �̳ǵ���
struct KBillingItemInfo 
{
	KBillingItemInfo()
	{
		memset(this, 0 ,sizeof(KBillingItemInfo));
	}

	int  nIndex;			// ���
	BYTE bySchoolType;		// ������
	BYTE byClassType;		// С����
	WORD wItemID;			// ��ƷID
	WORD wItemNum;			// ����
	WORD wPrice;			// �۸�
	BYTE byAgio;			// �ۿۼ۸� 90����9�� 85����85��
	ENUM_MONEY_DEFINE nMoneyType;		// ���ۻ��ҵ�λ

	enum ENUM_ITEM_FLAG
	{
		enum_Nothing	= 0,
		enum_Want_To_Become_Strong		= 0x0001,		// ��Ҫ��ǿ��
		enum_Want_To_Become_Fashion		= 0x0002,		// ��Ҫ������
		enum_Want_To_Become_MagicBaby	= 0x0004,		// ��Ҫ�÷���
		enum_Want_To_Become_LevelUp		= 0x0008,		// ��Ҫ������
	};
	BOOL EqualFlag(ENUM_ITEM_FLAG eFlag)
	{
		if (eFlag & nItemFlag)	return TRUE;
		return FALSE;
	}
	int nItemFlag;			// ��Ʒ�ı�־λ����������
};
DECLARE_SIMPLE_TYPE(KBillingItemInfo)

struct KBillingFreeItem
{
	KBillingFreeItem()
	{
		memset(this, 0 , sizeof(KBillingFreeItem));
	}
	int  nIndex;			// ���
	WORD wMaleItemID;		// ������ѵ���
	WORD wMaleItemNum;		// ������ѵ�������
	WORD wFemaleItemID;		// Ů����ѵ���
	WORD wFemaleItemNum;	// Ů����ѵ�������
};
DECLARE_SIMPLE_TYPE(KBillingFreeItem)

// ��ֵ��¼
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
	INT m_fX , m_fY;		// ������INT���㹻��
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
	BOOL bFreeExp;		// �Ƿ�ƽ�����侭��
	BOOL bFreeItem;		// �Ƿ����ɷ�����Ʒ
	BOOL bcAdd;			// �Ƿ��������
	BOOL bcUnit;		// �Ƿ�����ϲ�
	INT	 nDiceLevel;	// ɸ�Ӽ���
	BOOL bOccupation;	// ְҵ����
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

//--------------- ��������

struct KMagicBabyEgg
{
	DWORD dwEggID;			// ID
	DWORD dwOwnerID;		// ������ID
	TIME_T nIncubateTime;	// ��ʼ����ʱ��
	long  nType;			// ����ID
	DWORD dwRemineTime;		// ʣ��ʱ��
	char  szOwnerName[MAX_NAME_DEF - 1];
	BYTE  byIsAdult;		// �Ƿ�������
	BYTE  byNation;			// ����
	TIME_T freeTime;			// �ͷ�ʱ��
	TIME_T durTime;			// ����ʱ�� 
	TIME_T lostTime;		// ����ʱ��
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
// һ��·�����������1024���ڵ㣬�����ֽ��ڱ༭�����߼�����ʱ���ޣ�����ʱ������ֵ��������ֵ���Դ��ݵ���ʱ������Ӱ�죬
// �������洢�ṹΪKVector���������Ӱ�졣
#define	MAX_TRAFFIC_WAYPOINT_COUNT	1024

struct KTransportInfo
{
	//DWORD	dwDriverID;			// ˾��ID�����=TRANSPORT_INVALID_DRIVER, ���ʾ�����˼�ʻ�ĳ�
	INT		nTypeID;
	INT		nRouteIndex;		// ·��ID������jiaʻʱʹ��
	INT		nMaxPassenger;
	DWORD	dwPassenger[TRANSPORT_MAX_PASSENGER];	//�˿�ID

};

struct KTrafficWaypoint
{
	float fX;
	float fY;
	float fZ;
	float fSpeed;
	float fAngle[4];
	float fTime;	// ����ʼ�㵽�õ��ƫ��ʱ��
	WORD  wEventID;
	WORD  wStationID;
	BYTE  byFlyModelAndStation;	// ����ģʽ��ʶλ | ��վվ���ʶλ
	BYTE  byCanGetOn : 1;			// �Ƿ����������ϳ�
	BYTE  byCanGetOff : 1;			// �Ƿ����������ϳ�
	BYTE  byResv : 6;			// �Ƿ����������ϳ�
	BYTE  byReserve[2];			// ����λ
};

struct KTrafficWaypointNew
{
	wPoint mPoint;
	float fSpeed;
	const char * szTag;
};

struct KRouteBaseInfo
{
	char szName[MAX_TRAFFIC_ROUTE_NAME];	// ·������
	WORD nBusTypeID;	// ��ͨ��������
	BYTE byIsWalk;		// �Ƿ�������·��
	BYTE byIsStraightaway; // �Ƿ���ֱ����·
	WORD nSeatCount;	// ��λ����
	BYTE byCycleLine;	// �Ƿ���ѭ����·
	BYTE byMaxCount;	// �������������
	WORD wInterval;		// �������,����ǻ��ߣ�����Ը�����·���Ⱥ������������
	WORD wTotalInterval;// ��ʱ������ÿ��wTotalInterval������byMaxCount��������Щ������һ˲�䷢��ȥ�ģ������������wInterval����ȥ
	int	nWaypointCount;	// �ڵ�����
	float fTotalTime;	// ��������·�ߵ���ʱ��
};

struct KTrafficRouteInfo
{
	KRouteBaseInfo		Info;		// ·�ߵĻ�����Ϣ
	KTrafficWaypoint	Waypoints[MAX_TRAFFIC_WAYPOINT_COUNT];	// �ڵ�����
};

struct KTrafficRouteOperator
{
	int nRouteIndex;
	char szName[MAX_TRAFFIC_ROUTE_NAME];
	KRouteBaseInfo		Info;		// ·�ߵĻ�����Ϣ
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
	float fAngle;		// ������б�Ƕ�
};


struct KTrafficDeparture
{
	WORD	nCount;					// �ѷ��ĳ���
	DWORD	dwLastTime;				// ���һ�η���ʱ��
	DWORD	dwFirstTime;			// ����һ������ʱ��
};

typedef System::Collections::KVector<KTrafficWaypoint> KWaypointsVector;
typedef System::Collections::KVector<int> KTransportVector;

struct KTrafficRoute
{
	KRouteBaseInfo		Info;		// ·�߻�����Ϣ
	KWaypointsVector	Waypoints;	// �ڵ�����
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
	//		��½ϵͳ
	//////////////////////////////////////////////////////////////////////////
	DWORD	m_LoginCount;			// ��½����
	DWORD	m_LogoutCount;			// �ǳ�����
	DWORD	m_DisconnectCount;		// ��������
	DWORD	m_ErrorAccountCount;	// �ʺŴ������
	DWORD	m_ErrorPasswordCount;	// ����������

	//////////////////////////////////////////////////////////////////////////
	//		��ɫ����
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		ս��ϵͳ
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		����ϵͳ
	//////////////////////////////////////////////////////////////////////////
	INT64	m_AddMoney;				// ��Ϸ������
	INT64	m_CostMoney;			// ������Ϸ��

	//////////////////////////////////////////////////////////////////////////
	//		��ͨ����
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		����ϵͳ
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		����ϵͳ
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		��Ʒϵͳ
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		����ϵͳ
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		����ϵͳ
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//		�ʼ�ϵͳ
	//////////////////////////////////////////////////////////////////////////
	INT		Mail_SendOK;		// �ʼ����ͳɹ�����
	INT		Mail_SendFail;		// �ʼ�����ʧ�ܴ���
	INT		Mail_FetchOK;		// �ʼ���ȡ�ɹ�����
	INT		Mail_FetchFail;		// �ʼ���ȡʧ�ܴ���
	INT		Mail_SendGuild;		// �ʼ����Ͱ����ʼ�
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

///////////////////////////////////�ʼ����/////////////////////////////////
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
	enum		{FIX_PART_LEN = 90,};					//MP_BUFF֮ǰ�ĳ���
	INT64		MP_MailID;								//�ʼ�ID				BIGINT(8)
	DWORD		MP_SenderID;							//������ID				INT (4)
	char		MP_SenderName[MAIL_PLAYER_NAME + 1];	//����������			CHAR(20)
	DWORD		MP_ReceiverID;							//�ռ���ID				INT(4)
	char		MP_ReceiverName[MAIL_PLAYER_NAME + 1];	//�ռ�������			CHAR(20)
	INT64		MP_ReceiveTime;							//ʱ������				TIMESTAMP
	BYTE		MP_IsRead;								//�ʼ���ȡ״̬(�Ѷ�/δ��)	TINYINT(1)
	BYTE		MP_IsFetch;								//�Ѳ���				TINYINT(1)
	BYTE		MP_MailType;							//�ʼ�����(��ͨ/����/��Ʒ/ϵͳ)	TINYINT(1)
	BYTE		MP_AttachItemType;						//��Ʒ����(ItemBase/ItemID)		TINYINT(1)
	DWORD		MP_FeeAmount;							//��������				INT(4)
	DWORD		MP_AttachMoney;							//���ӽ�Ǯ����			INT(4)
	int			MP_HeadLen;								//�ʼ�����				VARCHAR(20)
	int			MP_BodyLen;								//�ʼ���Ϣ				TEXT
	int			MP_AttachmentLen;						//�ʼ�������Ʒ		BIGINT(8)
	//ʵ��Buffer��С�����������ݵ�ʵ�ʳ��ȵó���
	//|Head| + '\0' + |Body| + '\0' + |Attachment|
	//�ڷ�������ʱ��Ҳ���뱣֤����Head, Body, Attachment��˳�����η���
	char		MP_BUFF[MAX_BUFF_MAIL_HEAD + MAX_BUFF_MAIL_BODY + MAIL_ATTACHMENT_LEN];
	static int GetUsefulLen(const Mail_Packet& packet)
	{
		int nSize = sizeof(Mail_Packet);
		return (FIX_PART_LEN + packet.MP_HeadLen + packet.MP_BodyLen + packet.MP_AttachmentLen);
	}
	//��ȡ��ַ
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
	//��������
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
	//��������Ƿ�����⵽�ƻ�
	bool static CheckIntact(const Mail_Packet& packet)
	{
		//��鳤���Ƿ�Խ�磬����mail��body���ַ����Ƿ���0��β
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
	int attachItemType;							//������Ʒ����(0:KItemBase/1:Mail_Attach_ItemID)
	DWORD feeAmount;							//��������	
	DWORD attachMoney;							//���ӽ�Ǯ����
	int attachmentLen;							//������Ʒ����
	char attachment[MAIL_ATTACHMENT_LEN];		//������Ʒ
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
	//	error_Net_Error,		// �������
	//	error_Db_Error,			// ���ݿ����
	//	error_Sync_In_Part,		// ������������
	//	error_Sync_Accomplish,	// �Ѿ�ͬ������
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
	//	error_Net_Error,		// �������
	//	error_Db_Error,			// ���ݿ����
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
	//	error_Net_Error,		// �������
	//	error_Db_Error,			// ���ݿ����
	//	error_Receiver_No_Space,// �ռ�����������
	//	error_No_Receiver,		// �ռ��˲�����
	//	error_No_Enough_Money,	// �޷��۳��㹻�Ľ�Ǯ
	//	error_Attachment_Error,	// ������Ʒ��������
	//};
	BYTE	result;
	DWORD	playerID;
	char	receiverName[MAIL_PLAYER_NAME];

};

struct Mail_Delete_Rsp
{
	//enum { 
	//	error_Succeed = 0,
	//	error_Net_Error,		// �������
	//	error_Db_Error,			// ���ݿ����
	//	error_Fee_Nonzero,		// ��Ҫ��ȡ���ã�����ֱ��ɾ��
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
	//	error_Net_Error,		// �������
	//	error_Db_Error,			// ���ݿ����
	//	error_Bag_No_Space,		// �����������޿ռ�
	//	error_No_Enough_Money,	// �޷��۳��㹻�Ľ�Ǯ
	//};
	BYTE	result;
	DWORD	playerID;
	INT64	mailID;
};

struct Mail_ReturnToSender_Rsp
{
	//enum { 
	//	error_Succeed = 0,
	//	error_Net_Error,		// �������
	//	error_Db_Error,			// ���ݿ����
	//	error_Type_Unmatch,		// �����ʼ���֧������
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
		//���ֵ
		enumTCFUI_GetVal = 0,
		//���״̬������ֵ��TALENT_POINT_STATE
		enumTCFUI_GetStatus,
		//����츳�ܵ���
		enumTCFUI_GetTotalPoint,
		//����츳���õ���
		enumTCFUI_GetUsedPoint,
		//����츳ʣ�����
		enumTCFUI_GetRemainPoint,
		//����츳ͼ�괮
		enumTCFUI_GetIcon,

		//ѧϰĳ�츳
		enumTCFUI_Learn,
		//ϴ��
		enumTCFUI_Clear,

		//�õ�TalentData
		enumTCFUI_GetTalentData,
		//���ȫ��״̬������ֵΪ2^TALENT_POINT_STATE�����
		enumTCFUI_GetAllStatus,
		//ѯ��ϴ��
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
	int m_skillGraspPointCount;							//��ǰʣ���������
	int m_graspList[MB_SKILLGROUP_MAX];					//������Ϣ�б�
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
		//�츳���ʼ�����
		enumTETUI_Init = 0,
		//ĳ�츳��ѧϰ�ɹ�������Ϊѧϰ�ɹ����츳��
		enumTETUI_Learn,
		//�츳�������仯��
		enumTETUI_TotalPointChange,
		//�츳����ϴ��
		enumTETUI_Clear,
		//תְ
		enumTETUI_ChangeClass,
		//��ת
		enumTETUI_ChangeSubClass,
		//ǿ��ˢ��
		enumTETUI_Refresh,
		//ѯ��ȷ���츳���Ƿ�Ҫ���������Ϊ��Ҫ���ѵ�Ǯ
		enumTETUI_AskClear,
	};
};

struct PACKET_FILE
{
	enum		{PF_PATH_RELATIVE = 0, PF_PATH_POSITIVE};		//MP_BUFF֮ǰ�ĳ���
	int			PF_FileNameLen;									//�ļ����ֳ���
	int			PF_FileStreamLen;								//�ļ�����������
	//ʵ��Buffer��С�����������ݵ�ʵ�ʳ��ȵó���
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

	//��������
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

	//��������Ƿ�����⵽�ƻ�
	bool static CheckIntact(const PACKET_FILE& packet, int nLen)
	{
		//��鳤���Ƿ����
		return	( GetUsefulLen(packet) == nLen );
	}

};

struct KAttrChangeEventParam
{
	enum
	{
		enumACEP_PlayerTalent = 1,	//����츳
		enumACEP_Vigour,			//����
		enumACEP_Energy,			//����
		enumACEP_Effort,			//����
		enumACEP_HPNotFull,			//hp�����䵽����
		enumACEP_HPLess20,			//hp�䵽����20%
		enumACEP_HPLess50,			//hp�䵽����50%
		enumACEP_ExerciseMore10,	//����ֵ�������10%
		enumACEP_EquipOneStar,		//װ���ȼ�һ���ǣ�oldValue��currentValueһ�������ǵ�ǰֵ
		enumACEP_EquipTwoStars,		//װ���ȼ������ǣ�oldValue��currentValueһ�������ǵ�ǰֵ
		enumACEP_EquipThreeStars,	//װ���ȼ������ǣ�oldValue��currentValueһ�������ǵ�ǰֵ
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

// ��Ը������
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
	BYTE			byQuestResult;	// ������ɽ��
	DWORD			dwQuestID;		// ����ID
	BYTE			byGetWishType;	// ��ȡԸ������ 0:Tree, 1:Research
	BYTE			byTreeIndex;	// ��Ը��������λ��0-58
	DWORD			dwWishID;		// Ը��ID
	KWish			wish;
};

#pragma pack(pop)

struct K3DObjectCreateParam
{
	BOOL bMorph;
	DWORD dwMorphID;
	DWORD dwOriginalID;
};

// ս��ͳ����Ϣ���
struct KFightStatisticsLogInfo
{
	enum
	{
		enumFSLI_Damage,	//�˺��������Ӧm_damage
		enumFSLI_Heal,		//�����������Ӧm_heal
		enumFSLI_FightStateChange,	//ս��״̬ת��
	} m_logType;
	union{
		struct{
			const char* pSrcName;			//Դ������
			const char* pSrcMasterName;		//Դ��������ҵ����֣��編�������ˣ��ٻ��޵����˵ȣ���ͬ��
			const char* pDestName;			//Ŀ�������
			const char* pDestMasterName;	//Ŀ���������ҵ�����
			const char* pSkillName;			//������
			int nVal;						//����/�˺�ֵ
		}m_damage, m_heal;
		struct{
			BOOL enterFight;				//ΪTRUEΪ����ս�����뿪ΪFALSE
			const char* pName;				//����ս�����˵�����
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
		max_x = 10,			// ���� ��Ӧ��ĸABCDEFGH
		max_y = 8,			// ���� ��Ӧ����12345678910
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

// ԪӤ����
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


// ���йһ���������Ϣ
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

	DWORD mdwFormationID;		// ����ID
	DWORD mdwMinMember;			// ������Ҫ����
	DWORD mdwMaxMember;			// ���֧������
	DWORD mdwMutuRange;			// ���ⷶΧ ͬʱ��������鷶Χ
	DWORD mdwFormationRange;	// ���ͷ�Χ
	DWORD mdwFormationDelay;	// ���������ӳ�
	DWORD mdwFormationTotalTime;// ������ʱ��
	FLOAT mfNormalExpRate;		// ��ͨ����ֵ�������
	FLOAT mfPerfectExpRate;		// ��������ʱ����ֵ�������
	FLOAT mfPerfect1ExpRate;	// ׼��������ʱ����ֵ�������
	FLOAT mfReductionExpRate;	// ÿ��һ���˵ľ���ֵ�ݼ�����
	DWORD mdwFormationTC;		// ������Ч����ID
	DWORD mdwFormationExpTime;	// ��ͨ״̬�����ͼ�������ֵ��Ҫ��ʱ��0Ϊ���Ӿ���
	::System::Collections::DynArray<DWORD> mPerfectConditionIDs;	// �������͵�����

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
	DWORD mAwardBuffID;				// ����buff
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

// ������
struct KDropGroupParam
{
	VOID	Init()
	{
		wGroupID = 0;
		byQuantity = 0;
		dwProbability = 0;
		dwInfluenceDegree = 0;
	}
	WORD	wGroupID;				// ������ID
	BYTE	byQuantity;				// ��ȡ����
	DWORD	dwProbability;			// ��ȡ����dwProbability/1000000;�����֮n
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
	WORD	byID;				// �������ID
	WORD	wSGCCount;
	POINT	aSGC[MAX_DROP_ITEM_SGC_COUNT];	// ������
	BYTE	byQuantity;				// ��ȡ����
	DWORD	dwProbability;			// ��ȡ����dwProbability/1000000;�����֮n
	WORD	wQualityCount;
	POINT   aQuality[MAX_DROP_ITEM_QUALITY_COUNT];// Ʒ��ɸѡ
	BYTE	byLevelType;			// �ȼ�ɸѡ����
	short	shMinTC;				// ��С�ȼ�
	short	shMaxTC;				// ���ȼ�
};

struct KIdentityBuffTootipInfo
{
	DWORD m_nBuffID;		// BuffID
	DWORD m_nSrcID;			// srcID
	DWORD m_nStacks;		// ���Ӳ���
	DWORD m_nRemainTime;	// ʣ��ʱ��
	DWORD m_dwExpireTime;	// ��ʧʱ�� ����������Ȼʱ��Ϊ����ʱ��ʱ����
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
	enumCLS_Sector,//����
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
