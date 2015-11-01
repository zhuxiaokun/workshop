#include "DB_DeleteMessageByDate.h"
#include "../KDemoChatServerApp.h"
#include "../Cache/KAccountCache.h"
#include <JgErrorCode.h>
#include <KGlobalFunction.h>
#include <System/File/KFile.h>
#include "../Storage/FS_DeleteMessageByDate.h"

DB_DeleteMessageByDate::DB_DeleteMessageByDate():m_result(0),m_yyyymmdd(0)
{
	
}

DB_DeleteMessageByDate::~DB_DeleteMessageByDate()
{

}

void DB_DeleteMessageByDate::setMessage(int yyyymmdd)
{
	m_yyyymmdd = yyyymmdd;
}

BOOL DB_DeleteMessageByDate::checkDepends()
{
	return TRUE;
}

BOOL DB_DeleteMessageByDate::execute(KSqlContext& ctx)
{
	KSQLCommand& cmd = ctx.cmd;
	KSQLResultSet& rs = ctx.rs;

	const char* sql = "call proc_DeleteMessageByDate(?)";
	KSQLFieldDescribe params[1];
	params[0].m_name = "id";  params[0].m_cdt = KSQLTypes::sql_c_uint64;
	
	if(!cmd.SetSQLStatement(sql))
	{
		m_result = JgErrorCode::err_SqlStatement;
		return FALSE;
	}
	if(!cmd.DescribeParameters(params, 1))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}
	if(!cmd.SetInt64(0, m_yyyymmdd))
	{
		m_result = JgErrorCode::err_SqlParams;
		return FALSE;
	}

	if(!cmd.ExecuteQuery(rs))
	{
		m_result = JgErrorCode::err_SqlExecute;
		Log(LOG_ERROR, "error: DB_DeleteMessageByDate, %s", cmd.GetLastErrorMsg());
		return FALSE;
	}

	return TRUE;
}

BOOL DB_DeleteMessageByDate::onFinished()
{
	Log(LOG_DEBUG, "debug: DB_DeleteMessageByDate::onFinished yyyymmdd:%d result:%d",
		m_yyyymmdd, m_result);

	FS_DeleteMessageByDate* task = FS_DeleteMessageByDate::Alloc(); task->reset();
	task->m_yyyymmdd = m_yyyymmdd;
	g_pApp->m_storageAgent.Push(task);

	return TRUE;
}

void DB_DeleteMessageByDate::reset()
{
	m_result = 0;
	m_yyyymmdd = 0;
	KSqlTask::reset();
}

void DB_DeleteMessageByDate::destroy()
{
	this->reset();
	DB_DeleteMessageByDate::Free(this);
}
