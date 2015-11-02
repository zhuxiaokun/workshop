#pragma once
#include "interface/IFinderPlugin.h"
#include "interface/PathFinderDefine.h"
#include "L2Map/L2Map.h"
#include "algorithm/AStar.h"

namespace KPathFinderAbout
{
	class IAlgorithmBase;
	class KPathFinder
	{
	public:
		KPathFinder();
		bool Setup(IAlgorithmBase * pAStar, IFinderPlugin * pL1Plugin, IFinderPlugin * pL2Plugin, const KL2MapSettings &settings);
		
		void SetAStar(IAlgorithmBase * pAStar);
		IAlgorithmBase * GetAStar();
		void SetL2Map(KL2Map_Base * pMap);
		KL2Map_Base * GetL2Map();
		void SetSettings(const KL2MapSettings * pSettings);
		const KL2MapSettings * GetSettings();
		void SetIFinderPluginL1(IFinderPlugin * pL1Plugin);
		IFinderPlugin * GetIFinderPluginL1();
		void SetIFinderPluginL2(IFinderPlugin * pL2Plugin);
		IFinderPlugin * GetIFinderPluginL2();

		int_r SetAStarMaxStep(int_r nMax);
		int_r SetAStarMaxLineLength(int_r nMax);
		void OffsetL2Map(int_r x, int_r y);

		bool IsObstacle(int_r x, int_r y) const;
		bool IsObstacle(const _Node_Lite * pNode) const;
		bool IsExist(int_r x, int_r y) const;
		bool IsExist(const _Node_Lite * pNode) const;
		bool FindPathByCell(int_r fromx, int_r fromy, int_r tox, int_r toy, __OneRoad& road);
		bool FindPathByConnectedGraph(int_r from, int_r to, __Nodes& st, VALUE_T& cost);

	private:
		KL2Map_Base * mpL2Map;
		IAlgorithmBase * mpAStar;
		IFinderPlugin * m_L1_Plugin;
		IFinderPlugin * m_L2_Plugin;
	};
}