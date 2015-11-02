#include "l_mysql.h"

//// l_mysqlConnection

l_mysqlConnection::l_mysqlConnection()
{
	m_sqlCmd.SetConnection(m_sqlCnn);
}

l_mysqlConnection::~l_mysqlConnection()
{

}

bool l_mysqlConnection::connect(const char* host, int port, const char* dbname, const char* user, const char* passwd)
{
	KSQLConnParam cnnParam;
	cnnParam.SetHost(host);
	cnnParam.SetServerPort(port);
	cnnParam.SetDbName(dbname);
	cnnParam.SetUserName(user);
	cnnParam.SetPassword(passwd);

	m_sqlCnn.Close();

	m_cnnParam = cnnParam;
	return !!m_sqlCnn.Connect(m_cnnParam);
}

bool l_mysqlConnection::reconnect()
{
	this->close();
	return !!m_sqlCnn.Connect(m_cnnParam);
}

void l_mysqlConnection::close()
{
	m_sqlCnn.Close();
}

bool l_mysqlConnection::isReady() const
{
	return m_sqlCnn.m_pMysql != NULL;
}

bool l_mysqlConnection::checkAlive()
{
	return !!m_sqlCnn.IsActive();
}

l_mysqlResultSet* l_mysqlConnection::query(const char* sql)
{
	l_mysqlResultSet* rs = new l_mysqlResultSet();
	if(!m_sqlCmd.ExecuteQuery(sql, rs->m_resultSet))
	{
		delete rs;
		return NULL;
	}

	KSQLResultSet& resultSet = rs->m_resultSet;
	if(!resultSet.Begin())
	{
		int errCode = resultSet.GetLastError();
		if(errCode == MYSQL_NO_DATA)
		{
			rs->m_noDataFound = true;
			return rs;
		}else
		{
			delete rs;
			return NULL;
		}
	}
	return rs;
}

bool l_mysqlConnection::execute(const char* sql)
{
	return !!m_sqlCmd.Execute(sql);
}

const char* l_mysqlConnection::getLastError()
{
	return m_sqlCmd.GetLastErrorMsg();
}

//// l_mysqlResultSet

l_mysqlResultSet::l_mysqlResultSet():m_noDataFound(false)
{

}

l_mysqlResultSet::~l_mysqlResultSet()
{

}

LMREG::variant* l_mysqlResultSet::get(int colIndex)
{
	int colCount = m_resultSet.m_colNum;
	if(colIndex < 0 || colIndex >= colCount)
		return NULL;

	int dtype = 0;
	int len = 0;
	void* valp = NULL;

	if(!m_resultSet.GetColumnValue(colIndex, &dtype, &valp, &len))
		return NULL;

	LMREG::variant& varVal = m_columnValue;

	if(!len && !valp) // nil value
	{
		varVal.setnil();
		return &m_columnValue;
	}

	switch(dtype)
	{
	case KSQLTypes::sql_c_int8:
		{
			varVal = (int)*(INT8*)valp;
			break;
		}	
	case KSQLTypes::sql_c_uint8:
		{
			varVal = (int)*(UINT8*)valp;
			break;
		}
	case KSQLTypes::sql_c_int16:
		{
			varVal = (int)*(INT16*)valp;
			break;
		}
	case KSQLTypes::sql_c_uint16:
		{
			varVal = (int)*(UINT16*)valp;
			break;
		}
	case KSQLTypes::sql_c_int32:
		{
			varVal = (int)*(INT32*)valp;
			break;
		}
	case KSQLTypes::sql_c_uint32:
		{
			varVal = *(UINT32*)valp;
			break;
		}
	case KSQLTypes::sql_c_int64:
		{
			varVal = (double)(INT64)*(INT64*)valp;
			break;
		}
	case KSQLTypes::sql_c_uint64:
		{
			varVal = (double)(UINT64)*(UINT64*)valp;
			break;
		}
	case KSQLTypes::sql_c_float32:
		{
			varVal = (float)*(float*)valp;
			break;
		}
	case KSQLTypes::sql_c_float64:
		{
			varVal = (double)*(double*)valp;
			break;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			MYSQL_TIME_K t = *(MYSQL_TIME_K*)valp;
			KDatetime dtime(t);
			varVal = dtime.ToString().c_str();
			break;
		}
	case KSQLTypes::sql_c_string:
		{
			varVal.setstring((char*)valp, len);
			break;
		}
	case KSQLTypes::sql_c_varstring:
		{
			varVal.setstring((char*)valp, len);
			break;
		}
	case KSQLTypes::sql_c_binary:
		{
			varVal.setbinary(valp, len);
			break;
		}
	default:
		{
			ASSERT(FALSE);
			break;
		}
	}
	
	return &m_columnValue;
}

LMREG::variant* l_mysqlResultSet::get(const char* colName)
{
	int colIndex = m_resultSet.GetColumnIndex(colName);
	return this->get(colIndex);
}

bool l_mysqlResultSet::next()
{
	return !!m_resultSet.Next();
}

int l_mysqlResultSet::get_l(lua_State* L)
{
	int colIndex = -1;
	const char* colName = NULL;
	l_mysqlResultSet* rs = NULL;

	if(!lua_isuserdata(L, 1) || lua_islightuserdata(L, 1))
	{
		luaL_argerror(L, 1, "must be l_mysqlResultSet object");
		return 0;
	}

	rs = LMREG::read<l_mysqlResultSet*>(L, 1);
	if(!rs)
	{
		luaL_argerror(L, 1, "must be l_mysqlResultSet object");
		return 0;
	}

	if(lua_isstring(L, 2))
	{
		colName = lua_tostring(L, 2);
		if(!colName)
		{
			luaL_argerror(L, 2, "column index or column name expected.");
			return 0;
		}
	}
	else if(lua_isnumber(L, 2))
	{
		colIndex = lua_tointeger(L, 2);
		if(colIndex < 0)
		{
			luaL_argerror(L, 2, "column index or column name expected.");
			return 0;
		}
	}
	else
	{
		luaL_argerror(L, 2, "column index or column name expected.");
		return 0;
	}

	if(colName)
		colIndex = rs->m_resultSet.GetColumnIndex(colName);

	if(colIndex < 0 || colIndex >= rs->m_resultSet.m_colNum)
	{
		luaL_argerror(L, 2, "unknown column index or column name.");
		return 0;
	}

	LMREG::variant* val = rs->get(colIndex);
	LMREG::push<LMREG::variant>(L, *val);

	return 1;
}

bool l_mysqlResultSet::regMethodToLua(lua_State* L)
{
	LMREG::push_meta(L, LMREG::class_name<l_mysqlResultSet>::name());
	if(lua_istable(L, -1))
	{
		lua_pushstring(L, "get");
		lua_pushcclosure(L, l_mysqlResultSet::get_l, 0);
		lua_rawset(L, -3);
	}
	lua_pop(L, 1);
	return true;
}
