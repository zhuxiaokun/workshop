#pragma once

#include <System/KType.h>
#include <System/KMacro.h>
#include <System/Collections/DynArray.h>

template <DWORD startID,DWORD idCount,int initFreeCapacity=idCount/32>
class KIdFactory
{
public:
	typedef JG_C::DynArray<DWORD,initFreeCapacity,initFreeCapacity> FreeArray;

public:
	KIdFactory():m_nextPos(0)
	{

	}
	~KIdFactory()
	{

	}

public:
	// ����һ��ID��
	DWORD alloc()
	{
		if(!m_freeArray.empty())
		{
			return m_freeArray.pop_back();
		}
		ASSERT(m_nextPos < idCount);
		return startID + m_nextPos++;
	}
	// ����һ��ID��
	void free(DWORD id)
	{
		m_freeArray.push_back(id);
	}
	// ����ID�ŷ�����
	void reset()
	{
		m_nextPos = 0;
		m_freeArray.clear();
	}

public:
	int m_nextPos;
	FreeArray m_freeArray;
};
