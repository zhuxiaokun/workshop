#pragma once
#include "../KType.h"

#pragma pack(push, 1)
union KDataChecksum
{
	WORD checksum;
	DWORD ver;
};
#pragma pack(pop)

class KCheckSum
{
public:
	static KDataChecksum CalcChecksum(const void* pData, int len);
	static WORD CalcChecksum(int itemId, int strengthLevel, int privateFlags);
};