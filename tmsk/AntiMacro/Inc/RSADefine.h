//////////////////////////////////////////////////////////////////////
/// 
/// KSADefine.h
/// 
//////////////////////////////////////////////////////////////////////

#ifndef __KSADEFINE_H__
#define __KSADEFINE_H__

#define	MAX_SESSION_STRING			65							
#define	MAX_SESSION_BITS			((MAX_SESSION_STRING-1)*8)	//  Session Bits

struct st_OP_KEY
{
	unsigned char ucKey[MAX_SESSION_BITS];
	int len;
};

#endif
