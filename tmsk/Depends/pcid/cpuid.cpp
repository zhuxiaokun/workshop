#include <windows.h>

typedef struct _CPUINFO
{
	DWORD Vendor1;
	DWORD Vendor2;
	DWORD Vendor3;
	DWORD Feature;
	DWORD Serial1;
	DWORD Serial2;
	DWORD Serial3;
} CPUINFO, *PCPUINFO;

bool ReadCpuInfo(BYTE CpuInfoBuffer[28])
{
	PCPUINFO pCi = (PCPUINFO)CpuInfoBuffer;

	DWORD dwCpuidExist = 0;
	DWORD dwMaxArg;			//maximum value of CPUID argument supported by the CPU

	DWORD Vendor1 = 0;;
	DWORD Vendor2 = 0;
	DWORD Vendor3 = 0;
	DWORD Feature = 0;
	DWORD Serial1 = 0;
	DWORD Serial2 = 0;
	DWORD Serial3 = 0;

	//Check for CPUID support
	_asm 
	{
		pushfd
		pop eax
		mov ecx, eax
		xor eax, 200000h
		push eax
		popfd
		pushfd
		pop eax
		xor eax, ecx
		test eax, 200000h
		jnz Y_CPUID
		jmp N_CPUID
Y_CPUID:
		mov dwCpuidExist, 1
N_CPUID:
	}

	if(dwCpuidExist == 0) return false;	//not support CPUID

	_asm 
	{
		mov eax, 0				//Get vendor and max argument
		cpuid
		mov dwMaxArg, eax
		mov Vendor1, ecx
		mov Vendor2, edx
		mov Vendor3, ebx
		
		mov eax, 1				//Get serial3（signature) and feature
		cpuid
		mov Serial3, eax
		mov Feature, edx
	}

	//Get serial1 and serial2
	if((Feature & (((DWORD)1)<<18)) && dwMaxArg > 3) 
	{
		_asm 
		{
			mov eax, 3
			xor edx, edx
			xor ecx, ecx
			cpuid
			mov Serial1, ecx
			mov Serial2, edx
		}
	}

	pCi->Vendor1 = Vendor1;
	pCi->Vendor2 = Vendor2;
	pCi->Vendor3 = Vendor3;
	pCi->Feature = Feature;
	pCi->Serial1 = Serial1;
	pCi->Serial2 = Serial2;
	pCi->Serial3 = Serial3;

	return true;
}






