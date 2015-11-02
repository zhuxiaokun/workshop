/********************************************************************
created:	2009/3/21
filename: 	Level2MapSet.h
file path:	
file base:	Level2MapSet
file ext:	h
author:		Allen Wang
copyright:	AuroraGame

purpose:	It contains a class named KLevel2MapSet that used to create
			lot of blocks to seperate map.
*********************************************************************/
#pragma once

#include <System/KType.h>
#include <System/Collections/KVector.h>
#include "PathFinder.h"
#include "Level2Map.h"
#include <System/Collections/KMapByVector.h>
#include <iostream>

/*
 * 按场景划分，用以组织多个L2Map的类
 */
class KLevel2MapSet
{
public:
	const static int BOARD_WIDTH = 512;
	const static int BOARD_HEIGHT = 512;

	struct BLOCK_POS	//Block坐标
	{
		int blockx;
		int blocky;
		BLOCK_POS(int blockx = 0, int blocky = 0): blockx(blockx), blocky(blocky)
		{}
		bool operator <(const BLOCK_POS& other) const
		{
			if (blockx < other.blockx) return true;
			else if (blockx == other.blockx)
			{
				return blocky < other.blocky;
			}
			return false;
		}
		bool operator ==(const BLOCK_POS& other) const
		{
			return blockx == other.blockx && blocky == other.blocky;
		}
		bool operator !=(const BLOCK_POS& other) const
		{
			return !(operator == (other));
		}
	};

public:
	KLevel2MapSet()
	{
// 		m_maps.clear();
		m_pAStar = NULL;
	}

	virtual ~KLevel2MapSet()
	{
		Clear();
	}
	void Clear()
	{
		m_map.Reset();
	}

	BLOCK_POS GetBlockFromPos(int x, int y) const
	{
		//按AStarPlugin定义好的范围进行切块
		BLOCK_POS bp;
		bp.blockx = x / BOARD_WIDTH;
		bp.blocky = y / BOARD_HEIGHT;
		return bp;
	}

	void GetBasePosFromBlock(const BLOCK_POS& bp, int& basex, int& basey)
	{
		basex = bp.blockx * BOARD_WIDTH;
		basey = bp.blocky * BOARD_HEIGHT;
	}

	void GlobalToRelativePos(int& x, int& y)
	{
		x = x % BOARD_WIDTH;
		y = y % BOARD_HEIGHT;
	}

	bool IsExist(const BLOCK_POS& bp) const
	{
		return true;
	}

	KLevel2Map* GetL2Map(const BLOCK_POS& bp)
	{
		if (IsExist(bp))
		{
			return &m_map;
		}
		else
		{
			return NULL;
		}
	}
	const KLevel2Map* GetL2Map(const BLOCK_POS& bp) const
	{
		if (IsExist(bp))
		{
			return &m_map;
		}
		else
		{
			return NULL;
		}
	}

	KLevel2Map* GetL2MapByPos(int x, int y)
	{
		return GetL2Map(GetBlockFromPos(x, y));
	}

	const KLevel2Map* GetL2MapByPos(int x, int y) const
	{
		return GetL2Map(GetBlockFromPos(x, y));
	}

	inline void OffsetL2Map(int x, int y)
	{
		m_map.OffsetMap(x, y);
	}

protected:
	CAStar* m_pAStar;
	KLevel2Map m_map;
public:
	friend std::istream& operator >>(std::istream& is, KLevel2MapSet& maps);
};
std::istream& operator >>(std::istream& is, KLevel2MapSet& maps);
