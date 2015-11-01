#pragma once
#include <System/KType.h>
#include <System/Collections/KString.h>
#include <KCommonDefine.h>

typedef UINT64 KObjectID;

typedef System::Collections::KString<32>	string_32;
typedef System::Collections::KString<64>	string_64;
typedef System::Collections::KString<128>	string_128;
typedef System::Collections::KString<256>	string_256;
typedef System::Collections::KString<512>	string_512;
typedef System::Collections::KString<1024>	string_1024;
typedef System::Collections::KString<2048>	string_2048;

struct KPassport
{
	UINT64 value;
	KPassport():value(0) { }
};

class passport_cmp
{
public:
	int operator () (const KPassport& a, const KPassport& b) const
	{
		INT64 x = a.value - b.value;
		return x > 0 ? 1 : (x == 0 ? 0 : -1);
	}
};

class passport_hasher
{
public:
	int operator () (const KPassport& val) const
	{
		return val.value & 0x7fffffff;
	}
};

class passport_less
{
public:
	bool operator () (const KPassport& a, const KPassport& b) const
	{
		return a.value < b.value;
	}
};

typedef struct _StoreNode
{
	float   m_x;
	float   m_y;
	float   m_z;

	_StoreNode():m_x(0.0f),m_y(0.0f),m_z(0.0f)
	{
	}
	_StoreNode(float x, float y, float z):m_x(x),m_y(y),m_z(z)
	{
	}
} StoreNode;

struct SChangeSceneHistory
{
	SChangeSceneHistory():dwMapID(0),busID(0),busSeat(0)
	{
		memset(busLine, 0, sizeof(busLine));
	}

	int dwMapID;
	StoreNode backPoint;
	int busID;
	WORD busSeat;
	WORD busLine[MAX_BUS_LINE_NUM];// Stream 交通工具
};

struct KRegionBox
{
	int bgrx, bgry;
	int egrx, egry;
};

#pragma pack(push, 1)

// 对象位置
struct ObjectPosition
{
	float x,y,z;

	ObjectPosition():x(0.0f),y(0.0f),z(0.0f)
	{
	}
	ObjectPosition(float X, float Y, float Z):x(X),y(Y),z(Z)
	{
	}
	string_64 toString() const
	{
		char buf[64];
		return string_64(buf, sprintf_k(buf, sizeof(buf), "%.2f,%.2f,%.2f",x,y,z));
	}
	ObjectPosition toServerSide() const
	{
		ObjectPosition serverPos;
		serverPos.x = this->x + 128.0f;
		serverPos.y = this->z + 128.0f;
		serverPos.z = this->y;
		return serverPos;
	}
	ObjectPosition toClientSide() const
	{
		ObjectPosition clientPos;
		clientPos.x = this->x - 128.0f;
		clientPos.y = this->z;
		clientPos.z = this->y - 128.0f;
		return clientPos;
	}
};

// 对象姿态
struct ObjectAttitude
{
	float x,y,z;
	ObjectAttitude():x(0.0f),y(0.0f),z(0.0f) { }
	ObjectAttitude(float X, float Y, float Z):x(X),y(Y),z(Z) { }
	string_64 toString() const { char buf[64]; return string_64(buf, sprintf_k(buf, sizeof(buf), "%.2f,%.2f,%.2f",x,y,z)); }
};

#pragma pack(pop)
DECLARE_SIMPLE_TYPE(ObjectPosition);
DECLARE_SIMPLE_TYPE(ObjectAttitude);
