//---------------------------------------------------------------------------
inline void KRefObject::IncRefCount()
{
	++m_uiRefCount;
}
//---------------------------------------------------------------------------
inline void KRefObject::DecRefCount()
{
	--m_uiRefCount;
	if (m_uiRefCount == 0)
	{	
		DeleteThis();
	}
}
//---------------------------------------------------------------------------
inline unsigned int KRefObject::GetRefCount() const
{
	return m_uiRefCount;
}