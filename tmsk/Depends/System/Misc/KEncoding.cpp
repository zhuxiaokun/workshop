#include "KEncoding.h"
#include "StrUtil.h"
#include <locale.h>
#include "../Charset/KCharset.h"
#include "../Log/log.h"

bool KLanguage::isValidLanguage(int lang) const
{
	return lang > lang_ascii && lang < lang_count;
}

KLanguage::EnumLanguage KLanguage::languageFromName(const char* langName) const
{
	KCharset* charset = KCharset::getCharset(langName);
	return charset ? (EnumLanguage)charset->m_language : lang_ascii;
}

const char* KLanguage::languageName(KLanguage::EnumLanguage lang) const
{
	KCharset* charset = KCharset::getCharset(lang);
	return charset ? charset->m_name : "chinese";
}

int KLanguage::getCurrentLanguage() const
{
	EnumLanguage lang = lang_chinese;
	
	char ctmp[256] = {0};
	const char* org = setlocale(LC_CTYPE, NULL);
	if(org) strcpy_k(ctmp, sizeof(ctmp), org);

	const char* current = setlocale(LC_CTYPE, "");
	if(current) lang = (EnumLanguage)this->_get_lang(current);

	if(ctmp[0]) setlocale(LC_CTYPE, ctmp);
	return lang;
}

int KLanguage::_get_lang(const char* curr_locale) const
{
	char ctmp[512]; strcpy_k(ctmp, sizeof(ctmp), curr_locale);
	char* ss[3]; int n = split(ctmp, "_.", ss, 3);
	if(n != 3) return lang_chinese;
	KCharset* charset = KCharset::getCharset(ss[1]);
	if(!charset) return lang_chinese;
	return charset->m_language;
}
