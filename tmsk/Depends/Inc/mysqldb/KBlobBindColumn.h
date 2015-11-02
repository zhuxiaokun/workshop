#ifndef _K_BlobBindColumn_OBJECT_H_
#define _K_BlobBindColumn_OBJECT_H_

#include <System/KType.h>
#include "mysqldb/KCacheObject.h"

//////////////////////////////////////////////////////////////////////////
//	KBlobBindColumn<KGuildEmployee,KGuildEmployeeLess,Guild_EMPLOYEE_MAX> m_BlobEmployee;
//	KBlobBindColumn<KGuildBBS,KGuildBBSLess,Guild_BBS_MAX> m_BlobBBS;
//	m_BlobEmployee.BindColumn( KDBGuildRec::f_Employee,&m_cacheObj );
//	m_BlobBBS.BindColumn( KDBGuildRec::f_BBS,&m_cacheObj );
//
class KCacheUpdate
{
	KCacheObject*  m_pCacheObj;
	WORD			m_ncolIdx;
public:
	KCacheUpdate(KCacheObject* pCacheObj,int colIdx):m_pCacheObj(pCacheObj),m_ncolIdx(colIdx)
	{
		m_pCacheObj->BeginUpdate(m_ncolIdx);
	}
	~KCacheUpdate()
	{
		m_pCacheObj->EndUpdate(m_ncolIdx);
	}
};


template <typename T, int capacity=1, typename C=KLess<T> >
class KBlobBindColumn
{
	KDBBinary<sizeof(T)*capacity>* m_pBinary;
	DynSortedArray<T, C> m_Array;
public:
	KCacheObject*  m_pCacheObj;
	WORD			m_ncolIdx;
public:
	KBlobBindColumn():m_pCacheObj(NULL)
	{
	}

	int size() const
	{
		return m_Array.size();
	}

	T& at(int idx)
	{
		return m_Array.at(idx);
	}
	const T& at(int idx) const
	{
		return m_Array.at(idx);
	}
	T& operator [] (int idx)
	{
		return m_Array.at(idx);
	}
	const T& operator [] (int idx) const
	{
		return m_Array.at(idx);
	}
	int find(const T& val) const
	{
		return m_Array.find( val );
	}

	BOOL BindColumn(int colIdx, KCacheObject* pCacheObj)
	{
		m_ncolIdx = colIdx;
		m_pCacheObj = pCacheObj;

		ASSERT_RETURN(m_pCacheObj->BindColumnTo(colIdx, m_pBinary),FALSE);
		m_Array.attach((T*)&m_pBinary->binData, capacity, m_pBinary->actualLength / sizeof(T));

		return TRUE;
	}

	int Add( const T &v )
	{
		if(m_Array.size() >= capacity) return FALSE;
		KCacheUpdate u( this->m_pCacheObj,this->m_ncolIdx );

		int nPos = m_Array.insert_unique(v);
		m_pBinary->actualLength = sizeof(T) * m_Array.size();

		return nPos;
	}

	int erase( int idx )
	{
		if(m_Array.size() <= idx) return FALSE;
		KCacheUpdate u( this->m_pCacheObj,this->m_ncolIdx );

		m_Array.erase(idx);
		m_pBinary->actualLength = sizeof(T) * m_Array.size();
	
		return m_Array.size();
	}
	void clear()
	{
		m_Array.clear();

		KCacheUpdate u( this->m_pCacheObj,this->m_ncolIdx );
		m_pBinary->actualLength = 0;
	}
};


#endif



