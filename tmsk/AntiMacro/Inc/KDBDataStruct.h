#pragma once

#include "KCommonDefine.h"
#include "KCommonStruct.h"
#include <mysqldb/KCacheObject.h>

#pragma pack(push, 1)

template <typename BinaryStruct, int size> class DatabaseBinaryStruct : public BinaryStruct
{
	char __data__[size-sizeof(BinaryStruct)];
};

struct KDBPlayerRec
{
	enum
	{
		f_ID,
		f_AccountID,
		f_UserType,
		f_Name,
		f_Sex,
		f_Class,	
		f_Exp,
		f_MainMoney,
		f_OffMoney,
		f_Level,
		f_HP,
		f_MP,
		f_RP,
		f_Line,
		f_MapID,
		f_X,
		f_Y,
		f_Already_Login,
	};

	DWORD P_ID;					// int(10) unsigned NOT NULL auto_increment,
	DWORD P_AccountID;			// int(10) unsigned default NULL,
	DWORD P_UserType;			// int(10) unsigned default NULL,
	KDBString<40> P_Name;		// varchar(20) default NULL,
	BYTE  P_Sex;					// tinyint(3) unsigned default NULL,
	BYTE  P_Class;				// tinyint(3) unsigned default NULL,
	DWORD P_Exp;				// int(10) unsigned default NULL,
	INT64 P_MainMoney;			// int(10) unsigned default NULL,
	INT64 P_OffMoney;			// int(10) unsigned default NULL,
	BYTE  P_Level;				// tinyint(3) unsigned default NULL,
	DWORD P_HP;					// int(10) unsigned default NULL,
	DWORD P_MP;					// int(10) unsigned default NULL,
	DWORD P_RP;					// int(10) unsigned default NULL,
	BYTE  P_Line;
	int   P_MapID;				// int(10) unsigned default NULL,
	DWORD P_X;					// int(10) unsigned default NULL,
	DWORD P_Y;					// int(10) unsigned default NULL,
	INT8  P_Already_Login;		// tinyint(1) default '0',
};

struct KDBDupStateRec
{
	enum
	{
		f_stateID,
		f_version,
		f_stateData,
		f_expireTime,
	};

	DWORD stateID;
	unsigned short version;
	KDBBinary<1024> stateData;
	TIME_T expireTime;
};

#pragma pack(pop)
