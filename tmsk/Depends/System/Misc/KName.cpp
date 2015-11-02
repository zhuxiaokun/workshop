
#include "KName.h"
#include "../Misc/KCrc32.h"

/*-----------------------------------------------------------------------------
	KName statics.
-----------------------------------------------------------------------------*/
KNameEntryStack::KNameEntryStack()
{
	for( INT i=0; i<KNAME_HASH_TABLE_SIZE; i++ )
		NameHash[i] = NULL;
}

KNameEntryStack::~KNameEntryStack()
{
	for( INT i=0; i<Names.size(); i++ )
		if( Names[i] )
			delete Names[i];

	// Empty tables.
	Names.clear();
	Available.clear();
}

NAME_INDEX KNameEntryStack::AllocateNameIndex( NAME_INDEX iOldIndex, const char* Name, EFindName FindType )
{
	KNAME_CHECK( Name );

	// If empty or invalid name was specified, return KNAME_None.
	if( !Name[0] )
	{
		return KNAME_None;
	}

	// Try to find the name in the hash.
	INT iHash = crc32c((unsigned char*)Name, (unsigned int)strlen(Name)) & (KNAME_ARRAY_COUNT(NameHash)-1);
	for( KNameEntry* Hash=NameHash[iHash]; Hash; Hash=Hash->HashNext )
	{
		if( strcmp( Name, Hash->Name )==0 )
		{
			// Found it in the hash.
			return Hash->Index;
		}
	}

	// Didn't find name.
	if( FindType==KNAME_Find )
	{
		// Not found.
		return KNAME_None;
	}

	// Find an available entry in the name table.
	NAME_INDEX Index = -1;
	if( Available.size() )
	{
		Index = Available[ Available.size()-1 ];
		Available.erase( Available.size()-1 );

		KNameEntry* pEntry = Names[Index];
		KNAME_CHECK( pEntry );
		KNAME_CHECK( pEntry->Name[0] == 0 && pEntry->Name[1] >= 1 );
		if ( pEntry->Name[1] < (int)strlen(Name) )
		{
			// if the size of pEntry->Name is small than the new name, we reallocate it.
			delete pEntry;
		}
		else
		{
			pEntry->HashNext = NameHash[iHash];
			NameHash[iHash] = pEntry;
			strcpy( pEntry->Name, Name );
			return Index;
		}
	}
	else
	{
		Index = Names.size();
		Names.push_back( NULL );
	}

	// Allocate and set the name.
	Names[Index] = NameHash[iHash] = NewNameEntry( Name, Index, NameHash[iHash] );
	return Index;
}

NAME_INDEX KNameEntryStack::AllocateNameIndex( NAME_INDEX iOldIndex, NAME_INDEX iNewIndex )
{
	if ( iNewIndex>=0 && iNewIndex<Names.size() )
	{
		return iNewIndex;
	}
	return KNAME_None;
}

void KNameEntryStack::DeallocateNameIndex( NAME_INDEX iIndex )
{
	// DoNothing...
}

const char* KNameEntryStack::GetKNameString( NAME_INDEX iIndex )
{
	KNAME_CHECK( iIndex >= 0 && iIndex < Names.size() && Names[iIndex]!=NULL && Names[iIndex]->Name[0] != 0 );
	return Names[iIndex]->Name;
}

const bool KNameEntryStack::IsValidKName( NAME_INDEX iIndex )
{
	return iIndex >= 0 && iIndex < Names.size() && Names[iIndex]!=NULL && Names[iIndex]->Name[0] != 0;
}

void KNameEntryStack::CleanRubbishNameEntry()
{
	int iIndex = 0;
	for ( int ii=0; ii<Available.size(); ii++ )
	{
		iIndex = Available[ii];
		if ( Names[iIndex] )
		{
			delete Names[iIndex];
			Names[iIndex] = NULL;
		}
	}
}

KNameEntry* KNameEntryStack::NewNameEntry( const char* Name, DWORD Index, KNameEntry* HashNext )
{
	int iStringLen = strlen(Name);
	ASSERT( iStringLen <= KNAME_SIZE );
	KNameEntry* NameEntry = (KNameEntry*)malloc_k( sizeof(KNameEntry) - (KNAME_SIZE - iStringLen - 1)*sizeof(char) );
	NameEntry->Index      = Index;
	NameEntry->HashNext   = HashNext;
	strcpy( NameEntry->Name, Name );
	return NameEntry;
}

void KNameEntryStack::DeleteNameEntry( int iIndex )
{
	KNAME_CHECK( iIndex >= 0 && iIndex < Names.size() && Names[iIndex]!=NULL );

	KNameEntry* NameEntry = Names[iIndex];
	KNAME_CHECK(NameEntry);
	INT iHash = crc32c((unsigned char*)NameEntry->Name, (unsigned int)strlen(NameEntry->Name)) & (KNAME_ARRAY_COUNT(NameHash)-1);
	KNameEntry** HashLink = NULL;
	for( HashLink=&NameHash[iHash]; *HashLink && *HashLink!=NameEntry; HashLink=&(*HashLink)->HashNext );
	KNAME_CHECK( *HashLink );
	*HashLink = (*HashLink)->HashNext;

	// Delete it.
	// delete NameEntry;
	// Since the memery of NameEntry is not big, so we leave it until destructor. If needed, we also can open the below code.
	int iLen = (int)strlen(NameEntry->Name);
	KNAME_CHECK( iLen >= 1 );
	NameEntry->Name[1] = iLen;
	NameEntry->Name[0] = 0;
	Available.push_back( iIndex );

	// Clean NameEntry regularly.
	//static int s_iCleanAvailableSize = 64;
	//if ( Available.size() > s_iCleanAvailableSize
	//	&& Available.size() % s_iCleanAvailableSize == 0 )
	//{
	//	int iStopIndex = Available.size() - s_iCleanAvailableSize;
	//	int iTempIndex = 0;
	//	for ( int ii=0; ii<iStopIndex; ii++ )
	//	{
	//		iTempIndex = Available[ii];
	//		if ( Names[iTempIndex] )
	//		{
	//			delete Names[iTempIndex];
	//			Names[iTempIndex] = NULL;
	//		}
	//	}
	//}
}

//////////////////////////////////////////////////////////////////////////
KNameEntryStack* KName::DefaultDataStack = NULL;

KName::KName()
: Index( -1 )
{
}

KName::KName( NAME_INDEX iIndex )
: Index( -1 )
{
	KNAME_CHECK( DefaultDataStack );
	Index = DefaultDataStack->AllocateNameIndex( Index, iIndex );
}

KName::KName( const char* Name, EFindName FindType )
: Index( -1 )
{
	KNAME_CHECK( DefaultDataStack );
	Index = DefaultDataStack->AllocateNameIndex( Index, Name, FindType );
}

KName::KName( const KName& Other )
: Index( -1 )
{
	KNAME_CHECK( DefaultDataStack );
	Index = DefaultDataStack->AllocateNameIndex( Index, Other.Index );
}

KName& KName::operator=( const KName& Other )
{
	KNAME_CHECK( DefaultDataStack );
	Index = DefaultDataStack->AllocateNameIndex( Index, Other.Index );
	return *this;
}

KName::~KName()
{
	if ( DefaultDataStack )
	{
		if ( Index>=0 && Index<DefaultDataStack->Names.size() )
		{
			DefaultDataStack->DeallocateNameIndex( Index );
		}
	}
}

void KName::StaticInit( KNameEntryStack* pDefaultDataStack )
{
	KNAME_CHECK( pDefaultDataStack );
	KNAME_CHECK( !DefaultDataStack );
	DefaultDataStack = pDefaultDataStack;
}

void KName::StaticExit()
{
	KNAME_CHECK( DefaultDataStack );
	delete DefaultDataStack;
	DefaultDataStack = NULL;
}


//////////////////////////////////////////////////////////////////////////
KNameLocal::KNameLocal()
: Index(-1)
, DefaultDataStack( NULL )
{

}

KNameLocal::KNameLocal( KNameEntryStack* pStack )
: Index( -1 )
, DefaultDataStack( pStack )
{
	KNAME_CHECK( DefaultDataStack );
}

KNameLocal::KNameLocal( KNameEntryStack* pStack, NAME_INDEX iIndex )
: Index( -1 )
, DefaultDataStack( pStack )
{
	KNAME_CHECK( DefaultDataStack );
	Index = DefaultDataStack->AllocateNameIndex( Index, iIndex );
}

KNameLocal::KNameLocal( KNameEntryStack* pStack, const char* Name, EFindName FindType )
: Index( -1 )
, DefaultDataStack( pStack )
{
	KNAME_CHECK( DefaultDataStack );
	Index = DefaultDataStack->AllocateNameIndex( Index, Name, FindType );
}

KNameLocal::KNameLocal( const KNameLocal& Other )
: Index( -1 )
{
	*this = Other;
}

KNameLocal& KNameLocal::operator=( const KNameLocal& Other )
{
	DefaultDataStack = Other.GetDefaultDataStack();
	Index = DefaultDataStack->AllocateNameIndex( Index, Other.Index );
	return *this;
}

KNameLocal::~KNameLocal()
{
	if ( DefaultDataStack )
	{
		if ( Index>=0 && Index<DefaultDataStack->Names.size() )
		{
			DefaultDataStack->DeallocateNameIndex( Index );
		}
	}
}


/*-----------------------------------------------------------------------------
	The End.
-----------------------------------------------------------------------------*/

