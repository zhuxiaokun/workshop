#ifndef _K_CHARSET_H_
#define _K_CHARSET_H_

#include "../KType.h"
#include "../KMacro.h"
#include "../Misc/KEncoding.h"
#if !defined(WIN32)
#include <iconv.h>
#endif
#include "KWordTree.h"

class KCharset
{
public:
#if defined(WIN32)
	typedef void* iconv_t;
#endif

public:
	KCharset();
	virtual ~KCharset() { }

public:
	static KCharset* getLocalCharset();
	static KCharset* getCharset(KLanguage::EnumLanguage lang);
	static KCharset* getCharset(const char* name);

public: // illegal character and word // 非法字词文件格式：文本文件，每个非法字词占一行
	BOOL loadIllegalWords(const char* filename);
	BOOL loadIllegalNameWords(const char* filename);

public:
	BOOL loadIllegalWords(StreamInterface& si);
	BOOL loadIllegalNameWords(StreamInterface& si);
	
public:
	BOOL checkName(const char* name, int len=0);
	BOOL checkIllegal(const char* str);
	BOOL checkIllegal2(const char* str, char* matched, int len);
	BOOL replaceIllegal(char* str, int chr);

	// 字符串中的字符个数
	int_r countChar(const char* str, size_t len) const;

public:
	BOOL SetInvalidChars(const char* s, int len=0);
	// 返回值为发现的第一个非法字符位置，如果未发现非法字符，返回 -1
	int FindInvalidChar(const char* s, int len=0);

public:
	void SetInvalid(int chr);
	BOOL IsValidChar(int chr) const;
	int  ReadOneChar(const void* p, int len, int& chr) const;
	BOOL IsReadonly() const;

public: // new interface for international
	int to_uchar(const char* src, int len, unsigned int* uchar) const;
	int from_uchar(unsigned int uchar, char* dst, int dstlen) const;
	int to_utf8(const char** srcP, int* srclenP, char** dstP, int* dstlenP) const;
	int from_utf8(const char** srcP, int* srclenP, char** dstP, int* dstlenP) const;

protected:
	// assume m_language, m_codepage, m_name, m_aliases are all supplied
	BOOL _initialize();
	size_t _to_uchar(const char* str, size_t len, charset::uchar32* uchr);

protected:
	BOOL m_readonly;
	KCharset* m_pOriginal;

public:
	int m_language;
	int m_codepage;
	const char* m_name;
	const char* m_aliases; // separate by '|'
	iconv_t m_iconv_utf8, m_iconv_utf8_r;
	iconv_t m_iconv_utf32, m_iconv_utf32_r;

	charset::uchar32_array m_invalid_char_arr;
	charset::KWordTree m_illegalWordTree;
	charset::KWordTree m_illegalNameWordTree;

public:
	static KCharset* m_charsets[KLanguage::lang_count];
};

#endif
