#pragma once
#include <System/Collections/KString.h>
#include <System/Collections/KTree.h>
#include <lua/KLuaWrap.h>

class KPropertyMap : public KBaseLuaClass<KPropertyMap>
{
public:
	typedef JG_C::KString<32> string_32;
	typedef JG_C::KString<128> string_128;
	class keycmp { public: int operator () (const string_32& a, const string_32& b) const; };
	typedef JG_C::KMapByTree_staticPooled<string_32,string_128,256,keycmp> KeyValueMap;
	KeyValueMap m_kvmap;

public:
	KPropertyMap();
	KPropertyMap(const KPropertyMap& o);
	~KPropertyMap();
	KPropertyMap& operator = (const KPropertyMap& o);

public:
	bool parseLine(char* propertyLine);

public:
	// key value separated by ':'
	bool parse(char* keyValue, char** keyp, char** valp);
	void set(char* keyValue);
	
	bool has(const char* key) const;
	bool empty() const;
	int size() const;

	const char* get(const char* key) const;
	void erase(const char* key);

	int serialize(char* buf, int len) const;
	void clear();

	bool foreachPair(LMREG::l_func func, LMREG::l_stackval ctx);
	LongLuaString toString() const;

private:
	void _set(const char* key, const char* value);

public:
	template <typename V> void set(const char* key, V v)
	{
		JG_C::KString<128> s;
		s.append(v);
		this->_set(key, s.c_str());
	}
	template <> void set(const char* key, char* v)
	{
		this->_set(key, v);
	}
	template <> void set(const char* key, const char* v)
	{
		this->_set(key, v);
	}
	template <> void set(const char* key, char*& v)
	{
		this->_set(key, v);
	}
	template <> void set(const char* key, const char*& v)
	{
		this->_set(key, v);
	}

public:
	BeginDefLuaClassNoneGc(KPropertyMap);
		DefMemberFunc(get);
		DefMemberFunc(foreachPair);
		DefMemberFunc(toString);
	EndDef;
};

__luaClassPtr2Lua(KPropertyMap);

