#ifndef _K_CHARSET_CONVERT_H_
#define _K_CHARSET_CONVERT_H_

#include "../KType.h"

#if defined(WIN32) || defined(WINDOWS)

#else
//#include <iconv.h>
#endif

class KCharsetConvert
{
public:
	KCharsetConvert(const char *from_charset,const char *to_charset);
	~KCharsetConvert();

	//DWORD CharsetStr2Code(const char * pszCharset);
	//int Convert(char *inbuf,int inlen,char *outbuf,int outlen);
	static int Local2UTF8(char *pszSrc, int nSrcLen, char *pszBuff, int nBuffSize);
	static int UTF82Local(char *pszSrc, int nSrcLen, char *pszBuff, int nBuffSize);
private:
	//DWORD m_dwCpIn, m_dwCpOut;

#if defined(WIN32) || defined(WINDOWS)
#else
	//iconv_t m_handle;
#endif

};



#endif