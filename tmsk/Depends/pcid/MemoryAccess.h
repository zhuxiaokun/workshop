#ifndef __PHYSICS_MEMORY_ACCESS_H__
#define __PHYSICS_MEMORY_ACCESS_H__
#include <windows.h>

int ReadPhysicalMemoryInNT(BYTE* OuputBuffer, DWORD& start, SIZE_T& size);

#endif