//#include "KCharset_Normal.h"
//#include "../Misc/StrUtil.h"
//
//KCharset_Normal::KCharset_Normal()
//{
//
//}
//
//KCharset_Normal::~KCharset_Normal()
//{
//
//}
//
//KCharset_Normal* KCharset_Normal::getInstance()
//{
//	static KCharset_Normal* p = new KCharset_Normal();
//	return p;
//}
//
//void KCharset_Normal::SetInvalid(int chr)
//{
//	m_invalidCharMap[chr] = true;
//}
//
//BOOL KCharset_Normal::IsValidChar(int chr) const
//{
//	int len = mblen_k((const char*)&chr, sizeof(chr));
//	if(len < 0) return FALSE;
//
//	CharMap::const_iterator it = m_invalidCharMap.find(chr);
//	if(it != m_invalidCharMap.end()) return FALSE;
//	return TRUE;
//}
//
//int KCharset_Normal::ReadOneChar(const void* p, int len, int& chr) const
//{
//	chr = 0;
//	const char* pc = (const char*)p;
//	int l = mblen_k(pc, len<2 ? len:2);
//	if(l < 0) return 0;
//	memcpy(&chr, pc, l);
//	return l;
//}