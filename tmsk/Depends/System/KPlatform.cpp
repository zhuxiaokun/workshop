#include "KPlatform.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <locale.h>
#include "Misc/StrUtil.h"
#include "SockHeader.h"
#include "Misc/KStream.h"
#include "File/KTextReader.h"
#include "Memory/multi_module.h"
#include "KAssertCallBack.h"
#include "Misc/KName.h"
EnumEncoding g_encoding = encode_ansii;
KAssertCallBack* g_pAssertCallBack = NULL;

#if defined(__cplusplus)
extern "C"
{
#endif
int is_current_ansii_encoding()
{
	return g_encoding == encode_ansii ? 1 : 0;
}
int is_current_utf8_encoding()
{
	return g_encoding == encode_utf8 ? 1 : 0;
}
_EXPORT_K_ void _set_current_encoding(int encode)
{
	ASSERT(encode >= 0 && encode < encode_count);
	g_encoding = (EnumEncoding)encode;
}

_EXPORT_K_ void _set_Assert_CallBack(KAssertCallBack* pAssertCallBack)
{
	g_pAssertCallBack = pAssertCallBack;
}

_EXPORT_K_ void _set_InitKName(KNameEntryStack* pStack)
{
	KName::SetDefaultDataStack( pStack );
}


#if defined(__cplusplus)
}
#endif

DWORD GetTickDiff(DWORD nOldTicks, DWORD nNewTicks)
{
	if (nNewTicks >= nOldTicks)
	{
		return (nNewTicks - nOldTicks);
	}
	else
	{
		// 这种情况应当认为是有可能有错的，但是有的情况下是正确的。
		DWORD dwDiff = nOldTicks - nNewTicks;
		return (dwDiff);
	}
}

void msleep(int ms)
{
	::Sleep(ms);
}

void yieldThread_k()
{
	::Sleep(0);
}

int printf_k(const char* fmt, ...)
{
	char buf[4096];
	char buf2[4096];

	if(localToCurrent(fmt, buf2, sizeof(buf2)) < 0) return -1;
	else fmt = buf2;
	
	va_list args;
	va_start(args, fmt);
	int length = _vsnprintf_k(buf, sizeof(buf), fmt, args);
	va_end(args);
	if(length < 0) return -1;

	length = currentToLocal(buf, buf2, sizeof(buf2));
	if(length < 0) return -1;

	fwrite(buf2, 1, length, stdout);
	return length;
}

int strcpy_k(char* dst, size_t len, const char* src)
{
	//strcpy(dst, src);
	//return strlen(src);

	if(!dst || len < 1)
	{
		return 0;
	}

	if(!src)
	{
		dst[0] = '\0';
		return 0;
	}

	int i = 0;
	while(src[i])
	{
		dst[i]  = src[i]; i++;
		if(i >= (int)(len-1)) break;
	}

	ASSERT(!src[i]);

	dst[i] = '\0';
	return i;
}

int sprintf_k(char* buf, size_t len, const char* fmt, ...)
{
	if(!fmt) return 0;

	char fmt_2[1024];
	if(localToCurrent(fmt, fmt_2, sizeof(fmt_2)) < 0) return 0;
	else fmt = fmt_2;

	va_list args;
	va_start(args, fmt);
	int length = _vsnprintf_k(buf, len, fmt, args);
	va_end(args);
	return length;
}

DWORD GetModuleFileName_k(HMODULE hModule, char* lpFilename, DWORD nSize)
{
	char path[512];
	DWORD n = GetModuleFileName(hModule, path, sizeof(path));
	if(!n) return 0;
	int len = localToCurrent(path, lpFilename, nSize);
	if(len < 0) return 0;
	return len;
}

DWORD GetCurrentDirectory_k(DWORD nBufferLength, char* lpBuffer)
{
	char path[512];
	DWORD n = GetCurrentDirectory(sizeof(path), path);
	if(!n) return 0;
	int len = localToCurrent(path, lpBuffer, nBufferLength);
	if(len < 0) return 0;
	return len;
}

DWORD ELFHash(const char* szBuffer)
{
	unsigned int hash = 0;
	unsigned int x = 0;
	while (*szBuffer)
	{
		hash = (hash << 4) + (*szBuffer ++);
		if ((x = hash&0xF0000000L) != 0)
		{
			hash ^= (x >> 24);
			hash &= ~x;
		}
	}
	return (hash & 0x7FFFFFFF);
}

const char* backtrace(char* buf, size_t len)
{
	buf[0] = '\0';
	uint_r callstack[64];
	int n = getCallStack(callstack, 64);

	size_t pos = 0;
	for(int i=0; i<n; i++)
	{
		if(pos+12 >= len) break;
		if(i) buf[pos++] = ',';
		pos += sprintf_k(&buf[pos], len-pos, "0x%08p", callstack[i]);
	}

	buf[pos] = '\0';
	return buf;
}

DWORD getLocalAddress()
{
	DWORD addrs[64];
	unsigned char loopback[4] = {1,0,0,127};
	int n = getLocalAddresses(addrs, 64);
	if(!n) return *(DWORD*)&loopback;
	return addrs[n-1];
}

BOOL isLocalAddress(DWORD ipaddr)
{
	DWORD addrs[64];
	int n = getLocalAddresses(addrs, 64);
	if(!n) return FALSE;
	for(int i=0; i<n; i++)
		if(addrs[i] == ipaddr) return TRUE;
	return FALSE;
}

#include <sys/timeb.h>
INT64 GetMilliSeconds()
{
	struct timeb tb;
	memset(&tb, 0, sizeof(tb));
	ftime(&tb);
	return (INT64)tb.time*1000 + tb.millitm;
}

#if defined(WIN32) || defined(WINDOWS)
#include <intrin.h>
#include <direct.h>
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp")
#include <Wbemcli.h>
#include <comutil.h>
#include <sstream>
#pragma comment(lib, "Wbemuuid.lib")
#pragma comment(lib, "comsuppw.lib")
#include <IPHlpApi.h>
#pragma comment(lib,"Iphlpapi.lib")
#pragma comment(lib,"Advapi32.lib")

// 获得系统经过的时钟周期

UINT64 rdtsc(void)
{
	return __rdtsc();
	//unsigned long h;
	//unsigned long l;
	//__asm
	//{
	//	rdtsc
	//		mov h, edx
	//		mov l, eax
	//}
	//return (((UINT64)h<<32)|l);
}

DWORD GetCpuCyclesPerSecond()
{
	HKEY hUser; 
	LONG ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hUser);
	if(ret != ERROR_SUCCESS) return 1000;

	char buf[32];
	DWORD valType;
	DWORD len = sizeof(buf);

	ret = RegQueryValueEx(hUser, "~MHz", NULL, &valType, (BYTE*)&buf, &len);
	if(ret != ERROR_SUCCESS) return 1000;

	DWORD freq = *(DWORD*)&buf;
	freq *= 1000 * 1000;
	return freq;
}

uint_r up_ebp(uint_r ebp)
{
	uint_r uebp = 0;
	__try {
		uebp = *(uint_r*)ebp;
	} __except (EXCEPTION_EXECUTE_HANDLER)
	{ 
		uebp = 0;
	}
	return uebp;
}

uint_r getReturnAddr(uint_r ebp)
{
	uint_r retAddr = 0;
	__try {
		retAddr = *(uint_r*)(ebp+sizeof(void*));
	} __except (EXCEPTION_EXECUTE_HANDLER) 
	{ 
		retAddr = 0;
	}
	return retAddr;
}

int getCallStack(uint_r* callStack, int n)
{
	uint_r addrOfRetAddr = (uint_r)_AddressOfReturnAddress();
	uint_r rebp = addrOfRetAddr - sizeof(uint_r);
	//__asm { mov rebp, ebp };

	rebp = up_ebp(rebp);
	if(!rebp)
	{
		callStack[0] = 0;
		return 0;
	}

	int i = 0;
	while(i < n)
	{
		callStack[i] = getReturnAddr(rebp);
		if(!callStack[i]) break; i++;
		rebp = up_ebp(rebp);
		if(!rebp) break;
	}
	if(i < n) callStack[i] = 0;
	return i;
}

DWORD GetLocalIP()
{
	char szLocalIP[20];
	char szHostName[128];
	
	if(gethostname(szHostName, sizeof(szHostName)))
		return 0;

	hostent* phe = gethostbyname(szHostName); // Get host adresses
	for(int i=0; phe!=NULL && phe->h_addr_list[i]!= NULL; i++)
	{
		sprintf_k(szLocalIP, sizeof(szLocalIP), "%d.%d.%d.%d", 
			(UINT)((UCHAR*)phe->h_addr_list[i])[0],
			(UINT)((UCHAR*)phe->h_addr_list[i])[1],
			(UINT)((UCHAR*)phe->h_addr_list[i])[2], 
			(UINT)((UCHAR*)phe->h_addr_list[i])[3]);
	}

	return inet_addr(szLocalIP);
}

int makedir(const char *dirname)
{
	return _mkdir(dirname);
}

// '/' 到 '\'
static void path_sep_trans(char* out_path, const char* in_path)
{
	while(in_path[0])
	{
		char chr = in_path[0];

		if(chr == '/')
		{
			out_path[0] = '\\';
		}
		else
		{
			out_path[0] = chr;
		}

		in_path += 1;
		out_path += 1;
	}
	out_path[0] = '\0';
}

bool isabspath(const char* path)
{
	ASSERT(path && path[0]);
	char drive = path[0];
	if((drive >= 'a' && drive <= 'z') || (drive >= 'A' && drive <= 'Z'))
		return path[1] == ':';
	return false;
}

char* getfullpath(char* fullPath, const char* path, size_t len)
{
	char out_path[512];
	path_sep_trans(out_path, path);

	char path1[512];
	int n = currentToLocal(out_path, path1, sizeof(path1));
	if(n < 0) return NULL;

	char path2[512];
	char* p = _fullpath(path2, path1, sizeof(path2));
	if(!p) return NULL;

	n = localToCurrent(p, fullPath, len);
	if(n < 0) return NULL;
	return fullPath;
}

BYTE ror(BYTE chr, int bits)
{
	return _rotr8(chr, (unsigned char)bits);
}

BYTE rol(BYTE chr, int bits)
{
	return _rotl8(chr, (unsigned char)bits);
}

static LONG WINAPI DumpWriter(PEXCEPTION_POINTERS ExceptionInfo)
{
	char buff[200] = {0};
	sprintf_k(buff, 200, "%d.%d.dmp", GetCurrentProcessId(), rand());

	char path[512];
	if(currentToLocal(buff, path, sizeof(path)) < 0)
		return EXCEPTION_EXECUTE_HANDLER;

	HANDLE lhDumpFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);
	if (lhDumpFile)
	{
		MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
		loExceptionInfo.ExceptionPointers = ExceptionInfo;
		loExceptionInfo.ThreadId = GetCurrentThreadId();
		loExceptionInfo.ClientPointers = TRUE;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), lhDumpFile, MiniDumpWithFullMemory, &loExceptionInfo, NULL, NULL);

		CloseHandle(lhDumpFile);
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void create_dump()
{
	__try
	{
		*(int*)0 = 0;
	}
	__except(DumpWriter(GetExceptionInformation()))
	{}
}

static bool _querySystemInfoImpl(char* buff, int len, const char* queryString, const char* queryColumn)
{
	HRESULT hres;

	IWbemLocator *pLoc = NULL;

	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0, 
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *) &pLoc);

	if (FAILED(hres))
	{
		return false;
	}

	IWbemServices *pSvc = NULL;

	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
		NULL,                    // User name. NULL = current user
		NULL,                    // User password. NULL = current
		0,                       // Locale. NULL indicates current
		NULL,                    // Security flags.
		0,                       // Authority (e.g. Kerberos)
		0,                       // Context object 
		&pSvc                    // pointer to IWbemServices proxy
		);

	if (FAILED(hres))
	{
		pLoc->Release();
		return false;
	}

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
		NULL,                        // Server principal name
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities
		);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		return false;
	}

	IEnumWbemClassObject* pEnumerator = NULL;

	IWbemClassObject *pclsObj = NULL;
	ULONG uReturn = 0;

	hres = pSvc->ExecQuery(
		bstr_t("WQL"),
		bstr_t(queryString),
		WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
		NULL,
		&pEnumerator);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		return false;
	}

	//只存第一个网卡的地址
	if (pEnumerator)
	{
		INT i = 1;
		HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

		if(0 == uReturn)
		{
			if (pclsObj)
			{
				pclsObj->Release();
			}
			pEnumerator->Release();
			pSvc->Release();
			pLoc->Release();
			return false;
		}

		VARIANT vtProp;
		VariantInit(&vtProp);

		hr = pclsObj->Get(bstr_t(queryColumn), 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr))
		{
			std::stringstream ss;
			switch(vtProp.vt)
			{
			case VT_BSTR:
				ss << (_bstr_t)vtProp.bstrVal;
				break;
			case VT_I1:
			case VT_UI1:
				ss << vtProp.cVal;
				break;
			case VT_I2:
				ss << vtProp.iVal;
				break;
			case VT_I4:
				ss << vtProp.intVal;
				break;
			case VT_UI2:
				ss << vtProp.uiVal;
				break;
			case VT_UI4:
				ss << vtProp.uintVal;
				break;
			case VT_INT:
				ss << vtProp.intVal;
				break;
			case VT_UINT:
				ss << vtProp.uintVal;
				break;
			case VT_R4:
				ss << vtProp.fltVal;
				break;
			case VT_R8:
				ss << vtProp.dblVal;
				break;
			default:
				pclsObj->Release();
				pEnumerator->Release();
				pSvc->Release();
				pLoc->Release();
				return false;
			}
			strcpy_k(buff, len, ss.str().c_str());
		}

		VariantClear(&vtProp);
	}

	pclsObj->Release();
	pEnumerator->Release();
	pSvc->Release();
	pLoc->Release();
	return true;
}

bool _querySystemInfo(char* buff, int len, const char* queryString, const char* queryColumn)
{
	HRESULT hres;

	bool result = true;
	bool needUninitialize = true;
	hres =  CoInitializeEx(0, COINIT_MULTITHREADED);
	if (hres == S_FALSE || hres == RPC_E_CHANGED_MODE)
	{
		needUninitialize = false;
	}
	else if (FAILED(hres))
	{
		return false;
	}
	else
	{
		//如果是第一次创建则设定安全级别
		hres =  CoInitializeSecurity(
			NULL, 
			-1,                          // COM authentication
			NULL,                        // Authentication services
			NULL,                        // Reserved
			RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication
			RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
			NULL,                        // Authentication info
			EOAC_NONE,                   // Additional capabilities
			NULL                         // Reserved
			);

		if (FAILED(hres))
		{
			CoUninitialize();
			return false;
		}
	}

	try{
		result = _querySystemInfoImpl(buff, len, queryString, queryColumn);
	}catch(...)
	{
		result = false;
	}

	if (needUninitialize)
	{
		CoUninitialize();
	}

	return result;
}

bool getMacAddr(char* buff, int len)
{
	char mac[] = "00:00:00:00:00:00";
	if (len < sizeof(mac)) return false;
	buff[0] = 0;

	bool result = _querySystemInfo(buff, len
		, "SELECT MACAddress FROM Win32_NetworkAdapter WHERE ((MACAddress Is Not NULL) AND (Manufacturer <> 'Microsoft'))"
		, "MACAddress");

	//没有找到的话换一种方法找
	if (!result)
	{
		//只要一块的
		char buffer[10240];
		IP_ADAPTER_INFO& adapterInfo = *(IP_ADAPTER_INFO*)&buffer;
		ULONG dwSize = sizeof(buffer);

		ULONG ret = GetAdaptersInfo(&adapterInfo, &dwSize);
		if(ret == ERROR_BUFFER_OVERFLOW)
		{
			adapterInfo = *(IP_ADAPTER_INFO*)new char[dwSize];
			ret = GetAdaptersInfo(&adapterInfo, &dwSize);
		}

		if(ret == NO_ERROR)
		{
			sprintf_k(buff, len, "%02X:%02X:%02X:%02X:%02X:%02X"
				, adapterInfo.Address[0]
			, adapterInfo.Address[1]
			, adapterInfo.Address[2]
			, adapterInfo.Address[3]
			, adapterInfo.Address[4]
			, adapterInfo.Address[5]
			);
		}

		if((void*)&adapterInfo != &buffer)
			delete [] (char*)&adapterInfo;

		return (ret == NO_ERROR);
	}

	return result;
}

bool getMacAddr(BYTE mac[6])
{
	char buf[64];
	if(!getMacAddr(buf, sizeof(buf)))
		return false;

	char* ss[6];
	int ns = split(buf, ':', ss, 6);
	if(ns != 6) return false;

	for(int i=0; i<ns; i++)
	{
		mac[i] = (BYTE)strtoul(ss[i], NULL, 16);
	}

	return true;
}

bool getProcessorID(char* buff, int len)
{
	return _querySystemInfo(buff, len
		, "SELECT ProcessorId FROM Win32_Processor"
		, "ProcessorId");
}

int getLocalAddresses(DWORD* addrs, size_t n)
{
	struct hostent* pHostent = gethostbyname(NULL);
	if(!pHostent)
	{
		if(GetLastError() == WSANOTINITIALISED)
		{
			WSADATA wsaData;
			WSAStartup(MAKEWORD(2,2), &wsaData);
			pHostent = gethostbyname(NULL);
		}
	} if(!pHostent) return 0;

	if(pHostent->h_addrtype != AF_INET) return 0;
	if(pHostent->h_length != sizeof(DWORD)) return 0;
	size_t i = 0; for(; i<n && pHostent->h_addr_list[i]; i++)
		addrs[i] = ntohl(*(DWORD*)pHostent->h_addr_list[i]);
	return (int)i;
}

#include <Rpcdce.h>
#pragma comment(lib, "Rpcrt4.lib")
INT128 createUuid()
{
	UUID uid;
	UuidCreate(&uid);
	return *(INT128*)&uid;
}

uint_r getCurrentThreadId()
{
	return GetCurrentThreadId();
}

void _init_va_list(va_list* ap, void* p)
{
	*ap = (char*)p;
}

void setProcessEncoding(int encode)
{
	typedef void (*SetEncodingProc)(int);
	typedef BOOL (WINAPI *EnumProc)(HANDLE,HMODULE*,DWORD,LPDWORD);

	_set_current_encoding(encode);
	HMODULE hpsapi = LoadLibrary("psapi.dll");
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
				SetEncodingProc proc = (SetEncodingProc)GetProcAddress(h, "_set_current_encoding");
				if(!proc) continue;
				proc(encode);
			}
		}
	}

	::FreeLibrary(hpsapi);
	return;
}

void setAssertCallBack(KAssertCallBack* pAssertCallBack)
{
	foreach_module("_set_Assert_CallBack", _set_Assert_CallBack, pAssertCallBack);
}

void setInitStaticKName(KNameEntryStack* pStack)
{
	foreach_module("_set_InitKName", _set_InitKName, pStack);
}

const char* setLocaleToGbk()
{
	const char* org = setlocale(LC_CTYPE, NULL);
	setlocale(LC_CTYPE, ".936");
	return org;
}

BOOL SetWindowText_k(void* hwnd, const char* text)
{
	char buf[4096];
	if(!text) return SetWindowTextA((HWND)hwnd, text);
	if(currentToLocal(text, buf, sizeof(buf)) < 0)
		return SetWindowTextA((HWND)hwnd, text);
	return SetWindowTextA((HWND)hwnd, buf);
}

BOOL assureOneProgramInstance()
{
	char buf[512];
	DWORD len = GetModuleFileName_k(NULL, buf, sizeof(buf));
	
	char fileName[512];
	getfullpath(fileName, buf, sizeof(fileName));

	char* name = fileName;
	while(*name)
	{
		if(*name == PATH_SEPERATOR) *name = '-';
		else *name = tolower(*name);
		name++;
	}

	if(!CreateMutex(NULL, FALSE, fileName))
	{
		Log(LOG_ERROR, "error: CreateMutex('%s'), %s", fileName, strerror_r2(GetLastError()).c_str());
		return FALSE;
	}

	return GetLastError() != ERROR_ALREADY_EXISTS;
}

void sendmail(const char* subject, const char* body, const char* recipient)
{
	Log(LOG_INFO, "send to: %s", recipient);
	Log(LOG_INFO, subject);
	Log(LOG_INFO, body);
}

#else

#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
//#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static long getMillsecTicks()
{
	KInputFileStream fi;
	ASSERT(fi.Open("/proc/cpuinfo"));

	::System::File::KColumnReader r;
	r.SetInputStream(fi);

	char** cols;
	while(TRUE)
	{
		int n = r.ReadLine(cols, ':');
		if(!n) continue;
		if(n < 0) break;

		char* name = _trim(cols[0]);
		if(strcmp(name, "model name") == 0)
		{
			// Intel(R) Core(TM)2 Duo CPU     E7300  @ 2.66GHz
			char* v = _trim(cols[1]);
			int len = strlen(v);
			if(len > 5)
			{
				char ch1 = v[len-3];
				char ch2 = v[len-2];
				char ch3 = v[len-1];

				if(ch2 == 'H' && ch3 == 'z')
				{
					int unitHz = 1;
					if(ch1 == 'G')
					{
						unitHz = 1000*1000*1000;
					}
					else if(ch1 == 'M')
					{
						unitHz = 1000*1000;
					}
					else if(ch1 == 'K')
					{
						unitHz = 1000;
					}

					int pos = len - 1;
					while(pos >= 0)
					{
						if(v[pos] == ' ' || v[pos] == '\t')
						{
							pos += 1;
							break;
						}
						pos -= 1;
					}
					if(pos < 0) pos = 0;
					v = v + pos;

					float cpu_freq;
					if(sscanf(v, "%f", &cpu_freq) < 1)
					{
						continue;
					}
					cpu_freq *= unitHz;

					return (long)(cpu_freq/1000);
				}
			}
		}
		else if(strcmp(name, "cpu MHz") == 0)
		{
			char* v = _trim(cols[1]);
			double mhz = strtod(v, NULL);
			return (long)(mhz * 1000);
		}
	}
	ASSERT(FALSE);
	return 0;
}

UINT64 rdtsc(void)
{
#if defined(__clang__)
	return 0;
	//	unsigned long long retval;
	//	unsigned long junk;
	//	__asm__ __volatile__ ("\n\
	//						  1:      mftbu   %1\n\
	//						  mftb    %L0\n\
	//						  mftbu   %0\n\
	//						  cmpw    %0,%1\n\
	//						  bne     1b"
	//						  : "=r" (retval), "=r" (junk));
	//	return retval;
#else
#if defined(__x64__)
	unsigned hi, lo;
	__asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
#else
	unsigned long long int x;
	__asm__ volatile ("rdtsc" : "=A" (x));
	return x;
#endif
#endif
}

DWORD GetCpuCyclesPerSecond()
{
	return (DWORD)getMillsecTicks() * 1000;
}

DWORD GetLocalIP()
{
	struct ifreq ifr[32];
	struct ifconf ifc;
	char ipaddr[32] = "";
	int num,i;
	int sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock >= 0)
	{    
		ifc.ifc_len = sizeof ifr;
		ifc.ifc_buf = (caddr_t) ifr;
		ifc.ifc_req = ifr;
		if (!ioctl(sock, SIOCGIFCONF, (char *)&ifc) )
		{
			num = ifc.ifc_len /sizeof (struct ifreq);
			while (num > 0)
			{
				if (!ioctl(sock, SIOCGIFADDR, (char *) &ifr[num]))
				{
					return inet_addr((char*)inet_ntoa(((struct sockaddr_in*)(&ifr[num].ifr_addr) )->sin_addr));
				}
				num--;
			}
		}
	}
	return 0;
}

#include "Time/KMilliTimer.h"
static KMilliTimer _g_milliTimerInstance;

DWORD get_tick_count_2()
{
	timeval tv;
	unsigned long ms = 0;
	gettimeofday(&tv, NULL);
	static TIME_T firstsecs = tv.tv_sec;
	tv.tv_sec -= firstsecs;
	ms = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	return ms;
}

const wchar_16 *wcschr_k(const wchar_16 *str, wchar_16 c)
{
	int i = 0; wchar_16 chr;
	while(chr = str[i], chr)
	{
		if(c == chr) return str+i; i++;
	}
	return NULL;
}

DWORD GetCurrentThreadId()
{
	pthread_t tid = pthread_self();
	return (DWORD)(uint_r)tid;
}

INT64 _strtoi64(const char *nptr, char **endptr, int base)
{
	return strtoll(nptr, endptr, base);
}

void Sleep(DWORD msec)
{
	if(msec < 0) return;
	struct timespec ts;
	ts.tv_sec = (TIME_T)(msec / 1000);
	ts.tv_nsec = (msec % 1000) * 1000000;
	nanosleep(&ts, NULL);
}

DWORD GetModuleFileName(HMODULE hModule, char* lpFilename, DWORD nSize)
{
	ASSERT(hModule == NULL);
	int rslt = readlink("/proc/self/exe", lpFilename, nSize);
	if(rslt < 0 || rslt >= nSize)
	{
		sprintf(lpFilename, ".");
		return 1;
	}
	lpFilename[rslt] = '\0';
	return rslt;
	//else
	//{
	//	lpFilename[rslt] = '\0';
	//	char *s = strrchr(lpFilename, '/');
	//	if(s != NULL)
	//	{
	//		s[0] = '\0';
	//		return (DWORD)(s - &lpFilename[0]);
	//	}
	//	else
	//	{
	//		strcpy(lpFilename, ".");
	//		return 1;
	//	}
	//}
}

//int _stricmp(const char *string1, const char *string2)
//{
//	return strcasecmp(string1, string2);
//}
//
//int stricmp(const char *string1, const char *string2)
//{
//	return strcasecmp(string1, string2);
//}

int strnicmp(const char* s1, const char* s2, int size)
{
	return strncasecmp(s1, s2, size);
}

int makedir(const char *dirname)
{
	return ::mkdir(dirname, 0777);
}

int closesocket(SOCKET s)
{
	return close(s);
}

DWORD WSAGetLastError()
{
	return errno;
}

int ioctlsocket(SOCKET s, long cmd, ULONG* argp)
{
	return fcntl(s, cmd, argp);
}

DWORD GetLastError()
{
	return errno;
}

DWORD GetCurrentDirectory(DWORD nBufferLength, char* lpBuffer)
{
	char* pCwd = getcwd(lpBuffer, nBufferLength);
	if(!pCwd) return 0;
	return strlen(pCwd);
}

//int get_thread_area(struct user_desc* ptr)
//{
//	int res;
//	__asm__ __volatile__( "pushl %%ebx\n\t"
//		"movl %2,%%ebx\n\t"
//		"int $0x80\n\t"
//		"popl %%ebx"
//		: "=a" (res)
//		: "0" (244) /* SYS_get_thread_area */, "q" (ptr) );
//	if (res >= 0) return res;
//	errno = -res;
//	return -1;
//}
//
//int set_thread_area (struct user_desc* ptr)
//{
//	int res;
//	__asm__ __volatile__( "pushl %%ebx\n\t"
//		"movl %2,%%ebx\n\t"
//		"int $0x80\n\t"
//		"popl %%ebx"
//		: "=a" (res)
//		: "0" (243) /* SYS_set_thread_area */, "q" (ptr) );
//	if (res >= 0) return res;
//	errno = -res;
//	return -1;
//}

static void path_sep_trans(char* out_path, const char* in_path)
{
	while(in_path[0])
	{
		char chr = in_path[0];

		if(chr == '\\')
		{
			out_path[0] = '/';
		}
		else
		{
			out_path[0] = chr;
		}

		in_path += 1;
		out_path += 1;
	}
	out_path[0] = '\0';
}

// 改变目录分隔符，从 '\' 到 '/'
static int path_top_name(const char*& path, char* topName)
{
	char chr;
	int len = 0;

	while(chr=path[len], chr!='/' && chr!='\0')
	{
		topName[len] = chr;
		len += 1;
	}

	if(chr == '/')
	{
		path += len+1;
	}
	else
	{
		path += len;
	}

	topName[len] = '\0';
	return len;
}

static BOOL path_up(char* path, int& len)
{
	len -= 1;
	while(len > 0)
	{
		char chr = path[len];
		if(chr == '/')
		{
			path[len] = '\0';
			return TRUE;
		}
		len -= 1;
	}
	return FALSE;
}

bool isabspath(const char* path)
{
	ASSERT(path && path[0]);
	return path[0] == '/';
}

char* getfullpath(char* fullPath, const char* path, size_t len)
{
	int pathLen;
	char topName[64];
	fullPath[0] = '\0';

	char out_path[512];
	path_sep_trans(out_path, path);
	path = &out_path[0];

	// 本身就是绝对路径
	if(path[0] == '/')
	{
		pathLen = 0;

		path += 1;
		strcat(fullPath, "/");
		pathLen += 1;

		while(path[0])
		{
			int len = path_top_name(path, topName);
			if(!len) continue;
			if(strcmp(topName, ".") == 0) continue;
			if(strcmp(topName, "..") == 0)
			{
				if(!path_up(fullPath, pathLen)) return NULL;
				continue;
			}
			if(fullPath[pathLen-1] != '/')
			{
				strcat(fullPath, "/");
				pathLen += 1;
			}
			strcat(fullPath, topName);
			pathLen += len;
		}
		return fullPath;
	}

	if(!GetCurrentDirectory_k(len, fullPath))
		return NULL;

	pathLen = strlen(fullPath);
	while(path[0])
	{
		int len = path_top_name(path, topName);
		if(!len) continue;
		if(strcmp(topName, ".") == 0) continue;
		if(strcmp(topName, "..") == 0)
		{
			if(!path_up(fullPath, pathLen)) return NULL;
			continue;
		}
		if(fullPath[pathLen-1] != '/')
		{
			strcat(fullPath, "/");
			pathLen += 1;
		}
		strcat(fullPath, topName);
		pathLen += len;
	}
	return fullPath;
}

//
BYTE ror(BYTE chr, int bits)
{
#if defined(__clang__)
    return 0;
#else
	int ret;
	__asm__ __volatile__(
		"rorb  %%cl,%%al        \n\t"
		: "=r" (ret)
		: "a" (chr), "c" (bits)
		);

	return (BYTE)(ret & 0xff);
#endif
}

BYTE rol(BYTE chr, int bits)
{
#if defined(__clang__)
    return 0;
#else
	int ret;
	__asm__ __volatile__(
		"rolb  %%cl,%%al        \n\t"
		: "=r" (ret)
		: "a" (chr), "c" (bits)
		);

	return (BYTE)(ret & 0xff);
#endif
}

void create_dump()
{
#if defined(__APPLE__)
#define SIGCLD SIGCHLD
#endif
	signal(SIGCLD, SIG_IGN);
	if (fork() == 0)
	{
		rlimit res;
		res.rlim_cur = RLIM_INFINITY;
		res.rlim_max = RLIM_INFINITY;
		setrlimit(RLIMIT_CORE, &res);
		*(int*)0 = 0;
	}
}

#include <execinfo.h>
int getCallStack(uint_r* callStack, int n)
{
	size_t i = backtrace((void**)callStack, n);
	if(i < n) callStack[i] = 0;
	return i;
}

#include <uuid/uuid.h>
INT128 createUuid()
{
	uuid_t uid;
	uuid_generate(uid);
	return *(INT128*)&uid;
}

uint_r getCurrentThreadId()
{
	return (uint_r)pthread_self();
}

#if !defined(__x64__)
void _init_va_list(va_list* ap, void* p)
{
	*ap = (char*)p;
}
#else
struct _x64_va_list
{
	unsigned int gp_offset;
	unsigned int fp_offset;
	char* overflow_arg_area;
	char* reg_save_area;
};
void _init_va_list(va_list* ap, void* p)
{
	_x64_va_list* pArgs = (_x64_va_list*)(void*)ap;
	pArgs->gp_offset = 6*8;
	pArgs->fp_offset = 6*8 + 8*16;
	pArgs->overflow_arg_area = (char*)p;
	pArgs->reg_save_area = NULL;
}
#endif

void setProcessEncoding(int encode)
{
	_set_current_encoding(encode);
}

const char* setLocaleToGbk()
{
	const char* org = setlocale(LC_CTYPE, NULL);
	setlocale(LC_CTYPE, "zh_CN.GB18030");
	return org;
}

BOOL SetWindowText_k(void* hwnd, const char* text)
{
	return FALSE;
}

int getLocalAddresses(DWORD* addrs, size_t n)
{
	int fd;
	struct ifreq ifr;
	struct sockaddr_in* sin;
	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if(fd < 0) return 0;

	int i = 0; while(i<n)
	{
		memset(&ifr, 0x00, sizeof(ifr));
		sprintf(ifr.ifr_name, "eth%d", i);
		if(ioctl(fd, SIOCGIFADDR, &ifr)) break;
		sin = (struct sockaddr_in*)&ifr.ifr_addr;
		addrs[i++] = ntohl(sin->sin_addr.s_addr);
	}
	
	close(fd);
	return i;
}

// 保证程序只有一个实例
BOOL assureOneProgramInstance()
{
	char buf[512];
	DWORD len = GetModuleFileName_k(NULL, buf, sizeof(buf));

	char fileName[512];
	getfullpath(fileName, buf, sizeof(fileName));

	char* slash = strrchr(fileName, PATH_SEPERATOR);
	ASSERT(slash);

	char shortName[64];
	sprintf_k(shortName, sizeof(shortName), ".%s.lock", slash+1);
	strcpy_k(slash+1, sizeof(fileName) - (slash+1-&fileName[0]), shortName);

	int fd = open(fileName, O_CREAT|O_RDWR, 00777);
	if(fd == -1)
	{
		Log(LOG_ERROR, "error: open('%s'), %s", strerror_r2(errno).c_str());
		return FALSE;
	}

	struct flock ldata;
	memset(&ldata, 0, sizeof(ldata));

	ldata.l_type = F_WRLCK;
	ldata.l_whence = SEEK_SET;
	ldata.l_start = 0;
	ldata.l_len = sizeof(int_r);
	
	return !fcntl(fd, F_SETLK, &ldata);
}

void sendmail(const char* subject, const char* body, const char* recipient)
{
	FILE* fp;
	char buffer[4096];
	sprintf_k(buffer,sizeof(buffer), "echo \"%s\" |mail -s \"%s\" \"%s\"", body, subject, recipient);
	if(strlen(buffer) >= sizeof(buffer) - 2)
	{
		Log(LOG_ERROR, "send mail error. mail too long");
		return;
	}
	fp=popen(buffer, "r");
	if (fp)
		pclose(fp);

}
#endif
