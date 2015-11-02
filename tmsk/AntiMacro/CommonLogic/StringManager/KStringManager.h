///////////////////////////////////////////////////////////////////////////////
//
// FileName  :  KStringManager
// Creator   :  Calvin
// Date      :  2008-06-10
// Comment   :  字符串管理系统
//
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "System/KType.h"
#include "System/KMacro.h"
#include "System/File/KTabFile.h"
#include "System/Collections/DynArray.h"
#include "System/Collections/KHashTable.h"
#include <System/Misc/StrUtil.h>
#include <System/Patterns/KSingleton.h>

#pragma warning(disable: 4996)

//using namespace ::System::Collections;
//using namespace ::System::Sync;
//using namespace ::System::File;

class IVisitPatchItem
{
public:
	virtual void OnGetPatchItem(DWORD dwID, WORD wBuffLen, char* szBuff) = 0;
};

struct PatchStr 
{
	DWORD id;
	char* str;
};
struct PatchItem
{
	DWORD id;
	WORD  size;
	char  buff[1];
};
struct PatchSubItem
{
	BYTE type;
	BYTE len;
	char buf[1];
	int Size() 
	{
		return sizeof(type) + sizeof(len) + len;
	}
};
DECLARE_SIMPLE_TYPE(PatchStr)
class PK_PatchStr
{
public:
	BOOL operator () (const PatchStr& a, const PatchStr& b) const
	{
		return a.id < b.id;
	}
};

typedef JG_C::DynSortedArray<PatchStr, PK_PatchStr, 64,64>	StringArray;
typedef JG_C::KHashTable<DWORD,char*,KUintCompare<DWORD>,KHasher<DWORD>,JG_S::KMTLock,2048,2048> KStringHashMap;
typedef	JG_C::DynArray<char*,1024,1024>	StringVec;


class KStringManager
	: public ::System::Patterns::KSingleton<KStringManager>
	, public IStringTranslator
{
public:
	KStringManager();
	~KStringManager();

public:
	const char* Translate(const char* str);

public:
	// 初始化函数，读取传入的文件列表，并加载文件列表中所有文件
	BOOL	Init(const char* m_FileName);

	// 取得字符串函数，通过全局唯一ID获取对应字符串（通过全局唯一ID先找到对应的查询MAP），首先在PatchString中取
	const char*	GetStringByID(INT dwID);
	// 取得字符串函数，忽略PatchString
	const char*	GetStringByIDIgnorePatch(INT dwID);

	// 初始化非法字符和名字检查
	BOOL	InitInvalidString(BOOL bCheckName = TRUE);

	// 检查名字和字符是否包含非法字符，不做替换，用于创建角色名字检查
	BOOL	CheckNameValid(const char* m_SrcStr , char* m_ErrorStr = NULL);
	// 仅检查字符串中是否包含敏感词，用于聊天
	BOOL	CheckChatValid(const char* m_SrcStr , char* m_ErrorStr = NULL);

	// 如果源字符串包含非法字符，则替换为*
	BOOL	ReplaceIllegalCharAboutStr(const char* m_StrStr , char* m_DestStr , const char* m_ReplaceStr = "*");

	BOOL CoverDataFile(const char* FileName);
	BOOL IsOwnerFile( const char* filename );

	// 替换字符串接口，用来替换客户端已有的字符串，仅在内存中替换
	void	PatchWholeString(DWORD dwID, const char* str);
	// 根据参数来替换客户端对应的字符串，参数保存在服务器，玩家登陆时下发参数，在客户端组成目标字符串保存在客户端
	void	PatchString(DWORD dwID, int nBuffLen, char* szBuff);

	void	PushInt(int val);
	void	PushFloat(double val);
	void	PushString(const char* str);
	void	PushWholeString(const char* str);
	void	PushOver(DWORD dwStringID);
	void	VisitPatchList(IVisitPatchItem* pvi);
	void	MakePatchInterface(IVisitPatchItem* pvi);

private:
	// 加载单一文件中的ID2String具体内容到m_mapID2String
	BOOL	InitByDataFile(const char* m_FileName, BOOL bCover=FALSE);
	// 初始化非法字符集
	BOOL	InitIllegalChar(const char* m_FileName, StringVec& strvec);
	// 检查该字符是否包含非法字符，不做替换
	BOOL	CheckIllegalCharValid(StringVec& strvec, const char* m_SrcStr , char* m_ErrorStr = NULL);
	// 从m_StringPatch取得字符串
	const char*	GetPatchStringByID(INT dwID);
protected:
	enum PatchType
	{
		patchInt = 1,
		patchFloat,
		patchString,
		patchWholeString,	// 完全覆盖
	};
	//System::Collections::DynSortedArray<KStringData, KStringPKLess, 1024 , 1024> m_StringSortArray;
	StringVec			m_IllegalCharList;
	StringVec			m_CheckNameList;
	KStringHashMap	m_StringHashMap;
	StringArray			m_StringPatch;

	StringVec			m_PatchItemList;
	IVisitPatchItem*	m_pPatchInterface;
	int				m_nPatchItemSize;
	char			m_szPatchItem[1024];
	char			m_szTempString[4096];	// 拼接最长的字符串，拼凑后字符串长度不能超过
};

#define G_STR(x) KStringManager::Instance()->GetStringByID((x))