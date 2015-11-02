#ifndef __KSMARTPOINTER_H__
#define __KSMARTPOINTER_H__
namespace System { 
	template <class T>
	class KSmartPointer
	{
	public:
		inline KSmartPointer(T* pObject = (T*) 0);

		inline KSmartPointer(const KSmartPointer& ptr);

		inline ~KSmartPointer();

		inline operator T*() const;

		inline T* data() const;

		inline T& operator*() const;
		inline T* operator->() const;
		
		inline KSmartPointer& operator=(const KSmartPointer& ptr);
		inline KSmartPointer& operator=(T* pObject);
		
		inline bool operator==(T* pObject) const;
		inline bool operator!=(T* pObject) const;
		inline bool operator==(const KSmartPointer& ptr) const;
		inline bool operator!=(const KSmartPointer& ptr) const;
		inline bool operator<(T* pObject) const;
		inline bool operator>(T* pObject) const;
		inline bool operator<(const KSmartPointer& ptr) const;
		inline bool operator>(const KSmartPointer& ptr) const;
		inline void Swap(KSmartPointer& otherPtr);
	protected:
		// The managed smart pointer, a reference counting, self deleting pointer.
		T* m_pObject;
	};
	
	#include "KSmartPointer.inl"


}

#define KSmartPointer(classname) \
class classname; \
	typedef KSmartPointer<classname> classname##Ptr

#endif