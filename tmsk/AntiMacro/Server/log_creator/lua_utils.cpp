#include <lua/KLuaWrap.h>
#include <time.h>

void registLuaUtils();

static void help()
{
	Log(LOG_CONSOLE, "login(const char* server, const char* acct, const char* player, const char* extra)");
	Log(LOG_CONSOLE, "logout(const char* server, const char* acct, const char* extra)");
	Log(LOG_CONSOLE, "trade(const char* server, const char* srcAcct, const char* targetAcct, const char* extra)");
}

static void login(const char* server, const char* acct, const char* player, const char* extra)
{
	Log(LOG_ERROR, "login server=%s account=%s player=%s %s", server, acct, player, extra);
}

static void logout(const char* server, const char* acct, const char* extra)
{
	Log(LOG_ERROR, "logout server=%s account=%s %s", server, acct, extra);
}

static void trade(const char* server, const char* srcAcct, const char* targetAcct, const char* extra)
{
	Log(LOG_ERROR, "trade server=%s source=%s target=%s %s", server, srcAcct, targetAcct, extra);
}

void registLuaUtils()
{
	LuaWraper["help"] = help;
	LuaWraper["login"] = login;
	LuaWraper["logout"] = logout;
	LuaWraper["trade"] = trade;
}
