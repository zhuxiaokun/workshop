/********************************************************************
created:	2011/11/14
manager:	Allen Wang
copyright:	Aurora Interactive, 2011
*********************************************************************/

#pragma once

namespace System
{

namespace Algorithm
{
	/**
	 * 距离计算相关
	 */
	class DistanceComputer
	{
	public:
		static float PointToLine2D(float x0, float y0, float x1, float y1, float toTestX, float toTestY);
	};
}
}