#ifndef _K_AUTO_POOL_OBJPTR_H_
#define _K_AUTO_POOL_OBJPTR_H_

#include "./KObjectPool.h"
#include "./KStepObjectPool.h"

/**
 * 类型T必须支持两个静态方法
 * T* p = T::Alloc()
 * T::Free(p)
 * 类实例一般作为局部变量使用
 * 使用这个对象可以避免显示的调用Alloc和Free带来的不便
 */

template <typename T> class KAutoPoolObjPtr
{
public:
	typedef T obj_type;

public:
	obj_type* m_ptr;

public:
	KAutoPoolObjPtr():m_ptr(obj_type::Alloc())
	{
		ASSERT(m_ptr);
	}
	~KAutoPoolObjPtr()
	{
		obj_type::Free(m_ptr);
	}

public:
	obj_type* operator -> ()
	{
		return m_ptr;
	}

	operator obj_type* ()
	{
		return m_ptr;
	}

	obj_type& operator * ()
	{
		return *m_ptr;
	}
};

#endif