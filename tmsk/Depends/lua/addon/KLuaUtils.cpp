#include "../KLuaWrap.h"
#include <System/Log/log.h>
#include <stdio.h>
#include <locale>
#include <System/Charset/KCharset.h>

EXTERN_C FILE* getPrintOutput();
EXTERN_C int str_format (lua_State*);

static DirectFileLogger defaultLuaLogger("./luaDebug.log", "a+");
LoggerWraper g_luaLogger(&defaultLuaLogger);

extern "C" int readstream_k(void* s, size_t* from, char* buf, size_t len)
{
	StreamInterface* si = (StreamInterface*)s;
	if(*from == (size_t)-1)
	{
		size_t pos = si->tell();
		char szBinary[4];
		si->ReadData(szBinary, sizeof(szBinary));
		if(!strncmp(szBinary,LUA_SIGNATURE,sizeof(szBinary)))
			*from = encode_count + 1;
		si->Seek(pos);

		if(*from != encode_count + 1)
		{
			DWORD flag = 0;
			pos = si->tell();
			int_r n = si->ReadData(&flag, 3);
			if(n == 3 && flag == utf8_signature)
			{
				*from = (size_t)encode_utf8;
			}
			else
			{
				*from = (size_t)encode_ansii;
				si->Seek(pos);
			}
		}
	}

	if(*from == (size_t)g_encoding || *from == encode_count + 1)
	{
		int n = (int)si->ReadData(buf, len);
		if(n < 1) return 0;
		return n;
	}
	else
	{
		size_t pos = si->tell();

		char buf2[LUAL_BUFFERSIZE];
		int n = (int)si->ReadData(buf2, sizeof(buf2)-1);
		if(n < 1) return 0;
		buf2[n] = '\0';

		const char* src = buf2; int srclen = n;
		char* dst = buf;  int dstlen = (int)len;

		KCharset* cs = KCharset::getLocalCharset();
		if(g_encoding == encode_ansii) n = cs->from_utf8(&src, &srclen, &dst, &dstlen);
		else if(g_encoding == encode_utf8) n = cs->to_utf8(&src, &srclen, &dst, &dstlen);
		else return 0;

		int rbytes = (int)(src - &buf2[0]);
		int wbytes = (int)(dst - buf);
		if(wbytes < 1) return 0;
		if(srclen > 0) si->Seek(pos + rbytes);
		return wbytes;
	}
}

static BOOL arrange_va_list(lua_State* L, int fromArg, va_arg_decl* decls, size_t n, char* nargs)
{
	char argBuffer[1024];
	va_arg_decl* declBuffer[256];
	System::Collections::DynArray<va_arg_decl*> arr;
	arr.attach(declBuffer, 256, 0);

	for(size_t i=0; i<n; i++)
	{
		va_arg_decl& decl = decls[i];
		int ordinal = decl.ordinal;
		if(ordinal < 1 || ordinal > 256)
			return FALSE;
		arr[ordinal-1] = &decl;
	}

	size_t pos = 0;
	for(int i=0; i<(int)n; i++)
	{
		int arg = fromArg + i;
		va_arg_decl* decl = arr.at((int)i);
		if(!decl) return FALSE;
		switch(decl->argDataType)
		{
		case va_arg_decl::arg_integer:
			{
				if(!lua_isnumber(L, arg))
				{
					luaL_argerror(L, arg, "must be integer");
					return FALSE;
				}
				if(decl->size == sizeof(int))
				{
					int_r val = (int_r)luaL_checkinteger(L, arg);
					memcpy(&argBuffer[pos], &val, sizeof(val));
				}
				else
				{// long long
					double val = luaL_checknumber(L, arg);
					memcpy(&argBuffer[pos], &val, sizeof(val));
				}
			} break;
		case va_arg_decl::arg_float:
			{
				if(!lua_isnumber(L, arg))
				{
					luaL_argerror(L, arg, "must be number");
					return FALSE;
				}
				double val = luaL_checknumber(L, arg);
				memcpy(&argBuffer[pos], &val, sizeof(val));
			} break;
		case va_arg_decl::arg_string:
			{
				if(!lua_isstring(L, arg))
				{
					luaL_argerror(L, arg, "must be string");
					return FALSE;
				}
				const char* val = luaL_checkstring(L, arg);
				memcpy(&argBuffer[pos], &val, sizeof(val));
			} break;
		case va_arg_decl::arg_pointer:
			{
				if(!lua_islightuserdata(L, arg) && !lua_isuserdata(L, arg))
				{
					luaL_argerror(L, arg, "must be lightuserdata or userdata");
					return FALSE;
				}
				const void* val = lua_topointer(L, arg);
				memcpy(&argBuffer[pos], &val, sizeof(val));
			}
		default:
			ASSERT(FALSE);
			break;
		}

		decl->offset = (int)pos;
		pos += kmax((size_t)decl->size, sizeof(int_r));
	}

	pos = 0;
	for(int i=0; i<(int)n; i++)
	{
		va_arg_decl* decl = &decls[i];
		size_t size = kmax((size_t)decl->size, sizeof(int_r));
		memcpy(nargs+pos, &argBuffer[decl->offset], size);
		pos += size;
	}
	return TRUE;
}

extern "C" int str_format_k(lua_State* L, int fmtArg, char* buf, size_t len)
{
	if(!lua_isstring(L, fmtArg))
	{
		luaL_argerror(L, fmtArg, "is not a format string");
		return -1;
	}

	char nfmt[1024];
	char nargs[1024];
	va_arg_decl argDecls[256];

	const char* fmt = lua_tostring(L, fmtArg);

	char fmt_2[1024];
	if(localToCurrent(fmt, fmt_2, sizeof(fmt_2)) < 0)
		return 0;
	fmt = fmt_2;

	int n = scanFormatString(fmt, nfmt, argDecls);
	if(n > 0)
	{
		va_list newArgs;
		if(!arrange_va_list(L, fmtArg+1, argDecls, n, nargs))
			return strcpy_k(buf, len, fmt);
		_init_va_list(&newArgs, &nargs);
		return vsnprintf(buf, len, nfmt, newArgs);
	}
	else
		return strcpy_k(buf, len, fmt);
}

static int _getDebugInfo(lua_Debug& dbg, char* buf, int len)
{
	int n = 0;
	char* pc = buf;

	{
		int l = sprintf(pc, "\t");
		pc += l; n += l;
	}

	if(dbg.name)
	{
		int l = sprintf(pc, "%s(%s)", dbg.name, dbg.what?dbg.what:"");
		pc += l; n += l;
	}
	if(dbg.source)
	{
		int l = sprintf(pc, "%s%s", (n ? " at ":""), dbg.source);
		pc += l; n += l;
	}
	if(dbg.currentline > 0)
	{
		int l = sprintf(pc, "%s%d", n?" line:":"", dbg.currentline);
		pc += l; n += l;
	}
	if(n)
	{
		int l = sprintf(pc, "\n");
		pc += l; n += l;
	}

	return n;
}

int luaU_onError(lua_State* L)
{
	int n = 0;
	LuaString str;
	char buf[1024];
	buf[0] = 0;

	str = lua_tostring(L, -1);
	str.append("\n");

	int level = 0;
	lua_Debug luaDebug;
	while(lua_getstack(L, level++, &luaDebug))
	{
		if(!lua_getinfo(L, "nSl", &luaDebug)) break;
		n += _getDebugInfo(luaDebug, &buf[n], 1024-n);
	}

	str.append(buf);
	g_luaLogger.WriteLog(LOG_WARN, "%s", str.c_str());
	printf_k("LuaError: %s\n", str.c_str());

	return 0;
}

int luaU_sprintf(lua_State* L)
{
	return str_format(L);
}

// printf(fmt, ...)
int luaU_printf(lua_State* L)
{
	char buffer[10240];
	FILE* filep = getPrintOutput(); if(!filep) return 0;
	int len = str_format_k(L, 1, buffer, sizeof(buffer));
	if(len < 0)
	{
		luaL_argerror(L, 1, "invalid format string");
		return 0;
	}
	ASSERT(len < sizeof(buffer)); buffer[len] = '\0';

	char buf2[10240];
	if(currentToLocal(buffer, buf2, sizeof(buf2)) < 0)
	{
		luaL_argerror(L, 1, "printf:currentToLocal");
		return 0;
	}

	fprintf(filep, "%s\n", buf2);
	return 0;
}

// log(fmt, ...)
// log(loglevel, fmt, ...)
int luaU_log(lua_State* L)
{
	int fmtArg = 1; char buffer[10240];
	int loglevel = -1 & (~LOG_CONSOLE);
	if(lua_isnumber(L, 1))
	{
		loglevel = (int)lua_tointeger(L, 1);
		fmtArg = 2;
	}
	int len = str_format_k(L, fmtArg, buffer, sizeof(buffer));
	if(len < 0)
	{
		luaL_argerror(L, 1, "invalid format string");
		return 0;
	}
	ASSERT(len<sizeof(buffer));buffer[len]='\0';
	Log(loglevel, "%s", buffer);
	return 0;
}

////////////////////////////////////////////////
// parse object from a object address
// function parseObject(className, address)
// push a object with type className to stack
//
int luaU_parseObject(lua_State* L)
{
	typedef LMREG::voidPtr2user voidPtr2user;

	const char* className = luaL_checkstring(L, 1);

	void* addr = NULL;
	if(lua_isnumber(L,2))
	{	addr = (void*)(uint_r)luaL_checknumber(L, 2); }
	else if(lua_islightuserdata(L,2))
	{	addr = lua_touserdata(L,2); }
	else
	{
		luaL_argerror(L, 2, "address must be a number or pointer");
		return 0;
	}

	if (!addr)
	{
		lua_pushnil(L);
		return 1;
	}

	lua_pushstring(L, className);
	lua_gettable(L, LUA_GLOBALSINDEX);
	if(!lua_istable(L, -1))
	{
		luaL_argerror(L, 1, "metatable for classname not found");
		return 0;
	}

	lua_pushstring(L, "__ref");
	lua_gettable(L, -2);
	if(lua_istable(L, -1))
	{
		lua_remove(L, -2);
		lua_pushlightuserdata(L, addr); //lua_pushnumber(L, (DWORD)addr);
		lua_rawget(L, -2);
		ASSERT(lua_isuserdata(L, -1));
		lua_remove(L, -2);
	}
	else
	{
		lua_pop(L, 1);
		void* pUserData = lua_newuserdata(L, sizeof(voidPtr2user));
		new (pUserData) voidPtr2user(addr);
		lua_insert(L, -2);
		lua_setmetatable(L, -2);
	}

	return 1;
}

////////////////////////////////////////////////
// parse object from struct address and offset
// function parseMemberObj(className, structAddress, offset)
// push a object with type className to stack
//
int luaU_parseMemberObj(lua_State* L)
{
	typedef LMREG::voidPtr2user voidPtr2user;

	const char* className = luaL_checkstring(L, 1);

	void* structAddr = NULL;
	if(lua_isnumber(L,2))
	{	structAddr = (void*)(uint_r)luaL_checknumber(L, 2); }
	else if(lua_islightuserdata(L,2))
	{	structAddr = lua_touserdata(L,2); }
	else if(lua_isstring(L, 2))
	{ structAddr = (void*)lua_tostring(L, 2); }
	else if(lua_isstring(L,2))
	{ structAddr = (void*)lua_tostring(L, 2); }
	else
	{
		luaL_argerror(L, 2, "address must be a number or pointer");
		return 0;
	}

	if(!lua_isnumber(L, 3))
	{
		luaL_argerror(L, 3, "offset must be a number value (1 based)");
		return 0;
	}
	int offset = (int)lua_tonumber(L, 3);
	if(offset < 1)
	{
		luaL_argerror(L, 3, "offset must be a number value (1 based)");
		return 0;
	}

	void* addr = *(void**)((char*)structAddr + offset - 1);
	if (!addr)
	{
		lua_pushnil(L);
		return 1;
	}

	lua_pushstring(L, className);
	lua_gettable(L, LUA_GLOBALSINDEX);
	if(!lua_istable(L, -1))
	{
		luaL_argerror(L, 1, "metatable for classname not found");
		return 0;
	}

	lua_pushstring(L, "__ref");
	lua_gettable(L, -2);
	if(lua_istable(L, -1))
	{
		lua_remove(L, -2);
		lua_pushlightuserdata(L, addr); //lua_pushnumber(L, (DWORD)addr);
		lua_rawget(L, -2); ASSERT(lua_isuserdata(L, -1));
		lua_remove(L, -2);
	}
	else
	{
		lua_pop(L, 1);
		void* pUserData = lua_newuserdata(L, sizeof(voidPtr2user));
		new (pUserData) voidPtr2user(addr);
		lua_insert(L, -2);
		lua_setmetatable(L, -2);
	}

	return 1;
}

//////////////////////////////////////////////////
// parse a string from char*
// function parseString(strAddr)
// push a string into stack copied from char* strAddr
//
int luaU_parseString(lua_State* L)
{
	const char* addr = NULL;
	if(lua_isnumber(L,1))
	{
		addr = (const char*)(DWORD)lua_tonumber(L, 1);
	}
	else if(lua_islightuserdata(L,1))
	{
		addr = (const char*)lua_touserdata(L,1);
	}
	else
	{
		luaL_argerror(L,1,"strAddr must be a number or pointer");
		return 0;
	}
	lua_pushstring(L, addr);
	return 1;
}

/**
 * function sizeof(format)
 * return the size of format
 * such as:
 * local len = sizeof('II')
 * len is 8
 */
int luaU_sizeof(lua_State* L)
{
	char buf[512];
	int f_len = 0;
	char factor[16];

	char* fmt = NULL;
	if(lua_islightuserdata(L,1))
	{
		fmt = (char*)lua_touserdata(L,1);
	}
	else if(lua_isstring(L,1))
	{
		fmt = const_cast<char*>(lua_tostring(L, 1));
	}
	else
	{
		sprintf(buf, "format must be a string");
		luaL_argerror(L, 1, buf);
		return 0;
	}

	char chr; int length = 0;
	while(chr = *fmt, fmt++, chr)
	{
		switch(chr)
		{
		case 'a':
			{
				length += sizeof(void*);
			} break;
		case 'p':
			{
				length += sizeof(void*);
			} break;
		case 'A': // string of 10A
		case 'B':
			{
				factor[f_len] = '\0'; f_len = 0;
				length += str2int(factor);
			} break;
		case 'c': // Signed char value
			{
				length += sizeof(char);
			} break;
		case 'C': // Unsigned char value
			{
				length += sizeof(unsigned char);
			} break;
		case 's': // Signed short value
			{
				length += sizeof(short);
			} break;
		case 'S': // Unsigned short value
			{
				length += sizeof(unsigned short);
			} break;
		case 'i': // Signed integer value
			{
				length += sizeof(int);
			} break;
		case 'I': // Unsigned integer value
			{
				length += sizeof(unsigned int);
			} break;
		case 'l': // Signed long value
			{
				length += sizeof(long);
			} break;
		case 'L': // Unsigned long value
			{
				length += sizeof(unsigned long);
			} break;
		case 'f': // Single-precision float in the native format
			{
				length += sizeof(float);
			} break;
		case 'd': // Double-precision float in the native format
			{
				length += sizeof(double);
			} break;
		case 'o': // Signed integer64 value
			{
				length += sizeof(INT64);
			} break;
		case 'O': // Unsigned integer64 value
			{
				length += sizeof(UINT64);
			} break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			{
				factor[f_len++] = chr;
			} break;
		default:
			{
				sprintf(buf, "invalid format '%c'", chr);
				luaL_argerror(L, 1, buf);
			} break;
		}
	}
	lua_pushnumber(L, length);
	return 1;
}

/**
 * function readStruct(address, fmt, from)
 *
 *   a		string pointer in struct to lua string
 *   p		void* to light userdata
 *
 *   A		string
 *   c		Signed char value
 *   C		Unsigned char value
 *   s		Signed short value
 *   S		Unsigned short value
 *   i		Signed integer value
 *   I		Unsigned integer value
 *   l		Signed long value
 *   L		Unsigned long value
 *   f		Single-precision float in the native format
 *   d		Double-precision float in the native format
 *
 * example: local x,y,from = parseStruct(structAddress, 'II', from)
 *			return value 'from' is the next available position.
 */
int luaU_readStruct(lua_State* L)
{
	char buf[512];

	int ret_num = 0;
	int f_len = 0;
	char factor[16];

	char* src = NULL;
	if(lua_isnumber(L, 1))
	{
		src = (char*)(DWORD)lua_tonumber(L, 1);
	}
	else if(lua_islightuserdata(L,1))
	{
		src = (char*)lua_touserdata(L,1);
	}
	else if(lua_isstring(L,1))
	{
		src = const_cast<char*>(lua_tostring(L, 1));
	}
	else
	{
		sprintf(buf, "address must be a number or pointer");
		luaL_argerror(L, 1, buf);
		return 0;
	}
	ASSERT(src);
	const char* fmt = luaL_checkstring(L, 2);
	int from = (int)luaL_checknumber(L, 3) - 1;
	if(from < 0)
	{
		sprintf(buf, "from=%d, is start with 1", from+1);
		luaL_argerror(L, 3, buf);
		return 0;
	}

	char chr;
	while(chr = *fmt, fmt++, chr)
	{
		switch(chr)
		{
		case 'a':
			{
				const char* val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushstring(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 'p':
			{
				void* val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushlightuserdata(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 'A': // string
			{
				factor[f_len] = '\0'; f_len = 0;
				//int len = str2int(factor);
				int len; memcpy(&len, src+from, sizeof(len));
				lua_pushlstring(L, src+from+sizeof(len), len);
				from += sizeof(len)+len; ret_num++;
			} break;
		case 'c': // Signed char value
			{
				char val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushnumber(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 'C': // Unsigned char value
			{
				unsigned char val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushnumber(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 's': // Signed short value
			{
				short val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushnumber(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 'S': // Unsigned short value
			{
				unsigned short val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushnumber(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 'i': // Signed integer value
			{
				int val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushnumber(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 'I': // Unsigned integer value
			{
				unsigned int val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushnumber(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 'l': // Signed long value
			{
				long val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushnumber(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 'L': // Unsigned long value
			{
				unsigned long val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushnumber(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 'f': // Single-precision float in the native format
			{
				float val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushnumber(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 'd': // Double-precision float in the native format
			{
				double val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushnumber(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 'o': // Signed integer64 value
			{
				double val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushnumber(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case 'O': // Unsigned integer64 value
			{
				double val; f_len = 0;
				memcpy(&val, src+from, sizeof(val));
				lua_pushnumber(L, val);
				from += sizeof(val); ret_num++;
			} break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			{
				factor[f_len++] = chr;
			} break;
		default:
			{
				sprintf(buf, "invalid format '%c'", chr);
				luaL_argerror(L, 1, buf);
			} break;
		}
	}
	lua_pushnumber(L, from+1); // next avail position to read
	return ret_num+1;
}

/**
 * function writeStruct(address, from, fmt, ...)
 *
 *   A		string
 *   B		binary with length, such as 10B
 *   c		Signed char value
 *   C		Unsigned char value
 *   s		Signed short value
 *   S		Unsigned short value
 *   i		Signed integer value
 *   I		Unsigned integer value
 *   l		Signed long value
 *   L		Unsigned long value
 *   f		Single-precision float in the native format
 *   d		Double-precision float in the native format
 *
 * example: local from = writeStruct(structAddress, 1, 'II', 100, 200)
 *			return value 'from' is the next available position.
 */
int luaU_writeStruct(lua_State* L)
{
	char buf[512] = {0};
	int f_len = 0;
	char factor[16];
	int varArg = 4;

	char* src = NULL;
	if(lua_isnumber(L, 1))
	{
		src = (char*)(DWORD)lua_tonumber(L, 1);
	}
	else if(lua_islightuserdata(L,1))
	{
		src = (char*)lua_touserdata(L,1);
	}
	else
	{
		sprintf(buf, "address must be a number or pointer");
		luaL_argerror(L, 1, buf);
		return 0;
	}

	int from = (int)luaL_checknumber(L, 2) - 1;

	// if the arg3 is lightuserdata then assert arg4 is number, (ptr,size) copy to buf[from]
	if(lua_islightuserdata(L, 3))
	{
		char * userdata = (char*)lua_touserdata(L,3);
		ASSERT(userdata);
		int userdata_size = (int)luaL_checknumber(L, 4);
		ASSERT(userdata_size > 0);
		memcpy(src+from, userdata, userdata_size);
		from+=userdata_size;
		lua_pushnumber(L, from+1); // next avail position to write
		return 1;
	}
	else
	{
		const char* fmt = luaL_checkstring(L, 3);

		if(from < 0)
		{
			sprintf(buf, "from=%d, is start with 1", from+1);
			luaL_argerror(L, 2, buf);
			return 0;
		}

		char chr;
		while(chr = *fmt, fmt++, chr)
		{
			switch(chr)
			{
			case 'p':
				{
					f_len = 0;
					bool cond = lua_islightuserdata(L, varArg);
					void * val = NULL;
					if (cond)
					{
						val = lua_touserdata(L, varArg);
					}
					memcpy(src+from, &val, sizeof(val));
					from += sizeof(val); varArg++;
				} break;
			case 'A': // string of A
				{
					int i;
					factor[f_len] = '\0'; f_len = 0;
					//int len = str2int(factor);
					size_t len;
					const char* val = luaL_checklstring(L, varArg, &len);
					int _len = (int)len; memcpy(src+from, &_len, sizeof(_len)); from += sizeof(_len);
					for(i=0; i<(int)len && val[i]; i++) src[from+i] = val[i];
					src[from+i] = '\0';
					from += (int)len; varArg++;
				} break;
			case 'B': // string of 10B
				{
					int i;
					factor[f_len] = '\0'; f_len = 0;
					int len = str2int(factor);
					const char* val = luaL_checkstring(L, varArg);
					for(i=0; i<len && val[i]; i++) src[from+i] = val[i];
					for(; i<len; i++) src[from+i] = '\0';
					from += len; varArg++;
				} break;
			case 'c': // Signed char value
				{
					f_len = 0;
					char val = luaL_checkint(L, varArg);
					memcpy(src+from, &val, sizeof(val));
					from += sizeof(val); varArg++;
				} break;
			case 'C': // Unsigned char value
				{
					f_len = 0;
					unsigned char val = luaL_checkint(L, varArg);
					memcpy(src+from, &val, sizeof(val));
					from += sizeof(val); varArg++;
				} break;
			case 's': // Signed short value
				{
					f_len = 0;
					short val = luaL_checkint(L, varArg);
					memcpy(src+from, &val, sizeof(val));
					from += sizeof(val); varArg++;
				} break;
			case 'S': // Unsigned short value
				{
					f_len = 0;
					unsigned short val = luaL_checkint(L, varArg);
					memcpy(src+from, &val, sizeof(val));
					from += sizeof(val); varArg++;
				} break;
			case 'i': // Signed integer value
				{
					f_len = 0;
					int val = luaL_checkint(L, varArg);
					memcpy(src+from, &val, sizeof(val));
					from += sizeof(val); varArg++;
				} break;
			case 'I': // Unsigned integer value
				{
					f_len = 0;
					unsigned int val = luaL_checkint(L, varArg);
					memcpy(src+from, &val, sizeof(val));
					from += sizeof(val); varArg++;
				} break;
			case 'l': // Signed long value
				{
					f_len = 0;
					long val = luaL_checkint(L, varArg);
					memcpy(src+from, &val, sizeof(val));
					from += sizeof(val); varArg++;
				} break;
			case 'L': // Unsigned long value
				{
					f_len = 0;
					unsigned long val = luaL_checkint(L, varArg);
					memcpy(src+from, &val, sizeof(val));
					from += sizeof(val); varArg++;
				} break;
			case 'f': // Single-precision float in the native format
				{
					f_len = 0;
					float val = (float)luaL_checknumber(L, varArg);
					memcpy(src+from, &val, sizeof(val));
					from += sizeof(val); varArg++;
				} break;
			case 'd': // Double-precision float in the native format
				{
					f_len = 0;
					double val = luaL_checknumber(L, varArg);
					memcpy(src+from, &val, sizeof(val));
					from += sizeof(val); varArg++;
				} break;
			case 'o': // Signed integer64 value
			case 'O': // Unsigned integer64 value
				{
					f_len = 0;
					double d = luaL_checknumber(L, varArg);
					memcpy(src+from, &d, sizeof(d));
					from += sizeof(double); varArg++;
				} break;
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
				{
					factor[f_len++] = chr;
				} break;
			default:
				{
					sprintf(buf, "invalid format '%c'", chr);
					luaL_argerror(L, 1, buf);
				} break;
			}
		}
		lua_pushnumber(L, from+1); // next avail position to write
		return 1;
	}
}

/**
 *	lua: local buf = offsetbuf(buf, offset)
 *	对缓冲区指针做指定的偏移
 *
 *  buf : a buffer pointer (void*)
 *  offset : integer offset, maybe negative
 */
int luaU_offsetbuf(lua_State* L)
{
	const char* buf = NULL;
	// buf can be a number
	if(lua_isnumber(L, 1))
	{
		buf = (const char*)(uint_r)lua_tonumber(L, 1);
		int offset = (int)luaL_checkinteger(L, 2);
		lua_pushlightuserdata(L, (void*)(buf+offset)); //lua_pushnumber(L, (DWORD)(buf+offset));
		return 1;
	}
	// buf maybe a light userdata
	else if(lua_islightuserdata(L, 1))
	{
		buf = (const char*)lua_touserdata(L, 1);
		int offset = (int)luaL_checkinteger(L, 2);
		LMREG::push<void*>(L, (void*)(buf+offset));
		return 1;
	}

	luaL_argerror(L, 1, "param 1 buf must be a number or light userdata.");
	return 0;
}

/**
 * local address = addressof(obj_from_cpp)
 * return address is a light user data
 */

int luaU_addressof(lua_State* L)
{
	typedef LMREG::voidPtr2user void_user;
	if(!lua_isuserdata(L, 1))
	{
		luaL_argerror(L, 1, "arg 1 must be a userdata from c++");
		return 0;
	}
	void_user* p = (void_user*)lua_touserdata(L, 1);
	lua_pushlightuserdata(L, p->m_p); //lua_pushinteger(L, (DWORD)(p->m_p));
	return 1;
}

/*
 * 把数值型转为Identity32
 * local id = identity32(num)
 * 主要用于把lua中的数值常量转成Identity
 */
int luaU_identity32(lua_State* L)
{
	if(lua_isnumber(L, 1))
	{
		UINT32 val = (UINT32)lua_tonumber(L, 1);
		LMREG::push<Identity32>(L, Identity32(val));
		return 1;
	}
	else
	{
		luaL_argerror(L, 1, "arg:num should be a number value");
		return 0;
	}
}

/*
* 把数值型转为Identity64
* local id = identity64(num)
* 主要用于把lua中的数值常量转成Identity
*/
int luaU_identity64(lua_State* L)
{
	if(lua_isnumber(L, 1))
	{
		UINT64 val = (UINT64)lua_tonumber(L, 1);
		LMREG::push<Identity64>(L, Identity64(val));
		return 1;
	}
	else
	{
		luaL_argerror(L, 1, "arg:num should be a number value");
		return 0;
	}
}

// local s = oid2str(oid)
int luaU_oid2str(lua_State* L)
{
	char buf[256];
	if(lua_isnumber(L, 1))
	{
		double number = lua_tonumber(L, 1);
		UINT64 val;
		memcpy(&val, &number, sizeof(UINT64));
		size_t l = sprintf_k(buf, sizeof(buf), "%lld", val);
		lua_pushlstring(L, buf, l);
		return 1;
	}
	else if(lua_islightuserdata(L, 1))
	{
		UINT64 val = (UINT64)lua_touserdata(L, 1);
		size_t l = sprintf_k(buf, sizeof(buf), "%lld", val);
		lua_pushlstring(L, buf, l);
		return 1;
	}
	else
	{
		luaL_argerror(L, 1, "arg:oid should be a number value");
		return 0;
	}
}
////////////////////////////////////////
int luaU_str2oid(lua_State* L)
{
	const char *s1 = luaL_checkstring(L, 1);
	char *s2;
	double n;
	UINT64 value;
	n = strtod(s1, &s2);
	if (s1 != s2) {  /* at least one valid digit? */
		while (isspace((unsigned char)(*s2))) s2++;  /* skip trailing spaces */
		if (*s2 == '\0') {  /* no invalid trailing characters? */
			value = _strtoi64(s1, &s2, 10);
			memcpy(&n, &value, sizeof(UINT64));
			lua_pushnumber(L, (lua_Number)n);
			return 1;
		}
	}
	lua_pushnil(L);  /* else not a number */
	return 1;
}
////////////////////////////////////////
int luaU_ASSERT(lua_State* L)
{
	int expression = 1;
	if(lua_isnumber(L, 1))
	{
		expression = (int)lua_tointeger(L, 1);
	}
	else if(lua_isboolean(L, 1))
	{
		expression = lua_toboolean(L, 1);
	}
	else if (lua_isnoneornil(L, 1))
	{
		expression = 0;
	}

	ASSERT(expression);
	return 1;
}

int luaU_intMod(lua_State* L)
{
	if( lua_gettop(L)!=2 )
	{
		ASSERT(false);
	}

	int left  = 0;
	int right = 0;
	if(lua_isnumber(L, 1))
	{
		left = (int)lua_tointeger(L, 1);
	}
	else
	{
		ASSERT(false);
	}

	if(lua_isnumber(L, 2))
	{
		right = (int)lua_tointeger(L, 2);
	}
	else
	{
		ASSERT(false);
	}

	lua_pushnumber(L, left%right);
	return 1;
}

int luaU_intDivide(lua_State* L)
{
	if( lua_gettop(L)!=2 )
	{
		ASSERT(false);
	}

	int left  = 0;
	int right = 0;
	if(lua_isnumber(L, 1))
	{
		left = (int)lua_tointeger(L, 1);
	}
	else
	{
		ASSERT(false);
	}

	if(lua_isnumber(L, 2))
	{
		right = (int)lua_tointeger(L, 2);
	}
	else
	{
		ASSERT(false);
	}
	if(right==0)
	{
		ASSERT(false);
	}

	lua_pushnumber(L, left/right);
	return 1;
}

int luaU_Int64Less(lua_State* L)
{
	if( lua_gettop(L)!=2 )
	{
		ASSERT(false);
	}

	INT64 left  = 0;
	INT64 right = 0;
	if(lua_isnumber(L, 1))
	{
		double number = lua_tonumber(L, 1);
		memcpy(&left, &number, sizeof(INT64));
	}
	else if(lua_islightuserdata(L, 1))
	{
		left = (INT64)lua_touserdata(L, 1);
	}
	else
	{
		ASSERT(FALSE);
	}

	if(lua_isnumber(L, 2))
	{
		double number = lua_tonumber(L, 2);
		memcpy(&right, &number, sizeof(INT64));
	}
	else if(lua_islightuserdata(L, 2))
	{
		right = (INT64)lua_touserdata(L, 2);
	}
	else
	{
		ASSERT(FALSE);
	}

	lua_pushboolean(L,left<right );
	return 1;
}

int luaU_Int64Greater(lua_State* L)
{
	if( lua_gettop(L)!=2 )
	{
		ASSERT(false);
	}


	INT64 left  = 0;
	INT64 right = 0;
	if(lua_isnumber(L, 1))
	{
		double number = lua_tonumber(L, 1);
		memcpy(&left, &number, sizeof(INT64));
	}
	else if(lua_islightuserdata(L, 1))
	{
		left = (INT64)lua_touserdata(L, 1);
	}

	if(lua_isnumber(L, 2))
	{
		double number = lua_tonumber(L, 2);
		memcpy(&right, &number, sizeof(INT64));
	}
	else if(lua_islightuserdata(L, 2))
	{
		right = (INT64)lua_touserdata(L, 2);
	}

	lua_pushboolean(L,left>right );
	return 1;
}

int luaU_Int64Equal(lua_State* L)
{
	if( lua_gettop(L)!=2 )
	{
		ASSERT(false);
	}

	INT64 left  = 0;
	INT64 right = 0;
	if(lua_isnumber(L, 1))
	{
		double number = lua_tonumber(L, 1);
		memcpy(&left, &number, sizeof(INT64));
	}
	else if(lua_islightuserdata(L, 1))
	{
		left = (INT64)lua_touserdata(L, 1);
	}

	if(lua_isnumber(L, 2))
	{
		double number = lua_tonumber(L, 2);
		memcpy(&right, &number, sizeof(INT64));
	}
	else if(lua_islightuserdata(L, 2))
	{
		right = (INT64)lua_touserdata(L, 2);
	}

	lua_pushboolean(L,left==right );
	return 1;
}

int luaU_UInt64Less(lua_State* L)
{
	if( lua_gettop(L)!=2 )
	{
		ASSERT(false);
	}

	UINT64 left  = 0;
	UINT64 right = 0;
	if(lua_isnumber(L, 1))
	{
		double number = lua_tonumber(L, 1);
		memcpy(&left, &number, sizeof(UINT64));
	}
	else if(lua_islightuserdata(L, 1))
	{
		left = (UINT64)lua_touserdata(L, 1);
	}

	if(lua_isnumber(L, 2))
	{
		double number = lua_tonumber(L, 2);
		memcpy(&right, &number, sizeof(UINT64));
	}
	else if(lua_islightuserdata(L, 2))
	{
		right = (UINT64)lua_touserdata(L, 2);
	}

	lua_pushboolean(L,left<right );
	return 1;
}

int luaU_UInt64Greater(lua_State* L)
{
	if( lua_gettop(L)!=2 )
	{
		ASSERT(false);
	}

	UINT64 left  = 0;
	UINT64 right = 0;
	if(lua_isnumber(L, 1))
	{
		double number = lua_tonumber(L, 1);
		memcpy(&left, &number, sizeof(UINT64));
	}
	else if(lua_islightuserdata(L, 1))
	{
		left = (UINT64)lua_touserdata(L, 1);
	}

	if(lua_isnumber(L, 2))
	{
		double number = lua_tonumber(L, 2);
		memcpy(&right, &number, sizeof(UINT64));
	}
	else if(lua_islightuserdata(L, 2))
	{
		right = (UINT64)lua_touserdata(L, 2);
	}

	lua_pushboolean(L,left>right );
	return 1;
}

int luaU_UInt64Equal(lua_State* L)
{
	if( lua_gettop(L)!=2 )
	{
		ASSERT(false);
	}

	UINT64 left  = 0;
	UINT64 right = 0;
	if(lua_isnumber(L, 1))
	{
		double number = lua_tonumber(L, 1);
		memcpy(&left, &number, sizeof(UINT64));
	}
	else if(lua_islightuserdata(L, 1))
	{
		left = (UINT64)lua_touserdata(L, 1);
	}

	if(lua_isnumber(L, 2))
	{
		double number = lua_tonumber(L, 2);
		memcpy(&right, &number, sizeof(UINT64));
	}
	else if(lua_islightuserdata(L, 2))
	{
		right = (UINT64)lua_touserdata(L, 2);
	}

	lua_pushboolean(L,left==right );
	return 1;
}

////////////////////////////////////////
/*
 * 分配一块固定大小的内存块给lua使用
 * 内存块分为4K、10K和32K三部分，拥有个数分别为64、32和16
 */
int luaU_allocbuf(lua_State* L)
{
	const static int buf4k_size		= 4 * 1024;
	const static int buf4K_count	= 64;
	static int buf4k_index					= 0;

	const static int buf10k_size	= 10 * 1024;
	const static int buf10k_count	= 32;
	static int buf10k_index				= 0;

	const static int buf32k_size	= 32 * 1024;
	const static int buf32k_count	= 16;
	static int buf32k_index				= 0;

	static char buf4k[buf4K_count][buf4k_size];
	static char buf10k[buf10k_count][buf10k_size];
	static char buf32k[buf32k_count][buf32k_size];
	char* pbuf = NULL;
	if ( !lua_isnumber(L, 1) )
		return 0;

	int bufsize = (int)lua_tointeger(L, 1);

	if ( bufsize <= buf4k_size )
	{
		if (buf4k_index >= buf4K_count)
			buf4k_index = 0;

		pbuf = buf4k[buf4k_index++];
		memset(pbuf, 0, buf4k_size);
	}
	else if ( buf4k_size < bufsize && bufsize <= buf10k_size )
	{
		if (buf10k_index >= buf10k_count)
			buf10k_index = 0;

		pbuf = buf10k[buf10k_index++];
		memset(pbuf, 0, buf4k_size);	}
	else if ( buf32k_size > bufsize )
	{
		if (buf32k_index >= buf32k_count)
			buf32k_index = 0;

		pbuf = buf32k[buf32k_index++];
		memset(pbuf, 0, buf32k_size);
	}
	else
	{
		lua_pushnil(L);
		return 1;
	}

	lua_pushlightuserdata(L, (void*)pbuf);
	return 1;

}

// 把lua中的常量数值转为 oid (KOBjectID)
// local oid = tooid(100)
int luaU_tooid(lua_State* L)
{
	double n = (double)luaL_checknumber(L, 1);
	UINT64 val = (UINT64)n;
	memcpy(&n, &val, sizeof(n));
	lua_pushnumber(L, n);
	return 1;
}

int luaU_oid2Number(lua_State* L)
{
	if( lua_gettop(L)!=1 )
	{
		ASSERT(false);
	}
	if(lua_isnumber(L, 1))
	{
		double n = (double)lua_tonumber(L, 1);
		UINT64 ret;
		memcpy(&ret, &n, sizeof(n));
		lua_pushnumber(L, int(ret));
		return 1;
	}
	else
	{
		assert(false);	return 0;
	}
}

template <typename T> void luaU_registerGlobal(lua_State* L, const char* name, const T& val)
{
	LMREG::push(L, val);
	lua_setglobal(L, name);
}

////////////////////////////////////////
void RegisterLuaUtils(lua_State* L)
{
	luaU_registerGlobal(L, "LOG_FATAL",		LOG_FATAL);
	luaU_registerGlobal(L, "LOG_ERROR",		LOG_ERROR);
	luaU_registerGlobal(L, "LOG_WARN",		LOG_WARN);
	luaU_registerGlobal(L, "LOG_INFO",		LOG_INFO);
	luaU_registerGlobal(L, "LOG_DEBUG",		LOG_DEBUG);
	luaU_registerGlobal(L, "LOG_CONSOLE",	LOG_CONSOLE);
	luaU_registerGlobal(L, "LOG_ALL",		LOG_ALL);

	luaU_registerGlobal(L, "log_fatal",		LOG_FATAL);
	luaU_registerGlobal(L, "log_error",		LOG_ERROR);
	luaU_registerGlobal(L, "log_warn",		LOG_WARN);
	luaU_registerGlobal(L, "log_info",		LOG_INFO);
	luaU_registerGlobal(L, "log_debug",		LOG_DEBUG);
	luaU_registerGlobal(L, "log_console",	LOG_CONSOLE);
	luaU_registerGlobal(L, "log_all",		LOG_ALL);

	lua_register(L, "printf", luaU_printf);
	lua_register(L, "sprintf", luaU_sprintf);
	lua_register(L, "parseObject", luaU_parseObject);
	lua_register(L, "parseMemberObj", luaU_parseMemberObj);
	lua_register(L, "parseString", luaU_parseString);
	lua_register(L, "log", luaU_log);
	lua_register(L, "Log", luaU_log);
	lua_register(L, "sizeof", luaU_sizeof);
	lua_register(L, "readStruct", luaU_readStruct);
	lua_register(L, "writeStruct", luaU_writeStruct);
	lua_register(L, "readbuf", luaU_readStruct);
	lua_register(L, "writebuf", luaU_writeStruct);
	lua_register(L, "offsetbuf", luaU_offsetbuf);
	lua_register(L, "addressof", luaU_addressof);
	lua_register(L, "identity32", luaU_identity32);
	lua_register(L, "identity64", luaU_identity64);
	lua_register(L, "oid2str", luaU_oid2str);
	lua_register(L, "str2oid", luaU_str2oid);
	lua_register(L, "ASSERT", luaU_ASSERT);
	lua_register(L, "intMod", luaU_intMod);
	lua_register(L, "intDivide", luaU_intDivide);
	lua_register(L, "allocbuf", luaU_allocbuf);
	lua_register(L, "Int64Less", luaU_Int64Less);
	lua_register(L, "Int64Greater", luaU_Int64Greater);
	lua_register(L, "Int64Equal", luaU_Int64Equal);
	lua_register(L, "UInt64Less", luaU_UInt64Less);
	lua_register(L, "UInt64Greater", luaU_UInt64Greater);
	lua_register(L, "UInt64Equal", luaU_UInt64Equal);
	lua_register(L, "oidLess", luaU_UInt64Less);
	lua_register(L, "oidGreater", luaU_UInt64Greater);
	lua_register(L, "oidEqual", luaU_UInt64Equal);
	lua_register(L, "tooid", luaU_tooid);
	lua_register(L, "number2Oid", luaU_tooid);
	lua_register(L, "oid2Number", luaU_oid2Number);

}
