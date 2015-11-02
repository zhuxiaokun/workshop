/********************************************************************
created:	2009/3/21
filename: 	Level2MapSet.cpp
file path:	
file base:	Level2MapSet
file ext:	cpp
author:		Allen Wang
copyright:	AuroraGame

purpose:	Implementation part of Level2MapSet system.
*********************************************************************/
#include "Level2MapSet.h"

std::istream& operator >>(std::istream& is, KLevel2MapSet& maps)
{
	is >> maps.m_map;
	return is;
}
