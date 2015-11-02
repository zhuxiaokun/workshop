
#include "KLinuxThreadImpl.h"
#include "KThread.h"

#if defined(WIN32) || defined(WINDOWS)
#else

namespace System {			namespace Thread {

	///////////////////////////////////////////////////////////////////////////////
	// class KLinuxThreadImpl

	//-----------------------------------------------------------------------------
	// 描述: 线程执行函数
	// 参数:
	//   pArg - 线程参数，此处指向 KLinuxThreadImpl 对象
	//-----------------------------------------------------------------------------
	void* ThreadExecProc(void *pArg)
	{
		KLinuxThreadImpl *pThreadImpl = (KLinuxThreadImpl*)pArg;
		int nReturnValue = 0;

		INT nRet = 0;
		nRet = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		pThreadImpl->CheckThreadError(nRet == 0);
		nRet = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
		pThreadImpl->CheckThreadError(nRet == 0);

		{
			// 对象 AutoFinalizer 进行自动化善后工作
			struct CAutoFinalizer
			{
				KLinuxThreadImpl *m_pThreadImpl;
				CAutoFinalizer(KLinuxThreadImpl *pThreadImpl) { m_pThreadImpl = pThreadImpl; }
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
				//printf("Start Execute\n");
				try { pThreadImpl->Execute(); } catch (...) { ; }

				// 记下线程返回值
				nReturnValue = pThreadImpl->m_nReturnValue;
				//nReturnValue = 2;
				
			}
		}

		pthread_exit(reinterpret_cast<void*>(nReturnValue));
	}

	//-----------------------------------------------------------------------------
	// 描述: 构造函数
	//-----------------------------------------------------------------------------
	KLinuxThreadImpl::KLinuxThreadImpl(KThread *pThread) :
	KThreadImpl(pThread),
		m_ThreadHandle(0),
		m_nPriority(0)
	{
	}

	//-----------------------------------------------------------------------------
	// 描述: 析构函数
	//-----------------------------------------------------------------------------
	KLinuxThreadImpl::~KLinuxThreadImpl()
	{
		if (m_dwThreadId != 0 && !m_bFinished)
		{
			Terminate();
			WaitFor();
		}

		m_ThreadHandle = 0;
	}

	//-----------------------------------------------------------------------------
	// 描述: 线程错误处理
	//-----------------------------------------------------------------------------
	void KLinuxThreadImpl::CheckThreadError(BOOL bSuccess)
	{
		ASSERT(bSuccess);

	}

	//-----------------------------------------------------------------------------
	// 描述: 创建线程并执行
	// 注意: 此成员方法在对象声明周期中只可调用一次。
	//-----------------------------------------------------------------------------
	void KLinuxThreadImpl::Run()
	{
		CheckNotRunning();

		INT nRet = pthread_create(&m_ThreadHandle, NULL, ThreadExecProc, (LPVOID)this);
		CheckThreadError(nRet == 0);

		m_dwThreadId = (uint_r)m_ThreadHandle;

		// 设置离线状态，这个WIN32中是不用的。
		// pthread_detach(m_ThreadHandle);

		// 设置线程优先级
		if (m_nPriority != 0)
			SetPriority(m_nPriority);
	}

	// 2010/05/19韩玉信添加的专用退出线程函数：参数意义是是否强制杀死线程，一般由“该线程是否正在工作”来确定是和平退出还是强制退出
	void	KLinuxThreadImpl::Exit(bool bForce)
	{
		if( bForce )
		{
			if (m_dwThreadId != 0)
			{
				// 用户须在此处释放重要资源，比如释放锁资源
				try { BeforeKill(); } catch (...) {}

				int nRet = pthread_cancel(m_ThreadHandle);
				CheckThreadError(nRet == 0);

				//m_dwThreadId = 0;
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
	void KLinuxThreadImpl::Terminate()
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
	void KLinuxThreadImpl::Kill()
	{
		if (m_dwThreadId != 0)
		{
			// 用户须在此处释放重要资源，比如释放锁资源
			try { BeforeKill(); } catch (...) {}

			int nRet = pthread_cancel(m_ThreadHandle);
			CheckThreadError(nRet == 0);

			//m_dwThreadId = 0;
			
		}
	}

	//-----------------------------------------------------------------------------
	// 描述: 等待线程退出
	// 返回: 线程返回值
	//-----------------------------------------------------------------------------
	int KLinuxThreadImpl::WaitFor()
	{
		int nRet = -1;
		int_r nThreadRet = -1;
		if (m_dwThreadId != 0)
		{
			//printf("Waiting the thread for exit\n");
			nRet = pthread_join(m_ThreadHandle, (void**)&nThreadRet);
			CheckThreadError(nRet == 0);
			m_dwThreadId = 0;
			m_nReturnValue = (int)nThreadRet;
		}
		return nThreadRet;
	}

	//-----------------------------------------------------------------------------
	// 描述: 进入睡眠状态 (睡眠过程中会检测 m_bTerminated 的状态)
	// 参数:
	//   fSeconds - 睡眠的秒数，可为小数，可精确到毫秒
	// 注意:
	//   由于将睡眠时间分成了若干份，每次睡眠时间的小误差累加起来将扩大总误差。
	//-----------------------------------------------------------------------------
	void KLinuxThreadImpl::Sleep(INT nMisSeconds)
	{
		if (!GetTerminated())
		{
			msleep(nMisSeconds);
		}
	}

	//-----------------------------------------------------------------------------
	// 描述: 设置线程的优先级
	//-----------------------------------------------------------------------------
	void KLinuxThreadImpl::SetPriority(int nValue)
	{
		
	}

} /*Thread*/				} /* System */
#endif

