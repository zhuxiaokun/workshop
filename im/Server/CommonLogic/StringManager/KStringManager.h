#pragma once
#include <System/KType.h>
#include <System/KMacro.h>
#include <System/Patterns/KSingleton.h>
#include <System/File/KTabFile.h>
#include <System/Collections/DynArray.h>
#include <System/Collections/KHashTable.h>
#include <System/Misc/StrUtil.h>
#include <System/Misc/KStringtable.h>

#pragma warning(disable: 4996)

class KStringManager;
extern KStringManager* g_strMgr;

class KStringManager : public IStringTranslator, public JG_P::KSingleton<KStringManager>
{
public:
	KStringManager();
	~KStringManager();

public:
	bool loadAll(const char* path);
	bool loadFileList(int lang, const char* listfile);
	bool loadForLanguage(const char* path, int lang);
	bool loadLocalLang(const char* path);
	bool patch(int lang, StreamInterface& si);

public:
	// 初始化函数，读取传入的文件列表，并加载文件列表中所有文件
	BOOL Init(const char* filename);

	// 初始化非法字符和名字检查
	BOOL InitInvalidString(bool checkName = true);

	// 初始化非法字符和名字检查
	BOOL InitIllegalWords(const char* filename);

public:
	const char* getString(int lang, int strID);
	const char* Translate(const char* str);

	const char* GetStringByID(int strID);
	const char*	getStringByID(int lang, int strID, const char* defVal);

	BOOL CheckString(int lang, const char* str);
	BOOL CheckAndReplaceString(int lang, char* str, size_t len, int chr='*');

	// 检查名字和字符是否包含非法字符，不做替换，用于创建角色名字检查
	BOOL CheckNameValid(const char* name);

	// 仅检查字符串中是否包含敏感词，用于聊天
	BOOL CheckChatValid(const char* str);

	//// 如果源字符串包含非法字符，则替换为*
	//BOOL ReplaceIllegalCharAboutStr(const char* m_StrStr , char* m_DestStr , const char* m_ReplaceStr = "*");
	BOOL IsOwnerFile(const char* filename);

private:
	//// 检查该字符是否包含非法字符，不做替换
	//BOOL CheckIllegalCharValid(StringVec& strvec, const char* m_SrcStr , char* m_ErrorStr = NULL);
	JG_C::KString<512> _getResourceDir(const char* filename); // */*/StringManager
	int _getLangFromFilename(const char* filename);

private:
	BOOL _loadIllegalWords(int lang, const char* dir);

public:
	int m_language; // current platform language
	KStringtableManager m_strTableManager;
};

#define G_STR(x) KStringManager::Instance()->GetStringByID((x))
