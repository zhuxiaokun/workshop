#include <windows.h>
#include "miniddk.h"

NtOpenSectionPtr NtOpenSection = NULL;
NtClosePtr NtClose = NULL;
NtMapViewOfSectionPtr NtMapViewOfSection = NULL;
NtUnmapViewOfSectionPtr NtUnmapViewOfSection = NULL;
RtlInitUnicodeStringPtr RtlInitUnicodeString = NULL;
ZwSystemDebugControlPtr ZwSystemDebugControl = NULL;
EnumSystemFirmwareTablesPtr EnumSystemFirmwareTables_2 = NULL;
GetSystemFirmwareTablePtr GetSystemFirmwareTable_2 = NULL;

int LoadNtdllFuncs(void)
{
	HMODULE hNtdll;
	HMODULE hKerneldll;
	
	hNtdll = GetModuleHandle("ntdll.dll");
	hKerneldll = GetModuleHandle("kernel32.dll" );
	if (!(hNtdll && hKerneldll))
		return FALSE;
	
	// should be available across all systems no matter our priv level
	NtOpenSection        = (NtOpenSectionPtr) GetProcAddress(hNtdll, "NtOpenSection");
	NtClose              = (NtClosePtr) GetProcAddress(hNtdll, "NtClose");
	NtMapViewOfSection   = (NtMapViewOfSectionPtr) GetProcAddress(hNtdll, "NtMapViewOfSection");
	NtUnmapViewOfSection = (NtUnmapViewOfSectionPtr) GetProcAddress(hNtdll, "NtUnmapViewOfSection");
	RtlInitUnicodeString = (RtlInitUnicodeStringPtr) GetProcAddress(hNtdll, "RtlInitUnicodeString");
	ZwSystemDebugControl = (ZwSystemDebugControlPtr) GetProcAddress(hNtdll, "ZwSystemDebugControl");
	
	// Donot return false since these APIs are only available on W2K3 SP1 and higher.
	// returning FALSE will break libsmbios on W2K and W2K3( no SP )
	EnumSystemFirmwareTables_2 = (EnumSystemFirmwareTablesPtr) GetProcAddress(hKerneldll, "EnumSystemFirmwareTables");
	GetSystemFirmwareTable_2 = (GetSystemFirmwareTablePtr) GetProcAddress(hKerneldll, "GetSystemFirmwareTable");
	
	return TRUE;
}

//Return true if succeed
bool SetPrivilege(char* privilegeName, bool enable)
{
  HANDLE token_handle;
  bool done = false;

  if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &token_handle))
  {
    LUID luid;
    if(LookupPrivilegeValue(NULL, privilegeName, &luid))
    {
      TOKEN_PRIVILEGES tk_priv;

      tk_priv.PrivilegeCount=1;
      tk_priv.Privileges[0].Attributes=enable ? SE_PRIVILEGE_ENABLED : 0;
      tk_priv.Privileges[0].Luid=luid;

      if(AdjustTokenPrivileges(token_handle, FALSE, &tk_priv, 0, NULL, NULL)) 
		  done = true;
    }

    CloseHandle(token_handle);
  }

  return done;
}

bool ReadPhysicalMemoryInNTByObject(BYTE* OuputBuffer, DWORD& start, SIZE_T& size)
{
	if(	NtOpenSection == NULL || NtMapViewOfSection == NULL || NtUnmapViewOfSection == NULL || 
		RtlInitUnicodeString == NULL ||	NtClose == NULL )
		return false;

	bool done = false;

	HANDLE hPhysicalMemory = NULL;
    WCHAR PhysicalMemoryName[] = L"\\Device\\PhysicalMemory";
    UNICODE_STRING PhysicalMemoryString;
    OBJECT_ATTRIBUTES attributes;

	//打开物理内存对象
    RtlInitUnicodeString(&PhysicalMemoryString, PhysicalMemoryName);
    InitializeObjectAttributes(&attributes, &PhysicalMemoryString, 0, NULL, NULL);
	NTSTATUS status = NtOpenSection(&hPhysicalMemory, SECTION_MAP_READ, &attributes);

	if(NT_SUCCESS(status))
	{
		start -= start%0x1000;			//必须从4K页开始
		PVOID ba = NULL;
		LARGE_INTEGER so;
		SIZE_T ssize = size;
		so.LowPart=start;
		so.HighPart=0x00000000; 
		status = NtMapViewOfSection( 
					hPhysicalMemory,	//打开Section时得到的句柄 
					(HANDLE)0xFFFFFFFF, //将要映射进程的句柄， 
					(VOID**)&ba,          //映射的基址 
					0,
					0x10000,               //分配的大小 
					&so,                  //物理内存的地址 
					&ssize,               //指向读取内存块大小的指针 
					ViewShare,            //子进程的可继承性设定 
					0,                    //分配类型 
					2                     //保护类型 
					);

		if(NT_SUCCESS(status))
		{
			size = min(size, ssize);		//size为请求大小，ssize为实际分配的视图大小
			CopyMemory(OuputBuffer, ba, size);
			NtUnmapViewOfSection((HANDLE)0xFFFFFFFF, ba);
			done = true;
		}
	}

	CloseHandle(hPhysicalMemory);
    
	return done;
}
bool ReadPhysicalMemoryInNTByDebug(BYTE* OuputBuffer, DWORD& start, SIZE_T& size)
{
	if(ZwSystemDebugControl == NULL)
		return false;

	MEM_STRUCT mem;
	ULONG bytesReturned;

	start -= start%0x1000;			//必须从4K页开始

	ZeroMemory(&mem, sizeof(mem));
	mem.Addr = start;
	mem.pBuf = (PVOID)OuputBuffer;
	mem.NumBytes = (DWORD)size;

	NTSTATUS status = ZwSystemDebugControl(DebugSysReadPhysicalMemory,
											&mem,
											sizeof(mem),
											0,
											0,
											&bytesReturned);
 
	if(NT_SUCCESS(status))
	{
		size = (SIZE_T)bytesReturned;
		return true;
	}
	else
		return false;
}

//Only 0xC0000-0xDFFFF and 0xE0000-0xFFFFF supported under win2003sp1.
bool ReadFirmMemoryInNTByFirmware(BYTE* OuputBuffer, DWORD& start, SIZE_T& size)
{
	bool done = false;

	if(EnumSystemFirmwareTables_2 == NULL || GetSystemFirmwareTable_2 == NULL)
		return false;

	DWORD iSignature = 'FIRM';

	UINT iFIRMTableNum = EnumSystemFirmwareTables_2(iSignature, NULL, 0) / sizeof(DWORD);
	
	if(iFIRMTableNum == 0)
		return false;

	DWORD* FirmwareTableBuffer = NULL; 
	FirmwareTableBuffer = new DWORD[iFIRMTableNum];
	UINT* FirmwareTableItemLengthBuffer = NULL;
	FirmwareTableItemLengthBuffer = new UINT[iFIRMTableNum];
	if(FirmwareTableBuffer == NULL || FirmwareTableItemLengthBuffer == NULL)
		return false;

	ZeroMemory(FirmwareTableBuffer, iFIRMTableNum*sizeof(DWORD));
	ZeroMemory(FirmwareTableItemLengthBuffer, iFIRMTableNum*sizeof(UINT));
		
	EnumSystemFirmwareTables_2(iSignature, FirmwareTableBuffer, iFIRMTableNum*sizeof(DWORD));
	for(UINT i = 0; i < iFIRMTableNum; i++)
	{
		FirmwareTableItemLengthBuffer[i] = GetSystemFirmwareTable_2(iSignature, FirmwareTableBuffer[i], NULL, 0);
		if(start >= FirmwareTableBuffer[i] && (start + size) <= (FirmwareTableBuffer[i] + FirmwareTableItemLengthBuffer[i]))
		{
			BYTE* FirmwareTableItemBuffer = NULL;
			FirmwareTableItemBuffer = new BYTE[FirmwareTableItemLengthBuffer[i]];
			if(FirmwareTableItemBuffer == NULL)
				break;
			ZeroMemory(FirmwareTableItemBuffer, FirmwareTableItemLengthBuffer[i]);
			GetSystemFirmwareTable_2(iSignature, FirmwareTableBuffer[i], FirmwareTableItemBuffer, FirmwareTableItemLengthBuffer[i]);
			CopyMemory(OuputBuffer, FirmwareTableItemBuffer + start - FirmwareTableBuffer[i], size);
			delete[] FirmwareTableItemBuffer;
			done = true;
		}
	}
	
	delete[] FirmwareTableBuffer;
	delete[] FirmwareTableItemLengthBuffer;
	return done;
}

int ReadPhysicalMemoryInNT(BYTE* OutputBuffer, DWORD& start, SIZE_T& size)
{
	ZeroMemory(OutputBuffer, size);
	
	SetPrivilege(SE_DEBUG_NAME, true);
	if(!LoadNtdllFuncs())
		return 0;
	
	//三种ring3下读WIN32物理内存方法
	//利用\\Device\\PhysicalMemory，不支持2003sp1/vista
	if(ReadPhysicalMemoryInNTByObject(OutputBuffer, start, size))
	{
		return 1;
	}
	//利用ZwSystemDebugControl，不支持2003sp1/vista
	else if(ReadPhysicalMemoryInNTByDebug(OutputBuffer, start, size))
	{
		return 2;
	}
	//利用SystemFirmwareTable，要求2003sp1/vista，只能读取Firmware(0xC0000-0xFFFFF)
	else if(ReadFirmMemoryInNTByFirmware(OutputBuffer, start, size))
	{
		return 3;
	}
	else
	{
		return 0;
	}
}

