#ifndef _K_COORDINATE_TABLE_H_
#define _K_COORDINATE_TABLE_H_

#include <System/Collections/KMapByVector.h>
#include <lua/KLuaWrap.h>

struct KCoordinatePoint
{
	int m_Id;			// [0,65535] �������	2008-10-31
	int m_Xaxis;		// [0,65535-65535] ���X������	2008-10-31
	int m_Yaxis;		// [0,65535-65535] ���Y������	2008-10-31
	int m_CDirection;	// [0,360��-1] �����ˮƽ����	2008-10-31
	int	m_CAngle;		// [0,180��-1] �������ֱ�Ƕ�	2008-10-31
	int m_CFaces;		// [0,5000��-1]	���������Ŀ������	2008-10-31
	int m_RDirection;	// [0,360��-1] ��ɫ��ˮƽ����	2008-10-31
	int m_MapID;		// ��ͼID
	const char* m_desc;	// ��ı�ע��Ϣ����ȡ 2008-10-31

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
