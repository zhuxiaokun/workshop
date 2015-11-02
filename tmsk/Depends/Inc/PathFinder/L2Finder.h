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
	 * ����AStar����
	 */
	virtual void SetupAStar(CAStar* pAStar) = 0;
	/**
	 * ����ֱ��̽�����Ƿ������
	 */
	virtual bool LineTest(int srcX, int srcY, int destX, int destY) const = 0;

	virtual ~IL2FinderPlugin(){}
};

class KL2Finder
{
public:
	enum ENUM_LAST_FIND_RESULT_CODE
	{
		//�ɹ�
		enumLFRC_SUCCESS		= 0,
		//ʧ��
		enumLFRC_FAIL,
		//���������ϰ���
		enumLFRC_ORG_IN_OBSTACLE,
		//Ŀ������ϰ���
		enumLFRC_TARGET_IN_OBSTACLE,
		//��֧�ֿ糡��AStarѰ·
		enumLFRC_DO_NOT_SUPPORT_MULTISCENE_ASTAR,
		//�Ѿ���Ŀ���
		enumLFRC_ALREADY_REACHED,
		//С��Χ·�������ɹ�
		enumLFRC_SMALL_AREA_SEARCH_SUCCESS,
		//С��Χ·������ʧ��
		enumLFRC_SMALL_AREA_SEARCH_FAIL,
		//ֱ�߿ɴ�
		enumLFRC_REACH_DIRECTLY,
		//�����㸽��û��·��
		enumLFRC_CAN_NOT_FIND_ROAD_POINT_AROUND_ORG,
		//Ŀ��㸽��û��·��
		enumLFRC_CAN_NOT_FIND_ROAD_POINT_AROUND_TARGET,
		//·�㲻�ɴ�
		enumLFRC_CAN_NOT_REACH_FROM_RDPT_TO_RDPT,
		//��Χ·�������ɹ�
		enumLFRC_LARGE_AREA_SEARCH_SUCCESS,
		//�з�Χ·�������ɹ�
		enumLFRC_MIDDLE_AREA_SEARCH_SUCCESS,
		//��ΧѰ·ʧ�ܵ�С��ΧѰ·�ɹ�
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
		//�ɹ�
		case enumLFRC_SUCCESS:
			sprintf(m_strLastFindResult, "�ɹ�");
			break;
			//ʧ��
		case enumLFRC_FAIL:
			sprintf(m_strLastFindResult, "ʧ��");
			break;
			//���������ϰ���
		case enumLFRC_ORG_IN_OBSTACLE:
			sprintf(m_strLastFindResult, "���������ϰ���");
			break;
			//Ŀ������ϰ���
		case enumLFRC_TARGET_IN_OBSTACLE:
			sprintf(m_strLastFindResult, "Ŀ������ϰ���");
			break;
			//��֧�ֿ糡��AStarѰ·
		case enumLFRC_DO_NOT_SUPPORT_MULTISCENE_ASTAR:
			sprintf(m_strLastFindResult, "��֧�ֿ糡��AStarѰ·");
			break;
			//�Ѿ���Ŀ���
		case enumLFRC_ALREADY_REACHED:
			sprintf(m_strLastFindResult, "�Ѿ���Ŀ���");
			break;
			//С��Χ·�������ɹ�
		case enumLFRC_SMALL_AREA_SEARCH_SUCCESS:
			sprintf(m_strLastFindResult, "С��Χ·�������ɹ�");
			break;
			//С��Χ·������ʧ��
		case enumLFRC_SMALL_AREA_SEARCH_FAIL:
			sprintf(m_strLastFindResult, "С��Χ·������ʧ��");
			break;
			//ֱ�߿ɴ�
		case enumLFRC_REACH_DIRECTLY:
			sprintf(m_strLastFindResult, "ֱ�߿ɴ�");
			break;
			//�����㸽��û��·��
		case enumLFRC_CAN_NOT_FIND_ROAD_POINT_AROUND_ORG:
			sprintf(m_strLastFindResult, "�����㸽��û��·��");
			break;
			//Ŀ��㸽��û��·��
		case enumLFRC_CAN_NOT_FIND_ROAD_POINT_AROUND_TARGET:
			sprintf(m_strLastFindResult, "Զ����Ѱ·��Ŀ���(%d, %d)����û��·��", m_lastFindResultExtData[0], m_lastFindResultExtData[1]);
			break;
			//·�㲻�ɴ�
		case enumLFRC_CAN_NOT_REACH_FROM_RDPT_TO_RDPT:
			sprintf(m_strLastFindResult, "Զ��Ѱ·Ŀ�긽��·�㲻��ֱ�ӵ���[·��(%d, %d)��·��(%d, %d)]", m_lastFindResultExtData[0], m_lastFindResultExtData[1], m_lastFindResultExtData[2], m_lastFindResultExtData[3]);
			break;
			//��Χ·�������ɹ�
		case enumLFRC_LARGE_AREA_SEARCH_SUCCESS:
			sprintf(m_strLastFindResult, "��Χ·�������ɹ�");
			break;
			//�з�Χ·�������ɹ�
		case enumLFRC_MIDDLE_AREA_SEARCH_SUCCESS:
			sprintf(m_strLastFindResult, "�з�Χ·�������ɹ�");
			break;
			//��ΧѰ·ʧ�ܵ�С��ΧѰ·�ɹ�
		case enumLFRC_LARGE_FAIL_BUT_SMALL_SUCCESS:
			sprintf(m_strLastFindResult, "��ΧѰ·ʧ�ܵ�С��ΧѰ·�ɹ�");
			break;
		default:
			sprintf(m_strLastFindResult, "δ֪ԭ��");
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
	 * �ӵ��ߵ�L2Map��
	 * param:
	 *		x, y:		����������
	 *		st:			�����洢·��
	 * return:
	 *		��·������L2Map��JOINT��id������0
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
