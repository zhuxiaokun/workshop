
#ifndef _Common_Utils_H
#define _Common_Utils_H

#include <math.h>
#include <System/KType.h>

// ���ұ� (�������� *1024 ���ͻ�)
extern int_r	g_nSin[64];

// ���ұ� (�������� *1024 ���ͻ�)
extern int_r	g_nCos[64];

namespace KPathFinderAbout
{
	#define MAX_DIRECTION (4)
	#define ONE_DIRECTION (MAX_DIRECTION / 4)
	enum _BStar_Direct
	{
		eBD_Unknow = -1,
		eBD_Bottom = 0,	// ��
		//eBD_LBottom,	// ����
		eBD_Left,		// ��
		//eBD_LTop,		// ����
		eBD_Top,		// ��
		//eBD_RTop,		// ����
		eBD_Right,		// ��
		//eBD_RBottom,	// ����
	};

	int_r g_InternalDirSinCos(int_r pSinCosTable[], int_r nDir, int_r nMaxDir);

	int_r g_DirSin(int_r nDir, int_r nMaxDir);

	int_r g_DirCos(int_r nDir, int_r nMaxDir);

	float g_GetDistance(int_r nX1, int_r nY1, int_r nX2, int_r nY2);

	int_r g_GetDirIndex(int_r nX1, int_r nY1, int_r nX2, int_r nY2);

	int_r g_GetDirDiff(int_r nOriginX, int_r nOriginY, int_r nTargetX, int_r nTargetY, int_r nX, int_r nY);

	int_r g_GetDirAngle(int_r nOriginX, int_r nOriginY, int_r nTargetX, int_r nTargetY, int_r nX, int_r nY);

	int_r g_Dir64To8(int_r nDir);

	int_r g_Dir8To64(int_r nDir);

	const char * DirToString(_BStar_Direct nDir);
}

#endif // _Common_Utils_H