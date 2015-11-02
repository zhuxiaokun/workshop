/********************************************************************
	created:	2008/04/25
	created:	25:4:2008   14:01
	filename: 	Athena\Base\System\Thread\KWin32ThreadImpl.h
	file path:	Athena\Base\System\Thread
	file base:	KWin32ThreadImpl
	file ext:	h
	author:		xueyan
	
	purpose:	WIN32平台的具体实现
*********************************************************************/

#pragma once
#include "KThreadImpl.h"

#if defined(WIN32) || defined(WINDOWS)
namespace System {			namespace Thread {

///////////////////////////////////////////////////////////////////////////////
// class CWin32ThreadImpl - Win32平台线程实现类

class KWin32ThreadImpl : public KThreadImpl
{
	friend DWORD WINAPI ThreadExecProc(void *pArg);

protected:
	HANDLE				m_nHandle;               // 线程句柄
	int					m_nPriority;                // 线程优先级

private:    
	void				CheckThreadError(BOOL bSuccess);

public:
	KWin32ThreadImpl(KThread *pThread);
	virtual ~KWin32ThreadImpl();

	virtual void		Run();
	virtual void		Exit(bool bForce);	// 2010/05/19韩玉信添加的专用退出线程函数：参数意义是是否强制杀死线程，一般由“该线程是否正在工作”来确定是和平退出还是强制退出
	virtual void		Terminate();
	virtual void		Kill();
	virtual int			WaitFor();
	virtual void		Sleep(INT nMisSeconds);

	int					GetPriority() const { return m_nPriority; }
	void				SetPriority(int nValue);
};

} /*Thread*/				} /* System */

#endif
// using namespace ::System::Thread;