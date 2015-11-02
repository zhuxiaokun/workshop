/********************************************************************
	created:	2007/11/15
	created:	15:11:2007   16:09
	filename: 	System\System\Memory\New.h
	file path:	System\System\Memory
	file base:	New
	file ext:	h
	author:		xueyan
	
	purpose:	这里主要是先对new和delete和内存相关的操作符进行
				一个再定义，以方便日后处理。
*********************************************************************/

#pragma once

#ifndef MapZipEdit


	#ifndef WINDOWS
		#include <new>
	#endif

	//#include <malloc.h>

// 	void * 
// 	Talloc( unsigned int uiItemSize );
// 	void
// 	Tfree( void * pPointer );

#ifdef _CHK_MEM_LEAK_
// 	void *
// 	operator new( unsigned int uiItemSize );
// 	void
// 	operator delete( void * pPointer );
// 
// 	void *
// 	operator new[]( unsigned int uiItemSize );
// 	void
// 	operator delete[]( void * pPointer );
// 
// 
// 	inline void *
// 	operator new( unsigned int uiItemSize )
// 	{
// 
// 		//return new( uiItemSize );
// 		return ::Talloc( uiItemSize );
// 	}
// 
// 	inline void
// 	operator delete( void * pPointer )
// 	{
// 		if ( 0 == pPointer )
// 		{
// 			return;
// 		}
// 
// 		::Tfree( pPointer );
// 	}
// 
// 	inline void *
// 	operator new[]( unsigned int uiItemSize )
// 	{
// 
// 		return ::Talloc( uiItemSize );
// 	}
// 
// 	inline void
// 	operator delete[]( void * pPointer )
// 	{
// 		if ( 0 == pPointer)
// 		{
// 			return;
// 		}
// 
// 		::Tfree( pPointer );
// 	}
#endif


// 	inline void *
// 	Talloc( unsigned int uiItemSize )
// 	{
// 
// 		/*nAllocMemAllCount += uiItemSize;
// 		printf("used memory size:%d\n", nAllocMemAllCount);*/
// 
// 		return ::malloc_k( uiItemSize );
// 	}
// 
// 	inline void
// 	Tfree( void * pPointer )
// 	{
// 		::free_k( pPointer );
// 	}

#endif // MapZipEdit