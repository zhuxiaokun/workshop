/********************************************************************
	created:	2007/11/18
	created:	18:11:2007   22:09
	filename: 	e:\CVSXueyan(j)\System\System\Collections\KCoupler.h
	file path:	e:\CVSXueyan(j)\System\System\Collections
	file base:	KCoupler
	file ext:	h
	author:		xueyan
	
	purpose:	这里是要实现一个和STL pair类似的一个容器，把两个不相关的数据组合到一起去。
*********************************************************************/
#pragma once

namespace System {			namespace Collections {

template< class TypeA, class TypeB >
class KCoupler
{
public:
	typedef		const TypeA
				CTYPEA;
	typedef		const TypeB
				CTYPEB;
	typedef		const KCoupler<TypeA, TypeB>
				CSELF;

public:
	KCoupler();
	KCoupler( CTYPEA & rItemA );
	KCoupler( CTYPEA & rItemA, CTYPEB & rItemB );
	BLank		~KCoupler();

	BLank		operator TypeB();
	BLank		operator CTYPEB() const;


	BOOL		operator <( CSELF & rRightHandSide ) const;
	BOOL		operator >( CSELF & rRightHandSide ) const;

	BOOL		operator ==( CSELF & rRightHandSide ) const;
	BOOL		operator !=( CSELF & rRightHandSide ) const;

	BOOL		operator <=( CSELF & rRightHandSide ) const;
	BOOL		operator >=( CSELF & rRightHandSide ) const;

public:
	TypeA		clsItemA;
	TypeB		clsItemB;

};

template< class TypeA, class TypeB >
KCoupler<TypeA,TypeB>::KCoupler()
	: clsItemA(_type_default_value_<TypeA>::get())
	, clsItemB(_type_default_value_<TypeB>::get())
{
}

template< class TypeA, class TypeB >
KCoupler<TypeA,TypeB>::KCoupler( CTYPEA & rItemA )
	: clsItemA(rItemA)
	, clsItemB(_type_default_value_<TypeB>::get())
{
}

template< class TypeA, class TypeB >
KCoupler<TypeA,TypeB>::KCoupler( CTYPEA & rItemA, CTYPEB & rItemB )
	: clsItemA(rItemA)
	, clsItemB(rItemB)
{
}

template< class TypeA, class TypeB >
KCoupler<TypeA,TypeB>::~KCoupler()
{
}

template< class TypeA, class TypeB >
KCoupler<TypeA,TypeB>::operator TypeB()
{
	return clsItemB;
}

template< class TypeA, class TypeB >
KCoupler<TypeA,TypeB>::operator const TypeB() const
{
	return clsItemB;
}


template< class TypeA, class TypeB >
inline BOOL
KCoupler<TypeA,TypeB>::operator <( CSELF & rRightHandSide ) const
{
	if ( clsItemA < rRightHandSide.clsItemA )
	{
		return TRUE;
	}

	return FALSE;
}

template< class TypeA, class TypeB >
inline BOOL
KCoupler<TypeA,TypeB>::operator >( CSELF & rRightHandSide ) const
{
	if ( clsItemA > rRightHandSide.clsItemA )
	{
		return TRUE;
	}

	return FALSE;
}

template< class TypeA, class TypeB >
inline BOOL
KCoupler<TypeA,TypeB>::operator ==( CSELF & rRightHandSide ) const
{
	if ( clsItemA == rRightHandSide.clsItemA )
	{
		return TRUE;
	}

	return FALSE;
}

template< class TypeA, class TypeB >
inline BOOL
KCoupler<TypeA,TypeB>::operator !=( CSELF & rRightHandSide ) const
{
	if ( clsItemA != rRightHandSide.clsItemA )
	{
		return TRUE;
	}

	return FALSE;
}

template< class TypeA, class TypeB >
inline BOOL
KCoupler<TypeA,TypeB>::operator <=( CSELF & rRightHandSide ) const
{
	if ( clsItemA <= rRightHandSide.clsItemA )
	{
		return TRUE;
	}

	return FALSE;
}

template< class TypeA, class TypeB >
inline BOOL
KCoupler<TypeA,TypeB>::operator >=( CSELF & rRightHandSide ) const
{
	if ( clsItemA >= rRightHandSide.clsItemA )
	{
		return TRUE;
	}

	return FALSE;
}

} /* Collections */			} /* System */