#include "CommonUtils.h"
#include "../interface/PathFinderDefine.h"
#include "../PerfHitch/perf_PathFinderLib.h"

// 正弦表 (将浮点数 *1024 整型化)
int_r	g_nSin[64] = 
{
		1024,	1019,	1004,	979,	946,	903,	851,	791,
		724,	649,	568,	482,	391,	297,	199,	100,	
        0,	   -100,	-199,	-297,	-391,	-482,	-568,	-649,	
        -724,	-791,	-851,	-903,	-946,	-979,	-1004,	-1019,	
        -1024,	-1019,	-1004,	-979,	-946,	-903,	-851,	-791,	
        -724,	-649,	-568,	-482,	-391,	-297,	-199,	-100,	
        0,	     100,	199,	297,	391,	482,	568,	649,	
        724,	791,	851,	903,	946,	979,	1004,	1019
};

//---------------------------------------------------------------------------
// 余弦表 (将浮点数 *1024 整型化)
int_r	g_nCos[64] = 
{
		0,	    -100,	-199,	-297,	-391,	-482,	-568,	-649,	
        -724,	-791,	-851,	-903,	-946,	-979,	-1004,	-1019,	
        -1024,	-1019,	-1004,	-979,	-946,	-903,	-851,	-791,	
        -724,	-649,	-568,	-482,	-391,	-297,	-199,	-100,	
        0,	     100,	199,	297,	391,	482,	568,	649,	
        724,	791,	851,	903,	946,	979,	1004,	1019,	
        1024,	1019,	1004,	979,	946,	903,	851,	791,	
        724,	649,	568,	482,	391,	297,	199,	100,
};

namespace KPathFinderAbout
{
	int_r	g_InternalDirSinCos(int_r pSinCosTable[], int_r nDir, int_r nMaxDir)
	{	PERF_COUNTER(_G_g_InternalDirSinCos);

		if (nDir < 0 || nDir >= nMaxDir)
			return -1;
		
		int_r	nIndex = (nDir << 6) / nMaxDir;
		
		return pSinCosTable[nIndex];
	}

	int_r g_DirSin(int_r nDir, int_r nMaxDir)
	{	PERF_COUNTER(_G_g_DirSin);

		return g_InternalDirSinCos( g_nSin, nDir, nMaxDir );
	}

	int_r g_DirCos(int_r nDir, int_r nMaxDir)
	{	PERF_COUNTER(_G_g_DirCos);

		return g_InternalDirSinCos( g_nCos, nDir, nMaxDir );
	}

	float g_GetDistance(int_r nX1, int_r nY1, int_r nX2, int_r nY2)
	{	PERF_COUNTER(_G_g_GetDistance);

		return sqrtf(Distance2D(nX1, nY1, nX2, nY2));
	}

	int_r	g_GetDirIndex(int_r nX1, int_r nY1, int_r nX2, int_r nY2)
	{	PERF_COUNTER(_G_g_GetDirIndex);

		int_r	nRet = -1;
		
		if (nX1 == nX2 && nY1 == nY2)
			return -1;

		nX1 = nX1 << 6;
		nY1 = nY1 << 6;
		nX2 = nX2 << 6;
		nY2 = nY2 << 6;
		
		float nDistance = g_GetDistance(nX1, nY1, nX2, nY2);
		
		if (nDistance == 0 ) return -1;
		
		int_r nYLength = nY2 - nY1;
		int_r nSin = (int_r)((nYLength << 10) / nDistance);
		
		int_r nCompensation = 0;
		for (int_r i = 0; i < 32; i++)
		{
			if (nSin > g_nSin[i])
			{
				if (nSin == g_nSin[i - 1])
					nCompensation = 1;
				break;
			}
			nRet = i;
		}
		
		if (nRet != 0)
		{
			if ((nX2 - nX1) >= 0)
			{
				nRet = 63 - nRet;
				if (nCompensation == 1)
				{
					nRet += 1;
				}
			}		
		}

		return nRet;
	}

	int_r g_GetDirDiff(int_r nOriginX, int_r nOriginY, int_r nTargetX, int_r nTargetY, int_r nX, int_r nY)
	{	PERF_COUNTER(_G_g_GetDirDiff);

		int_r nTargetDir = g_GetDirIndex(nOriginX, nOriginY, nTargetX, nTargetY);
		int_r nTestDir = g_GetDirIndex(nOriginX, nOriginY, nX, nY);
		int_r dirDiff = nTargetDir - nTestDir;
		if (dirDiff < 0)
			dirDiff += 64;
		if (dirDiff > 32)
			dirDiff = 64 - dirDiff;
		return dirDiff;
	}

	int_r g_GetDirAngle(int_r nOriginX, int_r nOriginY, int_r nTargetX, int_r nTargetY, int_r nX, int_r nY)
	{	PERF_COUNTER(_G_g_GetDirAngle);

		int_r nTargetDir = g_GetDirIndex(nOriginX, nOriginY, nTargetX, nTargetY);
		int_r nTestDir = g_GetDirIndex(nOriginX, nOriginY, nX, nY);
		int_r dirAngle = nTargetDir - nTestDir;
		if (dirAngle < 0)
			dirAngle += 64;
		return dirAngle;
	}

	int_r g_Dir64To8(int_r nDir)
	{	PERF_COUNTER(_G_g_Dir64To8);

		return ((nDir + 4) >> 3) & 0x07;
	}

	int_r g_Dir8To64(int_r nDir)
	{	PERF_COUNTER(_G_g_Dir8To64);

		return nDir << 3;
	}

	const char * DirToString(_BStar_Direct nDir)
	{
		static char szDir[MAX_PATH] = {0};
		switch(nDir)
		{
		case eBD_Unknow:strcpy(szDir,"unknow direct");break;
		case eBD_Bottom:strcpy(szDir,"eBD_Bottom");break;
		//case eBD_LBottom:strcpy(szDir,"eBD_LBottom");break;
		case eBD_Left:strcpy(szDir,"eBD_Left");break;
		//case eBD_LTop:strcpy(szDir,"eBD_LTop");break;
		case eBD_Top:strcpy(szDir,"eBD_Top");break;
		//case eBD_RTop:strcpy(szDir,"eBD_RTop");break;
		case eBD_Right:strcpy(szDir,"eBD_Right");break;
		//case eBD_RBottom:strcpy(szDir,"eBD_RBottom");break;
		default:ASSERT(FALSE);
		}
		return szDir;
	}
};