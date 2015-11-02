#include "ThirdpartyLib.h"
#include "../Lua/lua.h"
#include "../Lua/lauxlib.h"

static const luaL_Reg luaThirdpartlibs[] = {
	{LUA_PACKNAME, luaopen_pack},
	{LUA_BITOPERATENAME, luaopen_bitOper},
	{NULL, NULL}
};

LUALIB_API void luaL_openThirdpartlibs(lua_State *L)
{
	const luaL_Reg *lib = luaThirdpartlibs;
	for (; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_pushstring(L, lib->name);
		lua_call(L, 1, 0);
	}
}