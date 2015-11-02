/********************************************************************
created:	2009/3/23
filename: 	L2Finder.h
file path:	
file base:	L2Finder
file ext:	h
author:		Allen Wang
copyright:	AuroraGame

purpose:	To find a path using l2map.
*********************************************************************/
#pragma once

#include "Level2MapSet.h"
#define L2MAP_INFO_FILENAME "World/output.l2m"

class IL2FinderPlugin
{
public:
	/**
	 * 设置AStar配置
	 */
	virtual void SetupAStar(CAStar* pAStar) = 0;
	/**
	 * 进行直线探测检查是否可行走
	 */
	virtual bool LineTest(int srcX, int srcY, int destX, int destY) const = 0;

	virtual ~IL2FinderPlugin(){}
};

class KL2Finder
{
public:
	enum ENUM_LAST_FIND_RESULT_CODE
	{
		//成功
		enumLFRC_SUCCESS		= 0,
		//失败
		enumLFRC_FAIL,
		//出发点在障碍中
		enumLFRC_ORG_IN_OBSTACLE,
		//目标点在障碍中
		enumLFRC_TARGET_IN_OBSTACLE,
		//不支持跨场景AStar寻路
		enumLFRC_DO_NOT_SUPPORT_MULTISCENE_ASTAR,
		//已经在目标点
		enumLFRC_ALREADY_REACHED,
		//小范围路径搜索成功
		enumLFRC_SMALL_AREA_SEARCH_SUCCESS,
		//小范围路径搜索失败
		enumLFRC_SMALL_AREA_SEARCH_FAIL,
		//直线可达
		enumLFRC_REACH_DIRECTLY,
		//出发点附近没有路点
		enumLFRC_CAN_NOT_FIND_ROAD_POINT_AROUND_ORG,
		//目标点附近没有路点
		enumLFRC_CAN_NOT_FIND_ROAD_POINT_AROUND_TARGET,
		//路点不可达
		enumLFRC_CAN_NOT_REACH_FROM_RDPT_TO_RDPT,
		//大范围路径搜索成功
		enumLFRC_LARGE_AREA_SEARCH_SUCCESS,
		//中范围路径搜索成功
		enumLFRC_MIDDLE_AREA_SEARCH_SUCCESS,
		//大范围寻路失败但小范围寻路成功
		enumLFRC_LARGE_FAIL_BUT_SMALL_SUCCESS,
	};

public:
	KL2Finder(CAStar* pAStar, IL2FinderPlugin* pFP)
		: m_pAStar(pAStar)
		, m_plugin(pFP)
		, m_bAStarCompact(false)
	{
		m_strLastFindResult[0] = 0;
	}
	bool LoadL2PathInfo(const char* filename = L2MAP_INFO_FILENAME);
	bool LoadL2PathInfo(std::istream& fs);
	bool ClearL2PathInfo();

	bool FindPath( int fromx, int fromy, int tox, int toy, KLevel2Map::STEP_TYPE& st, int maxLoop = 0 );

	bool FindPathByAStar( int fromx, int fromy, int tox, int toy, KLevel2Map::STEP_TYPE& st, int maxLoop = 0 );

	bool LineTest( int nSrcX, int nSrcY, int nDestX, int nDestY ) const;

	inline void SetPlugin(IL2FinderPlugin* pFP)
	{
		m_plugin = pFP;
	}

	inline const char* GetLastResultMsg() const
	{
		switch(m_lastFindResult)
		{
		//成功
		case enumLFRC_SUCCESS:
			sprintf(m_strLastFindResult, "成功");
			break;
			//失败
		case enumLFRC_FAIL:
			sprintf(m_strLastFindResult, "失败");
			break;
			//出发点在障碍中
		case enumLFRC_ORG_IN_OBSTACLE:
			sprintf(m_strLastFindResult, "出发点在障碍中");
			break;
			//目标点在障碍中
		case enumLFRC_TARGET_IN_OBSTACLE:
			sprintf(m_strLastFindResult, "目标点在障碍中");
			break;
			//不支持跨场景AStar寻路
		case enumLFRC_DO_NOT_SUPPORT_MULTISCENE_ASTAR:
			sprintf(m_strLastFindResult, "不支持跨场景AStar寻路");
			break;
			//已经在目标点
		case enumLFRC_ALREADY_REACHED:
			sprintf(m_strLastFindResult, "已经在目标点");
			break;
			//小范围路径搜索成功
		case enumLFRC_SMALL_AREA_SEARCH_SUCCESS:
			sprintf(m_strLastFindResult, "小范围路径搜索成功");
			break;
			//小范围路径搜索失败
		case enumLFRC_SMALL_AREA_SEARCH_FAIL:
			sprintf(m_strLastFindResult, "小范围路径搜索失败");
			break;
			//直线可达
		case enumLFRC_REACH_DIRECTLY:
			sprintf(m_strLastFindResult, "直线可达");
			break;
			//出发点附近没有路点
		case enumLFRC_CAN_NOT_FIND_ROAD_POINT_AROUND_ORG:
			sprintf(m_strLastFindResult, "出发点附近没有路点");
			break;
			//目标点附近没有路点
		case enumLFRC_CAN_NOT_FIND_ROAD_POINT_AROUND_TARGET:
			sprintf(m_strLastFindResult, "远距离寻路的目标点(%d, %d)附近没有路点", m_lastFindResultExtData[0], m_lastFindResultExtData[1]);
			break;
			//路点不可达
		case enumLFRC_CAN_NOT_REACH_FROM_RDPT_TO_RDPT:
			sprintf(m_strLastFindResult, "远距寻路目标附近路点不可直接到达[路点(%d, %d)到路点(%d, %d)]", m_lastFindResultExtData[0], m_lastFindResultExtData[1], m_lastFindResultExtData[2], m_lastFindResultExtData[3]);
			break;
			//大范围路径搜索成功
		case enumLFRC_LARGE_AREA_SEARCH_SUCCESS:
			sprintf(m_strLastFindResult, "大范围路径搜索成功");
			break;
			//中范围路径搜索成功
		case enumLFRC_MIDDLE_AREA_SEARCH_SUCCESS:
			sprintf(m_strLastFindResult, "中范围路径搜索成功");
			break;
			//大范围寻路失败但小范围寻路成功
		case enumLFRC_LARGE_FAIL_BUT_SMALL_SUCCESS:
			sprintf(m_strLastFindResult, "大范围寻路失败但小范围寻路成功");
			break;
		default:
			sprintf(m_strLastFindResult, "未知原因");
			;
		}
		return m_strLastFindResult;
	}

	inline INT GetLastResultCode() const
	{
		return m_lastFindResult;
	}

	inline void OffsetL2Map(int x, int y)
	{
		m_mapSet.OffsetL2Map(x, y);
	}

private:

	/**
	 * 从点走到L2Map上
	 * param:
	 *		x, y:		出发点坐标
	 *		st:			用来存储路径
	 * return:
	 *		有路径返回L2Map上JOINT的id，否则0
	 */
	DWORD PtStepToL2Map(KLevel2Map* pL2M, int x, int y, KLevel2Map::STEP_TYPE& st);

	DWORD L2MapStepToPt(KLevel2Map* pL2M, int x, int y, KLevel2Map::STEP_TYPE& st);

	bool _FindPath(int fromx, int fromy, int tox, int toy, KLevel2Map::ONE_ROAD& road);

	bool IsObstacle(int fromx, int fromy, int x, int y) const;
	bool IsObstacle(int x, int y) const;

	bool _FindPathByAStar( int fromx, int fromy, int tox, int toy, KLevel2Map::ONE_ROAD& road );

private:
	KLevel2MapSet m_mapSet;
	mutable char m_strLastFindResult[1000];
	CAStar* m_pAStar;
	IL2FinderPlugin* m_plugin;
	bool m_bAStarCompact;
	void SetAStarPathCompact(bool val = true)
	{
		m_bAStarCompact = val;
	}

	ENUM_LAST_FIND_RESULT_CODE m_lastFindResult;
	INT m_lastFindResultExtData[4];

	void SetLastResult(ENUM_LAST_FIND_RESULT_CODE code, INT d1 = 0, INT d2 = 0, INT d3 = 0, INT d4 = 0)
	{
		m_lastFindResult = code;
		m_lastFindResultExtData[0] = d1;
		m_lastFindResultExtData[1] = d2;
		m_lastFindResultExtData[2] = d3;
		m_lastFindResultExtData[3] = d4;
	}
	void CompactPath(KLevel2Map::STEP_TYPE& rd);
};
