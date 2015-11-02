#ifndef ThirdpartyLib_h
#define ThirdpartyLib_h

#include "../Lua/lua.h"

/// 用作格式数据打包成连续数据
#define LUA_PACKNAME	"Pack"
LUALIB_API int (luaopen_pack) (lua_State *L);

// 用作Lua下的位运算的替代解决方案
#define LUA_BITOPERATENAME	"BitOper"
LUALIB_API int (luaopen_bitOper) (lua_State *L);

/* open all Thirdpart libraries */
LUALIB_API void (luaL_openThirdpartlibs) (lua_State *L); 

#endif