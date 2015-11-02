#pragma once
#include <redis/hiredis.h>
#include <System/Collections/DynArray.h>
#include <System/Collections/KList.h>
#include <System/Collections/KHashTable.h>
#include <System/Collections/KTree.h>
#include <System/Memory/RingBuffer.h>
#include <lua/KLuaWrap.h>

typedef LMREG::l_func lua_func;
typedef LMREG::l_func l_func;
typedef LMREG::l_stackval l_stackval;

struct KNameBuffer
	: public JG_C::SelfListNode<KNameBuffer>
	, public KPieceBuffer<1024*5>
	, JG_M::KPortableMemoryPool<KNameBuffer,1024>
{
	~KNameBuffer()
	{
		this->reset();
	}
	void reset()
	{
		JG_C::SelfListNode<KNameBuffer>::reset();
		KPieceBuffer<1024*5>::Reset();
	}
};

class KNameArray
	: public JG_C::SelfListNode<KNameArray>
	, public JG_M::KPortableStepPool<KNameArray,1024>
{
public:
	enum { CAPACITY = 1024};
	typedef JG_C::SelfListNode<KNameArray> list_node;
	typedef JG_C::KSelfList<KNameBuffer> buffer_list;

public:
	int m_nextPos;
	KNameBuffer* m_current;
	buffer_list m_bufferList;
	const char* m_buffer[CAPACITY];

public:
	KNameArray();
	~KNameArray();

public:
	bool avail() const;
	int size() const;
	const char* append(const char* name);
	const char* at(int idx) const;
	void reset();

private:
	const char* _alloc(const char* str, size_t len);
};

class KLargeNameList
{
public:
	typedef JG_C::KSelfList<KNameArray> large_name_list;
	class iterator
	{
	public:
		int m_index;
		KNameArray* m_arr;

	public:
		iterator();
		iterator(const iterator& o);
		iterator(KNameArray* p, int i):m_index(i),m_arr(p) { }
		~iterator();
		iterator& operator = (const iterator& o);
		iterator& operator ++ ();
		iterator& operator ++ (int);
		bool operator == (const iterator& o) const;
		bool operator != (const iterator& o) const;
		const char* operator * () const;
	};

public:
	large_name_list m_list;

public:
	KLargeNameList();
	~KLargeNameList();

public:
	int size() const;
	const char* append(const char* name);
	void reset();
	bool foreach(lua_func func, l_stackval ctx);

public:
	iterator begin() { return iterator(m_list.begin(), 0); }
	iterator end()   { return iterator(NULL, 0); }

public:
	BeginDefLuaClassNoneGc(KLargeNameList);
	DefMemberFunc(size);
	DefMemberFunc(append);
	DefMemberFunc(reset);
	DefMemberFunc(foreach);
	EndDef;
};
