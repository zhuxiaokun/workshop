#include "../Configure/KTabfileLoader.h"
#include <string>
#include <System/Memory/KBuffer64k.h>
#include <System/Misc/StrUtil.h>
#include "KMsgTanslate.h"
#include "KStringManager.h"

using namespace std;
typedef ::System::File::KTabFile KTabFile;
typedef ::System::File::KTabFile2 KTabFile2;

KStringManager::KStringManager()
{
	IStringTranslator::SetGlobalTranslator(this);
}

KStringManager::~KStringManager()
{
}

const char* KStringManager::Translate(const char* str)
{
	if(!str) return NULL;
	if(str[0] == 's' && str[1] == ':')
	{
		const char* pID = str + 2;
		if(isnumber3(pID))
		{
			DWORD strID = str2int(pID);
			KStringHashMap::iterator it = m_StringHashMap.find(strID);
			if(it == m_StringHashMap.end())
			{
				return str;
			}
			else
			{
				return it->second;
			}
		}
	}
	return str;
}

BOOL KStringManager::Init(const char* m_FileName)
{
	m_nPatchItemSize = 0;
	memset(m_szPatchItem, 0, sizeof(m_szPatchItem));

	KTabfileLoader& loader = KTabfileLoader::GetInstance();
	KTabFile2* fileReader = loader.GetFileReader(m_FileName);
	if(!fileReader)
	{
		ASSERT_I(m_FileName);
		return FALSE;
	}

	char strFile[256] = {0};

	while(TRUE)
	{
		int nRet = fileReader->ReadLine();
		if(nRet == -1) { loader.CloseFileReader(fileReader); return FALSE; }
		if(nRet == 0) break;
	
		BOOL nRetCode = fileReader->GetString(0 , "" , strFile , 256);

		if (nRetCode && strFile[0])
		{
			char StringFileName[1024];
			sprintf_k(StringFileName, sizeof(StringFileName), "StringManager/%s" , strFile);
			InitByDataFile(StringFileName);
		}
	}

	loader.CloseFileReader(fileReader);
/*
	PushString("仙桃");
	PushInt(10);
	PushOver(2774);

	PushString("你妹");
	PushOver(2007);

	PushWholeString("hello world.");
	PushOver(2016);
	// PatchString(12, m_nPatchItemSize, m_szPatchItem);
	int nID = 2016;
	const char * s1 = GetStringByID(nID);
	const char * s2 = GetStringByIDIgnorePatch(nID);
	*/
	return TRUE;
}

BOOL KStringManager::CoverDataFile(const char* FileName)
{
	return this->InitByDataFile(FileName, TRUE);
}

BOOL KStringManager::InitByDataFile(const char* filename , BOOL bCover)
{
	KTabfileLoader& loader = KTabfileLoader::GetInstance();
	KTabFile2* fileReader = loader.GetFileReader(filename);
	if(!fileReader)
	{
		ASSERT_I(filename);
		return FALSE;
	}

	BOOL nRetCode = FALSE;

	int stringID = 0;
	char str[1024] = {0};
	JG_C::KString<1024> dstStr;

	while(TRUE)
	{
		int nRet = fileReader->ReadLine();
		if(nRet == -1) { loader.CloseFileReader(fileReader); return FALSE; }
		if(nRet == 0) break;

		nRetCode = fileReader->GetInteger(0 , 0, &stringID);
		if(!nRetCode) continue;

		nRetCode = fileReader->GetString(1, "", str, 1024);
		if(!nRetCode) continue;

		const char* ps = TranslateSourceMessage<1024>(str, dstStr);

		int len = dstStr.size();
		if (len == 0) dstStr = "";

		if(len < 0 || len >= 1024)
		{
			Log(LOG_ERROR, "error: string %u has a length %d exceed 1024 in file %s", stringID, len, filename);
			continue;
		}

		if(bCover)
		{
			const char* oldStr = this->GetStringByID(stringID);
			if(!oldStr || strcmp(oldStr, ps) != 0)
			{
				char* strValue = KBuffer64k::WriteData(ps, len+1);
				m_StringHashMap[stringID] = strValue;
			}
		}
		else
		{
			char* strValue = KBuffer64k::WriteData(ps, len+1);
			m_StringHashMap[stringID] = strValue;
		}
	}

	loader.CloseFileReader(fileReader);
	return TRUE;
}

const char* KStringManager::GetStringByID(INT dwID)
{
	const char* str = GetPatchStringByID(dwID);
	if(str != NULL) return str;
	
	return GetStringByIDIgnorePatch(dwID);
}

const char*	KStringManager::GetStringByIDIgnorePatch(INT dwID)
{
	KStringHashMap::iterator it = m_StringHashMap.find(dwID);
	if (it == m_StringHashMap.end())
		return "";

	return it->second;
}

BOOL KStringManager::InitIllegalChar(const char* m_FileName, StringVec& strvec)
{
	KTabfileLoader& loader = KTabfileLoader::GetInstance();
	KTabFile2* fileReader = loader.GetFileReader(m_FileName);
	if(!fileReader)
	{
		ASSERT_I(m_FileName);
		return FALSE;
	}

	BOOL nRetCode = FALSE;
	char str[1024] = {0};

	while(TRUE)
	{
		int nRet = fileReader->ReadLine();
		if(nRet == -1) { loader.CloseFileReader(fileReader); return FALSE; }
		if(nRet == 0) break;

		nRetCode = fileReader->GetString(0 , "" , str , 1024);

		if (!nRetCode)			continue;

		int len = strlen(str);

		if(len > 0 && len < 1024)
		{
			//char* strValue = new char[len + 1];
			//strcpy_k(strValue, len + 1, str);

			char* strValue = KBuffer64k::WriteData(str, len+1);
			strvec.push_back(strValue);
		}
	}

	return TRUE;
}


BOOL KStringManager::CheckIllegalCharValid(StringVec& strvec, const char* m_SrcStr , char* m_ErrorStr /* = NULL */)
{
	for (int i = 0; i < strvec.size() ; i++)
	{
		if (strstr(m_SrcStr, strvec[i]))
		{
			if (m_ErrorStr)		
			{
				m_ErrorStr = strvec[i];
			}

			return FALSE;
		}
	}
	return TRUE;
}

BOOL KStringManager::ReplaceIllegalCharAboutStr(const char* m_StrStr , char* m_DestStr , const char* m_ReplaceStr /* = */ )
{
	if(!m_StrStr)
		return FALSE;
	if(!m_DestStr)
		return FALSE;
	
	std::string s = m_StrStr;

	for (int i = 0 ; i < m_IllegalCharList.size() ; i++)
	{
		std::string sub = m_IllegalCharList[i];
		int n = s.find(sub);
		while (n >= 0)
		{
			s.replace(n, sub.size(), m_ReplaceStr);
			n = s.find(sub);
		}
	}

	strcpy(m_DestStr , s.c_str());

	return TRUE;
}

BOOL KStringManager::InitInvalidString(BOOL bCheckName)
{
	m_IllegalCharList.clear();
	if(!InitIllegalChar("StringManager/IllegalChars.txt", m_IllegalCharList))
		return FALSE;
	if(bCheckName)
	{
		m_CheckNameList.clear();
		if(!InitIllegalChar("ServerInfo/CheckName.txt", m_CheckNameList))
			return FALSE;
	}
	return TRUE;
}

// 检查名字和字符是否包含非法字符，不做替换，用于创建角色名字检查
BOOL KStringManager::CheckNameValid(const char* m_SrcStr , char* m_ErrorStr)
{
	if (!CheckIllegalCharValid(m_IllegalCharList, m_SrcStr, m_ErrorStr))
		return FALSE;
	if (!CheckIllegalCharValid(m_CheckNameList, m_SrcStr, m_ErrorStr))
		return FALSE;
	return TRUE;
}
// 仅检查字符串中是否包含敏感词，用于聊天
BOOL KStringManager::CheckChatValid(const char* m_SrcStr , char* m_ErrorStr)
{
	if (!CheckIllegalCharValid(m_IllegalCharList, m_SrcStr, m_ErrorStr))
		return FALSE;
	return TRUE;
}

BOOL KStringManager::IsOwnerFile( const char* filename )
{
	int prefixlen = 14;
	const char* prefix = "StringManager/";
	return strncmp(filename, prefix, prefixlen) == 0;
}

void KStringManager::PatchWholeString(DWORD dwID, const char* str)
{
	PatchStr ps;
	ps.id = dwID;
	if(str == NULL) return;
	int nPos = m_StringPatch.find(ps);
	if(nPos >= 0)
	{
		ps = m_StringPatch[nPos];
		if(strcmp(ps.str, str) == 0)
			return;
		m_StringPatch.erase(nPos);
	}

	int len = strlen(str);
	ps.id = dwID;
	ps.str = KBuffer64k::WriteData(str, len+1);
	m_StringPatch.insert_unique(ps);
}

const char*	KStringManager::GetPatchStringByID(INT dwID)
{
	PatchStr ps;
	ps.id = dwID;
	int nPos = m_StringPatch.find(ps);
	if(nPos < 0) return NULL;
	return m_StringPatch[nPos].str;
}

void	KStringManager::PushInt(int val)
{
	// flag(1) 4(1), int(4)
	ASSERT(sizeof(m_szPatchItem) - m_nPatchItemSize > 6);

	PatchSubItem* pItem = (PatchSubItem*)(m_szPatchItem + m_nPatchItemSize);
	pItem->type = patchInt;
	pItem->len = sizeof(int);
	memcpy(pItem->buf, &val, pItem->len);
	m_nPatchItemSize += pItem->Size();
}

void	KStringManager::PushFloat(double val)
{
	// flag(1) 8(1), double(8)
	ASSERT(sizeof(m_szPatchItem) - m_nPatchItemSize > 10);

	PatchSubItem* pItem = (PatchSubItem*)(m_szPatchItem + m_nPatchItemSize);
	pItem->type = patchFloat;
	pItem->len = sizeof(double);
	memcpy(pItem->buf, &val, pItem->len);
	m_nPatchItemSize += pItem->Size();
}

void	KStringManager::PushString(const char* str)
{
	// flag(1) length(1) str(n)
	int len = strlen(str) + 1;
	ASSERT(sizeof(m_szPatchItem) - m_nPatchItemSize > (unsigned int)len + 1);

	PatchSubItem* pItem = (PatchSubItem*)(m_szPatchItem + m_nPatchItemSize);
	pItem->type = patchString;
	pItem->len = len;
	memcpy(pItem->buf, str, pItem->len);
	m_nPatchItemSize += pItem->Size();
}

void	KStringManager::PushWholeString(const char* str)
{
	// flag(1) length(1) str(n)
	int len = strlen(str) + 1;
	ASSERT(sizeof(m_szPatchItem) - m_nPatchItemSize > (unsigned int)len + 1);

	PatchSubItem* pItem = (PatchSubItem*)(m_szPatchItem + m_nPatchItemSize);
	pItem->type = patchWholeString;
	pItem->len = len;
	memcpy(pItem->buf, str, pItem->len);
	m_nPatchItemSize += pItem->Size();
}

void	KStringManager::PushOver(DWORD dwStringID)
{
	// 将m_szPatchItem数据Patch到m_PatchItemList中
	int BuffSize = m_nPatchItemSize + sizeof(PatchItem);
	char* str = KBuffer64k::WriteData("", BuffSize);
	PatchItem* pi = (PatchItem*)str;
	pi->id = dwStringID;
	pi->size = m_nPatchItemSize;
	memcpy(pi->buff, m_szPatchItem, m_nPatchItemSize);
	m_PatchItemList.push_back((char*)pi);

	// Patch服务器字符串资源
	PatchString(dwStringID, m_nPatchItemSize, m_szPatchItem);
	// 恢复缓冲区
	m_nPatchItemSize = 0;
	memset(m_szPatchItem, 0, sizeof(m_szPatchItem));

	// 给所有在线的人发送patch数据
	if(m_pPatchInterface != NULL)
		m_pPatchInterface->OnGetPatchItem(pi->id, pi->size, pi->buff);
}

void	KStringManager::PatchString(DWORD dwID, int nBuffLen, char* szBuff)
{
	const char* fmt = GetStringByIDIgnorePatch(dwID);
	if(fmt == NULL || szBuff == NULL) return;

	char buf[256];
	int  pos = 0;
	char *pstr = szBuff;
	PatchSubItem* pItem;
	pItem = (PatchSubItem*)pstr;
	for (; pstr - szBuff < nBuffLen;)
	{
		if (pItem->type == patchInt)
		{
			memcpy(buf + pos, pItem->buf, sizeof(int));
			pos += sizeof(int);
		}
		else if(pItem->type == patchFloat)
		{
			memcpy(buf + pos, pItem->buf, sizeof(double));
			pos += sizeof(double);
		}
		else if(pItem->type == patchString)
		{
			int* val = (int*)(buf + pos);
			*val = (int)pItem->buf;
			pos += sizeof(char*);
		}
		else if(pItem->type == patchWholeString)
		{
			// 如果碰到patchWholeString, 那么只用这个字符串替换客户端对应的字符串
			PatchWholeString(dwID, pItem->buf);
			return;
		}
		pstr += pItem->Size();
		pItem = (PatchSubItem*)pstr;
		ASSERT(pstr - szBuff <= nBuffLen);
	}

	va_list ap = (va_list)&buf;
	vsprintf(m_szTempString, fmt, ap);

	ASSERT(strlen(m_szTempString) < sizeof(m_szTempString));
	PatchWholeString(dwID, m_szTempString);
}

void	KStringManager::VisitPatchList(IVisitPatchItem* pvi)
{
	PatchItem* pi;
	for(int i = 0; i < m_PatchItemList.size(); i++)
	{
		pi = (PatchItem*)m_PatchItemList[i];
		pvi->OnGetPatchItem(pi->id, pi->size, pi->buff);
	}
}

void	KStringManager::MakePatchInterface(IVisitPatchItem* pvi)
{
	m_pPatchInterface = pvi;
}