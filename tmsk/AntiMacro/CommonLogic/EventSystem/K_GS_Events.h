#ifndef _K_GS_EVENTS_H_
#define _K_GS_EVENTS_H_

const int evt_GS_adapter_begin    = 100000;
const int evt_GS_userdefine_begin = 200000;

/// event consts ///

enum enum_GS_ServerEvent
{
	evt_GS_server_begin = 0,
	// AI login/close
	// ...
	evt_GS_createWorld,
	evt_GS_destroyWorld,

	evt_GS_server_end,
};

enum enum_GS_WorldObjEvent
{
	evt_GS_worldObj_begin = evt_GS_server_end,

	evt_GS_worldObj_enterVillage,
	evt_GS_worldObj_leaveVillage,

	evt_GS_worldObj_end,
};

enum enum_GS_CharEvent
{
	evt_GS_char_begin = evt_GS_worldObj_end,

	// skill
	// level
	evt_GS_char_dead = evt_GS_char_begin,	// 角色死亡
	evt_GS_char_kill,						// 击杀
	evt_GS_char_killed,						// 被击杀

	evt_GS_char_enterFight,					// 进入战斗状态
	evt_GS_char_leaveFight,					// 离开战斗状态

	evt_GS_char_addBuff,
	evt_GS_char_delBuff,

	evt_GS_char_relive,
	evt_GS_char_useSkill,
	evt_GS_char_beEffect,
	evt_GS_char_talk,
	evt_GS_char_betalk,

	evt_GS_char_end,
};

enum enum_GS_PlayerEvent
{
	evt_GS_player_begin = evt_GS_char_end,
	// life skill
	// open box / carry box
	evt_GS_Player_UIAction,
	evt_GS_player_end,
};

enum enum_GS_NpcEvent
{
	evt_GS_npc_begin = evt_GS_player_end,
	// ...
	evt_GS_npc_restore,
	evt_GS_npc_end,
};

enum enum_GS_TcEvent
{
	evt_GS_tc_begin = evt_GS_npc_end,
	// ...
	evt_GS_tc_end,
};

enum enum_GS_WorldEvent
{
	evt_GS_world_begin = evt_GS_tc_end,
	// ...
	evt_GS_world_over = evt_GS_world_begin,	// 副本结束了，成功或失败
	evt_GS_world_worldObjEnter,
	evt_GS_world_worldObjLeave,

	evt_GS_world_UIAction, // an ui action which target is a world

	evt_GS_world_destroying, // 副本进入关闭流程

	evt_GS_world_end,
};

enum enum_GS_ActivityEvent
{
	evt_GS_activity_begin = evt_GS_world_end,

	evt_GS_activity_syncState = evt_GS_activity_begin,
	evt_GS_activity_start,
	evt_GS_activity_stop,
	evt_GS_activity_invite,
	evt_GS_activity_teleportPlayer,
	evt_GS_activity_reload,
	evt_GS_activity_notice, // notice from world server
	evt_GS_activity_UIAction,
	evt_GS_activity_restorePlayer,

	evt_GS_activity_end,
};

enum enum_GS_LuaPetEvent
{
	evt_GS_luapet_begin = evt_GS_activity_end,

	evt_GS_luapet_syncState = evt_GS_luapet_begin,
	evt_GS_luapet_reload,

	evt_GS_luapet_end,
};
//// event adapter 
enum evt_GS_AdapterEvent
{
	evt_GS_adapter_start = evt_GS_adapter_begin,	

	evt_GS_player_enterVillage,	// enter or leave village
	evt_GS_player_leaveVillage,
	evt_GS_npc_enterVillage,
	evt_GS_npc_leaveVillage,
	evt_GS_tc_enterVillage,
	evt_GS_tc_leaveVillage,

	evt_GS_world_playerEnter,		// enter or leave world
	evt_GS_world_playerLeave,
	evt_GS_world_npcEnter,
	evt_GS_world_npcLeave,
	evt_GS_world_tcEnter,
	evt_GS_world_tcLeave,

	evt_GS_player_dead,			// dead
	evt_GS_npc_dead,
	evt_GS_pet_dead,

	evt_GS_player_kill,			// kill
	evt_GS_npc_kill,
	evt_GS_pet_kill,

	evt_GS_player_killed,			// dead
	evt_GS_npc_killed,
	evt_GS_pet_killed,

	evt_GS_player_enterFight,
	evt_GS_npc_enterFight,
	evt_GS_pet_enterFight,

	evt_GS_player_leaveFight,
	evt_GS_npc_leaveFight,
	evt_GS_pet_leaveFight,

	evt_GS_player_addBuff,			// add buff
	evt_GS_npc_addBuff,
	evt_GS_pet_addBuff,

	evt_GS_player_delBuff,			// del buff
	evt_GS_npc_delBuff,
	evt_GS_pet_delBuff,

	evt_GS_player_relive,			// relive
	evt_GS_npc_relive,
	evt_GS_pet_relive,

	evt_GS_player_useSkill,			// use Skill
	evt_GS_npc_useSkill,
	evt_GS_pet_useSkill,

	evt_GS_player_beEffect,			// beEffect
	evt_GS_npc_beEffect,
	evt_GS_pet_beEffect,

	evt_GS_player_talk,				// talk
	evt_GS_npc_betalk,

	evt_GS_adapter_end,
};

enum enum_GS_TrafficEvent
{
	evt_GS_Traffic_begin = evt_GS_adapter_end,

	evt_GS_Traffic_reload = evt_GS_Traffic_begin,
	evt_GS_Traffic_lineup,

	evt_GS_Traffic_end,
};

template <int x> void export_gameserver_event_to_lua()
{
	// some constant
	export_enum_to_lua(evt_GS_adapter_begin);
	export_enum_to_lua(evt_GS_userdefine_begin);

	// game server
	export_enum_to_lua(evt_GS_server_begin);
	export_enum_to_lua(evt_GS_createWorld);
	export_enum_to_lua(evt_GS_destroyWorld);
	export_enum_to_lua(evt_GS_server_end);

	// world obj event
	export_enum_to_lua(evt_GS_worldObj_begin);
	export_enum_to_lua(evt_GS_worldObj_enterVillage);
	export_enum_to_lua(evt_GS_worldObj_leaveVillage);
	export_enum_to_lua(evt_GS_worldObj_end);


	// character event
	export_enum_to_lua(evt_GS_char_begin);
	export_enum_to_lua(evt_GS_char_dead);
	export_enum_to_lua(evt_GS_char_kill);
	export_enum_to_lua(evt_GS_char_killed);
	export_enum_to_lua(evt_GS_char_enterFight);
	export_enum_to_lua(evt_GS_char_leaveFight);
	export_enum_to_lua(evt_GS_char_addBuff);
	export_enum_to_lua(evt_GS_char_delBuff);
	export_enum_to_lua(evt_GS_char_relive);
	export_enum_to_lua(evt_GS_char_useSkill);
	export_enum_to_lua(evt_GS_char_beEffect);
	export_enum_to_lua(evt_GS_char_talk);
	export_enum_to_lua(evt_GS_char_betalk);
	export_enum_to_lua(evt_GS_char_end);

	// player event
	export_enum_to_lua(evt_GS_player_begin);
	export_enum_to_lua(evt_GS_Player_UIAction);
	export_enum_to_lua(evt_GS_player_end);

	// npc event
	export_enum_to_lua(evt_GS_npc_begin);
	export_enum_to_lua(evt_GS_npc_restore);
	export_enum_to_lua(evt_GS_npc_end);

	// tc event
	export_enum_to_lua(evt_GS_tc_begin);
	export_enum_to_lua(evt_GS_tc_end);

	// world event
	export_enum_to_lua(evt_GS_world_begin);
	export_enum_to_lua(evt_GS_world_over);
	export_enum_to_lua(evt_GS_world_worldObjEnter);
	export_enum_to_lua(evt_GS_world_worldObjLeave);
	export_enum_to_lua(evt_GS_world_UIAction);
	export_enum_to_lua(evt_GS_world_destroying);
	export_enum_to_lua(evt_GS_world_end);

	// activity
	export_enum_to_lua(evt_GS_activity_begin);
	export_enum_to_lua(evt_GS_activity_syncState);
	export_enum_to_lua(evt_GS_activity_start);
	export_enum_to_lua(evt_GS_activity_stop);
	export_enum_to_lua(evt_GS_activity_reload);
	export_enum_to_lua(evt_GS_activity_teleportPlayer);
	export_enum_to_lua(evt_GS_activity_invite);
	export_enum_to_lua(evt_GS_activity_notice);
	export_enum_to_lua(evt_GS_activity_UIAction);
	export_enum_to_lua(evt_GS_activity_restorePlayer);
	export_enum_to_lua(evt_GS_activity_end);

	// adapter output
	export_enum_to_lua(evt_GS_adapter_start);
	export_enum_to_lua(evt_GS_player_enterVillage);
	export_enum_to_lua(evt_GS_player_leaveVillage);
	export_enum_to_lua(evt_GS_npc_enterVillage);
	export_enum_to_lua(evt_GS_npc_leaveVillage);
	export_enum_to_lua(evt_GS_tc_enterVillage);
	export_enum_to_lua(evt_GS_tc_leaveVillage);
	export_enum_to_lua(evt_GS_world_playerEnter);
	export_enum_to_lua(evt_GS_world_playerLeave);
	export_enum_to_lua(evt_GS_world_npcEnter);
	export_enum_to_lua(evt_GS_world_npcLeave);
	export_enum_to_lua(evt_GS_world_tcEnter);
	export_enum_to_lua(evt_GS_world_tcLeave);
	export_enum_to_lua(evt_GS_player_dead);
	export_enum_to_lua(evt_GS_npc_dead);
	export_enum_to_lua(evt_GS_pet_dead);
	export_enum_to_lua(evt_GS_player_kill);
	export_enum_to_lua(evt_GS_npc_kill);
	export_enum_to_lua(evt_GS_pet_kill);
	export_enum_to_lua(evt_GS_player_killed);
	export_enum_to_lua(evt_GS_npc_killed);
	export_enum_to_lua(evt_GS_pet_killed);
	export_enum_to_lua(evt_GS_player_enterFight);
	export_enum_to_lua(evt_GS_npc_enterFight);
	export_enum_to_lua(evt_GS_pet_enterFight);
	export_enum_to_lua(evt_GS_player_leaveFight);
	export_enum_to_lua(evt_GS_npc_leaveFight);
	export_enum_to_lua(evt_GS_pet_leaveFight);
	export_enum_to_lua(evt_GS_player_addBuff);
	export_enum_to_lua(evt_GS_npc_addBuff);
	export_enum_to_lua(evt_GS_pet_addBuff);
	export_enum_to_lua(evt_GS_player_delBuff);
	export_enum_to_lua(evt_GS_npc_delBuff);
	export_enum_to_lua(evt_GS_pet_delBuff);
	export_enum_to_lua(evt_GS_player_relive);
	export_enum_to_lua(evt_GS_npc_relive);
	export_enum_to_lua(evt_GS_pet_relive);
	export_enum_to_lua(evt_GS_player_useSkill);
	export_enum_to_lua(evt_GS_npc_useSkill);
	export_enum_to_lua(evt_GS_pet_useSkill);
	export_enum_to_lua(evt_GS_player_beEffect);
	export_enum_to_lua(evt_GS_npc_beEffect);
	export_enum_to_lua(evt_GS_pet_beEffect);
	export_enum_to_lua(evt_GS_player_talk);
	export_enum_to_lua(evt_GS_npc_betalk);
	export_enum_to_lua(evt_GS_adapter_end);

	// trafficstation
	export_enum_to_lua(evt_GS_Traffic_begin);
	export_enum_to_lua(evt_GS_Traffic_reload);
	export_enum_to_lua(evt_GS_Traffic_lineup);
	export_enum_to_lua(evt_GS_Traffic_end);
}

#define export_GS_events() export_gameserver_event_to_lua<0>();

#endif
