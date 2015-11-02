/*
* packlib.c -- a Lua library for packing and unpacking binary data
*/
/*
* Added some new codes and modified some of the existing codes to
* match perl templates. 
*
* A lua string packed as (len, string)
* c Signed char value
* C Unsigned char value
* s Signed short value
* S Unsigned short value
* i Signed integer value
* I Unsigned integer value
* l Signed long value
* L Unsigned long value
* f Single-precision float in the native format
* d Double-precision float in the native format
* o long long value
* O Unsigned long long value
* n lua_Number
* _ Separator
*
* feel free to add the mising modes.
* Ignacio Casta駉
*/


#include <string.h>

#include "../Lua/lauxlib.h"
#include "../Lua/lua.h"
#include "ThirdpartyLib.h"

//#ifndef LUA_REGISTRYINDEX
//#define lua_Number double
//#endif

static void badmode(lua_State *L, int c)
{
 char s[]="bad mode `?'";
 s[sizeof(s)-3]=c;
 luaL_argerror(L,1,s);
}

static size_t copy(lua_State *L, void *a, size_t n, const char *s, size_t i, size_t len)
{
 if (i+n>len) luaL_argerror(L,1,"input stringBuf too short");
 memcpy(a,s+i,n);
 return i+n;
}


static int l_unpack(lua_State *L) 		/* unpack(s,f,[init]) */
{
 const char *s=luaL_checkstring(L,1);
 const char *f=luaL_checkstring(L,2);
 size_t i=luaL_optint(L,3,1)-1;
 size_t len=lua_strlen(L,1);
// if(i >= len) 
//luaL_argerror(L,1,"input stringBuf too short");
 int n, num;

 num = 0;
 for (n=0; f[n]; n++)
 {
  int c=f[n];
  switch (c)
  {
  case 'p':
	  {
		  void * p;
		  i=copy(L,&p,sizeof(p),s,i,len);
		  lua_pushlightuserdata(L,p);
		  num++;
	  }
	  break;
   case 'B':
	   {
			int k;
			int factor = 1;
			int length = 0;
			for(k=n-1; k>=0; k--)
			{
				char cDigit = f[k];
				if(cDigit<'0' || cDigit>'9') break;
				length += ((int)cDigit - (int)'0') * factor;
				factor *= 10;
			}
			lua_pushlstring(L,s+i,length);
			i+=length;
			num++;
	   } break;
   case 'A':
   {
    int l;
    i = copy(L,&l,sizeof(l),s,i,len);
    if (i+l>len) luaL_argerror(L,1,"input stringBuf too short");
    lua_pushlstring(L,s+i,l);
    i+=l;
	num++;
    break;
   }
   case 'n':
   {
    lua_Number a;
    i=copy(L,&a,sizeof(a),s,i,len);
    lua_pushnumber(L,a);
	num++;
    break;
   }
   case 'd':
   {
    double a;
    i=copy(L,&a,sizeof(a),s,i,len);
    lua_pushnumber(L,a);
	num++;
    break;
   }
   case 'f':
   {
    float a;
    i=copy(L,&a,sizeof(a),s,i,len);
    lua_pushnumber(L,a);
	num++;
    break;
   }
   case 'c':
   {
    char a;
    i=copy(L,&a,sizeof(a),s,i,len);
    lua_pushnumber(L,a);
	num++;
    break;
   }
   case 'C':
   {
    unsigned char a;
    i=copy(L,&a,sizeof(a),s,i,len);
    lua_pushnumber(L,a);
	num++;
    break;
   }
   case 's':
   {
    short a;
    i=copy(L,&a,sizeof(a),s,i,len);
    lua_pushnumber(L,a);
	num++;
    break;
   }
   case 'S':
   {
    unsigned short a;
    i=copy(L,&a,sizeof(a),s,i,len);
    lua_pushnumber(L,a);
	num++;
    break;
   }
   case 'i':
   {
    int a;
    i=copy(L,&a,sizeof(a),s,i,len);
    lua_pushnumber(L,a);
	num++;
    break;
   }
   case 'I':
   {
    unsigned int a;
    i=copy(L,&a,sizeof(a),s,i,len);
    lua_pushnumber(L,a);
	num++;
    break;
   }
   case 'l':
   {
    long a;
    i=copy(L,&a,sizeof(a),s,i,len);
    lua_pushnumber(L,a);
	num++;
    break;
   }
   case 'L':
   {
    unsigned long a;
    i=copy(L,&a,sizeof(a),s,i,len);
    lua_pushnumber(L,a);
	num++;
    break;
   }   
   case '_':
    i+=1;
    break;
   case 'o':
	   {
		   double a;//long long a;
		   i=copy(L,&a,sizeof(a),s,i,len);
		   lua_pushnumber(L,(lua_Number)a);
		   num++;
		   break;
	   }
   case 'O':
	   {
		   double a; //unsigned long long a;
		   i=copy(L,&a,sizeof(a),s,i,len);
		   lua_pushnumber(L,(lua_Number)a);
		   num++;
		   break;
	   }
   default:
	   if(c < '0' && c > '9')
	   {
		   badmode(L,c);
	   }
	   break;
  }
 }
 lua_pushnumber(L,(int)(i+1));
 return num+1;
}

/* s string, d double, f float, i int, w short, b byte, c char */

static int l_pack(lua_State *L) 		/* pack(s,...) */
{
 int i,j;
 const char *s=luaL_checkstring(L,1);
 luaL_Buffer b;
 luaL_buffinit(L,&b);
 for (i=0, j=2; s[i]; i++)
 {
  int c=s[i];
  switch (c)
  {
  case 'p':
	  {
		  void * p = lua_touserdata(L,j);
		  luaL_addlstring(&b,(const char*)&p,sizeof(p));
		  j++;
	  }
	  break;
  case 'B':
	  {
		  int k;
		  int factor = 1;
		  int length = 0;
		  char cDigit;

		  size_t l;
		  const char* a;

		  for(k=i-1; k>=0; k--)
		  {
			  cDigit = s[k];
			  if(cDigit<'0' || cDigit>'9') break;
			  length += ((int)cDigit - (int)'0') * factor;
			  factor *= 10;
		  }

		  a = luaL_checklstring(L, j, &l);
		  luaL_addlstring(&b,a,l);

		  while(l++ < (size_t)length)
 		  {
 			  luaL_addchar(&b, '\0');
 		  }

		  j++;
	  } break;
   case 'A':
   {
    size_t l;
    const char *a=luaL_checklstring(L,j,&l);
	int _l = (int)l;
    luaL_addlstring(&b,(const char*)&_l,sizeof(_l));
    luaL_addlstring(&b,a,l);

	j++;
    break;
   }
   case 'n':
   {
    lua_Number a=luaL_checknumber(L,j);
    luaL_addlstring(&b,(const char*)&a,sizeof(a));

	j++;
    break;
   }
   case 'd':
   {
    double a=luaL_checknumber(L,j);
    luaL_addlstring(&b,(const char*)&a,sizeof(a));

	j++;
    break;
   }
   case 'f':
   {
    float a=(float)luaL_checknumber(L,j);
    luaL_addlstring(&b,(const char*)&a,sizeof(a));

	j++;
    break;
   }
   case 'c':
   {
    char a=(char)luaL_checknumber(L,j);
    luaL_addlstring(&b,(const char*)&a,sizeof(a));

	j++;
    break;
   }
   case 'C':
   {
    unsigned char a=(unsigned char)luaL_checknumber(L,j);
    luaL_addlstring(&b,(const char*)&a,sizeof(a));

	j++;
    break;
   }
   case 's':
   {
    short a=(short)luaL_checknumber(L,j);
    luaL_addlstring(&b,(const char*)&a,sizeof(a));

	j++;
    break;
   }
   case 'S':
   {
    unsigned short a=(unsigned short)luaL_checknumber(L,j);
    luaL_addlstring(&b,(const char*)&a,sizeof(a));

	j++;
    break;
   }
   case 'i':
   {
    int a=luaL_checkint(L,j);
    luaL_addlstring(&b,(const char*)&a,sizeof(a));

	j++;
    break;
   }
   case 'I':
   {
    unsigned int a=luaL_checkint(L,j);
    luaL_addlstring(&b,(const char*)&a,sizeof(a));

	j++;
    break;
   }
   case 'l':
   {
    long a=luaL_checklong(L,j);
    luaL_addlstring(&b,(const char*)&a,sizeof(a));

	j++;
    break;
   }
   case 'L':
   {
    unsigned long a=luaL_checklong(L,j);
    luaL_addlstring(&b,(const char*)&a,sizeof(a));

	j++;
    break;
   }
   case '_':
   {
	j++;
    break;
   }
   case 'o':
	   {
		   double a = luaL_checknumber(L,j);
		   luaL_addlstring(&b,(const char*)&a,sizeof(a));
		   j++;
		   break;
	   }
   case 'O':
	   {
		   double a = luaL_checknumber(L,j);
		   luaL_addlstring(&b,(const char*)&a,sizeof(a));
		   j++;
		   break;
	   }
   default:
	   if(c < '0' && c > '9')
	   {
		   badmode(L,c);
	   }
    break;
  }
 }
 luaL_pushresult(&b);
 return 1;
}

//int lua_packlibopen(lua_State *L)
//{
// lua_register(L,"bpack",l_pack);
// lua_register(L,"bunpack",l_unpack);
// return 0;
//}


//int luaLM_import( lua_State *L )
//{
// lua_packlibopen(L);
// return 0;
//}

static const char * luaLM_version( void )
{
	return LUA_VERSION;
}


/* }====================================================== */


static const luaL_Reg pack_funcs[] = {
	{"unpack", l_unpack},
	{"pack", l_pack},
	{NULL, NULL}
};

LUALIB_API int luaopen_pack (lua_State *L) {
	luaL_register(L, LUA_PACKNAME, pack_funcs);
	return 1;
}