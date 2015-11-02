#pragma once
#include <mysqldb/KDatabase.h>
#include <lua/KLuaWrap.h>

class l_mysqlResultSet;
class l_mysqlConnection
{
public:
	KSQLCommand m_sqlCmd;
	KSQLConnection m_sqlCnn;
	KSQLConnParam m_cnnParam;

public:
	l_mysqlConnection();
	~l_mysqlConnection();

public:
	bool connect(const char* host, int port, const char* dbname, const char* user, const char* passwd);
	bool reconnect();
	void close();

public:
	bool isReady() const;
	bool checkAlive();
	l_mysqlResultSet* query(const char* sql);
	bool execute(const char* sql);
	const char* getLastError();

public:
	BeginDefLuaClassA(l_mysqlConnection, mysql);
		DefMemberFunc(connect);
		DefMemberFunc(reconnect);
		DefMemberFunc(close);
		DefMemberFunc(isReady);
		DefMemberFunc(checkAlive);
		DefMemberFunc(query);
		DefMemberFunc(execute);
		DefMemberFunc(getLastError);
	EndDef;
};

class l_mysqlResultSet
{
public:
	bool m_noDataFound;
	KSQLResultSet m_resultSet;
	LMREG::variant m_columnValue;

public:
	l_mysqlResultSet();
	~l_mysqlResultSet();

public:
	bool empty() const { return m_noDataFound; }
	LMREG::variant* get(int colIndex);
	LMREG::variant* get(const char* colName);
	bool next();

private:
	static int get_l(lua_State* L);
	static bool regMethodToLua(lua_State* L);

public:
	BeginDefLuaClassA(l_mysqlResultSet, __cursor);
		DefMemberFunc(empty);
		DefMemberFunc(next);
		l_mysqlResultSet::regMethodToLua(L);
	EndDef;
};
