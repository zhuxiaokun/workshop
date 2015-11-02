#pragma once

#include "KType.h"
#include "KMacro.h"
#include "Memory/KMemory.h"
#include <string.h>
#include "Misc/KEncoding.h"

class KAssertCallBack;
class KNameEntryStack;

extern EnumEncoding g_encoding;
DWORD GetTickDiff(DWORD nOldTicks, DWORD nNewTicks);

void msleep(int ms);
void yieldThread_k();
UINT64 rdtsc(void);
DWORD GetCpuCyclesPerSecond();

int printf_k(const char* fmt, ...);
int strcpy_k(char* dst, size_t len, const char* src);
int sprintf_k(char* buf, size_t len, const char* fmt, ...);
DWORD GetModuleFileName_k(HMODULE hModule, char* lpFilename, DWORD nSize);
DWORD GetCurrentDirectory_k(DWORD nBufferLength, char* lpBuffer);
BOOL SetWindowText_k(void* hwnd, const char* text);

DWORD GetLocalIP();

DWORD getLocalAddress();
int   getLocalAddresses(DWORD* addrs, size_t n);
BOOL  isLocalAddress(DWORD ipaddr);

DWORD ELFHash(const char* szBuffer);

void create_dump();
int getCallStack(uint_r* callStack, int n);
const char* backtrace(char* buf, size_t len);

INT128 createUuid();
uint_r getCurrentThreadId();

void _init_va_list(va_list* ap, void* p);

void setProcessEncoding(int encode);

void setAssertCallBack(KAssertCallBack* pAssertCallBack);

void setInitStaticKName(KNameEntryStack* pStack);

const char* setLocaleToGbk();

// 保证程序只有一个实例
BOOL assureOneProgramInstance();

// 1970.1.1 0:0:0
INT64 GetMilliSeconds();

bool isabspath(const char* path);
char* getfullpath_2(const char* cwd, const char* path, char* fullpath, size_t len);
void sendmail(const char* subject, const char* body, const char* recipient);

#if defined(WIN32) || defined(WINDOWS)

#define _EXPORT_K_ __declspec(dllexport)

#if (_MSC_VER < 1500)
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#endif

#define get_tick_count GetTickCount
#define get_tick_count_2 GetTickCount
#define wcschr_k wcschr

int makedir(const char *dirname);

char* getfullpath(char* fullPath, const char* path, size_t len);

// ror
BYTE ror(BYTE ch, int bits);
BYTE rol(BYTE ch, int bits);

bool getMacAddr(char* buff, int len);
bool getMacAddr(BYTE mac[6]);

bool getProcessorID(char* buff, int len);
int getCallStack(uint_r rebp, uint_r* callStack, int n);

#else

#include <errno.h>
#include <unistd.h>

#define _EXPORT_K_

#define ERROR_SUCCESS (0)
#define WSAEWOULDBLOCK (EWOULDBLOCK)
#define _snprintf snprintf
#define _strnicmp strncasecmp

DWORD get_tick_count_2();
inline DWORD get_tick_count() { return get_tick_count_2(); }

extern volatile DWORD _g_nowTick;
extern volatile BOOL _g_milliTimerStarted;
#define GetTickCount() (_g_milliTimerStarted ? _g_nowTick : get_tick_count_2())

const wchar_16 *wcschr_k(const wchar_16 *str, wchar_16 c);

DWORD GetCurrentThreadId();
INT64 _strtoi64(const char *nptr, char **endptr, int base);
void  Sleep(DWORD msec);
DWORD GetModuleFileName(HMODULE hModule, char* lpFilename, DWORD nSize);
int   strnicmp(const char* s1, const char* s2, int size);
int   makedir(const char *dirname);
int   closesocket(SOCKET s);
DWORD WSAGetLastError();
int ioctlsocket(SOCKET s, long cmd, ULONG* argp);
DWORD GetLastError();
DWORD GetCurrentDirectory(DWORD nBufferLength, char* lpBuffer);

//int get_thread_area(struct user_desc *u_info);
//int set_thread_area (struct user_desc *u_info);
char* getfullpath(char* fullPath, const char* path, size_t len);

// ror
BYTE ror(BYTE ch, int bits);
BYTE rol(BYTE ch, int bits);

#define WINAPI
#endif

DECLARE_SIMPLE_TYPE(INT128);
