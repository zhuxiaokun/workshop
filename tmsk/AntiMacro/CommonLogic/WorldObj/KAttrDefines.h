#pragma once

//
// 基本状态属性，内部使用，非策划需求
//
enum KCharAttr_0
{
	cac0_Start = 0,
	cac0_walkType = cac0_Start,			// 0 走 1 跑
	cac0_moveSpeed,						// 当前移动速度
	cac0_End,
};

enum KCharAttr_1
{
	cac1_Start = 1001,
	//cac1_GCD = cac1_Start,				//基础攻击间隔(公冷)
	//cac1_HPRecover,						//生命回复值
	//cac1_HPRecoverRate,					//生命回复比例
	//cac1_MPRecover,						//法力回复值
	//cac1_MPRecoverRate,					//法力回复比例
	//cac1_ACCExtra,						//额外命中率
	//cac1_DOGExtra,						//额外闪避率
	//cac1_DefIgnore,						//防御穿透值
	//cac1_DefIgnoreRate,					//防御穿透比例
	//cac1_DamageEnhanceRate,				//伤害增强比例
	//cac1_DamageReduceRate,				//伤害减免比例
	//cac1_DamageMaster,					//伤害精通(附加伤害值)
	//cac1_DamageReduce,					//伤害减免(伤害格挡值)
	//cac1_WaterMaster,					//水系精通
	//cac1_FireMaster,					//火系精通
	//cac1_WindMaster,					//风系精通
	//cac1_ThunderMaster,					//雷系精通
	//cac1_NegativeMaster,				//阴系精通
	//cac1_PositiveMaster,				//阳系精通
	//cac1_WaterReduce,					//水系抗性
	//cac1_FireReduce,					//火系抗性
	//cac1_WindReduce,					//风系抗性
	//cac1_ThunderReduce,					//雷系抗性
	//cac1_NegativeReduce,				//阴系抗性
	//cac1_PositiveReduce,				//阳系抗性
	//cac1_Critical,						//致命
	//cac1_Tenacious,						//坚韧
	//cac1_CRIExtra,						//额外致命几率
	//cac1_TENExtra,						//额外会心抵抗(坚韧几率)
	//cac1_CRIEffectEnhance,				//致命效果强化
	//cac1_CRIEffectReduce,				//致命效果减免
	//cac1_SlowReduce,					//减速时间缩短
	//cac1_FreezeReduce,					//冻结时间缩短
	//cac1_StunReduce,					//眩晕时间缩短
	//cac1_PalsyReduce,					//麻痹时间缩短
	//cac1_FearReduce,					//恐惧时间缩短
	//cac1_ComaReduce,					//睡眠时间缩短
	//cac1_SilenceReduce,					//沉默时间缩短
	//cac1_PoisonReduce,					//中毒时间缩短
	//cac1_BleedReduce,					//流血时间缩短
	//cac1_SealingReduce,					//封印时间缩短
	//cac1_ReserveReduce4,				//保留时间缩短4
	//cac1_ReserveReduce5,				//保留时间缩短5
	//cac1_ReserveReduce6,				//保留时间缩短6
	//cac1_ReserveReduce7,				//保留时间缩短7
	//cac1_ReserveReduce8,				//保留时间缩短8

	//cac1_AttackSpeed,					//攻击速度
	//cac1_AttackSpeedEnhance,			//攻击速度加成
	//cac1_SpellSpeed,					//施法速度
	//cac1_SpellSpeedEnhance,				//施法速度加成
	//cac1_MPCostReduce,					//法力消耗减少
	//cac1_MPCostReduceRate,				//法力消耗减少(比例)
	//cac1_SkillCDReduce,					//技能冷却减少
	//cac1_SkillCDReduceRate,				//技能冷却减少(比例)
	//cac1_SkillDisEnhance,				//技能作用距离增加
	//cac1_SkillDisEnhanceRate,			//技能作用距离增加(比例)

	//cac1_CureExtra,						//治疗能力强化
	//cac1_CuredExtra,					//承疗能力强化
	//cac1_CureExtraRate,					//治疗能力强化(比例)
	//cac1_CuredExtraRate,				//承疗能力强化(比例)
	//cac1_CureCRI,						//治疗会心几率
	//cac1_CuredCRI,						//承疗会心几率
	//cac1_CureCRIEffect,					//治疗会心效果
	//cac1_CuredCRIEffect,				//承疗会心效果
	//cac1_PBEffect,						//造成施法回退比率
	//cac1_PBResist,						//承受施法回退比率

	//cac1_Invincible,					//全免疫-无敌
	//cac1_PosEffectImmune,				//正面效果免疫
	//cac1_NegEffectImmune,				//负面效果免疫
	//cac1_WaterImmune,					//水伤害免疫
	//cac1_FireImmune,					//火伤害免疫
	//cac1_WindImmune,					//风伤害免疫
	//cac1_ThunderImmune,					//雷伤害免疫
	//cac1_NegativeImmune,				//阴伤害免疫
	//cac1_PositiveImmune,				//阳伤害免疫

	//cac1_SlowImmune,					//减速效果免疫
	//cac1_FreezeImmune,					//定身效果免疫
	//cac1_StunImmune,					//眩晕效果免疫
	//cac1_PalsyImmune,					//麻痹效果免疫
	//cac1_FearImmune,					//恐惧效果免疫
	//cac1_ComaImmune,					//昏迷效果免疫
	//cac1_SilenceImmune,					//沉默效果免疫
	//cac1_PoisionImmune,					//中毒效果免疫
	//cac1_BleedImmune,					//留血效果免疫
	//cac1_SealingImmune,					//封印效果免疫
	//cac1_KnockDownImmune,				//击倒效果免疫

	//cac1_ReserveImmune5,				//预留效果免疫5
	//cac1_ReserveImmune6,				//预留效果免疫6
	//cac1_ReserveImmune7,				//预留效果免疫7
	//cac1_ReserveImmune8,				//预留效果免疫8
	//cac1_PositiveDispel,				//增益驱散
	//cac1_PositiveDispelResist,			//增益驱散抵抗
	//cac1_NegativeDispel,				//减益驱散
	//cac1_NegativeDispelEnhance,			//减益驱散强化
	//cac1_HatredRate,					//仇恨倍数
	//cac1_ExpRate,						//打怪经验倍数
	//cac1_GlobalExpRate,					//全局经验倍数
	//cac1_DropRate,						//掉落倍数
	//cac1_HonorRate,						//荣誉获得倍数
	//cac1_DeathExpPunish,				//死亡经验损失
	//cac1_DurableCost,					//耐久消耗速度
	//cac1_Size,							//碰撞大小

	cac1_End,
};

enum KCharAttr_2
{
	cac2_Start = 2001,
	cac2_Strength = cac2_Start,			//力量
	//cac2_Agility,						//身法
	//cac2_Vitality,					//体质
	//cac2_Intelligence,				//灵力
	//cac2_Vigor,						//元气
	//cac2_GrowEnd = cac2_Vigor,		//成长属性完毕
	//cac2_Cure,						//治疗能力
	//cac2_Attack,						//攻击
	//cac2_Defence,						//防御
	//cac2_ACC,							//命中
	//cac2_DOG,							//闪避
	cac2_WalkSpeed,						//走路速度
	cac2_RunSpeed,						//跑步速度
	cac2_MHP,							//HP上限
	cac2_MMP,							//MP上限
	cac2_MRP,							//第三种能量上限

	cac2_End,
};

enum KCharAttr_6 // 基础存盘属性
{
	cac6_Start = 6001,
	cac6_Level = cac6_Start,		//等级
	cac6_Sex,						//性别 0 男 1 女 2 无性别
	cac6_Class,						//初级职业
	cac6_SubClass,					//进阶职业
	cac6_Force,						//势力
	cac6_End,
};
