#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define luac_c
#ifndef LUA_CORE
#define LUA_CORE
#endif

#include "Lua/lua.h"
#include "Lua/lauxlib.h"

#include "Lua/ldo.h"
#include "Lua/lfunc.h"
#include "Lua/lmem.h"
#include "Lua/lobject.h"
#include "Lua/lopcodes.h"
#include "Lua/lstring.h"
#include "Lua/lundump.h"
#include "Lua/lfunc.h"
#include "luacompiler.h"

#define	IS(s)	(strcmp(argv[i],s)==0)
#define toproto(L,i) (clvalue(L->top+(i))->l.p)

const Proto* combine(lua_State* L, int n)
{
	if (n==1)
		return toproto(L,-1);
	else
	{
		int i,pc;
		Proto* f=luaF_newproto(L);
		setptvalue2s(L,L->top,f); incr_top(L);
		f->source=luaS_newliteral(L,"=(lua)");
		f->maxstacksize=1;
		pc=2*n+1;
		f->code=luaM_newvector(L,pc,Instruction);
		f->sizecode=pc;
		f->p=luaM_newvector(L,n,Proto*);
		f->sizep=n;
		pc=0;
		for (i=0; i<n; i++)
		{
			f->p[i]=toproto(L,i-n-1);
			f->code[pc++]=CREATE_ABx(OP_CLOSURE,0,i);
			f->code[pc++]=CREATE_ABC(OP_CALL,0,1,1);
		}
		f->code[pc++]=CREATE_ABC(OP_RETURN,0,1,0);
		return f;
	}
}

int writer(lua_State* L, const void* p, size_t size, void* u)
{
	UNUSED(L);
	return (fwrite(p,size,1,(FILE*)u)!=1) && (size!=0);
}



const char* luaC_complie(lua_State* L, const char* szSrcFile, const char* szDesFile)
{
	const Proto* f;
	FILE* D;
	if (luaL_loadfile(L,szSrcFile)!=0)
	{
		return lua_tostring(L, -1);
	}
	f = combine(L, 1);


	D = fopen_k(szDesFile, "wb");
	lua_lock(L);
	luaU_dump(L, f, writer, D, 0);
	lua_unlock(L);
	fclose(D);
	
	return 0;
}

const char* luaC_complieN(lua_State* L, const char* szDesFile, int count)
{
	const Proto* f;
	FILE* D;
	if (!lua_checkstack(L, count))
	{
		return "too much files, can not compile once";
	}
	f = combine(L, count);


	D = fopen_k(szDesFile, "wb");
	lua_lock(L);
	luaU_dump(L, f, writer, D, 0);
	lua_unlock(L);
	fclose(D);

	return 0;
}