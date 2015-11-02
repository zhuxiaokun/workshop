#include "KCharset_Japan.h"

//static KCharset_Japan __KCharset_Japan;

KCharset_Japan::KCharset_Japan()
{
	m_language = KLanguage::lang_japanese;
	m_codepage = 932;
	m_name = "JP";
	m_aliases = "jis|japan|japanese";
	//ASSERT(!KCharset::m_charsets[m_language]);
	//KCharset::m_charsets[m_language] = this;
	ASSERT_I(this->_initialize());
}
