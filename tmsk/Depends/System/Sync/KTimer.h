/********************************************************************
	created:	2007/10/22
	created:	22:10:2007   9:48
	filename: 	KTimer.h
	file ext:	h
	author:		xuqin
	
	purpose:	
*********************************************************************/
#pragma once

#include "../KType.h"
#include "../KMacro.h"
#include "../Collections/KMapByVector.h"
#include "KSync.h"

#ifndef WINDOWS
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#endif

namespace System {			namespace Sync {

class ITimerHandler
{
public:
	virtual VOID 		OnTime() =0;
	ITimerHandler()
	{

	}
	virtual ~ITimerHandler()
	{

	}
};

class KTimer
{
public:
	KTimer()
	{
		m_nTickTime = 0;
		m_nTimerIndex = 0;
		m_bStop = TRUE;
		CreateTimer();
	}
	~KTimer()
	{
		DestroyTimer();
	}

	BOOL 		CreateTimer( INT nTickTime = 1)//nTickTime :win 微秒 linux 秒
	{
		m_nTickTime = nTickTime;
		if (TRUE ==  m_bStop)
		{
			#ifdef WINDOWS
					DWORD dwThreadId = 0;
					::CloseHandle( ::CreateThread( NULL, 0, ThreadStartForTime, this, 0, &dwThreadId ) );
			#else
					pthread_t t1;
					pthread_create(&t1,0,ThreadStartForTime,this);
			#endif
			m_bStop = FALSE;
		}
		return TRUE;
	}
	VOID 		DestroyTimer()
	{
		//	printf("DestroyTimer()\n");
		if (FALSE == m_bStop)
		{
			m_bStop = TRUE;
#ifdef	WINDOWS
			m_KSyncEvent.Block();
#else
			m_KSyncEvent.Block(m_CS.GetCS());
#endif
		}
		m_TimerMap.clear();
	}
	VOID 		SetTimer( INT nTickCount ,ITimerHandler* pTimerHandler)
	{
		if (TRUE == m_bStop)
		{
			return;
		}
		//printf("SetTimer()\n");
		KSync_CSAuto kSync_CSAuto(m_CS);
		Node tmpNode;
		tmpNode.nCurren = 0;
		tmpNode.nMax = nTickCount;
		tmpNode.pTimerHandler = pTimerHandler;
		m_TimerMap[m_nTimerIndex] = tmpNode;
		m_nTimerIndex ++;
	}
	VOID 		KillTimer( INT nSignal )
	{
		if (TRUE == m_bStop)
		{
			return;
		}
		KSync_CSAuto kSync_CSAuto(m_CS);
		m_TimerMap.erase(m_TimerMap.find(nSignal));
	}
	VOID 		KillTimer( ITimerHandler* pTimerHandler )
	{
		if (TRUE == m_bStop)
		{
			return;
		}
		KSync_CSAuto kSync_CSAuto(m_CS);
		for(m_TimerMapIter = m_TimerMap.begin();m_TimerMapIter != m_TimerMap.end();m_TimerMapIter++)
		{
			if ( pTimerHandler == m_TimerMapIter->second.pTimerHandler)
			{
				m_TimerMap.erase(m_TimerMapIter);
				break;
			}
		}
	}

	static KTimer*  Instance()
	{
		static KTimer sOnly;
		return &sOnly;
	}

private:
#ifdef WINDOWS
	static	DWORD __stdcall ThreadStartForTime(PVOID pvParam)
#else
	static VOID * ThreadStartForTime(VOID * pvParam)
#endif
	{
		KTimer* pThis = (KTimer*)pvParam;
		if (NULL == pThis)
		{
			printf("Timer Thread Error\n");
		}
		INT nTickTime = pThis->m_nTickTime;
		while( !pThis->m_bStop )
		{
#ifdef WINDOWS
			::Sleep( nTickTime );
#else
			sleep(nTickTime);
#endif
			KSync_CSAuto kSync_CSAuto(pThis->m_CS);
			for(pThis->m_TimerMapIter = pThis->m_TimerMap.begin();pThis->m_TimerMapIter != pThis->m_TimerMap.end();pThis->m_TimerMapIter++)
			{
				pThis->m_TimerMapIter->second.nCurren += nTickTime;
				if ( pThis->m_TimerMapIter->second.nCurren >= pThis->m_TimerMapIter->second.nMax)
				{
					ITimerHandler* pTimerHandler = pThis->m_TimerMapIter->second.pTimerHandler;
					if (NULL != pTimerHandler)
					{
						pTimerHandler->OnTime();
					}
					pThis->m_TimerMapIter->second.nCurren = 0;
				}
			}
		}
		pThis->m_KSyncEvent.Set();
		return NULL;
	}

private:
	INT						m_nTickTime;
	KSync_CS				m_CS;
	BOOL					m_bStop;
	KSync_Event				m_KSyncEvent;
	INT						m_nTimerIndex;

	struct Node
	{
		INT nCurren;
		INT nMax;
		ITimerHandler*			pTimerHandler;
	};
	
	typedef ::System::Collections::KMapByVector<INT,Node>	NodeMap;
	typedef NodeMap::iterator		NodeMapIter;

	NodeMap							m_TimerMap;
	NodeMapIter						m_TimerMapIter;
};

} /*Sync*/				} /* System */

// using namespace ::System::Sync;