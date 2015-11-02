#include "biosid.h"
#include "cpuid.h"
#include "diskid.h"
#include "md5.h"
#include "MachineID.h"
#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif

const char* ID_UNKNOWN = "MACHINEIDUNKNOWN";

BYTE charTable[] = "0123456789abcdefghijklmnoptrstuvwxyzABCDEFGHIJKLMNOPTRSTUVWXYZ+-";

//将MD5串转换为可显示的串
static void reformat(char MachineID[17])
{
	BYTE* tmp = (BYTE*)MachineID;

	for(int i = 0; i < 16; i++)
	{
		//tmp[i] = charTable[(tmp[i] & 0x3F)^((tmp[i] >> 6) & 0x3)];
		//if(tmp[i] < 33 || tmp[i] > 126)
		//	tmp[i] = 33 + tmp[i]%84;
		int n = ((tmp[i] & 0x3f) + (tmp[i]>>6)) & 0x3f;
		tmp[i] = charTable[n];
	}
}

bool GetMachineID(char MachineID[17], bool bUseCpu, bool bUseHdd, bool bUseBios)
{
	ZeroMemory(MachineID, 17);
	bool done = false;

	DWORD diskdata[256];
	BYTE cpuinfo[28];

	MD5_CTX ctx;
	MD5Init(&ctx);

	if(bUseCpu)
	{
		if(ReadCpuInfo(cpuinfo))
		{
			MD5Update(&ctx, cpuinfo, 28);
			done = true;
		}
	}

	OSVERSIONINFO ovi = {0};
	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx( &ovi );

	BYTE biosmd5[16];
	if(bUseBios)
	{
		if(ReadBiosInfo(biosmd5))
		{
			MD5Update(&ctx, (BYTE*)biosmd5, 16);
			done = true;
		}
	}

	if(bUseHdd)
	{
		bool bFindDrive = false;
		if(ovi.dwPlatformId != VER_PLATFORM_WIN32_NT )
		{
			for(int drive = 0; drive < MAX_IDE_DRIVES; drive++)
			{
				if(ReadIdePhysicalDriveIn98(drive, diskdata) && IsFixedIdeDrive(diskdata))
				{
					bFindDrive = true;
					break;
				}
			}
		}
		else
		{
			for(int drive = 0; drive < MAX_IDE_DRIVES; drive++)
			{
				if(ReadIdePhysicalDriveInNT(drive, diskdata) ||
					ReadIdePhysicalDriveInNTByScsi(drive, diskdata))
				{
					if(IsFixedIdeDrive(diskdata))
					{
						bFindDrive = true;
						break;
					}
				}	
			}
		}

		if(bFindDrive)
		{
			char str[256];
			FormatIdeInformation(str, diskdata, DriveSize);
			MD5Update(&ctx, (BYTE*)str, strlen(str));
			FormatIdeInformation(str, diskdata, DriveSerialNumber);
			MD5Update(&ctx, (BYTE*)str, strlen(str));
			FormatIdeInformation(str, diskdata, DriveModelNumber);
			MD5Update(&ctx, (BYTE*)str, strlen(str));
			done = true;
		}

	}

	if(done)
	{
		MD5Final((BYTE*)MachineID, &ctx);
		reformat(MachineID);
	}
	else
		memcpy(MachineID, ID_UNKNOWN, 16);
		
	return done;
}

#ifdef __cplusplus
}
#endif