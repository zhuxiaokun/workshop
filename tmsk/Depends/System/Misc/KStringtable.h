#pragma once
#include "../KPlatform.h"
#include "../Collections/KSDList.h"
#include "../Collections/KHashTable.h"
#include "../Memory/RingBuffer.h"
#include "StreamInterface.h"
#include "../File/KTabfileLoader.h"

template <size_t bucket=1024> class _KStringPool
{
public:
	struct Buffer : public KPieceBuffer<10240> { Buffer* m_pNextNode; };
	typedef ::System::Collections::KSDSelfList<Buffer> BufferList;
	typedef ::System::Collections::KHashTable< ccstr,bool,KccstrCmp,KStringHasher<ccstr>,JG_S::KNoneLock,bucket,bucket*256> Map;

public:
	Map m_map;
	Buffer* m_buffer;
	BufferList m_bufferList;

public:
	_KStringPool() : m_buffer(NULL)
	{

	}
	~_KStringPool()
	{

	}

public:
	const char* get(const char* str)
	{
		Map::iterator it = m_map.find(str);
		if(it != m_map.end()) return it->first;
		const char* s = this->allocString(str, strlen(str));
		m_map[s] = true;
		return s;
	}
	void destroy()
	{
		while(!m_bufferList.empty())
		{
			Buffer* p = m_bufferList.pop_front();
			delete p;
		}
		m_buffer = NULL;
		m_map.clear();
	}

private:
	const char* allocString(const char* str, size_t len)
	{
		if(!m_buffer || m_buffer->WriteAvail() < (int_r)len+1)
		{
			m_buffer = new Buffer();
			m_buffer->Reset();
			m_bufferList.push_back(m_buffer);
		}
		const char* p = m_buffer->GetWriteBuffer();
		m_buffer->Write(str, len+1);
		return p;
	}
};

typedef _KStringPool<1024> KStringPool;

class KDiffFileReader;
class KStringtable
{
public:
	typedef ::System::Collections::KString<2048> _KString;
	struct Buffer : public KPieceBuffer<10240> { Buffer* m_pNextNode; };
	typedef ::System::Collections::KSDSelfList<Buffer> BufferList;
	typedef ::System::Collections::KHashTable<int,ccstr,KIntCompare<int>,KHasher<int>,JG_S::KNoneLock,2048,8192> Map;

public:
	class IVisitor
	{
	public:
		virtual ~IVisitor() { }
		virtual bool visit(int id, const char* str) = 0;
	};

public:
	Map m_map;
	Buffer* m_buffer;
	BufferList m_bufferList;
	
private:
	KStringtable(const KStringtable&);
	KStringtable& operator = (const KStringtable&);

public:
	KStringtable();
	~KStringtable();

public:
	bool load(StreamInterface& si);
	void reset();

public:
	const char* getString(int id) const;
	const char* getString2(int id, const char* defVal) const;
	bool foreach(IVisitor& v) const;

public:
	bool patch(KDiffFileReader& diff);
	bool setString(int id, const char* val);

protected:
	const char* allocString(const char* str, size_t len);
	/////////////////////////////////////////////////////
	// 去转义，认为和源码一个级别
	// 对StringManager表中读取出来的字符串进行转换
	// \r	->	'\r'
	// \n	->	'\n'
	// \t	->	'\t'
	// \b	->	'\b'
	// #\n, #\t 不做转换，留给UI处理
	const char* translateSourceString(const char* s, _KString& str);
};

/// KStringtableManager

class KStringtableManager
{
public:
	KStringtableManager();
	~KStringtableManager();

public:
	const KStringtable* getByLang(int lang) const;

public:
	KStringtable* _getByLang(int lang);
	KStringtable* _getByLangName(const char* langName);

public:
	KStringtable* m_tables[KLanguage::lang_count];
};