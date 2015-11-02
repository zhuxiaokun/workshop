/********************************************************************
created:	2011/3/21
manager:	Allen Wang
copyright:	Aurora Interactive, 2011
*********************************************************************/

#pragma once

namespace System
{

namespace Algorithm
{
	/**
	 * 检查一条线段经过的所有格子
	 */

	class LineChecker
	{

	private:
		#define ___TO_INT(x) ((x) < 0 ? (int)(x) - 1 : (int)(x))
		/**
		 * 为了效率。原算法在文件的下部
		 */
		#define ___CHECK_LINE_BODY(NAME, CBINFO)							\
		template <typename T>												\
		inline static bool CheckLine##NAME(float fromx, float fromy			\
						, float tox, float toy								\
						, T& _Callback)										\
		{																	\
			float x = fromx;												\
			float y = fromy;												\
			int ifx = ___TO_INT(fromx);										\
			int ify = ___TO_INT(fromy);										\
			int itx = ___TO_INT(tox);										\
			int ity = ___TO_INT(toy);										\
			if (!CBINFO(ifx, ify)) return false;							\
			x = (float)(ifx + 1);											\
			float targetx = (float)itx;										\
			if (x > targetx)												\
			{																\
				if (ify != ity)												\
				{															\
					if (!CBINFO(itx, ity)) return false;					\
				}															\
			}																\
			else															\
			{																\
				float k = (toy - fromy) / (tox - fromx);					\
				y = (x - fromx) * k + fromy;								\
				float dy = k;												\
				int ty = ___TO_INT(y);										\
				if (ty != ify)												\
				{															\
					if (!CBINFO(ifx, ty)) return false;						\
				}															\
				int nextx = (int)(x);										\
				float nexty = y;											\
				int blockx = nextx;											\
				int blocky = ty;											\
				while (blockx < itx)										\
				{															\
					++nextx;												\
					nexty += k;												\
					int nextblocky = ___TO_INT(nexty);						\
					if (blocky == nextblocky)								\
					{														\
						if (!CBINFO(blockx, blocky)) return false;			\
					}														\
					else													\
					{														\
						if (!CBINFO(blockx, blocky)) return false;			\
						if (!CBINFO(blockx, nextblocky)) return false;		\
					}														\
					blockx = nextx;											\
					blocky = nextblocky;									\
				}															\
				if (targetx <= tox)											\
				{															\
					int targety = ___TO_INT((targetx - fromx) * k + fromy);	\
					if (ity == targety)										\
					{														\
						if (!CBINFO(itx, ity)) return false;				\
					}														\
					else													\
					{														\
						if (!CBINFO(itx, targety)) return false;			\
						if (!CBINFO(itx, ity)) return false;				\
					}														\
				}															\
			}																\
			return true;													\
		}

		#define ___AREA1(x, y) _Callback((x), (y))
		#define ___AREA2(x, y) _Callback((y), (x))
		#define ___AREA3(x, y) _Callback(-(y) - 1, (x))
		#define ___AREA4(x, y) _Callback(-(x) - 1, (y))
		#define ___AREA5(x, y) _Callback(-(x) - 1, -(y) - 1)
		#define ___AREA6(x, y) _Callback(-(y) - 1, -(x) - 1)
		#define ___AREA7(x, y) _Callback((y), -(x) - 1)
		#define ___AREA8(x, y) _Callback((x), -(y) - 1)

		___CHECK_LINE_BODY(1, ___AREA1);
		___CHECK_LINE_BODY(2, ___AREA2);
		___CHECK_LINE_BODY(3, ___AREA3);
		___CHECK_LINE_BODY(4, ___AREA4);
		___CHECK_LINE_BODY(5, ___AREA5);
		___CHECK_LINE_BODY(6, ___AREA6);
		___CHECK_LINE_BODY(7, ___AREA7);
		___CHECK_LINE_BODY(8, ___AREA8);
	
		#undef ___CHECK_LINE_BODY
		#undef ___AREA1
		#undef ___AREA2
		#undef ___AREA3
		#undef ___AREA4
		#undef ___AREA5
		#undef ___AREA6
		#undef ___AREA7
		#undef ___AREA8
		#undef ___TO_INT


	public:
		/**
		 * 使用cb检查一条线段经过的所有格子是否能通过检查
		 * param:
		 *		fromx, fromy:		出发点位置
		 *		tox, toy:			目标点位置
		 *		cb:					检查方法，要求可以按"if(cb((int)x, (int)y)){}"调用，即可以用()送入两个int型参数，返回值可用作if判定
		 * 函数会按(fromx, fromy)->(tox, toy)方向对线段覆盖到的单位格子(1x1)使用cb遍历，当cb返回false的时候函数将返回false，否则在遍历完成后返回true
		 */
		template <typename T>
		static bool CheckLine(float fromx, float fromy, float tox, float toy, T& cb)
		{
			if (tox < fromx)
			{
				if (toy < fromy)
				{
					if (fromy - toy < fromx - tox)
					{
						if (!CheckLine5(-fromx, -fromy, -tox, -toy, cb)) return false;
					}
					else
					{
						if (!CheckLine6(-fromy, -fromx, -toy, -tox, cb)) return false;
					}
				}
				else
				{
					if (toy - fromy < fromx - tox)
					{
						if (!CheckLine4(-fromx, fromy, -tox, toy, cb)) return false;
					}
					else
					{
						if (!CheckLine3(fromy, -fromx, toy, -tox, cb)) return false;
					}
				}
			}
			else
			{
				if (toy < fromy)
				{
					if (fromy - toy < tox - fromx)
					{
						if (!CheckLine8(fromx, -fromy, tox, -toy, cb)) return false;
					}
					else
					{
						if (!CheckLine7(-fromy, fromx, -toy, tox, cb)) return false;
					}
				}
				else
				{
					if (toy - fromy < tox - fromx)
					{
						if (!CheckLine1(fromx, fromy, tox, toy, cb)) return false;
					}
					else
					{
						if (!CheckLine2(fromy, fromx, toy, tox, cb)) return false;
					}
				}
			}
			return true;
		}
	};
}

}

/**
// 附：原始算法（经过一次速度优化）FullCheckLine是入口函数
#define TO_INT(x) ((x) < 0 ? (int)(x) - 1 : (int)(x))

template <typename T>
void FastCheckLine(float fromx, float fromy, float tox, float toy, T& cb)
{
	float x = fromx;
	float y = fromy;
	int ifx = TO_INT(fromx);
	int ify = TO_INT(fromy);
	int itx = TO_INT(tox);
	int ity = TO_INT(toy);

	// 0 到 pi / 4
	if (tox > fromx && toy >= fromy && toy - fromy <= tox - fromx)
	{
		cb(ifx, ify, 1);
		// 推进到第一个边界
		x = (float)(ifx + 1);
		// 发现最后的边界
		float targetx = (float)itx;
		if (x > targetx)	// 横向只有一个格子
		{
			if (ify != ity)	// 不在一个格子中
			{
				cb(itx, ity, 2);
			}
		}
		else	// 横向超过一个格子
		{
			float k = (toy - fromy) / (tox - fromx);
			y = (x - fromx) * k + fromy;
			float dy = k;
			// 处理开头
			int ty = TO_INT(y);
			if (ty != ify)	//两个格子，再补一个
			{
				cb(ifx, ty, 10);
			}
			int nextx = (int)(x);
			float nexty = y;
			int blockx = nextx;
			int blocky = ty;
			// 把能循环处理的都处理了
			while (blockx < itx)
			{
				++nextx;

				nexty += k;
				int nextblocky = TO_INT(nexty);
				if (blocky == nextblocky)	//是一个点
				{
					cb(blockx, blocky, 20);
				}
				else	//上下两个点
				{
					cb(blockx, blocky, 21);
					cb(blockx, nextblocky, 22);
				}
				blockx = nextx;
				blocky = nextblocky;
			}
			// 处理尾巴，认为在边界上的时候也占到了下一块
			if (targetx <= tox)
			{
				int targety = TO_INT((targetx - fromx) * k + fromy);
				if (ity == targety)
				{
					// 同一格子
					cb(itx, ity, 30);
				}
				else
				{
					// 不同一格子
					cb(itx, targety, 31);
					cb(itx, ity, 32);
				}
			}
		}
	}
}

template<typename T>
class AreaTransfer
{
public:
	AreaTransfer(T* pT)
		: m_area1(pT)
		, m_area2(pT)
		, m_area3(pT)
		, m_area4(pT)
		, m_area5(pT)
		, m_area6(pT)
		, m_area7(pT)
		, m_area8(pT)
	{};
public:
	class Area1
	{
	public:
		Area1(T* pT)
			: pT(pT)
		{}
		void operator()(int x, int y, int n)
		{
			(*pT)(x, y, n);
		}
	private:
		T* pT;
	} m_area1;
	class Area2
	{
	public:
		Area2(T* pT)
			: pT(pT)
		{}
		void operator()(int x, int y, int n)
		{
			(*pT)(y, x, n);
		}
	private:
		T* pT;
	} m_area2;
	class Area3
	{
	public:
		Area3(T* pT)
			: pT(pT)
		{}
		void operator()(int x, int y, int n)
		{
			(*pT)(-y - 1, x, n);
		}
	private:
		T* pT;
	} m_area3;
	class Area4
	{
	public:
		Area4(T* pT)
			: pT(pT)
		{}
		void operator()(int x, int y, int n)
		{
			(*pT)(-x - 1, y, n);
		}
	private:
		T* pT;
	} m_area4;
	class Area5
	{
	public:
		Area5(T* pT)
			: pT(pT)
		{}
		void operator()(int x, int y, int n)
		{
			(*pT)(-x - 1, -y - 1, n);
		}
	private:
		T* pT;
	} m_area5;
	class Area6
	{
	public:
		Area6(T* pT)
			: pT(pT)
		{}
		void operator()(int x, int y, int n)
		{
			(*pT)(-y - 1, -x - 1, n);
		}
	private:
		T* pT;
	} m_area6;
	class Area7
	{
	public:
		Area7(T* pT)
			: pT(pT)
		{}
		void operator()(int x, int y, int n)
		{
			(*pT)(y, -x - 1, n);
		}
	private:
		T* pT;
	} m_area7;
	class Area8
	{
	public:
		Area8(T* pT)
			: pT(pT)
		{}
		void operator()(int x, int y, int n)
		{
			(*pT)(x, -y - 1, n);
		}
	private:
		T* pT;
	} m_area8;
};

template <typename T>
void FullCheckLine(float fromx, float fromy, float tox, float toy, T& cb)
{
	AreaTransfer<T> at(&cb);
	if (tox < fromx)
	{
		if (toy < fromy)
		{
			if (fromy - toy < fromx - tox)
			{
				FastCheckLine(-fromx, -fromy, -tox, -toy, at.m_area5);
			}
			else
			{
				FastCheckLine(-fromy, -fromx, -toy, -tox, at.m_area6);
			}
		}
		else
		{
			if (toy - fromy < fromx - tox)
			{
				FastCheckLine(-fromx, fromy, -tox, toy, at.m_area4);
			}
			else
			{
				FastCheckLine(fromy, -fromx, toy, -tox, at.m_area3);
			}
		}
	}
	else
	{
		if (toy < fromy)
		{
			if (fromy - toy < tox - fromx)
			{
				FastCheckLine(fromx, -fromy, tox, -toy, at.m_area8);
			}
			else
			{
				FastCheckLine(-fromy, fromx, -toy, tox, at.m_area7);
			}
		}
		else
		{
			if (toy - fromy < tox - fromx)
			{
				FastCheckLine(fromx, fromy, tox, toy, at.m_area1);
			}
			else
			{
				FastCheckLine(fromy, fromx, toy, tox, at.m_area2);
			}
		}
	}
}
*/