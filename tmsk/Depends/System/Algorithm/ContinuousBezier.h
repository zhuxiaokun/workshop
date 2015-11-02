/********************************************************************
created:	2012/4/7
manager:	Allen Wang
copyright:	Aurora Interactive, 2012
*********************************************************************/

#pragma once
#include "../Collections/KVector.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "VECTOR3.h"

namespace System
{
namespace Algorithm
{
/**
 * 拟合的连续2次贝塞尔曲线
 */
class ContinuousBezier
{
public:
	ContinuousBezier(void){};
	virtual ~ContinuousBezier(void){};

	// 曲线计算点时用的数据精度
	typedef float V_TYPE;

	/**
	 * 控制点列表
	 */
	::System::Collections::KVector<VECTOR3> m_ptList;
	/**
	 * 一条曲线
	 */
	class OneCurve
	{
	public:
		VECTOR3 pt1;
		VECTOR3 pt2;
		VECTOR3 pt3;
		OneCurve()
		{}
		OneCurve(const VECTOR3& pt1, const VECTOR3& pt2, const VECTOR3& pt3)
			: pt1(pt1), pt2(pt2), pt3(pt3)
		{}
	public:
		inline VECTOR3 GetDirection(V_TYPE t) const
		{
			return (pt1 * (-2 * (1 - t)) + pt2 * (2 * (1 - 2 * t)) + pt3 * (2 * t)).normalize();
		}
		VECTOR3 GetPos(V_TYPE t) const
		{
			if (t < 0) t = 0;
			else if (t > 1) t = 1;
			return pt1 * ((1 - t) * (1 - t)) + pt2 * (2 * t * (1 - t)) + pt3 * (t * t);
		}

		//-------------------------------------------------------------------------------------
		//长度函数
		/*
		L(t) = Integrate[s[t], t]
		L(t_) = ((2*Sqrt[A]*(2*A*t*Sqrt[C + t*(B + A*t)] + B*(-Sqrt[C] + Sqrt[C + t*(B + A*t)])) + 
		(B^2 - 4*A*C) (Log[B + 2*Sqrt[A]*Sqrt[C]] - Log[B + 2*A*t + 2 Sqrt[A]*Sqrt[C + t*(B + A*t)]]))
		/(8* A^(3/2)));
		*/
		V_TYPE GetLength(V_TYPE t) const
		{
			V_TYPE ax = pt1.x - 2 * pt2.x + pt3.x;
			V_TYPE ay = pt1.y - 2 * pt2.y + pt3.y;
			V_TYPE az = pt1.z - 2 * pt2.z + pt3.z;
			V_TYPE bx = 2 * pt2.x - 2 * pt1.x;
			V_TYPE by = 2 * pt2.y - 2 * pt1.y;
			V_TYPE bz = 2 * pt2.z - 2 * pt1.z;

			V_TYPE A = (V_TYPE)(4 * (ax * ax + ay * ay + az * az));
			V_TYPE B = (V_TYPE)(4 * (ax * bx + ay * by + az * bz));
			V_TYPE C = (V_TYPE)(bx * bx + by * by + bz * bz);

			if (A != 0.0)
			{
				V_TYPE temp1 = sqrt(C + t * (B + A * t));
				V_TYPE temp2 = (2 * A * t * temp1 + B * (temp1 - sqrt(C)));
				V_TYPE temp3 = 0.0f;
				V_TYPE tt = B + 2 * sqrt(A) * sqrt(C);
				if (tt > 0)
				{
					temp3 = log(tt);
				}
				V_TYPE temp4 = 0.0f;
				tt = B + 2 * A * t + 2 * sqrt(A) * temp1;
				if (tt > 0)
				{
					temp4 = log(tt);
				}
				V_TYPE temp5 = 2 * sqrt(A) * temp2;
				V_TYPE temp6 = (B * B - 4 * A * C) * (temp3 - temp4);

				return (temp5 + temp6) / (8 * pow(A, (V_TYPE)1.5));
			}
			else if (B != 0.0f)
			{
				return 2 * pow(t, (V_TYPE)1.5) / (3 * B);
			}
			else
			{
				return sqrt(C) * t;
			}
		}

		//-------------------------------------------------------------------------------------
		//长度函数反函数，使用牛顿切线法求解
		/*
		X(n+1) = Xn - F(Xn)/F'(Xn)
		*/
		// TODO: 无法保证收敛，有解

		V_TYPE GetTByLengthNewton(V_TYPE initT, V_TYPE len) const
		{
			if (initT == 0.0f) return 0.0f;

			V_TYPE ax = pt1.x - 2 * pt2.x + pt3.x;
			V_TYPE ay = pt1.y - 2 * pt2.y + pt3.y;
			V_TYPE az = pt1.z - 2 * pt2.z + pt3.z;
			V_TYPE bx = 2 * pt2.x - 2 * pt1.x;
			V_TYPE by = 2 * pt2.y - 2 * pt1.y;
			V_TYPE bz = 2 * pt2.z - 2 * pt1.z;

			V_TYPE A = (V_TYPE)(4 * (ax * ax + ay * ay + az * az));
			V_TYPE B = (V_TYPE)(4 * (ax * bx + ay * by + az * bz));
			V_TYPE C = (V_TYPE)(bx * bx + by * by + bz * bz);

			V_TYPE st1 = sqrt(A * initT * initT + B * initT + C);

			// 	do
			// 	{
			// 		t2 = t1 - (CurveLength(pt1, pt2, pt3, t1)-len)/st1;
			// 		if(fabs(t1-t2)<0.000001f) break;
			// 		t1=t2;
			// 	}while(true);
			V_TYPE t1=initT, t2;
			// 保护一下
			for (int i = 0; i < 20; i++)
			{
				t2 = t1 - (GetLength(t1) - len) / st1;
				if (fabs(t1 - t2) < 0.0001) return t2;
				t1=t2;
			}
			return t2;
		}

		//-------------------------------------------------------------------------------------
		//长度函数反函数，使用二分法求解
		V_TYPE GetTByLengthBinary(V_TYPE initT, V_TYPE l) const
		{
			if (initT <= 0) return 0;
			if (initT >= 1) return l;
			V_TYPE t1 = 0;
			V_TYPE t2 = 1;

			// 细分2的10次方次
			// TODO: 考虑根据长度确定细分次数
			for (int i = 0; i < 10; i++)
			{
				V_TYPE len = GetLength(initT);
				if (fabs(len - l) < 0.0001) return initT;
				if (len < l)	//往大方向二分
				{
					t1 = initT;
					initT = (t2 + t1) / 2;
				}
				else	// 往小方向二分
				{
					t2 = initT;
					initT = (t2 + t1) / 2;
				}
			}
			return initT;
		}

		//-------------------------------------------------------------------------------------
		//长度函数反函数，根据flag决定使用哪种算法，0 二分，1 迭代
		V_TYPE GetTByLength(V_TYPE initT, V_TYPE len, int flag = 0) const
		{
			switch(flag)
			{
			case 1:
				return GetTByLengthNewton(initT, len);
			default:
				return GetTByLengthBinary(initT, len);
			}
		}

		//-------------------------------------------------------------------------------------
		//速度函数
		/*
		s(t_) = Sqrt[A*t*t+B*t+C]
		*/

		// float s(float t)
		// {
		// 	return sqrt(A*t*t+B*t+C);
		// }
	};

	/**
	 * 根据长度定位坐标
	 */
	bool GetPosByLength(V_TYPE len, VECTOR3& pt, int flag = 0)
	{
		V_TYPE t = 0;
		OneCurve oc;
		if (!GetCurveParamByLength(len, oc, t, flag)) return false;
		pt = oc.GetPos(t);
		return true;
	}

	bool GetCurveParamByLength(V_TYPE len, OneCurve& _oc, V_TYPE& _t, int flag = 0)
	{
		if (m_ptList.size() < 3) return false;
		if (len < 0.0f)
		{
			return false;
		}
		V_TYPE lenToSub = 0;

		// 第一组
		{
			OneCurve oc(m_ptList[0], m_ptList[1], MiddlePoint(m_ptList[1], m_ptList[2]));
			lenToSub += oc.GetLength(1);
			if (len <= lenToSub)
			{
				V_TYPE t = len / lenToSub;
				V_TYPE pos = oc.GetTByLength(t, len, flag);
				_oc = oc;
				_t = pos;
				return true;
			}
		}
		// 中间组
		{
			size_t totalTestStart = 2;	//开始两个点是开始组的，跳过
			size_t totalTestEnd = m_ptList.size() - 2;	//最后两个点是最后组的，跳过
			for (size_t i = totalTestStart; i < totalTestEnd; i++)
			{
				OneCurve oc(MiddlePoint(m_ptList[i - 1], m_ptList[i]), m_ptList[i], MiddlePoint(m_ptList[i], m_ptList[i + 1]));
				V_TYPE totalLen = oc.GetLength(1);
				if (len <= totalLen + lenToSub)
				{
					V_TYPE currLen = len - lenToSub;

					// 命中
					V_TYPE pos = oc.GetTByLength(currLen / totalLen, currLen, flag);
					_oc = oc;
					_t = pos;
					return true;
				}
				lenToSub += totalLen;
			}
		}
		// 结尾组
		{
			OneCurve oc(MiddlePoint(m_ptList[m_ptList.size() - 3], m_ptList[m_ptList.size() - 2]), m_ptList[m_ptList.size() - 2], m_ptList[m_ptList.size() - 1]);
			V_TYPE totalLen = oc.GetLength(1);
			if (len <= totalLen + lenToSub)
			{
				V_TYPE currLen = len - lenToSub;

				// 命中
				V_TYPE pos = oc.GetTByLength(currLen / totalLen, currLen, flag);
				_oc = oc;
				_t = pos;
				return true;
			}
		}

		// 超出范围了
		return false;
	}

	/**
	 * 得到中点
	 */
	static inline VECTOR3 MiddlePoint(const VECTOR3& pt1, const VECTOR3& pt2)
	{
		return (pt1 + pt2) / 2;
	}

	/**
	 * 得到两向量的夹角
	 */
	static inline V_TYPE GetAngle(const VECTOR3& vec1, const VECTOR3& vec2)
	{
		V_TYPE d1 = sqrt(vec1.x * vec1.x + vec1.y * vec1.y + vec1.z * vec1.z);
		V_TYPE d2 = sqrt(vec2.x * vec2.x + vec2.y * vec2.y + vec2.z * vec2.z);
		return acos((vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z) / (d1 * d2));
	}

	/**
	 * 在整条曲线上采样
	 * @param	listToSave:			要存储结果的数据结构，需要支持有clear()方法和push_back(VECTOR3)方法
	 * @param	samplingInterval:	采样间距，使用曲线距离
	 * @param	distanceToleration:	最远采样点间隔
	 * @param	slopeToleration:	方向变化容忍限度
	 */
	template <typename SAVE_LIST_TYPE>
	void GetSamplingList(SAVE_LIST_TYPE& listToSave, V_TYPE samplingInterval, V_TYPE distanceToleration = 200, V_TYPE directionToleration = M_PI / 32, int flag = 0)
	{
		// TODO: 以后增加考虑2个点的情况
		if (m_ptList.size() < 3) return;
		listToSave.clear();
		ContinuousBezier::OneCurve oc;
		// 根据曲线曲率密度绘制点
		float pos = 0.0f;
		float t;
		if (!GetCurveParamByLength(pos, oc, t, flag)) return;
		VECTOR3 k = oc.GetDirection(t);
		VECTOR3 lastPt = m_ptList[0];
		listToSave.push_back(m_ptList[0]);
		while(GetCurveParamByLength(pos, oc, t, flag))
		{
			bool saveFlag = false;
			ContinuousBezier::VECTOR3 pt = oc.GetPos(t);
			if (fabs(pt.x - lastPt.x) + fabs(pt.y - lastPt.y) + fabs(pt.z - lastPt.z) > distanceToleration)
			{
				saveFlag = true;
			}
			else
			{
				VECTOR3 newk = oc.GetDirection(t);
				if (GetAngle(newk, k) > directionToleration)
				{
					saveFlag = true;
					k = newk;
				}
			}
			if (saveFlag)
			{
				lastPt = pt;
				listToSave.push_back(pt);
			}
			pos += samplingInterval;
		}
		// TODO: 需要判定最后一个点是否已经压入了吗？
		listToSave.push_back(m_ptList[m_ptList.size() - 1]);
	}
	void UpdateByPosList(const ::System::Collections::KVector<VECTOR3>& posList)
	{
		m_ptList.clear();
		if (posList.size() >= 2)	// 至少需要2个点
		{
			m_ptList.push_back(posList[0]);
			VECTOR3 lastPos = posList[0];
			for (int i = 1; i < posList.size() - 1; ++i)
			{
				lastPos = (posList[i] - lastPos) * 2.0f + lastPos;
				m_ptList.push_back(lastPos);
			}
			m_ptList.push_back(posList[posList.size() - 1]);
		}
	}
};

}
}