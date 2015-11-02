///////////////////////////////////////////////////////////////////////////////
//
// FileName  :  KMySqlDB.h
// Creator   :  Jungle
// Date      :  2007-11-26
// Comment   :  access mysql database by mysql api for c.
//
///////////////////////////////////////////////////////////////////////////////

#if !defined(_MY_SQL_DATABASE_H__)
#define _MY_SQL_DATABASE_H__

#ifdef	_WIN32
#include "winsock2.h"		//2009/12/25韩玉信修改：#include "winsock.h" => #include "winsock2.h"
#endif

#include <mysqldb/mysql.h>

#pragma warning( disable : 4996 )

enum { DB_OK = 0, };

class KMySqlDB  
{
public:
	KMySqlDB();
	virtual ~KMySqlDB();

	// connect to mysql server 
	int Connect(const char* szHost, const char* szUser, const char* szPass, unsigned int nPort = MYSQL_PORT);
	// active a database for reading or writing
	int SelectDatabase(const char* szDatabase);
	// close connection
	void Close();
	// execute a query
	int Execute(const char* szSql);
	// move pointer to next row
	int MoveNext();
	int MoveNextResult();
	int ThrowResult();
	// clean up a query result
	void FreeResult();
	// return row count of current query
	int GetRowCount() { return m_nRows; }
	// return field count of current query
	int GetFieldCount() { return m_nFields; }
	// return special field value of current query
	const char* GetValue(int nField);

	int SetCharSet(const char* charset);

	static const char* GetErrorMsg() { return m_szError; }
public:	//2011-07-25韩玉信添加
	bool	IsActive()	const;
private:
	MYSQL*			m_pMySql;
	MYSQL_RES*		m_pRes;
	MYSQL_ROW		m_pRow;
	unsigned int	m_nRows;
	unsigned int	m_nFields;
	char		m_charset[64];
	static char		m_szError[1024];
};

#endif // !defined(_MY_SQL_DATABASE_H__)