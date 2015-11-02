/********************************************************************
created:	2011/3/15
manager:	Allen Wang
copyright:	Aurora Interactive, 2011
*********************************************************************/

#pragma once
#include <deque>
#include "System/Sync/KSync.h"

/**
 * 多线程间进行数据交换的管道
 */
template <typename T>
class KPipe
{
public:
	// 写入尾部
	void Write(const T& data)
	{
		::System::Sync::KSync_CSAuto ______(m_lock);
		m_queue.push_back(data);
	}
	// 从头部读出
	T Read()
	{
		::System::Sync::KSync_CSAuto ______(m_lock);
		if (m_queue.size() == 0) return T();
		T result = m_queue.front();
		m_queue.pop_front();
		return result;
	}
	// 读头，但不挪读写位置
	T Get() const
	{
		::System::Sync::KSync_CSAuto ______(m_lock);
		if (m_queue.size() == 0) return T();
		return m_queue.front();
	}
private:
	::System::Sync::KSync_CS m_lock;
	std::deque<T> m_queue;
};