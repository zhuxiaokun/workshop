#pragma once
#include <System/KType.h>
#include <System/Collections/DynArray.h>
#include <System/Collections/KMapByVector.h>
#include <System/Misc/KStream.h>
#include "../interface/IFinderPlugin.h"
#include "../interface/PathFinderDefine.h"

namespace KPathFinderAbout
{
	class IAlgorithmBase;
	struct KBlockPos
	{
		int_r blockx;
		int_r blocky;
		KBlockPos(int_r blockx = 0, int_r blocky = 0): blockx(blockx), blocky(blocky) {}
		bool operator <(const KBlockPos& other) const {return blockx < other.blockx ? true : (blockx == other.blockx ? blocky < other.blocky : false);}
		bool operator ==(const KBlockPos& other) const {return blockx == other.blockx && blocky == other.blocky;}
		bool operator !=(const KBlockPos& other) const {return !(operator == (other));}
	};

	class KL2Map_Base
	{
	public:
		class __ID_Cmp
		{
		public:
			int operator () (const _Node_Lite * a, const _Node_Lite * b) const {return a->id < b->id ? -1 : (a->id > b->id ? 1 : 0);}
		};
		class __Pos_Cmp
		{
		public:
			int operator () (const _Node_Lite * a, const _Node_Lite * b) const {return a->x < b->x ? -1 : (a->x > b->x ? 1 : (a->y < b->y ? -1 :(a->y > b->y ? 1 : 0)));}
		};

		// 节点位置和ID索引
		typedef JG_C::KDynSortedArray<const _Node_Lite *, __Pos_Cmp> __NodeList_ByPos;
		typedef JG_C::KDynSortedArray<const _Node_Lite *, __ID_Cmp> __NodeList_ByID;

		// 节点map
		typedef JG_C::KMapByVector<int_r, __NodeList_ByPos> __NodeMap_ByPos;
		typedef JG_C::KMapByVector<int_r, __NodeMap_ByPos> __NodeL2Map_ByPos;
		typedef JG_C::KMapByVector<int_r, int_r> __NodeMap_ByID;
		typedef JG_C::KMapByVector<int_r, __NodeMap_ByID> __NodeL2Map_ByID;

		// 到目标点的id和距离信息
		typedef KPair<const _Node_Lite *, VALUE_T> __DistanceInfo;
		class __DistanceInfoCmp
		{
		public:
			int operator () (const __DistanceInfo& a, const __DistanceInfo& b) const {return a.second < b.second ? -1 : (a.second > b.second ? 1 : 0);}
		};
		typedef JG_C::KDynSortedArray<__DistanceInfo, __DistanceInfoCmp> __NodeList_ByDistance;

	public:
		KL2Map_Base();
		virtual ~KL2Map_Base();

		inline void SetSettings(const KL2MapSettings * pSsettings) {mpSettings = pSsettings;}
		inline const KL2MapSettings * GetSettings() const {return mpSettings;}
		inline void SetFinder(IFinderPlugin * pFinder) {m_pFinder = pFinder;}
		inline IFinderPlugin * GetFinder() const {return m_pFinder;}
		inline void SetAStar(IAlgorithmBase * pAStar) {m_pAStar = pAStar;}
		inline IAlgorithmBase * GetAStar(IAlgorithmBase * pAStar) {return m_pAStar;}

		virtual const _Node_Lite * Find(int_r x, int_r y) const;
		virtual const _Node_Lite * Find(int_r id) const;

		/**
		 * 从已经算好的路中搜索二级路径，结果可由GetLastRoad()得到。
		 * param:
		 *		from:		起点索引
		 *		to:			终点索引
		 * return:
		 *		结果。
		 */
		virtual bool FindPath(int_r from, int_r to, __Nodes& st, VALUE_T& cost);

		/**
		 * 根据到目标点的距离计算排序列表
		 * param:
		 *		fromx, fromy:		起始点坐标
		 *		ptList:				用以填充排序序列
		 */
		virtual void ComputePtListWithDistance(int_r fromx, int_r fromy, __NodeList_ByDistance& ptList);

		/**
		 * 根据两个邻接节点的索引得到一条直接连接路
		 * param:
		 *		from:		起点索引
		 *		to:			目的索引
		 * return:
		 *		如果连通返回相应路径指针，否则NULL
		 */
		virtual const __OneRoad * GetRoad(int_r from, int_r to) const;

		/**
		 * 平移二级地图(x, y)个单位
		 * param:
		 *		x, y:		平移的位置。
		 */
		virtual void OffsetMap(int_r x, int_r y);

		/**
		 * 获得划分坐标
		 */
		virtual void GetJointBlockPos(int_r x, int_r y, int_r& bx, int_r& by) const;

		/**
		 * 重新计算所有节点的区块索引
		 */
		virtual void RecomputeJointBlock();

		/**
		 * 加入一个节点 会自动加入另外2中索引
		 */
		virtual bool AddOneJoint(const _Node_Lite * pNode);

		/**
		 * 创造一条路径 会自动设置l2map[起点id][终点id] = 路线id
		 */
		virtual bool CreateOneRoad(int_r nID, const _Node_Lite * pBegin, const _Node_Lite * pEnd, bool bUnidirectional);

	protected:
		/**
		 * 插入区块节点群
		 */
		bool _InsertJointToBlock(const _Node_Lite * pNode);

	protected:
		const KL2MapSettings * mpSettings;
		IFinderPlugin * m_pFinder;
		IAlgorithmBase * m_pAStar;
		int_r offsetX;
		int_r offsetY;

	public:
		__NodeList_ByPos mPosArray;
		__NodeList_ByID mIDArray;
		__NodeL2Map_ByPos mPosMap;
		__NodeL2Map_ByID mIDMap;

		//所有路的池
		__RoadMap_ByID mRoads;

		// 所有节点 区块排序
		__NodeL2Map_ByPos m_L2MapByBlock;
	};
}
DECLARE_SIMPLE_TYPE(KPathFinderAbout::KL2Map_Base::__DistanceInfo);