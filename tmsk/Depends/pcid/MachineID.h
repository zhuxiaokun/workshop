#ifndef __PCID_MACHINEID_H__
#define __PCID_MACHINEID_H__
#include <windows.h>

#ifdef __cplusplus
extern "C"
{
#endif

bool GetMachineID(char MachineID[17], bool bUseCpu = true, bool bUseHdd = true, bool bUseBios = true);

#ifdef __cplusplus
}
#endif

#endif