#ifndef _TMP_PLAYER_SESSIONS_H_
#define _TMP_PLAYER_SESSIONS_H_

#include <System/KPlatform.h>
#include <lua/KLuaWrap.h>

enum 
{
	world_server_tmp_session_length = 1024,
};

//----------------------------------------------------------------
// 玩家登陆或者切服时，从GameServer传递给WorldServer的SessionKey
// 在这里定义
//----------------------------------------------------------------

const DWORD tmp_player_s_take_bus		= 1;
const DWORD tmp_player_s_carriee_tc		= 2;
const DWORD tmp_player_s_summon_monster	= 3;
const DWORD tmp_player_s_force_position	= 4;

inline void export_tmpPlayerSessions_to_lua()
{
	export_const_to_lua(tmp_player_s_take_bus);
	export_const_to_lua(tmp_player_s_carriee_tc);
	export_const_to_lua(tmp_player_s_summon_monster);
	export_const_to_lua(tmp_player_s_force_position);
}

struct s_take_bus
{
	int busLine;
	DWORD coupleID;
};

struct s_carriee_tc
{
	DWORD tcType;
};

struct s_summon_monster
{
	DWORD id;
	int level;
	int hp, mp;
};

struct s_force_position
{
	float x, y, z;
};

#endif
