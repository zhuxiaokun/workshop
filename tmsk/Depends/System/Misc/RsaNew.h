//#ifndef __CRSA_H__
//#define __CRSA_H__
// 
//#define	MAX_SESSION_STRING			65							
//#define	MAX_SESSION_BITS			((MAX_SESSION_STRING-1)*8)	//  Session Bits
//
//struct st_OP_KEY
//{
//	unsigned char ucKey[MAX_SESSION_BITS];
//	int len;
//};
// 
//class CRSA 
//{
//	const unsigned char *m_szKey;
//	const int m_len;
//public:
//	unsigned char m_szOut[MAX_SESSION_STRING];
//
//public:
//	CRSA( const unsigned char *szKey,const int len );
//
//	static bool MakeRsa( st_OP_KEY &ucPubKey,st_OP_KEY &ucPriKey );
//
//	int Encrypt(const unsigned char * szSource,const unsigned int nSourceLen );
//
//	int Decrypt(const unsigned char * szSource );
//
//};
//
//#endif