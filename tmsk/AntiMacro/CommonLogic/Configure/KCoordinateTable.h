#ifndef _K_COORDINATE_TABLE_H_
#define _K_COORDINATE_TABLE_H_

#include <System/Collections/KMapByVector.h>
#include <lua/KLuaWrap.h>

struct KCoordinatePoint
{
	int m_Id;			// [0,65535] 点的索引	2008-10-31
	int m_Xaxis;		// [0,65535-65535] 点的X轴坐标	2008-10-31
	int m_Yaxis;		// [0,65535-65535] 点的Y轴坐标	2008-10-31
	int m_CDirection;	// [0,360，-1] 摄像机水平朝向	2008-10-31
	int	m_CAngle;		// [0,180，-1] 摄像机垂直角度	2008-10-31
	int m_CFaces;		// [0,5000，-1]	摄像机距离目标点距离	2008-10-31
	int m_RDirection;	// [0,360，-1] 角色的水平朝向	2008-10-31
	int m_MapID;		// 地图ID
	const char* m_desc;	// 点的备注信息不读取 2008-10-31

/////////////////////////////////////////////////
	int id() const	{ return m_Id;    }
	int x() const	{ return m_Xaxis; }
	int y() const	{ return m_Yaxis; }
	int map() const	{ return m_MapID; }
/////////////////////////////////////////////////
	BeginDefLuaClassNoCon(KCoordinatePoint);
		DefMemberFunc(id);
		DefMemberFunc(x);
		DefMemberFunc(y);
		DefMemberFunc(map);
	EndDef;
};

class KCoordinateTable
{
public:
	typedef JG_C::KMapByVector2<int,KCoordinatePoint> Map;

public:
	KCoordinateTable();
	~KCoordinateTable();

public:
	BOOL Initialize(const char* filename);
	void Finalize();
	const KCoordinatePoint* GetPoint(int pointID) const;

private:
	BOOL _add(const KCoordinatePoint& point);
	BOOL _remove(int pointID);

public:
	Map m_map;
};

#endif
