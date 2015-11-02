#ifndef _KNAME_H_
#define _KNAME_H_

#include "../Collections/DynArray.h"
#include "../Sync/KSync.h"

// Maximum size of name.
#define KNAME_SIZE 128
#define KNAME_None -1
#define KNAME_ARRAY_COUNT( array ) ( sizeof(array) / sizeof((array)[0]) )
#define KNAME_CHECK ASSERT

#ifndef KNAME_HASH_TABLE_SIZE
#define KNAME_HASH_TABLE_SIZE 4096
#endif

#ifndef KNAME_ENTRY_INIT_SIZE 
#define KNAME_ENTRY_INIT_SIZE 512
#endif

#ifndef KNAME_ENTRY_INFLATE_STEP
#define KNAME_ENTRY_INFLATE_STEP 128
#endif

// Name index.
typedef INT NAME_INDEX;

// Enumeration for finding name.
enum EFindName
{
	KNAME_Find,			// Find a name; return 0 if it doesn't exist.
	KNAME_Add,			// Find a name or add it if it doesn't exist.
};

/*----------------------------------------------------------------------------
	KNameEntry.
----------------------------------------------------------------------------*/

//
// A global name, as stored in the global name table.
//
struct KNameEntry
{
	// Variables.
	NAME_INDEX	Index;				// Index of name in hash.
	KNameEntry*	HashNext;			// Pointer to the next entry in this hash bin's linked list.

	// The name string.
	char		Name[KNAME_SIZE];	// Name, variable-sized - note that AllocateNameEntry only allocates memory as needed.
};

//////////////////////////////////////////////////////////////////////////
// The structure storing KNameEntry
class KNameEntryStack
{
public:
	::System::Collections::DynArray<KNameEntry*,KNAME_ENTRY_INIT_SIZE,KNAME_ENTRY_INFLATE_STEP>	Names;		// Table of all names.
	KNameEntry*			NameHash[KNAME_HASH_TABLE_SIZE];			// Hashed names.
	::System::Collections::DynArray<int>         Available;     // Indices of available names.

public:
	KNameEntryStack();
	virtual ~KNameEntryStack();
	virtual NAME_INDEX AllocateNameIndex( NAME_INDEX iOldIndex, const char* Name, EFindName FindType );
	virtual NAME_INDEX AllocateNameIndex( NAME_INDEX iOldIndex, NAME_INDEX iNewIndex );
	virtual void DeallocateNameIndex( NAME_INDEX iIndex );
	virtual const char* GetKNameString( NAME_INDEX iIndex );
	virtual const bool IsValidKName( NAME_INDEX iIndex );
	virtual void CleanRubbishNameEntry();

protected:
	KNameEntry* NewNameEntry( const char* Name, DWORD Index, KNameEntry* HashNext );
	void DeleteNameEntry( int iIndex );
};

//////////////////////////////////////////////////////////////////////////
typedef ::System::Sync::KSync_CS KNameDefaultLocker;

template<class TLockClass, class TNameEntryStackClass>
class KNameEntryStackLocker : public TNameEntryStackClass
{
public:
	typedef TLockClass TLockType;
	typedef TNameEntryStackClass SUPER;

public:
	TLockType m_Locker;

public:
	virtual NAME_INDEX AllocateNameIndex( NAME_INDEX iOldIndex, const char* Name, EFindName FindType )
	{
		m_Locker.Lock();
		NAME_INDEX iRet = SUPER::AllocateNameIndex( iOldIndex, Name, FindType );
		m_Locker.Unlock();
		return iRet;
	}
	virtual NAME_INDEX AllocateNameIndex( NAME_INDEX iOldIndex, NAME_INDEX iNewIndex )
	{
		m_Locker.Lock();
		NAME_INDEX iRet = SUPER::AllocateNameIndex( iOldIndex, iNewIndex );
		m_Locker.Unlock();
		return iRet;
	}
	virtual void DeallocateNameIndex( NAME_INDEX iIndex )
	{
		m_Locker.Lock();
		SUPER::DeallocateNameIndex( iIndex );
		m_Locker.Unlock();
	}
	virtual const char* GetKNameString( NAME_INDEX iIndex )
	{
		m_Locker.Lock();
		const char* szRet = SUPER::GetKNameString( iIndex );
		m_Locker.Unlock();

		// Must use the return value before this KName destruct.
		return szRet;
	}
	virtual const bool IsValidKName( NAME_INDEX iIndex )
	{
		m_Locker.Lock();
		bool bRet = SUPER::IsValidKName( iIndex );
		m_Locker.Unlock();
		return bRet;
	}
};

//////////////////////////////////////////////////////////////////////////
class KNameDefaultReferenceHelper
{
private:
	::System::Collections::DynArray<unsigned short,KNAME_ENTRY_INIT_SIZE,KNAME_ENTRY_INFLATE_STEP>	m_vUseCount;
public:
	int IncreaseUseCount( NAME_INDEX iIndex ) 
	{ 
		KNAME_CHECK( iIndex >= 0 );
		return ++m_vUseCount.at(iIndex);
	}
	int DecreaseUseCount( NAME_INDEX iIndex ) 
	{ 
		KNAME_CHECK( iIndex>=0 && iIndex<m_vUseCount.size() );
		KNAME_CHECK( m_vUseCount[iIndex] > 0 );
		return --m_vUseCount[iIndex]; 
	}
};

template<class T, class TNameEntryStackClass>
class KNameEntryStackReference : public TNameEntryStackClass
{
public:
	typedef T TReferenceHelper;
	typedef TNameEntryStackClass SUPER;

public:
	TReferenceHelper m_Helper;

public:
	virtual NAME_INDEX AllocateNameIndex( NAME_INDEX iOldIndex, const char* Name, EFindName FindType )
	{
		NAME_INDEX iRet = SUPER::AllocateNameIndex( iOldIndex, Name, FindType );
		if ( iOldIndex != iRet )
		{
			DeallocateNameIndex( iOldIndex );
			if ( iRet >= 0 )
			{
				m_Helper.IncreaseUseCount( iRet );
			}
		}
		return iRet;
	}
	virtual NAME_INDEX AllocateNameIndex( NAME_INDEX iOldIndex, NAME_INDEX iNewIndex )
	{	
		NAME_INDEX iRet = KNameEntryStack::AllocateNameIndex( iOldIndex, iNewIndex );
		if ( iOldIndex != iRet )
		{
			DeallocateNameIndex( iOldIndex );
			if ( iRet >= 0 )
			{
				m_Helper.IncreaseUseCount( iRet );
			}
		}
		return iRet;
	}
	virtual void DeallocateNameIndex( NAME_INDEX iIndex )
	{
		if( iIndex >= 0 )
		{
			if ( m_Helper.DecreaseUseCount( iIndex ) <= 0 )
			{
				SUPER::DeleteNameEntry( iIndex );
			}
		}
	}
};

//////////////////////////////////////////////////////////////////////////
typedef KNameEntryStackLocker<KNameDefaultLocker,KNameEntryStack> KNameEntryLockedStack;
typedef KNameEntryStackReference<KNameDefaultReferenceHelper,KNameEntryStack> KNameEntrySharedStack;
typedef KNameEntryStackLocker<KNameDefaultLocker,KNameEntrySharedStack> KNameEntryLockedSharedStack;


/*----------------------------------------------------------------------------
	KName.
----------------------------------------------------------------------------*/

//
// Public name, available to the world.  Names are stored as WORD indices
// into the name table and every name in Unreal is stored once
// and only once in that table.  Names are case-insensitive.
//

class KName 
{
public:
	// Accessors.
	const char* operator*() const
	{
		KNAME_CHECK(DefaultDataStack);
		return DefaultDataStack->GetKNameString( Index );
	}
	NAME_INDEX GetIndex() const
	{
		return Index;
	}
	bool operator==( const KName& Other ) const
	{
		return Index==Other.Index;
	}
	bool operator!=( const KName& Other ) const
	{
		return Index!=Other.Index;
	}
	bool IsValid()
	{
		KNAME_CHECK(DefaultDataStack);
		return DefaultDataStack->IsValidKName( Index );
	}
	const bool IsValid() const
	{
		KNAME_CHECK(DefaultDataStack);
		return DefaultDataStack->IsValidKName( Index );
	}
	bool operator<( const KName& tName )
	{
		return Index < tName.GetIndex();
	}
	const bool operator<( const KName& tName ) const
	{
		return Index < tName.GetIndex();
	}
	operator DWORD() const// for KHashTable
	{
		return Index;
	}
	operator const char*() const
	{
		KNAME_CHECK(DefaultDataStack);
		return DefaultDataStack->GetKNameString( Index );
	}

	// Constructors.
	KName();
	KName( NAME_INDEX iIndex );
	KName( const char* Name, EFindName FindType=KNAME_Add );
	KName( const KName& Other );
	~KName();

	// Copy operator.
	KName& operator=( const KName& Other );

	// Name subsystem.
	static bool IsInitialized() { return DefaultDataStack != NULL; }
	static void StaticInit( KNameEntryStack* pDefaultDataStack );
	static void StaticExit();
	static KNameEntryStack* GetDefaultDataStack() { return DefaultDataStack; }
	static void SetDefaultDataStack( KNameEntryStack* pStack ) { DefaultDataStack = pStack; }

public:
	static const KName INVALID_VALUE;

private:
	// Name index.
	NAME_INDEX Index;

	// Static subsystem variables.
	static KNameEntryStack* DefaultDataStack;
};

//////////////////////////////////////////////////////////////////////////
class KNameLocal
{
public:
	// Accessors.
	const char* operator*() const
	{
		KNAME_CHECK(DefaultDataStack);
		return DefaultDataStack->GetKNameString( Index );
	}
	NAME_INDEX GetIndex() const
	{
		return Index;
	}
	bool operator==( const KNameLocal& Other ) const
	{
		return Index==Other.Index;
	}
	bool operator!=( const KNameLocal& Other ) const
	{
		return Index!=Other.Index;
	}
	bool IsValid()
	{
		KNAME_CHECK(DefaultDataStack);
		return DefaultDataStack->IsValidKName( Index );
	}
	const bool IsValid() const
	{
		KNAME_CHECK(DefaultDataStack);
		return DefaultDataStack->IsValidKName( Index );
	}
	bool operator<( const KNameLocal& tName )
	{
		return Index < tName.GetIndex();
	}
	const bool operator<( const KNameLocal& tName ) const
	{
		return Index < tName.GetIndex();
	}
	operator DWORD() const// for KHashTable
	{
		return Index;
	}
	operator const char*() const
	{
		KNAME_CHECK(DefaultDataStack);
		return DefaultDataStack->GetKNameString( Index );
	}

	// Constructors.
	KNameLocal();
	KNameLocal( KNameEntryStack* pStack );
	KNameLocal( KNameEntryStack* pStack, NAME_INDEX iIndex );
	KNameLocal( KNameEntryStack* pStack, const char* Name, EFindName FindType=KNAME_Add );
	KNameLocal( const KNameLocal& Other );
	~KNameLocal();

	// Copy operator.
	KNameLocal& operator=( const KNameLocal& Other );
	
	KNameEntryStack* GetDefaultDataStack() const { return DefaultDataStack; }

public:
	static const KNameLocal INVALID_VALUE;

private:
	// Name index.
	NAME_INDEX Index;

	// Static subsystem variables.
	KNameEntryStack* DefaultDataStack;
};

/*----------------------------------------------------------------------------
	The End.
----------------------------------------------------------------------------*/

//struct KNameHashCompare
//{
//	enum   
//	{   //   parameters   for   hash   table   
//		bucket_size   =   4,   //   0   <   bucket_size   
//		min_buckets   =   8  //   min_buckets   =   2   ^^   N,   0   <   N   
//	};
//	size_t operator()( const KName& __x ) const
//	{ 
//		return __x.GetIndex();
//	} 
//	bool operator()(const KName& tSrc, const KName& tDest ) const
//	{
//		return tSrc.GetIndex() < tDest.GetIndex();
//	}
//	size_t operator()( const KNameLocal& __x ) const
//	{ 
//		return __x.GetIndex();
//	} 
//	bool operator()(const KNameLocal& tSrc, const KNameLocal& tDest ) const
//	{
//		return tSrc.GetIndex() < tDest.GetIndex();
//	}
//};

#endif