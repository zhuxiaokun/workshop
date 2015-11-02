#pragma once

//
// ����״̬���ԣ��ڲ�ʹ�ã��ǲ߻�����
//
enum KCharAttr_0
{
	cac0_Start = 0,
	cac0_walkType = cac0_Start,			// 0 �� 1 ��
	cac0_moveSpeed,						// ��ǰ�ƶ��ٶ�
	cac0_End,
};

enum KCharAttr_1
{
	cac1_Start = 1001,
	//cac1_GCD = cac1_Start,				//�����������(����)
	//cac1_HPRecover,						//�����ظ�ֵ
	//cac1_HPRecoverRate,					//�����ظ�����
	//cac1_MPRecover,						//�����ظ�ֵ
	//cac1_MPRecoverRate,					//�����ظ�����
	//cac1_ACCExtra,						//����������
	//cac1_DOGExtra,						//����������
	//cac1_DefIgnore,						//������͸ֵ
	//cac1_DefIgnoreRate,					//������͸����
	//cac1_DamageEnhanceRate,				//�˺���ǿ����
	//cac1_DamageReduceRate,				//�˺��������
	//cac1_DamageMaster,					//�˺���ͨ(�����˺�ֵ)
	//cac1_DamageReduce,					//�˺�����(�˺���ֵ)
	//cac1_WaterMaster,					//ˮϵ��ͨ
	//cac1_FireMaster,					//��ϵ��ͨ
	//cac1_WindMaster,					//��ϵ��ͨ
	//cac1_ThunderMaster,					//��ϵ��ͨ
	//cac1_NegativeMaster,				//��ϵ��ͨ
	//cac1_PositiveMaster,				//��ϵ��ͨ
	//cac1_WaterReduce,					//ˮϵ����
	//cac1_FireReduce,					//��ϵ����
	//cac1_WindReduce,					//��ϵ����
	//cac1_ThunderReduce,					//��ϵ����
	//cac1_NegativeReduce,				//��ϵ����
	//cac1_PositiveReduce,				//��ϵ����
	//cac1_Critical,						//����
	//cac1_Tenacious,						//����
	//cac1_CRIExtra,						//������������
	//cac1_TENExtra,						//������ĵֿ�(���ͼ���)
	//cac1_CRIEffectEnhance,				//����Ч��ǿ��
	//cac1_CRIEffectReduce,				//����Ч������
	//cac1_SlowReduce,					//����ʱ������
	//cac1_FreezeReduce,					//����ʱ������
	//cac1_StunReduce,					//ѣ��ʱ������
	//cac1_PalsyReduce,					//���ʱ������
	//cac1_FearReduce,					//�־�ʱ������
	//cac1_ComaReduce,					//˯��ʱ������
	//cac1_SilenceReduce,					//��Ĭʱ������
	//cac1_PoisonReduce,					//�ж�ʱ������
	//cac1_BleedReduce,					//��Ѫʱ������
	//cac1_SealingReduce,					//��ӡʱ������
	//cac1_ReserveReduce4,				//����ʱ������4
	//cac1_ReserveReduce5,				//����ʱ������5
	//cac1_ReserveReduce6,				//����ʱ������6
	//cac1_ReserveReduce7,				//����ʱ������7
	//cac1_ReserveReduce8,				//����ʱ������8

	//cac1_AttackSpeed,					//�����ٶ�
	//cac1_AttackSpeedEnhance,			//�����ٶȼӳ�
	//cac1_SpellSpeed,					//ʩ���ٶ�
	//cac1_SpellSpeedEnhance,				//ʩ���ٶȼӳ�
	//cac1_MPCostReduce,					//�������ļ���
	//cac1_MPCostReduceRate,				//�������ļ���(����)
	//cac1_SkillCDReduce,					//������ȴ����
	//cac1_SkillCDReduceRate,				//������ȴ����(����)
	//cac1_SkillDisEnhance,				//�������þ�������
	//cac1_SkillDisEnhanceRate,			//�������þ�������(����)

	//cac1_CureExtra,						//��������ǿ��
	//cac1_CuredExtra,					//��������ǿ��
	//cac1_CureExtraRate,					//��������ǿ��(����)
	//cac1_CuredExtraRate,				//��������ǿ��(����)
	//cac1_CureCRI,						//���ƻ��ļ���
	//cac1_CuredCRI,						//���ƻ��ļ���
	//cac1_CureCRIEffect,					//���ƻ���Ч��
	//cac1_CuredCRIEffect,				//���ƻ���Ч��
	//cac1_PBEffect,						//���ʩ�����˱���
	//cac1_PBResist,						//����ʩ�����˱���

	//cac1_Invincible,					//ȫ����-�޵�
	//cac1_PosEffectImmune,				//����Ч������
	//cac1_NegEffectImmune,				//����Ч������
	//cac1_WaterImmune,					//ˮ�˺�����
	//cac1_FireImmune,					//���˺�����
	//cac1_WindImmune,					//���˺�����
	//cac1_ThunderImmune,					//���˺�����
	//cac1_NegativeImmune,				//���˺�����
	//cac1_PositiveImmune,				//���˺�����

	//cac1_SlowImmune,					//����Ч������
	//cac1_FreezeImmune,					//����Ч������
	//cac1_StunImmune,					//ѣ��Ч������
	//cac1_PalsyImmune,					//���Ч������
	//cac1_FearImmune,					//�־�Ч������
	//cac1_ComaImmune,					//����Ч������
	//cac1_SilenceImmune,					//��ĬЧ������
	//cac1_PoisionImmune,					//�ж�Ч������
	//cac1_BleedImmune,					//��ѪЧ������
	//cac1_SealingImmune,					//��ӡЧ������
	//cac1_KnockDownImmune,				//����Ч������

	//cac1_ReserveImmune5,				//Ԥ��Ч������5
	//cac1_ReserveImmune6,				//Ԥ��Ч������6
	//cac1_ReserveImmune7,				//Ԥ��Ч������7
	//cac1_ReserveImmune8,				//Ԥ��Ч������8
	//cac1_PositiveDispel,				//������ɢ
	//cac1_PositiveDispelResist,			//������ɢ�ֿ�
	//cac1_NegativeDispel,				//������ɢ
	//cac1_NegativeDispelEnhance,			//������ɢǿ��
	//cac1_HatredRate,					//��ޱ���
	//cac1_ExpRate,						//��־��鱶��
	//cac1_GlobalExpRate,					//ȫ�־��鱶��
	//cac1_DropRate,						//���䱶��
	//cac1_HonorRate,						//������ñ���
	//cac1_DeathExpPunish,				//����������ʧ
	//cac1_DurableCost,					//�;������ٶ�
	//cac1_Size,							//��ײ��С

	cac1_End,
};

enum KCharAttr_2
{
	cac2_Start = 2001,
	cac2_Strength = cac2_Start,			//����
	//cac2_Agility,						//��
	//cac2_Vitality,					//����
	//cac2_Intelligence,				//����
	//cac2_Vigor,						//Ԫ��
	//cac2_GrowEnd = cac2_Vigor,		//�ɳ��������
	//cac2_Cure,						//��������
	//cac2_Attack,						//����
	//cac2_Defence,						//����
	//cac2_ACC,							//����
	//cac2_DOG,							//����
	cac2_WalkSpeed,						//��·�ٶ�
	cac2_RunSpeed,						//�ܲ��ٶ�
	cac2_MHP,							//HP����
	cac2_MMP,							//MP����
	cac2_MRP,							//��������������

	cac2_End,
};

enum KCharAttr_6 // ������������
{
	cac6_Start = 6001,
	cac6_Level = cac6_Start,		//�ȼ�
	cac6_Sex,						//�Ա� 0 �� 1 Ů 2 ���Ա�
	cac6_Class,						//����ְҵ
	cac6_SubClass,					//����ְҵ
	cac6_Force,						//����
	cac6_End,
};
