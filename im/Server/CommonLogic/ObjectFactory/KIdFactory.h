#pragma once

#include <System/KType.h>
#include <System/KMacro.h>
#include <System/Collections/DynArray.h>

template <KObjectID startID,INT64 idCount,int initFreeCapacity=(int)(idCount/32)>
class KIdFactory
{
public:
	typedef JG_C::DynArray<KObjectID,initFreeCapacity,initFreeCapacity> FreeArray;

public:
	KIdFactory():m_nextPos(0)
	{

	}
	~KIdFactory()
	{

	}

public:
	// 分配一个ID号
	KObjectID alloc()
	{
		if(!m_freeArray.empty())
		{
			return m_freeArray.pop_back();
		}
		ASSERT(m_nextPos < idCount);
		return startID + m_nextPos++;
	}
	// 还回一个ID号
	void free(KObjectID id)
	{
		ASSERT(FALSE);
		m_freeArray.push_back(id);
	}
	// 重置ID号发生器
	void reset()
	{
		m_nextPos = 0;
		m_freeArray.clear();
	}

public:
	INT64 m_nextPos;
	FreeArray m_freeArray;
};

template <typename ID_T, ID_T startID, int_r idCount, int_r initFreeCapacity = idCount / 32>
class KIdFactory2
{
public:
	typedef JG_C::DynArray<ID_T, initFreeCapacity, initFreeCapacity> FreeArray;
	KIdFactory2():m_nextPos(0){}
	virtual ~KIdFactory2(){}

public:
	virtual ID_T alloc()
	{
		if(!m_freeArray.empty())
		{
			return m_freeArray.pop_back();
		}
		ASSERT(m_nextPos < idCount);
		return startID + m_nextPos++;
	}
	virtual void free(ID_T id)
	{
		m_freeArray.push_back(id);
	}
	virtual void reset()
	{
		m_nextPos = 0;
		m_freeArray.clear();
	}

public:
	int_r m_nextPos;
	FreeArray m_freeArray;
};

template <typename ID_T, ID_T startID, int_r idCount, int_r initFreeCapacity = idCount / 32>
class KIdFactory3 : public KIdFactory2<ID_T,startID,idCount,initFreeCapacity>
{
public:
	typedef KIdFactory2<ID_T,startID,idCount,initFreeCapacity> SUPER;
	KIdFactory3():SUPER(){}
	virtual ~KIdFactory3(){}

public:
	virtual ID_T alloc()
	{
		if(!SUPER::m_freeArray.empty())
		{
			return SUPER::m_freeArray.pop_back();
		}
		ASSERT(SUPER::m_nextPos > -idCount);
		return startID + SUPER::m_nextPos--;
	}
};