#include "KThread.h"
#include "KWin32ThreadImpl.h"

///////////////////////////////////////////////////////////////////////////////
#if defined(WIN32) || defined(WINDOWS)
namespace System {			namespace Thread {



	///////////////////////////////////////////////////////////////////////////////
	// class KWin32ThreadImpl

	//-----------------------------------------------------------------------------
	// 描述: 线程执行函数
	// 参数:
	//   pArg - 线程参数，此处指向 KWin32ThreadImpl 对象
	//-----------------------------------------------------------------------------
	DWORD WINAPI ThreadExecProc(void *pArg)
	{
		KWin32ThreadImpl *pThreadImpl = (KWin32ThreadImpl*)pArg;
		int nReturnValue = 0;

		{
			// 对象 AutoFinalizer 进行自动化善后工作
			struct CAutoFinalizer
			{
				KWin32ThreadImpl *m_pThreadImpl;
				CAutoFinalizer(KWin32ThreadImpl *pThreadImpl) { m_pThreadImpl = pThreadImpl; }
				~CAutoFinalizer()
				{
					m_pThreadImpl->SetFinished(true);
					m_pThreadImpl->SetThreadId(0);
					if (m_pThreadImpl->GetFreeOnTerminate()) 
					{
						delete m_pThreadImpl->GetThread();
					}
				}
			} AutoFinalizer(pThreadImpl);

			if (!pThreadImpl->m_bTerminated)
			{
				//try { pThreadImpl->Execute(); } catch (...) { ; }
				pThreadImpl->Execute();

				// 记下线程返回值
				nReturnValue = pThreadImpl->m_nReturnValue;
			}
		}

		ExitThread(nReturnValue);
		return NULL;
	}

	//-----------------------------------------------------------------------------
	// 描述: 构造函数
	//-----------------------------------------------------------------------------
	KWin32ThreadImpl::KWin32ThreadImpl(KThread *pThread) :
	KThreadImpl(pThread),
		m_nHandle(0),
		m_nPriority(THREAD_PRI_NORMAL)
	{
	}

	//-----------------------------------------------------------------------------
	// 描述: 析构函数
	//-----------------------------------------------------------------------------
	KWin32ThreadImpl::~KWin32ThreadImpl()
	{
		if (m_dwThreadId != 0 && !m_bFinished)
		{
			Terminate();
			WaitFor();
		}

		if (m_dwThreadId != 0)
			CloseHandle(m_nHandle);
	}

	//-----------------------------------------------------------------------------
	// 描述: 线程错误处理
	//-----------------------------------------------------------------------------
	void KWin32ThreadImpl::CheckThreadError(BOOL bSuccess)
	{
		ASSERT(bSuccess);
	}

	//-----------------------------------------------------------------------------
	// 描述: 创建线程并执行
	// 注意: 此成员方法在对象声明周期中只可调用一次。
	//-----------------------------------------------------------------------------
	void KWin32ThreadImpl::Run()
	{
		CheckNotRunning();

		DWORD tid = 0;
		m_nHandle = CreateThread(NULL, 0, ThreadExecProc, (LPVOID)this, 0, &tid);
		m_dwThreadId = tid;
		CheckThreadError(m_nHandle != 0);

		// 设置线程优先级
		if (m_nPriority != THREAD_PRI_NORMAL)
			SetPriority(m_nPriority);
	}

	void	KWin32ThreadImpl::Exit(bool bForce)		// 2010/05/19韩玉信添加的专用退出线程函数：参数意义是是否强制杀死线程，一般由“该线程是否正在工作”来确定是和平退出还是强制退出
	{
		if( bForce )
		{
			if (m_dwThreadId != 0)
			{
				// 用户须在此处释放重要资源，比如释放锁资源
				try { BeforeKill(); } catch (...) {}

				m_dwThreadId = 0;
				TerminateThread(m_nHandle, 0);
			}
		}
		else
		{
			KThreadImpl::Terminate();
		}
		WaitFor();
	}
	//-----------------------------------------------------------------------------
	// 描述: 通知线程退出
	//-----------------------------------------------------------------------------
	void KWin32ThreadImpl::Terminate()
	{
		KThreadImpl::Terminate();
	}

	//-----------------------------------------------------------------------------
	// 描述: 强行杀死线程
	// 注意: 
	//   1. 调用此函数后，对线程类对象的一切操作皆不可用(Terminate(); WaitFor(); delete pThread; 等)。
	//   2. 线程被杀死后，用户所管理的某些重要资源可能未能得到释放，比如锁资源 (还未来得及解锁
	//      便被杀了)，所以重要资源的释放工作必须在 BeforeKill 中进行。
	//   3. Win32 下强杀线程，线程执行过程中的栈对象不会析构。故 Kill() 会显式释放 KThread 对象。
	//-----------------------------------------------------------------------------
	void KWin32ThreadImpl::Kill()
	{
		if (m_dwThreadId != 0)
		{
			// 用户须在此处释放重要资源，比如释放锁资源
			try { BeforeKill(); } catch (...) {}

			m_dwThreadId = 0;
			TerminateThread(m_nHandle, 0);

			// Win32 下强杀线程，不会析构栈对象。故此处必须显式释放 KThread 对象。
			delete (KThread*)&m_Thread;
		}
	}

	//-----------------------------------------------------------------------------
	// 描述: 等待线程退出
	// 返回: 线程返回值
	//-----------------------------------------------------------------------------
	int KWin32ThreadImpl::WaitFor()
	{
		if (m_dwThreadId != 0)
		{
			WaitForSingleObject(m_nHandle, INFINITE);
			GetExitCodeThread(m_nHandle, (LPDWORD)&m_nReturnValue);
			m_dwThreadId = 0;
		}
		return m_nReturnValue;
	}

	//-----------------------------------------------------------------------------
	// 描述: 进入睡眠状态 (睡眠过程中会检测 m_bTerminated 的状态)
	// 参数:
	//   fSeconds - 睡眠的秒数，可为小数，可精确到毫秒
	// 注意:
	//   由于将睡眠时间分成了若干份，每次睡眠时间的小误差累加起来将扩大总误差。
	//-----------------------------------------------------------------------------
	void KWin32ThreadImpl::Sleep(INT nMisSeconds)
	{
		if (!GetTerminated())
		{
			::Sleep(nMisSeconds);
		}
	}

	//-----------------------------------------------------------------------------
	// 描述: 设置线程的优先级
	//-----------------------------------------------------------------------------
	void KWin32ThreadImpl::SetPriority(int nValue)
	{
		int nPriorities[7] = {
				THREAD_PRIORITY_IDLE,
				THREAD_PRIORITY_LOWEST,
				THREAD_PRIORITY_BELOW_NORMAL,
				THREAD_PRIORITY_NORMAL,
				THREAD_PRIORITY_ABOVE_NORMAL,
				THREAD_PRIORITY_HIGHEST,
				THREAD_PRIORITY_TIME_CRITICAL
		};

		nValue = kmax(0, nValue);
		nValue = kmin(6, nValue);
		m_nPriority = nValue;
		if (m_dwThreadId != 0)
			SetThreadPriority(m_nHandle, nPriorities[nValue]);
	}

} /*Thread*/				} /* System */
#endif