//////////////////////////////////////////////////////////////////////
/// 
/// KPassportDefine.h
/// 
//////////////////////////////////////////////////////////////////////

#pragma once
#include <RSADefine.h>

#define	MAX_MD5_STRING				33							//  极光游戏帐号长度
#define	MAX_LOGIN_REQUEST			512							//  极光登录信息长度

#define	SDOA_MAX_ACCOUNT			64							//  盛大帐号长度
#define	SDOA_MAX_SESSION			256							//  盛大Session长度

#define	SDOA_MAX_ACCOUNT_DEF		SDOA_MAX_ACCOUNT+2			//  盛大帐号长度定义
#define	SDOA_MAX_SESSION_DEF		SDOA_MAX_SESSION+2			//  盛大Session长度定义

#define	SDO_ORDER_BUFF_SIZE			34
#define	SDO_APPENDIX_BUFF_SIZE		128

enum enumAccountType
{
	eAccount_Unknown = 0,
	eAccount_Aurora = 1,
	eAccount_SDO = 11,
};

//用作Log
enum enumSDORechargeOperateLog
{
	eSDORecharge_Unknown = 0,
	eSDORecharge_FromSdoRequest = 10,
	eSDORecharge_ToBillling = 20,
	eSDORecharge_FromBillling_OK = 30,
	eSDORecharge_FromBillling_Error = 31,
};
