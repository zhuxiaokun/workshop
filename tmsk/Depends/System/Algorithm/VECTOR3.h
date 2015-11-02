/********************************************************************
created:	2012/4/7
manager:	Allen Wang
copyright:	Aurora Interactive, 2012
*********************************************************************/

#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

namespace System
{
namespace Algorithm
{
/**
 * 一个三维向量
 */
struct VECTOR3
{
	typedef float V_TYPE;
	V_TYPE x;
	V_TYPE y;
	V_TYPE z;
	VECTOR3(V_TYPE x = 0, V_TYPE y = 0, V_TYPE z = 0) : x(x), y(y), z(z)
	{};
	inline VECTOR3 operator+(const VECTOR3& pt2) const
	{
		VECTOR3 pt;
		pt.x = x + pt2.x;
		pt.y = y + pt2.y;
		pt.z = z + pt2.z;
		return pt;
	}
	inline VECTOR3 operator-(const VECTOR3& pt2) const
	{
		VECTOR3 pt;
		pt.x = x - pt2.x;
		pt.y = y - pt2.y;
		pt.z = z - pt2.z;
		return pt;
	}
	template <typename T>
	inline VECTOR3 operator*(const T& v) const
	{
		VECTOR3 pt;
		pt.x = x * (V_TYPE)v;
		pt.y = y * (V_TYPE)v;
		pt.z = z * (V_TYPE)v;
		return pt;
	}
	template <typename T>
	inline VECTOR3 operator/(const T& v) const
	{
		VECTOR3 pt;
		pt.x = x / (V_TYPE)v;
		pt.y = y / (V_TYPE)v;
		pt.z = z / (V_TYPE)v;
		return pt;
	}
	inline VECTOR3& normalize()
	{
		V_TYPE v = sqrt(x * x + y * y + z * z);
		if (v == 0.0f) return *this;
		x /= v;
		y /= v;
		z /= v;
		return *this;
	}
};

}
}