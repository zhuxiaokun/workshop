#pragma once
#include "../KType.h"
#include "../KMacro.h"

enum EnumEncoding
{
	encode_ansii,
	encode_utf8,
	encode_utf16_be, // big endian
	encode_utf16_le, // little endian
	encode_count,
};

// 系统使用的编码方式
extern EnumEncoding g_encoding;

class KLanguage
{
public:
	enum EnumLanguage
	{
		lang_ascii,
		lang_english,
		lang_chinese,
		lang_big5,
		lang_japanese,
		lang_korean,
		lang_russian,
		lang_usa,
		lang_count,
	};

public:
	bool isValidLanguage(int lang) const;
	EnumLanguage languageFromName(const char* langName) const;
	const char* languageName(EnumLanguage lang) const;
	int getCurrentLanguage() const;

protected:
	int _get_lang(const char* curr_locale) const;
};
