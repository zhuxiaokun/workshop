#include "KCharset_GB2312.h"
#include "../Memory/KMemory.h"
#include <string.h>

//static KCharset_GB2312 __KCharset_GB2312;

KCharset_GB2312::KCharset_GB2312()
{
	m_language = KLanguage::lang_chinese;
	m_codepage = 936;
	m_name = "GBK";
	m_aliases = "chinese|china|gb2312|cn|hz|chn|chs|chinese-simplified";
	//ASSERT(!KCharset::m_charsets[m_language]);
	//KCharset::m_charsets[m_language] = this;
	ASSERT_I(this->_initialize());
}

KCharset_GB2312::~KCharset_GB2312()
{
	
}
