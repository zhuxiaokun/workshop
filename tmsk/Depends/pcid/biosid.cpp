#include "biosid.h"
#include "MemoryAccess.h"
#include "md5.h"
#include <windows.h>

#define MINTEXTLEN 8

//提取BIOS里面长度超过8的文本字符串，然后生成摘要作为BIOS INFO
static bool IdentifyBios(BYTE BiosMd5[16], BYTE* bios, SIZE_T length)
{
	BYTE TextBuf[0x10000];
	ZeroMemory(TextBuf, length);

	int TotalTextLen = 0;
	int TextLen = 0;
	BYTE* pBiosEnd = bios + length;
	BYTE* pBiosStart = bios;

	BYTE* pAsc = TextBuf;
	BYTE* pText = TextBuf;
	BYTE* pBios = bios;
	bool  bAscStart = false;

	while(pBios < pBiosEnd)
	{
		if(*pBios >= 32 && *pBios <= 126)	//全部文本字符
		{
			if(!bAscStart)
			{
				pAsc = pText;
				bAscStart = true;
			}

			*pAsc = *pBios;
			pAsc++;
			TextLen++;
		}
		else if(bAscStart)
		{
			bAscStart = false;

			if(TextLen >= MINTEXTLEN)
			{
				TotalTextLen += TextLen;
				pText += TextLen;
			}

			TextLen = 0;
		}
		
		pBios++;
	}

	MD5Digest(BiosMd5, TextBuf, TotalTextLen);

	return (TotalTextLen > MINTEXTLEN);
}

bool ReadBiosInfo(BYTE BiosMd5[16])
{
	BYTE BiosBuf[0x10000];

	OSVERSIONINFO ovi = {0};
	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&ovi);

	DWORD BiosStart = 0xF0000;
	SIZE_T BiosLen = 0x10000;

	if(ovi.dwPlatformId != VER_PLATFORM_WIN32_NT)
		CopyMemory(BiosBuf, (PVOID)BiosStart, BiosLen);
	else if(ReadPhysicalMemoryInNT(BiosBuf, BiosStart, BiosLen) == false)
		return false;
	
	return IdentifyBios(BiosMd5, BiosBuf, BiosLen);
}

//不可靠的搜索方式
/*
int FindAwardBios(BYTE** ppBiosAddr)
{
    BYTE* pBiosAddr = * ppBiosAddr + 0xEC71;

    BYTE szBiosData[128];
    CopyMemory(szBiosData, pBiosAddr, 127);
    szBiosData[127] = 0;
    
    int iLen = lstrlen((char*)szBiosData);
    if(iLen > 0 && iLen < 128)
    {
        //AWard:         07/08/2002-i845G-ITE8712-JF69VD0CC-00 
        //Phoenix-Award: 03/12/2002-sis645-p4s333
        if(szBiosData[2] == '/' && szBiosData[5] == '/')
        {
            BYTE* p = szBiosData;
            while(*p)
            {
                if(*p < ' ' || *p >= 127 )
                {
                    break;
                }
                ++p;
            }
            if(*p == 0 )
            {
                *ppBiosAddr = pBiosAddr;
                return iLen;
            }
        }
    }

    return 0;
}

int FindAmiBios(BYTE** ppBiosAddr)
{
    BYTE* pBiosAddr = *ppBiosAddr + 0xF478;
    
    BYTE szBiosData[128];
    CopyMemory(szBiosData, pBiosAddr, 127);
    szBiosData[127] = 0;
    
    int iLen = lstrlen((char*)szBiosData);
    if(iLen > 0 && iLen < 128)
    {
        // Example: "AMI: 51-2300-000000-00101111-030199-"
        if(szBiosData[2] == '-' && szBiosData[7] == '-')
        {
            BYTE* p = szBiosData;
            while(*p)
            {
                if(* p < ' ' || * p >= 127)
                {
                    break;
                }
                ++p;
            }
            if(*p == 0)
            {
                *ppBiosAddr = pBiosAddr;
                return iLen;
            }
        }
    }

    return 0;
}

int FindPhoenixBios(BYTE** ppBiosAddr)
{
    UINT uOffset[3] = { 0x6577, 0x7196, 0x7550 };
    for( UINT i = 0; i < 3; ++ i )
    {
        BYTE* pBiosAddr = *ppBiosAddr + uOffset[i];

        BYTE szBiosData[128];
        CopyMemory(szBiosData, pBiosAddr, 127);
        szBiosData[127] = 0;

        int iLen = lstrlen((char*)szBiosData);
        if(iLen > 0 && iLen < 128)
        {
            // Example: Phoenix "NITELT0.86B.0044.P11.9910111055"
            if(szBiosData[7] == '.' && szBiosData[11] == '.')
            {
                BYTE* p = szBiosData;
                while(*p)
                {
                    if(*p < ' ' || *p >= 127)
                    {
                        break;
                    }
                    ++p;
                }
                if(*p == 0)
                {
                    *ppBiosAddr = pBiosAddr;
                    return iLen;
                }
            }
        }
    }
    return 0;
}
*/