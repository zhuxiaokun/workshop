#include "../KPlatform.h"
#include "../Log/log.h"
#include "../Misc/StrUtil.h"
#include "KThreadLocalStorage.h"

KThreadLocalStorage::KThreadLocalStorage()
{

}

KThreadLocalStorage::~KThreadLocalStorage()
{

}

KThreadLocalStorage& KThreadLocalStorage::GetInstance()
{
	static KThreadLocalStorage m_inst;
	return m_inst;
}

int KThreadLocalStorage::AllocStorage()
{
	return KThreadLocalStorage::Alloc();
}

BOOL KThreadLocalStorage::SetValue(int tlsIndex, void* pData)
{
	return KThreadLocalStorage::Set(tlsIndex, pData);
}

BOOL KThreadLocalStorage::GetValue(int tlsIndex, void*& pData)
{
	return KThreadLocalStorage::Get(tlsIndex, pData);
}

int KThreadLocalStorage::Alloc()
{
#if defined(WIN32) || defined(WINDOWS)
	DWORD tlsIndex = TlsAlloc();
	if(tlsIndex == TLS_OUT_OF_INDEXES)
	{
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: TlsAlloc %s", strerror_r2(errCode).c_str());
		return -1;
	}
	return (int)tlsIndex;
#else
	pthread_key_t threadKey;
	if(pthread_key_create(&threadKey, NULL))
	{
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: TlsAlloc %s", strerror_r2(errCode).c_str());
		return -1;
	}
	return (int)threadKey;
#endif
}

BOOL KThreadLocalStorage::Set(int tlsIndex, void* pData)
{
#if defined(WIN32) || defined(WINDOWS)
	if(!TlsSetValue(tlsIndex, pData))
	{
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: TlsSetValue %s", strerror_r2(errCode).c_str());
		return FALSE;
	}
	return TRUE;
#else
	pthread_key_t threadKey = (pthread_key_t)tlsIndex;
	if(pthread_setspecific(threadKey, pData))
	{
		DWORD errCode = GetLastError();
		Log(LOG_ERROR, "error: TlsSetValue %s", strerror_r2(errCode).c_str());
		return FALSE;
	}
	return TRUE;
#endif
}

BOOL KThreadLocalStorage::Get(int tlsIndex, void*& pData)
{
#if defined(WIN32) || defined(WINDOWS)
	pData = TlsGetValue(tlsIndex);
	if(!pData)
	{
		DWORD errCode = GetLastError();

		// 数据未初始化
		if(errCode == NO_ERROR) return TRUE;

		// 错误
		Log(LOG_ERROR, "error: TlsGetValue %s", strerror_r2(errCode).c_str());
		return FALSE;
	}
	return TRUE;
#else
	pthread_key_t threadKey = (pthread_key_t)tlsIndex;
	pData = pthread_getspecific(threadKey);
	return pData != NULL;
#endif
}
