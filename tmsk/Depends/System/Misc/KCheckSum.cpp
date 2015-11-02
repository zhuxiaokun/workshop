#include "KCheckSum.h"

KDataChecksum KCheckSum::CalcChecksum(const void* pData, int len)
{
	KDataChecksum cs;
	cs.ver = 0;
	BYTE* byArray = (BYTE*)&cs;

	const BYTE* p = (const BYTE*)pData;
	for(int i=0; i<len; i++)
	{
		byArray[0] += *p;
		byArray[1] += (*p >> 1);
		p += 1;
	}
	return cs;
}

WORD KCheckSum::CalcChecksum( int itemId, int strengthLevel, int privateFlags )
{
	WORD checksum = 0;

	checksum += itemId;
	checksum += itemId >> 1;
	checksum += strengthLevel;
	checksum += strengthLevel >> 1;
	checksum += privateFlags;
	checksum += privateFlags >> 1;

	return checksum;
}