
/********************************************************************
history:
created:	2006/01/07
created:	7:1:2006   16:19

filename: 	\SanGuo\SanGuoMain\Source\Common\SGWorld\Source\DataStructure\KPoint.h
file path:	\SanGuo\SanGuoMain\Source\Common\SGWorld\Source\DataStructure
file base:	KPoint
file ext:	h

author:		xueyan
copyright:  KingSoft 2005-2007

purpose:	一个二维点和一个三维点类
comment:    
*********************************************************************/

#pragma once
//#include "../GlobalDefine.h"

namespace DataStructureAbout
{

	class KPoint
	{
	public:
		int m_nX;
		int m_nY;

	public:

		~KPoint(void)
		{
		}

		KPoint(int nX = 0, int nY = 0)
			: m_nX(nX)
			, m_nY(nY)
		{
		}

		KPoint& operator =(const KPoint& point)
		{
			m_nX = point.m_nX;
			m_nY = point.m_nY;

			return *this;
		}

		bool operator ==(KPoint& p1)
		{
			return (p1.m_nX == m_nX && p1.m_nY == m_nY);
		}

	};

	class K3DPoint
	{
	public:
		float m_fX;
		float m_fY;
		float m_fZ;

		~K3DPoint(void)
		{}

		K3DPoint(float fX = 0.00f, float fY = 0.00f, float fZ = 0.00f)
			: m_fX(fX)
			, m_fY(fY)
			, m_fZ(fZ)
		{}

		K3DPoint& operator =(const K3DPoint& point)
		{
			m_fX = point.m_fX;
			m_fY = point.m_fY;
			m_fZ = point.m_fZ;

			return *this;
		}

		bool operator ==(K3DPoint& p1)
		{
			return (p1.m_fX == m_fX && p1.m_fY == m_fY && p1.m_fZ == m_fZ);
		}
	};

	/*bool operator == (const KPoint& p1, const KPoint& p2);

	KPoint operator - (const KPoint& p1, const KPoint& p2);

	KPoint operator * (const KPoint& p1, int nVal);


	bool operator == (const KPoint& p1, const KPoint& p2)
	{
		return (p1.m_nX == p2.m_nX && p1.m_nY == p2.m_nY);
	}

	KPoint operator - (const KPoint& p1, const KPoint& p2)
	{
		KPoint p;
		p.m_nX = p1.m_nX - p2.m_nX;
		p.m_nY = p1.m_nY - p2.m_nY;

		return p;
	}

	KPoint operator * (const KPoint& p1, int nVal)
	{
		KPoint p;
		p.m_nX = p1.m_nX * nVal;
		p.m_nY = p1.m_nY * nVal;

		return p;
	}*/
} // end namespace DataStructureAbout

using namespace DataStructureAbout;
