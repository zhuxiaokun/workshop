#include "KCharsetConvert.h"
#include "../KPlatform.h"
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

KCharsetConvert::KCharsetConvert(const char *from_charset,const char *to_charset)
{
#if defined(WIN32) || defined(WINDOWS)
	//m_dwCpIn = CharsetStr2Code(from_charset);
	//m_dwCpOut = CharsetStr2Code(to_charset);
#else
	//m_handle = iconv_open(to_charset,from_charset);
#endif
}

KCharsetConvert::~KCharsetConvert()
{
#if defined(WIN32) || defined(WINDOWS)
#else
	//iconv_close(m_handle);
#endif
}

//DWORD KCharsetConvert::CharsetStr2Code(const char * pszCharset)
//{
//	DWORD dwCodePage = 0;
//	if (0 == strcasecmp(pszCharset, "gb2312"))
//	{
//		dwCodePage = 936;
//	}
//	else if (0 == strcasecmp(pszCharset, "shift_jis"))
//	{
//		dwCodePage = 932;
//	}
//	else if (0 == strcasecmp(pszCharset, "big5"))
//	{
//		dwCodePage = 950;
//	}
//	else if (0 == strcasecmp(pszCharset, "utf-8"))
//	{
//		dwCodePage = 65001;
//	}
//
//	return dwCodePage;
//}

//int KCharsetConvert::Convert(char *inbuf,int inlen,char *outbuf,int outlen)
//{
//#if defined(WIN32) || defined(WINDOWS)
//	
//
//
//#else
//	char **pin = &inbuf;
//	char **pout = &outbuf;
//
//	memset(outbuf,0,outlen);
//	return iconv(m_handle,pin,(size_t *)&inlen,pout,(size_t *)&outlen);
//#endif
//	return 0;
//}

int KCharsetConvert::Local2UTF8(char *pszSrc, int nSrcLen, char *pszBuff, int nBuffSize)
{
	if (NULL == pszBuff || 0 == nBuffSize)
		return 0;

	wchar_t wc;
	int nSrcPos = 0;
	int nBuffPos = 0;

	while(nSrcPos < nSrcLen) 
	{
		// Local 转到 Unicode
		int n = mbtowc(&wc, pszSrc+nSrcPos, nSrcLen-nSrcPos);
		if(n < 1)
		{
			break;
		}

		nSrcPos += n;

		//Unicode 转到 UTF-8
		//0000 - 007F  0xxxxxxx
		//0080 - 07FF 110xxxxx 10xxxxxx
		//0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx
		if(wc < 0x0080)
		{
			if (nBuffPos+1 > nBuffSize)
			{
				break;
			}
			pszBuff[nBuffPos++] = (BYTE)(wc&0xff);
		}
		else if(wc < 0x0800)
		{
			if (nBuffPos+2 > nBuffSize)
			{
				break;
			}
			BYTE by0 = 0xc0|(wc>>6);
			BYTE by1 = 0x80|(wc&0x003f);
			pszBuff[nBuffPos++] = by0;
			pszBuff[nBuffPos++] = by1;
		}
		else
		{
			if (nBuffPos+3 > nBuffSize)
			{
				break;
			}
			BYTE by0 = 0xe0|(wc>>12);
			BYTE by1 = 0x80|((wc>>6)&0x003f);
			BYTE by2 = 0x80|(wc&0x003f);
			pszBuff[nBuffPos++] = by0;
			pszBuff[nBuffPos++] = by1;
			pszBuff[nBuffPos++] = by2;
		}
	}
	pszBuff[nBuffPos] = '\0';

	return nBuffPos;
}

int KCharsetConvert::UTF82Local(char *pszSrc, int nSrcLen, char *pszBuff, int nBuffSize)
{
	if (NULL == pszBuff || 0 == nBuffSize)
		return 0;

	wchar_t wc;
	int nSrcPos = 0;
	int nBuffPos = 0;
	char achCharMaxSize[8];

	while(nSrcPos < nSrcLen)
	{
		//UTF-8 转到 Unicode
		//0000 - 007F  0xxxxxxx
		//0080 - 07FF 110xxxxx 10xxxxxx
		//0800 - FFFF 1110xxxx 10xxxxxx 10xxxxxx
		unsigned char ch = * (pszSrc + nSrcPos);

		if (ch >= 0xe0)
		{
			//3 bytes
			wchar_t w0 = (wchar_t)(*(pszSrc+nSrcPos));
			wchar_t w1 = (wchar_t)(*(pszSrc+nSrcPos+1));
			wchar_t w2 = (wchar_t)(*(pszSrc+nSrcPos+2));
			wc = (w0<<12) | ((w1&0x003f)<<6) | (w2&0x003f);
			nSrcPos+=3;
		}
		else if (ch >= 0xc0)
		{
			//2 bytes
			wchar_t w0 = (wchar_t)(*(pszSrc+nSrcPos));
			wchar_t w1 = (wchar_t)(*(pszSrc+nSrcPos+1));
			wc = (w0<<8 & 0x7fff) | (w1&0x003f);
			nSrcPos+=2;
		}
		else
		{
			//1 byte
			wc = (wchar_t)ch & 0x00ff;
			nSrcPos++;
		}

		//Unicode转换成Local
		int n = wctomb(achCharMaxSize, wc);
		if(n < 1)
		{
			break;
		}
		//空间是否够
		if(nBuffPos+n > nBuffSize)
		{
			break;
		}

		//copy
		strncpy(pszBuff+nBuffPos, achCharMaxSize,n);
		nBuffPos += n;
	}
	pszBuff[nBuffPos] = '\0';

	return nBuffPos;
}