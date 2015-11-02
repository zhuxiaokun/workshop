/********************************************************************
created:	2014/2/27
manager:	Allen Wang
copyright:	Aurora Interactive, 2014
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
 * ��ϵ�����3�α��������ߣ����Ƶ��Զ�����
 */
class ContinuousCubicBezier
{
public:
	ContinuousCubicBezier(void){};
	virtual ~ContinuousCubicBezier(void){};

private:
	// ���߼����ʱ�õ����ݾ���
	typedef float V_TYPE;
	// �Σ�������¼һ��3�����ߵ�һ������
	struct Segment
	{
		union
		{
			struct
			{
				V_TYPE a;
				V_TYPE b;
				V_TYPE c;
				V_TYPE d;
			};
			V_TYPE val[4];
		};
	};

	// �������ʱ�����߲𿪵ľ��ȣ���ɶ��ٶΣ�
	const static int PRECISION = 6;

public:
	typedef ::System::Collections::KVector<Segment> SEGMENT_LIST_TYPE;
	/**
	 * ���Ƶ��б�
	 */
	SEGMENT_LIST_TYPE m_curveListX;
	SEGMENT_LIST_TYPE m_curveListY;
	SEGMENT_LIST_TYPE m_curveListZ;
private:
	/**
	 * ��������õľ����б�[1]�����λ��[0]-[1]�ĳ��ȣ�[n]����[0]-[n]�ĳ��ȡ���¼����ÿ�ε��ۼӳ���
	 */
	::System::Collections::KVector<V_TYPE> m_distList;
private:
	/**
	 * ����dist�б�
	 */
	void UpdateDistList()
	{
		m_distList.clear();
		V_TYPE totalDist = 0;
		m_distList.push_back(totalDist);
		for (int i = 0; i < m_curveListX.size(); ++i)
		{
			Segment sx = m_curveListX[i];
			Segment sy = m_curveListY[i];
			Segment sz = m_curveListZ[i];
			VECTOR3 p0(sx.a, sy.a, sz.a);
			for (int j = 1; j < PRECISION; ++j)
			{
				V_TYPE t = (V_TYPE)(j) / (V_TYPE)(PRECISION - 1);
				V_TYPE inv = 1 - t;
				VECTOR3 p1(
						inv * inv * inv * sx.val[0] + 3 * inv * t * (inv * sx.val[1] + t * sx.val[2]) + t * t * t * sx.val[3],
						inv * inv * inv * sy.val[0] + 3 * inv * t * (inv * sy.val[1] + t * sy.val[2]) + t * t * t * sy.val[3],
						inv * inv * inv * sz.val[0] + 3 * inv * t * (inv * sz.val[1] + t * sz.val[2]) + t * t * t * sz.val[3]
					);
				totalDist += sqrt(pow(p1.x - p0.x, 2) + pow(p1.y - p0.y, 2) + pow(p1.z - p0.z, 2));
				m_distList.push_back(totalDist);
				p0 = p1;
			}
		}
	}
public:
	/**
	 * ���ݳ��ȶ�λ����
	 */
	bool GetPosByLength(V_TYPE len, VECTOR3& pt)
	{
		int left, mid, right;
		// ���ֲ��ң����left��Ҫ�����λ��
		left = 0; right = m_distList.size() - 1;
		while( left <= right )
		{
			mid = ( left + right ) / 2;
			if( m_distList[mid] < len ) left = mid + 1;
			else right = mid - 1;
		}
		if (m_curveListX.size() > 0)
		{
			if (left <= 0)
			{
				pt.x = m_curveListX[0].a;
				pt.y = m_curveListY[0].a;
				pt.z = m_curveListZ[0].a;
			}
			else if (left < m_distList.size())	// ����Ч��Χ��
			{
				len -= m_distList[left - 1];
				int currPos = (left - 1) / (PRECISION - 1);
				int currSeg = (left - 1) % (PRECISION - 1);
				V_TYPE t = ((V_TYPE)currSeg + len / (m_distList[left] - m_distList[left - 1])) / (V_TYPE)(PRECISION - 1);
				V_TYPE inv = 1 - t;
				Segment sx = m_curveListX[currPos];
				Segment sy = m_curveListY[currPos];
				Segment sz = m_curveListZ[currPos];

				pt.x = inv * inv * inv * sx.val[0] + 3 * inv * t * (inv * sx.val[1] + t * sx.val[2]) + t * t * t * sx.val[3];
				pt.y = inv * inv * inv * sy.val[0] + 3 * inv * t * (inv * sy.val[1] + t * sy.val[2]) + t * t * t * sy.val[3];
				pt.z = inv * inv * inv * sz.val[0] + 3 * inv * t * (inv * sz.val[1] + t * sz.val[2]) + t * t * t * sz.val[3];
			}
			else
			{
// 				int currPos = m_curveListX.size() - 1;
// 				pt.x = m_curveListX[currPos].d;
// 				pt.y = m_curveListY[currPos].d;
// 				pt.z = m_curveListZ[currPos].d;
				return false;
			}
			return true;
		}
		return false;
	}
	/**
	 * ���ݳ��Ȱٷֱȶ�λ
	 */
	bool GetPosByPercent(V_TYPE len, VECTOR3& pt)
	{
		if (m_distList.size() < 2) return false;
		V_TYPE length = m_distList[m_distList.size() - 1];
		if (length <= 0) return false;
		return GetPosByLength(len * length, pt);
	}
	/**
	 * ����ptlist���£�TΪλ����������Ҫ֧��ptList[pos].x,ptList[pos].y,ptList[pos].z��ʽ�Ķ�ȡ��ֵΪfloat
	 */
	template<typename T>
	void UpdateByPosList(const T& ptList)
	{
		m_curveListX.clear();
		m_curveListY.clear();
		m_curveListZ.clear();
		m_distList.clear();
		if (ptList.size() < 2) return;
		Segment sx, sy, sz;
		for (int i = 1; i < ptList.size(); ++i)
		{
			sx.a = ptList[i - 1].x;
			sy.a = ptList[i - 1].y;
			sz.a = ptList[i - 1].z;
			sx.d = ptList[i].x;
			sy.d = ptList[i].y;
			sz.d = ptList[i].z;
			m_curveListX.push_back(sx);
			m_curveListY.push_back(sy);
			m_curveListZ.push_back(sz);
		}
		CalculateControlPoints(m_curveListX);
		CalculateControlPoints(m_curveListY);
		CalculateControlPoints(m_curveListZ);
		UpdateDistList();
	}
	/**
	 * ���ݳ��ȶ�λ����
	 */
	bool GetDirectionByLength(V_TYPE len, VECTOR3& pt)
	{
		int left, mid, right;
		// ���ֲ��ң����left��Ҫ�����λ��
		left = 0; right = m_distList.size() - 1;
		while( left <= right )
		{
			mid = ( left + right ) / 2;
			if( m_distList[mid] < len ) left = mid + 1;
			else right = mid - 1;
		}
		if (m_curveListX.size() > 0)
		{
			V_TYPE t = 0;
			int currPos = 0;
			if (left <= 0)
			{
			}
			else if (left < m_distList.size())	// ����Ч��Χ��
			{
				len -= m_distList[left - 1];
				currPos = (left - 1) / (PRECISION - 1);
				int currSeg = (left - 1) % (PRECISION - 1);
				t = ((V_TYPE)currSeg + len / (m_distList[left] - m_distList[left - 1])) / (V_TYPE)(PRECISION - 1);
			}
			else
			{
				currPos = m_curveListX.size() - 1;
				t = 1;
			}
			V_TYPE inv = 1 - t;
			Segment sx = m_curveListX[currPos];
			Segment sy = m_curveListY[currPos];
			Segment sz = m_curveListZ[currPos];
			pt.x = -inv * inv * sx.a + (inv - 2 * t) * inv * sx.b + (2 * inv - t) * t * sx.c + t * t * sx.d;
			pt.y = -inv * inv * sy.a + (inv - 2 * t) * inv * sy.b + (2 * inv - t) * t * sy.c + t * t * sy.d;
			pt.z = -inv * inv * sz.a + (inv - 2 * t) * inv * sz.b + (2 * inv - t) * t * sz.c + t * t * sz.d;
			pt.normalize();

			return true;
		}
		return false;
	}
	/**
	 * ���ݳ��Ȱٷֱȵõ�����
	 */
	bool GetDirectionByPercent(V_TYPE len, VECTOR3& pt)
	{
		if (m_distList.size() < 2) return false;
		V_TYPE length = m_distList[m_distList.size() - 1];
		if (length <= 0) return false;
		return GetDirectionByLength(len * length, pt);
	}
	/**
	 * �õ������ܳ�
	 */
	V_TYPE GetTotalLength() const
	{
		if (m_distList.size())
		{
			return m_distList[m_distList.size() - 1];
		}
		return 0.0f;
	}
	/**
	 * �õ���n��ê��ĳ���
	 */
	V_TYPE GetLenByAnchorIndex(int nAnchorIndex)
	{
		int nID = nAnchorIndex * (PRECISION - 1);
		if (nID < 0 || nID >= m_distList.size())
		{
			return 0.f;
		}
		return m_distList[nID];
	}

private:
	void CalculateControlPoints(SEGMENT_LIST_TYPE& curveList, V_TYPE curviness = 0.5)
	{
		int l = curveList.size() - 1;
		V_TYPE cp1 = curveList[0].a;
		V_TYPE p1, p2, p3;
		V_TYPE m1, m2, mm, cp2;
		for (int i = 0; i < l; ++i) {
			Segment& seg = curveList[i];
			p1 = seg.a;
			p2 = seg.d;
			p3 = curveList[i + 1].d;

			m1 = p2 - (p2 - p1) * curviness * 0.5;
			m2 = p2 + (p3 - p2) * curviness * 0.5;
			mm = p2 - (m1 + m2) / 2;

			m1 += mm;
			m2 += mm;

			seg.c = cp2 = m1; 
			if (i != 0) {
				seg.b = cp1;
			} else {
				seg.b = cp1 = seg.a + (seg.c - seg.a) * 0.6; //instead of placing b on a exactly, we move it inline with c so that if the user specifies an ease like Back.easeIn or Elastic.easeIn which goes BEYOND the beginning, it will do so smoothly.
			}

			cp1 = m2;
		}
		Segment& seg = curveList[l];
		seg.b = cp1;
		seg.c = cp1 + (seg.d - cp1) * 0.4; //instead of placing c on d exactly, we move it inline with b so that if the user specifies an ease like Back.easeOut or Elastic.easeOut which goes BEYOND the end, it will do so smoothly.
	}
};

}
}