#include "KApplicationStatus.h"
#include <System/Misc/StrUtil.h>
#include <System/Misc/KDatetime.h>
#include "KDemoChatServerApp.h"
#include <KGlobalFunction.h>

KApplicationStatus::KApplicationStatus()
{

}

KApplicationStatus::~KApplicationStatus()
{

}

bool KApplicationStatus::initialize(const char* dataDir)
{
	char filename[512];
	sprintf_k(filename, sizeof(filename), "%s/.appstatus.dat", dataDir);
	
	if(!m_mmap.Initialize(filename, 0, sizeof(ApplicationData)))
		return false;

	m_appData = (ApplicationData*)m_mmap.GetMemory();
	
	KMMap::eFileOpenStatus openStatus = m_mmap.GetOpenStatus();
	switch(openStatus)
	{
	case KMMap::enum_CreateNew:
	case KMMap::enum_ModifyExists:
		{
			memset(m_appData, 0, sizeof(ApplicationData));
			KDatetime nowDate;
			KLocalDatetime ldt = nowDate.GetLocalTime();
			m_appData->yyyy_mm_dd = ldt.year * 10000 + ldt.month * 100 + ldt.day;
			m_appData->expireTime = KDatetime::nextDayFirstSecond((TIME_T)nowDate.Time());
			m_appData->daySequence = 0;
		} break;
	}

	return true;
}

void KApplicationStatus::breathe(int ms)
{
	if(g_pApp->m_nowTime >= m_appData->expireTime)
	{
		KDatetime nowDate(g_pApp->m_nowTime);
		KLocalDatetime ldt = nowDate.GetLocalTime();//获取当前时间
		m_appData->yyyy_mm_dd = ldt.year * 10000 + ldt.month * 100 + ldt.day;
		m_appData->expireTime = KDatetime::nextDayFirstSecond((TIME_T)nowDate.Time());
		m_appData->daySequence = 0;
	}
}

void KApplicationStatus::finalize()
{

}

JgMessageID KApplicationStatus::lastMessageID()//最后一次消息序列号
{
	JgMessageID id;
	id.yyyy_mm_dd = m_appData->yyyy_mm_dd;
	if(m_appData->daySequence)
	{
		id.sequence = m_appData->daySequence - 1;
		return id;
	}
	else
	{
		id.sequence = 0;
		INT64 mid = MessageID2Int64(id);
		return Int642MessageID(mid - 1);
	}
}

JgMessageID KApplicationStatus::nextMessageID()//下一条消息的序列号
{
	JgMessageID id;
	id.yyyy_mm_dd = m_appData->yyyy_mm_dd;
	id.sequence = m_appData->daySequence++;
	return id;
}

