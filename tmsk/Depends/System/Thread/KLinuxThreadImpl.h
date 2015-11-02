/********************************************************************
	created:	2008/04/25
	created:	25:4:2008   14:25
	filename: 	Athena\Base\System\Thread\KLinuxThreadImpl.h
	file path:	Athena\Base\System\Thread
	file base:	KLinuxThreadImpl
	file ext:	h
	author:		xueyan
	
	purpose:	linux下线程的所有操作
*********************************************************************/



#pragma once
#include "KThreadImpl.h"

#if defined(WIN32) || defined(WINDOWS)
#else

namespace System { namespace Thread {

	class KLinuxThreadImpl : public KThreadImpl
	{
		friend void*		ThreadExecProc(void *pArg);

	protected:
		pthread_t			m_ThreadHandle;               // 线程句柄
		int					m_nPriority;             // 线程优先级

	private:    
		void				CheckThreadError(BOOL bSuccess);

	public:
		KLinuxThreadImpl(KThread *pThread);
		virtual ~KLinuxThreadImpl();

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