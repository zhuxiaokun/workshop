#ifndef _K_OBJECT_POOL_H_
#define _K_OBJECT_POOL_H_

#include "KAlloc.h"
#include "../Collections/KSDList.h"
#include "KMemoryCleaner.h"
#include "../Collections/DynArray.h"

namespace System { namespace Memory {

	template <typename T, int capacity=64, typename L=::System::Sync::KNoneLock>
	class KObjectPool : private _autoreset_object_pool<T,capacity,false,L>
	{
	public:
		typedef T obj_type;
		typedef _autoreset_object_pool<T,capacity,false,L> base_type;

	public:
		KObjectPool()
		{

		}
		~KObjectPool()
		{

		}
		obj_type* Get()
		{
			return base_type::Alloc();
		}
		void Release(obj_type* pObj)
		{
			base_type::Free(pObj);
		}
		void clear()
		{
			base_type::clear();
		}
	};

	template <typename T, int capacity=64, typename L=::System::Sync::KNoneLock>
	class KObjectPool2 : public KObjectPool<T,capacity,L>
	{
	//
	};

	template <typename T, int capacity=64, typename L=::System::Sync::KNoneLock>
	class KPortablePool : public _portable_object_pool<T,capacity,false,L>
	{
	//
	};

}}

#endif