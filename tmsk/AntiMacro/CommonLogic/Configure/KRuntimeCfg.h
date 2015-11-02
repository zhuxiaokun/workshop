#ifndef _K_RUNTIME_CFG_H_
#define _K_RUNTIME_CFG_H_

#include <System/File/KTextReader.h>
#include "KTaskSchedule.h"

using namespace System::File;

class KRuntimeCfg;

class KIniObserver
{
public:
	virtual ~KIniObserver() { }
	virtual void OnIniUpdate(KIniReader& cfg) = 0;
};

class KRuntimeCfg : public KTaskObject
{
public:
	KRuntimeCfg();
	~KRuntimeCfg();

public:
	BOOL Initialize(const char* path, KIniObserver* pObv);

public:
	DWORD GetNextActivateTick() const;
	void OnActivate();
	void Destroy();

//public:
//	// 使用INI的对外接口
//	int GetInteger(const char* sec, const char* key, int defval);
//	float GetFloat(const char* sec, const char* key, float defval);
//	::System::Collections::KString<512> GetString(const char* sec, const char* key, const char* defVal);
//	int GetSection(const char* sec, char sep, char* buf, int len);

public:
	DWORD m_lastCheck;
	TIME_T m_lastModify;
	::System::Collections::KString<512> m_path;
	KIniObserver* m_pObv;
};

#endif
