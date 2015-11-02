#include "PathFinderLib.h"
#include <math.h>
#include "algorithm/AStar.h"

namespace KPathFinderAbout
{
	KPathFinder::KPathFinder()
	{
		//
	}

	void KPathFinder::SetAStar(IAlgorithmBase * pAStar)
	{
		mpAStar = pAStar;
	}

	IAlgorithmBase * KPathFinder::GetAStar()
	{
		return mpAStar;
	}

	void KPathFinder::SetL2Map(KL2Map_Base * pMap)
	{
		mpL2Map = pMap;
		if(mpAStar&&mpL2Map)
		{
			mpL2Map->SetAStar(mpAStar);
		}
		if(m_L2_Plugin&&mpL2Map)
		{
			mpL2Map->SetFinder(m_L2_Plugin);
		}
	}

	KL2Map_Base * KPathFinder::GetL2Map()
	{
		return mpL2Map;
	}

	void KPathFinder::SetSettings(const KL2MapSettings * pSettings)
	{
		if(mpL2Map)
			mpL2Map->SetSettings(pSettings);
	}

	const KL2MapSettings * KPathFinder::GetSettings()
	{
		if(mpL2Map)
			mpL2Map->GetSettings();
		return NULL;
	}

	void KPathFinder::SetIFinderPluginL1(IFinderPlugin * pL1Plugin)
	{
		m_L1_Plugin = pL1Plugin;
	}

	IFinderPlugin * KPathFinder::GetIFinderPluginL1()
	{
		return m_L1_Plugin;
	}

	void KPathFinder::SetIFinderPluginL2(IFinderPlugin * pL2Plugin)
	{
		m_L2_Plugin = pL2Plugin;
		if(m_L2_Plugin&&mpL2Map)
		{
			mpL2Map->SetFinder(m_L2_Plugin);
		}
	}

	IFinderPlugin * KPathFinder::GetIFinderPluginL2()
	{
		return m_L2_Plugin;
	}

	void KPathFinder::OffsetL2Map(int_r x, int_r y)
	{
		if(mpL2Map)
			mpL2Map->OffsetMap(x, y);
	}

	int_r KPathFinder::SetAStarMaxStep(int_r nMax)
	{
		if(mpAStar)
		{
			mpAStar->SetMaxLoop(nMax);
			return nMax;
		}
		return 0;
	}

	int_r KPathFinder::SetAStarMaxLineLength(int_r nMax)
	{
		if(mpAStar)
		{
			mpAStar->SetMaxLineLength(nMax);
			return nMax;
		}
		return 0;
	}

	bool KPathFinder::IsObstacle(int_r x, int_r y) const
	{
		return m_L1_Plugin ? m_L1_Plugin->IsObstacle(x, y) : false;
	}

	bool KPathFinder::IsObstacle(const _Node_Lite * pNode) const
	{
		return m_L2_Plugin ? m_L2_Plugin->IsObstacle(pNode->x, pNode->y) : false;
	}

	bool KPathFinder::FindPathByCell(int_r fromx, int_r fromy, int_r tox, int_r toy, __OneRoad& road)
	{
		if( NULL == mpAStar )
			return false;
		ASSERT(mpAStar->GetMaxLoop() > 0);

		m_L1_Plugin->SetupAStar(mpAStar, NULL);

		road.reset();
		if (IsObstacle(fromx, fromy) || IsObstacle(tox, toy))
		{
			return false;
		}

		if (fromx == tox && fromy == toy)
		{
			road.m_Cost = 0;
			push_point(road.m_Road, tox, toy);
			return true;
		}
		
		int_r nStep = mpAStar->GeneratePath(fromx, fromy, tox, toy, mpAStar->GetMaxLoop(), 0, 0);
		if ( nStep > 0 )
		{
			// 反转
			_Node * pNode = mpAStar->GetBestNodeFromStart();
			if (!pNode)
			{
				return false;
			}
			road.m_Cost = pNode->f;

			// 压缩
			m_L1_Plugin->PathCompact(pNode);

			while (pNode)
			{
				push_point(road.m_Road, pNode->x, pNode->y);
				pNode = pNode->parent;
			}
			mpAStar->ReleaseNodes();
			return true;
		}
		return false;
	}

	bool KPathFinder::FindPathByConnectedGraph(int_r from, int_r to, __Nodes& st, VALUE_T& cost)
	{
		if(NULL == mpL2Map)
			return false;

		const _Node_Lite * pFrom = mpL2Map->Find(from);
		const _Node_Lite * pTo = mpL2Map->Find(to);
		if(NULL == pFrom)
			return false;
		if(NULL == pTo)
			return false;
		return mpL2Map->FindPath(from, to, st, cost);
	}

	bool KPathFinder::IsExist( int_r x, int_r y ) const
	{
		return m_L1_Plugin ? m_L1_Plugin->IsExist(x, y) : true;	
	}

	bool KPathFinder::IsExist( const _Node_Lite * pNode ) const
	{
		ASSERT_RETURN(pNode, true);
		return IsExist(pNode->x, pNode->y);
	}


}