#include "KDatabase.h"
#include <stdio.h>

KSQLTypes::eCDataType KSQLTypes::MapToCType(enum_field_types sqlDatatype, DWORD fldFlags)
{
	switch(sqlDatatype)
	{
	case MYSQL_TYPE_DECIMAL:
		{
			return sql_c_float64;
		}
	case MYSQL_TYPE_TINY:
		{
			return (fldFlags&UNSIGNED_FLAG) ? sql_c_uint8 : sql_c_int8;
		}
	case MYSQL_TYPE_SHORT:
		{
			return (fldFlags&UNSIGNED_FLAG) ? sql_c_uint16 : sql_c_int16;
		}
	case MYSQL_TYPE_LONG:
		{
			return (fldFlags&UNSIGNED_FLAG) ? sql_c_uint32 : sql_c_int32;
		}
	case MYSQL_TYPE_FLOAT:
		{
			return sql_c_float32;
		}
	case MYSQL_TYPE_DOUBLE:
		{
			return sql_c_float64;
		}
	case MYSQL_TYPE_NULL:			return sql_c_invalid;
	case MYSQL_TYPE_TIMESTAMP:		return sql_c_timestamp;
	case MYSQL_TYPE_LONGLONG:
		{
			return (fldFlags&UNSIGNED_FLAG) ? sql_c_uint64 : sql_c_int64;
		}
	case MYSQL_TYPE_INT24:
		{
			return (fldFlags&UNSIGNED_FLAG) ? sql_c_uint32 : sql_c_int32;
		}
	case MYSQL_TYPE_DATE:			return sql_c_timestamp;
	case MYSQL_TYPE_TIME:			return sql_c_timestamp;
	case MYSQL_TYPE_DATETIME:		return sql_c_timestamp;
	case MYSQL_TYPE_YEAR:			return sql_c_int32;
	case MYSQL_TYPE_NEWDATE:		return sql_c_timestamp;
	case MYSQL_TYPE_VARCHAR:		return sql_c_varstring;
	case MYSQL_TYPE_BIT:			return sql_c_uint8;
	case MYSQL_TYPE_NEWDECIMAL:		return sql_c_float64;
	case MYSQL_TYPE_ENUM:			return sql_c_uint32;
	case MYSQL_TYPE_SET:			return sql_c_invalid;
	case MYSQL_TYPE_TINY_BLOB:		return sql_c_binary;
	case MYSQL_TYPE_MEDIUM_BLOB:	return sql_c_binary;
	case MYSQL_TYPE_LONG_BLOB:		return sql_c_binary;
	case MYSQL_TYPE_BLOB:			return sql_c_binary;
	case MYSQL_TYPE_VAR_STRING:		return sql_c_varstring;
	case MYSQL_TYPE_STRING:			return sql_c_string;
	case MYSQL_TYPE_GEOMETRY:		return sql_c_invalid;
	}
	return sql_c_invalid;
}

enum_field_types KSQLTypes::MapToSqlType(eCDataType ctype)
{
	switch(ctype)
	{
	case sql_c_int8:		return MYSQL_TYPE_TINY;
	case sql_c_uint8:		return MYSQL_TYPE_TINY;
	case sql_c_int16:		return MYSQL_TYPE_SHORT;
	case sql_c_uint16:		return MYSQL_TYPE_SHORT;
	case sql_c_int32:		return MYSQL_TYPE_LONG;
	case sql_c_uint32:      return MYSQL_TYPE_LONG;
	case sql_c_int64:       return MYSQL_TYPE_LONGLONG;
	case sql_c_uint64:      return MYSQL_TYPE_LONGLONG;
	case sql_c_float32:     return MYSQL_TYPE_FLOAT;
	case sql_c_float64:     return MYSQL_TYPE_DOUBLE;
	case sql_c_string:      return MYSQL_TYPE_STRING;
	case sql_c_varstring:   return MYSQL_TYPE_VAR_STRING;
	case sql_c_binary:      return MYSQL_TYPE_MEDIUM_BLOB;
	case sql_c_timestamp:   return MYSQL_TYPE_TIMESTAMP;
	}
	return (enum_field_types)-1;
}

BOOL KSQLTypes::IsUnsignedInteger(eCDataType ctype)
{
	switch(ctype)
	{
	case sql_c_int8:		return FALSE;
	case sql_c_uint8:		return TRUE;
	case sql_c_int16:		return FALSE;
	case sql_c_uint16:		return TRUE;
	case sql_c_int32:		return FALSE;
	case sql_c_uint32:      return TRUE;
	case sql_c_int64:       return FALSE;
	case sql_c_uint64:      return TRUE;
	case sql_c_float32:     return FALSE;
	case sql_c_float64:     return FALSE;
	case sql_c_string:      return FALSE;
	case sql_c_varstring:   return FALSE;
	case sql_c_binary:      return FALSE;
	case sql_c_timestamp:   return FALSE;
	}
	return FALSE;
}

int KSQLTypes::GetCDataWidth(eCDataType ctype)
{
	switch(ctype)
	{
	case sql_c_int8:		return 1;
	case sql_c_uint8:		return 1;
	case sql_c_int16:		return 2;
	case sql_c_uint16:		return 2;
	case sql_c_int32:		return 4;
	case sql_c_uint32:      return 4;
	case sql_c_int64:       return 8;
	case sql_c_uint64:      return 8;
	case sql_c_float32:     return 4;
	case sql_c_float64:     return 8;
	case sql_c_string:      return -1;
	case sql_c_varstring:   return -1;
	case sql_c_binary:      return -1;
	case sql_c_timestamp:   return sizeof(MYSQL_TIME);
	}
	return -1;
}

KSQLTypes::eCDataType KSQLTypes::MapToCType(const char* typeName)
{
	if(_stricmp(typeName, "int8") == 0)       return sql_c_int8;
	if(_stricmp(typeName, "uint8") == 0)      return sql_c_uint8;
	if(_stricmp(typeName, "int16") == 0)      return sql_c_int16;
	if(_stricmp(typeName, "uint16") == 0)     return sql_c_uint16;
	if(_stricmp(typeName, "int32") == 0)      return sql_c_int32;
	if(_stricmp(typeName, "uint32") == 0)     return sql_c_uint32;
	if(_stricmp(typeName, "int64") == 0)      return sql_c_int64;
	if(_stricmp(typeName, "uint64") == 0)     return sql_c_uint64;
	if(_stricmp(typeName, "float32") == 0)    return sql_c_float32;
	if(_stricmp(typeName, "float64") == 0)    return sql_c_float64;
	if(_stricmp(typeName, "string") == 0)     return sql_c_varstring;
	if(_stricmp(typeName, "binary") == 0)     return sql_c_binary;
	if(_stricmp(typeName, "timestamp") == 0)  return sql_c_timestamp;
	return sql_c_invalid;
}

KSQLTableField::KSQLTableField()
{
	m_length = 0;
	m_nullable = FALSE;
	m_autoIncrement = FALSE;
	m_primaryKey = FALSE;
	m_cdt = KSQLTypes::sql_c_invalid;
}

KSQLTableField::KSQLTableField(const KSQLTableField& o)
	: m_length(o.m_length),
	  m_nullable(o.m_nullable),
      m_autoIncrement(o.m_autoIncrement),
	  m_primaryKey(o.m_primaryKey),
	  m_cdt(o.m_cdt),
	  m_name(o.m_name),
	  m_defaultVal(o.m_defaultVal)
{
}

KSQLTableField::~KSQLTableField()
{
}

KSQLTableField& KSQLTableField::operator=(const KSQLTableField& o)
{
	if(this == &o) return *this;

	m_length = o.m_length;
	m_nullable = o.m_nullable;
	m_autoIncrement = o.m_autoIncrement;
	m_primaryKey = o.m_primaryKey;
	m_cdt = o.m_cdt;
	m_name = o.m_name;
	m_defaultVal = o.m_defaultVal;

	return *this;
}

KSQLTableSchema::KSQLTableSchema()
{

}

KSQLTableSchema::KSQLTableSchema(const KSQLTableSchema& o)
	: m_name(o.m_name)
	, m_dbName(o.m_dbName)
	, m_fields(o.m_fields)
{

}

KSQLTableSchema::~KSQLTableSchema()
{

}

KSQLTableSchema& KSQLTableSchema::operator=(const KSQLTableSchema& o)
{
	if(this == &o) return *this;
	m_name = o.m_name;
	m_dbName = o.m_dbName;
	m_fields = o.m_fields;
	return *this;
}

KSQLTableSchema& KSQLTableSchema::Append(const KSQLTableField& field)
{
	for(int i=0; i<m_fields.size(); i++)
	{
		const KSQLTableField& fld = m_fields[i];
		if(fld.m_name == field.m_name)
		{
			ASSERT(FALSE);
		}
	}

	int idx = m_fields.size();
	m_fields.push_back(field);

	return *this;
}

int KSQLTableSchema::GetFieldCount() const
{
	return m_fields.size();
}

int_r KSQLTableSchema::GetFieldIndex(const char* fieldName) const
{
	for(int i=0; i<m_fields.size(); i++)
	{
		const KSQLTableField& fld = m_fields[i];
		if(fld.m_name.icompare(fieldName) == 0)
			return i;
	}
	return -1;
}

const KSQLTableField* KSQLTableSchema::GetField(const char* fieldName) const
{
	for(int i=0; i<m_fields.size(); i++)
	{
		const KSQLTableField& fld = m_fields[i];
		if(fld.m_name.icompare(fieldName) == 0) return &fld;
	}
	return NULL;
}

const KSQLTableField* KSQLTableSchema::GetField(int fieldIndex) const
{
	if(fieldIndex < 0 || fieldIndex >= m_fields.size()) return NULL;
	return &m_fields[fieldIndex];
}

int KSQLTableSchema::GetPrimaryKeyFieldCount() const
{
	int count = 0;
	for(int i=0; i<m_fields.size(); i++)
	{
		const KSQLTableField& field = m_fields[i];
		if(!field.m_primaryKey) break;
		count++;
	}
	return count;
}

void KSQLTableSchema::Clear()
{
	m_name.clear();
	m_dbName.clear();
	m_fields.clear();
}

/// KSQLFieldDescribe
BOOL KSQLFieldDescribe::isNull() const
{
	return !!_isnull;
}

size_t KSQLFieldDescribe::getOutput(void* value, size_t len)
{
	size_t n = kmin((size_t)_actualLength, len);
	memcpy(value, _buffer, n);
	return n;
}

/// KSQLConnParam
KSQLConnParam::KSQLConnParam():m_serverPort(MYSQL_DEFAULT_PORT)
{
}

KSQLConnParam::KSQLConnParam(const KSQLConnParam& o)
	: m_serverPort(o.m_serverPort)
	, m_host(o.m_host)
	, m_dbName(o.m_dbName)
	, m_userName(o.m_userName)
	, m_passwd(o.m_passwd)
	, m_charset(o.m_charset)
{
}

KSQLConnParam::~KSQLConnParam()
{
}

KSQLConnParam& KSQLConnParam::operator=(const KSQLConnParam& o)
{
	if(this == &o) return *this;
	m_serverPort = o.m_serverPort;
	m_host = o.m_host;
	m_dbName = o.m_dbName;
	m_userName = o.m_userName;
	m_passwd = o.m_passwd;
	m_charset = o.m_charset;
	return *this;
}

void KSQLConnParam::SetHost(const char* host)
{
	m_host = host;
}

void KSQLConnParam::SetServerPort(unsigned int serverPort)
{
	m_serverPort = serverPort;
}

void KSQLConnParam::SetDbName(const char* dbName)
{
	m_dbName = dbName;
}

void KSQLConnParam::SetUserName(const char* userName)
{
	m_userName = userName;
}

void KSQLConnParam::SetPassword(const char* passwd)
{
	m_passwd = passwd;
}

void KSQLConnParam::SetCharset(const char* charset)
{
	m_charset = charset;
}

unsigned int KSQLConnParam::GetServerPort() const
{
	return m_serverPort;
}

const char* KSQLConnParam::GetHost() const
{
	return m_host.c_str();
}

const char* KSQLConnParam::GetDbName() const
{
	return m_dbName.c_str();
}

const char* KSQLConnParam::GetUserName() const
{
	return m_userName.c_str();
}

const char* KSQLConnParam::GetPasswd() const
{
	return m_passwd.c_str();
}

const char* KSQLConnParam::GetCharset() const
{
	return m_charset.c_str();
}

KSQLConnection::KSQLConnection():m_pMysql(NULL)
{
	m_connTimeout = 10;
	m_autoReconnect = TRUE;
	m_defCharset = g_encoding == encode_utf8 ? "utf8" : "gbk";
	m_errCode = 0;

	m_cnnLifetime = 2*60*60; // 缺省是一个小时重连一次
	m_cnnTime = 0;
}

KSQLConnection::~KSQLConnection()
{
	this->Close();
}

BOOL KSQLConnection::SetDefaultCharset(const char* charset)
{
	m_defCharset = charset;
	return TRUE;
}

BOOL KSQLConnection::SetAutoReconnect(BOOL autoReconnect)
{
	m_autoReconnect = autoReconnect;
	return TRUE;
}

BOOL KSQLConnection::SetConnectTimeout(int sec)
{
	m_connTimeout = sec;
	return TRUE;
}

void KSQLConnection::SetLifetime(int lifetime)
{
	m_cnnLifetime = lifetime;
}

BOOL KSQLConnection::KeepConnectionFresh()
{
	TIME_T now = (TIME_T)time(NULL);
	if(now - m_cnnTime > m_cnnLifetime)
	{
		this->Close();
		return this->Connect(m_connParam);
	}
	return TRUE;
}

BOOL KSQLConnection::IsActive() const
{
	if(!m_pMysql) return FALSE;
	return mysql_ping(m_pMysql) == 0;
}

BOOL KSQLConnection::Connect(const KSQLConnParam& connParam)
{
	this->Close();

	m_cnnTime = 0;
	m_connParam = connParam;

	m_pMysql = mysql_init(NULL);
	if(!m_pMysql)
	{
		this->AcquireErrorInfo();
		return FALSE;
	}

	const char* charset = m_defCharset.c_str();
	if(!connParam.m_charset.empty()) charset = connParam.GetCharset();
	
	if(mysql_options(m_pMysql, MYSQL_SET_CHARSET_NAME, charset))
	{
		this->AcquireErrorInfo();
		return FALSE;
	}

	unsigned long timeout = m_connTimeout;
	if(mysql_options(m_pMysql, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&timeout))
	{
		this->AcquireErrorInfo();
		return FALSE;
	}
	
	my_bool b = m_autoReconnect;
	if(mysql_options(m_pMysql, MYSQL_OPT_RECONNECT, (const char*)&b))
	{
		this->AcquireErrorInfo();
		return FALSE;
	}

	DWORD clientFlag = CLIENT_MULTI_STATEMENTS;
	if(!mysql_real_connect(m_pMysql,
					m_connParam.GetHost(),
					m_connParam.GetUserName(),
					m_connParam.GetPasswd(),
					m_connParam.GetDbName(),
					m_connParam.GetServerPort(),
					NULL, clientFlag))
	{
		this->AcquireErrorInfo();
		return FALSE;
	}

	mysql_options(m_pMysql, MYSQL_OPT_RECONNECT, (const char*)&b);
	m_cnnTime = (TIME_T)time(NULL);

	return TRUE;
}

BOOL KSQLConnection::Reconnect()
{
	this->Close();
	return this->Connect(m_connParam);
}

BOOL KSQLConnection::SelectDatabase(const char* dbName)
{
	if(!m_pMysql)
	{
		this->SetLastError(1, "you should first connect to database server");
		return FALSE;
	}
	if(mysql_select_db(m_pMysql, dbName))
	{
		this->AcquireErrorInfo();
		return FALSE;
	}
	return TRUE;
}

void KSQLConnection::Close()
{
	if(m_pMysql)
	{
		mysql_close(m_pMysql);
		m_pMysql = NULL;
		m_cnnTime = 0;
	}
}

int KSQLConnection::GetLastError() const
{
	return m_errCode;
}

const char* KSQLConnection::GetLastErrorMsg() const
{
	return m_errmsg.c_str();
}

BOOL KSQLConnection::QueryTableSchema(const char* tableName, KSQLTableSchema& tableSchema)
{
	ASSERT(m_pMysql);
	tableSchema.Clear();

	if(!m_pMysql)
	{
		this->SetLastError(1, "you should first connect to database server");
		return FALSE;
	}
	
	BOOL dbChanged = FALSE;
	const char* pDot = strchr(tableName, '.');
	if(pDot) // 包含数据库名
	{
		System::Collections::KString<64> db;
		db.assign(tableName, (int)(pDot-tableName));
		if(_stricmp(db.c_str(), m_connParam.m_dbName.c_str()))
		{
			if(!this->SelectDatabase(db.c_str()))
				return FALSE;
			dbChanged = TRUE;
		}
		tableName = pDot + 1;

		tableSchema.m_name = tableName;
		tableSchema.m_dbName = db;
	}
	else
	{
		tableSchema.m_name = tableName;
		tableSchema.m_dbName = m_connParam.m_dbName;
	}

	MYSQL_RES* pRs = mysql_list_fields(m_pMysql, tableName, "%");
	if(!pRs)
	{
		this->AcquireErrorInfo();
		if(dbChanged) this->SelectDatabase(m_connParam.m_dbName.c_str());
		return FALSE;
	}

	int fldNum = mysql_num_fields(pRs);
	MYSQL_FIELD* pflds = mysql_fetch_fields(pRs);
	for(int i=0; i<fldNum; i++)
	{
		KSQLTableField field;
		MYSQL_FIELD& fld = pflds[i];
		
		field.m_nullable = !(fld.flags & NOT_NULL_FLAG);
		field.m_autoIncrement = (fld.flags & AUTO_INCREMENT_FLAG);
		field.m_primaryKey = fld.flags & PRI_KEY_FLAG;
		field.m_cdt = KSQLTypes::MapToCType(fld.type, fld.flags);
		field.m_name.assign(fld.name, fld.name_length);
		if(fld.def) field.m_defaultVal.assign(fld.def, fld.def_length);
		
		switch(field.m_cdt)
		{
		case KSQLTypes::sql_c_int8:
		case KSQLTypes::sql_c_uint8:
		case KSQLTypes::sql_c_int16:
		case KSQLTypes::sql_c_uint16:
		case KSQLTypes::sql_c_int32:
		case KSQLTypes::sql_c_uint32:
		case KSQLTypes::sql_c_int64:
		case KSQLTypes::sql_c_uint64:
		case KSQLTypes::sql_c_float32:
		case KSQLTypes::sql_c_float64:
		case KSQLTypes::sql_c_timestamp:
			{
				field.m_length = KSQLTypes::GetCDataWidth(field.m_cdt);
				break;
			}
		case KSQLTypes::sql_c_string:
		case KSQLTypes::sql_c_varstring:
			{
				field.m_length = fld.length;
				break;
			}
		case KSQLTypes::sql_c_binary:
			{
				field.m_length = fld.length;
				if(field.m_length > 4096)
				{
					field.m_length = KSQLResultSet::def_binary_len;
				}
				break;
			}
		default:
			{
				this->SetLastError(1, "table %s field %s has an unexpected data type %d",
					tableName, field.m_name.c_str(), fld.type);
				return FALSE;
			}
		}
		tableSchema.Append(field);
	}

	if(dbChanged) this->SelectDatabase(m_connParam.m_dbName.c_str());
	return TRUE;
}

UINT64 KSQLConnection::GetLastAutoIncID()
{
	if(!m_pMysql)
	{
		this->SetLastError(1, "connect to database first");
		return 0;
	}
	UINT64 id = mysql_insert_id(m_pMysql);
	if(!id)
	{
		this->AcquireErrorInfo();
		return 0;
	}
	return id;
}

void KSQLConnection::ClearError()
{
	m_errCode = 0;
	m_errmsg.clear();
}

void KSQLConnection::AcquireErrorInfo()
{
	if(m_pMysql)
	{
		m_errCode = mysql_errno(m_pMysql);
		m_errmsg = mysql_error(m_pMysql);
	}
}

void KSQLConnection::AcquireStmtError(MYSQL_STMT* stmt)
{
	m_errCode = mysql_stmt_errno(stmt);
	m_errmsg = mysql_stmt_error(stmt);
}

void KSQLConnection::SetLastError(DWORD errCode, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
#if defined(WIN32) || defined(WINDOWS)
	int len = _vscprintf(fmt, args);
	if((int)m_errmsg.m_capacity < len+1)
	{
		int capacity = (len+1+1023) & (~1023);
		m_errmsg.resize(capacity);
	}
	vsprintf(m_errmsg.m_data, fmt, args);
#else
	vsnprintf(m_errmsg.m_data, m_errmsg.m_capacity-1, fmt, args);
#endif
	va_end(args);
}

KSQLCommand::KSQLCommand():m_pConn(NULL)
{
	m_buffer = NULL;
	m_bufferCapacity = 0;
	m_dynamicBuffer = NULL;
	m_binds = NULL;
	m_bindCapacity = 0;
	m_dynamicBinds = NULL;
	m_paramNum = 0;
	m_params = NULL;
	m_stmt = NULL;
}

KSQLCommand::KSQLCommand(KSQLConnection& conn):m_pConn(&conn)
{
	m_buffer = NULL;
	m_bufferCapacity = 0;
	m_dynamicBuffer = NULL;
	m_binds = NULL;
	m_bindCapacity = 0;
	m_dynamicBinds = NULL;
	m_paramNum = 0;
	m_params = NULL;
	m_stmt = NULL;
}

KSQLCommand::~KSQLCommand()
{
	this->Finalize();
}

void KSQLCommand::SetConnection(KSQLConnection& cnn)
{
	m_pConn = &cnn;
}

int KSQLCommand::GetLastError() const
{
	return m_pConn->GetLastError();
}

const char* KSQLCommand::GetLastErrorMsg() const
{
	return m_pConn->GetLastErrorMsg();
}

BOOL KSQLCommand::SetSQLStatement(const char* sql)
{
	int i = 0;
	while(i++ < 3)
	{
		this->Reset();
		m_sqlStatement = sql;
		if(this->Prepare()) return TRUE;
	}
	return FALSE;
}

BOOL KSQLCommand::DescribeParameters(KSQLFieldDescribe* describes, int num)
{
	m_paramNum = mysql_stmt_param_count(m_stmt);
	if(m_paramNum != num)
	{
		m_pConn->AcquireStmtError(m_stmt);
		return FALSE;
	}

	for(int i=0; i<num; i++)
	{
		KSQLFieldDescribe& describe = describes[i];
		describes->_isnull = 1; // 所有的参数初始化为NULL
		switch(describe.m_cdt)
		{
		case KSQLTypes::sql_c_int8:
		case KSQLTypes::sql_c_uint8:
		case KSQLTypes::sql_c_int16:
		case KSQLTypes::sql_c_uint16:
		case KSQLTypes::sql_c_int32:
		case KSQLTypes::sql_c_uint32:
		case KSQLTypes::sql_c_int64:
		case KSQLTypes::sql_c_uint64:
		case KSQLTypes::sql_c_float32:
		case KSQLTypes::sql_c_float64:
		case KSQLTypes::sql_c_timestamp:
			{
				describe.m_length = KSQLTypes::GetCDataWidth(describe.m_cdt);
				describe._actualLength = describe.m_length;
				break;
			}
		case KSQLTypes::sql_c_string:
		case KSQLTypes::sql_c_varstring:
			{
				// 貌似不用检查，否则写不进去空串，暂时先注掉 by Allen, 20140611
// 				int len = 1;
// 				if(describe.m_length < len)
// 				{
// 					m_pConn->SetLastError(1, "string parameter %s must has 1 byte width at least",
// 						describe.m_name.c_str());
// 					return FALSE;
// 				}
				describe.m_length += 1;
				break;
			}
		case KSQLTypes::sql_c_binary:
			{
				int len = 1;
				if(describe.m_length < len)
				{
					m_pConn->SetLastError(1, "binary parameter %s must has 1 byte width at least",
						describe.m_name.c_str());
					return FALSE;
				}
				break;
			}
		default:
			{
				m_pConn->SetLastError(1, "%s has an unknown data type %d", describe.m_name.c_str(), describe.m_cdt);
				return FALSE;
			}
		}
	}

	m_params = describes;
	if(!this->BindParameters()) return FALSE;
	return TRUE;
}

BOOL KSQLCommand::SetNull(const char* paramName)
{
	int paramIdx = this->GetParamIndex(paramName);
	if(paramIdx < 0)
	{
		m_pConn->SetLastError(1, "parameter %s not exists", paramName);
		return FALSE;
	}
	return this->SetNull(paramIdx);
}

BOOL KSQLCommand::SetInteger(const char* paramName, int value)
{
	int paramIdx = this->GetParamIndex(paramName);
	if(paramIdx < 0)
	{
		m_pConn->SetLastError(1, "parameter %s not exists", paramName);
		return FALSE;
	}
	return this->SetInteger(paramIdx, value);
}

BOOL KSQLCommand::SetInt64(const char* paramName, INT64 value)
{
	int paramIdx = this->GetParamIndex(paramName);
	if(paramIdx < 0)
	{
		m_pConn->SetLastError(1, "parameter %s not exists", paramName);
		return FALSE;
	}
	return this->SetInt64(paramIdx, value);
}

BOOL KSQLCommand::SetFloat(const char* paramName, float value)
{
	int paramIdx = this->GetParamIndex(paramName);
	if(paramIdx < 0)
	{
		m_pConn->SetLastError(1, "parameter %s not exists", paramName);
		return FALSE;
	}
	return this->SetFloat(paramIdx, value);
}

BOOL KSQLCommand::SetFloat64(const char* paramName, double value)
{
	int paramIdx = this->GetParamIndex(paramName);
	if(paramIdx < 0)
	{
		m_pConn->SetLastError(1, "parameter %s not exists", paramName);
		return FALSE;
	}
	return this->SetFloat64(paramIdx, value);
}

BOOL KSQLCommand::SetString(const char* paramName, const char* value, int len)
{
	int paramIdx = this->GetParamIndex(paramName);
	if(paramIdx < 0)
	{
		m_pConn->SetLastError(1, "parameter %s not exists", paramName);
		return FALSE;
	}
	return this->SetString(paramIdx, value, len);
}

BOOL KSQLCommand::SetBinary(const char* paramName, const void* value, int len)
{
	int paramIdx = this->GetParamIndex(paramName);
	if(paramIdx < 0)
	{
		m_pConn->SetLastError(1, "parameter %s not exists", paramName);
		return FALSE;
	}
	return this->SetBinary(paramIdx, value, len);
}

BOOL KSQLCommand::SetTimestamp(const char* paramName, time_t value)
{
	int paramIdx = this->GetParamIndex(paramName);
	if(paramIdx < 0)
	{
		m_pConn->SetLastError(1, "parameter %s not exists", paramName);
		return FALSE;
	}
	return this->SetTimestamp(paramIdx, value);
}

BOOL KSQLCommand::SetTimestamp(const char* paramName, const char* strDatetime)
{
	int paramIdx = this->GetParamIndex(paramName);
	if(paramIdx < 0)
	{
		m_pConn->SetLastError(1, "parameter %s not exists", paramName);
		return FALSE;
	}
	return this->SetTimestamp(paramIdx, strDatetime);
}

BOOL KSQLCommand::SetNull(int paramIdx)
{
	if(paramIdx < 0 || paramIdx >= m_paramNum)
	{
		m_pConn->SetLastError(1, "parameter index %d out of range", paramIdx);
		return FALSE;
	}

	KSQLFieldDescribe& param = m_params[paramIdx];
	param._isnull = 1;
	param._actualLength = 0;

	return TRUE;
}

BOOL KSQLCommand::SetInteger(int paramIdx, int value)
{
	if(paramIdx < 0 || paramIdx >= m_paramNum)
	{
		m_pConn->SetLastError(1, "parameter index %d out of range", paramIdx);
		return FALSE;
	}
	KSQLFieldDescribe& param = m_params[paramIdx];

	int bufferLen = param.m_length;
	void* pBuffer = param._buffer;
	switch(param.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val = (char)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint8:
		{
			BYTE val = (BYTE)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_int16:
		{
			short val = (short)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val = (unsigned short)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_int32:
		{
			int val = (int)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val = (unsigned int)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_int64:
		{
			long long val = (long long)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint64:
		{
			unsigned long long val = (unsigned long long)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_float32:
		{
			float val = (float)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_float64:
		{
			double val = (double)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
		{
			System::Collections::KString<32> str;
			str.Printf("%d", value);
			memcpy(pBuffer, str.c_str(), str.size()+1);
			return TRUE;
		}
	case KSQLTypes::sql_c_varstring:
		{
			System::Collections::KString<32> str;
			str.Printf("%d", value);
			memcpy(pBuffer, str.c_str(), str.size()+1);
			return TRUE;
		}
	case KSQLTypes::sql_c_binary:
		{
			m_pConn->SetLastError(1, "binary parameter %s can not accept an integer value",
				param.m_name.c_str());
			return FALSE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			m_pConn->SetLastError(1, "timestamp parameter %s can not accept an integer value",
				param.m_name.c_str());
			return FALSE;
		}
	default: return FALSE;
	}
}

BOOL KSQLCommand::SetInt64(int paramIdx, INT64 value)
{
	if(paramIdx < 0 || paramIdx >= m_paramNum)
	{
		m_pConn->SetLastError(1, "parameter index %d out of range", paramIdx);
		return FALSE;
	}

	KSQLFieldDescribe& param = m_params[paramIdx];

	int bufferLen = param.m_length;
	void* pBuffer = param._buffer;
	switch(param.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val = (char)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint8:
		{
			BYTE val = (BYTE)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_int16:
		{
			short val = (short)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val = (unsigned short)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_int32:
		{
			int val = (int)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val = (unsigned int)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_int64:
		{
			long long val = (long long)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint64:
		{
			unsigned long long val = (unsigned long long)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_float32:
		{
			float val = (float)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_float64:
		{
			double val = (double)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
		{
			System::Collections::KString<32> str;
			str.Printf("%I64d", value);
			memcpy(pBuffer, str.c_str(), str.size()+1);
			return TRUE;
		}
	case KSQLTypes::sql_c_varstring:
		{
			System::Collections::KString<32> str;
			str.Printf("%I64d", value);
			memcpy(pBuffer, str.c_str(), str.size()+1);
			return TRUE;
		}
	case KSQLTypes::sql_c_binary:
		{
			m_pConn->SetLastError(1, "binary parameter %s can not accept an int64 value",
				param.m_name.c_str());
			return FALSE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			m_pConn->SetLastError(1, "timestamp parameter %s can not accept an int64 value",
				param.m_name.c_str());
			return FALSE;
		}
	default: return FALSE;
	}
}

BOOL KSQLCommand::SetFloat(int paramIdx, float value)
{
	if(paramIdx < 0 || paramIdx >= m_paramNum)
	{
		m_pConn->SetLastError(1, "parameter index %d out of range", paramIdx);
		return FALSE;
	}
	KSQLFieldDescribe& param = m_params[paramIdx];

	int bufferLen = param.m_length;
	void* pBuffer = param._buffer;
	switch(param.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val = (char)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint8:
		{
			BYTE val = (BYTE)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_int16:
		{
			short val = (short)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val = (unsigned short)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_int32:
		{
			int val = (int)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val = (unsigned int)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_int64:
		{
			long long val = (long long)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint64:
		{
			unsigned long long val = (unsigned long long)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_float32:
		{
			float val = (float)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_float64:
		{
			double val = (double)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
		{
			System::Collections::KString<32> str;
			str.Printf("%.2f", value);
			memcpy(pBuffer, str.c_str(), str.size()+1);
			return TRUE;
		}
	case KSQLTypes::sql_c_varstring:
		{
			System::Collections::KString<32> str;
			str.Printf("%.2f", value);
			memcpy(pBuffer, str.c_str(), str.size()+1);
			return TRUE;
		}
	case KSQLTypes::sql_c_binary:
		{
			m_pConn->SetLastError(1, "binary parameter %s can not accept an float value",
				param.m_name.c_str());
			return FALSE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			m_pConn->SetLastError(1, "binary parameter %s can not accept an float value",
				param.m_name.c_str());
			return FALSE;
		}
	default: return FALSE;
	}
}

BOOL KSQLCommand::SetFloat64(int paramIdx, double value)
{
	if(paramIdx < 0 || paramIdx >= m_paramNum)
	{
		m_pConn->SetLastError(1, "parameter index %d out of range", paramIdx);
		return FALSE;
	}
	KSQLFieldDescribe& param = m_params[paramIdx];

	int bufferLen = param.m_length;
	void* pBuffer = param._buffer;
	switch(param.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val = (char)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint8:
		{
			BYTE val = (BYTE)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_int16:
		{
			short val = (short)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val = (unsigned short)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_int32:
		{
			int val = (int)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val = (unsigned int)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_int64:
		{
			long long val = (long long)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_uint64:
		{
			unsigned long long val = (unsigned long long)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_float32:
		{
			float val = (float)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_float64:
		{
			double val = (double)value;
			memcpy(pBuffer, &val, sizeof(val));
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
		{
			System::Collections::KString<32> str;
			str.Printf("%.2f", value);
			memcpy(pBuffer, str.c_str(), str.size()+1);
			return TRUE;
		}
	case KSQLTypes::sql_c_varstring:
		{
			System::Collections::KString<32> str;
			str.Printf("%.2f", value);
			memcpy(pBuffer, str.c_str(), str.size()+1);
			return TRUE;
		}
	case KSQLTypes::sql_c_binary:
		{
			m_pConn->SetLastError(1, "binary parameter %s can not accept an float64 value",
				param.m_name.c_str());
			return FALSE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			m_pConn->SetLastError(1, "binary parameter %s can not accept an float64 value",
				param.m_name.c_str());
			return FALSE;
		}
	default: return FALSE;
	}
}

BOOL KSQLCommand::SetString(int paramIdx, const char* value, int len)
{
	if(len < 1) len = (int)strlen(value);
	if(paramIdx < 0 || paramIdx >= m_paramNum)
	{
		m_pConn->SetLastError(1, "parameter index %d out of range", paramIdx);
		return FALSE;
	}
	KSQLFieldDescribe& param = m_params[paramIdx];

	int bufferLen = param.m_length;
	void* pBuffer = param._buffer;
	switch(param.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
	case KSQLTypes::sql_c_uint8:
	case KSQLTypes::sql_c_int16:
	case KSQLTypes::sql_c_uint16:
	case KSQLTypes::sql_c_int32:
	case KSQLTypes::sql_c_uint32:
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_uint64:
	case KSQLTypes::sql_c_float32:
	case KSQLTypes::sql_c_float64:
		{
			m_pConn->SetLastError(1, "type %d parameter %s can not accept an string value",
				param.m_cdt, param.m_name.c_str());
			return FALSE;
		}
	case KSQLTypes::sql_c_string:
		{
			if(bufferLen < len+1)
			{
				m_pConn->SetLastError(1, "parameter %s has no sufficient buffer to hold data with len %d",
					param.m_name.c_str(), len);
				return FALSE;
			}
			memcpy(pBuffer, value, len);
			((char*)pBuffer)[len] = '\0';
			param._actualLength = len;
			return TRUE;
		}
	case KSQLTypes::sql_c_varstring:
		{
			if(bufferLen < len+1)
			{
				m_pConn->SetLastError(1, "parameter %s has no sufficient buffer to hold data with len %d",
					param.m_name.c_str(), len);
				return FALSE;
			}
			memcpy(pBuffer, value, len);
			((char*)pBuffer)[len] = '\0';
			param._actualLength = len;
			return TRUE;
		}
	case KSQLTypes::sql_c_binary:
		{
			if(bufferLen < len)
			{
				m_pConn->SetLastError(1, "parameter %s has no sufficient buffer to hold data with len %d",
					param.m_name.c_str(), len);
				return FALSE;
			}
			memcpy(pBuffer, value, len);
			param._actualLength = len;
			return TRUE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			m_pConn->SetLastError(1, "type %d parameter %s can not accept an string value",
				param.m_cdt,
				param.m_name.c_str());
			return FALSE;
		}
	default: return FALSE;
	}
}

BOOL KSQLCommand::SetBinary(int paramIdx, const void* value, int len)
{
	if(paramIdx < 0 || paramIdx >= m_paramNum)
	{
		m_pConn->SetLastError(1, "parameter index %d out of range", paramIdx);
		return FALSE;
	}
	KSQLFieldDescribe& param = m_params[paramIdx];

	int bufferLen = param.m_length;
	void* pBuffer = param._buffer;
	switch(param.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
	case KSQLTypes::sql_c_uint8:
	case KSQLTypes::sql_c_int16:
	case KSQLTypes::sql_c_uint16:
	case KSQLTypes::sql_c_int32:
	case KSQLTypes::sql_c_uint32:
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_uint64:
	case KSQLTypes::sql_c_float32:
	case KSQLTypes::sql_c_float64:
	case KSQLTypes::sql_c_timestamp:
		{
			if(len != param.m_length)
			{
				m_pConn->SetLastError(1, "type %d parameter %s can not be set an binary value with len:%d",
					param.m_cdt, param.m_name.c_str(), len);
				return FALSE;
			}
			memcpy(pBuffer, value, len);
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
		{
			if(len >= param.m_length)
			{
				m_pConn->SetLastError(1, "string parameter %s can not be set an binary value with len:%d",
					param.m_name.c_str(), len);
				return FALSE;
			}
			else
			{
				memcpy(pBuffer, value, len);
				((char*)pBuffer)[len] = '\0';
				param._actualLength = len;
				return TRUE;
			}
		}
	case KSQLTypes::sql_c_binary:
		{
			if(bufferLen < len)
			{
				m_pConn->SetLastError(1, "parameter %s has no sufficient buffer to hold data with len %d  [bufferLen=%d]",
					param.m_name.c_str(),
					len, bufferLen);
				return FALSE;
			}
			memcpy(pBuffer, value, len);
			param._actualLength = len;
			return TRUE;
		}
	default: return FALSE;
	}
}

BOOL KSQLCommand::SetTimestamp(int paramIdx, time_t value)
{
	KDatetime datetime(value);
	KLocalDatetime ldt = datetime.GetLocalTime();
	
	MYSQL_TIME ts = {
		ldt.year, ldt.month, ldt.day,
		ldt.hour, ldt.minute, ldt.second,
		0, 0, MYSQL_TIMESTAMP_DATETIME
	};
	
	if(paramIdx < 0 || paramIdx >= m_paramNum) return FALSE;
	KSQLFieldDescribe& param = m_params[paramIdx];

	int bufferLen = param.m_length;
	void* pBuffer = param._buffer;
	switch(param.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
	case KSQLTypes::sql_c_uint8:
	case KSQLTypes::sql_c_int16:
	case KSQLTypes::sql_c_uint16:
	case KSQLTypes::sql_c_int32:
	case KSQLTypes::sql_c_uint32:
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_uint64:
	case KSQLTypes::sql_c_float32:
	case KSQLTypes::sql_c_float64:
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			m_pConn->SetLastError(1, "type %d parameter %s can not accept an timestamp value",
				param.m_cdt, param.m_name.c_str());
			return FALSE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			if(bufferLen < sizeof(ts))
			{
				m_pConn->SetLastError(1, "parameter %s has no sufficient buffer to hold timestamp value",
					param.m_name.c_str());
				return FALSE;
			}
			memcpy(pBuffer, &ts, sizeof(ts));
			return TRUE;
		}
	default: return FALSE;
	}
}

BOOL KSQLCommand::SetTimestamp(int paramIdx, const char* strDatetime)
{
	KDatetime datetime;
	if(paramIdx < 0 || paramIdx >= m_paramNum) return FALSE;
	if(!datetime.SetDatetimeString(strDatetime)) return FALSE;

	KLocalDatetime ldt = datetime.GetLocalTime();
	MYSQL_TIME ts = {
		ldt.year, ldt.month, ldt.day,
			ldt.hour, ldt.minute, ldt.second,
			0, 0, MYSQL_TIMESTAMP_DATETIME
	};

	KSQLFieldDescribe& param = m_params[paramIdx];

	int bufferLen = param.m_length;
	void* pBuffer = param._buffer;
	switch(param.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
	case KSQLTypes::sql_c_uint8:
	case KSQLTypes::sql_c_int16:
	case KSQLTypes::sql_c_uint16:
	case KSQLTypes::sql_c_int32:
	case KSQLTypes::sql_c_uint32:
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_uint64:
	case KSQLTypes::sql_c_float32:
	case KSQLTypes::sql_c_float64:
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			m_pConn->SetLastError(1, "type %d parameter %s can not accept an timestamp value",
				param.m_cdt, param.m_name.c_str());
			return FALSE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			if(bufferLen < sizeof(ts))
			{
				m_pConn->SetLastError(1, "parameter %s has no sufficient buffer to hold timestamp value",
					param.m_name.c_str());
				return FALSE;
			}
			memcpy(pBuffer, &ts, sizeof(ts));
			return TRUE;
		}
	default: return FALSE;
	}
	return FALSE;
}

BOOL KSQLCommand::ExecuteQuery(KSQLResultSet& recordset)
{
	recordset.Reset();

	if(m_paramNum && !m_binds)
	{
		m_pConn->SetLastError(1, "sql '%s' parameter not binded", m_sqlStatement.c_str());
		return FALSE;
	}
	
	if(mysql_stmt_execute(m_stmt))
	{
		m_pConn->AcquireStmtError(m_stmt);
		return FALSE;
	}
	recordset.m_stmt = m_stmt;
	recordset.m_pconn = m_pConn;
	
	return TRUE;
}

BOOL KSQLCommand::Execute()
{
	if(m_paramNum && !m_binds)
	{
		m_pConn->SetLastError(1, "sql '%s' parameter not binded", m_sqlStatement.c_str());
		return FALSE;
	}

	if(mysql_stmt_execute(m_stmt))
	{
		m_pConn->AcquireStmtError(m_stmt);
		return FALSE;
	}
	//if(mysql_stmt_store_result(m_stmt))
	//{
	//	m_pConn->AcquireStmtError(m_stmt);
	//	return FALSE;
	//}
	return TRUE;
}

BOOL KSQLCommand::Execute(const char* sql)
{
	if(!this->SetSQLStatement(sql)) return FALSE;
	return this->Execute();
}

BOOL KSQLCommand::ExecuteQuery(const char* sql, KSQLResultSet& rs)
{
	if(!this->SetSQLStatement(sql)) return FALSE;
	return this->ExecuteQuery(rs);
}

int KSQLCommand::GetParamIndex(const char* paramName)
{
	for(int i=0; i<m_paramNum; i++)
	{
		KSQLFieldDescribe& describe = m_params[i];
		if(_stricmp(describe.m_name, paramName) == 0)
		{
			return i;
		}
	}
	return -1;
}

void KSQLCommand::Reset()
{
	m_buffer = NULL;
	m_binds = NULL;
	m_paramNum = 0;
	m_params = NULL;
	m_sqlStatement.clear();
	if(m_stmt)
	{
		do 
		{
			mysql_stmt_free_result(m_stmt);
			//mysql_free_result(m_pConn->m_pMysql);
		}
		while(!mysql_next_result(m_pConn->m_pMysql));

		mysql_stmt_close(m_stmt);
		m_stmt = NULL;
	}
}

BOOL KSQLCommand::Prepare()
{
	size_t sqlLen = m_sqlStatement.size();
	const char* sql = m_sqlStatement.c_str();
	
	m_stmt = mysql_stmt_init(m_pConn->m_pMysql);
	if(!m_stmt)
	{
		m_pConn->AcquireErrorInfo();
		return FALSE;
	}
	
	if(mysql_stmt_prepare(m_stmt, sql, (unsigned long)sqlLen))
	{
		m_pConn->AcquireStmtError(m_stmt);
		return FALSE;
	}

	return TRUE;
}

void KSQLCommand::Finalize()
{
	m_buffer = NULL;
	m_bufferCapacity = 0;
	if(m_dynamicBuffer)
	{
		::free_k(m_dynamicBuffer);
		m_dynamicBuffer = NULL;
	}
	
	m_binds = NULL;
	m_bindCapacity = 0;
	if(m_dynamicBinds)
	{
		::free_k(m_dynamicBinds);
		m_dynamicBinds = NULL;
	}

	m_paramNum = 0;
	m_params = NULL;

	if(m_stmt)
	{
		mysql_stmt_close(m_stmt);
		m_stmt = NULL;
	}

	m_sqlStatement.clear();
}

BOOL KSQLCommand::BindParameters()
{
	if(m_paramNum <= c_def_bind_num)
	{
		m_binds = &m_staticBinds[0];
	}
	else
	{
		if(m_paramNum > m_bindCapacity)
		{
			if(!m_bindCapacity)
			{
				m_bindCapacity = m_paramNum;
				m_dynamicBinds = (MYSQL_BIND*)::malloc_k(sizeof(MYSQL_BIND)*m_bindCapacity);
			}
			else
			{
				m_bindCapacity = m_paramNum;
				m_dynamicBinds = (MYSQL_BIND*)::realloc_k(m_dynamicBinds, sizeof(MYSQL_BIND)*m_bindCapacity);
			}
		}
		m_binds = m_dynamicBinds;
	}

	int len = sizeof(MYSQL_BIND) * m_paramNum;
	memset(m_binds, 0, len);

	int wanted = 0;
	for(int i=0; i<m_paramNum; i++)
	{
		KSQLFieldDescribe& param = m_params[i];

		param._buffer = NULL;
		param._actualLength = 0;
		param._isnull = 0;
		param._iserror = 0;

		wanted += param.m_length;
	}

	if(wanted <= c_def_param_buffer_len)
	{
		m_buffer = &m_staticBuffer[0];
	}
	else
	{
		if(wanted > m_bufferCapacity)
		{
			if(!m_bufferCapacity)
			{
				m_bufferCapacity = (wanted + 1023) & (~1023);	
				m_dynamicBuffer = (char*)::malloc_k(m_bufferCapacity);
			}
			else
			{
				m_bufferCapacity = (wanted + 1023) & (~1023);
				m_dynamicBuffer = (char*)::realloc_k(m_dynamicBuffer, m_bufferCapacity);
			}
		}
		m_buffer = m_dynamicBuffer;
	}

	memset(m_buffer, 0, wanted);

	int offset = 0;
	for(int i=0; i<m_paramNum; i++)
	{
		KSQLFieldDescribe& param = m_params[i];
		param._buffer = m_buffer + offset; offset += param.m_length;

		MYSQL_BIND& bind = m_binds[i];
		bind.length = &param._actualLength;  /* output length pointer */
		bind.is_null = &param._isnull;	     /* Pointer to null indicator */
		bind.buffer = param._buffer;	     /* buffer to get/put data */
		bind.error = &param._iserror;        /* set this if you want to track data truncations happened during fetch */
		bind.buffer_type = KSQLTypes::MapToSqlType(param.m_cdt); /* buffer type */
		bind.buffer_length = param.m_length; /* output buffer length, must be set when fetching str/binary */
		bind.is_unsigned = KSQLTypes::IsUnsignedInteger(param.m_cdt); /* set if integer type is unsigned */
	}
	
	if(mysql_stmt_bind_param(m_stmt, m_binds))
	{
		m_pConn->AcquireStmtError(m_stmt);
		return FALSE;
	}
	return TRUE;
}

KSQLResultSet::KSQLResultSet()
{
	m_pconn = NULL;
	m_stmt = NULL;
	m_buffer = NULL;
	m_bufferCapacity = 0;
	m_dynamicBuffer = NULL;
	m_binds = NULL;
	m_bindCapacity = 0;
	m_dynamicBinds = NULL;
	m_colNum = 0;
	m_colCapacity = 0;
	m_describes = NULL;
	m_outDescribes = NULL;
	m_myDescribes = NULL;
}

KSQLResultSet::~KSQLResultSet()
{
	this->Finalize();
}

int KSQLResultSet::GetLastError() const
{
	if(m_pconn) return m_pconn->GetLastError();
	return 0;
}

const char* KSQLResultSet::GetLastErrorMsg() const
{
	if(m_pconn) return m_pconn->GetLastErrorMsg();
	return "";
}

BOOL KSQLResultSet::DescribeColumns(KSQLFieldDescribe* cols, int num)
{
	m_colNum = mysql_stmt_field_count(m_stmt);
	if(m_colNum != num)
	{
		m_pconn->AcquireStmtError(m_stmt);
		return FALSE;
	}

	for(int i=0; i<num; i++)
	{
		KSQLFieldDescribe& describe = cols[i];
		switch(describe.m_cdt)
		{
		case KSQLTypes::sql_c_int8:
		case KSQLTypes::sql_c_uint8:
		case KSQLTypes::sql_c_int16:
		case KSQLTypes::sql_c_uint16:
		case KSQLTypes::sql_c_int32:
		case KSQLTypes::sql_c_uint32:
		case KSQLTypes::sql_c_int64:
		case KSQLTypes::sql_c_uint64:
		case KSQLTypes::sql_c_float32:
		case KSQLTypes::sql_c_float64:
		case KSQLTypes::sql_c_timestamp:
			{
				describe.m_length = KSQLTypes::GetCDataWidth(describe.m_cdt);
				break;
			}
		case KSQLTypes::sql_c_string:
		case KSQLTypes::sql_c_varstring:
		case KSQLTypes::sql_c_binary:
			{
				if(describe.m_length < 1) return FALSE;
				break;
			}
		default:
			{
				m_pconn->SetLastError(1, "describe column %s an unknown type %d",
					describe.m_name.c_str(), describe.m_cdt);
				return FALSE;
			}
		}
	}

	m_outDescribes = cols;
	m_describes = m_outDescribes;

	return TRUE;
}

BOOL KSQLResultSet::SetColumnLength(const char* colName, int len)
{
	int colIdx = this->GetColumnIndex(colName);
	if(colIdx < 0)
	{
		m_pconn->SetLastError(1, "column %s not exists", colName);
		return FALSE;
	}
	return this->SetColumnLength(colIdx, len);
}

BOOL KSQLResultSet::SetColumnLength(int colIdx, int len)
{
	if(!m_describes)
		if(!this->BuildDescribes())
			return FALSE;

	//ASSERT(m_describes);
	if(colIdx < 0 || colIdx >= m_colNum)
	{
		m_pconn->SetLastError(1, "column index %d out of range", colIdx);
		return FALSE;
	}

	KSQLFieldDescribe& col = m_describes[colIdx];
	switch(col.m_cdt)
	{
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
		{
			m_describes[colIdx].m_length = len + 1; // 加上最后的空字符'\0'
			break;
		}
	case KSQLTypes::sql_c_binary:
		{
			m_describes[colIdx].m_length = len;
			break;
		}
	}
	return TRUE;
}

BOOL KSQLResultSet::Begin()
{
	m_pconn->ClearError();

	if(!m_describes)
	{
		if(!this->BuildDescribes())
			return FALSE;
	}

	if(!this->BindResult())
	{
		return FALSE;
	}

	int nRet = mysql_stmt_fetch(m_stmt);
	if(!nRet) return TRUE;
	if(nRet == 1)
	{
		m_pconn->AcquireStmtError(m_stmt);
		return FALSE;
	}
	if(nRet == MYSQL_NO_DATA)
	{
		m_pconn->SetLastError(1, "no data found");
		return FALSE;
	}
	if(nRet == MYSQL_DATA_TRUNCATED)
	{
		const char* colName = NULL;
		for(int i=0; i<m_colNum; i++)
		{
			KSQLFieldDescribe& col = m_describes[i];
			if(col._iserror) colName = col.m_name.c_str();
		}
		m_pconn->SetLastError(1, "column %s data truncated", colName);
		return FALSE;
	}

	return TRUE;
}

BOOL KSQLResultSet::Next()
{
	m_pconn->ClearError();

	int nRet = mysql_stmt_fetch(m_stmt);
	if(nRet == 1)
	{
		m_pconn->AcquireStmtError(m_stmt);
		return FALSE;
	}
	if(nRet == MYSQL_NO_DATA)
	{
		m_pconn->SetLastError(1, "no data found");
		return FALSE;
	}
	if(nRet == MYSQL_DATA_TRUNCATED)
	{
		const char* colName = NULL;
		for(int i=0; i<m_colNum; i++)
		{
			KSQLFieldDescribe& col = m_describes[i];
			if(col._iserror) colName = col.m_name.c_str();
		}
		m_pconn->SetLastError(1, "column %s data truncated", colName);
		return FALSE;
	}

	// 取数据是成功的，把NULL字段的实际长度置为零
	for(int i=0; i<m_colNum; i++)
	{
		KSQLFieldDescribe& col = m_describes[i];
		if(col._isnull) col._actualLength = 0;
	}

	return TRUE;
}

BOOL KSQLResultSet::IsNull(const char* colName, BOOL& isnull)
{
	int colIdx = this->GetColumnIndex(colName);
	if(colIdx < 0)
	{
		m_pconn->SetLastError(1, "column %s not exists", colName);
		return FALSE;
	}
	return this->IsNull(colIdx, isnull);
}

BOOL KSQLResultSet::GetInteger(const char* colName, int& value, int defVal)
{
	int colIdx = this->GetColumnIndex(colName);
	if(colIdx < 0)
	{
		m_pconn->SetLastError(1, "column %s not exists", colName);
		return FALSE;
	}
	return this->GetInteger(colIdx, value, defVal);
}

BOOL KSQLResultSet::GetInt64(const char* colName, INT64& value, INT64 defVal)
{
	int colIdx = this->GetColumnIndex(colName);
	if(colIdx < 0)
	{
		m_pconn->SetLastError(1, "column %s not exists", colName);
		return FALSE;
	}
	return this->GetInt64(colIdx, value, defVal);
}

BOOL KSQLResultSet::GetFloat(const char* colName, float& value, float defVal)
{
	int colIdx = this->GetColumnIndex(colName);
	if(colIdx < 0)
	{
		m_pconn->SetLastError(1, "column %s not exists", colName);
		return FALSE;
	}
	return this->GetFloat(colIdx, value, defVal);
}

// len 为输入输出参数，输入时为缓冲区长度，输出时为实际拷贝的字节数
BOOL KSQLResultSet::GetString(const char* colName, char* value, int& len)
{
	int colIdx = this->GetColumnIndex(colName);
	if(colIdx < 0)
	{
		m_pconn->SetLastError(1, "column %s not exists", colName);
		return FALSE;
	}
	return this->GetString(colIdx, value, len);
}

BOOL KSQLResultSet::GetBinary(const char* colName, void* value, int& len)
{
	int colIdx = this->GetColumnIndex(colName);
	if(colIdx < 0)
	{
		m_pconn->SetLastError(1, "column %s not exists", colName);
		return FALSE;
	}
	return this->GetBinary(colIdx, value, len);
}

// len 为输入输出参数，输入时为要求的字节数，输出时为实际拷贝的字节数
BOOL KSQLResultSet::GetString(const char* colName, int offset, char* value, int len)
{
	int colIdx = this->GetColumnIndex(colName);
	if(colIdx < 0)
	{
		m_pconn->SetLastError(1, "column %s not exists", colName);
		return FALSE;
	}
	return this->GetString(colIdx, offset, value, len);
}

BOOL KSQLResultSet::GetBinary(const char* colName, int offset, void* value, int len)
{
	int colIdx = this->GetColumnIndex(colName);
	if(colIdx < 0)
	{
		m_pconn->SetLastError(1, "column %s not exists", colName);
		return FALSE;
	}
	return this->GetBinary(colIdx, offset, value, len);
}

BOOL KSQLResultSet::IsNull(int colIndex, BOOL& isnull)
{
	if(colIndex < 0 || colIndex >= m_colNum)
	{
		m_pconn->SetLastError(1, "column index %s out of range", colIndex);
		return FALSE;
	}
	return m_describes[colIndex]._isnull;
}

BOOL KSQLResultSet::GetInteger(int colIdx, int& value, int defVal)
{
	if(colIdx < 0 || colIdx >= m_colNum)
	{
		m_pconn->SetLastError(1, "column index %d out of range", colIdx);
		return FALSE;
	}
	
	KSQLFieldDescribe& col = m_describes[colIdx];
	if(col._isnull)
	{
		value = defVal;
		return TRUE;
	}

	switch(col.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val;
			memcpy(&val, col._buffer, col.m_length);
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint8:
		{
			BYTE val;
			memcpy(&val, col._buffer, col.m_length);
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int16:
		{
			short val;
			memcpy(&val, col._buffer, col.m_length);
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val;
			memcpy(&val, col._buffer, col.m_length);
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int32:
		{
			int val;
			memcpy(&val, col._buffer, col.m_length);
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val;
			memcpy(&val, col._buffer, col.m_length);
			value = val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int64:
		{
			long long val;
			memcpy(&val, col._buffer, col.m_length);
			value = (INT)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint64:
		{
			unsigned long long val;
			memcpy(&val, col._buffer, col.m_length);
			value = (INT)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float32:
		{
			float val;
			memcpy(&val, col._buffer, col.m_length);
			value = (int)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float64:
		{
			double val;
			memcpy(&val, col._buffer, col.m_length);
			value = (int)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			MYSQL_TIME ts; memset(&ts, 0, sizeof(ts));
			memcpy(&ts, col._buffer, col.m_length);
			KLocalDatetime ldt = {
				ts.year, ts.month, ts.day,
				ts.hour, ts.minute, ts.second
			};
			KDatetime datetime((INT64)0);
			datetime.SetLocalTime(ldt);
			value = (int)datetime.Time();
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			return FALSE;
		}
	}
	return FALSE;
}

BOOL KSQLResultSet::GetInt64(int colIdx, INT64& value, INT64 defVal)
{
	if(colIdx < 0 || colIdx >= m_colNum)
	{
		m_pconn->SetLastError(1, "column index %d out of range", colIdx);
		return FALSE;
	}
	KSQLFieldDescribe& col = m_describes[colIdx];
	if(col._isnull)
	{
		value = defVal;
		return TRUE;
	}

	switch(col.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val;
			memcpy(&val, col._buffer, col.m_length);
			value = (INT64)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint8:
		{
			BYTE val;
			memcpy(&val, col._buffer, col.m_length);
			value = (INT64)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int16:
		{
			short val;
			memcpy(&val, col._buffer, col.m_length);
			value = (INT64)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val;
			memcpy(&val, col._buffer, col.m_length);
			value = (INT64)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int32:
		{
			int val;
			memcpy(&val, col._buffer, col.m_length);
			value = (INT64)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val;
			memcpy(&val, col._buffer, col.m_length);
			value = (INT64)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int64:
		{
			long long val;
			memcpy(&val, col._buffer, col.m_length);
			value = (INT64)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint64:
		{
			unsigned long long val;
			memcpy(&val, col._buffer, col.m_length);
			value = (INT64)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float32:
		{
			float val;
			memcpy(&val, col._buffer, col.m_length);
			value = (INT64)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float64:
		{
			double val;
			memcpy(&val, col._buffer, col.m_length);
			value = (INT64)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			MYSQL_TIME ts; memset(&ts, 0, sizeof(ts));
			memcpy(&ts, col._buffer, col.m_length);
			KLocalDatetime ldt = {
				ts.year, ts.month, ts.day,
				ts.hour, ts.minute, ts.second
			};
			KDatetime datetime((INT64)0);
			datetime.SetLocalTime(ldt);
			value = (INT64)datetime.Time();
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			return FALSE;
		}
	}
	return FALSE;
}

BOOL KSQLResultSet::GetFloat(int colIdx, float& value, float defVal)
{
	if(colIdx < 0 || colIdx >= m_colNum)
	{
		m_pconn->SetLastError(1, "column index %d out of range", colIdx);
		return FALSE;
	}
	KSQLFieldDescribe& col = m_describes[colIdx];
	if(col._isnull)
	{
		value = defVal;
		return TRUE;
	}

	switch(col.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
		{
			char val;
			memcpy(&val, col._buffer, col.m_length);
			value = (float)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint8:
		{
			BYTE val;
			memcpy(&val, col._buffer, col.m_length);
			value = (float)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int16:
		{
			short val;
			memcpy(&val, col._buffer, col.m_length);
			value = (float)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint16:
		{
			unsigned short val;
			memcpy(&val, col._buffer, col.m_length);
			value = (float)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int32:
		{
			int val;
			memcpy(&val, col._buffer, col.m_length);
			value = (float)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint32:
		{
			unsigned int val;
			memcpy(&val, col._buffer, col.m_length);
			value = (float)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_int64:
		{
			long long val;
			memcpy(&val, col._buffer, col.m_length);
			value = (float)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_uint64:
		{
			unsigned long long val;
			memcpy(&val, col._buffer, col.m_length);
			value = (float)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float32:
		{
			float val;
			memcpy(&val, col._buffer, col.m_length);
			value = (float)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_float64:
		{
			double val;
			memcpy(&val, col._buffer, col.m_length);
			value = (float)val;
			return TRUE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			MYSQL_TIME ts; memset(&ts, 0, sizeof(ts));
			memcpy(&ts, col._buffer, col.m_length);
			KLocalDatetime ldt = {
				ts.year, ts.month, ts.day,
				ts.hour, ts.minute, ts.second
			};
			KDatetime datetime((INT64)0);
			datetime.SetLocalTime(ldt);
			value = (float)datetime.Time();
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			return FALSE;
		}
	}
	return FALSE;
}

BOOL KSQLResultSet::GetString(int colIdx, char* value, int& len)
{
	if(colIdx < 0 || colIdx >= m_colNum) return FALSE;
	KSQLFieldDescribe& col = m_describes[colIdx];
	switch(col.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
	case KSQLTypes::sql_c_uint8:
	case KSQLTypes::sql_c_int16:
	case KSQLTypes::sql_c_uint16:
	case KSQLTypes::sql_c_int32:
	case KSQLTypes::sql_c_uint32:
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_uint64:
	case KSQLTypes::sql_c_float32:
	case KSQLTypes::sql_c_float64:
		{
			m_pconn->SetLastError(1, "can not translate column %s data to string", col.m_name.c_str());
			return FALSE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			if(len < (INT)col._actualLength+1)
			{
				m_pconn->SetLastError(1, "column %s has no sufficient buffer to hold data with length %d",
					col.m_name.c_str(), col._actualLength);
				return FALSE;
			}
			memcpy(value, col._buffer, col._actualLength);
			value[col._actualLength] = '\0';
			len = col._actualLength;
			return TRUE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			MYSQL_TIME ts; memset(&ts, 0, sizeof(ts));
			memcpy(&ts, col._buffer, col.m_length);
			KLocalDatetime ldt = { ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second };
			KDatetime datetime((INT64)0); datetime.SetLocalTime(ldt);
			System::Collections::KString<32> strDatetime = datetime.ToString();
			if(len < (int)strDatetime.size()+1)
			{
				m_pconn->SetLastError(1, "buffer not sufficient for column %s timestamp data", col.m_name.c_str());
				return FALSE;
			}
			len = strDatetime.size(); memcpy(value, strDatetime.c_str(), len); value[len] = '\0';
			return TRUE;
		}
	}
	return FALSE;
}

BOOL KSQLResultSet::GetBinary(int colIdx, void* value, int& len)
{
	if(colIdx < 0 || colIdx >= m_colNum) return FALSE;
	KSQLFieldDescribe& col = m_describes[colIdx];
	switch(col.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
	case KSQLTypes::sql_c_uint8:
	case KSQLTypes::sql_c_int16:
	case KSQLTypes::sql_c_uint16:
	case KSQLTypes::sql_c_int32:
	case KSQLTypes::sql_c_uint32:
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_uint64:
	case KSQLTypes::sql_c_float32:
	case KSQLTypes::sql_c_float64:
	case KSQLTypes::sql_c_timestamp:
		{
			if(len != col.m_length)
			{
				m_pconn->SetLastError(1, "column %s can not get binary data with len:%d", col.m_name.c_str(), len);
				return FALSE;
			}
			memcpy(value, col._buffer, col.m_length);
			return TRUE;
		}
		
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			if(len < (INT)col._actualLength)
			{
				m_pconn->SetLastError(1, "column %s has no sufficient buffer to hold data with len %d",
					col.m_name.c_str(), col._actualLength);
				return FALSE;
			}
			memcpy(value, col._buffer, col._actualLength);
			len = col._actualLength;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL KSQLResultSet::GetColumnValue(int colIdx, int* typep, void** valuepp, int* lenp)
{
	if(colIdx < 0 || colIdx >= m_colNum)
		return FALSE;

	KSQLFieldDescribe& col = m_describes[colIdx];
	if(col.isNull())
	{
		*valuepp = NULL;
		*lenp = 0;
		return TRUE;
	}

	switch(col.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
	case KSQLTypes::sql_c_uint8:
	case KSQLTypes::sql_c_int16:
	case KSQLTypes::sql_c_uint16:
	case KSQLTypes::sql_c_int32:
	case KSQLTypes::sql_c_uint32:
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_uint64:
	case KSQLTypes::sql_c_float32:
	case KSQLTypes::sql_c_float64:
	case KSQLTypes::sql_c_timestamp:
		{
			*typep = col.m_cdt;
			*valuepp = col._buffer;
			*lenp = col.m_length;
			return TRUE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			*typep = col.m_cdt;
			*valuepp = col._buffer;
			*lenp = col._actualLength;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL KSQLResultSet::GetString(int colIdx, int offset, char* value, int len)
{
	if(colIdx < 0 || colIdx >= m_colNum)
	{
		m_pconn->SetLastError(1, "column index %d out of range", colIdx);
		return FALSE;
	}
	KSQLFieldDescribe& col = m_describes[colIdx];
	switch(col.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
	case KSQLTypes::sql_c_uint8:
	case KSQLTypes::sql_c_int16:
	case KSQLTypes::sql_c_uint16:
	case KSQLTypes::sql_c_int32:
	case KSQLTypes::sql_c_uint32:
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_uint64:
	case KSQLTypes::sql_c_float32:
	case KSQLTypes::sql_c_float64:
		{
			m_pconn->SetLastError(1, "cam not translate column %s data to string", col.m_name.c_str());
			return FALSE;
		}
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
	case KSQLTypes::sql_c_binary:
		{
			if(offset >= (INT)col._actualLength)
			{
				m_pconn->SetLastError(1, "offset %d exceed column %s data range", offset, col.m_name.c_str());
				return FALSE;
			}
			if(offset+len > (INT)col._actualLength)
			{
				m_pconn->SetLastError(1, "offset+len (%d+%d) exceed column %s data range",
					offset, len, col.m_name.c_str());
				return FALSE;
			}
			memcpy(value, (char*)col._buffer+offset, len);
			value[len] = '\0';
			return TRUE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			m_pconn->SetLastError(1, "cam not translate column %s data to string", col.m_name.c_str());
			return FALSE;
		}
	}
	return FALSE;
}

BOOL KSQLResultSet::GetBinary(int colIdx, int offset, void* value, int len)
{
	if(colIdx < 0 || colIdx >= m_colNum)
	{
		m_pconn->SetLastError(1, "column index %d out of range", colIdx);
		return FALSE;
	}
	KSQLFieldDescribe& col = m_describes[colIdx];
	switch(col.m_cdt)
	{
	case KSQLTypes::sql_c_int8:
	case KSQLTypes::sql_c_uint8:
	case KSQLTypes::sql_c_int16:
	case KSQLTypes::sql_c_uint16:
	case KSQLTypes::sql_c_int32:
	case KSQLTypes::sql_c_uint32:
	case KSQLTypes::sql_c_int64:
	case KSQLTypes::sql_c_uint64:
	case KSQLTypes::sql_c_float32:
	case KSQLTypes::sql_c_float64:
	case KSQLTypes::sql_c_string:
	case KSQLTypes::sql_c_varstring:
		{
			m_pconn->SetLastError(1, "cam not translate column %s data to binary", col.m_name.c_str());
			return FALSE;
		}
	case KSQLTypes::sql_c_binary:
		{
			if(offset >= (INT)col._actualLength)
			{
				m_pconn->SetLastError(1, "offset %d exceed column %s data range", offset, col.m_name.c_str());
				return FALSE;
			}
			if(offset+len > (INT)col._actualLength)
			{
				m_pconn->SetLastError(1, "offset+len (%d+%d) exceed column %s data range",
					offset, len, col.m_name.c_str());
				return FALSE;
			}
			memcpy(value, (char*)col._buffer+offset, len);
			return TRUE;
		}
	case KSQLTypes::sql_c_timestamp:
		{
			m_pconn->SetLastError(1, "cam not translate column %s data to binary", col.m_name.c_str());
			return FALSE;
		}
	}
	return FALSE;
}

BOOL KSQLResultSet::GetTimestamp(const char* colName, MYSQL_TIME_K& value, MYSQL_TIME_K defVal)
{
	return this->GetInt64(colName, value, defVal);
}

BOOL KSQLResultSet::GetTimestamp(int colIdx, MYSQL_TIME_K& value, MYSQL_TIME_K defVal)
{
	return this->GetInt64(colIdx, value, defVal);
}

void* KSQLResultSet::_GetRecvBuffer()
{
	return m_buffer;
}

BOOL KSQLResultSet::GetColumnValue(const char* colName, int* typep, void** valuepp, int* lenp)
{
	int colIndex = this->GetColumnIndex(colName);
	return this->GetColumnValue(colIndex, typep, valuepp, lenp);
}

int KSQLResultSet::GetColumnIndex(const char* colName)
{
	for(int i=0; i<m_colNum; i++)
	{
		KSQLFieldDescribe& col = m_describes[i];
		if(_stricmp(col.m_name, colName) == 0) return i;
	}
	return -1;
}

void KSQLResultSet::Reset()
{
	m_stmt = NULL;
	m_pconn = NULL;
	m_buffer = NULL;
	m_binds = NULL;
	m_colNum = 0;                   // 结果集中的记录有多少列
	m_describes = NULL;
	m_outDescribes = NULL;
}

void KSQLResultSet::Finalize()
{
	m_stmt = NULL;
	m_pconn = NULL;
	m_buffer = NULL;
	m_bufferCapacity = 0;
	if(m_dynamicBuffer)
	{
		::free_k(m_dynamicBuffer);
		m_dynamicBuffer = NULL;
	}
	
	m_binds = NULL;
	m_bindCapacity = 0;
	if(m_dynamicBinds)
	{
		::free_k(m_dynamicBinds);
		m_dynamicBinds = NULL;
	}

	m_colNum = 0;
	m_describes = NULL;
	m_outDescribes = NULL;
	if(m_colCapacity)
	{
		::free_k(m_myDescribes);
		m_myDescribes = NULL;
		m_colCapacity = 0;
	}
}

/**
 * 使用mysql_stmt_result_metadata得到结果集的描述信息
 * 生成并且填写输出的列Describe数组
 */
BOOL KSQLResultSet::BuildDescribes()
{
	MYSQL_RES* mysqlRs = mysql_stmt_result_metadata(m_stmt);
	if(!mysqlRs)
	{
		m_pconn->AcquireStmtError(m_stmt);
		return FALSE;
	}

	m_colNum = mysql_num_fields(mysqlRs);
	if(m_colNum < 1)
	{
		m_pconn->AcquireErrorInfo();
		mysql_free_result(mysqlRs);
		return FALSE;
	}

	if(m_colNum > m_colCapacity)
	{
		if(!m_colCapacity)
		{
			m_colCapacity = m_colNum;
			m_myDescribes = (KSQLFieldDescribe*)::malloc_k(sizeof(KSQLFieldDescribe)*m_colCapacity);
		}
		else
		{
			m_colCapacity= m_colNum;
			m_myDescribes = (KSQLFieldDescribe*)::realloc_k(m_describes, sizeof(KSQLFieldDescribe)*m_colCapacity);
		}
	}

	m_describes = m_myDescribes;
	memset(m_describes, 0, sizeof(KSQLFieldDescribe)*m_colNum);

	for(int i=0; i<m_colNum; i++)
	{
		KSQLFieldDescribe& col = m_describes[i];
		MYSQL_FIELD* fld = mysql_fetch_field_direct(mysqlRs, i);
		col.m_name = fld->name;
		col.m_cdt = KSQLTypes::MapToCType(fld->type, fld->flags);
		switch(col.m_cdt)
		{
		case KSQLTypes::sql_c_int8:      col.m_length = 1; break;
		case KSQLTypes::sql_c_uint8:     col.m_length = 1; break;
		case KSQLTypes::sql_c_int16:     col.m_length = 2; break;
		case KSQLTypes::sql_c_uint16:    col.m_length = 2; break;
		case KSQLTypes::sql_c_int32:     col.m_length = 4; break;
		case KSQLTypes::sql_c_uint32:    col.m_length = 4; break;
		case KSQLTypes::sql_c_int64:     col.m_length = 8; break;
		case KSQLTypes::sql_c_uint64:    col.m_length = 8; break;
		case KSQLTypes::sql_c_float32:   col.m_length = 4; break;
		case KSQLTypes::sql_c_float64:   col.m_length = 8; break;
		case KSQLTypes::sql_c_string:    col.m_length = fld->length+1; break;
		case KSQLTypes::sql_c_varstring: col.m_length = fld->length+1; break;
		case KSQLTypes::sql_c_binary:
			{
				col.m_length = fld->length;
				if(col.m_length > 4096)
				{// 如果数据库中定义的太长，先设为缺省长度，使用者可以通过SetColumnLength()去改变
					col.m_length = def_binary_len;
				}
				break;
			}
		case KSQLTypes::sql_c_timestamp: col.m_length = sizeof(MYSQL_TIME); break;
		}
	}

	mysql_free_result(mysqlRs);
	return TRUE;
}

/**
 * 生成并且填写实际绑定使用的MYSQL_BIND数组
 * 根据结果集中所有列的宽度信息，生成接收数据用的缓冲区
 * 填写Describe和MYSQL_BIND数组中的缓冲区指针，该指针指向数据接收缓冲区
 */
BOOL KSQLResultSet::BindResult()
{
	if(m_colNum <= c_def_bind_num)
	{
		m_binds = &m_staticBinds[0];
	}
	else
	{
		if(m_colNum > m_bindCapacity)
		{
			if(!m_bindCapacity)
			{
				m_bindCapacity = m_colNum;
				m_dynamicBinds = (MYSQL_BIND*)::malloc_k(sizeof(MYSQL_BIND)*m_bindCapacity);
			}
			else
			{
				m_bindCapacity = m_colNum;
				m_dynamicBinds = (MYSQL_BIND*)::realloc_k(m_dynamicBinds, sizeof(MYSQL_BIND)*m_bindCapacity);
			}
		}
		m_binds = m_dynamicBinds;
	}

	int len = sizeof(MYSQL_BIND) * m_colNum;
	memset(m_binds, 0, len);

	int wanted = 0;
	for(int i=0; i<m_colNum; i++)
	{
		KSQLFieldDescribe& param = m_describes[i];

		param._buffer = NULL;
		param._actualLength = 0;
		param._isnull = 0;
		param._iserror = 0;

		wanted += param.m_length;
	}

	if(wanted <= c_def_param_buffer_len)
	{
		m_buffer = &m_staticBuffer[0];
	}
	else
	{
		if(wanted > m_bufferCapacity)
		{
			if(!m_bufferCapacity)
			{
				m_bufferCapacity = (wanted + 1023) & (~1023);	
				m_dynamicBuffer = (char*)::malloc_k(m_bufferCapacity);
			}
			else
			{
				m_bufferCapacity = (wanted + 1023) & (~1023);
				m_dynamicBuffer = (char*)::realloc_k(m_dynamicBuffer, m_bufferCapacity);
			}
		}
		m_buffer = m_dynamicBuffer;
	}

	memset(m_buffer, 0, wanted);

	int offset = 0;
	for(int i=0; i<m_colNum; i++)
	{
		KSQLFieldDescribe& param = m_describes[i];
		param._buffer = m_buffer + offset; offset += param.m_length;

		MYSQL_BIND& bind = m_binds[i];
		bind.length = &param._actualLength;  /* output length pointer */
		bind.is_null = &param._isnull;	     /* Pointer to null indicator */
		bind.buffer = param._buffer;	     /* buffer to get/put data */
		bind.error = &param._iserror;        /* set this if you want to track data truncations happened during fetch */
		bind.buffer_type = KSQLTypes::MapToSqlType(param.m_cdt); /* buffer type */
		bind.buffer_length = param.m_length; /* output buffer length, must be set when fetching str/binary */
		bind.is_unsigned = KSQLTypes::IsUnsignedInteger(param.m_cdt); /* set if integer type is unsigned */
	}

	if(mysql_stmt_bind_result(m_stmt, m_binds))
	{
		m_pconn->AcquireStmtError(m_stmt);
		return FALSE;
	}
	return TRUE;
}

const char* KSQLResultSet::GetColumnName( int colIndex )
{
	if (colIndex < 0 || colIndex >= m_colNum) return NULL;
	KSQLFieldDescribe& col = m_describes[colIndex];
	return col.m_name.c_str();
}