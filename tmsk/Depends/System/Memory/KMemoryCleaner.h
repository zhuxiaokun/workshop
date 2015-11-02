#ifndef _K_MEMORY_CLEANER_H_
#define _K_MEMORY_CLEANER_H_

#include "../Collections/KVector.h"
#include "../Sync/KSync.h"

namespace System { namespace Memory {

class IObjectDestroyer
{
public:
	IObjectDestroyer(void* p):m_pMem(p) { }
	virtual ~IObjectDestroyer() { }
	virtual void _Destroy(void* p) = 0;
	void Destroy()
	{
		if(m_pMem)
		{
			this->_Destroy(m_pMem);
			m_pMem = NULL;
		}
	}
public:
	void* m_pMem;
};

class KCDestroyer : public IObjectDestroyer
{
public:
	KCDestroyer(void* p):IObjectDestroyer(p) { }
	void _Destroy(void* p) { free_k(p); }
};

template <typename T> class KCppDestroy : public IObjectDestroyer
{
public:
	KCppDestroy(void* p):IObjectDestroyer(p) { }
	void _Destroy(void* p) { delete (T*)p; }
};

template <typename T> class KCppArrDestroyer : public IObjectDestroyer
{
public:
	KCppArrDestroyer(void* p) : IObjectDestroyer(p) { }
	void _Destroy(void* p) { delete [] (T*)p; }
};

class KMemoryCleaner
{
private:
	KMemoryCleaner();

public:
	static KMemoryCleaner* GetInstance();
	static void Destroy();

public:
	~KMemoryCleaner();
	void AddDestroyer(IObjectDestroyer* p);
	void Clean();

private:
	::System::Sync::KThreadMutex m_x;
	::System::Collections::KVector<IObjectDestroyer*> m_vec;
	static KMemoryCleaner* m_pInstance;
};

}}

#endif