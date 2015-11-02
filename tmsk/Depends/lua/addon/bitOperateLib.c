/*
* bitOperateLib.c -- a Lua library for bit operate
* Created By Kuijiang.Li
*/

#include <string.h>

#include "../Lua/lauxlib.h"
#include "../Lua/lua.h"
#include "ThirdpartyLib.h"

static int l_bitand(lua_State *L)
{
	int opt1 = luaL_checkint(L, 1);
	int opt2 = luaL_checkint(L, 2);

	int result = opt1 & opt2;
	lua_pushnumber(L, result);
	return 1;
}

static int l_bitor(lua_State *L)
{
	int opt1 = luaL_checkint(L, 1);
	int opt2 = luaL_checkint(L, 2);

	int result = opt1 | opt2;
	lua_pushnumber(L, result);
	return 1;
}

static int l_bitnot(lua_State *L)
{
	int opt1 = luaL_checkint(L, 1);

	int result = ~opt1;
	lua_pushnumber(L, result);
	return 1;
}

static int l_bitxor(lua_State *L)
{
	int opt1 = luaL_checkint(L, 1);
	int opt2 = luaL_checkint(L, 2);

	int result = opt1 ^ opt2;
	lua_pushnumber(L, result);
	return 1;
}

/* }====================================================== */


static const luaL_Reg bit_funcs[] = {
	{"bitAnd", l_bitand},
	{"bitOr", l_bitor},
	{"bitNot", l_bitnot},
	{"bitXor", l_bitxor},
	{NULL, NULL}
};

LUALIB_API int  luaopen_bitOper (lua_State *L) {
	luaL_register(L, LUA_BITOPERATENAME, bit_funcs);
	return 1;
}