/********************************************************************
created:	2011/5/18
manager:	Allen Wang
copyright:	Aurora Interactive, 2011
*********************************************************************/

#pragma once

namespace System
{

namespace Algorithm
{
	/**
	 * 角度计算相关
	 */

	class AngleComputer
	{
	public:
		/**
		 * 根据原点和要看到的点计算角度
		 */
		static float GetFaceAngle(float fromx, float fromy, float tox, float toy);
		/**
		 * 得到两个角的差，范围在0-pi之间
		 */
		static float AngleDiff(float a, float b);
		/**
		 * 把角度变换到0-2pi范围
		 */
		static float NormalizeAngle(float a);
	};
}

}