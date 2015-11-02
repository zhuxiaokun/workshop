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
	KXyPos currPos;				// ��ҵ�ǰλ��
	//float fZ;
	INT		nHp;
	INT		nMp;
	WORD	nDirection;		// angle*1000
	DWORD nSelectTarget;	// Ŀ��
	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_PlayerAppear_2 : public K_AppearBase
{
	DWORD  byStateID    : 8;	// ����ǰ��״̬���� csOnStand ...
	DWORD  byMoveType   : 8;	// �ƶ���ʽ���� Walk, Run
	DWORD  byShowWeapon : 1;	// �Ƿ���ʾ����
	DWORD  byIsCombat   : 1;	// �Ƿ���ս��״̬
	DWORD  byShowFashion: 1;	// �Ƿ���ʾʱװ
	DWORD  byCT         : 1;	// CT
	DWORD  byST         : 1;	// ST
	DWORD  reserved		: 11;

	DWORD  dwStateTime;//״̬ʱ��
	char   szBuf[1];

	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_PlayerAppear_3 : public K_AppearBase
{
	CHAR	szName[MAX_NAME];		// ��ҵ�����
	BYTE	byIsDeath;				// �Ƿ�����
	BYTE	byLevel;
	char	bySex;					// �Ա���Ů
	BYTE	byClass;				// ְҵ
	BYTE	byForce;				// ��������
	INT		nMaxHp;					// �������ֵ
	INT		nMaxMp;					// �������ֵ
	WORD	wRunSpeed;				// �߶��ٶ� ����/��
	WORD	wWalkSpeed;				// �ܶ��ٶ� ����/��
	BYTE	byModel;				// ģ��(��ɫģ�����ͱ��)
	SHORT	shSize;					// ģ�͵����ű���
	BYTE	byMasterLevel;			// GMȨ��
	Equipment_3DShow equip_3DShow[enumEE3_Count];	// ֪ͨ3D��װ�����֣����5����λ��װ��

	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_NpcAppear_1 : public K_AppearBase
{
	KXyPos currPos;
	float  fAngle;
	WORD nDirection;
	DWORD nSelectTarget;	// Ŀ��

	char   szBuf[1];
	int GetBaseAttrOffset() { return sizeof(*this) - sizeof(szBuf); }
	void Init() { byDataLen = GetBaseAttrOffset(); }
};

struct K_NpcAppear_2 : public K_AppearBase
{
	INT  mnCurrentHP;
	INT  mnCurrentMP;
	DWORD byStateID  : 8;	// �״̬ID
	DWORD byMoveType : 8;	// �ƶ���ʽ
	DWORD byIsCombat : 1;
	DWORD byIsFly    : 1;
	DWORD byIsQuest	 : 1;	// NPC�Ƿ�������
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
	BYTE	byIsDeath		: 1;		// ����״̬
	float	fRelativeHeight;	//��Ե���ĸ߶�
	INT		nCruiseSpeed;
	INT		nChaseSpeed;
	DWORD	dwFlags;
	INT		mnMaxHP;
	INT		mnMaxMP;
	WORD	nDirectionZ;
	SHORT	shNpcSize;		// 
	BYTE	byForce;		// ����
	BYTE	byRace;			// ����
	BYTE	byNpcLevel;		// �ȼ�
	BYTE	byAbility;		// ְҵ
	DWORD	dwSummonMasterID;		// ����id
	DWORD	dwRelatedCharID;	// ������ɫid
	DWORD	dwStateTime;	// �ʱ��
	BYTE	m_byQuestEff;			// ͷ��������Ч
	BYTE	m_byQuestStateValid;	// ��Ч�Ƿ�ʧЧ !0δʧЧ 0ʧЧ
	BYTE	byVanishLevel;		//����ȼ�
	INT		nAntiDetectRange;	//��������
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
	BYTE	byIsDeath		: 1;		// ����״̬
	BYTE	bOpened			: 1;		// ����
	float	fRelativeHeight;	//��Ե���ĸ߶�
	DWORD	dwHolder;
	DWORD	dwOwner;
	DWORD	dwCreatedTime;
	INT		mnMaxHP;
	WORD	nDirectionZ;
	BYTE	byForce;		// ����
	BYTE	byRace;			// ����
	DWORD	dwRelatedCharID;		// ����id

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
