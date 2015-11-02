/********************************************************************
	created:	2007/11/15
	created:	15:11:2007   16:41
	filename: 	System\System\KSingleton.h
	file path:	System\System
	file base:	KSingleton
	file ext:	h
	author:		xueyan
	
	purpose:	实现一个Singleton模式的模版类，该类采用Double-Checked Locking
				模式改进，在多线程情况仍然是安全的。
				建议的用法是：
				1、希望惰性创建，并且对调用接口Check并不太在意开销的情况下，直接并一直调用Instance()函数。
				2、在一开始就要创建对象时，先在初始化时调用一次Instance()函数;在以后就一直调用GetInstancePtr()函数。
*********************************************************************/

#pragma once

#include "../KMacro.h"
#include "../KType.h"

#include "../Sync/KSync.h"

namespace System {			namespace Patterns {

template< class Type >
class KSingleton
{
public:
	//惰性创建，支持Double-Checked　Locking
	static Type*			Instance();
	static void				DestroyInstance();

	//不做任何检验，直接返回实例指针
	static Type*			GetInstancePtr();

	static void				SetInstancePtr(Type* p);


	Type*					MakeInstance();

protected:
	KSingleton();
	~KSingleton();
	
private:
	//static volatile Type*			m_pInstance;
	static Type*					m_pInstance;
	static ::System::Sync::KSync_CS	m_CS;

}; // end KSingleton


template< class Type > 
Type*
KSingleton<Type>::m_pInstance = NULL;

template< class Type > 
::System::Sync::KSync_CS
KSingleton<Type>::m_CS;


template< class Type > 
KSingleton<Type>::KSingleton()
{
	//MakeInstance();
	m_pInstance = NULL;
}

template< class Type > 
KSingleton<Type>::~KSingleton()
{
	m_pInstance = NULL;
}

template< class Type >
inline Type*
KSingleton<Type>::Instance()
{

	if ( NULL == m_pInstance )
	{
		//m_CS.Lock();
		if ( NULL == m_pInstance )
		{
			//m_pInstance = static_cast<Type*>(new KSingleton<Type>);
			m_pInstance = static_cast<Type*>(new Type());
			//m_pInstance = static_cast<Type*>(this);
		}
		//m_CS.Unlock();
		
	}
	return m_pInstance;
}

template< class Type >
inline void
KSingleton<Type>::DestroyInstance()
{
	if ( NULL != m_pInstance )
	{
		//m_CS.Lock();
		SAFE_DELETE(m_pInstance);
		//m_CS.Unlock();

	}
	
}

template< class Type >
inline Type*
KSingleton<Type>::MakeInstance()
{

	if ( NULL == m_pInstance )
	{
		//m_CS.Lock();
		if ( NULL == m_pInstance )
		{
			//m_pInstance = static_cast<Type*>(new KSingleton<Type>);
			m_pInstance = static_cast<Type*>(this);
		}
		//m_CS.Unlock();

	}
	return m_pInstance;
}

template< class Type >
inline Type*
KSingleton<Type>::GetInstancePtr()
{
	// ASSERT( NULL != m_pInstance);

	return m_pInstance;
}


template< class Type>
inline void
KSingleton<Type>::SetInstancePtr(Type* p)
{
	ASSERT( NULL != p);
	ASSERT( m_pInstance == NULL );
	m_pInstance = p;
}


} /* end Patterns */		} /* System */