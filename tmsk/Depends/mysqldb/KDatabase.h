#ifndef _K_DATABASE_H_
#define _K_DATABASE_H_

#include <System/KType.h>
#include <System/Collections/KString.h>
#include <System/Collections/KVector.h>
#include <System/Misc/KDatetime.h>

#if defined(WIN32) || defined(WINDOWS)
#include <WinSock2.h>
#else

#endif

#include <mysqldb/mysql.h>
#include <System/KPlatform.h>

typedef INT64 MYSQL_TIME_K;

/**
 * 数据库操作类
 */

class KSQLCommand;
class KSQLResultSet;
class KSQLTableSchema;

class KSQLTypes
{
public:
	enum eCDataType
	{
		sql_c_invalid = -1,
		sql_c_int8      = 0,
		sql_c_uint8     = 1,
		sql_c_int16     = 2,
		sql_c_uint16    = 3,
		sql_c_int32     = 4,
		sql_c_uint32    = 5,
		sql_c_int64     = 6,
		sql_c_uint64    = 7,
		sql_c_float32   = 8,
		sql_c_float64   = 9,
		sql_c_string    = 10,
		sql_c_varstring = 11,
		sql_c_binary    = 12,
		sql_c_timestamp = 13,
		sql_c_max
	};

public:
	static eCDataType MapToCType(enum_field_types sqlDatatype, DWORD fldFlags);
	static enum_field_types MapToSqlType(eCDataType ctype);
	static BOOL IsUnsignedInteger(eCDataType ctype);
	static int GetCDataWidth(eCDataType ctype);
	static eCDataType MapToCType(const char* typeName);
};

// 在快速查询时由调用者提供每个字段的信息
struct KSQLFieldDescribe
{
	int m_length;
	System::Collections::KString<32> m_name;
	KSQLTypes::eCDataType m_cdt;

	// 以下字段在内部使用，不要直接对这些字段赋值
	void*                 _buffer;
	unsigned long         _actualLength;
	my_bool               _isnull;
	my_bool               _iserror;

	// 获得输出参数
	BOOL isNull() const;
	size_t getOutput(void* value, size_t len);
};

class KSQLConnParam
{
public:
	enum { MYSQL_DEFAULT_PORT=3306 };

public:
	KSQLConnParam();
	KSQLConnParam(const KSQLConnParam& o);
	~KSQLConnParam();
	KSQLConnParam& operator=(const KSQLConnParam& o);

public:
	void SetHost(const char* host);
	void SetServerPort(unsigned int serverPort);
	void SetDbName(const char* dbName);
	void SetUserName(const char* userName);
	void SetPassword(const char* passwd);
	void SetCharset(const char* charset);

public:
	unsigned int GetServerPort() const;
	const char* GetHost() const;
	const char* GetDbName() const;
	const char* GetUserName() const;
	const char* GetPasswd() const;
	const char* GetCharset() const;

public:
	unsigned int m_serverPort;
	System::Collections::KString<32> m_host;
	System::Collections::KString<32> m_dbName;
	System::Collections::KString<32> m_userName;
	System::Collections::KString<32> m_passwd;
	System::Collections::KString<32> m_charset;
};

class KSQLConnection
{
private:
	friend class KSQLCommand;
	friend class KSQLResultSet;
	friend class KTableCache;

public:
	KSQLConnection();
	~KSQLConnection();

public:
	BOOL SetDefaultCharset(const char* charset);
	BOOL SetAutoReconnect(BOOL autoReconnect);
	BOOL SetConnectTimeout(int sec);

public:
	void SetLifetime(int lifetime);
	// 使这个连接保持新鲜的状态，在连续使用了一段时间后，重连一次
	BOOL KeepConnectionFresh();

	BOOL IsActive() const;
	BOOL Connect(const KSQLConnParam& connParam);
	BOOL Reconnect();
	BOOL SelectDatabase(const char* dbName);
	void Close();

public:
	int GetLastError() const;
	const char* GetLastErrorMsg() const;
	void ClearError(); // 清空错误信息

public:
	// 表名里面可以包含数据库名称，如: DBGame.tb_Skill
	BOOL QueryTableSchema(const char* tableName, KSQLTableSchema& tableSchema);
	UINT64 GetLastAutoIncID();

private:
	void AcquireErrorInfo();
	void AcquireStmtError(MYSQL_STMT* stmt);
	void SetLastError(DWORD errCode, const char* fmt, ...);

public:
	int m_connTimeout;
	BOOL m_autoReconnect;
	System::Collections::KString<32> m_defCharset;

	MYSQL* m_pMysql;
	KSQLConnParam m_connParam;

	DWORD m_errCode;
	System::Collections::KDString<512> m_errmsg;

	TIME_T m_cnnTime;   // 建立连接的时间
	int m_cnnLifetime;  // 一个连接可持续使用的时间(秒)
};

class KSQLCommand
{
public:
	friend class KTableCache;
	enum
	{
		c_def_param_buffer_len = 1024,
		c_def_bind_num = 10,
	};

public:
	KSQLCommand();
	KSQLCommand(KSQLConnection& conn);
	~KSQLCommand();

public:
	void SetConnection(KSQLConnection& cnn);
	int GetLastError() const;
	const char* GetLastErrorMsg() const;

public:
	BOOL SetSQLStatement(const char* sql);

	// 如果SQL语句中是带参数的(里面有 ?)，下面的操作在Execute之前是必须的
	BOOL DescribeParameters(KSQLFieldDescribe* describes, int num);

	// 为参数设置值
	BOOL SetNull(const char* paramName);
	BOOL SetInteger(const char* paramName, int value);
	BOOL SetInt64(const char* paramName, INT64 value);
	BOOL SetFloat(const char* paramName, float value);
	BOOL SetFloat64(const char* paramName, double value);
	BOOL SetString(const char* paramName, const char* value, int len=0);
	BOOL SetBinary(const char* paramName, const void* value, int len);

	BOOL SetTimestamp(const char* paramName, time_t value);
	BOOL SetTimestamp(const char* paramName, const char* strDatetime);

	BOOL SetNull(int paramIdx);
	BOOL SetInteger(int paramIdx, int value);
	BOOL SetInt64(int paramIdx, INT64 value);
	BOOL SetFloat(int paramIdx, float value);
	BOOL SetFloat64(int paramIdx, double value);
	BOOL SetString(int paramIdx, const char* value, int len=0);
	BOOL SetBinary(int paramIdx, const void* value, int len);

	BOOL SetTimestamp(int paramIdx, time_t value);
	BOOL SetTimestamp(int paramIdx, const char* strDatetime);

	// 一个SQL语句可以被多次执行，使用不同的参数值
	BOOL Execute();
	BOOL ExecuteQuery(KSQLResultSet& recordset);

	// 直接指定SQL语句执行，SQL语句不能带参数
	BOOL Execute(const char* sql);
	BOOL ExecuteQuery(const char* sql, KSQLResultSet& rs);

public:
	int GetParamIndex(const char* paramName);
	void Reset();          // 清空以准备执行下一个SQL
	BOOL Prepare();        // 为新的SQL语句做准备
	void Finalize();       // 释放所有的资源
	BOOL BindParameters(); // 绑定参数

public:
	char* m_buffer;
	int m_bufferCapacity;
	char* m_dynamicBuffer;
	char  m_staticBuffer[c_def_param_buffer_len];

	MYSQL_BIND* m_binds;
	int m_bindCapacity;
	MYSQL_BIND* m_dynamicBinds;
	MYSQL_BIND m_staticBinds[c_def_bind_num];

	int m_paramNum;
	KSQLConnection* m_pConn;
	KSQLFieldDescribe* m_params;

	MYSQL_STMT* m_stmt;
	System::Collections::KDString<256> m_sqlStatement;
};

class KSQLResultSet
{
public:
	friend class KSQLCommand;
	friend class KTableCache;
	enum { def_string_len=256, def_binary_len=1024 };
	enum { c_def_param_buffer_len = 1024, c_def_bind_num = 10 };

public:
	KSQLResultSet();
	~KSQLResultSet();

public:
	int GetLastError() const;
	const char* GetLastErrorMsg() const;

public:
	BOOL DescribeColumns(KSQLFieldDescribe* cols, int num);
	BOOL BuildDescribes(); // 使用mysql_stmt_result_metadata生成字段描述

	// 一般只需要对BLOB字段进行设置，因为BLOB在数据库中定义的长度会很大
	// 而实际上存储的数据却不会太长
	// 只有设置的长度比表中定义的字段长度短，才会真正设置
	BOOL SetColumnLength(const char* colName, int len);
	BOOL SetColumnLength(int colIdx, int len);

	BOOL Begin();
	BOOL Next();
	
public:
	BOOL IsNull(const char* colName, BOOL& isnull);
	BOOL GetInteger(const char* colName, int& value, int defVal=0);
	BOOL GetInt64(const char* colName, INT64& value, INT64 defVal=0);
	BOOL GetFloat(const char* colName, float& value, float defVal=0.0f);
	
	// len 为输入输出参数，输入时为缓冲区长度，输出时为实际拷贝的字节数
	BOOL GetString(const char* colName, char* value, int& len);
	BOOL GetBinary(const char* colName, void* value, int& len);

	// len 为输入参数，为要求的字节数
	BOOL GetString(const char* colName, int offset, char* value, int len);
	BOOL GetBinary(const char* colName, int offset, void* value, int len);

	BOOL IsNull(int colIndex, BOOL& isnull);
	BOOL GetInteger(int colIndex, int& value, int defVal=0);
	BOOL GetInt64(int colIdx, INT64& value, INT64 defVal=0);
	BOOL GetFloat(int colIndex, float& value, float defVal=0.0f);
	BOOL GetString(int colIndex, char* value, int& len);
	BOOL GetBinary(int colIndex, void* value, int& len);
	BOOL GetString(int colIndex, int offset, char* value, int len);
	BOOL GetBinary(int colIndex, int offset, void* value, int len);

	BOOL GetTimestamp(const char* colName, MYSQL_TIME_K& value, MYSQL_TIME_K defVal=0);
	BOOL GetTimestamp(int colIdx, MYSQL_TIME_K& value, MYSQL_TIME_K defVal=0);

	void* _GetRecvBuffer();

	const char* GetColumnName(int colIndex);

	// get inner data pointer and data length
	BOOL GetColumnValue(int colIndex, int* typep, void** valuepp, int* lenp);
	BOOL GetColumnValue(const char* colName, int* typep, void** valuepp, int* lenp);

	int GetColumnIndex(const char* colName);

public:
	void Reset();              // 重置以容纳下一个结果集
	void Finalize();
	BOOL BindResult();         // 把内部缓冲区绑定到结果集的输出

public:
	MYSQL_STMT* m_stmt;                           // 从外面传入，不负责关闭
	KSQLConnection* m_pconn;

	char* m_buffer;
	int m_bufferCapacity;
	char* m_dynamicBuffer;
	char  m_staticBuffer[c_def_param_buffer_len];

	MYSQL_BIND* m_binds;
	int m_bindCapacity;
	MYSQL_BIND* m_dynamicBinds;
	MYSQL_BIND m_staticBinds[c_def_bind_num];

	int m_colNum;                   // 结果集中的记录有多少列
	int m_colCapacity;              // 数组的容量
	KSQLFieldDescribe* m_describes; // 结果集中的字段描述数组
	KSQLFieldDescribe* m_outDescribes;
	KSQLFieldDescribe* m_myDescribes;
};

class KSQLTableField
{
public:
	KSQLTableField();
	KSQLTableField(const KSQLTableField& o);
	~KSQLTableField();
	KSQLTableField& operator=(const KSQLTableField& o);

public:
	int  m_length;
	BOOL m_nullable;
	BOOL m_autoIncrement;
	BOOL m_primaryKey;
	KSQLTypes::eCDataType m_cdt;
	System::Collections::KString<32> m_name;
	System::Collections::KString<32> m_defaultVal;
};

class KSQLTableSchema
{
public:
	KSQLTableSchema();
	KSQLTableSchema(const KSQLTableSchema& o);
	~KSQLTableSchema();
	KSQLTableSchema& operator=(const KSQLTableSchema& o);

public:
	KSQLTableSchema& Append(const KSQLTableField& field);
	int GetFieldCount() const;

	int_r GetFieldIndex(const char* fieldName) const;
	const KSQLTableField* GetField(const char* fieldName) const;
	const KSQLTableField* GetField(int fieldIndex) const;

	// 假定主键必须在所有字段的前面连续存在
	// 主键由多少字段组成
	int GetPrimaryKeyFieldCount() const;

	void Clear();

public:
	System::Collections::KString<32> m_name;
	System::Collections::KString<32> m_dbName;
	System::Collections::KVector<KSQLTableField> m_fields;
};

#endif