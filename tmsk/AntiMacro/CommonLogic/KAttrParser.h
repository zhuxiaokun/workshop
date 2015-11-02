#ifndef _K_ATTR_PARSER_H_
#define _K_ATTR_PARSER_H_

#include <System/KPlatform.h>
#include <System/Misc/StrUtil.h>
#include <System/Collections/DynArray.h>
#include <System/Memory/KBuffer64k.h>

struct KAttrPair
{
	const char* name;
	const char* val;
};
DECLARE_SIMPLE_TYPE(KAttrPair);

// name=xxx;a=1;b=2;c=1,2,3
class KAttrParser
{
public:
	class AttrComapre
	{
	public:
		int operator()(const KAttrPair& a, const KAttrPair& b) const
		{
			return stricmp(a.name, b.name);
		}
	};

	typedef JG_C::KDynSortedArray<KAttrPair,AttrComapre,128> AttrArray;

public:
	KAttrParser();
	~KAttrParser();

public:
	bool Parse(const char* attrs, char delimeter=';');
	bool GetParameter(int idx, char*& name, char*& val);

public:
	bool HasAttr(const char* name) const;
	
	const char* Get(const char* name) const;
	const KAttrPair& Get(int idx) const;

	int GetInteger(const char* name, int defVal);
	const char* GetString(const char* name, const char* defVal);

	bool Set(const char* name, const char* val);
	bool Remove(const char* name);
	void Clear();
	int Size() const;

	size_t ToString(char* buf, size_t len) const;

private:
	KBuffer64k* getBuffer();
	KAttrPair* get(const char* name);

public:
	KBuffer64k* m_buf;
	AttrArray m_attrs;
};

#endif