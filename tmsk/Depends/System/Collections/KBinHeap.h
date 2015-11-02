///********************************************************************
//	created:	2011/03/18
//	filename: 	System\Collections\KBinHeap.h
//	file path:	System\Collections
//	file base:	KBinHeap
//	file ext:	h
//	author:		panfeng
//	
//	purpose:	二叉堆，特性：父结点的键值总是大于或小于任何一个子节点的键值。
//*********************************************************************/
#ifndef SystemCollectionsBinHeap
#define SystemCollectionsBinHeap

#include "DynArray.h"

namespace System {			namespace Collections {

template< class T, typename C=KLessCompare<T,KLess<T> >, typename E=KLessCompare<T,KLess<T> >, int initCapacity=32, int inflateStep=16 >
class KBinHeap
{
public:
	enum enum_Index{eI_Invalid = -1};
	KBinHeap(){};
	~KBinHeap(){};

private:
	typedef C cmp_type;
	typedef E equal_type;

	cmp_type m_cmp;
	equal_type m_equal;

	T m_swapValue;
	DynArray<T, initCapacity, inflateStep> m_vals;

	inline int parent_index(int nIndex)
	{
		return (nIndex+1)/2 - 1;
	}

	inline int lchild_index(int nIndex)
	{
		int nResult = 2*(nIndex+1)-1;
		if (nResult < m_vals.size())
			return nResult;
		else return eI_Invalid;
	}

	inline int rchild_index(int nIndex)
	{
		int nResult = 2*(nIndex+1);
		if (nResult < m_vals.size())
			return nResult;
		else return eI_Invalid;
	}

	inline void swap(int nIndex1, int nIndex2)
	{
		m_swapValue = m_vals[nIndex1];
		m_vals[nIndex1] = m_vals[nIndex2];
		m_vals[nIndex2] = m_swapValue;
	}

	inline int min_child_index(int nIndex)
	{
		int nResult = lchild_index(nIndex);
		if (eI_Invalid == nResult)
			return nResult;
		else
		{
			int nRightIndex = rchild_index(nIndex);
			if (eI_Invalid == nRightIndex)
				return nResult;
			else if (1 == m_cmp(m_vals[nResult], m_vals[nRightIndex]))
				return nResult;
			else
				return nRightIndex;

		}
	}

	void push_down(int nIndex)
	{
		int nMinChildIndex = eI_Invalid;
		if (eI_Invalid != (nMinChildIndex = min_child_index(nIndex)))
		{
			if (-1 == m_cmp(m_vals[nIndex], m_vals[nMinChildIndex]))
			{
				swap(nIndex, nMinChildIndex);
				push_down(nMinChildIndex);
			}
		}
		
	}

	void push_up(int nIndex)
	{
		int nParentIndex = eI_Invalid;
		if (eI_Invalid != (nParentIndex = parent_index(nIndex)))
		{
			if (1 == m_cmp(m_vals[nIndex], m_vals[nParentIndex]))
			{
				swap(nIndex, nParentIndex);
				push_up(nParentIndex);
			}
		}

	}

public:

	void insert(T val)
	{
		m_vals.push_back(val);
		push_up(m_vals.size()-1);
	}

	BOOL erase_min()
	{
		if (0 == m_vals.size())
			return FALSE;

		int nSize = m_vals.size();
		m_vals[0] = m_vals[nSize - 1];
		m_vals.erase(nSize-1);
		push_down(0);

		return TRUE;
	}

	T* get_min()
	{
		if (0 == m_vals.size())
			return NULL;
		else
			return &(m_vals[0]);
	}

	BOOL erase(T val)
	{
		int nSize = m_vals.size();
		int nPos = 0;
		for (nPos=0; nPos<nSize; nPos++)
		{
			if (0 == m_equal(m_vals[nPos], val))
			{
				break;
			}
		}
		if (nPos == nSize) return FALSE;
		
		m_vals[nPos] = m_vals[nSize-1];
		m_vals.erase(nSize-1);
		push_up(nPos);
		push_down(nPos);
		return TRUE;
	}

	void clear()
	{
		m_vals.clear();
	}

};

} /* Collections */			} /* System */


#endif