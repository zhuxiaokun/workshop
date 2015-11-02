//
//#include "RsaNew.h"
//#include <string>
//#include <algorithm>
//#include <openssl/evp.h>
//#include <openssl/rsa.h>
//
//
//bool bFirst = true;
//CRSA::CRSA( const unsigned char *szKey,const int len ):m_szKey(szKey),m_len(len)
//{
//	memset( m_szOut,0,sizeof(m_szOut) );
//}
//
//void My_add_all_algorithms(void) 
//{ 
//	OpenSSL_add_all_ciphers(); /* 加载加密算法 */ 
//	OpenSSL_add_all_digests(); /* 加载单向散列函数 */ 
//} 
//
//bool CRSA::MakeRsa( st_OP_KEY &ucPubKey,st_OP_KEY &ucPriKey )
//{
//	if( bFirst )
//	{
//		My_add_all_algorithms();
//		bFirst = false;
//	}
//
//	// Save SessionKey
//	RSA* rsa = RSA_generate_key( MAX_SESSION_BITS,RSA_F4,0,0);
//	if( rsa == NULL )
//		return false;
//	//RSA_print_fp(stdout, rsa, 5);
//
//	ucPriKey.len = i2d_RSAPrivateKey(rsa,NULL);
//	unsigned char *pszPri = ucPriKey.ucKey;
//	ucPriKey.len = i2d_RSAPrivateKey(rsa,&pszPri);
//
//	ucPubKey.len = i2d_RSAPublicKey(rsa,NULL);
//	unsigned char *pszPub = ucPubKey.ucKey;
//	ucPubKey.len = i2d_RSAPublicKey(rsa, &pszPub);
//
//	RSA_free(rsa);
//	return true;
//}
//
////RSA_public_encrypt() returns the size of the encrypted data
//int CRSA::Encrypt( const unsigned char * szSource,const unsigned int nSourceLen )
//{
//	RSA* rsaPub = d2i_RSAPublicKey(NULL, &m_szKey, m_len );
//	if( rsaPub == NULL ) return false;
//
//	int len = RSA_size( rsaPub );
//	if( len >= MAX_SESSION_STRING ) return false;
//
//	int nReturn = RSA_public_encrypt( nSourceLen , szSource , m_szOut , rsaPub ,  RSA_PKCS1_PADDING );
//	RSA_free(rsaPub);
//
//	return nReturn ;
//}
//
////RSA_private_decrypt() returns the size of the recovered plaintext.
//int CRSA::Decrypt( const unsigned char * szSource )
//{
//	RSA* rsaPri = d2i_RSAPrivateKey(NULL, &m_szKey, m_len);
//	if(rsaPri==NULL) return false;
//
//	int raslen = RSA_size( rsaPri );
//	int nReturn = RSA_private_decrypt( raslen , szSource , m_szOut , rsaPri , RSA_PKCS1_PADDING );
//	RSA_free(rsaPri);
//
//	return nReturn ;
//}
//
// 
