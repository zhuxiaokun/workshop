#ifndef _K_OBJECT_FACTORY_H_
#define _K_OBJECT_FACTORY_H_

#include <System/Collections/KMapByVector.h>
#include <System/Sync/KSync.h>
#include <System/Collections/KString.h>

#define DECLARE_DYNAMIC_CLASS(className) \
	public: \
	static void* _CreateInstance() \
	{ \
		return new className(); \
	} \
	void _destroyThis() \
	{ \
		delete this; \
	}

#define DECLARE_POOL_DYNAMIC_CLASS(className) \
	public: \
	static void* _CreateInstance() \
	{ \
		return className::Alloc(); \
	} \
	void _destroyThis() \
	{ \
		className::Free(this); \
	}

#define DECLARE_POOL_DYNAMIC_CLASS_2(className,poolStep) \
	public: \
	static System::Memory::KPortableStepPool<className,poolStep>& _getClassPool() \
	{ \
		static System::Memory::KPortableStepPool<className,poolStep> pool; \
		return pool; \
	} \
	static void* _CreateInstance() \
	{ \
		return className::_getClassPool().Alloc(); \
	} \
	void _destroyThis() \
	{ \
		className::_getClassPool().Free(this); \
	}

#define REGISTER_DYNAMIC_CLASS(className) \
	BOOL _##className##_register_rst = \
	KObjectFactory::GetInstance().Register(#className, &className::_CreateInstance);

#define DESTROY_DYNAMIC_INSTANCE(p) \
	p->_destroyThis();

class KObjectFactory
{
public:
	typedef void* (*Creator)();
	typedef JG_C::KString<64> ClassName;
	typedef JG_C::KMapByVector<ClassName, Creator> ClassMap;
	typedef ClassMap::iterator Iterator;

private:
	KObjectFactory();

public:
	~KObjectFactory();
	static KObjectFactory& GetInstance();
	BOOL Register(const char* className, Creator creator);

public:
	void* Create(const char* className);

private:
	JG_S::KThreadMutex m_mx;
	ClassMap m_classMap;
};

#endif