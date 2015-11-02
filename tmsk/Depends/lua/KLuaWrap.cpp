#include "KLuaWrap.h"
#include <System/File/KFile.h>
#include <System/Collections/KMapByVector.h>
#include <System/Memory/KBuffer64k.h>
#include <System/Cipher/jgencryptor.h>
#include <System/KPlatform.h>
#include <time.h>
#include <math.h>
#include "KLuaTableHolder.h"

using namespace ::System::File;
using namespace ::System::Collections;

extern int luaU_onError(lua_State* L);
extern void RegisterLuaUtils(lua_State* L);
extern LoggerWraper g_luaLogger;

//BOOL WINAPI DllMain(HANDLE hinstDLL, 
//					DWORD dwReason, 
//					LPVOID lpvReserved
//					)
//{
//	if(dwReason == DLL_PROCESS_ATTACH)
//	{
//		int x = 0;
//		x++;
//	}
//	return TRUE;
//}
//

class KRecursiveLuaFileFilter : public KFileFilter
{
public:
	BOOL Accept(const KFileData& fileData)
	{
		if(fileData.m_shortName[0] == '.') return FALSE;
		if(fileData.IsDir()) return TRUE;
		const char* pDot = strrchr(fileData.m_shortName, '.');
		if(!pDot) return FALSE;
		return stricmp(pDot+1, "lua") == 0;
	}
};

char m_caCurScript[MAX_PATH];
KLuaWrap* g_pLuaWrapper = new KLuaWrap();

/// KLuaItem ///////////////////////////////////

KLuaWrap &	GetLuaSystem()
{
	return LuaWraper;
}

void luaSetGlobalLogger(ILogger* logger)
{
	GetGlobalLogger().SetLogger(logger);
}

void luaSetLuaLogger(ILogger* logger)
{
	g_luaLogger.SetLogger(logger);
}

BOOL LuaItem::isfunctype(const char* typeName)
{
#if defined(WIN32) || defined(WINDOWS)
	return strstr(typeName, "_cdecl") != NULL;
#else //函数引用, 函数指针
	return typeName[0] == 'F' || (typeName[0] == 'P' && typeName[1]=='F');
#endif
}

void LuaItem::pushmyname()
{
	if(m_isNum)
	{
		lua_pushinteger(m_L, (int)(uint_r)m_myName);
	}
	else
	{
		lua_pushstring(m_L, m_myName);
	}
}

LuaItem::LuaItem(lua_State* L, int parent, const char* myName)
	: m_L(L)
	, m_parentIdx(parent)
	, m_isNum(FALSE)
	, m_level(0)
	, m_myName(myName)
{
	m_top =  lua_gettop(m_L) - 1;
}

LuaItem::LuaItem(lua_State* L, int parent, int idx)
	: m_L(L)
	, m_parentIdx(parent)
	, m_isNum(TRUE)
	, m_level(0)
	, m_myName((const char*)idx)
{
	m_top =  lua_gettop(m_L) - 1;
}

LuaItem::~LuaItem()
{
	lua_settop(m_L, m_top);
}

bool LuaItem::isnil() const
{
	return lua_isnil(m_L, -1);
}

bool LuaItem::isboolean() const
{
	return lua_isboolean(m_L, -1);
}

bool LuaItem::isnumber() const
{
	return lua_isnumber(m_L, -1) ? true : false;
}

bool LuaItem::istable() const
{
	return lua_istable(m_L, -1);
}

bool LuaItem::isstring() const
{
	return lua_isstring(m_L, -1) ? true : false;
}

bool LuaItem::isuserdata() const
{
	return lua_isuserdata(m_L, -1) ? true : false;
}

bool LuaItem::isfunction() const
{
	return lua_isfunction(m_L, -1);
}

bool LuaItem::isCfunction() const
{
	return lua_iscfunction(m_L, -1) ? true : false;
}

BOOL LuaItem::IsFunction() const
{
	return lua_isfunction(m_L, -1);
}

LuaItem& LuaItem::operator [] (int idx)
{
	// 如果本对象不是table，那么放弃现在的值，构造一个table
	if(!lua_istable(m_L, -1))
	{
		lua_pop(m_L, 1);
		lua_newtable(m_L);
		this->pushmyname();
		lua_pushvalue(m_L, -2);
		lua_settable(m_L, m_parentIdx);
	}

	m_parentIdx = lua_gettop(m_L);
	m_myName = (const char*)idx; m_isNum = TRUE;

	this->pushmyname();
	lua_gettable(m_L, m_parentIdx);

	m_level += 1;
	return *this;
}

LuaItem& LuaItem::operator [] (const char* name)
{
	// 如果本对象不是table，那么放弃现在的值，构造一个table
	if(!lua_istable(m_L, -1))
	{
		lua_pop(m_L, 1);
		lua_newtable(m_L);
		this->pushmyname();
		lua_pushvalue(m_L, -2);
		lua_settable(m_L, m_parentIdx);
	}

	m_parentIdx = lua_gettop(m_L);
	m_myName = name; m_isNum = FALSE;

	this->pushmyname();
	lua_gettable(m_L, m_parentIdx);

	m_level += 1;
	return *this;
}

LuaItem& LuaItem::operator = (bool val)
{
	lua_pushboolean(m_L, val);
	this->pushmyname();
	lua_pushvalue(m_L, -2);
	lua_settable(m_L, m_parentIdx);
	return *this;
}

LuaItem& LuaItem::operator = (char ch)
{
	return this->operator = ((int)ch);
}

LuaItem& LuaItem::operator = (BYTE by)
{
	return this->operator = ((unsigned int)by);
}

LuaItem& LuaItem::operator = (short sh)
{
	return this->operator = ((int)sh);
}

LuaItem& LuaItem::operator = (WORD w)
{
	return this->operator = ((unsigned int)w);
}

LuaItem& LuaItem::operator = (int i)
{
	lua_pushinteger(m_L, i);
	this->pushmyname();
	lua_pushvalue(m_L, -2);
	lua_settable(m_L, m_parentIdx);
	return *this;
}

LuaItem& LuaItem::operator = (unsigned int ui)
{
	lua_pushnumber(m_L, ui);
	this->pushmyname();
	lua_pushvalue(m_L, -2);
	lua_settable(m_L, m_parentIdx);
	return *this;
}

LuaItem& LuaItem::operator = (long l)
{
	return this->operator = ((int)l);
}

LuaItem& LuaItem::operator = (unsigned long ul)
{
	return this->operator = ((unsigned int)ul);
}

LuaItem& LuaItem::operator = (INT64 i64)
{
	LMREG::push(m_L, i64);
	this->pushmyname();
	lua_pushvalue(m_L, -2);
	lua_settable(m_L, m_parentIdx);
	return *this;
}

LuaItem& LuaItem::operator = (UINT64 ui64)
{
	LMREG::push(m_L, ui64);
	this->pushmyname();
	lua_pushvalue(m_L, -2);
	lua_settable(m_L, m_parentIdx);
	return *this;
}

LuaItem& LuaItem::operator = (float f)
{
	lua_pushnumber(m_L, f);
	this->pushmyname();
	lua_pushvalue(m_L, -2);
	lua_settable(m_L, m_parentIdx);
	return *this;
}

LuaItem& LuaItem::operator = (double d)
{
	lua_pushnumber(m_L, d);
	this->pushmyname();
	lua_pushvalue(m_L, -2);
	lua_settable(m_L, m_parentIdx);
	return *this;
}

LuaItem& LuaItem::operator = (const char* val)
{
	lua_pop(m_L, 1);
	lua_pushstring(m_L, val);
	this->pushmyname();
	lua_pushvalue(m_L, -2);
	lua_settable(m_L, m_parentIdx);
	return *this;
}

LuaItem& LuaItem::operator = (const LuaString& val)
{
	lua_pop(m_L, 1);
	lua_pushlstring(m_L, val, val.size());
	this->pushmyname();
	lua_pushvalue(m_L, -2);
	lua_settable(m_L, m_parentIdx);
	return *this;
}

LuaItem& LuaItem::operator = (LMREG::table& val)
{
	lua_pop(m_L, 1);
	LMREG::push(m_L, val);
	this->pushmyname();
	lua_pushvalue(m_L, -2);
	lua_settable(m_L, m_parentIdx);
	return *this;
}

LuaItem& LuaItem::operator = (LMREG::CppTable& t)
{
	lua_pop(m_L, 1);
	LMREG::push(m_L, t);
	this->pushmyname();
	lua_pushvalue(m_L, -2);
	lua_settable(m_L, m_parentIdx);
	return *this;
}

LuaItem& LuaItem::operator = (LMREG::KPtrObjcet val)
{
	lua_pop(m_L, 1);
	LMREG::push(m_L, val);
	this->pushmyname();
	lua_pushvalue(m_L, -2);
	lua_settable(m_L, m_parentIdx);
	return *this;
}
/// KLuaWrap ///////////////////////////////////

KLuaWrap::KLuaWrap(void)
{
	m_funcErrReport = 0;
	_LastError = NULL;
	m_dwScripts = 1;
	m_l = NULL;
	m_mainLS = NULL;
	m_extendTableRef = LUA_REFNIL;
	m_typename2userdataableRef = LUA_REFNIL;
	m_pScriptContextPool = NULL;
	m_ud.m_this = this;
	this->InitScript(0);
}

KLuaWrap::~KLuaWrap(void)
{

}

void KLuaWrap::Destory()
{
	//m_mpScripts.clear();
	lua_close(m_l);
}

void LuaInclude(char* cafile, int udir)
{
	//lua_State* l = KLuaWrap::GetInstance()->m_l;
	lua_State* l = LuaWraper.m_l;
	DWORD id = ELFHash(cafile);
	static System::Collections::KMapByVector<DWORD, DWORD> incmap;
	char caPath[MAX_PATH]; 
	char caLuaPath[MAX_PATH];
	char caFunPath[MAX_PATH];

	JG_C::DynArray<char*,512> lstFname;
	JG_C::KString<512> fname;

	int count = 0, i = 0;
	DWORD buf[30];
	char** sp = NULL;

	for (i=0; i<(int)strlen(cafile); ++i)
	{
		if (cafile[i] == '\\') cafile[i] = '/';
	}

	if (cafile[1] == ':')
	{
		if (incmap[id] == 0)
		{
			if (!luaL_dofile(l, cafile))
			{
				incmap[id] = 1;
			}
		}
		return;
	}

	lua_Debug ar;
	lua_getstack(l, 1, &ar);
	lua_getinfo(l, "nS", &ar);
	
	char* p = (char*)ar.source;
	for (i=0; i<(int)strlen(p); ++i)
	{
		if (p[i] == '\\') p[i] = '/';
	}

	if (ar.source[0] == '@')
	{
		if ((ar.source[2] != ':' && !udir) 
			|| (udir && m_caCurScript[1]!=':'))
		{
			GetCurrentDirectory(MAX_PATH, caPath); 
			sp = spilwhit(caPath, "/", buf, 30, count);
			for (i=0; i<count; ++i)
			{
				if((sp[i])[0]) lstFname.push_back(sp[i]);
			}
		}
		
		if (udir)
		{
			for (i=0; i<(int)strlen(m_caCurScript); ++i)
			{
				if (m_caCurScript[i] == '\\') m_caCurScript[i] = '/';
			}
			strcpy(caLuaPath, m_caCurScript);
		}
		else
			strcpy(caLuaPath, ar.source+1);

		sp = spilwhit(caLuaPath, "/", buf, 30, count);
		for (i=0; i<count-1; ++i)
		{
			if (strcmp(sp[i], "..") == 0)
			{
				int idx = lstFname.size() - 1;
				if(idx >= 0) lstFname.erase(idx);
			}
			else if (strcmp(sp[i], ".") != 0)
			{
				if((sp[i])[0]) lstFname.push_back(sp[i]);
			}
		}

		strcpy(caFunPath, cafile);
		sp = spilwhit(caFunPath, "/", buf, 30, count);
		for (i=0; i<count; ++i)
		{
			if (strcmp(sp[i], "..") == 0)
			{
				int idx = lstFname.size() - 1;
				if(idx >= 0) lstFname.erase(idx);
			}
			else if (strcmp(sp[i], ".") != 0)
			{
				if((sp[i])[0]) lstFname.push_back(sp[i]);
			}
		}
	}
	else
	{
		GetCurrentDirectory(MAX_PATH, caPath); 
		sp = spilwhit(caPath, "/", buf, 30, count);
		for (i=0; i<count; ++i)
		{
			if((sp[i])[0]) lstFname.push_back(sp[i]);
		}

		strcpy(caFunPath, cafile);
		sp = spilwhit(caFunPath, "/", buf, 30, count);
		for (i=0; i<count; ++i)
		{
			if (strcmp(sp[i], "..") == 0)
			{
				int idx = lstFname.size() - 1;
				if(idx >= 0) lstFname.erase(idx);
			}
			else if (strcmp(sp[i], ".") != 0)
			{
				if((sp[i])[0]) lstFname.push_back(sp[i]);
			}
		}
	}

	int ccc = lstFname.size();
	for(int i=0; i<lstFname.size(); i++)
	{
		fname.append(lstFname[i]);
		if(i != ccc-1) fname.append("/");
	}

	id = ELFHash((char*)fname.c_str());
	if (incmap[id] == 0)
	{
		if (!luaL_dofile(l, fname.c_str()))
		{
			incmap[id] = 1;
		}
	}
}

int KLuaWrap::InitScript(int bDebugMode)
{
	if(m_l) return lwp_success;

	m_l = lua_open();
	if(!m_l) return lwp_error;

	m_funcErrReport = luaU_onError;
	m_pScriptContextPool = new KScriptContextPool();

	G(m_l)->ud = &m_ud;

	luaL_openlibs(m_l);
	lua_atpanic(m_l, luaU_onError);
	luaL_openThirdpartlibs(m_l);
	::RegisterLuaUtils(m_l);
	
	(*this)["include"] = LuaInclude;
	(*this)["ELFHash"] = ELFHash;

	{///
		KLuaStackRecover rv(m_l);
		lua_pushcclosure(m_l, luaU_onError, 0);
		m_ud.m_errfunc = luaL_ref(m_l, LUA_REGISTRYINDEX);
	}
	// extend
	createExtendTable();
	// name2userdatatable
	createName2UserdataTable();

	m_mainLS = m_l;
	return lwp_success;
}

int KLuaWrap::InitScript(lua_State* L)
{
	if(!L) return lwp_error;
	m_l = L;
	m_mainLS = m_l;
	return lwp_success;
}

// 掉用lua函数，如果调用失败，则尝试读取脚本后再次调用
int KLuaWrap::DoScriptFunc(char* file, char* func, char* fmt, ...)
{
	for (int it=0; it<2; it++)
	{
		va_list args; 
		va_start(args, fmt);

		int i = 0, n = 0;
		char* buf = fmt;

		lua_pushstring(m_l, func); 
		lua_gettable(m_l, LUA_GLOBALSINDEX); 
		if(!lua_isfunction(m_l,-1))
		{
			if (!file) return lwp_error;
			const char* s = lua_tostring(m_l, 1);
			lua_pop(m_l, 1);
			if (s && strcmp(s, "attempt to call a nil value") == 0)
			{
				if (!file || isScriptLoaded(file)) return lwp_error;
				luaL_loadfile(m_l, file);
			}
			continue;
		}

		while (*buf)
		{
			switch (*buf++)
			{
			case 's':
				lua_pushstring(m_l, va_arg(args, char*));
				break;

			case 'd':
				lua_pushnumber(m_l, va_arg(args, int));
				break;

			case 'f':
				lua_pushnumber(m_l, va_arg(args, double));
				break;

			case '>':
				goto dofunc;
				break;

			default:
				return lwp_error;
				break;
			}
			n++;
			if (!lua_checkstack(m_l, 1)) return lwp_error;
		}

dofunc:
		int rs = (int)strlen(buf);
		if (lua_pcall(m_l, n, rs, 0) == 0)
		{
			while (*buf) 
			{ 
				switch (*buf++) 
				{
				case 's': 
					*va_arg(args, const char **) = lua_tostring(m_l, -rs);
					break;
				case 'd': 
					*va_arg(args, int *) = (int)lua_tonumber(m_l, -rs);
					break;
				case 'f': 
					*va_arg(args, float *) = (float)lua_tonumber(m_l, -rs);
					break;
				}
				rs--;
			}
			return lwp_success;
		}
		else
		{
			if (_LastError = (char*)lua_tostring(m_l, -1))
				lua_pop(m_l, 1);
			if (_LastError && strcmp(_LastError, "attempt to call a nil value") == 0)
			{
				if (!file || isScriptLoaded(file)) return lwp_error;
				luaL_loadfile(m_l, file);
			}

		}
		va_end(args);
	}
	return lwp_error;
}

bool KLuaWrap::isScriptLoaded(char* name)
{
	//return m_mpScripts[ELFHash(name)] > 0;
	return false;
}

int KLuaWrap::LoadScripts(const char* fn)
{
	KLuaStackRecover sr(m_l);
	if(luaL_loadfile(m_l, fn))
	{
		if(m_funcErrReport) m_funcErrReport(m_l);
		return lwp_error;
	}
	return lwp_success;
}

int KLuaWrap::doScripts(const char* fn)
{
	KInputFileStream fi;
	if(!fi.Open(fn)) return lwp_error;
	return this->doStream(fi,fn) ? lwp_success : lwp_error;
}

int KLuaWrap::doDirScripts(const char* dir)
{
	char fulldir[512];
	char fullPath[512];

	System::File::KDirectory dirObj;
	System::File::KDirectory::FileList fileList;
	System::File::KFileSuffixFilter filter(".lua");

	getfullpath(fulldir, dir, 512);

	dirObj.Open(dir);
	if(!dirObj.ListFile(fileList, &filter))
		return lwp_error;
	
	int nRet = lwp_success;
	for(int i=0; i<fileList.size(); i++)
	{
		const JG_F::KFileData& fileData = fileList[i];
		sprintf(fullPath, "%s%c%s", fulldir, PATH_SEPERATOR, fileData.m_shortName);
		int scriptResult = this->doScripts(fullPath);

		if (scriptResult != lwp_success)
		{
			nRet = scriptResult;
			Log(LOG_FATAL, "error: KLuaWrap::doDirScripts, open file %s error %s", fullPath, _LastError);
			ASSERT(FALSE);
		}
	}

	return nRet;
}

BOOL KLuaWrap::loadStream(StreamInterface& si)
{
	if(luaL_loadstream_k(m_l, &si, NULL) == lwp_success)
		return TRUE;
	const char* err = lua_tostring(m_l, -1);
	Log(LOG_ERROR|LOG_CONSOLE, "error: load stream '%s' error:%s", "<NULL>", err ? err : "<empty>");
	return FALSE;
}

BOOL KLuaWrap::loadStream(StreamInterface& si,const char* fn)
{
	if(luaL_loadstream_k(m_l, &si, fn) == lwp_success)
		return TRUE;
	const char* err = lua_tostring(m_l, -1);
	Log(LOG_ERROR|LOG_CONSOLE, "error: load stream '%s' error:%s", fn ? fn : "<NULL>", err ? err : "<empty>");
	return FALSE;
}

BOOL KLuaWrap::doStream(StreamInterface& si)
{
	KLuaStackRecover sr(m_l);
	int errfunc = LMREG::lua_geterrfunc(m_l);
	if(!this->loadStream(si))
	{
		_LastError = lua_tostring(sr.m_L, -1);
		return FALSE;
	}
	if(lua_pcall(m_l, 0, LUA_MULTRET, errfunc))
	{
		_LastError = lua_tostring(sr.m_L, -1);
		return FALSE;
	}
	return TRUE;
}

BOOL KLuaWrap::doStream(StreamInterface& si,const char* fn)
{
	KLuaStackRecover sr(m_l);
	int errfunc = LMREG::lua_geterrfunc(m_l);
	if(!this->loadStream(si,fn))
	{
		_LastError = lua_tostring(sr.m_L, -1);
		return FALSE;
	}
	if(lua_pcall(m_l, 0, LUA_MULTRET, errfunc))
	{
		_LastError = lua_tostring(sr.m_L, -1);
		return FALSE;
	}
	return TRUE;
}

bool KLuaWrap::DoDirScripts_Recursive(const char* dirName)
{
	KVector<KFileData::FilePath> dirs;
	dirs.push_back(dirName);

	KDirectory dir;
	KRecursiveLuaFileFilter filter;
	KDirectory::FileList flist;

	while(dirs.size())
	{
		KFileData::FilePath fpath = dirs.at(0); dirs.erase(0);
		if(!dir.Open(fpath.c_str()))
		{
			Log(LOG_ERROR, "error: KLuaWrap::DoDirScripts_Recursive, open dir %s", fpath.c_str());
			continue;
		}
		if(!dir.ListFile(flist, &filter))
		{
			Log(LOG_ERROR, "error: KLuaWrap::DoDirScripts_Recursive, list dir %s", fpath.c_str());
			continue;
		}

		int n = flist.size();
		for(int i=0; i<n; i++)
		{
			const KFileData& fdata = flist.at(i);
			KFileData::FilePath fpath = fdata.getPath();
			if(fdata.IsDir())
			{
				dirs.push_back(fpath);
			}
			else
			{
				if(this->doScripts(fpath.c_str()) == lwp_error)
				{
					Log(LOG_ERROR, "error: KLuaWrap::DoDirScripts_Recursive, dofile %s", fpath.c_str());
					continue;
				}
			}
		}
	}

	return true;
}

int KLuaWrap::doString(const char* str)
{
	return this->doString(str, NULL);
}

int KLuaWrap::doString(const char* str, const char* name)
{
	return this->doBuffer(str, (int)strlen(str), name);
}

bool KLuaWrap::doStringWithParam(KParamPackage& params, const char* script, size_t len)
{
	KLuaStackRecover sr(m_l);
	int errfunc = LMREG::lua_geterrfunc(m_l);

	KLuaTableWraper& tbl = *m_pScriptContextPool->allocContext();
	params.dump(tbl);

	if(luaL_loadbuffer(m_l, script, len, NULL))
	{
		luaU_onError(m_l);
		params.remove(tbl);
		m_pScriptContextPool->freeContext(&tbl);
		return false;
	}

	tbl.loadTableInstance(m_l);
	lua_setfenv(m_l, -2);

	int nRet = lua_pcall(m_l, 0, LUA_MULTRET, errfunc);

	params.remove(tbl);
	m_pScriptContextPool->freeContext(&tbl);
	return nRet == lwp_success;
}

bool KLuaWrap::doStringWithParam(KParamPackage& params, const char* script)
{
	return this->doStringWithParam(params, script, strlen(script));
}

bool KLuaWrap::doStringInMainThread( const char* szString )
{
	if (0 == luaL_dostring(m_mainLS, szString))
		return true;

	_LastError = lua_tostring(m_mainLS, -1);
	lua_pop(m_mainLS, 1);

	return false;
}

int KLuaWrap::doBuffer(const void* str, int length, const char* name)
{
	KLuaStackRecover sr(m_l);
	int errfunc = LMREG::lua_geterrfunc(m_l);
	if(!this->loadBuffer(str, length, name))
		return lwp_error;
	if(lua_pcall(m_l, 0, LUA_MULTRET, errfunc))
		return lwp_error;
	return lwp_success;

	//JgEncInputMemeryStream fi(str, length);
	//return this->doStream(fi) ? lwp_success : lwp_error;

	//const char* script = NULL;
	//JgEncInputMemeryStream stream(str, length);

	//int len = stream.getString(script);

	//KLuaStackRecover sr(m_l);
	//int errfunc = LMREG::lua_geterrfunc(m_l);

	//if(luaL_loadbuffer(m_l, script, len, name))
	//{
	//	if(m_funcErrReport) m_funcErrReport(m_l);
	//	return lwp_error;
	//}
	//if(lua_pcall(m_l, 0, LUA_MULTRET, errfunc))
	//	return lwp_error;

	//return lwp_success;
}

BOOL KLuaWrap::loadBuffer(const void* str, int length, const char* name)
{
	if(luaL_loadbuffer(m_l, (const char*)str, length, name))
	{
		luaU_onError(m_l);
		return FALSE;
	}
	return TRUE;

	//JgEncInputMemeryStream fi(str, length);
	//return this->loadStream(fi);

	//const char* script = NULL;
	//JgEncInputMemeryStream stream(str, length);

	//int len = stream.getString(script);

	//KLuaStackRecover sr(m_l);
	//if(luaL_loadbuffer(m_l, script, len, name))
	//{
	//	if(m_funcErrReport) m_funcErrReport(m_l);
	//	return lwp_error;
	//}

	//return lwp_success;
}

int KLuaWrap::Ref(const char* resName)
{
	lua_getglobal(m_l,resName);
	return luaL_ref(m_l, LUA_REGISTRYINDEX);
}

void KLuaWrap::UnRef(int ref)
{
	if(ref==LUA_NOREF || ref==LUA_REFNIL)	return;
	luaL_unref(m_l, LUA_REGISTRYINDEX, ref);
}

void KLuaWrap::DoRef(int ref)
{
	// retrieve function
	lua_rawgeti(m_l, LUA_REGISTRYINDEX, ref);
	// call it
	int error = lua_pcall(m_l, 0, 0, 0);
	if(error)
	{
		LMREG::print_error(m_l, "%s", lua_tostring(m_l, -1));
		lua_pop(m_l, 1);
	}
}

LuaItem KLuaWrap::GetRef(int ref)
{
	// retrieve function
	lua_rawgeti(m_l, LUA_REGISTRYINDEX, ref);
	return LuaItem(m_l, LUA_GLOBALSINDEX, ref);
}

int KLuaWrap::GetDumpProtoBuffer(const char* funName, lua_Writer w, void* data, int strip)
{
	lua_getglobal(m_l, funName);
	if(isLfunction(m_l->top -1))
	{
		/// 这里就应该导出 lfunction ，取出它的 proto
		luaU_dump(m_l, clvalue(m_l->top - 1)->l.p, w, data, strip);
		lua_pop(m_l, 1);

		return 1;
	}
	else
	{
		/// 这里返回失败
		lua_pop(m_l, 1);
		return 0;
	}
}

int KLuaWrap::GetDumpFileBuffer(const char* fileName, lua_Writer w)
{
	return 1;
}

bool KLuaWrap::complieScriptFile(const char* srcFilename,const char* destFilename)
{
	luaC_complie(m_l,srcFilename,destFilename);
	return true;
}

int KLuaWrap::LoadString(const char* str)
{
	const char* script = NULL;
	JgEncInputMemeryStream stream(str, (int)strlen(str));
	
	int len = stream.getString(script);

	if(luaL_loadbuffer(m_l, script, len, NULL))
	{
		if(m_funcErrReport) m_funcErrReport(m_l);
		return lwp_error;
	}
	
	return lwp_success;
}

void KLuaWrap::releaseScript(char* name)
{
	//int id = 0;
	//if (id = m_mpScripts[ELFHash(name)])
	//{
	//	m_mpScripts[ELFHash(name)] = 0;
	//	char c[30];
	//	sprintf(c, "spr%d", id);
	//	lua_pushstring(m_l, c);
	//	lua_pushnil(m_l);
	//	lua_settable(m_l, LUA_GLOBALSINDEX);
	//}
}

void KLuaWrap::releaseScript(int id)
{
	//ScriptMap::iterator it = m_mpScripts.begin();
	//for(; it != m_mpScripts.end(); ++it)
	//{
	//	if (it->second == id)
	//	{
	//		it->second = 0;
	//		char c[30];
	//		sprintf(c, "spr%d", id);
	//		lua_pushstring(m_l, c);
	//		lua_pushnil(m_l);
	//		lua_settable(m_l, LUA_GLOBALSINDEX);
	//		return ;
	//	}
	//}
}

LuaItem KLuaWrap::operator[](const char* s)
{
	lua_pushstring(m_l, s);
	lua_gettable(m_l, LUA_GLOBALSINDEX);
	return LuaItem(m_l, LUA_GLOBALSINDEX, s);
}

LuaItem KLuaWrap::operator[](int idx)
{
	lua_pushinteger(m_l, idx);
	lua_gettable(m_l, LUA_GLOBALSINDEX);
	return LuaItem(m_l, LUA_GLOBALSINDEX, idx);
}

BOOL KLuaWrap::buildFunction(const char* name, const char* luaString)
{
	KBuffer64k* pBuffer = KBuffer64k::Alloc(); pBuffer->Reset();
	char* buf = pBuffer->GetWriteBuffer();

	int len = sprintf(buf, "function %s()\n", name);
	len += sprintf(buf+len, "%s", luaString);
	len += sprintf(buf+len, "\nend");

	if(this->doString(buf, "__anonymous__"))
	{
		KBuffer64k::Free(pBuffer);
		return FALSE;
	}

	KBuffer64k::Free(pBuffer);
	return TRUE;
}

BOOL KLuaWrap::buildFunction(const char* name, const char* luaString, const char* arg1)
{
	KBuffer64k* pBuffer = KBuffer64k::Alloc(); pBuffer->Reset();
	char* buf = pBuffer->GetWriteBuffer();

	int len = sprintf(buf, "function %s(%s)\n", name, arg1);
	len += sprintf(buf+len, "%s", luaString);
	len += sprintf(buf+len, "\nend");

	if(this->doString(buf, "__anonymous__"))
	{
		KBuffer64k::Free(pBuffer);
		return FALSE;
	}

	KBuffer64k::Free(pBuffer);
	return TRUE;
}

BOOL KLuaWrap::buildFunction(const char* name, const char* luaString, const char* arg1, const char* arg2)
{
	KBuffer64k* pBuffer = KBuffer64k::Alloc(); pBuffer->Reset();
	char* buf = pBuffer->GetWriteBuffer();

	int len = sprintf(buf, "function %s(%s,%s)\n", name, arg1, arg2);
	len += sprintf(buf+len, "%s", luaString);
	len += sprintf(buf+len, "\nend");

	if(this->doString(buf, "__anonymous__"))
	{
		KBuffer64k::Free(pBuffer);
		return FALSE;
	}

	KBuffer64k::Free(pBuffer);
	return TRUE;
}

BOOL KLuaWrap::buildFunction(const char* name, const char* luaString, const char* arg1, const char* arg2, const char* arg3)
{
	KBuffer64k* pBuffer = KBuffer64k::Alloc(); pBuffer->Reset();
	char* buf = pBuffer->GetWriteBuffer();

	int len = sprintf(buf, "function %s(%s,%s,%s)\n", name, arg1, arg2, arg3);
	len += sprintf(buf+len, "%s", luaString);
	len += sprintf(buf+len, "\nend");

	if(this->doString(buf, "__anonymous__"))
	{
		KBuffer64k::Free(pBuffer);
		return FALSE;
	}

	KBuffer64k::Free(pBuffer);
	return TRUE;
}

BOOL KLuaWrap::buildFunction(const char* name, const char* luaString, const char* arg1, const char* arg2, const char* arg3, const char* arg4)
{
	KBuffer64k* pBuffer = KBuffer64k::Alloc(); pBuffer->Reset();
	char* buf = pBuffer->GetWriteBuffer();

	int len = sprintf(buf, "function %s(%s,%s,%s,%s)\n", name, arg1, arg2, arg3, arg4);
	len += sprintf(buf+len, "%s", luaString);
	len += sprintf(buf+len, "\nend");

	if(this->doString(buf, "__anonymous__"))
	{
		KBuffer64k::Free(pBuffer);
		return FALSE;
	}

	KBuffer64k::Free(pBuffer);
	return TRUE;
}

BOOL KLuaWrap::buildFunction(const char* name, const char* luaString, const char* arg1, const char* arg2, const char* arg3, const char* arg4, const char* arg5)
{
	KBuffer64k* pBuffer = KBuffer64k::Alloc(); pBuffer->Reset();
	char* buf = pBuffer->GetWriteBuffer();

	int len = sprintf(buf, "function %s(%s,%s,%s,%s,%s)\n", name, arg1, arg2, arg3, arg4, arg5);
	len += sprintf(buf+len, "%s", luaString);
	len += sprintf(buf+len, "\nend");

	if(this->doString(buf, "__anonymous__"))
	{
		KBuffer64k::Free(pBuffer);
		return FALSE;
	}

	KBuffer64k::Free(pBuffer);
	return TRUE;
}

const char* KLuaWrap::GetLastError(void) const
{
	if (NULL == _LastError)
		return "";

	return _LastError;
}

///*全面封装 lua 基础功能*/
void KLuaWrap::GetGlobal(const char* objectName)
{
	lua_getglobal(m_l, objectName);
}

//void KLuaWrap::GetField(const char* objectName)
//{
//	lua_getfield(m_l,-1,objectName);
//}

void KLuaWrap::StackPop(int n)
{
	lua_pop(m_l, n);
}

bool KLuaWrap::CheckType(int n, int type)
{
	return lua_type(m_l, n) == type;
}
int KLuaWrap::getsctnum(const char* src, int ch)
{
	int retval = 0;

	for (; *src; src++) if (*src == (char)ch) retval ++;

	return retval;
}

char* KLuaWrap::getsctstr(const char* src, int ch, int pos, char* dst)
{
	for (int i=0; *src && i<pos; src++) if (*src == (char)ch) i ++;

	if (!*src) return NULL;

	char* retval = dst;
	for (; *src && (*src != (char)ch); src++, dst++) *dst = *src;
	*dst = 0;

	return retval;
}

int KLuaWrap::getNestValue(lua_State *L, const char* scr)
{
	int sp = lua_gettop(L);

	char buf[256];
	int n = getsctnum(scr, '.');
	char* ptname = getsctstr(scr, '.', 0, buf);
	lua_pushstring(L, ptname);
	lua_gettable(L, LUA_GLOBALSINDEX);
	/// 当i == n 的时候，把最后一个 '.' 后面的元素也压进去
	for(int i = 1; i <= n; i++)
	{
		/// 当执行到这个步骤，必然为从 table 中索取 value，所以这里需要检查
		// 其他的地方就不需要了
		if (!lua_istable(L, -1))
		{
			lua_settop(L, sp);
			return 0;
		}
		char* ptname = getsctstr(scr, '.', i, buf);
		if (NULL == ptname)
		{
			lua_settop(L, sp);
			return 0;
		}

		lua_pushstring(L, ptname);
		lua_gettable(L, -2);
	}

	return lua_gettop(L) - sp;
}

void KLuaWrap::createExtendTable()
{
	if(m_extendTableRef == LUA_REFNIL)
	{
		// just initial once
		lua_newtable(m_l);										// table
		lua_newtable(m_l);										// table table
		lua_pushstring(m_l, "__mode");							// table table "__mode"
		lua_pushstring(m_l, "k");								// table table "__mode" "k"
		lua_settable(m_l, -3);									// table table(key weak table)
		lua_setmetatable(m_l, -2);								// table
		m_extendTableRef = luaL_ref(m_l, LUA_REGISTRYINDEX);	// (empty)
	}
}

void KLuaWrap::pushExtendTable()
{
	if(m_extendTableRef == LUA_REFNIL)
	{
		lua_pushnil(m_l);										// nil
	}
	else
	{
		lua_rawgeti(m_l, LUA_REGISTRYINDEX, m_extendTableRef);	// table/nil
	}
}

void KLuaWrap::createName2UserdataTable()
{
	if(m_typename2userdataableRef == LUA_REFNIL)
	{
		// just initial once
		lua_newtable(m_l);										// table
		lua_newtable(m_l);										// table table
		lua_pushstring(m_l, "__mode");							// table table "__mode"
		lua_pushstring(m_l, "v");								// table table "__mode" "v"
		lua_settable(m_l, -3);									// table table(key weak table)
		lua_setmetatable(m_l, -2);								// table
		m_typename2userdataableRef = luaL_ref(m_l, LUA_REGISTRYINDEX);	// (empty)
	}
}

void KLuaWrap::pushUserdataTable(const char* name)
{
	if(m_extendTableRef == LUA_REFNIL)
	{
		lua_pushnil(m_l);										// nil
		return;
	}
	else
	{
		lua_rawgeti(m_l, LUA_REGISTRYINDEX, m_extendTableRef);	// table
		lua_pushstring(m_l, name);									// table name
		lua_gettable(m_l, -2);									// table value
		lua_remove(m_l, -2);									// value
	}
}

/////////////////////////////////////////////////
/////////////////////////////////////////////////
namespace LMREG
{
	static void call_stack(lua_State* L, int n)
	{
		lua_Debug ar;
		if(lua_getstack(L, n, &ar) == 1)
		{
			lua_getinfo(L, "nSlu", &ar);

			const char* indent;
			if(n == 0)
			{
				indent = "->\t";
				print_error(L, "\t<call stack>");
			}
			else
			{
				indent = "\t";
			}

			if(ar.name)
				print_error(L, "%s%s() : line %d [%s : line %d]", indent, ar.name, ar.currentline, ar.source, ar.linedefined);
			else
				print_error(L, "%sunknown : line %d [%s : line %d]", indent, ar.currentline, ar.source, ar.linedefined);

			call_stack(L, n+1);
		}
	}

	//LUAWARP_API int on_error(lua_State *L)
	//{
	//	print_error(L, "%s", lua_tostring(L, -1));

	//	//call_stack(L, 0);

	//	return 0;	
	//}

	void print_error(lua_State *L, const char* fmt, ...)
	{
		char text[4096];

		va_list args;
		va_start(args, fmt);
		vsprintf(text, fmt, args);
		va_end(args);

		lua_pushstring(L, text);
		luaU_onError(L);

		#if defined(WIN32) || defined(WINDOWS)
		OutputDebugString(text);
		#endif
	}

	void PrintStack(lua_State *L)
	{
		int nLevel = 0;
		lua_Debug luaDebug;
		while(lua_getstack(L,nLevel,&luaDebug))
		{
			lua_getinfo(L,"nsl",&luaDebug);
			if(luaDebug.name)
			    printf_k("Func:%s \t",luaDebug.name);
			if(luaDebug.source)
			    printf_k("Source:%s \t",luaDebug.source);
			printf_k("Line:%d \n",luaDebug.currentline);
			++nLevel;         
		}
	}

	//  read functions
	template<>	LUAWARP_API void* read(lua_State *L, int index)
	{
		if(lua_isnil(L, index))
		{
			return NULL;
		}
		else if(lua_isstring(L, index))
		{
			return (void*)lua_tostring(L, index);
		}
		else
		{
			return lua_touserdata(L, index);
		}
	}

	template<>	LUAWARP_API const void* read(lua_State *L, int index)
	{
		if(lua_isnil(L,index))
		{
			return NULL;
		}
		else if(lua_isstring(L, index))
		{
			return lua_tostring(L, index);
		}
		else
		{
			return lua_touserdata(L, index);
		}
	}

	template<> LUAWARP_API char* read(lua_State *L, int index)
	{
		/// 这里都需要注意的是，也许是一个lightuserd
		if(lua_isuserdata(L, index))
		{
			return (char*)lua_touserdata(L, index);
		}
		else
			return (char*)lua_tostring(L, index);				
	}

	template<>
		LUAWARP_API const char* read(lua_State *L, int index)
	{
		if(lua_isuserdata(L, index))
		{
			return (const char*)lua_touserdata(L, index);
		}
		else
			return (const char*)lua_tostring(L, index);	
	}

	template<> LUAWARP_API LuaString read(lua_State *L, int index)
	{
		size_t len;
		const char* p = lua_tolstring(L, index, &len);
		return LuaString(p, (int)len);
	}

	template<>	LUAWARP_API LongLuaString read(lua_State *L, int index)
	{
		size_t len;
		const char* p = lua_tolstring(L, index, &len);
		return LongLuaString(p, (int)len);
	}

	template<>
		LUAWARP_API char read(lua_State *L, int index)
	{
		return (char)lua_tonumber(L, index);				
	}

	template<>
		LUAWARP_API unsigned char read(lua_State *L, int index)
	{
		return (unsigned char)lua_tonumber(L, index);		
	}

	template<>
		LUAWARP_API short read(lua_State *L, int index)
	{
		return (short)lua_tonumber(L, index);				
	}

	template<>
		LUAWARP_API unsigned short read(lua_State *L, int index)
	{
		return (unsigned short)lua_tonumber(L, index);	
	}

	template<>
		LUAWARP_API long read(lua_State *L, int index)
	{
		return (long)lua_tonumber(L, index);				
	}

	template<>
		LUAWARP_API unsigned long read(lua_State *L, int index)
	{
		return (unsigned long)lua_tonumber(L, index);		
	}

	template<>
		LUAWARP_API int read(lua_State *L, int index)
	{
		return (int)lua_tonumber(L, index);				
	}

	template<>
		LUAWARP_API unsigned int read(lua_State *L, int index)
	{
		return (unsigned int)lua_tonumber(L, index);		
	}

	template<>
		LUAWARP_API float read(lua_State *L, int index)
	{
		return (float)lua_tonumber(L, index);				
	}

	template<>
		LUAWARP_API double read(lua_State *L, int index)
	{
		return (double)lua_tonumber(L, index);			
	}

	template<>
		LUAWARP_API bool read(lua_State *L, int index)
	{
		if(lua_isboolean(L, index))
			return lua_toboolean(L, index) != 0;				
		else
			return lua_tonumber(L, index) != 0;
	}

	template<>
		LUAWARP_API void read(lua_State *L, int index)
	{
		return;											
	}

	//template<> LUAWARP_API long long read(lua_State *L, int index)
	//{
	//	if(lua_isnumber(L,index))
	//		return (long long)lua_tonumber(L, index);
	//	else
	//		return *(long long*)lua_touserdata(L, index);
	//}
	//template<> LUAWARP_API unsigned long long read(lua_State *L, int index)
	//{
	//	if(lua_isnumber(L,index))
	//		return (unsigned long long)lua_tonumber(L, index);
	//	else
	//		return *(unsigned long long*)lua_touserdata(L, index);
	//}

	template<>
		LUAWARP_API table read(lua_State *L, int index)
	{
		return table(L, index);
	}

	template<> LUAWARP_API CppTable read(lua_State *L, int index)
	{
		CppTable ret;
		ASSERT(lua_istable(L,index));
		typedef JG_C::KString<256> Key;

		int t = index;
		if(t < 0)
		{
			if (t > LUA_REGISTRYINDEX)
			{
				api_check(L, idx != 0 && -idx <= L->top - L->base);
				t = (int)((L->top + t) - L->base) + 1;
			}
		}

		Key key;

		lua_pushnil(L);  // first key
		while (lua_next(L, t) != 0)
		{
			// 'key' is at index -2 and 'value' at index -1
			ASSERT(lua_isboolean(L,-2) || lua_isnumber(L,-2) || lua_isstring(L,-2));
			
			if(lua_type(L,-2) == LUA_TSTRING)
			{
				key = lua_tostring(L,-2);
			}
			else
			{	
				lua_pushvalue(L,-2);
				key = lua_tostring(L,-1);
				lua_pop(L, 1);
			}

			int vtype = lua_type(L, -1);
			switch(vtype)
			{
			case LUA_TBOOLEAN:
				{
					ret[key.c_str()] = lua_toboolean(L,-1);
				} break;
			case LUA_TNUMBER:
				{
					lua_Number num = lua_tonumber(L,-1);
					ret[key.c_str()] = num;
				} break;
			case LUA_TSTRING:
				{
					size_t len;
					const char* str = lua_tolstring(L, -1, &len);
					if(str)
					{
						ret.setbinary(key.c_str(), str, (int)len);
					}
					else
					{
						ret[key.c_str()] = variant();
					}
				} break;
			case LUA_TTABLE:
				{
					ret[key.c_str()] = read<CppTable>(L, -1);
				} break;
			case LUA_TUSERDATA:
				{
					user* p = (user*)lua_touserdata(L,-1);
					ret.setobject(key.c_str(), p->m_p, "userdata");
				} break;
			case LUA_TLIGHTUSERDATA:
				{
					void* p = (void*)lua_touserdata(L,-1);
					ret[key.c_str()] = p;
				} break;
			case LUA_TFUNCTION:
				{
					break;
					ASSERT(lua_iscfunction(L,-1));
					void* p = (void*)lua_tocfunction(L,-1);
					ret.setobject(key.c_str(), p, "cfunction");
				} break;
			default:
				{
					ASSERT(FALSE);
				} break;
			}
			lua_pop(L, 1);  // removes 'value'; keeps 'key' for next iteration
		}

		return ret;
	}

	template<> LUAWARP_API l_func read(lua_State *L, int index)
	{
		return l_func(L, index);
	}

	template<> LUAWARP_API l_stackval read(lua_State *L, int index)
	{
		return l_stackval(L, index);
	}
	
	template<> LUAWARP_API void push(lua_State *L, void* ret)
	{
		lua_pushlightuserdata(L, ret);
	}

	template<> LUAWARP_API void push(lua_State *L, const void* ret)
	{
		lua_pushlightuserdata(L, (void*)ret);
	}

	template<> LUAWARP_API void push(lua_State *L, char ret)
	{
		lua_pushnumber(L, ret);						
	}

	template<> LUAWARP_API void push(lua_State *L, unsigned char ret)
	{
		lua_pushnumber(L, ret);						
	}

	template<> LUAWARP_API void push(lua_State *L, short ret)
	{
		lua_pushnumber(L, ret);						
	}

	template<> LUAWARP_API void push(lua_State *L, unsigned short ret)
	{
		lua_pushnumber(L, ret);						
	}

	template<> LUAWARP_API void push(lua_State *L, long ret)
	{
		lua_pushnumber(L, ret);						
	}

	template<> LUAWARP_API void push(lua_State *L, unsigned long ret)
	{
		lua_pushnumber(L, ret);						
	}

	template<> LUAWARP_API void push(lua_State *L, int ret)
	{
		lua_pushnumber(L, ret);						
	}

	template<> LUAWARP_API void push(lua_State *L, unsigned int ret)
	{
		lua_pushnumber(L, ret);						
	}

	template<> LUAWARP_API void push(lua_State *L, float ret)
	{
		lua_pushnumber(L, ret);						
	}

	template <> LUAWARP_API void push(lua_State *L, double ret)
	{
		lua_pushnumber(L, ret);						
	}

	template<> LUAWARP_API void push(lua_State *L, char* ret)
	{
		lua_pushstring(L, ret);						
	}

	template<> LUAWARP_API void push(lua_State *L, const char* ret)
	{
		lua_pushstring(L, ret);						
	}

	template<>	LUAWARP_API  void push(lua_State *L, LuaString lstr)
	{
		lua_pushlstring(L, lstr.c_str(), lstr.size());
	}

	template<>	LUAWARP_API  void push(lua_State *L, LuaString& lstr)
	{
		lua_pushlstring(L, lstr.c_str(), lstr.size());
	}

	template<>	LUAWARP_API  void push(lua_State *L, const LuaString& lstr)
	{
		lua_pushlstring(L, lstr.c_str(), lstr.size());
	}

	// for long lua string
	template<>	LUAWARP_API  void push(lua_State *L, LongLuaString lstr)
	{
		lua_pushlstring(L, lstr.c_str(), lstr.size());
	}
	template<>	LUAWARP_API  void push(lua_State *L, LongLuaString& lstr)
	{
		lua_pushlstring(L, lstr.c_str(), lstr.size());
	}
	template<>	LUAWARP_API  void push(lua_State *L, const LongLuaString& lstr)
	{
		lua_pushlstring(L, lstr.c_str(), lstr.size());
	}

	template<> LUAWARP_API void push(lua_State *L, bool ret)
	{
		lua_pushboolean(L, ret);						
	}

	template<> LUAWARP_API void push(lua_State *L, lua_value* ret)
	{
		if(ret) ret->to_lua(L); else lua_pushnil(L);	
	}

	template<> LUAWARP_API void push(lua_State *L, INT64 ret)
	{
// 		lua_pushnumber(L, (lua_Number)ret);
		// 直接当成Identity64处理，不能参与运算
		push(L, (Identity64)ret);
	}

	template<> LUAWARP_API void push(lua_State *L, UINT64 ret)
	{
// 		lua_pushnumber(L, (lua_Number)ret);
		// 直接当成Identity64处理，不能参与运算
		push(L, (Identity64)ret);
	}

	template<> LUAWARP_API void push(lua_State *L, Identity32 ret)
	{
		lua_pushlightuserdata(L, (void*)(uint_r)ret);
	}

	template<> LUAWARP_API void push(lua_State *L, Identity64 ret)
	{
		double d; memcpy(&d, &ret, sizeof(ret));
		lua_pushnumber(L, d);
	//#if defined(__x64__)
	//	void* p = (void*)(uint_r)ret;
	//	lua_pushlightuserdata(L, p);
	//#else
	//	double d; memcpy(&d, &ret, sizeof(ret));
	//	lua_pushnumber(L, d);
	//#endif
	}

	template<> LUAWARP_API void push(lua_State *L, variant val)
	{
		switch(val.vt)
		{
		case variant::vt_nil:
			{
				lua_pushnil(L);
				break;
			}
		case variant::vt_bool:
			{
				lua_pushboolean(L, val.b);
				break;
			}
		case variant::vt_int32:
			{
				lua_pushinteger(L, val.n);
				break;
			}
		case variant::vt_uint32:
			{
				push<DWORD>(L, val.un);
				break;
			}
		case variant::vt_int64:
			{
				push<INT64>(L, val.ll);
				break;
			}
		case variant::vt_uint64:
			{
				push<UINT64>(L, val.ull); //this->set(name, val.ull);
				break;
			}
		case variant::vt_float:
			{
				push<float>(L, val.f);
				break;
			}
		case variant::vt_double:
			{
				push<double>(L, val.d);
				break;
			}
		case variant::vt_pointer:
			{
				push<const void*>(L, val.p);
				break;
			}
		case variant::vt_string:
			{
				lua_pushlstring(L, val.str.second, val.str.first);
				break;
			}
		case variant::vt_d_string:
			{
				lua_pushlstring(L, val.dstr.second, val.dstr.first);
				break;
			}
		case variant::vt_table:
			{
				push<CppTable>(L, *val.tab);
				break;
			}
		case variant::vt_object:
			{
				KPtrObjcet obj(val.obj.second, val.obj.first);
				push<KPtrObjcet>(L, obj);
				break;
			}
		default:
			{
				ASSERT(FALSE);
				break;
			}
		}
		return;
	}

	template<> LUAWARP_API INT64 read(lua_State *L, int index)
	{
// 		return (INT64)lua_tonumber(L, index);
		// 直接当成Identity64处理，不能参与运算
		return (INT64)read<Identity64>(L, index).value;
	}
	
	template<> LUAWARP_API UINT64 read(lua_State *L, int index)
	{
// 		return (UINT64)lua_tonumber(L, index);
		// 直接当成Identity64处理，不能参与运算
		return (UINT64)read<Identity64>(L, index).value;
	}
	
	template<> LUAWARP_API Identity32 read(lua_State *L, int index)
	{
		if(!lua_islightuserdata(L, index)) return Identity32();
		return Identity32((UINT32)(uint_r)lua_touserdata(L, index));
	}
	
	template<> LUAWARP_API Identity64 read(lua_State *L, int index)
	{
		if(lua_isnumber(L, index))
		{
			double d = lua_tonumber(L, index);
			UINT64 u64; memcpy(&u64, &d, sizeof(u64));
			return Identity64(u64);
		}
		else if(lua_islightuserdata(L, index))
		{
			return Identity64((uint_r)lua_touserdata(L, index));
		}
		else if (lua_isnoneornil(L, index))
		{
			// 支持不填参数默认值是0
			return Identity64();
		}
		else
		{
			ASSERT_I(FALSE);
			return Identity64();
		}
	}

	//template<> LUAWARP_API void push(lua_State *L, long long ret)			
	//{ 
	//	*(long long*)lua_newuserdata(L, sizeof(long long)) = ret;
	//	lua_pushstring(L, "__s64");
	//	lua_gettable(L, LUA_GLOBALSINDEX);
	//	lua_setmetatable(L, -2);
	//}

	//template<> LUAWARP_API void push(lua_State *L, unsigned long long ret)
	//{
	//	*(unsigned long long*)lua_newuserdata(L, sizeof(unsigned long long)) = ret;
	//	lua_pushstring(L, "__u64");
	//	lua_gettable(L, LUA_GLOBALSINDEX);
	//	lua_setmetatable(L, -2);
	//}

	template<>	LUAWARP_API  void push(lua_State *L, CppTable ret)
	{
		table t(L, ret);
		t.m_obj->inc_ref();
	}

	template<>
		LUAWARP_API void push(lua_State *L, table ret)
	{
		lua_pushvalue(L, ret.m_obj->m_index);
	}
	
	template<> LUAWARP_API  void push( lua_State *L, KPtrObjcet PtrObj )
	{
		pushPtrObject( L, PtrObj.pObject, PtrObj.strObjectName.c_str() );
	}
	
	template<> LUAWARP_API  void push(lua_State *L, l_stackval ret)
	{
		lua_pushvalue(L, ret.index);
	}

	template<> LUAWARP_API void pop(lua_State *L)
	{
		lua_pop(L, 1);
	}

	template<> LUAWARP_API table pop(lua_State *L)
	{
		return table(L, lua_gettop(L));
	}

	///*---------------------------------------------------------------------------*/ 
	///* return a value and set stack                                              */ 
	///*---------------------------------------------------------------------------*/ 
	template<>
		void retset<void>(lua_State *L, int nTop)
	{
		lua_settop(L, nTop);
	}

	///*---------------------------------------------------------------------------*/ 
	///* 解析表函数名，支持表嵌套                                                  */ 
	///*---------------------------------------------------------------------------*/ 
	int getsctnum(const char* src, int ch)
	{
		int retval = 0;

		for (; *src; src++) if (*src == (char)ch) retval ++;

		return retval;
	}

	char* getsctstr(const char* src, int ch, int pos, char* dst)
	{
		for (int i=0; *src && i<pos; src++) if (*src == (char)ch) i ++;

		if (!*src) return NULL;

		char* retval = dst;
		for (; *src && (*src != (char)ch); src++, dst++) *dst = *src;
		*dst = 0;

		return retval;
	}

	// 判断脚本中有没有指定函数, 支持嵌套表中的函数
	bool hasfunction(lua_State* L, const char* tfname)
	{
		char buf[256];
		sprintf_k(buf, sizeof(buf), "%s", tfname);

		char* ss[64];
		int ns = split(buf, ".:", ss, 64);
		if(ns < 1) return false;
		
		int sp = lua_gettop(L);

		for (int i=0; i<ns-1; i++)
		{
			char* ptname = ss[i];
			if (!ptname[0])
			{
				lua_settop(L, sp);
				return false;
			}

			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				lua_settop(L, sp);
				return false;
			}
		}

		char* pfname = ss[ns-1];
		if(!pfname[0])
		{
			lua_settop(L, sp);
			return false;
		}

		lua_pushstring(L, pfname);
		ns == 1 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			lua_settop(L, sp);
			return false;
		}

		lua_settop(L, sp);
		return true;
	}

	static void invoke_parent(lua_State *L)
	{
																				// srctable
		lua_pushstring(L, "__parent");											// srctable "__parent"
		lua_rawget(L, -2);														// srctable parenttable
		if(lua_istable(L,-1))
		{
			lua_pushvalue(L,2);													// srctable parenttable key
			lua_rawget(L, -2);													// srctable parenttable value
			if(!lua_isnil(L,-1))
			{
				lua_remove(L,-2);												// srctable value
			}
			else
			{
				lua_remove(L, -1);												// scrtable parenttable
				invoke_parent(L);												// srctable parenttable value
				lua_remove(L,-2);												// srctable parenttable
			}
		}
	}

	//template<typename RVal> 
	//void push_functor(lua_State *L, RVal* s)
	//{

	//};
	LUAWARP_API int RetValue_(lua_State *L)
	{
		unsigned long n = (unsigned long)lua_tonumber(L, -1);
		if (n == cn_MtlRetFlag)
		{
			lua_pop(L, 1);
			int m = (int)lua_tonumber(L, -1);
			lua_pop(L, 1);
			return m;
		}
		else if (n == cn_TabRetFlag)
		{
			lua_pop(L, 1);
		}

		return 1;
	}

	LUAWARP_API int meta_get(lua_State *L)
	{
		LMREG::KLuaState_UD& ud = *(LMREG::KLuaState_UD*)G(L)->ud;
		KLuaWrap* pThis = ud.m_this;									// userdata key
		lua_getmetatable(L,1);											// userdata key metatable
		lua_pushvalue(L,2);												// userdata key metatable key
		lua_rawget(L,-2);												// userdata key metateble value

		if(lua_isuserdata(L,-1))
		{
			user2type<var_base*>::invoke(L,-1)->get(L);					// userdata key metatable  value retvalue
			lua_remove(L, -2);											// userdata key metatable retvalue
		}
		else if(lua_isnil(L,-1))
		{
			lua_remove(L,-1);											// userdata key metatable
			invoke_parent(L);											// userdata key metatable retvalue
			if(lua_isuserdata(L,-1))
			{
				user2type<var_base*>::invoke(L,-1)->get(L);					// userdata key metatable  value retvalue
				lua_remove(L, -2);											// userdata key metatable retvalue
			}
			else if(lua_isnil(L,-1))
			{
				lua_remove(L,-1);									// userdata key metatable
				pThis->pushExtendTable();							// userdata key metatable extendtable
				if(!lua_isnil(L, -1))
				{
					lua_pushvalue(L, 1);							// userdata key metatable extendtable userdata
					lua_gettable(L, -2);							// userdata key metatable extendtable value(table or nil)
					if(!lua_isnil(L, -1))
					{
						// 不用检查是否是table，我保证插入的都是table
						lua_pushvalue(L, 2);						// userdata key metatable extendtable value(table) key
						lua_gettable(L, -2);						// userdata key metatable extendtable value(table) retvalue
						lua_remove(L, -2);							// userdata key metatable extendtable retvalue
					}
					lua_remove(L, -2);								// userdata key metatable retvalue
				}
			}
		} 

		lua_remove(L,-2);												// userdata key retvalue

		return 1;
	}

	LUAWARP_API int meta_set(lua_State *L)
	{
		LMREG::KLuaState_UD& ud = *(LMREG::KLuaState_UD*)G(L)->ud;
		KLuaWrap* pThis = ud.m_this;									// userdata key
		lua_getmetatable(L,1);											// userdata key value metatable
		lua_pushvalue(L,2);												// userdata key value metatable key
		lua_rawget(L,-2);												// userdata key value metateble value

		if(lua_isuserdata(L,-1))
		{
			user2type<var_base*>::invoke(L,-1)->set(L);					// userdata key value metateble value retvalue(maybe)
		}
		else if(lua_isnil(L, -1))
		{
			lua_remove(L,-1);											// userdata kay value metatable
			invoke_parent(L);											// userdata kay value metatable value

			if(lua_isuserdata(L,-1))
			{
				user2type<var_base*>::invoke(L,-1)->set(L);					// userdata key value metateble value retvalue(maybe)
			}
			else if(lua_isnil(L, -1))
			{
				lua_remove(L,-1);
				pThis->pushExtendTable();									// userdata key value metatable extendtable
				if(!lua_isnil(L, -1))
				{
					// exsit
					lua_pushvalue(L, 1);							// userdata key value metatable extendtable userdata
					lua_gettable(L, -2);							// userdata key value metatable extendtable table(nil)
					if(lua_isnil(L, -1))
					{
						lua_remove(L,-1);							// userdata key value metatable extendtable
						lua_newtable(L);							// userdata key value metatable extendtable newtable userdata newtable
						lua_pushvalue(L, 1);						// userdata key value metatable extendtable newtable userdata
						lua_pushvalue(L, -2);							// userdata key value metatable extendtable newtable userdata newtable
						lua_settable(L, -4);						// userdata key value metatable extendtable newtable(table)
					}
					lua_pushvalue(L,2);								// userdata key value metatable extendtable newtable(table) key
					lua_pushvalue(L,3);								// userdata key value metatable extendtable newtable(table) key value
					lua_settable(L, -3);

				}
			}
		}
		lua_settop(L, 3);										// userdata key value
		return 0;
	}

	LUAWARP_API void push_meta(lua_State *L, const char* name)
	{
		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);
	}

	table_obj::table_obj(lua_State* L, int index)
		:m_L(L)
		,m_index(index)
		,m_ref(0)
	{
		m_pointer = lua_topointer(m_L, m_index);
	}

	table_obj::~table_obj()
	{
		if(validate())
		{
			lua_remove(m_L, m_index);
		}
	}

	void table_obj::inc_ref()
	{
		++m_ref;
	}

	void table_obj::dec_ref()
	{
		if(--m_ref == 0)
			delete this;
	}

	bool table_obj::validate()
	{
		if(m_pointer != NULL)
		{
			if(m_pointer == lua_topointer(m_L, m_index))
			{
				return true;
			}
			else
			{
				int top = lua_gettop(m_L);

				for(int i=1; i<=top; ++i)
				{
					if(m_pointer == lua_topointer(m_L, i))
					{
						m_index = i;
						return true;
					}
				}

				m_pointer = NULL;
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	table::table(lua_State* L)
	{
		lua_newtable(L);
		m_obj = new table_obj(L, lua_gettop(L));

		m_obj->inc_ref();
	}

	table::table(lua_State* L, const char* name)
	{
		lua_pushstring(L, name);
		lua_gettable(L, LUA_GLOBALSINDEX);

		if(lua_istable(L, -1) == 0)
		{
			lua_pop(L, 1);

			lua_newtable(L);
			lua_pushstring(L, name);
			lua_pushvalue(L, -2);
			lua_settable(L, LUA_GLOBALSINDEX);
		}

		m_obj = new table_obj(L, lua_gettop(L));
	}

	table::table(lua_State* L, const CppTable& dataset)
	{
		lua_newtable(L);
		m_obj = new table_obj(L, lua_gettop(L));
		m_obj->inc_ref();

		int c = dataset.size();
		for(int i=0; i<c; i++)
		{
			const CppTable::NameValue& nv = dataset.at(i);
			
			const char* name = nv.first;
			const variant& val = nv.second;

			switch(val.vt)
			{
			case variant::vt_nil:
				break;
			case variant::vt_bool:
				this->set(name, val.b);
				break;
			case variant::vt_int32:
				this->set(name, val.n);
				break;
			case variant::vt_uint32:
				this->set(name, val.un);
				break;
			case variant::vt_int64:
				this->set(name, val.ll);
				break;
			case variant::vt_uint64:
				this->set(name, val.ull);
				break;
			case variant::vt_float:
				this->set(name, val.f);
				break;
			case variant::vt_double:
				this->set(name, val.d);
				break;
			case variant::vt_pointer:
				this->set(name, val.p);
				break;
			case variant::vt_string:
				{
					lua_pushstring(L, name);
					lua_pushlstring(L, val.str.second, val.str.first);
					lua_settable(L, m_obj->m_index);
				} break;
			case variant::vt_d_string:
				{
					lua_pushstring(L, name);
					lua_pushlstring(L, val.dstr.second, val.dstr.first);
					lua_settable(L, m_obj->m_index);
				} break;
			case variant::vt_table:
				{
					table tb(L, *val.tab);
					this->set(name, tb);
				} break;
			case variant::vt_object:
				{
					this->setobject(name, val.obj.second, val.obj.first);
				} break;
			}
		}
	}

	table::table(lua_State* L, int index)
	{
		if(index < 0)
		{
			index = lua_gettop(L) + index + 1;
		}

		m_obj = new table_obj(L, index);

		m_obj->inc_ref();
	}

	table::table(const table& input)
	{
		m_obj = input.m_obj;

		m_obj->inc_ref();
	}

	table::~table()
	{
		m_obj->dec_ref();
	}

	// 把最上层基类压入到栈中
	// 类似 invoke_parent ,目前是专用函数，不检查 index 是否正确
	void pushBaserootMeta(lua_State *L, int index)
	{																				// ***
		lua_pushvalue(L, index);													// srctable
		lua_pushstring(L, "__parent");												// srctable "__parent"
		lua_gettable(L, -2);														// srctable parenttable
		if(!lua_isnil(L, -1))
		{
			pushBaserootMeta(L, -1);												// srctable parenttable srctable(top)
			lua_remove(L, -2);														// srctable srctable(top/baseroot)
			lua_remove(L, -2);														// srctable(top/baseroot)
		}
		else
		{
			lua_remove(L, -1);														// srctable(baseroot)
		}
																					// *** srctable
	}

	// 判断2个metatable是否直接有继承关系(单一继承树)。配合 CLASSMETA_INHERITLEVEL 属性.
	// 1 是则返回true， 否则false; 2 同一table会返回 true
	bool isInheritDirect(lua_State *L, int meta_index1, int meta_index2)
	{
		int top = lua_gettop(L);
																					// begin
		lua_rawgeti(L, meta_index1, CLASSMETA_INHERITLEVEL);						// begin CLASSMETA_INHERITLEVEL
		int level1 = (int)lua_tonumber(L, -1);											// begin CLASSMETA_INHERITLEVEL
		lua_pop(L, 1);																// begin
		lua_rawgeti(L, meta_index2, CLASSMETA_INHERITLEVEL);						// begin CLASSMETA_INHERITLEVEL
		int level2 = (int)lua_tonumber(L, -1);											// begin CLASSMETA_INHERITLEVEL
		lua_pop(L, 1);																// begin
		if(level1 == level2)														// begin
		{
			return lua_equal(L, meta_index1, meta_index2) != 0;
		}																
		int base_index = meta_index1, inherit_index = meta_index2;					// ... ...
		int index_from = level2, index_to = level1;
		if(level1 > level2)
		{
			base_index = meta_index2, inherit_index = meta_index1;
			index_from = level1, index_to = level2;
		}																			// begin
		lua_pushvalue(L, base_index);												// begin base_table
		lua_pushstring(L, "__parent");												// begin base_table "__parent"
		lua_pushvalue(L, inherit_index > 0? inherit_index : inherit_index - 2);											// begin base_table "__parent" inherit_table
		for(int i = index_from; i > index_to; --i)									// ... ...
		{
			lua_pushvalue(L, -2);													// begin base_table "__parent" inherit_table "__parent"
			lua_gettable(L, -2);													// begin base_table "__parent" inherit_table parent_table
			lua_remove(L, -2);														// begin base_table "__parent" parent_table("inherit_table")
		}
		bool ret = false;
		if(lua_equal(L, -3, -1) != 0)
		{
			ret = true;
		}

		lua_settop(L, top);															// begin(呵呵，不自信 + 更高效)

		return ret;
	}

	///* 1 这里为 class meta 追加了一个 ptr2user 的表,使用固定位置 CLASSMETA_PTR2USERDATA
	//   2 (2010.6.4)由于继承关系，接口通常抛出 基类 类型关系，所以 userdata 会重新创建，这样扩展数据就一并实效了，所以尽量让继承关系使用同一张表
	//*/
	void pushPtrObject(lua_State *L, void* input, const char* metaname)
	{
		// 没有注册的不允许运行到这里
		ASSERT(metaname != 0);
		//																			// (top)	
		push_meta(L, metaname);														// metatable
		ASSERT(lua_istable(L, -1));
		lua_rawgeti(L, -1, (int)CLASSMETA_PTR2USERDATA);							// metatable ptrtable
		if(lua_isnil(L, -1))
		{
			lua_remove(L, -1);														// metatable
			pushBaserootMeta(L, -1);												// metatable rootmatetable
			lua_rawgeti(L, -1, (int)CLASSMETA_PTR2USERDATA);						// metatable rootmatetable ptrtable
			if (lua_isnil(L, -1))
			{
				lua_remove(L, -1);													// metatable rootmatetable
				// 创建
				lua_newtable(L);													// metatable rootmatetable ptrtable
				lua_newtable(L);													// metatable rootmatetable ptrtable table
				lua_pushstring(L, "__mode");										// metatable rootmatetable ptrtable table "__mode"
				lua_pushstring(L, "v");												// metatable rootmatetable ptrtable table "__mode" "v"
				lua_settable(L, -3);												// metatable rootmatetable ptrtable table(value weak table)
				lua_setmetatable(L, -2);											// metatable rootmatetable ptrtable
				lua_pushvalue(L, -1);												// metatable rootmatetable ptrtable ptrtable
				lua_rawseti(L, -3, CLASSMETA_PTR2USERDATA);							// metatable rootmatetable ptrtable
			}
			lua_remove(L, -2);														// metatable ptrtable
			lua_pushvalue(L, -1);													// metatable ptrtable ptrtable
			lua_rawseti(L, -3, CLASSMETA_PTR2USERDATA);								// metatable ptrtable
		}
		// 不是 table ，被占用，这是不允许的
		ASSERT(lua_istable(L, -1));
		
		// 这里保证 ptrtable 一定在
		lua_pushlightuserdata(L, input);											// lua_pushnumber(L, (int)input);												// metatable ptrtable ptrkey
		lua_gettable(L, -2);														// metatable ptrtable ptruserdata
		if(lua_isnil(L, -1))
		{
			lua_remove(L, -1);														// metatable ptrtable
			lua_pushlightuserdata(L, input);										//lua_pushnumber(L, (int)input);											// metatable ptrtable ptrkey
			new (lua_newuserdata(L, sizeof(voidPtr2user))) voidPtr2user(input);		// metatable ptrtable ptrkey ptruserdata
			lua_pushvalue(L, -4);													// metatable ptrtable ptrkey ptruserdata metatable
			lua_setmetatable(L, -2);												// metatable ptrtable ptrkey ptruserdata
			lua_insert(L, -2);														// metatable ptrtable ptruserdata ptrkey
			lua_pushvalue(L, -2);													// metatable ptrtable ptruserdata ptrkey ptruserdata
			lua_settable(L, -4);													// metatable ptrtable ptruserdata
		}
		else
		{
			lua_getmetatable(L, -1);												// metatable ptrtable ptruserdata udmetatable
			if(lua_equal(L, -4, -1) == 0)											// ... ... 判断 2 metable 是否一致
			{																		// ... ... 类型不一致,需要判定使用哪种类型
				bool isInhertDirect = isInheritDirect(L, -1, -4);					// ... ...
				if(isInhertDirect)													
				{																	// ... ... 有继承关系,选择level最高的

					// 使用继承书中层级高的子类meta
					lua_rawgeti(L, -4, CLASSMETA_INHERITLEVEL);						// metatable ptrtable ptruserdata udmetatable metatableinherlevel
					int toLevel = lua_isnil(L, -1) ? 0 : (int)lua_tonumber(L, -1);
					lua_pop(L, 1);													// metatable ptrtable ptruserdata udmetatable
					lua_rawgeti(L, -1, CLASSMETA_INHERITLEVEL);						// metatable ptrtable ptruserdata udmetatable usermetainhertlevel
					int udLevel = lua_isnil(L, -1) ? 0 : (int)lua_tonumber(L, -1);
					lua_pop(L, 1);													// metatable ptrtable ptruserdata udmetatable
					if(toLevel > udLevel)
					{
						lua_pushvalue(L, -4);										// metatable ptrtable ptruserdata udmetatable metatable
						lua_setmetatable(L, -3);									// metatable ptrtable ptruserdata(metachanged) udmetatable
					}
				}																	// metatable ptrtable ptruserdata udmetatable
				else
				{																	//  metatable ptrtable ptruserdata udmetatable 没有继承关系,使用最新的
					lua_pushvalue(L, -4);											// metatable ptrtable ptruserdata udmetatable metatable
					lua_setmetatable(L, -3);										// metatable ptrtable ptruserdata(metachanged) udmetatable

				}																	// metatable ptrtable ptruserdata udmetatable
			}
			lua_pop(L, 1);															// metatable ptrtable ptruserdata
		}																			// metatable ptrtable ptruserdata
		lua_remove(L, -2);															// metatable ptruserdata
		lua_remove(L, -2);															// ptruserdata
	}

	LUAWARP_API bool get_func( lua_State* L, const char* tfname )
	{
		char* ss[16];
		char buf[128];
		strcpy_k(buf, sizeof(buf), tfname);
		int ns = split(buf, '.', ss, sizeof(ss) / sizeof(ss[0]));

		// 		char* ss2[2];
		// 		char* lasts = ss[ns-1];
		// 		int ns2 = split(lasts, ':', ss2, 2);
		// 		if(ns2 == 2)
		// 		{
		// 			ss[ns-1] = ss2[0];
		// 			ss[ns] = ss2[1];
		// 			ns += 1;
		// 		}

		int sp = lua_gettop(L);

		// int errfunc = LMREG::lua_geterrfunc(L);

		int i = 0;
		for(; i < ns - 1; ++i)
		{
			char* ptname = ss[i];
			lua_pushstring(L, ptname);
			i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				print_error(L, "LMREG::get_func() attempt a table `%s' (not a table), fullpath: %s", ptname, tfname);
				lua_settop(L, sp);
				lua_pushnil(L);
				return false;
			}
		}

		char* pfname = ss[ns-1];
		lua_pushstring(L, pfname);

		i == 0 ? lua_gettable(L, LUA_GLOBALSINDEX) : lua_gettable(L, -2);
		if (!lua_isfunction(L, -1))
		{
			print_error(L, "LMREG::get_func() attempt to call table function `%s' (not a function), fullpath: %s", pfname, tfname);
			lua_settop(L, sp);
			lua_pushnil(L);
			return false;
		}

		return true;
	}
} // name space

//DWORD ELFHash(const char*str)
//{
//	unsigned int hash = 0;
//	unsigned int x = 0;
//	while(*str)
//	{
//		hash = (hash<<4) + (*str++);
//		if((x = hash&0xF0000000L) != 0)
//		{
//			hash ^= (x>>24);
//			hash &= ~x;
//		}
//	}
//	return (hash & 0x7FFFFFFF);
//}

char** spilwhit(char* str, char* strsp, DWORD* buf, int buflen, int& ct)
{
	if(!str || !buf) return NULL;
	int splen = (int)strlen(strsp), k = 0;
	char *s = str, *p = str;
	ct = 0;
	char** pr = (char**)buf;
	while(*s)
	{
		if(strstr(s, strsp) == s) //if (FindChars(s, strsp, 0) == 0)
		{
			pr[ct] = p;
			*s=0;
			s+=splen;
			p=s;
			++ct;
			if (ct==buflen) return pr;
		}
		++s;
	}
	pr[ct]=p;
	++ct;
	return pr;
}

void Utc2Local(int& _y, int& _m, int& _d, int& _h)
{
	#define IsLeapYear(uiYear) ((((uiYear)%4)==0)&&(((uiYear)%100)!=0))||(((uiYear)%400)==0)

#if defined(WIN32) || defined(WINDOWS)
	TIME_ZONE_INFORMATION tzi;
	DWORD dwRet(GetTimeZoneInformation(&tzi));
	int iTimeZone((0-tzi.Bias)/60);
#else
	int iTimeZone = -timezone/60/60;
#endif

	_h += iTimeZone;

	int hinv = 0;
	if (_h>24) 
	{
		_h = _h - 24;
		hinv = 1;
	}
	else if (_h<0)
	{
		_h = _h + 24;
		hinv = - 1;
	}

	if(hinv != 0)
	{
		_d += hinv;

		int iDays(0);

		switch(_m)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			{
				iDays = 31;
			}
			break;
		case 2:
			{
				iDays = IsLeapYear(_y)?29:28;
			}
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			{
				iDays = 30;
			}
			break;
		}

		int _pm = _m-1;
		if (!_pm) _pm = 12;
		int _pd = 0;
		switch(_pm)
		{
		case 1:
		case 3:
		case 5:
		case 7:
		case 8:
		case 10:
		case 12:
			{
				_pd = 31;
			}
			break;
		case 2:
			{
				_pd = IsLeapYear(_y)?29:28;
			}
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			{
				_pd = 30;
			}
			break;
		}

		if (_d > iDays)
		{
			_m+=1;
			if (_m > 12)
			{
				_m = 1;
				_y+=1;
			}
		}
		else if (_d < 1)
		{
			_d = _pd;
			_m-=1;
			if (!_m)
			{
				_m=12;
				_y-=1;
			}
		}
	}
}

int KLuaWrap::SetPrintOutput( FILE* f )
{
	return setPrintOutput(f);
}