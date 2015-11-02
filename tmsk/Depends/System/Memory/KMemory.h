#ifndef _K_MEMORY_H_
#define _K_MEMORY_H_

//#define _CHK_MEM_LEAK_

#ifdef __cplusplus
#include "../KType.h"
#include "../KMacro.h"
#else
//#include <io.h>
typedef int BOOL;
//typedef long size_t;
#endif

void enableRecordMemory(BOOL enable);

#ifdef _CHK_MEM_LEAK_

#ifdef __cplusplus
extern "C"
{
#endif

void* calloc_k(size_t nmemb, size_t size);
void* malloc_k(size_t size);
void  free_k(void *ptr);
void* realloc_k(void *ptr, size_t size);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

#if defined(WIN32) || defined(WINDOWS)

#include <new>

void* operator new (size_t size);
void operator delete (void* ptr);
//void *operator new[] (size_t size);
void operator delete[] (void* ptr);
//void* operator new(size_t, void *_Where);
_Ret_bytecap_(_Size) void *__CRTDECL operator new[](size_t _Size) _THROW1(std::bad_alloc);

#else

#include <new>

void* operator new(size_t) throw (std::bad_alloc);
void* operator new [] (size_t) throw (std::bad_alloc);
void operator delete(void*) throw ();
void operator delete [] (void*) throw ();
//void* operator new(size_t, void*) throw ();

#endif

#endif

#else

#if defined(WIN32) || defined(WINDOWS)
#include <new.h>
#include <malloc.h>
#else
//#include <new>
#include <memory.h>
#endif

#define calloc_k calloc
#define malloc_k malloc
#define free_k free
#define realloc_k realloc

#endif

#endif