///////////////////////////////////////////////////////////////////////////////
//
// FileName  :  KStringManager
// Creator   :  Calvin
// Date      :  2008-06-10
// Comment   :  �ַ�������ϵͳ
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
	// ��ʼ����������ȡ������ļ��б��������ļ��б��������ļ�
	BOOL	Init(const char* m_FileName);

	// ȡ���ַ���������ͨ��ȫ��ΨһID��ȡ��Ӧ�ַ�����ͨ��ȫ��ΨһID���ҵ���Ӧ�Ĳ�ѯMAP����������PatchString��ȡ
	const char*	GetStringByID(INT dwID);
	// ȡ���ַ�������������PatchString
	const char*	GetStringByIDIgnorePatch(INT dwID);

	// ��ʼ���Ƿ��ַ������ּ��
	BOOL	InitInvalidString(BOOL bCheckName = TRUE);

	// ������ֺ��ַ��Ƿ�����Ƿ��ַ��������滻�����ڴ�����ɫ���ּ��
	BOOL	CheckNameValid(const char* m_SrcStr , char* m_ErrorStr = NULL);
	// ������ַ������Ƿ�������дʣ���������
	BOOL	CheckChatValid(const char* m_SrcStr , char* m_ErrorStr = NULL);

	// ���Դ�ַ��������Ƿ��ַ������滻Ϊ*
	BOOL	ReplaceIllegalCharAboutStr(const char* m_StrStr , char* m_DestStr , const char* m_ReplaceStr = "*");

	BOOL CoverDataFile(const char* FileName);
	BOOL IsOwnerFile( const char* filename );

	// �滻�ַ����ӿڣ������滻�ͻ������е��ַ����������ڴ����滻
	void	PatchWholeString(DWORD dwID, const char* str);
	// ���ݲ������滻�ͻ��˶�Ӧ���ַ��������������ڷ���������ҵ�½ʱ�·��������ڿͻ������Ŀ���ַ��������ڿͻ���
	void	PatchString(DWORD dwID, int nBuffLen, char* szBuff);

	void	PushInt(int val);
	void	PushFloat(double val);
	void	PushString(const char* str);
	void	PushWholeString(const char* str);
	void	PushOver(DWORD dwStringID);
	void	VisitPatchList(IVisitPatchItem* pvi);
	void	MakePatchInterface(IVisitPatchItem* pvi);

private:
	// ���ص�һ�ļ��е�ID2String�������ݵ�m_mapID2String
	BOOL	InitByDataFile(const char* m_FileName, BOOL bCover=FALSE);
	// ��ʼ���Ƿ��ַ���
	BOOL	InitIllegalChar(const char* m_FileName, StringVec& strvec);
	// �����ַ��Ƿ�����Ƿ��ַ��������滻
	BOOL	CheckIllegalCharValid(StringVec& strvec, const char* m_SrcStr , char* m_ErrorStr = NULL);
	// ��m_StringPatchȡ���ַ���
	const char*	GetPatchStringByID(INT dwID);
protected:
	enum PatchType
	{
		patchInt = 1,
		patchFloat,
		patchString,
		patchWholeString,	// ��ȫ����
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
	char			m_szTempString[4096];	// ƴ������ַ�����ƴ�պ��ַ������Ȳ��ܳ���
};

#define G_STR(x) KStringManager::Instance()->GetStringByID((x))