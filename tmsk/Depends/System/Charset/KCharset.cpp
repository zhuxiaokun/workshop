#include "KCharset.h"
#include "../Misc/StrUtil.h"
#include "KCharset_Normal.h"
#include "KCharset_Ascii.h"
#include "KCharset_GB2312.h"
#include "../Sync/KSync.h"
#include "../Misc/KStream.h"

#include "KCharset_Ascii.h"
#include "KCharset_GB2312.h"
#include "KCharset_Big5.h"
#include "KCharset_Japan.h"

static KCharset* m_localCharset = NULL;
KCharset* KCharset::m_charsets[KLanguage::lang_count] = {
				new KCharset_Ascii(),		// lang_ascii
				new KCharset_Ascii(),		// lang_english
				new KCharset_GB2312(),		// lang_chinese
				new KCharset_Big5(),		// lang_big5
				new KCharset_Japan(),		// lang_japanese
				NULL,						// lang_korean
				NULL,						// lang_russian
				new KCharset_Ascii(),		// lang_usa
};

KCharset::KCharset():m_readonly(TRUE),m_pOriginal(NULL)
{
	m_pOriginal = this;
	m_language = 0;
	m_codepage = 0;
	m_name = NULL;
	m_aliases = NULL; // separate by '|'
	m_iconv_utf8 = m_iconv_utf8_r = NULL;
	m_iconv_utf32 = m_iconv_utf32_r = NULL;
}

static ::System::Sync::KThreadMutex _mx_getLocalCharset;
KCharset* KCharset::getLocalCharset()
{
	if(m_localCharset) return m_localCharset;
	::System::Sync::KAutoThreadMutex mx(_mx_getLocalCharset);
	if(m_localCharset) return m_localCharset;
	int lang = KLanguage().getCurrentLanguage();
	m_localCharset = KCharset::getCharset((KLanguage::EnumLanguage)lang);
	return m_localCharset;
}

KCharset* KCharset::getCharset(KLanguage::EnumLanguage lang)
{
	if(lang < 0 || lang >= KLanguage::lang_count) return NULL;
	//KCharset::_initialize_all();
	return m_charsets[lang];
}

KCharset* KCharset::getCharset(const char* name)
{
	//KCharset::_initialize_all();
	char tmp[512]; char* alises[32];
	for(int i=0; i<KLanguage::lang_count; i++)
	{
		KCharset* p = m_charsets[i]; if(!p) continue;
		if(stricmp(name, p->m_name) == 0) return p;
		
		strcpy_k(tmp, sizeof(tmp), p->m_aliases);
		int n = split(tmp, '|', alises, sizeof(alises)/sizeof(char*));
		for(int k=0; k<n; k++)
		{
			const char* nm = alises[k];
			if(stricmp(nm, name) == 0) return p;
		}
	}
	return NULL;
}

BOOL KCharset::loadIllegalWords(const char* filename)
{
	KInputFileStream fi(filename);
	if(!fi.Good()) return FALSE;
	return this->loadIllegalWords(fi);
}

BOOL KCharset::loadIllegalNameWords(const char* filename)
{
	KInputFileStream fi(filename);
	if(!fi.Good()) return FALSE;
	return this->loadIllegalNameWords(fi);
}

BOOL KCharset::loadIllegalWords(StreamInterface& si)
{
	if(g_encoding == encode_utf8)
		return m_illegalWordTree.loadFile(NULL, si);
	return m_illegalWordTree.loadFile(this, si);
}

BOOL KCharset::loadIllegalNameWords(StreamInterface& si)
{
	if(g_encoding == encode_utf8)
		return m_illegalNameWordTree.loadFile(NULL, si);
	return m_illegalNameWordTree.loadFile(this, si);
}

BOOL KCharset::checkName(const char* name, int _len)
{
	charset::uchar32 uname[256];
	size_t char_count = 0;
	size_t len = _len ? _len : strlen(name);
	while(*name && char_count < 255)
	{
		charset::uchar32 uchr;
		size_t n = this->_to_uchar(name, len, &uchr);
		if(n < 1) return FALSE; name += n; len -= n;
		//if(!this->IsValidChar(uchr)) return FALSE;
		uname[char_count++] = uchr;
	}	uname[char_count] = 0;

	const charset::uchar32* matched;
	KCharset* cs = g_encoding == encode_utf8 ? NULL : this;
	return !m_illegalNameWordTree._match_uchar(cs, uname, &matched);
}

BOOL KCharset::checkIllegal(const char* str)
{
	if(g_encoding == encode_utf8)
		return m_illegalWordTree.match(NULL, str);
	return m_illegalWordTree.match(this, str);
}

BOOL KCharset::checkIllegal2(const char* str, char* matched, int len)
{
	if(g_encoding == encode_utf8)
		return m_illegalWordTree.match2(NULL, str, matched, len);
	return m_illegalWordTree.match2(this, str, matched, len);
}

BOOL KCharset::replaceIllegal(char* str, int chr)
{
	if(g_encoding == encode_utf8)
		return m_illegalWordTree.matchReplace(NULL, str, chr);
	return m_illegalWordTree.matchReplace(this, str, chr);
}

int_r KCharset::countChar(const char* str, size_t len) const
{
	int_r count = 0;
	unsigned int uchar;
	if(g_encoding == encode_utf8)
	{
		while(len > 0)
		{
			int n = utf8ToUnicode(str, &uchar);
			if(!n) return count;
			str += n; len -= n; count++;
		}
		return count;
	}
	else
	{
		while(len > 0)
		{
			int n = this->to_uchar(str, (int)len, &uchar);
			if(!n) return count;
			str += n; len -= n; count++;
		}
		return count;
	}
}

BOOL KCharset::SetInvalidChars(const char* s, int len)
{
	int chr;
	if(!len) len = (int)strlen(s);
	while(len > 0)
	{
		int chrlen = m_pOriginal->ReadOneChar(s, len, chr);
		if(!chrlen) return FALSE;

		this->SetInvalid(chr);

		s += chrlen;
		len -= chrlen;
	}
	return TRUE;
}

int KCharset::FindInvalidChar(const char* s, int len)
{
	int chr;
	if(!len) len = (int)strlen(s);

	int lencpy = len;

	while(len > 0)
	{
		// 按该字符集能读出来就认为是合法字符了
		int chrlen = this->ReadOneChar(s, len, chr);
		if(!chrlen) return (lencpy - len);
		if(!this->IsValidChar(chr)) return (lencpy - len);
		s += chrlen;
		len -= chrlen;
	}
	return -1;
}

void KCharset::SetInvalid(int chr)
{
	m_invalid_char_arr.insert_unique(chr);
}

BOOL KCharset::IsValidChar(int chr) const
{
	return m_invalid_char_arr.find(chr) < 0;
}

int KCharset::ReadOneChar(const void* p, int len, int& chr) const
{
	int n;
	if(g_encoding == encode_utf8)
	{
		n = utf8ToUnicode((char*)p, (charset::uchar32*)&chr);
	}
	else
	{
		n = this->to_uchar((char*)p, len, (charset::uchar32*)&chr);
	}
	return n > 0 ? n : 0;
}

BOOL KCharset::IsReadonly() const
{
	return m_readonly;
}

int KCharset::to_uchar(const char* src, int len, unsigned int* uchar) const
{
	*uchar = 0;
#if defined(WIN32)
	int bytes = IsDBCSLeadByteEx(m_codepage,src[0]) ? 2:1;
	if(len < bytes) return -1;
	int n = MultiByteToWideChar(m_codepage, 0, src, bytes, (wchar_16*)uchar, 1);
	if(!n) return -1;
	return bytes;
#else
	char* src2 = (char*)src; size_t srclen = len;
	char* dst2 = (char*)uchar; size_t dstlen = sizeof(int);
	size_t n = iconv(m_iconv_utf32, &src2, &srclen, &dst2, &dstlen);
	if(n == -1 && errno != E2BIG) return -1;
	return (int)(src2 - src);
#endif
}

int KCharset::from_uchar(unsigned int uchr, char* dst, int dstlen) const
{
	char tmp[8];
	if(uchr == 0) return 0;
#if defined(WIN32)
	int n = WideCharToMultiByte(m_codepage, WC_NO_BEST_FIT_CHARS, (wchar_16*)&uchr, 1, tmp, sizeof(tmp), NULL, NULL);
	if(!n) return -1;
#else
	char* src2 = (char*)&uchr; size_t len = sizeof(uchr);
	char* dst2 = tmp; size_t dstlen2 = sizeof(tmp);
	int n = (int)iconv(m_iconv_utf32_r, &src2, &len, &dst2, &dstlen2);
	if(n == -1) return -1; n = (int)(sizeof(tmp) - dstlen2);
#endif
	if(dstlen < n) return -1;
	memcpy(dst, tmp, n);
	return n;
}

int KCharset::to_utf8(const char** srcP, int* srclenP, char** dstP, int* dstlenP) const
{
	char* src = (char*)*srcP; char* dst = *dstP;
	size_t srclen = *srclenP, dstlen = *dstlenP;
#if defined(WIN32)
	unsigned int uchar = 0; char ctmp[8];
	while(src[0] && srclen > 0)
	{
		int n = this->to_uchar(src, (int)srclen, &uchar);
		if(n < 0)
		{
			*srcP = src; *srclenP = (int)srclen;
			*dstP = dst; *dstlenP = (int)dstlen;
			return -1;
		}
		int n2 = unicodeToUtf8(uchar, ctmp, sizeof(ctmp));
		if(n2 < 0)
		{
			*srcP = src; *srclenP = (int)srclen;
			*dstP = dst; *dstlenP = (int)dstlen;
			return -1;
		}
		if((int)dstlen < n2) break; memcpy(dst, ctmp, n2);
		src += n; srclen -= n; dst += n2; dstlen -= n2;
	}
	int wbytes = (int)(dst - *dstP); if(dstlen > 0) *dst = '\0';
	*srcP = src; *srclenP = (int)srclen; *dstP = dst; *dstlenP = (int)dstlen;
	return wbytes;
#else
	size_t n = iconv(m_iconv_utf8, &src, &srclen, dstP, &dstlen);
	if(n == -1)
	{
		*srcP = src;
		*srclenP = srclen;
		*dstlenP = dstlen;
		return -1;
	}
	if(dstlen > 0) (*dstP)[0] = '\0';
	*srcP = src; *srclenP = srclen; *dstlenP = dstlen;
	return (int)(*dstP - dst);
#endif
}

int KCharset::from_utf8(const char** srcP, int* srclenP, char** dstP, int* dstlenP) const
{
	char* src = (char*)*srcP; char* dst = *dstP;
	size_t srclen = *srclenP, dstlen = *dstlenP;
#if defined(WIN32)
	unsigned int uchar = 0; char ctmp[8];
	while(src[0] && srclen > 0)
	{
		int n = utf8ToUnicode(src, &uchar);
		if(n < 0)
		{
			*srcP = src; *srclenP = (int)srclen;
			*dstP = dst; *dstlenP = (int)dstlen;
			return -1;
		}
		int n2 = this->from_uchar(uchar, ctmp, sizeof(ctmp));
		if(n2 < 0)
		{
			*srcP = src; *srclenP = (int)srclen;
			*dstP = dst; *dstlenP = (int)dstlen;
			return -1;
		}
		if((int)dstlen < n2) break; memcpy(dst, ctmp, n2);
		src += n; srclen -= n; dst += n2; dstlen -= n2;
	}
	int wbytes = (int)(dst - *dstP); if(dstlen > 0) *dst = '\0';
	*srcP = src; *srclenP = (int)srclen; *dstP = dst; *dstlenP = (int)dstlen;
	return wbytes;
#else
	size_t n = iconv(m_iconv_utf8_r, &src, &srclen, dstP, &dstlen);
	if(n == -1)
	{
		*srcP = src;
		*srclenP = srclen;
		*dstlenP = dstlen;
		return -1;
	}
	if(dstlen > 0) (*dstP)[0] = '\0';
	*srcP = src; *srclenP = srclen; *dstlenP = dstlen;
	return (int)(*dstP - dst);
#endif
}

// assume m_language, m_codepage, m_name, m_aliases are all supplied 
BOOL KCharset::_initialize()
{
#if defined(WIN32)
	return TRUE;
#else
	m_iconv_utf8 = iconv_open("UTF-8", m_name);
	m_iconv_utf8_r = iconv_open(m_name, "UTF-8");
	m_iconv_utf32 = iconv_open("UTF-32LE", m_name);
	m_iconv_utf32_r = iconv_open(m_name, "UTF-32LE");
	return m_iconv_utf8 != NULL
		&& m_iconv_utf8_r != NULL
		&& m_iconv_utf32 != NULL
		&& m_iconv_utf32_r != NULL;
#endif
}

size_t KCharset::_to_uchar(const char* str, size_t len, charset::uchar32* uchr)
{
	return g_encoding == encode_utf8 ? utf8ToUnicode(str, uchr) : this->to_uchar(str, (int)len, uchr);
}
