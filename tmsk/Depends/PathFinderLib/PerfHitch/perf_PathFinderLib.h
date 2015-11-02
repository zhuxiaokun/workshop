#pragma once
#include <System/Time/KPerfCounter.h>
#include <System/Time/KPerfHitchCounter.h>

#if defined(_MONIT_PERFORMANCE_) || defined(DEF_PERF_HITCH_COUNTER)
	#define REGISTER_PATHFINDERLIB_PERF() register_PathFinderLib_perf<0>()
#else
	#define REGISTER_PATHFINDERLIB_PERF()
#endif

enum Enum_PathFinderLib_Perf
{
	enum_PathFinderLib_start,

	// KAStar
	KAStar_KAStar,
	KAStar_D_KAStar,
	KAStar_ReleaseNodes,
	KAStar_GetBestNodeStepNum,
	KAStar_GeneratePath,
	KAStar_StepInitialize,
	KAStar_GetBestNodeFromStartNum,
	KAStar_Step,
	KAStar_CreateChildren,
	KAStar_LinkChild,
	KAStar_AddToOpen,
	KAStar_UpdateParents,
	KAStar_Push,
	KAStar2_KAStar2,
	KAStar2_D_KAStar2,
	KAStar2_ReleaseNodes,
	KAStar2_GeneratePath,
	KAStar2_StepInitialize,

	// KBStar
	KBStarIndependence_KBStarIndependence,
	KBStarIndependence_D_KBStarIndependence,
	KBStarIndependence_ReleaseNodes,
	KBStarIndependence_GeneratePath,
	KBStarIndependence__GoStraight,
	KBStarIndependence__TurnLeft,
	KBStarIndependence__TurnRight,
	KBStarIndependence__ClimbRight,
	KBStarIndependence__ClimbLeft,
	KBStarIndependence_GeneratePath_NoRelease,
	KBStarIndependence_StepInitialize,
	KBStarIndependence_clear_open,
	KBStarIndependence_add_to_open,
	KBStar_KBStar,
	KBStar_D_KBStar,
	KBStar_GeneratePath,
	KBStarComplex_KBStarComplex,
	KBStarComplex_D_KBStarComplex,
	KBStarComplex_SetMaxLoop,
	KBStarComplex_SetMaxLineLength,
	KBStarComplex_SetPlugin,
	KBStarComplex_GeneratePath,
	KBStarComplex_ReleaseNodes,

	// 
	_G_g_InternalDirSinCos,
	_G_g_DirSin,
	_G_g_DirCos,
	_G_g_GetDistance,
	_G_g_GetDirIndex,
	_G_g_GetDirDiff,
	_G_g_GetDirAngle,
	_G_g_Dir64To8,
	_G_g_Dir8To64,

	// KDirectAStar
	KDirectAStar_KDirectAStar,
	KDirectAStar_D_KDirectAStar,
	KDirectAStar_ReleaseNodes,
	KDirectAStar_GeneratePath,
	KDirectAStar_StepInitialize,

	// IAlgorithmHash
	IAlgorithmHash_ReleaseNodes,
	IAlgorithmHash_add_to_open,
	IAlgorithmHash_erase_from_open,
	IAlgorithmHash_add_to_close,
	IAlgorithmHash_erase_from_close,

	// KL2Map
	KL2Map_Base_KL2Map_Base,
	KL2Map_Base_D_KL2Map_Base,
	KL2Map_Base_FindPath,
	KL2Map_Base_ComputePtListWithDistance,
	KL2Map_Base_OffsetMap,
	KL2Map_Base_GetJointBlockPos,
	KL2Map_Base_RecomputeJointBlock,
	KL2Map_Base_AddOneJoint,
	KL2Map_Base_CreateOneRoad,
	KL2Map_Base__InsertJointToBlock,

	// ...

	enum_PathFinderLib_perf_end,
};

template <size_t x> void register_PathFinderLib_perf()
{
#if defined(DEF_PERF_HITCH_COUNTER)
	KPerfHitchIns& perfStat = KPerfHitchIns::GetInstance();
#else
	KPerfStat& perfStat = KPerfStat::GetInstance();
#endif
	perfStat.Register(KAStar_KAStar, "KAStar::KAStar");
	perfStat.Register(KAStar_D_KAStar, "KAStar::~KAStar");
	perfStat.Register(KAStar_ReleaseNodes, "KAStar::ReleaseNodes");
	perfStat.Register(KAStar_GetBestNodeStepNum, "KAStar::GetBestNodeStepNum");
	perfStat.Register(KAStar_GeneratePath, "KAStar::GeneratePath");
	perfStat.Register(KAStar_StepInitialize, "KAStar::StepInitialize");
	perfStat.Register(KAStar_GetBestNodeFromStartNum, "KAStar::GetBestNodeFromStartNum");
	perfStat.Register(KAStar_Step, "KAStar::Step");
	perfStat.Register(KAStar_CreateChildren, "KAStar::CreateChildren");
	perfStat.Register(KAStar_LinkChild, "KAStar::LinkChild");
	perfStat.Register(KAStar_AddToOpen, "KAStar::AddToOpen");
	perfStat.Register(KAStar_UpdateParents, "KAStar::UpdateParents");
	perfStat.Register(KAStar_Push, "KAStar::Push");
	perfStat.Register(KAStar2_KAStar2, "KAStar2::KAStar2");
	perfStat.Register(KAStar2_D_KAStar2, "KAStar2::~KAStar2");
	perfStat.Register(KAStar2_ReleaseNodes, "KAStar2::ReleaseNodes");
	perfStat.Register(KAStar2_GeneratePath, "KAStar2::GeneratePath");
	perfStat.Register(KAStar2_StepInitialize, "KAStar2::StepInitialize");

	perfStat.Register(KBStarIndependence_KBStarIndependence, "KBStarIndependence::KBStarIndependence");
	perfStat.Register(KBStarIndependence_D_KBStarIndependence, "KBStarIndependence::~KBStarIndependence");
	perfStat.Register(KBStarIndependence_ReleaseNodes, "KBStarIndependence::ReleaseNodes");
	perfStat.Register(KBStarIndependence_GeneratePath, "KBStarIndependence::GeneratePath");
	perfStat.Register(KBStarIndependence__GoStraight, "KBStarIndependence::_GoStraight");
	perfStat.Register(KBStarIndependence__TurnLeft, "KBStarIndependence::_TurnLeft");
	perfStat.Register(KBStarIndependence__TurnRight, "KBStarIndependence::_TurnRight");
	perfStat.Register(KBStarIndependence__ClimbRight, "KBStarIndependence::_ClimbRight");
	perfStat.Register(KBStarIndependence__ClimbLeft, "KBStarIndependence::_ClimbLeft");
	perfStat.Register(KBStarIndependence_GeneratePath_NoRelease, "KBStarIndependence::GeneratePath_NoRelease");
	perfStat.Register(KBStarIndependence_StepInitialize, "KBStarIndependence::StepInitialize");
	perfStat.Register(KBStarIndependence_clear_open, "KBStarIndependence::clear_open");
	perfStat.Register(KBStarIndependence_add_to_open, "KBStarIndependence::add_to_open");
	perfStat.Register(KBStar_KBStar, "KBStar::KBStar");
	perfStat.Register(KBStar_D_KBStar, "KBStar::~KBStar");
	perfStat.Register(KBStar_GeneratePath, "KBStar::GeneratePath");
	perfStat.Register(KBStarComplex_KBStarComplex, "KBStarComplex::KBStarComplex");
	perfStat.Register(KBStarComplex_D_KBStarComplex, "KBStarComplex::~KBStarComplex");
	perfStat.Register(KBStarComplex_SetMaxLoop, "KBStarComplex::SetMaxLoop");
	perfStat.Register(KBStarComplex_SetMaxLineLength, "KBStarComplex::SetMaxLineLength");
	perfStat.Register(KBStarComplex_SetPlugin, "KBStarComplex::SetPlugin");
	perfStat.Register(KBStarComplex_GeneratePath, "KBStarComplex::GeneratePath");
	perfStat.Register(KBStarComplex_ReleaseNodes, "KBStarComplex::ReleaseNodes");

	perfStat.Register(_G_g_InternalDirSinCos, "g_InternalDirSinCos");
	perfStat.Register(_G_g_DirSin, "g_DirSin");
	perfStat.Register(_G_g_DirCos, "g_DirCos");
	perfStat.Register(_G_g_GetDistance, "g_GetDistance");
	perfStat.Register(_G_g_GetDirIndex, "g_GetDirIndex");
	perfStat.Register(_G_g_GetDirDiff, "g_GetDirDiff");
	perfStat.Register(_G_g_GetDirAngle, "g_GetDirAngle");
	perfStat.Register(_G_g_Dir64To8, "g_Dir64To8");
	perfStat.Register(_G_g_Dir8To64, "g_Dir8To64");

	perfStat.Register(KDirectAStar_KDirectAStar, "KDirectAStar::KDirectAStar");
	perfStat.Register(KDirectAStar_D_KDirectAStar, "KDirectAStar::~KDirectAStar");
	perfStat.Register(KDirectAStar_ReleaseNodes, "KDirectAStar::ReleaseNodes");
	perfStat.Register(KDirectAStar_GeneratePath, "KDirectAStar::GeneratePath");
	perfStat.Register(KDirectAStar_StepInitialize, "KDirectAStar::StepInitialize");

	perfStat.Register(IAlgorithmHash_ReleaseNodes, "IAlgorithmHash::ReleaseNodes");
	perfStat.Register(IAlgorithmHash_add_to_open, "IAlgorithmHash::add_to_open");
	perfStat.Register(IAlgorithmHash_erase_from_open, "IAlgorithmHash::erase_from_open");
	perfStat.Register(IAlgorithmHash_add_to_close, "IAlgorithmHash::add_to_close");
	perfStat.Register(IAlgorithmHash_erase_from_close, "IAlgorithmHash::erase_from_close");

	perfStat.Register(KL2Map_Base_KL2Map_Base, "KL2Map_Base::KL2Map_Base");
	perfStat.Register(KL2Map_Base_D_KL2Map_Base, "KL2Map_Base::~KL2Map_Base");
	perfStat.Register(KL2Map_Base_FindPath, "KL2Map_Base::FindPath");
	perfStat.Register(KL2Map_Base_ComputePtListWithDistance, "KL2Map_Base::ComputePtListWithDistance");
	perfStat.Register(KL2Map_Base_OffsetMap, "KL2Map_Base::OffsetMap");
	perfStat.Register(KL2Map_Base_GetJointBlockPos, "KL2Map_Base::GetJointBlockPos");
	perfStat.Register(KL2Map_Base_RecomputeJointBlock, "KL2Map_Base::RecomputeJointBlock");
	perfStat.Register(KL2Map_Base_AddOneJoint, "KL2Map_Base::AddOneJoint");
	perfStat.Register(KL2Map_Base_CreateOneRoad, "KL2Map_Base::CreateOneRoad");
	perfStat.Register(KL2Map_Base__InsertJointToBlock, "KL2Map_Base::_InsertJointToBlock");

	// .....
}
