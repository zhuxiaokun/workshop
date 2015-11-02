#ifndef _K_AS_EVENTS_H_
#define _K_AS_EVENTS_H_

const int evt_AS_adapter_begin    = 100000;
const int evt_AS_userdefine_begin = 200000;

/// event consts ///

enum enum_AS_ServerEvent
{
	evt_AS_server_begin = 0,
	// ...
	evt_AS_server_end,
};

enum enum_AS_WorldObjEvent
{
	evt_AS_worldObj_begin = evt_AS_server_end,

	evt_AS_worldObj_enterVillage,
	evt_AS_worldObj_leaveVillage,

	evt_AS_worldObj_end,
};

enum enum_AS_CharEvent
{
	evt_AS_char_begin = evt_AS_worldObj_end,

	// skill
	// level
	evt_AS_char_dead = evt_AS_char_begin,	// 角色死亡
	evt_AS_char_kill,						// 击杀
	evt_AS_char_killed,						// 被击杀

	evt_AS_char_end,
};

enum enum_AS_PlayerEvent
{
	evt_AS_player_begin = evt_AS_char_end,
	// life skill
	// open box / carry box
	// ...
	evt_AS_player_end,
};

enum enum_AS_NpcEvent
{
	evt_AS_npc_begin = evt_AS_player_end,
	// ...
	evt_AS_npc_end,
};

enum enum_AS_TcEvent
{
	evt_AS_tc_begin = evt_AS_npc_end,
	// ...
	evt_AS_tc_end,
};

enum enum_AS_WorldEvent
{
	evt_AS_world_begin = evt_AS_tc_end,
	// ...
	evt_AS_world_over = evt_AS_world_begin,	// 副本结束了，成功或失败
	evt_AS_world_worldObjEnter,
	evt_AS_world_worldObjLeave,

	evt_AS_world_end,
};

enum enum_AS_ActivityEvent
{
	evt_AS_activity_begin = evt_AS_world_end,

	evt_AS_activity_syncState = evt_AS_activity_begin,
	evt_AS_activity_start,
	evt_AS_activity_stop,
	evt_AS_activity_invite,
	evt_AS_activity_reload,
	evt_AS_activity_UIMsg,

	evt_AS_activity_end,
};

enum enum_AS_HateListEvent
{
	evt_hatelist_begin = evt_AS_activity_end,
	evt_hatelist_add,
	evt_hatelist_del,
	evt_hatelist_update,
	evt_hatelist_end,
};

enum enum_AS_BeHateListEvent
{
	evt_behatelist_begin = evt_hatelist_end,
	evt_behatelist_add = evt_behatelist_begin,
	evt_behatelist_del,
	evt_behatelist_end,
};

//// event adapter 
enum evt_AS_AdapterEvent
{
	evt_AS_adapter_start = evt_AS_adapter_begin,

	evt_AS_player_enterVillage,	// enter or leave village
	evt_AS_player_leaveVillage,
	evt_AS_npc_enterVillage,
	evt_AS_npc_leaveVillage,
	evt_AS_tc_enterVillage,
	evt_AS_tc_leaveVillage,

	evt_AS_world_playerEnter,		// enter or leave world
	evt_AS_world_playerLeave,
	evt_AS_world_npcEnter,
	evt_AS_world_npcLeave,
	evt_AS_world_tcEnter,
	evt_AS_world_tcLeave,

	evt_AS_player_dead,			// dead
	evt_AS_npc_dead,
	evt_AS_pet_dead,

	evt_AS_adapter_end,
};

template <int x> void export_aiserver_event_to_lua()
{
	// some constant
	export_enum_to_lua(evt_AS_adapter_begin);
	export_enum_to_lua(evt_AS_userdefine_begin);

	// game server
	export_enum_to_lua(evt_AS_server_begin);
	export_enum_to_lua(evt_AS_server_end);

	// world obj event
	export_enum_to_lua(evt_AS_worldObj_begin);
	export_enum_to_lua(evt_AS_worldObj_enterVillage);
	export_enum_to_lua(evt_AS_worldObj_leaveVillage);
	export_enum_to_lua(evt_AS_worldObj_end);

	// character event
	export_enum_to_lua(evt_AS_char_begin);
	export_enum_to_lua(evt_AS_char_dead);
	export_enum_to_lua(evt_AS_char_kill);
	export_enum_to_lua(evt_AS_char_killed);
	export_enum_to_lua(evt_AS_char_end);

	// player event
	export_enum_to_lua(evt_AS_player_begin);
	export_enum_to_lua(evt_AS_player_end);

	// npc event
	export_enum_to_lua(evt_AS_npc_begin);
	export_enum_to_lua(evt_AS_npc_end);

	// tc event
	export_enum_to_lua(evt_AS_tc_begin);
	export_enum_to_lua(evt_AS_tc_end);

	// world event
	export_enum_to_lua(evt_AS_world_begin);
	export_enum_to_lua(evt_AS_world_over);
	export_enum_to_lua(evt_AS_world_worldObjEnter);
	export_enum_to_lua(evt_AS_world_worldObjLeave);
	export_enum_to_lua(evt_AS_world_end);

	// activity
	export_enum_to_lua(evt_AS_activity_begin);
	export_enum_to_lua(evt_AS_activity_syncState);
	export_enum_to_lua(evt_AS_activity_start);
	export_enum_to_lua(evt_AS_activity_stop);
	export_enum_to_lua(evt_AS_activity_invite);
	export_enum_to_lua(evt_AS_activity_UIMsg);
	export_enum_to_lua(evt_AS_activity_end);

	// adapter output
	export_enum_to_lua(evt_AS_adapter_start);
	export_enum_to_lua(evt_AS_player_enterVillage);
	export_enum_to_lua(evt_AS_player_leaveVillage);
	export_enum_to_lua(evt_AS_npc_enterVillage);
	export_enum_to_lua(evt_AS_npc_leaveVillage);
	export_enum_to_lua(evt_AS_tc_enterVillage);
	export_enum_to_lua(evt_AS_tc_leaveVillage);
	export_enum_to_lua(evt_AS_world_playerEnter);
	export_enum_to_lua(evt_AS_world_playerLeave);
	export_enum_to_lua(evt_AS_world_npcEnter);
	export_enum_to_lua(evt_AS_world_npcLeave);
	export_enum_to_lua(evt_AS_world_tcEnter);
	export_enum_to_lua(evt_AS_world_tcLeave);
	export_enum_to_lua(evt_AS_player_dead);
	export_enum_to_lua(evt_AS_npc_dead);
	export_enum_to_lua(evt_AS_pet_dead);
	export_enum_to_lua(evt_AS_adapter_end);

	// hate list
	export_enum_to_lua(evt_hatelist_begin);
	export_enum_to_lua(evt_hatelist_add);
	export_enum_to_lua(evt_hatelist_del);
	export_enum_to_lua(evt_hatelist_update);
	export_enum_to_lua(evt_hatelist_end);

	// behate list
	export_enum_to_lua(evt_behatelist_begin);
	export_enum_to_lua(evt_behatelist_add);
	export_enum_to_lua(evt_behatelist_del);
	export_enum_to_lua(evt_behatelist_end);
}

#define export_AS_events() export_aiserver_event_to_lua<0>();

#endif // _K_AS_EVENTS_H_
