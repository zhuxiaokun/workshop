#include "KCharset_Big5.h"

//static KCharset_Big5 __KCharset_Big5;

KCharset_Big5::KCharset_Big5()
{
	m_language = KLanguage::lang_big5;
	m_codepage = 950;
	m_name = "BIG5";
	m_aliases = "tw|twn|cht|chinese-traditional";
	//ASSERT(!KCharset::m_charsets[m_language]);
	//KCharset::m_charsets[m_language] = this;
	ASSERT_I(this->_initialize());
}
