//#ifndef _K_CHARSET_NORMAL_H_
//#define _K_CHARSET_NORMAL_H_
//
//#include "KCharset_GB2312.h"
//#include "../Collections/KTree.h"
//
//class KCharset_Normal : public KCharset
//{
//public:
//	typedef ::System::Collections::KMapByTree_staticPooled<int,bool,1024> CharMap;
//
//public:
//	KCharset_Normal();
//	~KCharset_Normal();
//
//public:
//	static KCharset_Normal* getInstance();
//
//public:
//	void SetInvalid(int chr);
//	BOOL IsValidChar(int chr) const;
//	int ReadOneChar(const void* p, int len, int& chr) const;
//
//public:
//	CharMap m_invalidCharMap;
//};
//
//#endif