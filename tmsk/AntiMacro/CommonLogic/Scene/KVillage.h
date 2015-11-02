#ifndef _K_VILIAGE_H_
#define _K_VILIAGE_H_

#include <System/Collections/KString.h>
#include <System/Collections/KMapByVector.h>
#include <System/File/KTabFile.h>
#include <System/Memory/KObjectPool.h>
#include <System/Collections/KSDList.h>
#include <System/Memory/KAlloc.h>
#include <System/Sync/KSync.h>
#include <System/tinyxml/xmldom.h>
#include "KMapSetting.h"
#include "KCommonDefine.h"

const char VILLAGE_CONFIG_FILE[] = "Villages.xml";

struct KVillageData
{
	class IVisitor
	{
	public:
		virtual ~IVisitor() { }
		virtual bool visit(KVillageData& vd) = 0;
	};

//////////////////////////////////////////////////////////////////
	int m_index;
	const char* m_name;
	const char* m_bgMusic;			//��������
	const char* m_amMusic;			//������Ч
	const char* m_bgMusicGroup;		//����������
	const char* m_clientKey;
	int m_nationarity;
	DWORD m_color;
	int* m_relivePoints;
	//DWORD* m_returnPoints;
	const char* m_luaTable;			// lua script table with name suach as Village_<villageID>
	union
	{
		struct
		{
			DWORD m_noFight       : 1;	//����ս����			1
			DWORD m_noPK          : 1;	//����pk��				2
			DWORD m_nationProtect : 1;	//���ұ�����			4
			DWORD m_PK            : 1;	//ǿ��pk��				8
			DWORD m_trade         : 1;	//��̯��				16
			DWORD m_noHomeScroll  : 1;	//���ɻس���			32
			DWORD m_noFly         : 1;	//������				64
			DWORD m_noRide        : 1;	//���������			128
			DWORD m_noJump        : 1;	//������Ծ��			256
			DWORD m_relax         : 1;	//������				512
			DWORD m_fireState     : 1;	//������				1024
			DWORD m_canDuel       : 1;	//���ɾ�������			2048
			DWORD m_showPVPInfo   : 1;	//�ṩPVP����ɱ��Ϣ��	4096
			DWORD m_passiveSafe   : 1;	//����������			8192
			DWORD m_fightBotArea  : 1;	//ս���һ���			16384
			DWORD m_levelCareless : 1;	//�Եȼ������������	32768
			DWORD m_reserved      : 16;
		} m_attr;
		DWORD m_attrVal;
	};
};

class KVillageDataManager
{
public:
	typedef JG_M::KStepObjectPool<KVillageData,256> VillageDataPool;
	typedef JG_C::KMapByVector2<int,KVillageData*> Idx2Village;
	typedef JG_C::KMapByVector2<int,Idx2Village> MapID2Villages;
	typedef JG_C::KMapByVector2<ccstr,Idx2Village,KccstrCmp> MapName2Villages;
	struct VillageID
	{
		int mapID;
		int villageIdx;
		VillageID(int villageID)
		{
			ASSERT(villageID > 0);
			mapID = villageID/10000;
			villageIdx = villageID%10000;
			ASSERT(villageIdx < MAX_MAP_VILLAGE);
		}
		VillageID(int map, int idx):mapID(map),villageIdx(idx)
		{

		}
		operator DWORD() const
		{
			return mapID*10000 + villageIdx;
		}
	};

private:
	KVillageDataManager(const KVillageDataManager&);
	KVillageDataManager& operator=(const KVillageDataManager&);

public:
	KVillageDataManager();
	virtual ~KVillageDataManager();

public:
	BOOL Initialize(KMapSettings& mss, const char* villageXml);
	BOOL Initialize(KMapSettings& mss, StreamInterface* pVillageStream);
	void Finalize();

public:
	KVillageData* GetVillageData(DWORD villageID);
	KVillageData* GetVillageData(int mapID, int villageIdx);
	KVillageData* GetVillageData(const char* mapName, int villageIdx);

public:
	int GetVillageCount(int mapID) const;
	int GetVillageCount(const char* mapName) const;

public:
	bool foreach(KVillageData::IVisitor& v);
	bool foreach(int mapID, KVillageData::IVisitor& v);
	bool foreach(const char* mapName, KVillageData::IVisitor& v);

public:
	BOOL SaveAs(const char* xmlFileName);

public:
	MapID2Villages m_mid2villages;
	MapName2Villages m_mn2villages;

protected:
	XMLElementNode m_villagesXml;
	VillageDataPool m_dataPool;
};

#endif
