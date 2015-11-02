
/********************************************************************
	created:	2008/04/16
	created:	16:4:2008   11:03
	filename: 	Athena\Base\System\Thread\KThread.h
	file path:	Athena\Base\System\Thread
	file base:	KThread
	file ext:	h
	author:		xueyan
	
	purpose:	采用桥接（Bridge)模式设计一个跨平台的线程库
*********************************************************************/

///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include "../KPlatform.h"

#if defined(WIN32) || defined(WINDOWS)
#include "KWin32ThreadImpl.h"
#pragma warning(disable:4355)
#else
#include <pthread.h>
#include "KLinuxThreadImpl.h"
#endif

///////////////////////////////////////////////////////////////////////////////
/* 说明

一、Win32平台下和Linux平台下线程的主要区别:

    1. Win32线程拥有Handle和ThreadId，而Linux线程只有ThreadId。
    2. Win32线程只有ThreadPriority，而Linux线程有ThreadPolicy和ThreadPriority。
    3. Win32线程在强行杀死时，线程执行过程中的栈对象不会析构，而Linux线程则会。

*/
///////////////////////////////////////////////////////////////////////////////
// 类型定义

namespace System { namespace Thread {


///////////////////////////////////////////////////////////////////////////////
// class KThread - 线程类

class KThread
{
friend class KThreadImpl;

private:

#ifndef WINDOWS
	KLinuxThreadImpl		m_ThreadImpl;
#else
	KWin32ThreadImpl		m_ThreadImpl;
#endif

protected:
    // 线程的执行函数，子类必须重写。
    virtual void			Execute() {}

    // 执行 Terminate() 前的附加操作。
    // 注: 由于 Terminate() 属于自愿退出机制，为了能让线程能尽快退出，除了
    // m_bTerminated 标志被设为 true 之外，有时还应当补充一些附加的操作以
    // 便能让线程尽快从阻塞操作中解脱出来。
    virtual void			BeforeTerminate() {}

    // 执行 Kill() 前的附加操作。
    // 注: 线程被杀死后，用户所管理的某些重要资源可能未能得到释放，比如锁资源
    // (还未来得及解锁便被杀了)，所以重要资源的释放工作必须在 BeforeKill 中进行。
    virtual void			BeforeKill() {}
public:
	KThread() :m_ThreadImpl(this) {}
    virtual ~KThread() {}

    // 创建并执行线程。
    // 注: 此成员方法在对象声明周期中只可调用一次。
    void					Run() { m_ThreadImpl.Run(); }

	// 2010/05/19韩玉信添加的专用退出线程函数：参数意义是是否强制杀死线程，一般由“该线程是否正在工作”来确定是和平退出还是强制退出
	void					Exit(bool bForce = true)		{	m_ThreadImpl.Exit(bForce);	}
    // 通知线程退出 (自愿退出机制)
    // 注: 若线程由于某些阻塞式操作迟迟不退出，可调用 Kill() 强行退出。
    void					Terminate() { m_ThreadImpl.Terminate(); }

    // 强行杀死线程 (强行退出机制)
    void					Kill() { m_ThreadImpl.Kill(); }

    // 等待线程退出
    int						WaitFor() { return m_ThreadImpl.WaitFor(); }

    // 进入睡眠状态 (睡眠过程中会检测 m_bTerminated 的状态)
    // 注: 此函数必须由线程自己调用方可生效。
    void					Sleep(INT nMisSeconds) { m_ThreadImpl.Sleep(nMisSeconds); }

    // 属性 (getter)
    int						GetThreadId() const { return m_ThreadImpl.GetThreadId(); }
    int						GetTerminated() const { return m_ThreadImpl.GetTerminated(); }
    int						GetReturnValue() const { return m_ThreadImpl.GetReturnValue(); }
    BOOL					GetFreeOnTerminate() const { return m_ThreadImpl.GetFreeOnTerminate(); }
    int						GetTermElapsedSecs() const { return m_ThreadImpl.GetTermElapsedSecs(); }
    int						GetPriority() const { return m_ThreadImpl.GetPriority(); }
    // 属性 (setter)
    void					SetTerminated(BOOL bValue) { m_ThreadImpl.SetTerminated(bValue); }
    void					SetReturnValue(int nValue) { m_ThreadImpl.SetReturnValue(nValue); }
    void					SetFreeOnTerminate(BOOL bValue) { m_ThreadImpl.SetFreeOnTerminate(bValue); }
    void					SetPriority(int nValue) { m_ThreadImpl.SetPriority(nValue); }
};

} /*Thread*/ } /* System */