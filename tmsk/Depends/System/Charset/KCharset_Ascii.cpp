#include "KCharset_Ascii.h"
#include "../Misc/StrUtil.h"
#include "../Memory/KMemory.h"

//static KCharset_Ascii __charset_ascii;

KCharset_Ascii::KCharset_Ascii()
{
	m_language = KLanguage::lang_ascii;
	m_codepage = 28591;
	m_name = "ISO_8859-1";
	m_aliases = "ascii";
	//ASSERT(!KCharset::m_charsets[m_language]);
	//KCharset::m_charsets[m_language] = this;
	ASSERT_I(this->_initialize());
}

KCharset_Ascii::~KCharset_Ascii()
{
	
}
