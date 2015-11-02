/********************************************************************
created:	2012/5/3
manager:	Allen Wang
copyright:	Aurora Interactive, 2012
*********************************************************************/

#pragma once
#include <math.h>
#include "AngleComputer.h"


namespace System
{

namespace Algorithm
{

/**
 * 检查一条圆弧经过的所有格子
 */
class ArcChecker
{
private:

#define ___TO_INT(x) ((x) < 0 ? (int)(x) - 1 : (int)(x))

// DIRECTION:			AC逆时针，C顺时针
// AREA:				象限号
// FIRST_Y:				向上填-0，向下填-1
// UPDOWN_SIGN_1:		向上填<，向下填>
// UPDOWN_SIGN_2:		向上填+，向下填-
// SOLUTION_SIGN:		与平行线相交，取大的x解用+，取小的用-
// LEFTRIGHT_SIGN_1:	从左向右遍历填<=，从右向左填>=
// LEFTRIGHT_SIGN_2:	从左向右遍历填+，从右向左填-
#define ___DECLARE_FAST_CHECK_ARC_BODY(DIRECTION, AREA, FIRST_Y, UPDOWN_SIGN_1, UPDOWN_SIGN_2, SOLUTION_SIGN, LEFTRIGHT_SIGN_1, LEFTRIGHT_SIGN_2)	\
	template <typename T>																		\
	inline static bool FastCheckArc##DIRECTION##AREA(float fromx, float fromy					\
													, float tox, float toy						\
													, float ox, float oy, float r, T& cb)		\
	{																							\
		float x = fromx;																		\
		float y = fromy;																		\
		int ifx = ___TO_INT(fromx);																\
		int ify = ___TO_INT(fromy) + 1;															\
		int itx = ___TO_INT(tox);																\
		int ity = ___TO_INT(toy) + 1;															\
		y = (float)ify FIRST_Y;																	\
		float targety = (float)ity;																\
		if (targety UPDOWN_SIGN_1 y)															\
		{																						\
			for (int i = ifx; i LEFTRIGHT_SIGN_1 itx; i = i LEFTRIGHT_SIGN_2 1)					\
			{																					\
				if (!cb(i, ify - 1)) return false;												\
			}																					\
		}																						\
		else																					\
		{																						\
			int blockx = ifx;																	\
			int blocky = ify;																	\
			while (y UPDOWN_SIGN_1 toy)															\
			{																					\
				float toSqrt = r * r - (y - oy) * (y - oy);										\
				if (toSqrt < 0) toSqrt = 0;														\
				else toSqrt = sqrt(toSqrt);														\
				x = ox SOLUTION_SIGN toSqrt;													\
				int nextblockx = ___TO_INT(x);													\
				for (int i = blockx; i LEFTRIGHT_SIGN_1 nextblockx; i = i LEFTRIGHT_SIGN_2 1)	\
				{																				\
					if (!cb(i, blocky - 1)) return false;										\
				}																				\
				blockx = nextblockx;															\
				blocky = blocky UPDOWN_SIGN_2 1;												\
				y = y UPDOWN_SIGN_2 1.0f;														\
			}																					\
			for (int i = blockx; i LEFTRIGHT_SIGN_1 itx; i = i LEFTRIGHT_SIGN_2 1)				\
			{																					\
				if (!cb(i, blocky - 1)) return false;											\
			}																					\
		}																						\
		return true;																			\
	}

	___DECLARE_FAST_CHECK_ARC_BODY(AC, 1, -0, <, +, +, >=, -);
	___DECLARE_FAST_CHECK_ARC_BODY(AC, 2, -1, >, -, -, >=, -);
	___DECLARE_FAST_CHECK_ARC_BODY(AC, 3, -1, >, -, -, <=, +);
	___DECLARE_FAST_CHECK_ARC_BODY(AC, 4, -0, <, +, +, <=, +);

	___DECLARE_FAST_CHECK_ARC_BODY(C, 1, -1, >, -, +, <=, +);
	___DECLARE_FAST_CHECK_ARC_BODY(C, 2, -0, <, +, -, <=, +);
	___DECLARE_FAST_CHECK_ARC_BODY(C, 3, -0, <, +, -, >=, -);
	___DECLARE_FAST_CHECK_ARC_BODY(C, 4, -1, >, -, +, >=, -);

#undef ___DECLARE_FAST_CHECK_ARC_BODY
#undef ___TO_INT

public:
	/**
	 * 使用cb检查一条圆弧经过的所有格子是否能通过检查
	 * param:
	 *		fromx, fromy:	出发点位置
	 *		tox, toy:		目标点位置
	 *		ox, oy:			圆弧所在圆的圆心坐标
	 *		r:				圆弧所在圆的半径
	 *		clockwise:		行走方向标记，true为顺时针，false为逆时针
	 *		cb:				检查方法，要求可以按"if(cb((int)x, (int)y)){}"调用，即可以用()送入两个int型参数，返回值可用作if判定
	 * 函数会按(fromx, fromy)->(tox, toy), clockwise指定的方向对圆弧覆盖到的单位格子(1x1)使用cb遍历，当cb返回false的时候函数将返回false，否则在遍历完成后返回true
	 */
	template <typename T>
	static bool CheckArc(float fromx, float fromy, float tox, float toy, float ox, float oy, float r, bool clockwise, T& cb)
	{
		if (clockwise)
		{
			float angleFrom = AngleComputer::GetFaceAngle(ox, oy, tox, toy);
			float angleTo = AngleComputer::GetFaceAngle(ox, oy, fromx, fromy);
			if (angleTo < angleFrom)
			{
				angleTo += (float)M_PI * 2;
			}
			if (angleFrom < 0.0f)
			{
				angleFrom += (float)M_PI * 2;
				angleTo += (float)M_PI * 2;
			}

			// 划分区域
			float edge = angleFrom / (float)M_PI_2;
			int nEdge = (int)edge;
			float edgeEnd = angleTo / (float)M_PI_2;
			int nEdgeEnd = (int)edgeEnd;
			if (edge > (int)nEdge) nEdge += 1;

			float fx = fromx;
			float fy = fromy;
			float tx = 0.0f;
			float ty = 0.0f;
			for (int i = nEdgeEnd; i >= nEdge; i--)
			{
				edge = i * (float)M_PI_2;

				tx = ox + r * cos(edge);
				ty = oy + r * sin(edge);

				int edgeRange = i;
				if (edgeRange < 0) edgeRange += 4;
				else if (edgeRange >= 4) edgeRange -= 4;
				// TODO: 算位置，根据这个决定用哪组变换。
				switch(edgeRange)
				{
				case 0:	// 第1象限
					if (!FastCheckArcC1(fx, fy, tx, ty, ox, oy, r, cb)) return false;
					break;
				case 1:	// 第2象限
					if (!FastCheckArcC2(fx, fy, tx, ty, ox, oy, r, cb)) return false;
					break;
				case 2:	// 第3象限
					if (!FastCheckArcC3(fx, fy, tx, ty, ox, oy, r, cb)) return false;
					break;
				case 3:	// 第4象限
					if (!FastCheckArcC4(fx, fy, tx, ty, ox, oy, r, cb)) return false;
					break;
				}
				fx = tx;
				fy = ty;
			}
			// 补齐尾巴
			int edgeRange = nEdge - 1;
			if (edgeRange < 0) edgeRange += 4;
			else if (edgeRange >= 4) edgeRange -= 4;
			switch(edgeRange)
			{
			case 0:	// 第1象限
				if (!FastCheckArcC1(fx, fy, tox, toy, ox, oy, r, cb)) return false;
				break;
			case 1:	// 第2象限
				if (!FastCheckArcC2(fx, fy, tox, toy, ox, oy, r, cb)) return false;
				break;
			case 2:	// 第3象限
				if (!FastCheckArcC3(fx, fy, tox, toy, ox, oy, r, cb)) return false;
				break;
			case 3:	// 第4象限
				if (!FastCheckArcC4(fx, fy, tox, toy, ox, oy, r, cb)) return false;
				break;
			}
		}
		else
		{
			float angleFrom = AngleComputer::GetFaceAngle(ox, oy, fromx, fromy);
			float angleTo = AngleComputer::GetFaceAngle(ox, oy, tox, toy);
			if (angleTo < angleFrom)
			{
				angleTo += (float)M_PI * 2;
			}
			if (angleFrom < 0.0f)
			{
				angleFrom += (float)M_PI * 2;
				angleTo += (float)M_PI * 2;
			}
			// 划分区域
			float edge = angleFrom / (float)M_PI_2;
			int nEdge = (int)edge;
			float edgeEnd = angleTo / (float)M_PI_2;
			int nEdgeEnd = (int)edgeEnd;
			if (edge > (int)nEdge) nEdge += 1;

			float fx = fromx;
			float fy = fromy;
			float tx = 0.0f;
			float ty = 0.0f;
			for (int i = nEdge; i <= nEdgeEnd; i++)
			{
				edge = i * (float)M_PI_2;

				tx = ox + r * cos(edge);
				ty = oy + r * sin(edge);

				int edgeRange = i - 1;
				if (edgeRange < 0) edgeRange += 4;
				else if (edgeRange >= 4) edgeRange -= 4;

				// TODO: 算位置，根据这个决定用哪组变换。
				switch(edgeRange)
				{
				case 0:	// 第1象限
					if (!FastCheckArcAC1(fx, fy, tx, ty, ox, oy, r, cb)) return false;
					break;
				case 1:	// 第2象限
					if (!FastCheckArcAC2(fx, fy, tx, ty, ox, oy, r, cb)) return false;
					break;
				case 2:	// 第3象限
					if (!FastCheckArcAC3(fx, fy, tx, ty, ox, oy, r, cb)) return false;
					break;
				case 3:	// 第4象限
					if (!FastCheckArcAC4(fx, fy, tx, ty, ox, oy, r, cb)) return false;
					break;
				}
				fx = tx;
				fy = ty;
			}
			// 补齐尾巴
			int edgeRange = nEdgeEnd;
			if (edgeRange < 0) edgeRange += 4;
			else if (edgeRange >= 4) edgeRange -= 4;
			switch(edgeRange)
			{
			case 0:	// 第1象限
				if (!FastCheckArcAC1(fx, fy, tox, toy, ox, oy, r, cb)) return false;
				break;
			case 1:	// 第2象限
				if (!FastCheckArcAC2(fx, fy, tox, toy, ox, oy, r, cb)) return false;
				break;
			case 2:	// 第3象限
				if (!FastCheckArcAC3(fx, fy, tox, toy, ox, oy, r, cb)) return false;
				break;
			case 3:	// 第4象限
				if (!FastCheckArcAC4(fx, fy, tox, toy, ox, oy, r, cb)) return false;
				break;
			}
		}
		return true;
	}
};

}

}
// 原算法，计算走过的块
/*
	// 第1象限逆时针方向，假定一定在 0 到 pi / 2
	template <typename T>
	void FastCheckArcAC1(float fromx, float fromy, float tox, float toy, float ox, float oy, float r, T& cb)
	{
		float x = fromx;
		float y = fromy;
		int ifx = TO_INT(fromx);
		int ify = TO_INT(fromy) + 1;
		int itx = TO_INT(tox);
		int ity = TO_INT(toy) + 1;

		// 推进到第一个边界
		y = (float)ify;
		// 发现最后的边界
		float targety = (float)ity;
		if (targety < y)	// 纵向只有一个格子
		{
			for (int i = ifx; i >= itx; i--)
			{
				cb(i, ify);
			}
		}
		else	// 纵向超过一个格子
		{
			int blockx = ifx;
			int blocky = ify;
			while (y < toy)
			{
				x = ox + sqrt(r * r - (y - oy) * (y - oy));
				int nextblockx = TO_INT(x);
				for (int i = blockx; i >= nextblockx; i--)
				{
					cb(i, blocky);
				}
				blockx = nextblockx;
				blocky += 1;
				y += 1.0f;
			}
			// 处理结尾
			for (int i = blockx; i >= itx; i--)
			{
				cb(i, blocky);
			}
		}
	}

	// 第2象限逆时针方向，假定一定在 pi / 2 到 pi
	template <typename T>
	void FastCheckArcAC2(float fromx, float fromy, float tox, float toy, float ox, float oy, float r, T& cb)
	{
		float x = fromx;
		float y = fromy;
		int ifx = TO_INT(fromx);
		int ify = TO_INT(fromy) + 1;
		int itx = TO_INT(tox);
		int ity = TO_INT(toy) + 1;

		// 推进到第一个边界
		y = (float)ify - 1;
		// 发现最后的边界
		float targety = (float)ity;
		if (targety > y)	// 纵向只有一个格子
		{
			for (int i = ifx; i >= itx; i--)
			{
				cb(i, ify);
			}
		}
		else	// 纵向超过一个格子
		{
			int blockx = ifx;
			int blocky = ify;
			while (y > toy)
			{
				x = ox - sqrt(r * r - (y - oy) * (y - oy));
				int nextblockx = TO_INT(x);
				for (int i = blockx; i >= nextblockx; i--)
				{
					cb(i, blocky);
				}
				blockx = nextblockx;
				blocky -= 1;
				y -= 1.0f;
			}
			// 处理结尾
			for (int i = blockx; i >= itx; i--)
			{
				cb(i, blocky);
			}
		}
	}

	// 第3象限逆时针方向，假定一定在 pi 到 pi * 3 / 2
	template <typename T>
	void FastCheckArcAC3(float fromx, float fromy, float tox, float toy, float ox, float oy, float r, T& cb)
	{
		float x = fromx;
		float y = fromy;
		int ifx = TO_INT(fromx);
		int ify = TO_INT(fromy) + 1;
		int itx = TO_INT(tox);
		int ity = TO_INT(toy) + 1;

		// 推进到第一个边界
		y = (float)ify - 1;
		// 发现最后的边界
		float targety = (float)ity;
		if (targety > y)	// 纵向只有一个格子
		{
			for (int i = ifx; i <= itx; i++)
			{
				cb(i, ify);
			}
		}
		else	// 纵向超过一个格子
		{
			int blockx = ifx;
			int blocky = ify;
			while (y > toy)
			{
				x = ox - sqrt(r * r - (y - oy) * (y - oy));
				int nextblockx = TO_INT(x);
				for (int i = blockx; i <= nextblockx; i++)
				{
					cb(i, blocky);
				}
				blockx = nextblockx;
				blocky -= 1;
				y -= 1.0f;
			}
			// 处理结尾
			for (int i = blockx; i <= itx; i++)
			{
				cb(i, blocky);
			}
		}
	}

	// 第4象限逆时针方向，假定一定在pi * 3 / 2到pi * 2
	template <typename T>
	void FastCheckArcAC4(float fromx, float fromy, float tox, float toy, float ox, float oy, float r, T& cb)
	{
		float x = fromx;
		float y = fromy;
		int ifx = TO_INT(fromx);
		int ify = TO_INT(fromy) + 1;
		int itx = TO_INT(tox);
		int ity = TO_INT(toy) + 1;

		// 推进到第一个边界
		y = (float)ify;
		// 发现最后的边界
		float targety = (float)ity;
		if (targety < y)	// 纵向只有一个格子
		{
			for (int i = ifx; i <= itx; i++)
			{
				cb(i, ify);
			}
		}
		else	// 纵向超过一个格子
		{
			int blockx = ifx;
			int blocky = ify;
			while (y < toy)
			{
				x = ox + sqrt(r * r - (y - oy) * (y - oy));
				int nextblockx = TO_INT(x);
				for (int i = blockx; i <= nextblockx; i++)
				{
					cb(i, blocky);
				}
				blockx = nextblockx;
				blocky += 1;
				y += 1.0f;
			}
			// 处理结尾
			for (int i = blockx; i <= itx; i++)
			{
				cb(i, blocky);
			}
		}
	}
*/