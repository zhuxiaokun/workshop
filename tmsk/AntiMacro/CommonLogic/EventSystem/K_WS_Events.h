#ifndef _K_WS_EVENTS_H_
#define _K_WS_EVENTS_H_

enum enum_WorldServer_Event
{
	evt_WS_worldserver_begin,

	evt_WS_gameserver_connect,
	evt_WS_gameserver_close,

	evt_WS_account_enter_world,
	evt_WS_account_leave_world,

	evt_WS_player_enter_game,
	evt_WS_player_leave_game,

	evt_WS_player_enterGameReq,
	evt_WS_player_changeSceneReq,

	evt_WS_worldserver_end,
};

enum enum_Activity_Event
{
	evt_WS_activity_begin = evt_WS_worldserver_end,

	evt_WS_activity_syncState = evt_WS_activity_begin,
	evt_WS_activity_start,
	evt_WS_activity_stop,
	evt_WS_activity_invite,
	evt_WS_activity_reload,
	evt_WS_activity_notice, // notice from game server

	evt_WS_activity_isAllow,
	evt_WS_activity_invitePlayer,
	evt_WS_activity_getTimePeriod,
	evt_WS_activity_acceptPlayer,
	evt_WS_activity_getPlayer,

	evt_WS_activity_end,
};

template <int x> void export_world_evt_to_lua()
{
	// world server event
	export_enum_to_lua(evt_WS_worldserver_begin);
	export_enum_to_lua(evt_WS_gameserver_connect);
	export_enum_to_lua(evt_WS_gameserver_close);
	export_enum_to_lua(evt_WS_account_enter_world);
	export_enum_to_lua(evt_WS_account_leave_world);
	export_enum_to_lua(evt_WS_player_enter_game);
	export_enum_to_lua(evt_WS_player_leave_game);
	export_enum_to_lua(evt_WS_player_enterGameReq);
	export_enum_to_lua(evt_WS_player_changeSceneReq);
	export_enum_to_lua(evt_WS_worldserver_end);

	// activity
	export_enum_to_lua(evt_WS_activity_begin);
	export_enum_to_lua(evt_WS_activity_isAllow);
	export_enum_to_lua(evt_WS_activity_invitePlayer);
	export_enum_to_lua(evt_WS_activity_syncState);
	export_enum_to_lua(evt_WS_activity_start);
	export_enum_to_lua(evt_WS_activity_stop);
	export_enum_to_lua(evt_WS_activity_invite);
	export_enum_to_lua(evt_WS_activity_reload);
	export_enum_to_lua(evt_WS_activity_notice);
	export_enum_to_lua(evt_WS_activity_getTimePeriod);
	export_enum_to_lua(evt_WS_activity_acceptPlayer);
	export_enum_to_lua(evt_WS_activity_getPlayer);
	export_enum_to_lua(evt_WS_activity_end);
}

#define export_WS_events() export_world_evt_to_lua<0>();

#endif
