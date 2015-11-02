// MySqlDB.cpp: implementation of the KMySqlDB class.
//
//////////////////////////////////////////////////////////////////////

//#ifdef	_WIN32
//#include "stdafx.h"
//#endif
#include <stdio.h>
#include <string.h>
#include "kmysqldb.h"
#include <System/KPlatform.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
char	KMySqlDB::m_szError[1024] = {0};

KMySqlDB::KMySqlDB()
{
	m_pMySql  = NULL;
	m_pRes    = NULL;
	m_nRows   = 0;
	m_nFields = 0;
	strcpy(m_charset, g_encoding == encode_ansii? "GBK" : "utf8");
}

KMySqlDB::~KMySqlDB()
{
	Close();
}


int KMySqlDB::Connect(const char* szHost, const char* szUser, const char* szPass, unsigned int nPort /* = MYSQL_PORT */)
{
	m_pMySql = mysql_init(NULL);
	if (NULL == m_pMySql)
	{
		strcpy(m_szError, "init mysql failed.");
		return -1;

	}

	mysql_options(m_pMySql, MYSQL_SET_CHARSET_NAME, m_charset);

	if (NULL == mysql_real_connect(m_pMySql, szHost, szUser, szPass, NULL, nPort, NULL, CLIENT_MULTI_STATEMENTS))
	{
		sprintf(m_szError, "Failed to connect to database: Error: %s.", mysql_error(m_pMySql));
		mysql_close(m_pMySql);
		m_pMySql = NULL;
		return -2;
	}
	else

	{
		m_pMySql->reconnect= 1;

		////MY_CHARSET_INFO cs;
		////mysql_get_character_set_info(m_pMySql, &cs);
		////char buf[1024];
		////sprintf(buf,"character set information:\n");
		////sprintf(buf,"character set+collation number: %d\n", cs.number);
		////sprintf(buf,"character set name: %s\n", cs.name);
		////sprintf(buf,"collation name: %s\n", cs.csname);
		////sprintf(buf,"comment: %s\n", cs.comment);
		////sprintf(buf,"directory: %s\n", cs.dir);
		////sprintf(buf,"multi byte character min. length: %d\n", cs.mbminlen);
		////sprintf(buf,"multi byte character max. length: %d\n", cs.mbmaxlen);

		//if(mysql_set_character_set(m_pMySql , "gbk") != DB_OK)
		//{
		//	sprintf(m_szError, "Failed to Set GBK character to database: Error: %s.", mysql_error(m_pMySql));
		//	mysql_close(m_pMySql);
		//	m_pMySql = NULL;
		//	return -2;
		//}
	}

	m_szError[0] = 0;
	return 0;
}

int KMySqlDB::SelectDatabase(const char* szDatabase)
{
	if (NULL == m_pMySql)
	{
		strcpy(m_szError, "Can't select database before connect to database.");
		return -1;
	}

	if (mysql_select_db(m_pMySql, szDatabase) < 0)
	{
		strcpy(m_szError, mysql_error(m_pMySql));
		sprintf(m_szError, "Can't select the %s database !", szDatabase);
		return 2 ;
	}

	m_szError[0] = 0;
	return 0;
}

void KMySqlDB::Close()
{
	ThrowResult();

	if (m_pMySql != NULL)
	{
		mysql_close(m_pMySql);
		m_pMySql = NULL;
	}
}

int KMySqlDB::Execute(const char* szSql)
{
	if (NULL == m_pMySql)
	{
		strcpy(m_szError, "Can't execute any sql before connect to database.");
		return -1;
	}

	// throw last result
	ThrowResult();

	if (mysql_real_query(m_pMySql, szSql, (unsigned long)strlen(szSql)) == 0)
	{
		m_pRes    = mysql_store_result(m_pMySql);
		if (NULL == m_pRes)
		{
			strcpy(m_szError, "query result is empty.");
			return 0;
		}
		m_nRows   = (unsigned int)mysql_num_rows(m_pRes);
		m_nFields = mysql_num_fields(m_pRes);
		m_pRow    = mysql_fetch_row(m_pRes);
	}
	else
	{
		sprintf(m_szError, "#%d:%s", mysql_errno(m_pMySql), mysql_error(m_pMySql));
		return -3;
	}

	m_szError[0] = 0;
	return 0;
}

const char* KMySqlDB::GetValue(int nField)
{
	if (NULL == m_pRow)
	{
		strcpy(m_szError, "invalid query result.");
		return NULL;
	}

	if (nField < 0 || (unsigned int)nField >= m_nFields)
	{
		sprintf(m_szError, "you should input between 0 and %d when get field value.", m_nFields);
		return NULL;
	}

	m_szError[0] = 0;
	return m_pRow[nField];
}

int KMySqlDB::MoveNext()
{
	m_pRow = mysql_fetch_row(m_pRes);
	if (m_pRow != NULL)
		return 0;
	return -1;
}

int KMySqlDB::MoveNextResult()
{
	if (NULL == m_pMySql)
	{
		strcpy(m_szError, "Can't get query result before connect to database.");
		return -1;
	}
	FreeResult();

	int r = mysql_next_result(m_pMySql);

	// sql error
	if (r > 0)
	{
		sprintf(m_szError, "#%d-%s", mysql_errno(m_pMySql), mysql_error(m_pMySql));
	}
	// more result
	else if (0 == r)
	{
		m_pRes = mysql_store_result(m_pMySql);
		if (NULL == m_pRes)
		{
			strcpy(m_szError, "query result is empty.");
			return 0;
		}
		m_nRows   = (unsigned int)mysql_num_rows(m_pRes);
		m_nFields = mysql_num_fields(m_pRes);
		m_pRow    = mysql_fetch_row(m_pRes);
	}
	else // -1 none result
	{
		strcpy(m_szError, "have not more result.");
	}
	return r;
}

int KMySqlDB::ThrowResult()
{
	if (NULL == m_pMySql)
	{
		strcpy(m_szError, "Can't get query result before connect to database.");
		return -1;
	}

	while(1)
	{
		FreeResult();
		if (!mysql_more_results(m_pMySql))
			break;
		int r = mysql_next_result(m_pMySql);
		if (r != 0)
			break;
		m_pRes = mysql_store_result(m_pMySql);
	}
	return 0;
}


void KMySqlDB::FreeResult()
{
	if (m_pRes != NULL)
	{
		mysql_free_result(m_pRes);
		m_pRes = NULL;
	}
	m_nRows   = 0;
	m_nFields = 0;
}
//***************************************************************************************/
bool	KMySqlDB::IsActive()	const
{
	if(!m_pMySql) return false;
	KMySqlDB* pThis = (KMySqlDB*)this;
	pThis->ThrowResult();
	bool bRet = mysql_ping(m_pMySql) == 0;
	pThis->ThrowResult();
	return bRet;
}

int KMySqlDB::SetCharSet( const char* charset )
{
	strcpy(m_charset, charset);
	if( m_pMySql )	//如果连接已经建立，则取设置已经工作的连接采用的字符集
	{
		return mysql_options(m_pMySql, MYSQL_SET_CHARSET_NAME, m_charset);
	}
	return 1;
}