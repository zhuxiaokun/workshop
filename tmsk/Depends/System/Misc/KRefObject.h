#ifndef __KREFOBJECT_H__
#define __KREFOBJECT_H__
#include "../KType.h"
#include "../KMacro.h"

namespace System { 

	class KRefObject
	{
	public:
		KRefObject():m_uiRefCount(0) { }
		virtual ~KRefObject() { }
		inline void IncRefCount();
		inline void DecRefCount();
		inline unsigned int GetRefCount() const;
	protected:
		virtual void DeleteThis();
		UINT32 m_uiRefCount;
	};
	#include "KRefObject.inl"
}

class KRef : private ::System::KRefObject
{
public:
	KRef() { }
	virtual ~KRef() { }

public:
	int addRef() { return ++m_uiRefCount; }
	int releaseRef() { return --m_uiRefCount; }
	int refCount() const { return m_uiRefCount; }
	void reset() { m_uiRefCount = 0; }
	virtual void DeleteThis() { delete this; }
	virtual void destroy(bool ignore) { if(!ignore) delete this; }
};

template <typename T> class KRefPtr
{
public:
	typedef T* ref_ptr;
	ref_ptr m_ptr;
	bool m_ignoreDestroy;

public:
	KRefPtr():m_ptr(NULL),m_ignoreDestroy(false)
	{
	}
	KRefPtr(ref_ptr ptr):m_ptr(ptr),m_ignoreDestroy(false)
	{
		if(m_ptr) m_ptr->addRef();
	}
	KRefPtr(ref_ptr ptr, bool ignoreDestroy):m_ptr(ptr),m_ignoreDestroy(ignoreDestroy)
	{
		if(m_ptr) m_ptr->addRef();
	}
	KRefPtr(const KRefPtr& o):m_ptr(o.m_ptr)
	{
		if(m_ptr) m_ptr->addRef();
	}
	~KRefPtr()
	{
		if(m_ptr)
		{
			if(m_ptr->releaseRef() < 1)
				m_ptr->destroy(m_ignoreDestroy);
		}
	}
	KRefPtr& operator = (ref_ptr ptr)
	{
		if(m_ptr == ptr) return *this;
		if(m_ptr)
		{
			if(m_ptr->releaseRef() < 1)
				m_ptr->destroy(m_ignoreDestroy);
			m_ptr = NULL;
		}
		if(ptr)
		{
			ptr->addRef();
			m_ptr = ptr;
		}
		return *this;
	}
	KRefPtr& operator = (const KRefPtr& o)
	{
		if(this == &o) return *this;
		if(m_ptr)
		{
			if(m_ptr->releaseRef() < 1)
				m_ptr->destroy(m_ignoreDestroy);
			m_ptr = NULL;
		}
		if(o.m_ptr)
		{
			m_ptr = o.m_ptr;
			m_ptr->addRef();
		}
		return *this;
	}
	operator ref_ptr ()
	{
		return m_ptr;
	}
	operator const ref_ptr () const
	{
		return m_ptr;
	}
	ref_ptr operator -> ()
	{
		return m_ptr;
	}
	const ref_ptr operator -> () const
	{
		return m_ptr;
	}
	T& operator * ()
	{
		ASSERT(m_ptr);
		return *m_ptr;
	}
	const T& operator * () const
	{
		ASSERT(m_ptr);
		return *m_ptr;
	}
	bool operator == (const KRefPtr& o) const
	{
		return m_ptr == o.m_ptr;
	}
	bool operator == (ref_ptr ptr) const
	{
		return m_ptr == ptr;
	}
	operator bool () const
	{
		return m_ptr != NULL;
	}
	bool operator ! () const
	{
		return !m_ptr;
	}
};

template <typename T> bool operator < (const KRefPtr<T>& a, const KRefPtr<T>& b)
{
	if(a)
	{
		if(b)
		{
			if(a == b) return false;
			return *a < *b;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if(b) return true;
		else return false;
	}
}

template <typename T> struct _is_simple_type_< KRefPtr<T> >
{
	static const bool value = true;
};

#endif
