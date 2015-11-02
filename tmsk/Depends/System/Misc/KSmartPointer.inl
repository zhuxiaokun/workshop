template <class T>
inline KSmartPointer<T>::KSmartPointer(T* pObject)
: m_pObject(pObject)
{
	if (m_pObject)
		m_pObject->IncRefCount();
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline KSmartPointer<T>::KSmartPointer(const KSmartPointer& ptr)
: m_pObject(ptr.m_pObject)
{
	if (m_pObject)
		m_pObject->IncRefCount();
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline KSmartPointer<T>::~KSmartPointer()
{
	if (m_pObject)
		m_pObject->DecRefCount();
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline KSmartPointer<T>::operator T*() const
{
	return m_pObject;
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline T* KSmartPointer<T>::data() const
{
	return m_pObject;
}


//-------------------------------------------------------------------------------------------------
template <class T>
inline T& KSmartPointer<T>::operator*() const
{
	return *m_pObject;
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline T* KSmartPointer<T>::operator->() const
{
	return m_pObject;
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline KSmartPointer<T>& KSmartPointer<T>::operator=(const KSmartPointer& ptr)
{
	if (m_pObject != ptr.m_pObject)
	{
		if (m_pObject)
			m_pObject->DecRefCount();
		m_pObject = ptr.m_pObject;
		if (m_pObject)
			m_pObject->IncRefCount();
	}
	return *this;
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline KSmartPointer<T>& KSmartPointer<T>::operator=(T* pObject)
{
	if (m_pObject != pObject)
	{
		if (m_pObject)
			m_pObject->DecRefCount();
		m_pObject = pObject;
		if (m_pObject)
			m_pObject->IncRefCount();
	}
	return *this;
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline bool KSmartPointer<T>::operator==(T* pObject) const
{
	return (m_pObject == pObject);
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline bool KSmartPointer<T>::operator!=(T* pObject) const
{
	return (m_pObject != pObject);
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline bool KSmartPointer<T>::operator==(const KSmartPointer& ptr) const
{
	return (m_pObject == ptr.m_pObject);
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline bool KSmartPointer<T>::operator!=(const KSmartPointer& ptr) const
{
	return (m_pObject != ptr.m_pObject);
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline bool KSmartPointer<T>::operator<(T* pObject) const
{
	return (m_pObject < pObject);
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline bool KSmartPointer<T>::operator>(T* pObject) const
{
	return (m_pObject > pObject);
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline bool KSmartPointer<T>::operator<(const KSmartPointer& ptr) const
{
	return (m_pObject < ptr.m_pObject);
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline bool KSmartPointer<T>::operator>(const KSmartPointer& ptr) const
{
	return (m_pObject > ptr.m_pObject);
}
//-------------------------------------------------------------------------------------------------
template <class T>
inline void KSmartPointer<T>::Swap(KSmartPointer& otherPtr)
{
	T* pTemporary = m_pObject;
	m_pObject = otherPtr.m_pObject;
	otherPtr.m_pObject = pTemporary;
}