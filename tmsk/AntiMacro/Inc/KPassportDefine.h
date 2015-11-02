//////////////////////////////////////////////////////////////////////
/// 
/// KPassportDefine.h
/// 
//////////////////////////////////////////////////////////////////////

#pragma once
#include <RSADefine.h>

#define	MAX_MD5_STRING				33							//  ������Ϸ�ʺų���
#define	MAX_LOGIN_REQUEST			512							//  �����¼��Ϣ����

#define	SDOA_MAX_ACCOUNT			64							//  ʢ���ʺų���
#define	SDOA_MAX_SESSION			256							//  ʢ��Session����

#define	SDOA_MAX_ACCOUNT_DEF		SDOA_MAX_ACCOUNT+2			//  ʢ���ʺų��ȶ���
#define	SDOA_MAX_SESSION_DEF		SDOA_MAX_SESSION+2			//  ʢ��Session���ȶ���

#define	SDO_ORDER_BUFF_SIZE			34
#define	SDO_APPENDIX_BUFF_SIZE		128

enum enumAccountType
{
	eAccount_Unknown = 0,
	eAccount_Aurora = 1,
	eAccount_SDO = 11,
};

//����Log
enum enumSDORechargeOperateLog
{
	eSDORecharge_Unknown = 0,
	eSDORecharge_FromSdoRequest = 10,
	eSDORecharge_ToBillling = 20,
	eSDORecharge_FromBillling_OK = 30,
	eSDORecharge_FromBillling_Error = 31,
};
