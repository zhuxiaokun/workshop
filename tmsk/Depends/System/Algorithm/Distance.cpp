/********************************************************************
created:	2011/3/21
manager:	Allen Wang
copyright:	Aurora Interactive, 2011
*********************************************************************/

#define _USE_MATH_DEFINES
#include "math.h"
#include "Distance.h"

namespace System
{
namespace Algorithm
{
	float DistanceComputer::PointToLine2D( float x0, float y0, float x1, float y1, float toTestX, float toTestY )
	{
		float dx = x1 - x0;
		float dy = y1 - y0;
		if (fabs(dx) < 0.1f)
		{
			if (fabs(dy) < 0.1f)
			{
				// 输入的是个点
				return sqrt(pow(toTestX - x0, 2) + pow(toTestY - y0, 2));
			}
			// 输入的是与y轴平行的线，返回x的绝对值
			return fabs(toTestX - x0);
		}
		else if (fabs(dy) < 0.1f)
		{
			// 输入的是与x轴平行的线，返回y的绝对值
			return fabs(toTestY - y0);
		}
		// 不与坐标轴平行，按两点式计算直线方程，得到直线的一般式方程，再计算点到直线的距离
		return (dy * (toTestX - x0) + dx * (y0 - toTestY)) / sqrt(dx * dx + dy * dy);
	}
}
}