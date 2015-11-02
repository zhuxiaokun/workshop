#pragma once
#include "../KType.h"
#include "../KMacro.h"
#include "KMemory.h"

namespace System { namespace Memory {

	/**
	* ʹ��malloc��free�������ڴ�
	* ����û������
	*/
	class HeapAllocator
	{
	public:
		HeapAllocator()
		{
		}
		~HeapAllocator()
		{
		}
		void* alloc(int size)
		{
			return ::malloc_k(size);
		}
		BOOL free(void* p)
		{
			::free_k(p);
			return TRUE;
		}
		int unitSize() const
		{
			return 0;
		}
		void clear()
		{
			return;
		}
	};

// namespace end
}}
