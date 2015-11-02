/********************************************************************
created:	2014/2/24
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
 * 连续折线
 */
class KPolyline
{
public:
	KPolyline(void){};
	virtual ~KPolyline(void){};

	// 曲线计算点时用的数据精度
	typedef float V_TYPE;

public:
	/**
	 * 控制点列表
	 */
	::System::Collections::KVector<VECTOR3> m_ptList;
	/**
	 * 方便计算用的距离列表。[1]代表点位置[0]-[1]的长度，[n]代表[0]-[n]的长度
	 */
private:
	::System::Collections::KVector<V_TYPE> m_distList;
public:
	/**
	 * 更新dist列表
	 */
	void UpdateDistList()
	{
		m_distList.clear();
		V_TYPE totalDist = 0;
		m_distList.push_back(totalDist);
		for (int i = 1; i < m_ptList.size(); ++i)
		{
			VECTOR3 v0 = m_ptList[i - 1];
			VECTOR3 v1 = m_ptList[i];
			totalDist += sqrt(pow(v0.x - v1.x, 2) + pow(v0.y - v1.y, 2) + pow(v0.z - v1.z, 2));
			m_distList.push_back(totalDist);
		}
	}
	/**
	 * 根据长度定位坐标
	 */
	bool GetPosByLength(V_TYPE len, VECTOR3& pt)
	{
		int left, mid, right;
		// 二分查找，最后left是要插入的位置
		left = 0; right = m_distList.size() - 1;
		while( left <= right )
		{
			mid = ( left + right ) / 2;
			if( m_distList[mid] < len ) left = mid + 1;
			else right = mid - 1;
		}
		if (m_ptList.size() >= 2)
		{
			if (left <= 0)
			{
				pt = m_ptList[0];
			}
			else if (left < m_distList.size())	// 在有效范围内
			{
				len -= m_distList[left - 1];
				VECTOR3 v = m_ptList[left] - m_ptList[left - 1];
				pt = v.normalize() * len + m_ptList[left - 1];
			}
			else
			{
				pt = m_ptList[m_ptList.size() - 1];
			}
			return true;
		}
		return false;
	}
	/**
	 * 根据长度百分比定位
	 */
	bool GetPosByPercent(V_TYPE len, VECTOR3& pt)
	{
		if (m_distList.size() < 2) return false;
		V_TYPE length = m_distList[m_distList.size() - 1];
		if (length <= 0) return false;
		return GetPosByLength(len * length, pt);
	}
	/**
	 * 根据ptlist更新
	 */
	void UpdateByPosList(const ::System::Collections::KVector<VECTOR3>& posList)
	{
		m_ptList = posList;
		UpdateDistList();
	}
	/**
	 * 得到总长度
	 */
	V_TYPE GetTotalLength() const
	{
		if (m_distList.size() < 2) return 0;
		return m_distList[m_distList.size() - 1];
	}
};

}
}