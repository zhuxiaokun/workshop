#include "KVillage.h"
#include <System/Log/log.h>
#include <System/File/KFile.h>
#include <System/Misc/StrUtil.h>
#include <System/Misc/KStream.h>
#include <KCommonDefine.h>

/// Visitor ///

class KVisitor_initVillage : public KMapSetting::IVisitor
{
public:
	KVillageDataManager& m_vdm;
	KVisitor_initVillage(KVillageDataManager& vdm):m_vdm(vdm)
	{
	}
	bool visit(const KMapSetting& ms)
	{
		int mapID = ms.m_mapID;
		const char* mapName = ms.m_mapName;
		if(m_vdm.m_mn2villages.find(mapName) != m_vdm.m_mn2villages.end())
			m_vdm.m_mid2villages[mapID] = m_vdm.m_mn2villages[mapName];
		return true;
	}
};
class KVisitor_villageWriter : public KVillageData::IVisitor
{
public:
	StreamInterface& so;
	::System::Collections::KString<1024> line;
	KVisitor_villageWriter(StreamInterface& s):so(s)
	{
	}
	bool visit(KVillageData& vd)
	{
		line.Printf("\t\t<Village index=\"%d\">\n", vd.m_index);so.WriteData(line.c_str(), line.size());
		line.Printf("\t\t\t<Name value=\"%s\"/>\n", vd.m_name);so.WriteData(line.c_str(), line.size());
		line.Printf("\t\t\t<BackgroundMusic value=\"%s\"/>\n", vd.m_bgMusic);so.WriteData(line.c_str(), line.size());
		line.Printf("\t\t\t<ClientKey value=\"%s\"/>\n", vd.m_clientKey);so.WriteData(line.c_str(), line.size());
		line.Printf("\t\t\t<Nation value=\"%d\"/>\n", vd.m_nationarity);so.WriteData(line.c_str(), line.size());
		line.Printf("\t\t\t<ShowColor value=\"%u\"/>\n", vd.m_color);so.WriteData(line.c_str(), line.size());
		line.Printf("\t\t\t<RelivePoint shan=\"%d\" shui=\"%d\" xue=\"%d\"/>\n", vd.m_relivePoints[0],vd.m_relivePoints[1],vd.m_relivePoints[2]);so.WriteData(line.c_str(), line.size());
		line.Printf("\t\t\t<Attribute value=\"%u\"/>\n", vd.m_attrVal);so.WriteData(line.c_str(), line.size());
		line.Printf("\t\t\t<BackgroundMusicGroup value=\"%s\"/>\n", vd.m_bgMusicGroup);so.WriteData(line.c_str(), line.size());
		line.Printf("\t\t\t<AmbientMusic value=\"%s\"/>\n", vd.m_amMusic);so.WriteData(line.c_str(), line.size());
		line.Printf("\t\t</Village>\n", vd.m_index);so.WriteData(line.c_str(), line.size());so.WriteData(line.c_str(), line.size());
		return true;
	}
};

/// KVillageDataManager

KVillageDataManager::KVillageDataManager()
{

}

KVillageDataManager::~KVillageDataManager()
{

}

BOOL KVillageDataManager::Initialize(KMapSettings& mss, const char* villageXml)
{
	KInputFileStream fi;
	if(!fi.Open(villageXml))
	{
		Log(LOG_ERROR, "error: open village file %s", villageXml);
		return FALSE;
	}
	return this->Initialize(mss, &fi);
}

BOOL KVillageDataManager::Initialize(KMapSettings& mss, StreamInterface* pVillageStream)
{
	m_villagesXml.Clear();
	
	XMLDomParser parser;
	parser.Initialize();

	if(!parser.Parse(*pVillageStream, m_villagesXml))
	{
		Log(LOG_ERROR, "error: parse village file, %s", parser.GetErrorMsg());
		return FALSE;
	}

	XMLNodeColl mapColl = m_villagesXml.QueryChildren("Villages/Map");
	int n = mapColl.Size();
	for(int i=0; i<n; i++)
	{
		XMLElementNode* mapXml = (XMLElementNode*)mapColl.GetNode(i);
		const char* mapName = mapXml->QueryAtt("name")->c_str();

		mapName = KBuffer64k::WriteData(mapName, strlen(mapName)+1);
		Idx2Village& idx2villages = m_mn2villages[mapName];

		XMLNodeColl villageColl = mapXml->QueryChildren("Village");
		int m = villageColl.Size();
		for(int k=0; k<m; k++)
		{
			int relivep[MAX_NATION];
			memset(&relivep, 0, sizeof(relivep));

			XMLElementNode* villageXml = (XMLElementNode*)villageColl.GetNode(k);
			int idx = villageXml->QueryAtt("index")->Integer();
			const char* vname = villageXml->QueryAtt("name.value") ? villageXml->QueryAtt("name.value")->c_str() : "";
			const char* bgmusic = villageXml->QueryAtt("BackgroundMusic.value") ? villageXml->QueryAtt("BackgroundMusic.value")->c_str() : "";
			const char* clnkey = villageXml->QueryAtt("ClientKey.value") ? villageXml->QueryAtt("ClientKey.value")->c_str() : "";
			int nation = villageXml->QueryAtt("Nation.value") ? villageXml->QueryAtt("Nation.value")->Integer() : 0;
			DWORD showColor = villageXml->QueryAtt("ShowColor.value") ? villageXml->QueryAtt("ShowColor.value")->Integer() : 0;
			relivep[0] = villageXml->QueryAtt("RelivePoint.shui") ? villageXml->QueryAtt("RelivePoint.shui")->Integer() : 0;
			relivep[1] = villageXml->QueryAtt("RelivePoint.xue") ? villageXml->QueryAtt("RelivePoint.xue")->Integer() : 0;
			relivep[2] = villageXml->QueryAtt("RelivePoint.shan") ? villageXml->QueryAtt("RelivePoint.shan")->Integer() : 0;
			DWORD attrVal = villageXml->QueryAtt("Attribute.value") ? villageXml->QueryAtt("Attribute.value")->Integer() : 0;
			const char* bgmusicgrp = villageXml->QueryAtt("BackgroundMusicGroup.value") ? villageXml->QueryAtt("BackgroundMusicGroup.value")->c_str() : "";
			const char* abmusic = villageXml->QueryAtt("AmbientMusic.value") ? villageXml->QueryAtt("AmbientMusic.value")->c_str() : "";

			KVillageData* vd = m_dataPool.Alloc();
			
			vd->m_index = idx;
			vd->m_name = KBuffer64k::WriteData(vname, strlen(vname)+1);
			vd->m_bgMusic = KBuffer64k::WriteData(bgmusic, strlen(bgmusic)+1);
			vd->m_amMusic = KBuffer64k::WriteData(abmusic, strlen(abmusic)+1);
			vd->m_bgMusicGroup = KBuffer64k::WriteData(bgmusicgrp, strlen(bgmusicgrp)+1);
			vd->m_clientKey = KBuffer64k::WriteData(clnkey, strlen(clnkey)+1);
			vd->m_nationarity = nation;
			vd->m_color = showColor;
			vd->m_relivePoints = (int*)KBuffer64k::WriteData(relivep, sizeof(relivep));
			vd->m_attrVal = attrVal;

			idx2villages[idx] = vd;
		}
	}

	KVisitor_initVillage v(*this);
	mss.foreach(v);

	return TRUE;
}

void KVillageDataManager::Finalize()
{
	m_dataPool.Destroy();
}

KVillageData* KVillageDataManager::GetVillageData(DWORD villageID)
{
	VillageID vid(villageID);
	int mapID = vid.mapID;
	MapID2Villages::iterator it = m_mid2villages.find(mapID);
	if(it == m_mid2villages.end()) return NULL;

	Idx2Village& vs = it->second;
	Idx2Village::iterator it2 = vs.find(vid.villageIdx);
	if(it2 == vs.end()) return NULL;
	return it2->second;
}

KVillageData* KVillageDataManager::GetVillageData(int mapID, int villageIdx)
{
	MapID2Villages::iterator it = m_mid2villages.find(mapID);
	if(it == m_mid2villages.end()) return NULL;
	Idx2Village& vs = it->second;
	Idx2Village::iterator it2 = vs.find(villageIdx);
	if(it2 == vs.end()) return NULL;
	return it2->second;
}

KVillageData* KVillageDataManager::GetVillageData(const char* mapName, int villageIdx)
{
	MapName2Villages::iterator it = m_mn2villages.find(mapName);
	if(it == m_mn2villages.end()) return NULL;

	Idx2Village& vs = it->second;
	Idx2Village::iterator it2 = vs.find(villageIdx);
	if(it2 == vs.end()) return NULL;
	return it2->second;
}

int KVillageDataManager::GetVillageCount(int mapID) const
{
	MapID2Villages::const_iterator it = m_mid2villages.find(mapID);
	if(it == m_mid2villages.end()) return 0;
	return it->second.size();
}

int KVillageDataManager::GetVillageCount(const char* mapName) const
{
	MapName2Villages::const_iterator it = m_mn2villages.find(mapName);
	if(it == m_mn2villages.end()) return 0;
	return it->second.size();
}

bool KVillageDataManager::foreach(KVillageData::IVisitor& v)
{
	MapName2Villages::iterator it = m_mn2villages.begin();
	MapName2Villages::iterator ite = m_mn2villages.end();
	while(it != ite)
	{
		Idx2Village& vds = it->second; it++;
		Idx2Village::iterator it2 = vds.begin();
		Idx2Village::iterator it2e = vds.end();
		while(it2 != it2e)
		{
			KVillageData* vd = it2->second; it2++;
			if(!v.visit(*vd)) return false;
		}
	}
	return true;
}

bool KVillageDataManager::foreach(int mapID, KVillageData::IVisitor& v)
{
	MapID2Villages::iterator it = m_mid2villages.find(mapID);
	if(it == m_mid2villages.end()) return true;

	Idx2Village& vds = it->second;
	Idx2Village::iterator it2 = vds.begin();
	Idx2Village::iterator it2e = vds.end();
	while(it2 != it2e)
	{
		KVillageData* vd = it2->second; it2++;
		if(!v.visit(*vd)) return false;
	}
	return true;
}

bool KVillageDataManager::foreach(const char* mapName, KVillageData::IVisitor& v)
{
	MapName2Villages::iterator it = m_mn2villages.find(mapName);
	if(it == m_mn2villages.end()) return true;

	Idx2Village& vds = it->second;
	Idx2Village::iterator it2 = vds.begin();
	Idx2Village::iterator it2e = vds.end();
	while(it2 != it2e)
	{
		KVillageData* vd = it2->second; it2++;
		if(!v.visit(*vd)) return false;
	}
	return true;
}

BOOL KVillageDataManager::SaveAs(const char* xmlFileName)
{
	KOutputFileStream fo;
	if(!fo.Open(xmlFileName, "w+")) return FALSE;

	System::Collections::KString<1024> line;
	MapName2Villages::iterator it = m_mn2villages.begin();
	MapName2Villages::iterator ite = m_mn2villages.end();

	line.Printf("<?xml version=\"1.0\" ?>\n"); fo.WriteData(line.c_str(), line.size());
	line.Printf("<Villages>\n"); fo.WriteData(line.c_str(), line.size());
	while(it != ite)
	{
		const char* mapName = it->first;
		Idx2Village& vds = it->second; it++;
		line.Printf("\t<Map name=\"%s\">\n", mapName); fo.WriteData(line.c_str(), line.size());
		KVisitor_villageWriter v(fo);
		this->foreach(mapName, v);
		line.Printf("\t</Map>\n", mapName); fo.WriteData(line.c_str(), line.size());
	}
	line.Printf("</Villages>\n"); fo.WriteData(line.c_str(), line.size());
	return TRUE;
}
