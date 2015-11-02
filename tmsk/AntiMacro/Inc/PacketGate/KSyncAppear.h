#pragma once

#include <System/KType.h>
#include <System/KMacro.h>
#include <KCommonDefine.h>
#include <KCommonStruct.h>

#pragma warning(disable:4200)
#pragma pack(push, 1)

struct  KXyPos
{
	float x;
	float y;

	void SetPos(float fx, float fy)
	{
		x = fx;
		y = fy;
	}
	float X() const
	{
		return x;
	}
	float Y() const
	{
		return y;
	}
};

struct K_AppearBase
{
	BYTE   byDataLen;
	void AppendAttr(int nValue, int nSize)
	{
		char* p = (char*)this + byDataLen;
		memcpy(p, &nValue, nSize);
		byDataLen += nSize;
	}
	int ReadAttr(int& nOffset, int nSize)
	{
		int nValue = 0;
		char* p = (char*)this + nOffset;
		memcpy(&nValue, p, nSize);
		nOffset += nSize;
		return nValue;
	}
	int Size() const { return byDataLen; }
};


struct K_PlayerAppear_1 : public K_AppearBase
{
	KXyPos currPos;				// 玩家当前位置
	//float fZ;
	INT		nHp;
	INT		nMp;
	WORD	nDirection;		// angle*1000
	DWORD nSelectTarget;	// 目标
	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_PlayerAppear_2 : public K_AppearBase
{
	DWORD  byStateID    : 8;	// 对象当前的状态，如 csOnStand ...
	DWORD  byMoveType   : 8;	// 移动方式，如 Walk, Run
	DWORD  byShowWeapon : 1;	// 是否显示武器
	DWORD  byIsCombat   : 1;	// 是否处于战斗状态
	DWORD  byShowFashion: 1;	// 是否显示时装
	DWORD  byCT         : 1;	// CT
	DWORD  byST         : 1;	// ST
	DWORD  reserved		: 11;

	DWORD  dwStateTime;//状态时间
	char   szBuf[1];

	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_PlayerAppear_3 : public K_AppearBase
{
	CHAR	szName[MAX_NAME];		// 玩家的名字
	BYTE	byIsDeath;				// 是否死亡
	BYTE	byLevel;
	char	bySex;					// 性别，男女
	BYTE	byClass;				// 职业
	BYTE	byForce;				// 所属势力
	INT		nMaxHp;					// 最大生命值
	INT		nMaxMp;					// 最大内力值
	WORD	wRunSpeed;				// 走动速度 厘米/秒
	WORD	wWalkSpeed;				// 跑动速度 厘米/秒
	BYTE	byModel;				// 模型(角色模型类型编号)
	SHORT	shSize;					// 模型的缩放比例
	BYTE	byMasterLevel;			// GM权限
	Equipment_3DShow equip_3DShow[enumEE3_Count];	// 通知3D的装备表现，玩家5个部位的装备

	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_NpcAppear_1 : public K_AppearBase
{
	KXyPos currPos;
	float  fAngle;
	WORD nDirection;
	DWORD nSelectTarget;	// 目标

	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_NpcAppear_2 : public K_AppearBase
{
	INT  mnCurrentHP;
	INT  mnCurrentMP;
	DWORD byStateID  : 8;	// 活动状态ID
	DWORD byMoveType : 8;	// 移动方式
	DWORD byIsCombat : 1;
	DWORD byIsFly    : 1;
	DWORD byIsQuest	 : 1;	// NPC是否有任务
	DWORD byCT       : 1;
	DWORD byST       : 1;
	DWORD reserved   : 11;

	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_NpcAppear_3 : public K_AppearBase
{
	DWORD	dwTypeID;
	BYTE	byIsDeath		: 1;		// 死亡状态
	float	fRelativeHeight;	//相对地面的高度
	INT		nCruiseSpeed;
	INT		nChaseSpeed;
	DWORD	dwFlags;
	INT		mnMaxHP;
	INT		mnMaxMP;
	WORD	nDirectionZ;
	SHORT	shNpcSize;		// 
	BYTE	byForce;		// 势力
	BYTE	byRace;			// 种族
	BYTE	byNpcLevel;		// 等级
	BYTE	byAbility;		// 职业
	DWORD	dwSummonMasterID;		// 主人id
	DWORD	dwRelatedCharID;	// 关联角色id
	DWORD	dwStateTime;	// 活动时间
	BYTE	m_byQuestEff;			// 头顶任务特效
	BYTE	m_byQuestStateValid;	// 特效是否失效 !0未失效 0失效
	BYTE	byVanishLevel;		//隐身等级
	INT		nAntiDetectRange;	//反隐距离
	int		nCurrentWeaponDisplayID;


	STRU_KNpcDisplayBaseInfo m_ModelSize;

	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_TreasureCaseAppear_1 : public K_AppearBase
{
	KXyPos currPos;
	float  fAngle;
	WORD	nDirection;
	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_TreasureCaseAppear_2 : public K_AppearBase
{
	INT  mnCurrentHP;
	INT  mnCurrentMP;
	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_TreasureCaseAppear_3 : public K_AppearBase
{
	DWORD	dwTypeID;
	BYTE	byIsDeath		: 1;		// 死亡状态
	BYTE	bOpened			: 1;		// 开启
	float	fRelativeHeight;	//相对地面的高度
	DWORD	dwHolder;
	DWORD	dwOwner;
	DWORD	dwCreatedTime;
	INT		mnMaxHP;
	WORD	nDirectionZ;
	BYTE	byForce;		// 势力
	BYTE	byRace;			// 种族
	DWORD	dwRelatedCharID;		// 主人id

	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};


struct K_GroundItemAppear_1 : public K_AppearBase
{
	KXyPos currPos;
	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_GroundItemAppear_2 : public K_AppearBase
{
	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_GroundItemAppear_3 : public K_AppearBase
{
	DWORD	dwTypeID;
	BYTE	byQuality;
	BYTE	byBusy;
	WORD	wItemCount;
	DWORD	dwOwner;
	DWORD	dwTeamOwner;
	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_PlayerAppear
{
	enum { eMaxCache = 512};
	char dataBuf[eMaxCache];
	K_PlayerAppear_1*	GetData1()
	{
		return (K_PlayerAppear_1*)&dataBuf[0];
	}
	K_PlayerAppear_2*	GetData2()
	{
		K_PlayerAppear_1* p1 = GetData1();
		int offset = p1->Size();
		return (K_PlayerAppear_2*)&dataBuf[offset];
	}
	K_PlayerAppear_3*	GetData3()
	{
		K_PlayerAppear_1* p1 = GetData1();
		K_PlayerAppear_2* p2 = GetData2();
		int offset = p1->Size() + p2->Size();
		return (K_PlayerAppear_3*)&dataBuf[offset];
	}
	int					Size()
	{
		K_PlayerAppear_1*	p1 = GetData1();
		K_PlayerAppear_2*	p2 = GetData2();
		K_PlayerAppear_3*	p3 = GetData3();
		int nLen = p1->Size() + p2->Size() + p3->Size();
		return nLen;
	}
};

struct K_NpcAppear
{
	enum { eMaxCache = 512};
	char dataBuf[eMaxCache];
	K_NpcAppear_1*	GetData1()
	{
		return (K_NpcAppear_1*)&dataBuf[0];
	}
	K_NpcAppear_2*	GetData2()
	{
		K_NpcAppear_1* p1 = GetData1();
		int offset = p1->Size();
		return (K_NpcAppear_2*)&dataBuf[offset];
	}
	K_NpcAppear_3*	GetData3()
	{
		K_NpcAppear_1* p1 = GetData1();
		K_NpcAppear_2* p2 = GetData2();
		int offset = p1->Size() + p2->Size();
		return (K_NpcAppear_3*)&dataBuf[offset];
	}
	int					Size()
	{
		K_NpcAppear_1*	p1 = GetData1();
		K_NpcAppear_2*	p2 = GetData2();
		K_NpcAppear_3*	p3 = GetData3();
		int nLen = p1->Size() + p2->Size() + p3->Size();
		return nLen;
	}
};

struct K_TreasureCaseAppear
{
	enum { eMaxCache = 512};
	char dataBuf[eMaxCache];
	K_TreasureCaseAppear_1*	GetData1()
	{
		return (K_TreasureCaseAppear_1*)&dataBuf[0];
	}
	K_TreasureCaseAppear_2*	GetData2()
	{
		K_TreasureCaseAppear_1* p1 = GetData1();
		int offset = p1->Size();
		return (K_TreasureCaseAppear_2*)&dataBuf[offset];
	}
	K_TreasureCaseAppear_3*	GetData3()
	{
		K_TreasureCaseAppear_1* p1 = GetData1();
		K_TreasureCaseAppear_2* p2 = GetData2();
		int offset = p1->Size() + p2->Size();
		return (K_TreasureCaseAppear_3*)&dataBuf[offset];
	}
	int					Size()
	{
		K_TreasureCaseAppear_1*	p1 = GetData1();
		K_TreasureCaseAppear_2*	p2 = GetData2();
		K_TreasureCaseAppear_3*	p3 = GetData3();
		int nLen = p1->Size() + p2->Size() + p3->Size();
		return nLen;
	}
};

struct K_GroundItemAppear
{
	enum { eMaxCache = 512};
	char dataBuf[eMaxCache];
	K_GroundItemAppear_1*	GetData1()
	{
		return (K_GroundItemAppear_1*)&dataBuf[0];
	}
	K_GroundItemAppear_2*	GetData2()
	{
		K_GroundItemAppear_1* p1 = GetData1();
		int offset = p1->Size();
		return (K_GroundItemAppear_2*)&dataBuf[offset];
	}
	K_GroundItemAppear_3*	GetData3()
	{
		K_GroundItemAppear_1* p1 = GetData1();
		K_GroundItemAppear_2* p2 = GetData2();
		int offset = p1->Size() + p2->Size();
		return (K_GroundItemAppear_3*)&dataBuf[offset];
	}
	int					Size()
	{
		K_GroundItemAppear_1*	p1 = GetData1();
		K_GroundItemAppear_2*	p2 = GetData2();
		K_GroundItemAppear_3*	p3 = GetData3();
		int nLen = p1->Size() + p2->Size() + p3->Size();
		return nLen;
	}
};

#pragma pack(pop)
