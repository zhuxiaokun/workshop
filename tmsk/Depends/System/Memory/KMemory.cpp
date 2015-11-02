#include "KMemoryMonitor.h"
#include "KMemory.h"
#include "../Log/log.h"
#include "../Collections/KString.h"
#include "multi_module.h"
using namespace ::System::Collections;

static BOOL g_recordMemory = FALSE;

#ifdef _CHK_MEM_LEAK_

namespace System { namespace Memory {
extern KMemoryMonitor* g_mmonitor;
}}

#if defined(__cplusplus)
extern "C"
{
#endif
//--------------------------------------------
_EXPORT_K_ void _setMemoryMonitor(::System::Memory::KMemoryMonitor* monitor) { ::System::Memory::g_mmonitor = monitor; }
_EXPORT_K_ void _setMemoryRecordFlag(BOOL flag) { g_recordMemory = flag; }
//--------------------------------------------
#if defined(__cplusplus)
}
#endif

//KString<512> traceback_str(const CallStack& callstack, int n)
int traceback_str(const CallStack& callstack, int n, char* buf, size_t length)
{
	size_t len = 0;
	for(int i=0; i<n && len+32<length; i++)
	{
		if(!callstack[i]) break;
		len += sprintf_k(buf+len, length-len, ".%08p", callstack[i]);
	}
	buf[len] = '\0';
	return (int)len;
}
#endif

void enableRecordMemory(BOOL enable)
{
#ifdef _CHK_MEM_LEAK_
	if(enable && !g_recordMemory)
	{
		if(!::System::Memory::g_mmonitor)
		{
			::System::Memory::KMemoryMonitor* monitor = new ::System::Memory::KMemoryMonitor();
			foreach_module("_setMemoryMonitor", _setMemoryMonitor, monitor);
		}
		::System::Memory::g_mmonitor->start();
	}

	BOOL b = g_recordMemory;
	foreach_module("_setMemoryRecordFlag", _setMemoryRecordFlag, enable);
	
	if(b && !enable)
	{
		if(::System::Memory::g_mmonitor)
			::System::Memory::g_mmonitor->stop();
	}
#endif
}

#ifdef _CHK_MEM_LEAK_

void* calloc_k(size_t nmemb, size_t size)
{
	size_t n = nmemb * size;
	return malloc_k(n);
}

void* malloc_k(size_t size)
{
	void* p = malloc(size);
	if(g_recordMemory)
	{
		CallStack callstack;
		int n = getCallStack(callstack, 16);
		//traceback_str(callstack, 16);
		::System::Memory::KMemoryRecorder* recorder = ::System::Memory::g_mmonitor->getThreadRecorder();
		if(recorder) recorder->push_alloc(callstack, p, size);
	}
	return p;
}

void free_k(void* ptr)
{
	free(ptr);
	if(g_recordMemory)
	{
		CallStack callstack;
		int n = getCallStack(callstack, 16);
		//traceback_str(callstack, 16);
		::System::Memory::KMemoryRecorder* recorder = ::System::Memory::g_mmonitor->getThreadRecorder();
		if(recorder) recorder->push_free(callstack, ptr);
	}
}

void* realloc_k(void* ptr, size_t size)
{
	void* p = realloc(ptr, size);
	if(g_recordMemory)
	{
		CallStack callstack;
		int n = getCallStack(callstack, 16);
		//traceback_str(callstack, 16);
		::System::Memory::KMemoryRecorder* recorder = ::System::Memory::g_mmonitor->getThreadRecorder();
		if(recorder) recorder->push_realloc(callstack, ptr, p, size);
	}
	return p;
}

#if defined(WIN32) || defined(WINDOWS)

void* operator new (size_t size)
{
	return malloc_k(size);
}

void operator delete (void* ptr)
{
	return free_k(ptr);
}

//void *operator new[] (size_t size)
_Ret_bytecap_(_Size) void *__CRTDECL operator new[](size_t _Size) _THROW1(std::bad_alloc)
{
	return malloc_k(_Size);
}

void operator delete[] (void* ptr)
{
	free_k(ptr);
}

#else

void* operator new (size_t size) throw (std::bad_alloc)
{
	return malloc_k(size);
}

void operator delete (void* ptr) throw ()
{
	return free_k(ptr);
}

void *operator new[] (size_t size) throw (std::bad_alloc)
{
	return malloc_k(size);
}

void operator delete[] (void* ptr) throw ()
{
	free_k(ptr);
}

// void* operator new(size_t, void *_Where) throw ()
// {
// 	return (_Where);
// }

#endif

#endif