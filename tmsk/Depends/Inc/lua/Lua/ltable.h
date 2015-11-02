/*
** $Id: ltable.h,v 1.1 2009/06/16 07:19:50 JIGUANG\seacat Exp $
** Lua tables (hash)
** See Copyright Notice in lua.h
*/

#ifndef ltable_h
#define ltable_h

#include "lobject.h"


#define gnode(t,i)	(&(t)->node[i])
#define gkey(n)		(&(n)->i_key.nk)
#define gval(n)		(&(n)->i_val)
#define gnext(n)	((n)->i_key.nk.next)

#define key2tval(n)	(&(n)->i_key.tvk)


LUAI_FUNC const TValue *luaH_getnum (Table *t, int key);
LUAI_FUNC TValue *luaH_setnum (lua_State *L, Table *t, int key);
LUAI_FUNC const TValue *luaH_getstr (Table *t, KLuaString *key);
LUAI_FUNC TValue *luaH_setstr (lua_State *L, Table *t, KLuaString *key);
LUAI_FUNC const TValue *luaH_get (Table *t, const TValue *key);
LUAI_FUNC TValue *luaH_set (lua_State *L, Table *t, const TValue *key);
LUAI_FUNC Table *luaH_new (lua_State *L, int narray, int lnhash);
LUAI_FUNC void luaH_resizearray (lua_State *L, Table *t, int nasize);
LUAI_FUNC void luaH_free (lua_State *L, Table *t);
LUAI_FUNC int luaH_next (lua_State *L, Table *t, StkId key);
LUAI_FUNC int luaH_getn (Table *t);


#if defined(LUA_DEBUG)
LUAI_FUNC LuaNode *luaH_mainposition (const Table *t, const TValue *key);
LUAI_FUNC int luaH_isdummy (LuaNode *n);
#endif


#endif
