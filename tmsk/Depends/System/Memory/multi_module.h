#pragma once
#include "../KType.h"

#if defined(WIN32)

template <typename T,typename P1>
void foreach_module(const char* func, T f, const P1& p1)
{
	typedef void (*SetEncodingProc)(int);
	typedef BOOL (WINAPI *EnumProc)(HANDLE,HMODULE*,DWORD,LPDWORD);

	//f(p1);

	HMODULE hpsapi = LoadLibraryA("psapi.dll");
	if(!hpsapi) return;

	EnumProc enumProc = (EnumProc)GetProcAddress(hpsapi, "EnumProcessModules");
	if(enumProc)
	{
		HMODULE mods[256];
		DWORD cbNeeded = 0;
		HANDLE hProcess = GetCurrentProcess();
		if(enumProc(hProcess, mods, sizeof(mods), &cbNeeded))
		{
			int n = (int)(cbNeeded/sizeof(HMODULE));
			for(int i=0; i<n; i++)
			{
				HMODULE h = mods[i];
				T proc = (T)GetProcAddress(h, func); if(!proc) continue;
				proc(p1);
			}
		}
	}

	::FreeLibrary(hpsapi);
	return;
}

template <typename T,typename P1,typename P2>
void foreach_module(const char* func, T f, const P1& p1, const P2& p2)
{
	typedef void (*SetEncodingProc)(int);
	typedef BOOL (WINAPI *EnumProc)(HANDLE,HMODULE*,DWORD,LPDWORD);

	//f(p1, p2);

	HMODULE hpsapi = LoadLibraryA("psapi.dll");
	if(!hpsapi) return;

	EnumProc enumProc = (EnumProc)GetProcAddress(hpsapi, "EnumProcessModules");
	if(enumProc)
	{
		HMODULE mods[256];
		DWORD cbNeeded = 0;
		HANDLE hProcess = GetCurrentProcess();
		if(enumProc(hProcess, mods, sizeof(mods), &cbNeeded))
		{
			int n = (int)(cbNeeded/sizeof(HMODULE));
			for(int i=0; i<n; i++)
			{
				HMODULE h = mods[i];
				T proc = (T)GetProcAddress(h, func); if(!proc) continue;
				proc(p1, p2);
			}
		}
	}

	::FreeLibrary(hpsapi);
	return;
}

#else

template <typename T,typename P1> void foreach_module(const char* func, T f, const P1& p1)
{
	f(p1);
}

template <typename T,typename P1,typename P2> void foreach_module(const char* func, T f, const P1& p1, const P2& p2)
{
	f(p1, p2);
}

#endif
